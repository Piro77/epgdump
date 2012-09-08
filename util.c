#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "aribstr.h"
#include "util.h"

int strrep(char *buf, char *mae, char *ato)
{
    char *mitsuke, *findpos;
	size_t maelen, atolen;
	int shift;
		    
	findpos = buf;
	maelen = strlen(mae);
	atolen = strlen(ato);
	shift = (int)(strlen(ato)-strlen(mae));

	if (maelen == 0 || strstr(findpos, mae) == NULL) return 0;
	while ((mitsuke = strstr(findpos, mae)) != NULL) {
		if (shift > 0) {
			memmove(mitsuke + shift, mitsuke, strlen(mitsuke) + 1);
		} else if (shift < 0) {
			memmove(mitsuke, mitsuke - shift, strlen(mitsuke) + shift + 1);
		}
		memmove(mitsuke, ato, atolen);
		findpos = mitsuke + atolen;
	}
	return 1;
}

int getBit(unsigned char *byte, int *pbit, int gbit) {
	int pbyte = *pbit / 8;
	unsigned char *fbyte = byte + pbyte;

	int cutbit = *pbit - (pbyte * 8);
	int lcutbit = 32 - (cutbit + gbit);

	unsigned char tbuf[4]; /* intの最大32bit */
	unsigned int tnum;

        tnum = fbyte[0] << 24 | fbyte[1] << 16 | fbyte[2] << 8 | fbyte[3];
        tnum = ((tnum >> lcutbit) & ~(~0 << gbit));
        *pbit += gbit;
        return tnum;


	//memcpy(tbuf, fbyte, sizeof(unsigned char) * 4);
	tbuf[0]=fbyte[0];
	tbuf[1]=fbyte[1];
	tbuf[2]=fbyte[2];
	tbuf[3]=fbyte[3];

	/* 先頭バイトから不要bitをカット */
	tbuf[0] = tbuf[0] << cutbit;
	tbuf[0] = tbuf[0] >> cutbit;

	/* intにしてしまう */
	tnum = tbuf[0] << 24 | tbuf[1] << 16 | tbuf[2] << 8 | tbuf[3];

	/* 後ろの不要バイトをカット */
	tnum = tnum >> lcutbit;

	*pbit += gbit;

	return tnum;
  
}

void getStr(char *tostr, unsigned char *byte, int *pbit, int len) {
	char str[MAXSECLEN];
	int pbyte = *pbit / 8;
	unsigned char *fbyte = byte + pbyte;

	memset(str, 0, sizeof(char) * MAXSECLEN);
	memcpy(str, fbyte, len);

	*pbit += (len * 8);
  
	AribToString(tostr, str, len);

	return;
  
}

int parseOTHERdesc(unsigned char *data,unsigned char *tag) {
	int boff = 0;
	int descriptor_tag;
	int descriptor_length;

	descriptor_tag = getBit(data, &boff, 8);
	descriptor_length = getBit(data, &boff, 8);

	if (tag) {
		*tag = descriptor_tag;
#ifdef DEBUG
		printf("other desc_tag:0x%x len %d\n", descriptor_tag,descriptor_length);
#endif
	}

	return descriptor_length + 2;
}


void* allocCopy(void* src, int *boff, size_t size) {
        void* mem = NULL;
        if ( size == 0 ) return NULL;

        mem = malloc(size);
        memcpy(mem, src + *boff / 8, size);
        *boff += size * 8;
        return mem;
}

time_t getStartTime(unsigned char *starttime)
{

	int tnum,yy,mm,dd;
	char buf[10];
        time_t  l_time ;
        time_t  end_time ;
        struct  tm      tl ;
        struct  tm      *endtl ;
        char    cendtime[32];
        char    cstarttime[32];

	tnum = (starttime[0] & 0xFF) << 8 | (starttime[1] & 0xFF);

	yy = (tnum - 15078.2) / 365.25;
	mm = ((tnum - 14956.1) - (int)(yy * 365.25)) / 30.6001;
	dd = (tnum - 14956) - (int)(yy * 365.25) - (int)(mm * 30.6001);

        if(mm == 14 || mm == 15) {
                yy += 1;
                mm = mm - 1 - (1 * 12);
        } else {
                mm = mm - 1;
        }

	tl.tm_year = yy;
	tl.tm_mon = mm - 1;
	tl.tm_mday = dd;
	memset(buf, '\0', sizeof(buf));
	sprintf(buf, "%x", starttime[2]);
	tl.tm_hour = atoi(buf);
	memset(buf, '\0', sizeof(buf));
	sprintf(buf, "%x", starttime[3]);
	tl.tm_min = atoi(buf);
	memset(buf, '\0', sizeof(buf));
	sprintf(buf, "%x", starttime[4]);
	tl.tm_sec = atoi(buf);

	tl.tm_wday = 0;
	tl.tm_isdst = 0;
	tl.tm_yday = 0;

	l_time = mktime(&tl);
	return l_time;

}

int getDurationSec(unsigned char *duration)
{
	int hh,mm,ss;
	char buf[24];
        if((duration[0] == 0xFF) && (duration[1] == 0xFF) && (duration[2] == 0xFF)){
		// 終了未定
                hh = mm = ss = 0;
		ss = -1;
        }else{
                memset(buf, '\0', sizeof(buf));
		sprintf(buf, "%x", duration[0]);
        	hh = atoi(buf)*3600;
                memset(buf, '\0', sizeof(buf));
        	sprintf(buf, "%x", duration[1]);
        	mm = atoi(buf)*60;
                memset(buf, '\0', sizeof(buf));
        	sprintf(buf, "%x", duration[2]);
        	ss = atoi(buf);
	}

	return hh+mm+ss;
}

char *strTime(time_t tx,char *format)
{
	static char tstr[32];
	struct tm *tl;

	memset(tstr,0,sizeof(tstr));
	tl = localtime(&tx);
	strftime(tstr, (sizeof(tstr) - 1), format, tl);
	return tstr;
}

double getTimeDiff(time_t tottime)
{
	struct timeval timeofday;

	gettimeofday( &timeofday, NULL );
	return (double)tottime-(double)timeofday.tv_sec+(double)timeofday.tv_usec*1e-6;
}


/*
 *   YYYY/MM/DD HH:MM:SS -> time_t
 *   YYYY-MM-DDTHH:MM:SS -> time_t
 *
 */
time_t str2timet(char *str)
{
  int len;
  struct tm tms;

  if (!str) return 0;
  len = strlen(str);
  if (len == 0) return 0;

  memset(&tms, 0, sizeof(struct tm));

  if((len >= 19) && ((str[4] == '-')||(str[4] == '/'))) {
    tms.tm_year = atoi(str) - 1900;
    tms.tm_mon  = atoi(str+5)-1;
    tms.tm_mday = atoi(str+8);
    tms.tm_hour = atoi(str+11);
    tms.tm_min = atoi(str+14);
    tms.tm_sec = atoi(str+17);
    return mktime(&tms);
  }
  return 0;
}
