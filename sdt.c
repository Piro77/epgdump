#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sdt.h"
#include "ts_ctl.h"

int parseSDThead(unsigned char *data, SDThead *h) {
	int boff = 0;

	memset(h, 0, sizeof(SDThead));

	boff = 0;
	h->table_id = getBit(data, &boff, 8);
	h->section_syntax_indicator = getBit(data, &boff, 1);
	h->reserved_future_use1 = getBit(data, &boff, 1);
	h->reserved1 = getBit(data, &boff, 2);
	h->section_length = getBit(data, &boff, 12);
	h->transport_stream_id = getBit(data, &boff, 16);
	h->reserved2 = getBit(data, &boff, 2);
	h->version_number = getBit(data, &boff, 5);
	h->current_next_indicator = getBit(data, &boff, 1);
	h->section_number = getBit(data, &boff, 8);
	h->last_section_number = getBit(data, &boff, 8);
	h->original_network_id = getBit(data, &boff, 16);
	h->reserved_future_use2 = getBit(data, &boff, 8);

	return 11;
}

int parseSDTbody(unsigned char *data, SDTbody *b) {
	int boff = 0;

	memset(b, 0, sizeof(SDTbody));

	b->service_id = getBit(data, &boff, 16);
	b->reserved_future_use1 = getBit(data, &boff, 3);
	b->EIT_user_defined_flags = getBit(data, &boff, 3);
	b->EIT_schedule_flag = getBit(data, &boff, 1);
	b->EIT_present_following_flag = getBit(data, &boff, 1);
	b->running_status = getBit(data, &boff, 3);
	b->free_CA_mode = getBit(data, &boff, 1);
	b->descriptors_loop_length = getBit(data, &boff, 12);

	return 5;
}

int parseSVCdesc(unsigned char *data, SVCdesc *desc) {
	int boff = 0;
  
	memset(desc, 0, sizeof(SVCdesc));

	desc->descriptor_tag = getBit(data, &boff, 8);
	desc->descriptor_length = getBit(data, &boff, 8);
	desc->service_type = getBit(data, &boff, 8);
	desc->service_provider_name_length = getBit(data, &boff, 8);
	getStr(desc->service_provider_name, data, &boff, desc->service_provider_name_length);
	desc->service_name_length = getBit(data, &boff, 8);
	getStr(desc->service_name, data, &boff, desc->service_name_length);

	return desc->descriptor_length + 2;
}
int		serachid(SVT_CONTROL *top, int service_id)
{
	SVT_CONTROL	*cur = top ;
	while(cur != NULL){
		if(cur->event_id == service_id){
			return 1 ;
		}
		cur = cur->next ;
	}
	return 0 ;
}

void	enqueue_sdt(SVT_CONTROL *top, SVT_CONTROL *sdtptr)
{
	SVT_CONTROL	*cur ;

	if(top->next == NULL){
		top->next = sdtptr ;
		top->prev = top ;
		return ;
	}
	cur = top->next ;
	while(cur != NULL){
		if(sdtptr->event_id < cur->event_id){
			if(cur->prev != NULL){
				cur->prev->next = sdtptr ;
				sdtptr->prev = cur->prev ;
			}
			cur->prev = sdtptr ;
			sdtptr->next = cur ;
			return ;
		}
		if(cur->next == NULL){
			cur->next = sdtptr ;
			sdtptr->prev = cur ;
			return ;
		}
		cur = cur->next ;
	}
	return ;

}

void dumpSDT(unsigned char *ptr, SVT_CONTROL *top)
{

	SDThead  sdth;
	SDTbody  sdtb;
	SVCdesc  desc;
	SVT_CONTROL	*svtptr ;
	int		rc ;

	int len = 0;
	int loop_len = 0;

	/* SDT */
	len = parseSDThead(ptr, &sdth); 
	ptr += len;
	loop_len = sdth.section_length - (len - 3 + 4); // 3は共通ヘッダ長 4はCRC
	while(loop_len > 0) {
		len = parseSDTbody(ptr, &sdtb);
		ptr += len;
		loop_len -= len;
		parseSVCdesc(ptr, &desc);

		rc = serachid(top, sdtb.service_id);
		if(rc == 0){
			svtptr = calloc(1, sizeof(SVT_CONTROL));
			svtptr->event_id = sdtb.service_id;
			svtptr->original_network_id = sdth.original_network_id;
			svtptr->transport_stream_id = sdth.transport_stream_id;
			svtptr->event_id = sdtb.service_id;
			memcpy(svtptr->servicename, desc.service_name, strlen(desc.service_name));
			enqueue_sdt(top, svtptr);
#if 0
			printf("SDT=%s,%d,%x,%x,%x,%x,%x,%x,%x\n",
				desc.service_name, sdtb.service_id, sdtb.reserved_future_use1,
				sdtb.EIT_user_defined_flags, sdtb.EIT_schedule_flag, sdtb.EIT_present_following_flag,
				sdtb.running_status, sdtb.free_CA_mode, sdtb.descriptors_loop_length);
/*
#else
0x01:デジタルTVサービス
0xA5:プロモーション映像サービス
0x0C:データサービス
 */
			printf("SDT=(%x:%x)%s,%d,%d,%d,%d,%d(%d,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x)\n",
					sdth.table_id, desc.service_type, 
					desc.service_name, sdtb.service_id,
					desc.descriptor_tag, desc.descriptor_length, desc.service_type,
					desc.service_provider_name_length, desc.service_name_length,
					sdth.table_id, sdth.section_syntax_indicator, sdth.reserved_future_use1,
					sdth.reserved1, sdth.section_length, sdth.transport_stream_id,
					sdth.reserved2, sdth.version_number, sdth.current_next_indicator,
					sdth.section_number, sdth.last_section_number, sdth.original_network_id,
					sdth.reserved_future_use2);
#endif
		}

		ptr += sdtb.descriptors_loop_length;
		loop_len -= sdtb.descriptors_loop_length;
	}
  
	return;
}
