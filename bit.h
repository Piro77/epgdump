#ifndef BIT_H
#define BIT_H 1

#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "ts_ctl.h"

typedef struct _BIThead {
	char table_id;
	int  section_syntax_indicator;
	int  reserved_future_use;
	int  reserved;
	int  section_length;
	int  original_network_id;
	int  reserved1;
	int  version_number;
	int  current_next_indicator;
	int  section_number;
	int  last_section_number;
	int  reserved_future_use1;
	int  broadcast_view_propriety;
	int  first_descriptors_length;
} BIThead;

typedef struct _BITbody {
	int  transport_stream_id;
	int  original_network_id;
	int  service_id;
	int  event_id;
	char reserved_future_use;
	char running_status;
} BITbody;

typedef struct _SIParameterTable {
    unsigned char   table_id;
    unsigned char   table_description_length;
    unsigned char   table_description_byte[256];
}SIParameterTable;

typedef struct _SIParameterDescriptor {
    unsigned char   parameter_version;
    unsigned short  update_time;
    SIParameterTable    sipt[256];
}SIParameterDescriptor;

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

	int parseBIThead(unsigned char *data, BIThead *h);
	int parseBITbody(unsigned char *data, BITbody *b);
	void dumpBIT(unsigned char *data, SVT_CONTROL *top);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
