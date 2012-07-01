#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <iconv.h>
#include <time.h>

#include "ts.h"
#include "sdt.h"
#include "eit.h"
#include "nit.h"
#include "ts_ctl.h"

#include "xmldata.c"


SVT_CONTROL	*svttop = NULL;
#define		SECCOUNT	10
char	title[1024];
char	subtitle[1024];
char	Category[1024];
char	*extdesc;
char	ServiceName[1024];

/* prototype */
extern int strrep(char *buf, char *mae, char *ato);

void	xmlspecialchars(char *str)
{
	strrep(str, "&", "&amp;");
	strrep(str, "'", "&apos;");
	strrep(str, "\"", "&quot;");
	strrep(str, "<", "&lt;");
	strrep(str, ">", "&gt;");
}


void	GetSDTEITInfo(FILE *infile,SECcache *secs,int count)
{
	SVT_CONTROL	*svtcur ;
	int 		pid;
	SECcache  *bsecs;
	int		sdtflg;
	int ret;

	sdtflg=0;

	while((bsecs = readTS(infile, secs, count)) != NULL) {
		pid = bsecs->pid & 0xFF;
		switch (pid) {
			case 0x10:  //NIT
				dumpNIT(bsecs->buf,svttop);
				break;

			case 0x11: // SDT
				if (sdtflg==0) { 
					sdtflg=1;
					dumpSDT(bsecs->buf, svttop);

					svtcur = svttop->next;
					while(svtcur) {
						if (svtcur->eit == NULL) {
							svtcur->eit = calloc(1, sizeof(EIT_CONTROL));
						}
						svtcur = svtcur->next;
					}

				}
				break;
			case 0x12: // EIT
				if (sdtflg) {
					ret = dumpEIT2(bsecs->buf,svttop);
					if (ret == 0) sdtflg = 0;
				}
				break;
			case 0x14: // TDT
				ret = dumpTDT(bsecs->buf);
				break;
			case 0x23: // TDT
			case 0x26: // TDT
			case 0x27: // TDT
		//		ret = dumpSDTT(bsecs->buf);
				break;
		}

	}
}
void	dumpCSV(FILE *outfile)
{
	SVT_CONTROL	*svtcur ;
	EIT_CONTROL	*eitcur ;

	svtcur=svttop->next;
	while(svtcur != NULL) {
		if (!svtcur->haveeitschedule) {
			svtcur = svtcur->next;
			continue;
		}
		eitcur = svtcur->eit;
		while(eitcur != NULL){
			if(!eitcur->servid){
				eitcur = eitcur->next ;
				continue ;
			}
			fprintf(outfile,"%s,0x%x,0x%x,%d,%d,",svtcur->servicename,svtcur->original_network_id,svtcur->transport_stream_id,svtcur->event_id,svtcur->frequency);
			fprintf(outfile,"0x%x,0x%x,%s,%s,%04d/%02d/%02d %02d:%02d:%02d,%02d:%02d:%02d,%s,%s,%s,0x%x,%s,0x%x,%s,%s,%s\n",
					eitcur->event_id,
					eitcur->content_type,
					ContentCatList[(eitcur->content_type >> 4)].japanese,
					getContentCat(eitcur->content_type),
					eitcur->yy,eitcur->mm,eitcur->dd,eitcur->hh,eitcur->hm,eitcur->ss,
					eitcur->ehh,eitcur->emm,eitcur->ess,
					eitcur->title,
					eitcur->subtitle,
					eitcur->extdesc?eitcur->extdesc:"",
					(unsigned char)eitcur->video,
					getVideoComponentDescStr((unsigned char)eitcur->video),
					eitcur->audio,
					getAudioComponentDescStr(eitcur->audio),
					eitcur->multiaudio?eitcur->multiaudio:"",
					eitcur->freeCA?"有料":"");
			eitcur=eitcur->next;
		}
		svtcur=svtcur->next;
	}
	return ;
}
void	dumpXML(FILE *outfile,char *bs_cs_grch)
{
	SVT_CONTROL	*svtcur ;
	EIT_CONTROL	*eitcur ;
	time_t	l_time ;
	time_t	end_time ;
	struct	tm	tl ;
	struct	tm	*endtl ;
	char	cendtime[32];
	char	cstarttime[32];


	fprintf(outfile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(outfile, "<!DOCTYPE tv SYSTEM \"xmltv.dtd\">\n\n");
	fprintf(outfile, "<tv generator-info-name=\"tsEPG2xml\" generator-info-url=\"http://localhost/\">\n");

	svtcur=svttop->next;
	while(svtcur != NULL) {
		if (!svtcur->haveeitschedule) {
			svtcur = svtcur->next;
			continue;
		}
		memset(ServiceName, '\0', sizeof(ServiceName));
		strcpy(ServiceName, svtcur->servicename);
		xmlspecialchars(ServiceName);

		fprintf(outfile, "  <channel id=\"%s_%d\">\n",bs_cs_grch, svtcur->event_id);
		fprintf(outfile, "    <display-name lang=\"ja_JP\">%s</display-name>\n", ServiceName);
		fprintf(outfile, "  </channel>\n");
		svtcur=svtcur->next;
	}
	svtcur=svttop->next;
	while(svtcur != NULL) {
		if (!svtcur->haveeitschedule) {
			svtcur = svtcur->next;
			continue;
		}
		eitcur = svtcur->eit;
		while(eitcur != NULL){
			if(!eitcur->servid){
				eitcur = eitcur->next ;
				continue ;
			}
			memset(title, '\0', sizeof(title));
			if (eitcur->title) strcpy(title, eitcur->title);
			xmlspecialchars(title);

			memset(subtitle, '\0', sizeof(subtitle));
			if (eitcur->subtitle) strcpy(subtitle, eitcur->subtitle);
			xmlspecialchars(subtitle);

			memset(Category, '\0', sizeof(Category));
			strcpy(Category, ContentCatList[(eitcur->content_type >> 4)].japanese);
			xmlspecialchars(Category);

			tl.tm_sec = eitcur->ss ;
			tl.tm_min = eitcur->hm ;
			tl.tm_hour = eitcur->hh ;
			tl.tm_mday = eitcur->dd ;
			tl.tm_mon = (eitcur->mm - 1);
			tl.tm_year = (eitcur->yy - 1900);
			tl.tm_wday = 0;
			tl.tm_isdst = 0;
			tl.tm_yday = 0;
			l_time = mktime(&tl);
			if((eitcur->ehh == 0) && (eitcur->emm == 0) && (eitcur->ess == 0)){
				(void)time(&l_time);
				end_time = l_time + (60 * 5);		// ５分後に設定
				endtl = localtime(&end_time);
			}else{
				end_time = l_time + eitcur->ehh * 3600 + eitcur->emm * 60 + eitcur->ess;
				endtl = localtime(&end_time);
			}
			memset(cendtime, '\0', sizeof(cendtime));
			memset(cstarttime, '\0', sizeof(cstarttime));
			strftime(cendtime, (sizeof(cendtime) - 1), "%Y%m%d%H%M%S", endtl);
			strftime(cstarttime, (sizeof(cstarttime) - 1), "%Y%m%d%H%M%S", &tl);
			
			fprintf(outfile, "  <programme start=\"%s +0900\" stop=\"%s +0900\" channel=\"%s_%d\">\n",	
				cstarttime, cendtime, bs_cs_grch,svtcur->event_id);
			
			fprintf(outfile, "    <title lang=\"ja_JP\">%s</title>\n", title);
			
			fprintf(outfile, "    <desc lang=\"ja_JP\">%s</desc>\n", subtitle);
			
			fprintf(outfile, "    <category lang=\"ja_JP\">%s</category>\n", Category);
			
			fprintf(outfile, "    <category lang=\"en\">%s</category>\n", ContentCatList[(eitcur->content_type >> 4)].english);
			
			fprintf(outfile, "  </programme>\n");
			eitcur=eitcur->next;
		}
		svtcur=svtcur->next;
	}
    fprintf(outfile, "</tv>\n");
}

int main(int argc, char *argv[])
{

	FILE *infile = stdin;
	FILE *outfile = stdout;
	char *file;
	int   inclose = 0;
	int   outclose = 0;
	SECcache   secs[SECCOUNT];

	/* 興味のあるpidを指定 */
	memset(secs, 0,  sizeof(SECcache) * SECCOUNT);
	secs[0].pid = 0x11; /* SDT */
	secs[1].pid = 0x12; /* EIT */
	secs[2].pid = 0x14; /* TDT */
	secs[3].pid = 0x23; /* TDT */
	secs[4].pid = 0x28; /* TDT */
	secs[5].pid = 0x10; /* NIT */
	secs[6].pid = 0x23; /* SDTT */
	secs[7].pid = 0x26;
	secs[8].pid = 0x27;

	if(argc == 4){
		file = argv[2];
		if(strcmp(file, "-")) {
			infile = fopen(file, "r");
			inclose = 1;
		}
		if(strcmp(argv[3], "-")) {
			outfile = fopen(argv[3], "w+");
			outclose = 1;
		}
	}else{
		fprintf(stdout, "Usage : %s {/BS|/CS|csv} <tsFile> <outfile>\n", argv[0]);
		fprintf(stdout, "Usage : %s <GR Channel> <tsFile> <outfile>\n", argv[0]);
		fprintf(stdout, "Usage : %s check <device> <sid> <eventid>\n", argv[0]);
		fprintf(stdout, "Usage : %s wait <device> <sid> <eventid>\n", argv[0]);
		fprintf(stdout, "\n");
		fprintf(stdout, "  GR Channel Channel identifier (ex. 27)\n");
		fprintf(stdout, "  /BS        BS mode\n");
		fprintf(stdout, "               This mode reads the data of all BS TV stations\n");
		fprintf(stdout, "               from one TS data.\n");
		fprintf(stdout, "  /CS        CS mode\n");
		fprintf(stdout, "               This mode reads the data of two or more CS TV stations\n");
		fprintf(stdout, "               from one TS data.\n");
		fprintf(stdout, "  csv        csv  output mode\n");
		fprintf(stdout, "  check      check event\n");
		fprintf(stdout, "  wait       wait  event\n");
		fprintf(stdout, "VERSION : %s\n",VERSION);
		return 0;
	}

	if(infile == NULL){
		fprintf(stderr, "Can't open file: %s\n", file);
		return 1;
	}

	if(((strcmp(argv[1], "check") == 0)||(strcmp(argv[1],"wait"))==0)){
	}

	svttop = calloc(1, sizeof(SVT_CONTROL));
	GetSDTEITInfo(infile, secs, SECCOUNT);

	if(strcmp(argv[1], "/BS") == 0){
		dumpXML(outfile,"BS");
	}else if(strcmp(argv[1], "/CS") == 0){
		dumpXML(outfile,"CS");
	}else if(strcmp(argv[1], "csv") == 0){
		dumpCSV(outfile);
	}else{
		dumpXML(outfile,argv[1]);
	}
	if(inclose) fclose(infile);
	if(outclose) fclose(outfile);
#if 0

	for(lp = 0 ; lp < staCount ; lp++){
		memset(ServiceName, '\0', sizeof(ServiceName));
		strcpy(ServiceName, pStas[lp].name);
		xmlspecialchars(ServiceName);

		fprintf(outfile, "  <channel id=\"%s\">\n", pStas[lp].ontv);
		fprintf(outfile, "    <display-name lang=\"ja_JP\">%s</display-name>\n", ServiceName);
		fprintf(outfile, "  </channel>\n");
	}
	for(lp = 0 ; lp < staCount ; lp++){
		GetEIT(infile, outfile, &pStas[lp], secs, SECCOUNT);
	}
	fprintf(outfile, "</tv>\n");
	if(inclose) {
		fclose(infile);
	}

	if(outclose) {
		fclose(outfile);
	}
	if(act){
		free(pStas);
		svtcur = svttop ;	//先頭
		while(svtcur != NULL){
			svtsave = svtcur->next ;
			free(svtcur);
			svtcur = svtsave ;
		}
	}
#endif
	return 0;
}
