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

typedef		struct	_ContentTYPE{
	char	*japanese ;
	char	*english ;
}CONTENT_TYPE;

#define		CAT_COUNT		16
static  CONTENT_TYPE	ContentCatList[CAT_COUNT] = {
	{ "ニュース・報道", "news" },
	{ "スポーツ", "sports" },
	{ "情報", "information" },
	{ "ドラマ", "drama" },
	{ "音楽", "music" },
	{ "バラエティ", "variety" },
	{ "映画", "cinema" },
	{ "アニメ・特撮", "anime" },
	{ "ドキュメンタリー・教養", "documentary" },
	{ "演劇", "stage" },
	{ "趣味・実用", "hobby" },
	{ "福祉", "etc" },			//福祉
	{ "予備", "etc" }, //予備
	{ "予備", "etc" }, //予備
	{ "予備", "etc" }, //予備
	{ "その他", "etc" } //その他
};
typedef struct _TAG_STATION
{
	char	*name;
	char	*ontv;
	int		tsId;		// OriginalNetworkID
	int		onId;		// TransportStreamID
	int		svId;		// ServiceID
} STATION;

static STATION bsSta[] = {
	{ "NHK BS1", "3001.ontvjapan.com", 16625, 4, 101},
	{ "NHK BS2", "3002.ontvjapan.com", 16625, 4, 102},
	{ "NHK BSh", "3003.ontvjapan.com", 16626, 4, 103},
	{ "BS日テレ", "3004.ontvjapan.com", 16592, 4, 141},
	{ "BS朝日", "3005.ontvjapan.com", 16400, 4, 151},
	{ "BS-TBS", "3006.ontvjapan.com", 16401, 4, 161},
	{ "BSジャパン", "3007.ontvjapan.com", 16433, 4, 171},
	{ "BSフジ", "3008.ontvjapan.com", 16593, 4, 181},
	{ "WOWOW", "3009.ontvjapan.com", 16432, 4, 191},
	{ "WOWOW2", "3010.ontvjapan.com", 16432, 4, 192},
	{ "WOWOW3", "3011.ontvjapan.com", 16432, 4, 193},
	{ "スター・チャンネル", "3012.ontvjapan.com", 16529, 4, 200},
	{ "BS11", "3013.ontvjapan.com", 16528, 4, 211},
	{ "TwellV", "3014.ontvjapan.com", 16530, 4, 222},
};

static int bsStaCount = sizeof(bsSta) / sizeof (STATION);



