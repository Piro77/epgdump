#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bit.h"
#include "ts_ctl.h"

int parseBIThead(unsigned char *data, BIThead *h) {
	int boff = 0;

	memset(h, 0, sizeof(BIThead));

	boff = 0;
	h->table_id = getBit(data, &boff, 8);
	h->section_syntax_indicator = getBit(data, &boff, 1);
	h->reserved_future_use = getBit(data, &boff, 1);
	h->reserved = getBit(data, &boff, 2);
	h->section_length = getBit(data, &boff, 12);
	h->original_network_id = getBit(data, &boff, 16);
	h->reserved = getBit(data, &boff, 2);
	h->version_number = getBit(data, &boff, 5);
	h->current_next_indicator = getBit(data, &boff, 1);
	h->section_number = getBit(data, &boff, 8);
	h->last_section_number = getBit(data, &boff, 8);
	h->reserved_future_use1 = getBit(data, &boff, 3);
	h->broadcast_view_propriety = getBit(data, &boff, 1);
	h->first_descriptors_length = getBit(data, &boff, 12);

	return boff/8;
}

int parseBITbody(unsigned char *data, BITbody *b) {
	int boff = 0;

	memset(b, 0, sizeof(BITbody));

	b->transport_stream_id = getBit(data, &boff, 16);
	b->original_network_id = getBit(data, &boff, 16);
	b->service_id = getBit(data, &boff, 16);
	b->event_id = getBit(data, &boff, 16);
	b->reserved_future_use = getBit(data, &boff, 5);
	b->running_status = getBit(data, &boff, 3);

	return boff/8;
}
int parseSIParameterDescriptor(unsigned char *data, SIParameterDescriptor *d) {
	int boff = 0;
    int i,j,tag,len,looplen,numt,wk;

    memset(d,0,sizeof(SIParameterDescriptor));

    tag = getBit(data, &boff, 8);
    len = getBit(data, &boff, 8);

    d->parameter_version = getBit(data, &boff, 8);
    d->update_time = getBit(data, &boff, 16);

    looplen = len;
    i=0;
    while(looplen > 0) {
        d->sipt[i].table_id = getBit(data, &boff, 8);
        d->sipt[i].table_description_length = getBit(data, &boff, 8);
	switch(d->sipt[i].table_id) 
	{
		case 0x40:  //NIT
            wk = getBit(data, &boff, 8);
			printf("NIT CYCLE %d\n",wk);
			break;
		case 0xc4:  //BIT
            wk = getBit(data, &boff, 8);
			printf("BIT CYCLE %d\n",wk);
			break;
		case 0x42:  //SDT
            wk = getBit(data, &boff, 8);
			printf("SDT CYCLE %d\n",wk);
			break;
		case 0x4E:  //HEIT
            wk = getBit(data, &boff, 8);
			printf("HEIT CYCLE %d\n",wk);
            wk = getBit(data, &boff, 8);
			printf("MEIT CYCLE %d\n",wk);
            wk = getBit(data, &boff, 8);
			printf("LEIT CYCLE %d\n",wk);
            wk = getBit(data, &boff, 4);
			printf("MEIT EvNUM %d\n",wk);
            wk = getBit(data, &boff, 4);
			printf("LEIT EvNUM %d\n",wk);
			break;
		case 0x50:  //HEIT
			printf("HEIT LOOP %d\n",d->sipt[i].table_description_length/6);
        		getStr(d->sipt[i].table_description_byte, data, &boff,d->sipt[i].table_description_length);
			break;
	}
/*
 loop
   media_type 2
   pattern    2
   reserved   4
   schedule_range 8
   base_cycle    12
   reserved       2
   cycle_group_count 2
      num_of_segment 8
      cycle          8
*/
        looplen = looplen - d->sipt[i].table_description_length - 2;
        i++;
        if (d->sipt[i-1].table_id == 0x50) break;
    }
#ifdef DEBUG
    printf("%x \n",getBit(data, &boff, 8));
    printf("%d \n",getBit(data, &boff, 4));
#endif
    looplen = getBit(data, &boff, 12);

    while(looplen > 0) {
        d->sipt[i].table_id = getBit(data, &boff, 8);
if (d->sipt[i].table_id == 0xff ) break;
        d->sipt[i].table_description_length = getBit(data, &boff, 8);
        getStr(d->sipt[i].table_description_byte, data, &boff,d->sipt[i].table_description_length);
#ifdef DEBUG
        printf("2Table %x len %d\n",d->sipt[i].table_id ,d->sipt[i].table_description_length);
#endif
        looplen = looplen - d->sipt[i].table_description_length - 2;
        i++;
    }

    return len+2;
}

void dumpBIT(unsigned char *ptr, SVT_CONTROL *top)
{

	BIThead  bith;
	BITbody  bitb;
    SIParameterDescriptor sipd;
	SVT_CONTROL	*svtptr ;
	int		rc ;
    unsigned char tag;

	int len = 0;
	int loop_len = 0;

	/* BIT */
	len = parseBIThead(ptr, &bith); 
	ptr += len;
	loop_len = bith.section_length - (len - 3 + 4); // 3は共通ヘッダ長 4はCRC
	while(loop_len > 0) {
		len = parseOTHERdesc(ptr,&tag);
        switch(tag) {
            case 0xd7:
                len = parseSIParameterDescriptor(ptr,&sipd);
                break;
        }
		ptr += len;
		loop_len -= len;
	}
  
	return;
}
