#ifndef XMLDATA_C
#define XMLDATA_C 1

typedef		struct	_ContentTYPE{
	char	*japanese ;
	char	*english ;
}CONTENT_TYPE;

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
static char *getContentCat(int cat) {
	char *ret;
	ret=NULL;
	switch(cat) {
		case 0x00: ret = "定時・総合"; break;
		case 0x01: ret = "天気"; break;
		case 0x02: ret = "特集・ドキュメント"; break;
		case 0x03: ret = "政治・国会"; break;
		case 0x04: ret = "経済・市況"; break;
		case 0x05: ret = "海外・国際"; break;
		case 0x06: ret = "解説"; break;
		case 0x07: ret = "討論・会談"; break;
		case 0x08: ret = "報道特番"; break;
		case 0x09: ret = "ローカル・地域"; break;
		case 0x0a: ret = "交通"; break;
		case 0x10: ret = "スポーツニュース"; break;
		case 0x11: ret = "野球"; break;
		case 0x12: ret = "サッカー"; break;
		case 0x13: ret = "ゴルフ"; break;
		case 0x14: ret = "その他の球技"; break;
		case 0x15: ret = "相撲・格闘技"; break;
		case 0x16: ret = "オリンピック・国際大会"; break;
		case 0x17: ret = "マラソン・陸上・水泳"; break;
		case 0x18: ret = "モータースポーツ"; break;
		case 0x19: ret = "マリン・ウインタースポーツ"; break;
		case 0x1a: ret = "競馬・公営競技"; break;
		case 0x1f: ret = "その他"; break;
		case 0x20: ret = "芸能・ワイドショー"; break;
		case 0x21: ret = "ファッション"; break;
		case 0x22: ret = "暮らし・住まい"; break;
		case 0x23: ret = "健康・医療"; break;
		case 0x24: ret = "ショッピング・通販"; break;
		case 0x25: ret = "グルメ・料理"; break;
		case 0x26: ret = "イベント"; break;
		case 0x27: ret = "番組紹介・お知らせ"; break;
		case 0x2f: ret = "その他"; break;
		case 0x30: ret = "国内ドラマ"; break;
		case 0x31: ret = "海外ドラマ"; break;
		case 0x32: ret = "時代劇"; break;
		case 0x3f: ret = "その他"; break;
		case 0x40: ret = "国内ロック・ポップス"; break;
		case 0x41: ret = "海外ロック・ポップス"; break;
		case 0x42: ret = "クラシック・オペラ"; break;
		case 0x43: ret = "ジャズ・フュージョン"; break;
		case 0x44: ret = "歌謡曲・演歌"; break;
		case 0x45: ret = "ライブ・コンサート"; break;
		case 0x46: ret = "ランキング・リクエスト"; break;
		case 0x47: ret = "カラオケ・のど自慢"; break;
		case 0x48: ret = "民謡・邦楽"; break;
		case 0x49: ret = "童謡・キッズ"; break;
		case 0x4a: ret = "民族音楽・ワールドミュージック"; break;
		case 0x50: ret = "クイズ"; break;
		case 0x51: ret = "ゲーム"; break;
		case 0x52: ret = "トークバラエティ"; break;
		case 0x53: ret = "お笑い・コメディ"; break;
		case 0x54: ret = "音楽バラエティ"; break;
		case 0x55: ret = "旅バラエティ"; break;
		case 0x56: ret = "料理バラエティ"; break;
		case 0x60: ret = "洋画"; break;
		case 0x61: ret = "邦画"; break;
		case 0x62: ret = "アニメ"; break;
		case 0x70: ret = "国内アニメ"; break;
		case 0x71: ret = "海外アニメ"; break;
		case 0x72: ret = "特撮"; break;
		case 0x80: ret = "社会・時事"; break;
		case 0x81: ret = "歴史・紀行"; break;
		case 0x82: ret = "自然・動物・環境"; break;
		case 0x83: ret = "宇宙・科学・医学"; break;
		case 0x84: ret = "カルチャー・伝統文化"; break;
		case 0x85: ret = "文学・文芸"; break;
		case 0x86: ret = "スポーツ"; break;
		case 0x87: ret = "ドキュメンタリー全般"; break;
		case 0x88: ret = "インタビュー・討論"; break;
		case 0x90: ret = "現代劇・新劇"; break;
		case 0x91: ret = "ミュージカル"; break;
		case 0x92: ret = "ダンス・バレエ"; break;
		case 0x93: ret = "落語・演芸"; break;
		case 0x94: ret = "歌舞伎・古典"; break;
		case 0xa0: ret = "旅・釣り・アウトドア"; break;
		case 0xa1: ret = "園芸・ペット・手芸"; break;
		case 0xa2: ret = "音楽・美術・工芸"; break;
		case 0xa3: ret = "囲碁・将棋"; break;
		case 0xa4: ret = "麻雀・パチンコ"; break;
		case 0xa5: ret = "車・オートバイ"; break;
		case 0xa6: ret = "コンピュータ・ＴＶゲーム"; break;
		case 0xa7: ret = "会話・語学"; break;
		case 0xa8: ret = "幼児・小学生"; break;
		case 0xa9: ret = "中学生・高校生"; break;
		case 0xaa: ret = "大学生・受験"; break;
		case 0xab: ret = "生涯教育・資格"; break;
		case 0xac: ret = "教育問題"; break;
		case 0xb0: ret = "高齢者"; break;
		case 0xb1: ret = "障害者"; break;
		case 0xb2: ret = "社会福祉"; break;
		case 0xb3: ret = "ボランティア"; break;
		case 0xb4: ret = "手話"; break;
		case 0xb5: ret = "文字（字幕）"; break;
		case 0xb6: ret = "音声解説"; break;
		default: ret="その他";
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

#endif
