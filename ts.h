#ifndef TS_H
#define TS_H 1

#include <stdio.h>
#include <stdlib.h>

#include "util.h"

#define TSPAYLOADMAX 184

typedef struct _TSpacket { 
	char     sync;
	int      transport_error_indicator;
	int      payload_unit_start_indicator;
	int      transport_priority;
	int      pid;
	int      transport_scrambling_control;
	int      adaptation_field_control;
	int      continuity_counter;
	int      adaptation_field;
	unsigned char payload[TSPAYLOADMAX];
	int      payloadlen;
	int      rcount;
} TSpacket;

typedef struct _SECcache { 
	int      pid;
	unsigned char buf[MAXSECLEN];
	int      seclen;
	int      setlen;
	TSpacket cur;
	int      curlen;
	int      cont;
} SECcache;

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

	SECcache *readTS(FILE *in, SECcache secs[], int secscount);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
