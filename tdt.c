#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tdt.h"

void dumpTDT(unsigned char *ptr,EITCHECK *chk)
{

	int		rc ;

	int len = 0;
	int loop_len = 0;
	int boff;
	int table_id,section_length,wk;
	unsigned char JST_time[6];
	time_t prevtdt;

	boff = 0;
	prevtdt = 0;
	table_id = getBit(ptr, &boff, 8);
	if (table_id != 0x70 && table_id != 0x73) return;
	wk = getBit(ptr, &boff, 4);
	section_length = getBit(ptr,&boff,12);
	memcpy(JST_time, ptr + (boff / 8),5);
	boff += 40;

	if (chk) {
		if ((chk->tdttime > 0) && (chk->tdtdiff == 0)) {
			prevtdt = chk->tdttime;
		}
		chk->tdttime = getStartTime(JST_time);
		if ((chk->tdtdiff == 0) && (prevtdt > 0)) {
			chk->tdtdiff = (int)chk->tdttime - prevtdt;
#ifdef DEBUG
			printf("tdt diff %d\n",chk->tdtdiff);
#endif
		}
	}
#ifdef DEBUG
	printf("[%s]\n",strTime(getStartTime(JST_time),"%Y/%m/%d %H:%M:%S"));
#endif
	return;
}
