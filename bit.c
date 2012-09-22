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
int parseSIParameterDescriptor(unsigned char *data, SIParameterDescriptor *d,int onid,int *maxcycle) {
    int boff = 0;
    int i,j,k,tag,alen,len,looplen,wk;
    HEITParam basiceitparam[5];
    unsigned char *ptr;

    memset(d,0,sizeof(SIParameterDescriptor));
    memset(basiceitparam,0,sizeof(HEITParam)*5);

    tag = getBit(data, &boff, 8);
    len = getBit(data, &boff, 8);

    d->parameter_version = getBit(data, &boff, 8);
    d->update_time = getBit(data, &boff, 16);

    looplen = len - 3;
    ptr = data + 5;
    i=0;
    while(looplen > 0) {
        boff = 0;
        d->sipt[i].table_id = getBit(ptr, &boff, 8);
        d->sipt[i].table_description_length = getBit(ptr, &boff, 8);
        switch(d->sipt[i].table_id) 
        {
#ifdef DEBUG
            case 0x40:  //NIT
                wk = getBit(ptr, &boff, 8);
                printf("NIT CYCLE %d\n",wk);
                break;
            case 0xc4:  //BIT
                wk = getBit(ptr, &boff, 8);
                printf("BIT CYCLE %d\n",wk);
                break;
            case 0xc3:  //SDTT
                wk = getBit(ptr, &boff, 16);
                printf("SDTT CYCLE %d\n",wk);
                break;
            case 0xc8:  //CDT
                wk = getBit(ptr, &boff, 16);
                printf("CDT CYCLE %d\n",wk);
                break;
            case 0x42:  //SDT
            case 0x46:
                wk = getBit(ptr, &boff, 8);
                printf("SDT CYCLE %d\n",wk);
                break;
            case 0x4E:  //HEIT(GR) Or (EIT p/f actual)
            case 0x4F:  //BSCS (EIT p/f other)
                if (onid > 0x0010) {
                    wk = getBit(ptr, &boff, 8);
                    printf("HEIT CYCLE %d\n",wk);
                    wk = getBit(ptr, &boff, 8);
                    printf("MEIT CYCLE %d\n",wk);
                    wk = getBit(ptr, &boff, 8);
                    printf("LEIT CYCLE %d\n",wk);
                    wk = getBit(ptr, &boff, 4);
                    printf("MEIT EvNUM %d\n",wk);
                    wk = getBit(ptr, &boff, 4);
                    printf("LEIT EvNUM %d\n",wk);
                }
                else {
                    wk = getBit(ptr, &boff, 8);
                    printf("EITpf CYCLE %d\n",wk);
                }
                break;
#endif
            case 0x50:  //HEIT
            case 0x58:  //EIText
            case 0x60:  //EIT(sche)
                for(j=0;j<d->sipt[i].table_description_length/6;j++) {
                    basiceitparam[j].media_type = getBit(ptr, &boff, 2);
                    basiceitparam[j].pattern = getBit(ptr, &boff, 2);
                    basiceitparam[j].reserved = getBit(ptr, &boff, 4);
                    basiceitparam[j].schedule_range = getBit(ptr, &boff, 4) *10 + getBit(ptr,&boff,4);
                    basiceitparam[j].base_cycle = getBit(ptr, &boff, 4) * 100 + getBit(ptr,&boff,4) * 10 + getBit(ptr,&boff,4);
                    if (basiceitparam[j].media_type == 0x01) {
                        if (basiceitparam[j].base_cycle > *maxcycle)
                            *maxcycle = basiceitparam[j].base_cycle;
                    }
#ifdef DEBUG
                    printf("HEIT%x Basic media %d  schedule_range %d base_cycle %d\n",d->sipt[i].table_id,basiceitparam[j].media_type ,basiceitparam[j].schedule_range,basiceitparam[j].base_cycle);
#endif
                    basiceitparam[j].reserved2 = getBit(ptr, &boff, 2);
                    basiceitparam[j].cycle_group_count = getBit(ptr, &boff, 2);
                    for(k=0;k<basiceitparam[j].cycle_group_count;k++) {
                        basiceitparam[j].cycle[k].num_of_segment = getBit(ptr, &boff, 8);
                        basiceitparam[j].cycle[k].cycle = getBit(ptr, &boff, 8);
                    }
                }
                break;
        }
        looplen = looplen - d->sipt[i].table_description_length - 2;
        ptr +=  d->sipt[i].table_description_length + 2;
        i++;
    }
    return len + 2;
}

int parseBroadcasterLoop(unsigned char *data,BroadcasterLoop *l) 
{
    int boff = 0;

    memset(l,0,sizeof(BroadcasterLoop));

    l->broadcaster_id = getBit(data,&boff,8);
    getBit(data,&boff,4);
    l->broadcaster_descriptors_length = getBit(data,&boff,12);

    return 3;

}

void dumpBIT(unsigned char *ptr, int *maxcycle)
{

    BIThead  bith;
    BITbody  bitb;
    SIParameterDescriptor sipd;
    BroadcasterLoop bl;
    int		rc ;
    unsigned char tag;
    static unsigned char version = 0xff;

    int len = 0;
    int loop_len = 0;
    int boff = 0;

    /* BIT */
    len = parseBIThead(ptr, &bith); 
    ptr += len;
    loop_len = bith.section_length - (len - 3 + 4); // 3は共通ヘッダ長 4はCRC

    if (bith.version_number == version) return;

    version = bith.version_number;

    len = parseSIParameterDescriptor(ptr,&sipd,bith.original_network_id, maxcycle);
    ptr += len;
    loop_len = loop_len - len;
    while(loop_len > 0) {
        int loop_len2;
        // broadcaster loop
        len = parseBroadcasterLoop(ptr,&bl);
        loop_len2 = bl.broadcaster_descriptors_length;
        ptr += len;
        loop_len -= len;
        while(loop_len2 > 0) {
            len = parseOTHERdesc(ptr,&tag);
            if (tag == 0xd7)
                parseSIParameterDescriptor(ptr,&sipd,bith.original_network_id, maxcycle);
#ifdef DEBUG
            if (tag == 0xd8) {
                int boff1=0;
                char name[40];
                memset(name,0,40);
                getStr(name, ptr+2, &boff1, len-2);
                printf("%s\n",name);
            }
#endif
            ptr += len;
            loop_len2 -= len;
            loop_len -= len;
        }
    }

    return;
}
