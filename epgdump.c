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
#include "tdt.h"
#include "bit.h"
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


int     CheckEIT(FILE *infile,SECcache *secs,int count,EITCHECK *chk)
{
	SVT_CONTROL	*svtcur ;
	int 		pid,ret,sdtflg;
	SECcache  *bsecs;

	svttop = calloc(1, sizeof(SVT_CONTROL));
    sdtflg = 0;

	while((bsecs = readTS(infile, secs, count)) != NULL) {
		pid = bsecs->pid & 0xFF;
		switch (pid) {
			case 0x11: //SDT
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
				ret = dumpEIT2(bsecs->buf,svttop,chk);
				if (ret == EIT_CHECKOK || ret == EIT_CHECKNG) { //CHECK COMPLETE
					return ret - EIT_CHECKOK;
				}
				if (ret == EIT_SDTNOTFOUND) sdtflg=0;
				break;
			case 0x14: // TDT
				dumpTDT(bsecs->buf,chk);
				break;
			case 0x13: // RST
				printf("RST\n");
				break;
        }
	if (chk->waitend < time(NULL)) {return 1;}
    }
    return 1; // EOF
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
			case 0x26: // EIT(地デジ)
			case 0x27: // EIT(地デジ)
				if (sdtflg) {
					ret = dumpEIT2(bsecs->buf,svttop,NULL);
					if (ret == 0) sdtflg = 0;
				}
				break;
			case 0x14: // TDT
				dumpTDT(bsecs->buf,NULL);
				break;
			case 0x23: // SDTT
		//		ret = dumpSDTT(bsecs->buf);
				break;
			case 0x13: // RST
				printf("RST\n");
				break;
			case 0x24: // BIT
				//dumpBIT(bsecs->buf,svttop);
				break;
		}

	}
}
void	dumpCSV(FILE *outfile)
{
	SVT_CONTROL	*svtcur ;
	EIT_CONTROL	*eitcur ;
    int i;

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
			fprintf(outfile,"0x%x,0x%x,%s,%s,%04d/%02d/%02d %02d:%02d:%02d,%02d:%02d:%02d,\"%s\",\"%s\",",
					eitcur->event_id,
					eitcur->content_type,
					ContentCatList[(eitcur->content_type >> 4)].japanese,
					getContentCat(eitcur->content_type),
					eitcur->yy,eitcur->mm,eitcur->dd,eitcur->hh,eitcur->hm,eitcur->ss,
					eitcur->ehh,eitcur->emm,eitcur->ess,
					eitcur->title,
					eitcur->subtitle);
            if (eitcur->eitextcnt>0) {
                fprintf(outfile,"\"");
                for(i=0;i<eitcur->eitextcnt;i++) {
                    if (eitcur->eitextdesc[i].item_description)
			            fprintf(outfile, "%s\n",eitcur->eitextdesc[i].item_description);
                    if (eitcur->eitextdesc[i].item)
			            fprintf(outfile, "%s\n", eitcur->eitextdesc[i].item);
                }
                fprintf(outfile,"\",");
            }
            else {
                fprintf(outfile,",");
            }
			fprintf(outfile,"0x%x,%s,",
					(unsigned char)eitcur->video,
					getVideoComponentDescStr((unsigned char)eitcur->video));
            for(i=0;i<2;i++) {
                if (eitcur->audiodesc[i].audiotype > 0) {
			    fprintf(outfile,"0x%x-%s-%s-%s,",
                        eitcur->audiodesc[i].audiotype,
                        getAudioComponentDescStr(eitcur->audiodesc[i].audiotype),
                        eitcur->audiodesc[i].langcode,
                        eitcur->audiodesc[i].audiodesc?eitcur->audiodesc[i].audiodesc:"");
                }
                else {
			fprintf(outfile,",");
                }
            }
			fprintf(outfile,"%s\n",eitcur->freeCA?"有料":"");
			eitcur=eitcur->next;
		}
		svtcur=svtcur->next;
	}
	return ;
}
void	dumpXML(FILE *outfile)
{
	SVT_CONTROL	*svtcur ;
	EIT_CONTROL	*eitcur ;
	time_t	l_time ;
	time_t	end_time ;
	struct	tm	tl ;
	struct	tm	*endtl ;
	char	cendtime[32];
	char	cstarttime[32];
	int	i;


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

		fprintf(outfile, "  <channel id=\"%s_%d\" transport_stream_id=\"%d\" original_network_id=\"%d\" service_id=\"%d\">\n",getBSCSGR(svtcur), svtcur->event_id,svtcur->transport_stream_id,svtcur->original_network_id,svtcur->event_id);
		fprintf(outfile, "    <display-name lang=\"ja_JP\">%s</display-name>\n", ServiceName);
		if (svtcur->original_network_id < 0x0010) {
			fprintf(outfile, "    <satelliteinfo>\n");
            if (svtcur->frequency > 0) {
			    fprintf(outfile, "       <frequency>%d</frequency>\n",svtcur->frequency);
            }
			fprintf(outfile, "       <TP>%s%d</TP>\n",getTSID2BSCS(svtcur->transport_stream_id),getTSID2TP(svtcur->transport_stream_id));
			fprintf(outfile, "       <SLOT>%d</SLOT>\n",getTSID2SLOT(svtcur->transport_stream_id));
			fprintf(outfile, "    </satelliteinfo>\n");
		}
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
			
			fprintf(outfile, "  <programme start=\"%s +0900\" stop=\"%s +0900\" channel=\"%s_%d\" ",
				cstarttime, cendtime, getBSCSGR(svtcur),svtcur->event_id);
			fprintf(outfile, "transport_stream_id=\"%d\" original_network_id=\"%d\" service_id=\"%d\" event_id=\"%d\">\n",
				svtcur->transport_stream_id,
				svtcur->original_network_id,
				svtcur->event_id,eitcur->event_id);

			fprintf(outfile, "    <title lang=\"ja_JP\">%s</title>\n", title);
			
			fprintf(outfile, "    <desc lang=\"ja_JP\">%s</desc>\n", subtitle);

			fprintf(outfile, "    <category lang=\"ja_JP\">%s</category>\n", Category);
			
			fprintf(outfile, "    <category lang=\"en\">%s</category>\n", ContentCatList[(eitcur->content_type >> 4)].english);

			fprintf(outfile, "    <subcategory id=\"%d\">%s</subcategory>\n",eitcur->content_type, getContentCat(eitcur->content_type));

			fprintf(outfile, "    <video id=\"%d\">\n",(unsigned char)eitcur->video);
			fprintf(outfile, "       <resolution>%s</resolution>\n",getVideoResolution(eitcur->video));
			fprintf(outfile, "       <aspect>%s</aspect>\n",getVideoAspect(eitcur->video));
			fprintf(outfile, "    </video>\n");
			for(i=0;i<2;i++) {
				if (eitcur->audiodesc[i].audiotype > 0) {
					fprintf(outfile, "    <audio id=\"%d\">\n",(unsigned char)eitcur->audiodesc[i].audiotype);
					fprintf(outfile, "       <desc>%s</desc>\n",getAudioComponentDescStr(eitcur->audiodesc[i].audiotype));
					fprintf(outfile, "       <lang>%s</lang>\n",eitcur->audiodesc[i].langcode);
					fprintf(outfile, "       <extdesc>%s</extdesc>\n",eitcur->audiodesc[i].audiodesc?eitcur->audiodesc[i].audiodesc:"");
					fprintf(outfile, "    </audio>\n");
				}
			}
			if (eitcur->eitextcnt>0) {
				char *p;
				fprintf(outfile, "    <extdesc>\n");
				for(i=0;i<eitcur->eitextcnt;i++) {
					if (eitcur->eitextdesc[i].item_description) {
						p = realloc(eitcur->eitextdesc[i].item_description,(strlen(eitcur->eitextdesc[i].item_description)*2)+1);
						xmlspecialchars(p);
						fprintf(outfile, "     <item_description>%s</item_description>\n",p);
						free(p);
					}
					if (eitcur->eitextdesc[i].item) {
						p = realloc(eitcur->eitextdesc[i].item,(strlen(eitcur->eitextdesc[i].item)*2)+1);
						xmlspecialchars(p);
						fprintf(outfile, "     <item>%s</item>\n",p);
						free(p);
					}
				}
				fprintf(outfile, "    </extdesc>\n");
			}
			
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
	char *file,*fileout;
	int   inclose = 0;
	int   outclose = 0;
	int	ret;
	SECcache   secs[SECCOUNT];

	/* 興味のあるpidを指定 */
	memset(secs, 0,  sizeof(SECcache) * SECCOUNT);
	secs[0].pid = 0x11; /* SDT */
	secs[1].pid = 0x12; /* EIT */
	secs[2].pid = 0x14; /* TDT */
	secs[3].pid = 0x23; /* SDTT */
	secs[4].pid = 0x28; /* SDTT */
	secs[5].pid = 0x10; /* NIT */
	secs[6].pid = 0x13; /* RST */
	secs[7].pid = 0x24; /* BIT */

    file = NULL;
    fileout= NULL;
	if (argc > 2) {
    if (argc == 3) {
        file = argv[1];
        fileout = argv[2];
    }
    else {
        file = argv[2];
        fileout = argv[3];
   }
		if(strcmp(file, "-")) {
			infile = fopen(file, "r");
			inclose = 1;
		}
	if(infile == NULL){
		fprintf(stderr, "Can't open file: %s\n", file);
		return 1;
	}
	}

	if(argc == 6 && ((strcmp(argv[1], "check") == 0)||(strcmp(argv[1],"wait"))==0)){
		EITCHECK chk;
		memset(&chk,0,sizeof(EITCHECK));
		chk.svid = atoi(argv[3]);
		chk.evid = atoi(argv[4]);
		if (strcmp(argv[1],"check")==0) {
			chk.starttime = str2timet(argv[5]);
			chk.waitend = time(NULL) + 11;
		}
		else {
			chk.waitend = time(NULL) + atoi(argv[5]);
		}
		ret = CheckEIT(infile,secs, SECCOUNT,&chk);
		if (inclose) fclose(infile);
		// 0..ok 1..fail
		return ret;
	}

	if(argc >= 3){
		if(strcmp(fileout, "-")) {
			outfile = fopen(fileout, "w+");
			outclose = 1;
		}
	}else{
		fprintf(stdout, "Usage : %s <tsFile> <outfile>\n", argv[0]);
		fprintf(stdout, "Usage : %s csv <tsFile> <outfile>\n", argv[0]);
		fprintf(stdout, "Usage : %s check <device> <sid> <eventid> <eventtime>\n", argv[0]);
		fprintf(stdout, "Usage : %s wait <device> <sid> <eventid> <maxwaitsec>\n", argv[0]);
		fprintf(stdout, "  csv        csv  output mode\n");
		fprintf(stdout, "  check      check event\n");
		fprintf(stdout, "  wait       wait  event\n");
		fprintf(stdout, "VERSION : %s\n",VERSION);
		return 0;
	}



	svttop = calloc(1, sizeof(SVT_CONTROL));

	GetSDTEITInfo(infile, secs, SECCOUNT);

	if(strcmp(argv[1], "csv") == 0){
		dumpCSV(outfile);
	}else{
		dumpXML(outfile);
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
