#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eit.h"

char		*subtitle_cnv_str[] = {
	NULL
};
static void timecmp(int *,int *,int *,
					int, int, int);
void append_desc(EIT_CONTROL* eittop, int service_id, int event_id, EEVTDitem* eevtitem);

int parseEIThead(unsigned char *data, EIThead *h) {
	int boff = 0;

	memset(h, 0, sizeof(EIThead));

	h->table_id = getBit(data, &boff, 8);
	h->section_syntax_indicator = getBit(data, &boff, 1);
	h->reserved_future_use = getBit(data, &boff, 1);
	h->reserved1 = getBit(data, &boff, 2);
	h->section_length =getBit(data, &boff,12);
	h->service_id = getBit(data, &boff, 16);
	h->reserved2 = getBit(data, &boff, 2);
	h->version_number = getBit(data, &boff, 5);
	h->current_next_indicator = getBit(data, &boff, 1);
	h->section_number = getBit(data, &boff, 8);
	h->last_section_number = getBit(data, &boff, 8);
	h->transport_stream_id = getBit(data, &boff, 16);
	h->original_network_id = getBit(data, &boff, 16);
	h->segment_last_section_number = getBit(data, &boff, 8);
	h->last_table_id = getBit(data, &boff, 8);
#ifdef DEBUG
	printf("EIT Table id 0x%x current %d\n",h->table_id,h->section_number);
#endif
  
	return 14;
}

int parseEITbody(unsigned char *data, EITbody *b)
{
	int boff = 0;
	int tnum;
	char buf[4];

	memset(b, 0, sizeof(EITbody));

	b->event_id = getBit(data, &boff, 16);

	memcpy(b->start_time, data + boff / 8, 5);
	/* b->start_time = getBit(data, &boff, 40); */
	boff += 40;
	memcpy(b->duration, data + boff / 8, 3);
	/* b->duration = getBit(data, &boff, 24); */
	boff += 24;
	b->running_status = getBit(data, &boff, 3);
	b->free_CA_mode = getBit(data, &boff, 1);
	b->descriptors_loop_length = getBit(data, &boff, 12);

	/* 日付変換 */
	tnum = (b->start_time[0] & 0xFF) << 8 | (b->start_time[1] & 0xFF);
  
	b->yy = (tnum - 15078.2) / 365.25;
	b->mm = ((tnum - 14956.1) - (int)(b->yy * 365.25)) / 30.6001;
	b->dd = (tnum - 14956) - (int)(b->yy * 365.25) - (int)(b->mm * 30.6001);

	if(b->dd == 0) {
		printf("aa");
	}

	if(b->mm == 14 || b->mm == 15) {
		b->yy += 1;
		b->mm = b->mm - 1 - (1 * 12);
	} else {
		b->mm = b->mm - 1;
	}

	b->yy += 1900;
  
	memset(buf, '\0', sizeof(buf));
	sprintf(buf, "%x", b->start_time[2]);
	b->hh = atoi(buf);
	memset(buf, '\0', sizeof(buf));
	sprintf(buf, "%x", b->start_time[3]);
	b->hm = atoi(buf);
	memset(buf, '\0', sizeof(buf));
	sprintf(buf, "%x", b->start_time[4]);
	b->ss = atoi(buf);

	if((b->duration[0] == 0xFF) && (b->duration[1] == 0xFF) && (b->duration[2] == 0xFF)){
		b->dhh = b->dhm = b->dss = 0;
	}else{
		memset(buf, '\0', sizeof(buf));
	sprintf(buf, "%x", b->duration[0]);
	b->dhh = atoi(buf);
		memset(buf, '\0', sizeof(buf));
	sprintf(buf, "%x", b->duration[1]);
	b->dhm = atoi(buf);
		memset(buf, '\0', sizeof(buf));
	sprintf(buf, "%x", b->duration[2]);
	b->dss = atoi(buf);
	}
	return 12;
}

/* 短形式イベント記述子 */
int parseSEVTdesc(unsigned char *data, SEVTdesc *desc) {
	int boff = 0;
  
	memset(desc, 0, sizeof(SEVTdesc));

	desc->descriptor_tag = getBit(data, &boff, 8);
	if((desc->descriptor_tag & 0xFF) != 0x4D) {
		return -1;
	}
	desc->descriptor_length = getBit(data, &boff, 8);
	memcpy(desc->ISO_639_language_code, data + boff / 8, 3);
	/* desc->ISO_639_language_code = getBit(data, &boff, 24); */
	boff += 24;
	desc->event_name_length = getBit(data, &boff, 8);
	getStr(desc->event_name, data, &boff, desc->event_name_length);
	desc->text_length = getBit(data, &boff, 8);
	getStr(desc->text, data, &boff, desc->text_length);
#ifdef DEBUG
	printf("EventName [%s]\n",desc->event_name);
	printf("ShortDesc [%s]\n",desc->text);
#endif

	return desc->descriptor_length + 2;
}

