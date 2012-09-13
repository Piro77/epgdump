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
int parseBITFirstLoop(unsigned char *data, SIParameterDescriptor *d,int flen) {
	int boff = 0;
    int i,j,tag,alen,len,looplen,numt,wk;

    memset(d,0,sizeof(SIParameterDescriptor));

    tag = getBit(data, &boff, 8);
    len = getBit(data, &boff, 8);

    d->parameter_version = getBit(data, &boff, 8);
    d->update_time = getBit(data, &boff, 16);

    looplen = flen;
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
	looplen = looplen - d->sipt[i].table_description_length;
	i++;
    }
return flen;
}
int parseSIParameterDescriptor(unsigned char *data, SIParameterDescriptor *d) {
	int boff = 0;
    int i,j,tag,alen,len,looplen,numt,wk;

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
		case 0xc3:  //SDTT
            wk = getBit(data, &boff, 16);
			printf("SDTT CYCLE %d\n",wk);
			break;
		case 0xc8:  //CDT
            wk = getBit(data, &boff, 16);
			printf("CDT CYCLE %d\n",wk);
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
		case 0x58:  //HEIT
			printf("2ndHEIT LOOP %d\n",d->sipt[i].table_description_length/6);
        		getStr(d->sipt[i].table_description_byte, data, &boff,d->sipt[i].table_description_length);
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
printf("loop minus %d tag %x\n",d->sipt[i].table_description_length,d->sipt[i].table_id);
        looplen = looplen - d->sipt[i].table_description_length + 2;
        i++;
    }
    return boff/8+2;
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
	int boff = 0;

	/* BIT */
	len = parseBIThead(ptr, &bith); 
	ptr += len;
	loop_len = bith.section_length - (len - 3 + 4); // 3は共通ヘッダ長 4はCRC

	len = parseBITFirstLoop(ptr,&sipd,bith.first_descriptors_length);
	ptr += len;
	loop_len = loop_len - len;
	//skip broarcaster loop??
	while(loop_len > 0) {
		boff=0;
		tag = getBit(ptr,&boff,8);
		switch(tag) {
			case 0xff:
				len = 3;
				break;

			case 0xd7:
				len =parseSIParameterDescriptor(ptr,&sipd);
				break;
			default:
				len = parseOTHERdesc(ptr,NULL);
				break;
		}
printf("BIT TAG %x len %d\n",tag,len);
		ptr += len;
		loop_len -= len;
	}
  
	return;
}
