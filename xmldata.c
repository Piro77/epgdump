#ifndef XMLDATA_C
#define XMLDATA_C 1

#define CONTENT_LARGE	0
#define CONTENT_MIDDLE	1
#define CONTENT_EX_CS	2
#define CONTENT_EX_BS	3
#define CONTENT_EX_GR	4
#define CONTENT_LANG_JA	0
#define CONTENT_LANG_EN	1

typedef		struct	_ContentTYPE{
	char	*japanese ;
	char	*english ;
}CONTENT_TYPE;

typedef		struct _ContentTYPEA {
	char	*lang[2];
}CONTENT_TYPEA;

#define		CAT_COUNT		16
static  CONTENT_TYPE	ContentCatList[CAT_COUNT] = {
	{ "ニュース／報道", "news" },
	{ "スポーツ", "sports" },
	{ "情報／ワイドショー", "information" },
	{ "ドラマ", "drama" },
	{ "音楽", "music" },
	{ "バラエティ", "variety" },
	{ "映画", "cinema" },
	{ "アニメ／特撮", "anime" },
	{ "ドキュメンタリー／教養", "documentary" },
	{ "劇場／公演", "stage" },
	{ "趣味／教育", "hobby" },
	{ "福祉", "welfare" },			//福祉
	{ "予備", "etc" }, //予備
	{ "予備", "etc" }, //予備
	{ "予備", "etc" }, //予備
	{ "その他", "etc" } //その他
};
/* コンテント記述子　ジャンル中分類 */
#define		MIDDLECAT_COUNT		256
static  CONTENT_TYPE	ContentMiddleCatList[MIDDLECAT_COUNT] = {
 {"定時・総合","Regular/General"},
 {"天気","Weather"},
 {"特集・ドキュメント","Special topic/documentary"},
 {"政治・国会","Politics/diet"},
 {"経済・市況","Economy/market"},
 {"海外・国際","Overseas"},
 {"解説","Commentary"},
 {"討論・会談","Discussion/meeting"},
 {"報道特番","Special news report"},
 {"ローカル・地域","Local"},
 {"交通","Traffic"},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"スポーツニュース","Sports news"},
 {"野球","Baseball"},
 {"サッカー","soccer"},
 {"ゴルフ","Golf"},
 {"その他の球技","Other ball games"},
 {"相撲・格闘技","Sumo/fighting sports"},
 {"オリンピック・国際大会","Olympic/international games"},
 {"マラソン・陸上・水泳","Marathon/field and track/swimming"},
 {"モータースポーツ","Motor sports"},
 {"マリン・ウインタースポーツ","Marine/winter sports"},
 {"競馬・公営競技","Horse race/public sports"},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"その他","Other"},
 {"芸能・ワイドショー","Show business/tabloid show"},
 {"ファッション","Fashion"},
 {"暮らし・住まい","Life/residence"},
 {"健康・医療","Health/medical care"},
 {"ショッピング・通販","Shopping/catalog shopping"},
 {"グルメ・料理","Gourmet/cooking show"},
 {"イベント","Events"},
 {"番組紹介・お知らせ","Promotion/information"},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"その他","Others"},
 {"国内ドラマ","Japanese drama"},
 {"海外ドラマ","Overseas drama"},
 {"時代劇","Historical drama"},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"その他","Others"},
 {"国内ロック・ポップス","Japanese rock/pops"},
 {"海外ロック・ポップス","Overseas rock/pops"},
 {"クラシック・オペラ","Classical/opera"},
 {"ジャズ・フュージョン","Jazz/fusion"},
 {"歌謡曲・演歌","Japanese popular song/ballad (Enka)"},
 {"ライブ・コンサート","Live show/concert"},
 {"ランキング・リクエスト","Ranking/Request show"},
 {"カラオケ・のど自慢","Karaoke/singing contest"},
 {"民謡・邦楽","Folk/Japanese music"},
 {"童謡・キッズ","Children's song"},
 {"民族音楽・ワールドミュージック","Ethnic/world music"},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"その他","Other"},
 {"クイズ","Quiz"},
 {"ゲーム","Game"},
 {"トークバラエティ","Talk show"},
 {"お笑い・コメディ","Comedy"},
 {"音楽バラエティ","Musical variety show"},
 {"旅バラエティ","Travel variety show"},
 {"料理バラエティ","Cooking variety show"},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"その他","Other"},
 {"洋画","Overseas movie"},
 {"邦画","Japanese movie"},
 {"アニメ","Animation movie"},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"その他","Other"},
 {"国内アニメ","Japanese animation"},
 {"海外アニメ","Overseas animation"},
 {"特撮","Special effects program"},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"その他","Other"},
 {"社会・時事","Social/topical news"},
 {"歴史・紀行","History/travel"},
 {"自然・動物・環境","Nature/animal/environment"},
 {"宇宙・科学・医学","Universe/science/science of medicine"},
 {"カルチャー・伝統文化","Culture/traditional culture"},
 {"文学・文芸","Literature/liberal arts"},
 {"スポーツ","Sports"},
 {"ドキュメンタリー全般","General documentary"},
 {"インタビュー・討論","Interview/discussion"},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"その他","Other"},
 {"現代劇・新劇","Modern play"},
 {"ミュージカル","Musical"},
 {"ダンス・バレエ","Dance/ballet"},
 {"落語・演芸","Comic monologue (Rakugo)/ entertainment"},
 {"歌舞伎・古典","Kabuki/classical play"},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"その他","Other"},
 {"旅・釣り・アウトドア","Travel/fishing/outdoors"},
 {"園芸・ペット・手芸","Gardening/pet/ handicraft"},
 {"音楽・美術・工芸","Music/art/ artifice"},
 {"囲碁・将棋","Japanese go/chess (Shogi)"},
 {"麻雀・パチンコ","Mah-jong/Pachinko"},
 {"車・オートバイ","Automobile/motorcycle"},
 {"コンピュータ・ＴＶゲーム","Computer/TV game"},
 {"会話・語学","Conversation/language"},
 {"幼児・小学生","Infants/primary school students"},
 {"中学生・高校生","Junior/senior high school students"},
 {"大学生・受験","College/preparatory students"},
 {"生涯教育・資格","Lifelong education/qulification"},
 {"教育問題","Educational problem"},
 {"",""},
 {"",""},
 {"その他","Other"},
 {"高齢者","Aged persons"},
 {"障害者","Handicapped person"},
 {"社会福祉","Social welfare"},
 {"ボランティア","Volunteers"},
 {"手話","Sign language"},
 {"文字（字幕）","Texts (caption)"},
 {"音声解説","Audio commentary"},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"その他","Other"}
};
#define		CSCAT_COUNT		3
static  CONTENT_TYPE	ContentCSCatList[CSCAT_COUNT] = {
 {"スポーツ","Sports"},
 {"洋画","Overseas movie"},
 {"邦画","Japanese movie"}
};
#define		CSMIDDLECAT_COUNT		48
static  CONTENT_TYPE	ContentCSMiddleCatList[CSMIDDLECAT_COUNT] = {
 {"テニス","Tennis"},
 {"バスケットボール","Basketball"},
 {"ラグビー","Rugby"},
 {"アメリカンフットボール","Football"},
 {"ボクシング","Boxing"},
 {"プロレス","Professional wrestling"},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"",""},
 {"その他","Others"},
 {"アクション","Action movies"},
 {"SF／ファンタジー","SF/fantasy movies"},
 {"コメディー","Comedy movies"},
 {"サスペンス／ミステリー","Suspense/mystery movies"},
 {"恋愛／ロマンス","Love/romance movies"},
 {"ホラー／スリラー","Horror/thriller movies"},
 {"ウエスタン","Western movies"},
 {"ドラマ／社会派ドラマ","Drama/social drama"},
 {"アニメーション","Animation movies"},
 {"ドキュメンタリー","Documentary movies"},
 {"アドベンチャー／冒険","Adventure movies"},
 {"ミュージカル／音楽映画","Musical movies"},
 {"ホームドラマ","Domestic drama"},
 {"",""},
 {"",""},
 {"その他","Others"},
 {"アクション","Action movies"},
 {"SF／ファンタジー","SF/fantasy movies"},
 {"お笑い／コメディー","Comedy"},
 {"サスペンス／ミステリー","Suspense/mystery movies"},
 {"恋愛／ロマンス","Love/romance movies"},
 {"ホラー／スリラー","Horror/thriller movies"},
 {"青春／学園／アイドル","Young/campus/idol dramas"},
 {"任侠／時代劇","Japanese gangster/costume drama"},
 {"アニメーション","Animation movies"},
 {"ドキュメンタリー","Documentary movies"},
 {"アドベンチャー／冒険","Adventure movies"},
 {"ミュージカル／音楽映画","Musical movies"},
 {"ホームドラマ","Domestic drama"},
 {"",""},
 {"",""},
 {"その他","Others"}
};