int parseComponentDesc(unsigned char *data, ComponentDesc *desc) {
	int boff = 0;
  
	memset(desc, 0, sizeof(ComponentDesc));

	desc->descriptor_tag = getBit(data, &boff, 8);
	if((desc->descriptor_tag & 0xFF) != 0x50) {
		return -1;
	}
	desc->descriptor_length = getBit(data, &boff, 8);
	desc->reserved_future_use = getBit(data, &boff, 4);
	desc->stream_content = getBit(data, &boff, 4);
	desc->component_type = getBit(data, &boff, 8);
	desc->component_tag = getBit(data, &boff, 8);
	memcpy(desc->ISO_639_language_code, data + boff / 8, 3);
	boff += 24;
	memcpy(desc->content, data+(boff/8), desc->descriptor_length);
	//getStr(desc->content, data, &boff, desc->descriptor_length);
#ifdef DEBUG
	switch(desc->component_type) {
		case 0x03:
			printf("  映像480i(525i)、アスペクト比16:9 パンベクトルなし\n");
			break;
		case 0xb3:
			printf("  映像1080i(1125i)、アスペクト比16:9 パンベクトルなし\n");
			break;
		default:
			printf("  映像type 0x%x stream %x\n",desc->component_type,desc->stream_content);
			break;
	}
#endif
	return desc->descriptor_length + 2;
}

int parseAudioComponentDesc(unsigned char *data,AudioComponentDesc *desc) {
	int boff = 0;
  
	memset(desc, 0, sizeof(AudioComponentDesc));

	desc->descriptor_tag = getBit(data, &boff, 8);
	if((desc->descriptor_tag & 0xFF) != 0xC4) {
		return -1;
	}
	desc->descriptor_length = getBit(data, &boff, 8);
	desc->reserved_future_use = getBit(data, &boff, 4);
	desc->stream_content = getBit(data, &boff, 4);
	desc->component_type = getBit(data, &boff, 8);
	desc->component_tag = getBit(data, &boff, 8);
	desc->stream_type = getBit(data, &boff, 8);
	desc->simulcast_group_tag = getBit(data, &boff, 8);
	desc->ES_multi_lingual_flag = getBit(data, &boff, 1);
	desc->main_component_flag = getBit(data, &boff, 1);
	desc->quality_indicator = getBit(data, &boff, 2);
	desc->sampling_rate = getBit(data, &boff, 3);
	desc->reserved_future_use2 = getBit(data, &boff, 1);
	memcpy(desc->ISO_639_language_code, data + boff / 8, 3);
	boff += 24;
	if (desc->ES_multi_lingual_flag==0x01) {
		memcpy(desc->ISO_639_language_code2, data + boff / 8, 3);
		boff += 24;
	}
	memcpy(desc->content, data+(boff/8), desc->descriptor_length);
	//getStr(desc->content, data, &boff, desc->descriptor_length);
#ifdef DEBUG
	printf("  Audio %s%s",desc->main_component_flag?"":"副",desc->main_component_flag?"":desc->ISO_639_language_code);
	switch (desc->component_type) {
		case 0x01:
			printf("シングルモノ");
			break;
		case 0x02:
			printf("デュアルモノ");
			break;
		case 0x03:
			printf("ステレオ    ");
			break;
		case 0x09:
			printf("3/2.1mode   ");
			break;
		default:
			printf("Unknown     ");
			break;
	}

	printf(" streamtype 0x%x samplerate 0x%x ",desc->component_type,desc->stream_type,desc->sampling_rate);
	if (desc->ES_multi_lingual_flag==0x01) {
	printf("二ヶ国語");
	}
	printf("\n");
#endif
	return desc->descriptor_length + 2;

}

