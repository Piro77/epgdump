#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"
#include "ts_ctl.h"

#include "nit.h"


int parseNIThead(unsigned char *data, NIThead *h) {
	int boff = 0;

	memset(h, 0, sizeof(NIThead));

	h->table_id = getBit(data, &boff, 8);
	h->section_syntax_indicator = getBit(data, &boff, 1);
	h->reserved1 = getBit(data, &boff, 3);
	h->section_length = getBit(data, &boff, 12);
	h->network_id = getBit(data, &boff, 16);
	h->reserved2 = getBit(data, &boff, 2);
	h->version_number = getBit(data, &boff, 5);
	h->current_next_indicator = getBit(data, &boff, 1);
	h->section_number = getBit(data, &boff, 8);
	h->last_section_number = getBit(data, &boff, 8);
	h->reserved_future_use3 = getBit(data, &boff, 4);
	h->network_descriptors_length = getBit(data, &boff, 12);
	memset(h->network_descriptor,0,sizeof(h->network_descriptor));
	getStr(h->network_descriptor,data,&boff,h->network_descriptors_length);
	h->reserved_future_use2 = getBit(data, &boff, 4);
	h->transport_stream_loop_length = getBit(data, &boff, 12);
#ifdef DEBUG1
	printf("NIT %x %x [%s][%d] loop[%d]\n",h->table_id,h->network_id,h->network_descriptor,h->network_descriptors_length,h->transport_stream_loop_length);
#endif
	return boff/8;
}

int parseNITbody(unsigned char *data, NITbody *b) {
	int boff = 0;

	memset(b, 0, sizeof(NITbody));

	b->transport_stream_id = getBit(data, &boff, 16);
	b->original_network_id = getBit(data, &boff, 16);
	b->reserved_future_use = getBit(data, &boff, 4);
	b->transport_descriptors_loop_length = getBit(data, &boff, 12);
#ifdef DEBUG1
	printf("NITB %x %x\n",b->transport_stream_id ,b->original_network_id);
#endif

	return boff/8;
}

int parseServiceListDescriptor(unsigned char *data)
{
	int boff = 0;
	int i;
	int length,tag;

	ServiceListDescriptor sld;
	tag = getBit(data,&boff,8);
	length = getBit(data,&boff,8);

	for(i=0;i<length;) {
		sld.service_id = getBit(data, &boff, 16);
		sld.service_type = getBit(data, &boff, 8);
		printf("id[%d] 0x[%x] ",sld.service_id,sld.service_type);
		i+=3;
	}
	
	printf("\n");
	return boff/8;
}

int parseSatelliteDeliverySystemDescriptor(unsigned char *data)
{
	int boff = 0;
	int i;
	int length,tag;
	SatelliteDeliverySystemDescriptor sdsd;

	tag = getBit(data,&boff,8);
	length = getBit(data,&boff,8);

	sdsd.frequency = getBit(data,&boff,32);
	sdsd.orbital_position = getBit(data,&boff,16);
	sdsd.west_east_flag = getBit(data,&boff,1);
	sdsd.polarisation = getBit(data,&boff,2);
	sdsd.modulation = getBit(data,&boff,5);
	sdsd.symbol_rate = getBit(data,&boff,28);
	sdsd.FEC_inner = getBit(data,&boff,4);

#ifdef DEBUG1
	printf("SatteliteInfo\n");
	printf(" Freq             %x\n",sdsd.frequency); 
	printf(" orbital position %x\n",sdsd.orbital_position); 
	printf("         westeast %s\n",sdsd.west_east_flag?"west":"east"); 
#endif
	return boff/8;
}

void dumpNIT(unsigned char *ptr, SVT_CONTROL *top)
{

	int len = 0;
	int loop_len = 0;
	int loop_blen = 0;
	int boff;
	int i;
	NIThead nith;
	NITbody nitb;

	len = parseNIThead(ptr,&nith);

        ptr += len;
        loop_len = nith.section_length - (len - 3 + 4); // 3は共通ヘッダ長 4はCRC
        while(loop_len > 0) {
               len = parseNITbody(ptr, &nitb);

                ptr += len;
                loop_len -= len;

                loop_blen = nitb.transport_descriptors_loop_length;
                loop_len -= loop_blen;
                while(loop_blen > 0) {
			int sboff;
			unsigned char desctag;
			unsigned char desclen;
			sboff = 0;
			desctag = getBit(ptr,&sboff,8);
			desclen = getBit(ptr,&sboff,8);
#ifdef DEBUG
	printf("Desc %x len %d\n",desctag,desclen);
#endif
			len=0;
			switch(desctag) {
				case 0x41:
					len = parseServiceListDescriptor(ptr);
					break;
				case 0x43:
					len = parseSatelliteDeliverySystemDescriptor(ptr);
					break;
				default:
					len = parseOTHERdesc(ptr);
					break;

			}
                        ptr += len;
                        loop_blen -= len;
		}

	}

	return;
}
