#ifndef SDT_H
#define SDT_H 1

#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "ts_ctl.h"

typedef struct _SDThead {
	char table_id;
	int  section_syntax_indicator;
	int  reserved_future_use1;
	int  reserved1;
	int  section_length;
	int  transport_stream_id;
	int  reserved2;
	int  version_number;
	int  current_next_indicator;
	int  section_number;
	int  last_section_number;
	int  original_network_id;
	int  reserved_future_use2;
} SDThead;

typedef struct _SDTbody {
	int  service_id;
	int  reserved_future_use1;
	int  EIT_user_defined_flags;
	int  EIT_schedule_flag;
	int  EIT_present_following_flag;
	int  running_status;
	int  free_CA_mode;
	int  descriptors_loop_length;
} SDTbody;

typedef struct _SVCdesc {
	int  descriptor_tag;
	int  descriptor_length;
	int  service_type;
	int  service_provider_name_length;
	char service_provider_name[MAXSECLEN];
	int  service_name_length;
	char service_name[MAXSECLEN];
} SVCdesc;


#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

	int parseSDThead(unsigned char *data, SDThead *h);
	int parseSDTbody(unsigned char *data, SDTbody *b);
	int parseSVCdesc(unsigned char *data, SVCdesc *desc);
	void dumpSDT(unsigned char *data, SVT_CONTROL *top);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
