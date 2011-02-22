#ifndef EIT_H
#define EIT_H 1

#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "ts_ctl.h"

typedef struct _EIThead {
	char table_id;
	int section_syntax_indicator;
	int reserved_future_use;
	int reserved1;
	int section_length;
	int service_id;
	int reserved2;
	int version_number;
	int current_next_indicator;
	int section_number;
	int last_section_number;
	int transport_stream_id;
	int original_network_id;
	int segment_last_section_number;
	int last_table_id;
} EIThead;

typedef struct _EITbody {
	int event_id;
	char start_time[5];
	unsigned char duration[3];
	int running_status;
	int free_CA_mode;
	int descriptors_loop_length;
	/* 以下は解析結果保存用 */
	int yy;
	int mm;
	int dd;
	int hh;
	int hm;
	int ss;
	int dhh;
	int dhm;
	int dss;
} EITbody;

typedef struct _SEVTdesc {
	int  descriptor_tag;
	int  descriptor_length;
	char ISO_639_language_code[3];
	int  event_name_length;
	char event_name[MAXSECLEN];
	int  text_length;
	char text[MAXSECLEN];
} SEVTdesc;

typedef struct _ContentDesc {
	int descriptor_tag;
	int descriptor_length;
	char content[MAXSECLEN];
} ContentDesc;

typedef struct _SeriesDesc {
	int descriptor_tag;
	int descriptor_length;
	int series_id;
	int repeat_label;
	int program_pattern;
	int expire_date_valid_flag;
	int expire_date;
	int episode_number;
	int last_episode_number;
	char series_name_char[MAXSECLEN];
} SeriesDesc;

typedef struct _EEVTDhead {
	int  descriptor_tag;
	int  descriptor_length;
	int  descriptor_number;
	int  last_descriptor_number;
	char ISO_639_language_code[3];
	int  length_of_items;
} EEVTDhead;

typedef struct _EEVTDitem {
	int  item_description_length;
	char item_description[MAXSECLEN];
	int  item_length;
	char item[MAXSECLEN];
	/* 退避用 */
	int  descriptor_number;
} EEVTDitem;

typedef struct _EEVTDtail {
	int  text_length;
	char text[MAXSECLEN];
} EEVTDtail;

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

	int parseEIThead(unsigned char *data, EIThead *h);
	int parseEITbody(unsigned char *data, EITbody *b);
	int parseSEVTdesc(unsigned char *data, SEVTdesc *desc) ;

	int parseContentDesc(unsigned char *data, ContentDesc *desc);
	int parseSeriesDesc(unsigned char *data, SeriesDesc *desc);

	int parseEEVTDhead(unsigned char *data, EEVTDhead *desc) ;
	int parseEEVTDitem(unsigned char *data, EEVTDitem *desc) ;
	int parseEEVTDtail(unsigned char *data, EEVTDtail *desc) ;

	void dumpEIT(unsigned char *data, int serv_id, int original_network_id, int transport_stream_id, EIT_CONTROL *eittop);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