int parseContentDesc(unsigned char *data, ContentDesc *desc) {
	int boff = 0;
  
	memset(desc, 0, sizeof(ContentDesc));

	desc->descriptor_tag = getBit(data, &boff, 8);
	if((desc->descriptor_tag & 0xFF) != 0x54) {
		return -1;
	}
	desc->descriptor_length = getBit(data, &boff, 8);
	memcpy(desc->content, data+(boff/8), desc->descriptor_length);
	//getStr(desc->content, data, &boff, desc->descriptor_length);
#ifdef DEBUG
	unsigned char cn;
	printf("  Content 0x%02x\n",desc->content[0]);
#endif
	return desc->descriptor_length + 2;
}

int parseSeriesDesc(unsigned char *data, SeriesDesc *desc) {
	int boff = 0;
  
	memset(desc, 0, sizeof(SeriesDesc));

	desc->descriptor_tag = getBit(data, &boff, 8);
	if((desc->descriptor_tag & 0xFF) != 0xD5) {
		return -1;
	}
	desc->descriptor_length = getBit(data, &boff, 8);
	desc->series_id = getBit(data, &boff, 16);
	desc->repeat_label = getBit(data, &boff, 4);
	desc->program_pattern = getBit(data, &boff, 3);
	desc->expire_date_valid_flag = getBit(data, &boff, 1);

	desc->expire_date = getBit(data, &boff, 16);
	//memcpy(desc->expire_date, data + boff / 8, 2);
	//boff += 16;

	desc->episode_number = getBit(data, &boff, 12);
	desc->last_episode_number = getBit(data, &boff, 12);

	getStr(desc->series_name_char, data, &boff, desc->descriptor_length - 8);
#ifdef DEBUG
	printf("SERI [%d] [%s]\n",desc->repeat_label,desc->series_name_char);
#endif
	return desc->descriptor_length + 2;
}

/* 拡張形式イベント記述子 */
int parseEEVTDhead(unsigned char *data, EEVTDhead *desc) {
	int boff = 0;
  
	memset(desc, 0, sizeof(EEVTDhead));

	desc->descriptor_tag = getBit(data, &boff, 8);
	if((desc->descriptor_tag & 0xFF) != 0x4E) {
		return -1;
	}
	desc->descriptor_length = getBit(data, &boff, 8);
	desc->descriptor_number = getBit(data, &boff, 4);
	desc->last_descriptor_number = getBit(data, &boff, 4);
	memcpy(desc->ISO_639_language_code, data + boff / 8, 3);
	/* desc->ISO_639_language_code = getBit(data, &boff, 24); */
	boff += 24;

	desc->length_of_items = getBit(data, &boff, 8);

	return 7;
}

int parseEEVTDitem(unsigned char *data, EEVTDitem *desc,EIT_CONTROL *eitcur) {
	int boff = 0;
  
	memset(desc, 0, sizeof(EEVTDitem));

	desc->item_description_length = getBit(data, &boff, 8);
	getStr(desc->item_description, data, &boff, desc->item_description_length);

	desc->item_length = getBit(data, &boff, 8);
	memcpy(desc->item, data + (boff / 8), desc->item_length);
	/* getStr(desc->item, data, &boff, desc->item_length); */
#ifdef DEBUG
	char *tmpbuf;
	int len,extlen;
	tmpbuf = malloc(1024*1024);
	getStr(tmpbuf,data,&boff,desc->item_length);
	printf("desc[%d][%s]\nitem[%d]strlenitem[%d]\n  [%s]\n",desc->item_description_length,desc->item_description,desc->item_length,strlen(tmpbuf),tmpbuf);
	if (eitcur != NULL )  {
	if (eitcur->extdesc == NULL) {
		len = strlen(desc->item_description) + 2 + strlen(tmpbuf);
		eitcur->extdesc = malloc(len);
		strcpy(eitcur->extdesc,desc->item_description);
		strcat(eitcur->extdesc,"　");
		strcat(eitcur->extdesc,tmpbuf);
	}
	else {
		len = strlen(desc->item_description);
		if (len == 0) {
			len = strlen(tmpbuf) + strlen(eitcur->extdesc);
			eitcur->extdesc = realloc(eitcur->extdesc,len);
			strcat(eitcur->extdesc,tmpbuf);
		}
		else {
			len = 2 + strlen(desc->item_description) + 2 + strlen(tmpbuf) + strlen(eitcur->extdesc);
			eitcur->extdesc = realloc(eitcur->extdesc,len);
			strcat(eitcur->extdesc,"　");
			strcat(eitcur->extdesc,desc->item_description);
			strcat(eitcur->extdesc,"　");
			strcat(eitcur->extdesc,tmpbuf);
		}
	}
	printf(" extdesc[%s]\n",eitcur->extdesc);
	}
	free(tmpbuf);
#endif
	return desc->item_description_length + desc->item_length + 2;
}

