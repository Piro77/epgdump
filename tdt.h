#ifndef TDT_H
#define TDT_H 1

#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "ts_ctl.h"

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

	void dumpTDT(unsigned char *data,EITCHECK *eitcheck);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