static char *getContentStr(unsigned char cat,unsigned char usernibble,int kbn,int lang) {
    CONTENT_TYPEA *large;
    CONTENT_TYPEA *middle;
    char *ret = "";

    if (lang > 2) return ret;
    if (kbn > 2) return ret;


    large = (CONTENT_TYPEA *)ContentCatList;
    middle = (CONTENT_TYPEA *)ContentMiddleCatList;
    if (cat == 0xe1) {
        large = (CONTENT_TYPEA *)ContentCSCatList;
        middle = (CONTENT_TYPEA *)ContentCSMiddleCatList;
        cat = usernibble;
        if (cat > 0x2f) return ret;
    }

    switch(kbn) {
        case CONTENT_LARGE:
            return large[cat >> 4].lang[lang];
            break;
        case CONTENT_MIDDLE:
            return middle[cat].lang[lang];
            break;
    }
    return ret;
}
/* コンテント記述子 0xe0 番組付属情報 */
static char *getAttachInfo(unsigned char cat) {
	char *ret;
	ret=NULL;
	switch(cat) {
		case 0x00: ret = "中止の可能性あり"; break;
		case 0x01: ret = "延長の可能性あり"; break;
		case 0x02: ret = "中断の可能性あり"; break;
		case 0x03: ret = "同一シリーズの別話数放送の可能性あり"; break;
		case 0x04: ret = "編成未定枠"; break;
		case 0x05: ret = "繰り上げの可能性あり"; break;
		case 0x10: ret = "中断ニュースあり"; break;
		case 0x11: ret = "当該イベントに関連する臨時サービスあり"; break;
		case 0x20: ret = "当該イベント中に3D映像あり"; break;
		default: ret="";
	}
	return ret;
}
static char *getVideoAspect(unsigned char cat) {
	char *ret;
	ret="";
    switch(cat&0x0f) {
        case 0x01:ret = "4:3";break;
        case 0x02:
        case 0x03:ret = "16:9";break;
        case 0x04:ret = ">16:9";break;
    }
    return ret;
}
static char *getVideoResolution(unsigned char cat) {
	char *ret;
	ret="";
    switch(cat&0xf0) {
        case 0x90:ret = "QFHD";break;
        case 0xb0:
        case 0xc0:
        case 0xe0:ret = "HD";break;
        case 0x00:
        case 0xa0:ret = "SD";break;
    }
    return ret;
}