static STATION csSta[] = {
	{ "スターｃｈプラス", "1002.ontvjapan.com", 24608, 6, 237},
	{ "日本映画専門ｃｈＨＤ", "1086.ontvjapan.com", 24608, 6, 239},
	{ "フジテレビＣＳＨＤ", "306ch.epgdata.ontvjapan", 24608, 6, 306},
	{ "ショップチャンネル", "1059.ontvjapan.com", 24704, 6, 55},
	{ "ザ・シネマ", "1217.ontvjapan.com", 24736, 6, 228},
	{ "スカチャンＨＤ８００", "800ch.epgdata.ontvjapan", 24736, 6, 800},
	{ "スカチャン８０１", "801ch.epgdata.ontvjapan", 24736, 6, 801},
	{ "スカチャン８０２", "802ch.epgdata.ontvjapan", 24736, 6, 802},
	{ "ｅ２プロモ", "100ch.epgdata.ontvjapan", 28736, 7, 100},
	{ "インターローカルＴＶ", "194ch.epgdata.ontvjapan", 28736, 7, 194},
	{ "Ｊスポーツ　ＥＳＰＮ", "1025.ontvjapan.com", 28736, 7, 256},
	{ "ＦＯＸ", "1016.ontvjapan.com", 28736, 7, 312},
	{ "スペースシャワーＴＶ", "1018.ontvjapan.com", 28736, 7, 322},
	{ "カートゥーン　ネット", "1046.ontvjapan.com", 28736, 7, 331},
	{ "トゥーン・ディズニー", "1213.ontvjapan.com", 28736, 7, 334},
	{ "東映チャンネル", "1010.ontvjapan.com", 28768, 7, 221},
	{ "衛星劇場", "1005.ontvjapan.com", 28768, 7, 222},
	{ "チャンネルＮＥＣＯ", "1008.ontvjapan.com", 28768, 7, 223},
	{ "洋画★シネフィル", "1009.ontvjapan.com", 28768, 7, 224},
	{ "スター・クラシック", "1003.ontvjapan.com", 28768, 7, 238},
	{ "時代劇専門チャンネル", "1133.ontvjapan.com", 28768, 7, 292},
	{ "スーパードラマ", "1006.ontvjapan.com", 28768, 7, 310},
	{ "ＡＸＮ", "1014.ontvjapan.com", 28768, 7, 311},
	{ "ナショジオチャンネル", "1204.ontvjapan.com", 28768, 7, 343},
	{ "ワンテンポータル", "110ch.epgdata.ontvjapan", 28864, 7, 110},
	{ "ゴルフチャンネル", "1028.ontvjapan.com", 28864, 7, 260},
	{ "テレ朝チャンネル", "1092.ontvjapan.com", 28864, 7, 303},
	{ "ＭＴＶ", "1019.ontvjapan.com", 28864, 7, 323},
	{ "ミュージック・エア", "1024.ontvjapan.com", 28864, 7, 324},
	{ "朝日ニュースター", "1067.ontvjapan.com", 28864, 7, 352},
	{ "ＢＢＣワールド", "1070.ontvjapan.com", 28864, 7, 353},
	{ "ＣＮＮｊ", "1069.ontvjapan.com", 28864, 7, 354},
	{ "ジャスト・アイ", "361ch.epgdata.ontvjapan", 28864, 7, 361},
	{ "Ｊスポーツ　１", "1041.ontvjapan.com", 28896, 7, 251},
	{ "Ｊスポーツ　２", "1042.ontvjapan.com", 28896, 7, 252},
	{ "ＪスポーツＰｌｕｓＨ", "1043.ontvjapan.com", 28896, 7, 253},
	{ "ＧＡＯＲＡ", "1026.ontvjapan.com", 28896, 7, 254},
	{ "ｓｋｙ・Ａスポーツ＋", "1040.ontvjapan.com", 28896, 7, 255},
	{ "宝塚プロモチャンネル", "101ch.epgdata.ontvjapan", 28928, 7, 101},
	{ "ＳＫＹ・ＳＴＡＧＥ", "1207.ontvjapan.com", 28928, 7, 290},
	{ "チャンネル銀河", "305ch.epgdata.ontvjapan", 28928, 7, 305},
	{ "ＡＴ-Ｘ", "1201.ontvjapan.com", 28928, 7, 333},
	{ "ヒストリーチャンネル", "1050.ontvjapan.com", 28928, 7, 342},
	{ "スカチャン８０３", "803ch.epgdata.ontvjapan", 28928, 7, 803},
	{ "スカチャン８０４", "804ch.epgdata.ontvjapan", 28928, 7, 804},
	{ "ムービープラスＨＤ", "1007.ontvjapan.com", 28960, 7, 240},
	{ "ゴルフネットワーク", "1027.ontvjapan.com", 28960, 7, 262},
	{ "ＬａＬａ　ＨＤ", "1074.ontvjapan.com", 28960, 7, 314},
	{ "フジテレビ７３９", "1073.ontvjapan.com", 28992, 7, 258},
	{ "フジテレビ７２１", "1072.ontvjapan.com", 28992, 7, 302},
	{ "アニマックス", "1047.ontvjapan.com", 28992, 7, 332},
	{ "ディスカバリー", "1062.ontvjapan.com", 28992, 7, 340},
	{ "アニマルプラネット", "1193.ontvjapan.com", 28992, 7, 341},
	{ "Ｃ-ＴＢＳウエルカム", "160ch.epgdata.ontvjapan", 29024, 7, 160},
	{ "ＱＶＣ", "1120.ontvjapan.com", 29024, 7, 161},
	{ "プライム３６５．ＴＶ", "185ch.epgdata.ontvjapan", 29024, 7, 185},
	{ "ファミリー劇場", "1015.ontvjapan.com", 29024, 7, 293},
	{ "ＴＢＳチャンネル", "3201.ontvjapan.com", 29024, 7, 301},
	{ "ディズニーチャンネル", "1090.ontvjapan.com", 29024, 7, 304},
	{ "MUSIC ON! TV", "1022.ontvjapan.com", 29024, 7, 325},
	{ "キッズステーション", "1045.ontvjapan.com", 29024, 7, 330},
	{ "ＴＢＳニュースバード", "1076.ontvjapan.com", 29024, 7, 351},
	{ "ＣＳ日本番組ガイド", "147ch.epgdata.ontvjapan", 29056, 7, 147},
	{ "日テレＧ＋", "1068.ontvjapan.com", 29056, 7, 257},
	{ "fashion TV", "5004.ontvjapan.com", 29056, 7, 291},
	{ "日テレプラス", "300ch.epgdata.ontvjapan", 29056, 7, 300},
	{ "エコミュージックＴＶ", "1023.ontvjapan.com", 29056, 7, 320},
	{ "Music Japan TV", "1208.ontvjapan.com", 29056, 7, 321},
	{ "日テレＮＥＷＳ２４", "2002.ontvjapan.com", 29056, 7, 350},
};