int parseEEVTDtail(unsigned char *data, EEVTDtail *desc) {
	int boff = 0;
  
	memset(desc, 0, sizeof(EEVTDtail));

	desc->text_length = getBit(data, &boff, 8);
	getStr(desc->text, data, &boff, desc->text_length);

	return desc->text_length + 1;
}

int checkEEVTDitem(EEVTDitem *save, EEVTDitem *new, int descriptor_number) {

	EEVTDitem swap;
	int boff = 0;

	if(new == NULL) { /* 最後のチェック*/
		if(save->item_length != 0) {
			swap = *save;
			memset(save->item,0,MAXSECLEN);
			getStr(save->item, (unsigned char*)swap.item, &boff, swap.item_length);
			return 1;
		} else {
			return 0;
		}
	} 

	if(new->item_description_length == 0) {
		/* 続き 保存 */
		memcpy(save->item + save->item_length, new->item, new->item_length);
		save->item_length += new->item_length;
		return 0;
	} else {
		/* ブレーク。saveを印刷対象にする。saveをクリア? */
		if(save->item_length != 0) {
			/* 退避済みがあり */
			swap = *save;
			memset(save->item,0,MAXSECLEN);
			getStr(save->item, (unsigned char*)swap.item, &boff, swap.item_length);
			swap = *new;
			*new = *save;
			*save = swap;
			save->descriptor_number = descriptor_number;
		} else {
			*save = *new;
			save->descriptor_number = descriptor_number;
			return 0;
		}
	}

	return 1;
}
EIT_CONTROL	*searcheit(EIT_CONTROL *top, int servid, int eventid)
{
	EIT_CONTROL	*cur ;
	cur = top ;

	while(cur != NULL){
		if((cur->event_id == eventid) && (cur->servid == servid)){
			return cur ;
		}

		cur = cur->next ;
	}
	return NULL ;
}
char	*strstr_eucjp(const char *str, const char *search)
{
	char *pos ;
	pos = (char *)str ;

	while (*pos != '\0') {
		if (*pos == *search) {
			if (strncmp(pos, search, strlen(search)) == 0) {
				return pos ;
			}
		}
		if ((unsigned char)*pos == 0x8Fu) {
			pos += 3 ;
		} else if ((unsigned char)*pos >= 0x80u) {
			pos += 2 ;
		} else {
			pos += 1 ;
		}
	}

	return NULL ;
}
void	conv_title_subtitle(EIT_CONTROL *eitptr)
{
	int		lp = 0 ;
//	size_t	addsize ;
	char	*ptr ;
	char	*ptr2 ;
	char	*newsubtitle ;

	for(lp = 0 ; subtitle_cnv_str[lp] != NULL ; lp++){
		ptr = strstr(eitptr->title, subtitle_cnv_str[lp]);
		if(ptr == NULL){
			continue ;
		}
		// タイトルがなくならないように
		if(ptr == eitptr->title){
			continue ;
		}
		ptr2 = ptr ;
		for( ; (unsigned char)*ptr2 == 0x20u ; ptr2++ );
		for( ; (unsigned char)*ptr2 == 0xA1u && (unsigned char)*(ptr2+1) == 0xA1u ; ptr2 += 2);
		for( ; (unsigned char)*ptr2 == 0x20u ; ptr2++ );
		newsubtitle = calloc(1, ((strlen(ptr2) + 2) + (strlen(eitptr->subtitle) + 1)));
		memcpy(newsubtitle, ptr2, strlen(ptr2));
//		*(newsubtitle+strlen(ptr)) = ' ';
		strcat(newsubtitle, "▽");
		*ptr = '\0';
		strcat(newsubtitle, eitptr->subtitle);
		free(eitptr->subtitle);
		eitptr->subtitle = newsubtitle ;
		return ;
	}
}
void	enqueue(EIT_CONTROL *top, EIT_CONTROL *eitptr)
{
	EIT_CONTROL	*cur ;
	cur = top ;
	int		rc ;

	if(top->next == NULL){
		top->next = eitptr ;
		eitptr->prev = top ;
		return ;
	}
	cur = top->next ;
	while(cur != NULL){
		rc = memcmp(&cur->yy, &eitptr->yy, (sizeof(int) * 3));
		if(rc == 0){
			rc = memcmp(&cur->hh, &eitptr->hh, (sizeof(int) * 3));
			if(rc == 0){
				free(eitptr->title);
				free(eitptr->subtitle);
				free(eitptr);
				return ;
			}
			if(rc > 0){
				if(cur->prev != 0){
					cur->prev->next = eitptr ;
					eitptr->prev = cur->prev ;
				}
				cur->prev = eitptr ;
				eitptr->next = cur ;
				conv_title_subtitle(eitptr);
				return ;
			}
		}
		if(rc > 0){
			if(cur->prev != 0){
				cur->prev->next = eitptr ;
				eitptr->prev = cur->prev ;
			}
			cur->prev = eitptr ;
			eitptr->next = cur ;
			conv_title_subtitle(eitptr);
			return ;
		}
		if(cur->next == NULL){
			cur->next = eitptr ;
			eitptr->prev = cur ;
			conv_title_subtitle(eitptr);
			return ;
		}
		cur = cur->next ;
	}
	return ;

}