/* 映像コンポーネント種別 */
static char *getVideoComponentDescStr(int cat) {
	char *ret;
	ret=NULL;
	switch (cat) {
		case 0x01: ret = "映像480i(525i)、アスペクト比4:3"; break;
		case 0x02: ret = "映像480i(525i)、アスペクト比16:9 パンベクトルあり"; break;
		case 0x03: ret = "映像480i(525i)、アスペクト比16:9 パンベクトルなし"; break;
		case 0x04: ret = "映像480i(525i)、アスペクト比 > 16:9"; break;
		case 0x91: ret = "映像2160p、アスペクト比4:3"; break;
		case 0x92: ret = "映像2160p、アスペクト比16:9 パンベクトルあり"; break;
		case 0x93: ret = "映像2160p、アスペクト比16:9 パンベクトルなし"; break;
		case 0x94: ret = "映像2160p、アスペクト比 > 16:9"; break;
		case 0xa1: ret = "映像480p(525p)、アスペクト比4:3"; break;
		case 0xa2: ret = "映像480p(525p)、アスペクト比16:9 パンベクトルあり"; break;
		case 0xa3: ret = "映像480p(525p)、アスペクト比16:9 パンベクトルなし"; break;
		case 0xa4: ret = "映像480p(525p)、アスペクト比 > 16:9"; break;
		case 0xb1: ret = "映像1080i(1125i)、アスペクト比4:3"; break;
		case 0xb2: ret = "映像1080i(1125i)、アスペクト比16:9 パンベクトルあり"; break;
		case 0xb3: ret = "映像1080i(1125i)、アスペクト比16:9 パンベクトルなし"; break;
		case 0xb4: ret = "映像1080i(1125i)、アスペクト比 > 16:9"; break;
		case 0xc1: ret = "映像720p(750p)、アスペクト比4:3"; break;
		case 0xc2: ret = "映像720p(750p)、アスペクト比16:9 パンベクトルあり"; break;
		case 0xc3: ret = "映像720p(750p)、アスペクト比16:9 パンベクトルなし"; break;
		case 0xc4: ret = "映像720p(750p)、アスペクト比 > 16:9"; break;
		case 0xd1: ret = "映像240p、アスペクト比4:3"; break;
		case 0xd2: ret = "映像240p、アスペクト比16:9 パンベクトルあり"; break;
		case 0xd3: ret = "映像240p、アスペクト比16:9 パンベクトルなし"; break;
		case 0xd4: ret = "映像240p、アスペクト比 > 16:9"; break;
		case 0xe1: ret = "映像1080p(1125p)、アスペクト比4:3"; break;
		case 0xe2: ret = "映像1080p(1125p)、アスペクト比16:9 パンベクトルあり"; break;
		case 0xe3: ret = "映像1080p(1125p)、アスペクト比16:9 パンベクトルなし"; break;
		case 0xe4: ret = "映像1080p(1125p)、アスペクト比 > 16:9"; break;
		default:
			   ret = "映像不明";
	}
	return ret;
}
/* 音声コンポーネント種別 */
static char *getAudioComponentDescStr(int cat) {
	char *ret;
	ret=NULL;
	switch (cat) {
		case 0x01: ret = "シングルモノ"; break;
		case 0x02: ret = "デュアルモノ"; break;
		case 0x03: ret = "ステレオ"; break;
		case 0x09: ret = "サラウンドステレオ"; break;
		default:
			   ret = "音声不明";
	}
	return ret;
}
/* */
static char *getTP(int freq) {
	char *ret;
	ret="";
	switch (freq) {
		case 1172748: ret = "BS1"; break;
		case 1176584: ret = "BS3"; break;
		case 1180420: ret = "BS5"; break;
		case 1184256: ret = "BS7"; break;
		case 1188092: ret = "BS9"; break;
		case 1191928: ret = "BS11"; break;
		case 1195764: ret = "BS13"; break;
		case 1199600: ret = "BS15"; break;
		case 1203436: ret = "BS17"; break;
		case 1207272: ret = "BS19"; break;
		case 1211108: ret = "BS21"; break;
		case 1214944: ret = "BS23"; break;
		case 1229100: ret = "CS2"; break;
		case 1233100: ret = "CS4"; break;
		case 1237100: ret = "CS6"; break;
		case 1241100: ret = "CS4"; break;
		case 1245100: ret = "CS10"; break;
		case 1249100: ret = "CS12"; break;
		case 1253100: ret = "CS14"; break;
		case 1257100: ret = "CS16"; break;
		case 1261100: ret = "CS18"; break;
		case 1265100: ret = "CS20"; break;
		case 1269100: ret = "CS22"; break;
		case 1273100: ret = "CS24"; break;
    }
    return ret;
}
static char *getPolarization(int polarization)
{
	char *ret;
	ret="";
	switch (polarization) {
        case 0x00: ret = "水平";break;
        case 0x01: ret = "垂直";break;
        case 0x02: ret = "左旋";break;
        case 0x03: ret = "右旋";break;
	default:break;
    }
return ret;
}
/* arib tr-b14 割り当てガイドラインより*/
static unsigned short getTSID2TP(unsigned short tsid) {
    return (tsid & 0x01f0)>>4;
}
static unsigned short getTSID2SLOT(unsigned short tsid) {
    /* for NHK BS */
    if (tsid == 16625 || tsid == 16626) {
        tsid = tsid - 1;
    }
    return tsid & 0x0007;
}
static char *getTSID2BSCS(unsigned short tsid) {
	char *ret;
	ret="";
	switch ((tsid & 0xf000)>>12) {
		case 4: ret = "BS";break;
		case 6:
		case 7: ret = "CS";break;
		default:break;
	}
	return ret;
}

static unsigned short freqToCH(unsigned short freq)
{
 return ((freq/7)-473)/6 + 13;
}

static char *getBSCSGR(SVT_CONTROL *svtcur) {
	static char ret[10];
	ret[0]=0;
    if (svtcur->original_network_id < 0x10) {
        return getTSID2BSCS(svtcur->transport_stream_id);
    }
    sprintf(ret,"GR%d",svtcur->remote_control_key_id);
    return ret;
}
#endif
