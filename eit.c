#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eit.h"

char		*subtitle_cnv_str[] = {
    NULL
};
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
    if (desc->descriptor_length+2>(boff/8)) {
        getStr(desc->content, data, &boff, desc->descriptor_length+2-(boff/8));
    }
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
    int boff = 0,i,j;

    memset(desc, 0, sizeof(ContentDesc));

    desc->descriptor_tag = getBit(data, &boff, 8);
    if((desc->descriptor_tag & 0xFF) != 0x54) {
        return -1;
    }
    desc->descriptor_length = getBit(data, &boff, 8);
    desc->numcontent = desc->descriptor_length/2;
    for(i=0,j=0;i<desc->numcontent;i++) {
        desc->content[j] = getBit(data, &boff,8);
        desc->usernibble[j] = getBit(data, &boff, 8);
        if (desc->content[j] == 0xe0) {
            desc->attachinfo[desc->numattachinfo] = desc->usernibble[j];
            desc->numattachinfo = desc->numattachinfo + 1;
            desc->content[j] = 0x00;
            desc->usernibble[j] = 0x00;
            continue;
        }
        j++;
    }
    desc->numcontent = desc->numcontent - desc->numattachinfo;
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
        newsubtitle = calloc(1, ((strlen(ptr2) + 3) + (strlen(eitptr->subtitle) + 1)));
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
    time_t		rc ;

    if(top->next == NULL){
        top->next = eitptr ;
        eitptr->prev = top ;
        return ;
    }
    cur = top->next ;
    while(cur != NULL){
        rc = cur->start_time - eitptr->start_time;
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
                else {
                    top->next = eitptr;
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

int dumpEIT2(unsigned char *ptr, SVT_CONTROL *svttop,EITCHECK *chk)
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
        return EIT_SDTNOTFOUND;
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
        if (eith.table_id == 0x4e) {
            if (chk) {
                if (eith.section_number < 2) {
                    if ((svtcur->cnev[eith.section_number].event_id  != (short)eitb.event_id) ||
                            (memcmp(svtcur->cnev[eith.section_number].start_time,eitb.start_time,5)!=0) ||
                            (memcmp(svtcur->cnev[eith.section_number].duration,eitb.duration,3)!=0) ||
                            (eitb.running_status == 0x02)) {
                        // Event Changed
                        printf("sv[%d]%s [%d] [%s][%d] state %x\n",svtcur->event_id,eith.section_number?"next":"curr",
                                eitb.event_id,
                                strTime(getStartTime(eitb.start_time),"%Y/%m/%d %H:%M:%S"),
                                getDurationSec(eitb.duration),
                                eitb.running_status);
                        svtcur->cnev[eith.section_number].event_id = (short)eitb.event_id;

                        memcpy(svtcur->cnev[eith.section_number].start_time,eitb.start_time,5);
                        memcpy(svtcur->cnev[eith.section_number].duration,eitb.duration,3);
                        if  ((svtcur->event_id == chk->svid) && (eitb.event_id == chk->evid)) {
                            if (eith.section_number == 0) {
                                // 現在のイベント＝チェックイベント(すでに始まっている)
                                return EIT_CHECKOK;
                            }
                            if (eith.section_number == 1) { // 次のイベントとマッチ
                                if (chk->starttime > 0) { //check
                                    printf("%d\n",getStartTime(eitb.start_time)-chk->starttime);
                                    return EIT_CHECKOK;
                                }
                            }
                        }
                        if (chk->starttime > 0 && svtcur->event_id == chk->svid && svtcur->cnev[1].event_id>0) {
                            //次のイベント情報を取得してもマッチしていない
                            return EIT_CHECKNG;
                        }
                    } //event changed
                    else {
                        if  ((svtcur->event_id == chk->svid) && (svtcur->cnev[1].event_id == chk->evid)) {
                            if (chk->tdttime > 0) { // TDT時刻とチェックして35秒以内ならOK
                                if (getStartTime(svtcur->cnev[1].start_time) < chk->tdttime + 35) {
                                    return EIT_CHECKOK;
                                }
                            }
                        }
                    }
                }
            }
            return EIT_OK;
        }
        cur = searcheit(eittop, eith.service_id, eitb.event_id);

        ptr += len;
        loop_len -= len;

        loop_blen = eitb.descriptors_loop_length;
        loop_len -= loop_blen;
        while(loop_blen > 0) {
            unsigned char desctag;
            int sboff,ix;
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
                    if (cur) { // XXX 2track only
                        if (audioComponentDesc.main_component_flag) ix = 0; //主音声
                        else ix = 1;
                        if (cur->audiodesc[ix].audiotype == 0) {
                            cur->audiodesc[ix].audiotype = audioComponentDesc.component_type;
                            strncpy(cur->audiodesc[ix].langcode,audioComponentDesc.ISO_639_language_code,3);
                            if (audioComponentDesc.ES_multi_lingual_flag) {
                                strcat(cur->audiodesc[ix].langcode,"_");
                                strncat(cur->audiodesc[ix].langcode,audioComponentDesc.ISO_639_language_code2,3);
                            }
                            if (audioComponentDesc.content)
                                cur->audiodesc[ix].audiodesc = strdup(audioComponentDesc.content);
                        }
                    }
                    break;
                case 0x4D:
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
                            cur->table_id = eith.table_id ;
                            cur->freeCA = eitb.free_CA_mode;
                            cur->duration = getDurationSec(eitb.duration);
                            cur->start_time = getStartTime(eitb.start_time);
                            enqueue(eittop, cur);
                            if ((eith.table_id >= 0x50) && (!svtcur->haveeitschedule)) {
                                svtcur->haveeitschedule=1;
#ifdef DEBUG
                                printf("HAVE SCH\n");
#endif
                            }
                        }
                    }
                    break;
                case 0x4E:
                    len = parseEEVTDhead(ptr, &eevthead);
                    if(len > 0) {
                        ptr += len;
                        loop_blen -= len;

                        loop_elen = eevthead.length_of_items;
                        loop_len -= loop_elen;

                        if (cur && cur->eitextcnt == 0 && eevthead.last_descriptor_number > 0) {
                            cur->eitextcnt = eevthead.last_descriptor_number+1;
                            cur->eitextdesc = calloc(cur->eitextcnt,sizeof(EITEXTDESC));
                        }
                        while(loop_elen > 0) {
                            len = parseEEVTDitem(ptr, &eevtitem,cur);

                            ptr += len;
                            loop_elen -= len;
                            loop_blen -= len;

                            if(checkEEVTDitem(&save_eevtitem, &eevtitem, 
                                        eevthead.descriptor_number)) {
                                if (cur && cur->eitextdesc) {
                                    if (!cur->eitextdesc[eevtitem.descriptor_number].item_description)
                                        cur->eitextdesc[eevtitem.descriptor_number].item_description = strdup(eevtitem.item_description);
                                    if (!cur->eitextdesc[eevtitem.descriptor_number].item)
                                        cur->eitextdesc[eevtitem.descriptor_number].item = strdup(eevtitem.item);

                                }

                            }
                        }

                        len = parseEEVTDtail(ptr, &eevttail);

                    }
                    break;

                case 0x54:
                    len = parseContentDesc(ptr, &contentDesc);
                    if (len > 0 && cur) {
                        cur->numcontent = contentDesc.numcontent;
                        memcpy(cur->content,contentDesc.content,7);
                        memcpy(cur->usernibble,contentDesc.usernibble,7);
                        cur->numattachinfo = contentDesc.numattachinfo;
                        memcpy(cur->attachinfo,contentDesc.attachinfo,7);
                    }
                default:
                    len = parseOTHERdesc(ptr,NULL);
                    break;
            }

            ptr += len;
            loop_blen -= len;
        }
        /* 最後のブレークチェック */

        if(checkEEVTDitem(&save_eevtitem, NULL, 0)) {
            if (cur && cur->eitextdesc) {
                if (!cur->eitextdesc[save_eevtitem.descriptor_number].item_description)
                    cur->eitextdesc[save_eevtitem.descriptor_number].item_description = strdup(save_eevtitem.item_description);
                if (!cur->eitextdesc[save_eevtitem.descriptor_number].item)
                    cur->eitextdesc[save_eevtitem.descriptor_number].item = strdup(save_eevtitem.item);
            }

        }
    }

    return EIT_OK;
}

