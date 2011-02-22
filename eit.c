#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eit.h"

char		*subtitle_cnv_str[] = {
	NULL
};
static void timecmp(int *,int *,int *,
					int, int, int);

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
	return desc->descriptor_length + 2;
}

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

int parseEEVTDitem(unsigned char *data, EEVTDitem *desc) {
	int boff = 0;
  
	memset(desc, 0, sizeof(EEVTDitem));

	desc->item_description_length = getBit(data, &boff, 8);
	getStr(desc->item_description, data, &boff, desc->item_description_length);

	desc->item_length = getBit(data, &boff, 8);
	memcpy(desc->item, data + (boff / 8), desc->item_length);
	/* getStr(desc->item, data, &boff, desc->item_length); */

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

	if(new == NULL) {
		if(save->item_length != 0) {
			swap = *save;
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

	EIT_CONTROL	*cur ;

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
						len = parseEEVTDitem(ptr, &eevtitem);

						ptr += len;
						loop_elen -= len;
						loop_blen -= len;

						if(checkEEVTDitem(&save_eevtitem, &eevtitem, 
										  eevthead.descriptor_number)) {
#if 0
							if(mode == 1) { /* long format */
								fprintf(out, "EEVT,%d,%d,%d,%s,%s\n", 
										eith.service_id,
										eitb.event_id,
										eevtitem.descriptor_number, /* 退避項目 */
										eevtitem.item_description,
										eevtitem.item);
							}
#endif
						}
					}

					len = parseEEVTDtail(ptr, &eevttail);
#if 0
					if(mode == 1) { /* long format */
						fprintf(out, "EEVTt,%d,%d,%d,%s\n", 
								eith.service_id,
								eitb.event_id,
								eevthead.descriptor_number,
								eevttail.text);
					}
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
								fprintf(out, "Content,%d,%d",
									eith.service_id,
									eitb.event_id);
								header_printed = 1;
							}
#endif
#if 0
							fprintf(out, ",%02x%02x", (unsigned char)contentDesc.content[i], (unsigned char)contentDesc.content[i+1]);
#endif
						}
						if((eith.original_network_id == original_network_id) && (eith.transport_stream_id == transport_stream_id)){
							cur = searcheit(eittop, eith.service_id, eitb.event_id);
							if(cur != NULL){
								cur->content_type = (unsigned char)(contentDesc.content[0] >> 4);
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
			if(mode == 1) { /* long format */
				fprintf(out, "EEVT,%d,%d,%d,%s,%s\n", 
						eith.service_id,
						eitb.event_id,
						save_eevtitem.descriptor_number,
						save_eevtitem.item_description,
						save_eevtitem.item);
			}
#endif
		}
	}

	return;
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
