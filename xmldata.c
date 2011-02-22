#ifndef XMLDATA_C
#define XMLDATA_C 1

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
	{ "フジテレビＮＥＸＴ", "306ch.epgdata.ontvjapan", 24608, 6, 306},
	{ "ショップチャンネル", "1059.ontvjapan.com", 24704, 6, 55},
	{ "ザ・シネマ", "1217.ontvjapan.com", 24736, 6, 228},
	{ "スカチャンＨＤ８００", "800ch.epgdata.ontvjapan", 24736, 6, 800},
	{ "スカチャン８０１", "801ch.epgdata.ontvjapan", 24736, 6, 801},
	{ "スカチャン８０２", "802ch.epgdata.ontvjapan", 24736, 6, 802},
	{ "ｅ２プロモ", "100ch.epgdata.ontvjapan", 28736, 7, 100},
	{ "ホームドラマチャンネル", "294ch.epgdata.ontvjapan", 28736, 7, 294},
	{ "Ｊスポーツ　ＥＳＰＮ", "1025.ontvjapan.com", 28736, 7, 256},
	{ "ＦＯＸ", "1016.ontvjapan.com", 28736, 7, 312},
	{ "スペースシャワーＴＶ", "1018.ontvjapan.com", 28736, 7, 322},
	{ "カートゥーン　ネット", "1046.ontvjapan.com", 28736, 7, 331},
	{ "ディズニーＸＤ", "1213.ontvjapan.com", 28736, 7, 334},
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
	{ "フジテレビＯＮＥ", "1073.ontvjapan.com", 28992, 7, 307},
	{ "フジテレビＴＷＯ", "1072.ontvjapan.com", 28992, 7, 308},
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
	{ "キッズステーション", "1045.ontvjapan.com", 29024, 7, 335},
	{ "ＴＢＳニュースバード", "1076.ontvjapan.com", 29024, 7, 351},
	{ "ＣＳ日本番組ガイド", "147ch.epgdata.ontvjapan", 29056, 7, 147},
	{ "日テレＧ＋", "1068.ontvjapan.com", 29056, 7, 257},
	{ "fashion TV", "5004.ontvjapan.com", 29056, 7, 291},
	{ "日テレプラス", "300ch.epgdata.ontvjapan", 29056, 7, 300},
	{ "Music Japan TV", "1208.ontvjapan.com", 29056, 7, 321},
	{ "日テレＮＥＷＳ２４", "2002.ontvjapan.com", 29056, 7, 350},
};

static int csStaCount = sizeof(csSta) / sizeof (STATION);


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
	{ "福祉", "etc" },			//福祉
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
		case 0x0f: ret = "その他"; break;
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
		case 0x4f: ret = "その他"; break;
		case 0x50: ret = "クイズ"; break;
		case 0x51: ret = "ゲーム"; break;
		case 0x52: ret = "トークバラエティ"; break;
		case 0x53: ret = "お笑い・コメディ"; break;
		case 0x54: ret = "音楽バラエティ"; break;
		case 0x55: ret = "旅バラエティ"; break;
		case 0x56: ret = "料理バラエティ"; break;
		case 0x5f: ret = "その他"; break;
		case 0x60: ret = "洋画"; break;
		case 0x61: ret = "邦画"; break;
		case 0x62: ret = "アニメ"; break;
		case 0x6f: ret = "その他"; break;
		case 0x70: ret = "国内アニメ"; break;
		case 0x71: ret = "海外アニメ"; break;
		case 0x72: ret = "特撮"; break;
		case 0x7f: ret = "その他"; break;
		case 0x80: ret = "社会・時事"; break;
		case 0x81: ret = "歴史・紀行"; break;
		case 0x82: ret = "自然・動物・環境"; break;
		case 0x83: ret = "宇宙・科学・医学"; break;
		case 0x84: ret = "カルチャー・伝統文化"; break;
		case 0x85: ret = "文学・文芸"; break;
		case 0x86: ret = "スポーツ"; break;
		case 0x87: ret = "ドキュメンタリー全般"; break;
		case 0x88: ret = "インタビュー・討論"; break;
		case 0x8f: ret = "その他"; break;
		case 0x90: ret = "現代劇・新劇"; break;
		case 0x91: ret = "ミュージカル"; break;
		case 0x92: ret = "ダンス・バレエ"; break;
		case 0x93: ret = "落語・演芸"; break;
		case 0x94: ret = "歌舞伎・古典"; break;
		case 0x9f: ret = "その他"; break;
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
		case 0xaf: ret = "その他"; break;
		case 0xb0: ret = "高齢者"; break;
		case 0xb1: ret = "障害者"; break;
		case 0xb2: ret = "社会福祉"; break;
		case 0xb3: ret = "ボランティア"; break;
		case 0xb4: ret = "手話"; break;
		case 0xb5: ret = "文字（字幕）"; break;
		case 0xb6: ret = "音声解説"; break;
		case 0xbf: ret = "その他"; break;
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
	}
	return ret;
}
#endif
