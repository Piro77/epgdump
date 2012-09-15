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

typedef struct _HEITCycle {
  unsigned char num_of_segment;
  unsigned char cycle;
}HEITCycle;

typedef struct _HEITParam {
  unsigned char media_type;
  unsigned char pattern;
  unsigned char reserved;
  unsigned char schedule_range;
  unsigned char base_cycle;
  unsigned char reserved2;
  unsigned char cycle_group_count;
  HEITCycle cycle[4];
}HEITParam;

typedef struct	_BroadcasterLoop
{
	unsigned char broadcaster_id;
	short broadcaster_descriptors_length;
}BroadcasterLoop;

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

	int parseBIThead(unsigned char *data, BIThead *h);
	int parseBITbody(unsigned char *data, BITbody *b);
	void dumpBIT(unsigned char *data, int *maxcycle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