void dumpEIT(unsigned char *ptr, int serv_id, int original_network_id, int transport_stream_id, EIT_CONTROL *eittop)
{

	EIThead  eith;
	EITbody  eitb;
	SEVTdesc sevtd;

	EEVTDhead eevthead;
	EEVTDitem eevtitem;
	EEVTDtail eevttail;

	EEVTDitem save_eevtitem;

	EIT_CONTROL	*cur = NULL;

	int len = 0;
	int loop_len = 0;
	int loop_blen = 0;
	int loop_elen = 0;

	int ehh, emm, ess;

	/* EIT */
	len = parseEIThead(ptr, &eith); 

	ptr += len;
	loop_len = eith.section_length - (len - 3 + 4); // 3は共通ヘッダ長 4はCRC
	while(loop_len > 0) {
		/* 連続する拡張イベントは、漢字コードが泣き別れして
		   分割されるようだ。連続かどうかは、item_description_lengthが
		   設定されているかどうかで判断できるようだ。 */
		memset(&save_eevtitem, 0, sizeof(EEVTDitem));

		len = parseEITbody(ptr, &eitb);

		ptr += len;
		loop_len -= len;
    
		/* printf("evtid:%d\n", eitb.event_id); */
    
		loop_blen = eitb.descriptors_loop_length;
		loop_len -= loop_blen;
		while(loop_blen > 0) {
			unsigned char desctag;
			int sboff;
			ComponentDesc componentDesc;
			AudioComponentDesc audioComponentDesc;
			sboff=0;
			desctag = getBit(ptr,&sboff,8);
#ifdef DEBUG
			printf("Event 0x%x desctag [0x%x] savelen %d\n",eitb.event_id,desctag,save_eevtitem.item_length);
#endif
			switch (desctag) {
				case 0x50:
					len = parseComponentDesc(ptr, &componentDesc);
					break;
				case 0xC4:
					len = parseAudioComponentDesc(ptr, &audioComponentDesc);
					break;
				default:
					break;
			}

			len = parseSEVTdesc(ptr, &sevtd);
			if(len > 0) {

				/*
				  if(eith.service_id == 19304 && 
				  eitb.event_id == 46564) {
				  printf("aa");
				  }
				*/

				ehh = eitb.hh;
				emm = eitb.hm;
				ess = eitb.ss;
				if(eith.service_id != serv_id){
					ptr += len;
					loop_blen -= len;
					continue ;
				}

				timecmp(&ehh, &emm, &ess,
						eitb.dhh, eitb.dhm, eitb.dss);
				cur = searcheit(eittop, eith.service_id, eitb.event_id);
				if(cur == NULL){
					cur = calloc(1, sizeof(EIT_CONTROL));
					cur->event_id = eitb.event_id ;
					cur->servid = eith.service_id ;
					cur->title = calloc(1, (strlen(sevtd.event_name) + 1));

					memcpy(cur->title, sevtd.event_name, strlen(sevtd.event_name));
					cur->subtitle = calloc(1, (strlen(sevtd.text) + 1));
					memcpy(cur->subtitle, sevtd.text, strlen(sevtd.text));
					cur->yy = eitb.yy;
					cur->mm = eitb.mm;
					cur->dd = eitb.dd;
					cur->hh = eitb.hh;
					cur->hm = eitb.hm;
					cur->ss = eitb.ss;
					cur->ehh = eitb.dhh;
					cur->emm = eitb.dhm;
					cur->ess = eitb.dss ;
					cur->table_id = eith.table_id ;
					enqueue(eittop, cur);
				}
			} else {
				len = parseEEVTDhead(ptr, &eevthead);

				/*
				  if(eith.service_id == 19304 && 
				  eitb.event_id == 46564) {
				  printf("aa");
				  }
				*/


				if(len > 0) {
					ptr += len;
					loop_blen -= len;

					loop_elen = eevthead.length_of_items;
					loop_len -= loop_elen;
					while(loop_elen > 0) {
						len = parseEEVTDitem(ptr, &eevtitem,cur);

						ptr += len;
						loop_elen -= len;
						loop_blen -= len;

						if(checkEEVTDitem(&save_eevtitem, &eevtitem, 
										  eevthead.descriptor_number)) {
#if 0
								fprintf(stdout, "1EEVT,%d,%d,%d\ndesc[%s]\nitemlen[%d]strlenitem[%d]\nitem[%s]\n", 
										eith.service_id,
										eitb.event_id,
										eevtitem.descriptor_number, /* 退避項目 */
										eevtitem.item_description,
										eevtitem.item_length,
										strlen(eevtitem.item),
										eevtitem.item);
#endif
							append_desc(eittop,eith.service_id,eitb.event_id,&eevtitem);
						}
					}

					len = parseEEVTDtail(ptr, &eevttail);
#if 0
						fprintf(stdout, "EEVTt,%d,%d,%d\ntail[%s]\n", 
								eith.service_id,
								eitb.event_id,
								eevthead.descriptor_number,
								eevttail.text);
#endif
				} else {
					ContentDesc contentDesc;
					len = parseContentDesc(ptr, &contentDesc);
					if (len > 0) {
//						int header_printed = 0;
						for (int i = 0; i < contentDesc.descriptor_length - 1; i+=2) {
							/*
							if (0xff == (unsigned char)contentDesc.content[i])
								continue;
							*/
#if 0
							if (!header_printed) {
								fprintf(stdout, "Content,%d,%d",
									eith.service_id,
									eitb.event_id);
								header_printed = 1;
							}
#endif
#if 0
							fprintf(stdout, ",%02x%02x", (unsigned char)contentDesc.content[i], (unsigned char)contentDesc.content[i+1]);
#endif
						}
						if((eith.original_network_id == original_network_id) && (eith.transport_stream_id == transport_stream_id)){
							cur = searcheit(eittop, eith.service_id, eitb.event_id);
							if(cur != NULL){
								cur->content_type = (unsigned char)(contentDesc.content[0]);
#if 0
								fprintf(stdout, "%s:", cur->title);
								fprintf(stdout, ",%02x%02x", (unsigned char)contentDesc.content[0], (unsigned char)contentDesc.content[1]);
								fprintf(stdout, ",%02x%02x\n", (unsigned char)contentDesc.content[2], (unsigned char)contentDesc.content[3]);
#endif

							}
#if 0
							if (header_printed) {
								fprintf(out, "\n");
							}
#endif
						}
					} else {
						SeriesDesc seriesDesc;
						len = parseSeriesDesc(ptr, &seriesDesc);
						if (len > 0) {
#if 0
							fprintf(out, "Series,%d,%d,series=%d,repeat=%01x,pattern=%d,expire_valid=%d,expire=%04x,epinum=%d,lastepinum=%d,%s\n",
								eith.service_id,
								eitb.event_id,
								seriesDesc.series_id,
								seriesDesc.repeat_label,
								seriesDesc.program_pattern,
								seriesDesc.expire_date_valid_flag,
								seriesDesc.expire_date,
								seriesDesc.episode_number,
								seriesDesc.last_episode_number,
								seriesDesc.series_name_char);
#endif
						} else {
							len = parseOTHERdesc(ptr);
						}
					}
				}
			}
			ptr += len;
			loop_blen -= len;
		}
		/* 最後のブレークチェック */
    
		if(checkEEVTDitem(&save_eevtitem, NULL, 0)) {
#if 0
				fprintf(stdout, "2EEVT,%d,%d,%d\ndesc1[%s]\ndesc2[%s]\n", 
						eith.service_id,
						eitb.event_id,
						save_eevtitem.descriptor_number,
						save_eevtitem.item_description,
						save_eevtitem.item);
#endif
			append_desc(eittop,eith.service_id,eitb.event_id,&save_eevtitem);
		}
	}

	return;
}

