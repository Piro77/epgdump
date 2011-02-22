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
#include "ts_ctl.h"

typedef struct _TAG_STATION
{
	char	*name;
	char	*ontv;
	int		tsId;		// OriginalNetworkID
	int		onId;		// TransportStreamID
	int		svId;		// ServiceID
} STATION;

#include "xmldata.c"


SVT_CONTROL	*svttop = NULL;
#define		SECCOUNT	4
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



void	GetSDT(FILE *infile, SVT_CONTROL *svttop, SECcache *secs, int count)
{
	SECcache  *bsecs;

	while((bsecs = readTS(infile, secs, count)) != NULL) {
		/* SDT */
		if((bsecs->pid & 0xFF) == 0x11) {
			dumpSDT(bsecs->buf, svttop);
		}
	}
}
void	GetEIT(FILE *infile, FILE *outfile, STATION *psta, SECcache *secs, int count)
{
	SECcache  *bsecs;
	EIT_CONTROL	*eitcur ;
	EIT_CONTROL	*eitnext ;
	EIT_CONTROL	*eittop = NULL;
	time_t	l_time ;
	time_t	end_time ;
	struct	tm	tl ;
	struct	tm	*endtl ;
	char	cendtime[32];
	char	cstarttime[32];

	eittop = calloc(1, sizeof(EIT_CONTROL));
	eitcur = eittop ;
	fseek(infile, 0, SEEK_SET);
	while((bsecs = readTS(infile, secs, SECCOUNT)) != NULL) {
		/* EIT */
		if((bsecs->pid & 0xFF) == 0x12) { // H-EIT 固定受信機
			dumpEIT(bsecs->buf, psta->svId, psta->onId, psta->tsId, eittop);
		}else if((bsecs->pid & 0xFF) == 0x26) { //M-EIT 移動受信機
			//dumpEIT(bsecs->buf, psta->svId, psta->onId, psta->tsId, eittop);
		}else if((bsecs->pid & 0xFF) == 0x27) { //L-EIT 部分受信機
			//dumpEIT(bsecs->buf, psta->svId, psta->onId, psta->tsId, eittop);
		}
	}
	eitcur = eittop ;
	while(eitcur != NULL){
		if(!eitcur->servid){
			eitcur = eitcur->next ;
			continue ;
		}
		if((eitcur->content_type >> 4) > CAT_COUNT){
			eitcur->content_type = CAT_COUNT -1 ;
		}
		memset(title, '\0', sizeof(title));
		if (eitcur->title) strcpy(title, eitcur->title);
		xmlspecialchars(title);

		memset(subtitle, '\0', sizeof(subtitle));
		if (eitcur->subtitle) strcpy(subtitle, eitcur->subtitle);
		xmlspecialchars(subtitle);

		if (eitcur->extdesc) {
			extdesc = malloc((strlen(eitcur->extdesc)+1)*2);
			strcpy(extdesc, eitcur->extdesc);
			xmlspecialchars(extdesc);
		}
		else {
			extdesc = NULL;
		}

		memset(Category, '\0', sizeof(Category));
		sprintf(Category,"%s %s",ContentCatList[(eitcur->content_type >> 4)].japanese,getContentCat(eitcur->content_type));
		//strcpy(Category, ContentCatList[(eitcur->content_type >> 4)].japanese);
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
#if 1
		fprintf(outfile, "  <programme start=\"%s +0900\" stop=\"%s +0900\" channel=\"%s\">\n",	
				cstarttime, cendtime, psta->ontv);
		fprintf(outfile, "    <title lang=\"ja_JP\">%s</title>\n", title);
		fprintf(outfile, "    <desc lang=\"ja_JP\">%s</desc>\n", subtitle);
		if (extdesc) {
			fprintf(outfile, "    <desc lang=\"ja_JP1\">%s</desc>\n", extdesc);
		}
		fprintf(outfile, "    <category lang=\"ja_JP\">%s</category>\n", Category);
		fprintf(outfile, "    <category lang=\"en\">%s</category>\n", ContentCatList[(eitcur->content_type >> 4)].english);
		fprintf(outfile, "  </programme>\n");
#else
		fprintf(outfile, "(%x:%x:%x)%s,%s,%s,%s,%s,%s\n",
					eitcur->servid, eitcur->table_id, eitcur->event_id,
					cstarttime, cendtime,
					title, subtitle,
					Category,
					ContentCatList[(eitcur->content_type >> 4)].english);
#endif
#if 0
		fprintf(outfile, "(%x:%x)%04d/%02d/%02d,%02d:%02d:%02d,%02d:%02d:%02d,%s,%s,%s,%s\n",
					eitcur->table_id, eitcur->event_id,
					eitcur->yy, eitcur->mm, eitcur->dd,
					eitcur->hh, eitcur->hm, eitcur->ss,
					eitcur->ehh, eitcur->emm, eitcur->ess,
					eitcur->title, eitcur->subtitle,
					ContentCatList[(eitcur->content_type >> 4)].japanese,
					ContentCatList[(eitcur->content_type >> 4)].english);
#endif
		eitnext = eitcur->next ;
		if (eitcur->title) free(eitcur->title);
		if (eitcur->subtitle) free(eitcur->subtitle);
		if (extdesc) {
			free(eitcur->extdesc);
			free(extdesc);
		}
		free(eitcur);
		eitcur = eitnext ;
	}
	free(eittop);
	eittop = NULL;
}
int main(int argc, char *argv[])
{

	FILE *infile = stdin;
	FILE *outfile = stdout;
	char	*arg_onTV ;
	int		staCount ;
	char *file;
	int   inclose = 0;
	int   outclose = 0;
	SVT_CONTROL	*svtcur ;
	SVT_CONTROL	*svtsave ;
	SECcache   secs[SECCOUNT];
	int		lp ;
	STATION	*pStas ;
	int		act ;

	/* 興味のあるpidを指定 */
	memset(secs, 0,  sizeof(SECcache) * SECCOUNT);
	secs[0].pid = 0x11;
	secs[1].pid = 0x12;
	secs[2].pid = 0x26;
	secs[3].pid = 0x27;

	if(argc == 4){
		arg_onTV = argv[1];
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
		fprintf(stdout, "Usage : %s {/BS|/CS} <tsFile> <outfile>\n", argv[0]);
		fprintf(stdout, "Usage : %s <ontvcode> <tsFile> <outfile>\n", argv[0]);
		fprintf(stdout, "\n");
		fprintf(stdout, "  ontvcode   Channel identifier (ex. ****.ontvjapan.com)\n");
		fprintf(stdout, "  /BS        BS mode\n");
		fprintf(stdout, "               This mode reads the data of all BS TV stations\n");
		fprintf(stdout, "               from one TS data.\n");
		fprintf(stdout, "  /CS        CS mode\n");
		fprintf(stdout, "               This mode reads the data of two or more CS TV stations\n");
		fprintf(stdout, "               from one TS data.\n");
		return 0;
	}

	if(strcmp(arg_onTV, "/BS") == 0){
		pStas = bsSta;
		staCount = bsStaCount;
		act = 0 ;
	}else if(strcmp(arg_onTV, "/CS") == 0){
		pStas = csSta;
		staCount = csStaCount;
		act = 0 ;
	}else{
		if(infile == NULL){
			fprintf(stderr, "Can't open file: %s\n", file);
			return 1;
		}
		act = 1 ;
		svttop = calloc(1, sizeof(SVT_CONTROL));
		GetSDT(infile, svttop, secs, SECCOUNT);
		svtcur = svttop->next ;	//先頭
		if(svtcur == NULL){
			free(svttop);
			return 1;
		}

		pStas = calloc(1, sizeof(STATION));
		pStas->tsId = svtcur->transport_stream_id ;
		pStas->onId = svtcur->original_network_id ;
		pStas->svId = svtcur->event_id ;
		pStas->ontv = arg_onTV ;
		pStas->name = svtcur->servicename ;
		staCount = 1;
	}

	fprintf(outfile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(outfile, "<!DOCTYPE tv SYSTEM \"xmltv.dtd\">\n\n");
	fprintf(outfile, "<tv generator-info-name=\"tsEPG2xml\" generator-info-url=\"http://localhost/\">\n");

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

	return 0;
}