static int csStaCount = sizeof(csSta) / sizeof (STATION);
SVT_CONTROL	*svttop = NULL;
#define		SECCOUNT	4
char	title[1024];
char	subtitle[1024];
char	Category[1024];
char	ServiceName[1024];

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
		if((bsecs->pid & 0xFF) == 0x12) {
			dumpEIT(bsecs->buf, psta->svId, psta->onId, psta->tsId, eittop);
		}else if((bsecs->pid & 0xFF) == 0x26) {
			dumpEIT(bsecs->buf, psta->svId, psta->onId, psta->tsId, eittop);
		}else if((bsecs->pid & 0xFF) == 0x27) {
			dumpEIT(bsecs->buf, psta->svId, psta->onId, psta->tsId, eittop);
		}
	}
	eitcur = eittop ;
	while(eitcur != NULL){
		if(!eitcur->servid){
			eitcur = eitcur->next ;
			continue ;
		}
		if(eitcur->content_type > CAT_COUNT){
			eitcur->content_type = CAT_COUNT -1 ;
		}
		memset(title, '\0', sizeof(title));
		strcpy(title, eitcur->title);
		xmlspecialchars(title);

		memset(subtitle, '\0', sizeof(subtitle));
		strcpy(subtitle, eitcur->subtitle);
		xmlspecialchars(subtitle);

		memset(Category, '\0', sizeof(Category));
		strcpy(Category, ContentCatList[eitcur->content_type].japanese);
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
		fprintf(outfile, "    <category lang=\"ja_JP\">%s</category>\n", Category);
		fprintf(outfile, "    <category lang=\"en\">%s</category>\n", ContentCatList[eitcur->content_type].english);
		fprintf(outfile, "  </programme>\n");
#else
		fprintf(outfile, "(%x:%x:%x)%s,%s,%s,%s,%s,%s\n",
					eitcur->servid, eitcur->table_id, eitcur->event_id,
					cstarttime, cendtime,
					title, subtitle,
					Category,
					ContentCatList[eitcur->content_type].english);
#endif
#if 0
		fprintf(outfile, "(%x:%x)%04d/%02d/%02d,%02d:%02d:%02d,%02d:%02d:%02d,%s,%s,%s,%s\n",
					eitcur->table_id, eitcur->event_id,
					eitcur->yy, eitcur->mm, eitcur->dd,
					eitcur->hh, eitcur->hm, eitcur->ss,
					eitcur->ehh, eitcur->emm, eitcur->ess,
					eitcur->title, eitcur->subtitle,
					ContentCatList[eitcur->content_type].japanese,
					ContentCatList[eitcur->content_type].english);
#endif
		eitnext = eitcur->next ;
		free(eitcur->title);
		free(eitcur->subtitle);
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