void append_desc(EIT_CONTROL* eittop, int service_id, int event_id, EEVTDitem* eevtitem) {
	EIT_CONTROL *cur;
	int str_alen = 0;

	cur = searcheit(eittop, service_id, event_id);
	if (cur == NULL) {
		return;
	}

	if ( cur->extdesc ) {
		str_alen = strlen( cur->extdesc );
	}
	else {
		str_alen = 0;
	}
	//eevtitem->item_description_length = strlen(eevtitem->item_description);
	//eevtitem->item_length = strlen(eevtitem->item);
	cur->extdesc = realloc(cur->extdesc, str_alen + eevtitem->item_description_length + eevtitem->item_length + 1000);
	if ( !str_alen ) *cur->extdesc = '\0';

	if ( eevtitem->item_description_length && !strstr(cur->extdesc, eevtitem->item_description) ) {
		if (str_alen > 0) {
			strcat(cur->extdesc + str_alen, "　");
		}
		strrep(eevtitem->item_description,"　","");
		strcat(cur->extdesc, eevtitem->item_description);
	}

	if ( eevtitem->item_length && !strstr(cur->extdesc, eevtitem->item) ) {
		strcat(cur->extdesc + str_alen, "　");
		strcat(cur->extdesc, eevtitem->item);
	}
}
void timecmp(int *thh, int *tmm, int *tss,
			 int dhh, int dmm, int dss) {

	int ama;

	*tss += dss;
	ama = *tss % 60;
	*tmm += (*tss / 60);
	*tss = ama;

	*tmm += dmm;
	ama   = *tmm % 60;
	*thh += (*tmm / 60);
	*tmm  = ama;

	*thh += dhh;

}
int dumpEIT2(unsigned char *ptr, SVT_CONTROL *svttop)
{
	EIThead  eith;
	EITbody  eitb;
	SEVTdesc sevtd;

	EEVTDhead eevthead;
	EEVTDitem eevtitem;
	EEVTDtail eevttail;

	EEVTDitem save_eevtitem;

	EIT_CONTROL	*cur ;
	EIT_CONTROL	*eittop;
	SVT_CONTROL	*svtcur;

	int len = 0;
	int loop_len = 0;
	int loop_blen = 0;
	int loop_elen = 0;

	int ehh, emm, ess;

	/* EIT */
	len = parseEIThead(ptr, &eith); 
	/* EIT ヘッダから、どのSVTのEIT情報か特定する */
	svtcur=svttop->next;
	eittop = NULL;
	while(svtcur) {
        	if ((eith.transport_stream_id == svtcur->transport_stream_id) &&
	    	    (eith.original_network_id == svtcur->original_network_id) &&
		    (eith.service_id          == svtcur->event_id)) {
			eittop = svtcur->eit;
			break;
		}
		svtcur = svtcur->next;
	}
	if (eittop == NULL) {
#ifdef DEBUG
		printf("Not Match %x  %x %x \n",eith.transport_stream_id,eith.original_network_id,eith.service_id);
#endif
		// 別のストリーム？？
		return 0;
	}
#ifdef DEBUG
			printf("SV  0x%x Table  [0x%x] \n",eith.service_id,eith.table_id);
#endif
	ptr += len;
	loop_len = eith.section_length - (len - 3 + 4); // 3は共通ヘッダ長 4はCRC
	while(loop_len > 0) {
		/* 連続する拡張イベントは、漢字コードが泣き別れして
		   分割されるようだ。連続かどうかは、item_description_lengthが
		   設定されているかどうかで判断できるようだ。 */
		memset(&save_eevtitem, 0, sizeof(EEVTDitem));

		len = parseEITbody(ptr, &eitb);
#ifdef DEBUG1
	if (eith.table_id == 0x4e) {
		if (eith.section_number < 2) {
			if ((svtcur->cnev[eith.section_number].event_id  != (short)eitb.event_id) ||
				(memcmp(svtcur->cnev[eith.section_number].start_time,eitb.start_time,5)!=0) ||
				(memcmp(svtcur->cnev[eith.section_number].duration,eitb.duration,3)!=0)) {
				// Event Changed
				printf("sv[%x]%s [%x] [%s][%d] state %x\n",svtcur->event_id,eith.section_number?"next":"curr",
				eitb.event_id,
				strTime(getStartTime(eitb.start_time),"%Y/%m/%d %H:%M:%S"),
				getDurationSec(eitb.duration),
				eitb.running_status);
				svtcur->cnev[eith.section_number].event_id = (short)eitb.event_id;
				
				memcpy(svtcur->cnev[eith.section_number].start_time,eitb.start_time,5);
				memcpy(svtcur->cnev[eith.section_number].duration,eitb.duration,3);
			}
		}
	}
#endif

		ehh = eitb.hh;
		emm = eitb.hm;
		ess = eitb.ss;

		timecmp(&ehh, &emm, &ess, eitb.dhh, eitb.dhm, eitb.dss);
		cur = searcheit(eittop, eith.service_id, eitb.event_id);

		ptr += len;
		loop_len -= len;
    
		loop_blen = eitb.descriptors_loop_length;
		loop_len -= loop_blen;
		while(loop_blen > 0) {
			unsigned char desctag;
			int sboff;
			ContentDesc contentDesc;
			ComponentDesc componentDesc;
			AudioComponentDesc audioComponentDesc;
			sboff=0;
			desctag = getBit(ptr,&sboff,8);
#ifdef DEBUG
			printf("Event 0x%x desctag [0x%x] savelen %d\n",eitb.event_id,desctag,save_eevtitem.item_length);
#endif
			switch (desctag) {
				case 0x50:
					len = parseComponentDesc(ptr, &componentDesc);
					if (cur) cur->video = componentDesc.component_type;
					break;
				case 0xC4:
					len = parseAudioComponentDesc(ptr, &audioComponentDesc);
					if (cur) {
						cur->audio = audioComponentDesc.component_type;
						if (audioComponentDesc.ES_multi_lingual_flag==0x01) {
							cur->multiaudio = calloc(1,20);
							strcpy(cur->multiaudio,"二ヶ国語[");
							strncat(cur->multiaudio,audioComponentDesc.ISO_639_language_code,3);
							strcat(cur->multiaudio," ");
							strncat(cur->multiaudio,audioComponentDesc.ISO_639_language_code2,3);
							strcat(cur->multiaudio,"]");
						}

					}
					break;
				default:
					break;
			}

			len = parseSEVTdesc(ptr, &sevtd);
			if(len > 0) {
				if(cur == NULL){
					cur = calloc(1, sizeof(EIT_CONTROL));
					cur->event_id = eitb.event_id ;
					cur->servid = eith.service_id ;
					cur->title = calloc(1, (strlen(sevtd.event_name) + 1));
					memcpy(cur->title, sevtd.event_name, strlen(sevtd.event_name));
					cur->subtitle = calloc(1, (strlen(sevtd.text) + 1));
					memcpy(cur->subtitle, sevtd.text, strlen(sevtd.text));
					cur->yy = eitb.yy;
					cur->mm = eitb.mm;
					cur->dd = eitb.dd;
					cur->hh = eitb.hh;
					cur->hm = eitb.hm;
					cur->ss = eitb.ss;
					cur->ehh = eitb.dhh;
					cur->emm = eitb.dhm;
					cur->ess = eitb.dss ;
					cur->table_id = eith.table_id ;
					cur->freeCA = eitb.free_CA_mode;
					enqueue(eittop, cur);
					if ((eith.table_id >= 0x50) && (!svtcur->haveeitschedule)) {
						svtcur->haveeitschedule=1;
#ifdef DEBUG
						printf("HAVE SCH\n");
#endif
					}
				}
			} else {
				len = parseEEVTDhead(ptr, &eevthead);
				if(len > 0) {
					ptr += len;
					loop_blen -= len;

					loop_elen = eevthead.length_of_items;
					loop_len -= loop_elen;
					while(loop_elen > 0) {
						len = parseEEVTDitem(ptr, &eevtitem,cur);

						ptr += len;
						loop_elen -= len;
						loop_blen -= len;

						if(checkEEVTDitem(&save_eevtitem, &eevtitem, 
										  eevthead.descriptor_number)) {
							append_desc(eittop,eith.service_id,eitb.event_id,&eevtitem);
						}
					}

					len = parseEEVTDtail(ptr, &eevttail);
				} else {
					len = parseContentDesc(ptr, &contentDesc);
					if (len > 0) {
						if (cur) cur->content_type = (unsigned char)(contentDesc.content[0]);
					} else {
						SeriesDesc seriesDesc;
						len = parseSeriesDesc(ptr, &seriesDesc);
						if (len > 0) {
						} else {
							len = parseOTHERdesc(ptr);
						}
					}
				}
				
			}
			ptr += len;
			loop_blen -= len;
		}
		/* 最後のブレークチェック */
    
		if(checkEEVTDitem(&save_eevtitem, NULL, 0)) {
			append_desc(eittop,eith.service_id,eitb.event_id,&save_eevtitem);
		}
	}

	return 1;
}

