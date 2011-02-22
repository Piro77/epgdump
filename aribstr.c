#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iconv.h>

#include "aribstr.h"

#define CODE_UNKNOWN 		0	// 不明なグラフィックセット(非対応)
#define CODE_KANJI 		1	// Kanji
#define CODE_ALPHANUMERIC 	2	// Alphanumeric
#define CODE_HIRAGANA 		3	// Hiragana
#define CODE_KATAKANA 		4	// Katakana
#define CODE_MOSAIC_A 		5	// Mosaic A
#define CODE_MOSAIC_B 		6	// Mosaic B
#define CODE_MOSAIC_C 		7	// Mosaic C
#define CODE_MOSAIC_D 		8	// Mosaic D
#define CODE_PROP_ALPHANUMERIC 	9	// Proportional Alphanumeric
#define CODE_PROP_HIRAGANA 	10	// Proportional Hiragana
#define CODE_PROP_KATAKANA 	11	// Proportional Katakana
#define CODE_JIS_X0201_KATAKANA 12	// JIS X 0201 Katakana
#define CODE_JIS_KANJI_PLANE_1 	13	// JIS compatible Kanji Plane 1
#define CODE_JIS_KANJI_PLANE_2 	14	// JIS compatible Kanji Plane 2
#define CODE_ADDITIONAL_SYMBOLS	15	// Additional symbols


#define TCHAR char
#define BYTE  char
#define WORD  int
#define DWORD int
#define bool  int
#define true  1
#define false 0
#define TEXT(a) a
#define _T(a) a
#define CODE_SET int

static int m_CodeG[4];
static int *m_pLockingGL;
static int *m_pLockingGR;
static int *m_pSingleGL;

static	BYTE m_byEscSeqCount;
static	BYTE m_byEscSeqIndex;
static	bool m_bIsEscSeqDrcs;


static	const DWORD AribToStringInternal(TCHAR *lpszDst, const BYTE *pSrcData, const DWORD dwSrcLen);
static	const DWORD ProcessCharCode(TCHAR *lpszDst, const WORD wCode, const CODE_SET CodeSet);

static	const DWORD PutKanjiChar(TCHAR *lpszDst, const WORD wCode);
static	const DWORD PutAlphanumericChar(TCHAR *lpszDst, const WORD wCode);
//static	const DWORD PutAlphanumericHankakuChar(TCHAR *lpszDst, const WORD wCode);
static	const DWORD PutHiraganaChar(TCHAR *lpszDst, const WORD wCode);
static	const DWORD PutKatakanaChar(TCHAR *lpszDst, const WORD wCode);
static	const DWORD PutJisKatakanaChar(TCHAR *lpszDst, const WORD wCode);
static	const DWORD PutSymbolsChar(TCHAR *lpszDst, const WORD wCode);

static	void ProcessEscapeSeq(const BYTE byCode);

static	void LockingShiftGL(const BYTE byIndexG);
static	void LockingShiftGR(const BYTE byIndexG);
static	void SingleShiftGL(const BYTE byIndexG);

static	const bool DesignationGSET(const BYTE byIndexG, const BYTE byCode);
static	const bool DesignationDRCS(const BYTE byIndexG, const BYTE byCode);

static const bool abCharSizeTable[] =
{
	false,	// CODE_UNKNOWN					不明なグラフィックセット(非対応)
	true,	// CODE_KANJI					Kanji
	false,	// CODE_ALPHANUMERIC			Alphanumeric
	false,	// CODE_HIRAGANA				Hiragana
	false,	// CODE_KATAKANA				Katakana
	false,	// CODE_MOSAIC_A				Mosaic A
	false,	// CODE_MOSAIC_B				Mosaic B
	false,	// CODE_MOSAIC_C				Mosaic C
	false,	// CODE_MOSAIC_D				Mosaic D
	false,	// CODE_PROP_ALPHANUMERIC		Proportional Alphanumeric
	false,	// CODE_PROP_HIRAGANA			Proportional Hiragana
	false,	// CODE_PROP_KATAKANA			Proportional Katakana
	false,	// CODE_JIS_X0201_KATAKANA		JIS X 0201 Katakana
	true,	// CODE_JIS_KANJI_PLANE_1		JIS compatible Kanji Plane 1
	true,	// CODE_JIS_KANJI_PLANE_2		JIS compatible Kanji Plane 2
	true	// CODE_ADDITIONAL_SYMBOLS		Additional symbols
};

typedef enum{
	STR_SMALL = 0,	//SSZ
	STR_MEDIUM,		//MSZ
	STR_NORMAL,		//NSZ
	STR_MICRO,		//SZX 0x60
	STR_HIGH_W,		//SZX 0x41
	STR_WIDTH_W,	//SZX 0x44
	STR_W,			//SZX 0x45
	STR_SPECIAL_1,	//SZX 0x6B
	STR_SPECIAL_2,	//SZX 0x64
} STRING_SIZE;

STRING_SIZE m_emStrSize;

bool IsSmallCharMode(void)
{
	bool bRet = false;
	switch(m_emStrSize){
		case STR_SMALL:
			bRet = true;
			break;
		case STR_MEDIUM:
			bRet = true;
			break;
		case STR_NORMAL:
			bRet = false;
			break;
		case STR_MICRO:
			bRet = true;
			break;
		case STR_HIGH_W:
			bRet = false;
			break;
		case STR_WIDTH_W:
			bRet = false;
			break;
		case STR_W:
			bRet = false;
			break;
		case STR_SPECIAL_1:
			bRet = false;
			break;
		case STR_SPECIAL_2:
			bRet = false;
			break;
		default:
			break;
	}
	return bRet;
}

int AribToString(
	char *lpszDst, 
	const char *pSrcData, 
	const int dwSrcLen) {
  
	return AribToStringInternal(lpszDst, pSrcData, dwSrcLen);
}


const DWORD AribToStringInternal(TCHAR *lpszDst, 
								 const BYTE *pSrcData, const DWORD dwSrcLen)
{
	if(!pSrcData || !dwSrcLen || !lpszDst)return 0UL;
  
	DWORD dwSrcPos = 0UL;
	DWORD dwDstLen = 0UL;
	int   dwSrcData;
  
	// 状態初期設定
	m_byEscSeqCount = 0U;
	m_pSingleGL = NULL;

	m_CodeG[0] = CODE_KANJI;
	m_CodeG[1] = CODE_ALPHANUMERIC;
	m_CodeG[2] = CODE_HIRAGANA;
	m_CodeG[3] = CODE_KATAKANA;

	m_pLockingGL = &m_CodeG[0];
	m_pLockingGR = &m_CodeG[2];

	m_emStrSize = STR_NORMAL;

	while(dwSrcPos < dwSrcLen){
		dwSrcData = pSrcData[dwSrcPos] & 0xFF;

		if(!m_byEscSeqCount){
      
			// GL/GR領域
			if((dwSrcData >= 0x21U) && (dwSrcData <= 0x7EU)){
				// GL領域
				const CODE_SET CurCodeSet = (m_pSingleGL)? *m_pSingleGL : *m_pLockingGL;
				m_pSingleGL = NULL;
				
				if(abCharSizeTable[CurCodeSet]){
					// 2バイトコード
					if((dwSrcLen - dwSrcPos) < 2UL)break;
					
					dwDstLen += ProcessCharCode(&lpszDst[dwDstLen], ((WORD)pSrcData[dwSrcPos + 0] << 8) | (WORD)pSrcData[dwSrcPos + 1], CurCodeSet);
					dwSrcPos++;
				}
				else{
					// 1バイトコード
					dwDstLen += ProcessCharCode(&lpszDst[dwDstLen], (WORD)dwSrcData, CurCodeSet);
				}
			}
			else if((dwSrcData >= 0xA1U) && (dwSrcData <= 0xFEU)){
				// GR領域
				const CODE_SET CurCodeSet = *m_pLockingGR;
				
				if(abCharSizeTable[CurCodeSet]){
					// 2バイトコード
					if((dwSrcLen - dwSrcPos) < 2UL)break;
					
					dwDstLen += ProcessCharCode(&lpszDst[dwDstLen], ((WORD)(pSrcData[dwSrcPos + 0] & 0x7FU) << 8) | (WORD)(pSrcData[dwSrcPos + 1] & 0x7FU), CurCodeSet);
					dwSrcPos++;
				}
				else{
					// 1バイトコード
					dwDstLen += ProcessCharCode(&lpszDst[dwDstLen], (WORD)(dwSrcData & 0x7FU), CurCodeSet);
				}
			}
			else{
				// 制御コード
				switch(dwSrcData){
				case 0x0FU	: LockingShiftGL(0U);				break;	// LS0
				case 0x0EU	: LockingShiftGL(1U);				break;	// LS1
				case 0x19U	: SingleShiftGL(2U);				break;	// SS2
				case 0x1DU	: SingleShiftGL(3U);				break;	// SS3
				case 0x1BU	: m_byEscSeqCount = 1U;				break;	// ESC
				case 0x89U	: m_emStrSize = STR_MEDIUM;			break;	// MSZ
				case 0x8AU	: m_emStrSize = STR_NORMAL;			break;	// NSZ
				case 0x20U	:
				case 0xA0U	:
					//SP 空白
					//空白は文字サイズの影響あり
					if( IsSmallCharMode() == false ){
						strcpy(&lpszDst[dwDstLen], "　");
						dwDstLen += 3U;
					} else {
						lpszDst[dwDstLen++] = TEXT(' ');
					}
					break;
				default		: break;	// 非対応
				}
			}
		}
		else{
			// エスケープシーケンス処理
			ProcessEscapeSeq(dwSrcData);
		}
		
		dwSrcPos++;
	}

	// 終端文字
	lpszDst[dwDstLen] = TEXT('\0');

	return dwDstLen;
}

const DWORD ProcessCharCode(TCHAR *lpszDst, const WORD wCode, const CODE_SET CodeSet)
{
	switch(CodeSet){
	case CODE_KANJI	:
	case CODE_JIS_KANJI_PLANE_1 :
	case CODE_JIS_KANJI_PLANE_2 :
		// 漢字コード出力
		return PutKanjiChar(lpszDst, wCode);

	case CODE_ALPHANUMERIC :
	case CODE_PROP_ALPHANUMERIC :
		// 英数字コード出力
		if( IsSmallCharMode() == false ){
			//全角テーブルコード取得
		return PutAlphanumericChar(lpszDst, wCode);
		} else {
			//半角はそのまま出力
			lpszDst[0] = wCode;
			return 1UL;
		}

	case CODE_HIRAGANA :
	case CODE_PROP_HIRAGANA :
		// ひらがなコード出力
		return PutHiraganaChar(lpszDst, wCode);

	case CODE_PROP_KATAKANA :
	case CODE_KATAKANA :
		// カタカナコード出力
		return PutKatakanaChar(lpszDst, wCode);

	case CODE_JIS_X0201_KATAKANA :
		// JISカタカナコード出力
		return PutJisKatakanaChar(lpszDst, wCode);

	case CODE_ADDITIONAL_SYMBOLS :
		// 追加シンボルコード出力
		return PutSymbolsChar(lpszDst, wCode);

	default :
		return 0UL;
	}
}

const DWORD PutKanjiChar(TCHAR *lpszDst, const WORD wCode)
{
	char code[9];
	char xcode[5];
	iconv_t cd;

	size_t inbyte = 8;
	size_t outbyte = sizeof(xcode);

	memset(xcode, '\0', sizeof(xcode));
  
	const char *fptr;
	char *tptr;

	code[0] = 0x1BU;
	code[1] = 0x24U;
	code[2] = 0x40U;
	code[3] = wCode >> 8;
	code[4] = wCode & 0xFF;
	code[5] = 0x1BU;
	code[6] = 0x28U;
	code[7] = 0x4AU;
	code[8] = '\0';

	cd = iconv_open("UTF-8","ISO-2022-JP");

	fptr = code;
	tptr = xcode;
	iconv(cd, (char **)&fptr, &inbyte, &tptr, &outbyte);

	iconv_close(cd);

	strncpy(lpszDst, xcode, strlen(xcode));

	return strlen(lpszDst);
}

const DWORD PutAlphanumericChar(TCHAR *lpszDst, const WORD wCode)
{
	// 英数字全角文字コード変換
	static const TCHAR *acAlphanumericTable = 
		TEXT("　　　　　　　　　　　　　　　　")
		TEXT("　　　　　　　　　　　　　　　　")
		TEXT("　！”＃＄％＆’（）＊＋，－．／")
		TEXT("０１２３４５６７８９：；＜＝＞？")
		TEXT("＠ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯ")
		TEXT("ＰＱＲＳＴＵＶＷＸＹＺ［￥］＾＿")
		TEXT("　ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏ")
		TEXT("ｐｑｒｓｔｕｖｗｘｙｚ｛｜｝￣　");

#ifdef _UNICODE
	lpszDst[0] = acAlphanumericTableZenkaku[wCode];

	return 1UL;
#else
	lpszDst[0] = acAlphanumericTable[wCode * 3U + 0U];
	lpszDst[1] = acAlphanumericTable[wCode * 3U + 1U];
	lpszDst[2] = acAlphanumericTable[wCode * 3U + 2U];

	return 3UL;
#endif
}

const DWORD PutHiraganaChar(TCHAR *lpszDst, const WORD wCode)
{
	// ひらがな文字コード変換
	static const TCHAR *acHiraganaTable = 
		TEXT("　　　　　　　　　　　　　　　　")
		TEXT("　　　　　　　　　　　　　　　　")
		TEXT("　ぁあぃいぅうぇえぉおかがきぎく")
		TEXT("ぐけげこごさざしじすずせぜそぞた")
		TEXT("だちぢっつづてでとどなにぬねのは")
		TEXT("ばぱひびぴふぶぷへべぺほぼぽまみ")
		TEXT("むめもゃやゅゆょよらりるれろゎわ")
		TEXT("ゐゑをん　　　ゝゞー。「」、・　");
	
#ifdef _UNICODE
	lpszDst[0] = acHiraganaTable[wCode];

	return 1UL;
#else
	lpszDst[0] = acHiraganaTable[wCode * 3U + 0U];
	lpszDst[1] = acHiraganaTable[wCode * 3U + 1U];
	lpszDst[2] = acHiraganaTable[wCode * 3U + 2U];

	return 3UL;
#endif
}

const DWORD PutKatakanaChar(TCHAR *lpszDst, const WORD wCode)
{
	// カタカナ英数字文字コード変換
	static const TCHAR *acKatakanaTable = 
		TEXT("　　　　　　　　　　　　　　　　")
		TEXT("　　　　　　　　　　　　　　　　")
		TEXT("　ァアィイゥウェエォオカガキギク")
		TEXT("グケゲコゴサザシジスズセゼソゾタ")
		TEXT("ダチヂッツヅテデトドナニヌネノハ")
		TEXT("バパヒビピフブプヘベペホボポマミ")
		TEXT("ムメモャヤュユョヨラリルレロヮワ")
		TEXT("ヰヱヲンヴヵヶヽヾー。「」、・　");
	
#ifdef _UNICODE
	lpszDst[0] = acKatakanaTable[wCode];

	return 1UL;
#else
	lpszDst[0] = acKatakanaTable[wCode * 3U + 0U];
	lpszDst[1] = acKatakanaTable[wCode * 3U + 1U];
	lpszDst[2] = acKatakanaTable[wCode * 3U + 2U];

	return 3UL;
#endif
}

const DWORD PutJisKatakanaChar(TCHAR *lpszDst, const WORD wCode)
{
	// JISカタカナ文字コード変換
	static const TCHAR *acJisKatakanaTable = 
		TEXT("　　　　　　　　　　　　　　　　")
		TEXT("　　　　　　　　　　　　　　　　")
		TEXT("　。「」、・ヲァィゥェォャュョッ")
		TEXT("ーアイウエオカキクケコサシスセソ")
		TEXT("タチツテトナニヌネノハヒフヘホマ")
		TEXT("ミムメモヤユヨラリルレロワン゛゜")
		TEXT("　　　　　　　　　　　　　　　　")
		TEXT("　　　　　　　　　　　　　　　　");
	
#ifdef _UNICODE
	lpszDst[0] = acJisKatakanaTable[wCode];

	return 1UL;
#else
	lpszDst[0] = acJisKatakanaTable[wCode * 3U + 0U];
	lpszDst[1] = acJisKatakanaTable[wCode * 3U + 1U];
	lpszDst[2] = acJisKatakanaTable[wCode * 3U + 2U];

	return 3UL;
#endif
}

const DWORD PutSymbolsChar(TCHAR *lpszDst, const WORD wCode)
{
	// 追加シンボル文字コード変換(とりあえず必要そうなものだけ)
	static const TCHAR *aszSymbolsTable1[] =
		{
			_T("【HV】"),	_T("【SD】"),	_T("【Ｐ】"),	_T("【Ｗ】"),	_T("【MV】"),	_T("【手】"),	_T("【字】"),	_T("【双】"),		// 0x7A50 - 0x7A57	90/48 - 90/55
			_T("【デ】"),	_T("【Ｓ】"),	_T("【二】"),	_T("【多】"),	_T("【解】"),	_T("【SS】"),	_T("【Ｂ】"),	_T("【Ｎ】"),		// 0x7A58 - 0x7A5F	90/56 - 90/63
			_T("■"),		_T("●"),		_T("【天】"),	_T("【交】"),	_T("【映】"),	_T("【無】"),	_T("【料】"),	_T("【年齢制限】"),	// 0x7A60 - 0x7A67	90/64 - 90/71
			_T("【前】"),	_T("【後】"),	_T("【再】"),	_T("【新】"),	_T("【初】"),	_T("【終】"),	_T("【生】"),	_T("【販】"),		// 0x7A68 - 0x7A6F	90/72 - 90/79
			_T("【声】"),	_T("【吹】"),	_T("【PPV】"),	_T("（秘）"),	_T("ほか")															// 0x7A70 - 0x7A74	90/80 - 90/84
		};

	static const TCHAR *aszSymbolsTable2[] =
		{
			_T("→"),		_T("←"),		_T("↑"),		_T("↓"),		_T("●"),		_T("○"),		_T("年"),		_T("月"),			// 0x7C21 - 0x7C28	92/01 - 92/08
			_T("日"),		_T("円"),		_T("㎡"),		_T("㎥"),		_T("㎝"),		_T("㎠"),		_T("㎤"),		_T("０."),			// 0x7C29 - 0x7C30	92/09 - 92/16
			_T("１."),		_T("２."),		_T("３."),		_T("４."),		_T("５."),		_T("６."),		_T("７."),		_T("８."),			// 0x7C31 - 0x7C38	92/17 - 92/24
			_T("９."),		_T("氏"),		_T("副"),		_T("元"),		_T("故"),		_T("前"),		_T("[新]"),		_T("０,"),			// 0x7C39 - 0x7C40	92/25 - 92/32
			_T("１,"),		_T("２,"),		_T("３,"),		_T("４,"),		_T("５,"),		_T("６,"),		_T("７,"),		_T("８,"),			// 0x7C41 - 0x7C48	92/33 - 92/40
			_T("９,"),		_T("(社)"),		_T("(財)"),		_T("(有)"),		_T("(株)"),		_T("(代)"),		_T("(問)"),		_T("▶"),			// 0x7C49 - 0x7C50	92/41 - 92/48
			_T("◀"),		_T("〖"),		_T("〗"),		_T("⟐"),		_T("^2"),		_T("^3"),		_T("(CD)"),		_T("(vn)"),			// 0x7C51 - 0x7C58	92/49 - 92/56
			_T("(ob)"),		_T("(cb)"),		_T("(ce"),		_T("mb)"),		_T("(hp)"),		_T("(br)"),		_T("(p)"),		_T("(s)"),			// 0x7C59 - 0x7C60	92/57 - 92/64
			_T("(ms)"),		_T("(t)"),		_T("(bs)"),		_T("(b)"),		_T("(tb)"),		_T("(tp)"),		_T("(ds)"),		_T("(ag)"),			// 0x7C61 - 0x7C68	92/65 - 92/72
			_T("(eg)"),		_T("(vo)"),		_T("(fl)"),		_T("(ke"),		_T("y)"),		_T("(sa"),		_T("x)"),		_T("(sy"),			// 0x7C69 - 0x7C70	92/73 - 92/80
			_T("n)"),		_T("(or"),		_T("g)"),		_T("(pe"),		_T("r)"),		_T("(R)"),		_T("(C)"),		_T("(箏)"),			// 0x7C71 - 0x7C78	92/81 - 92/88
			_T("DJ"),		_T("[演]"),		_T("Fax")																							// 0x7C79 - 0x7C7B	92/89 - 92/91
		};

	static const TCHAR *aszSymbolsTable3[] =
		{
			_T("㈪"),		_T("㈫"),		_T("㈬"),		_T("㈭"),		_T("㈮"),		_T("㈯"),		_T("㈰"),		_T("㈷"),			// 0x7D21 - 0x7D28	93/01 - 93/08
			_T("㍾"),		_T("㍽"),		_T("㍼"),		_T("㍻"),		_T("№"),		_T("℡"),		_T("〶"),		_T("○"),			// 0x7D29 - 0x7D30	93/09 - 93/16
			_T("〔本〕"),	_T("〔三〕"),	_T("〔二〕"),	_T("〔安〕"),	_T("〔点〕"),	_T("〔打〕"),	_T("〔盗〕"),	_T("〔勝〕"),		// 0x7D31 - 0x7D38	93/17 - 93/24
			_T("〔敗〕"),	_T("〔Ｓ〕"),	_T("［投］"),	_T("［捕］"),	_T("［一］"),	_T("［二］"),	_T("［三］"),	_T("［遊］"),		// 0x7D39 - 0x7D40	93/25 - 93/32
			_T("［左］"),	_T("［中］"),	_T("［右］"),	_T("［指］"),	_T("［走］"),	_T("［打］"),	_T("㍑"),		_T("㎏"),			// 0x7D41 - 0x7D48	93/33 - 93/40
			_T("㎐"),		_T("ha"),		_T("㎞"),		_T("㎢"),		_T("㍱"),		_T("・"),		_T("・"),		_T("1/2"),			// 0x7D49 - 0x7D50	93/41 - 93/48
			_T("0/3"),		_T("1/3"),		_T("2/3"),		_T("1/4"),		_T("3/4"),		_T("1/5"),		_T("2/5"),		_T("3/5"),			// 0x7D51 - 0x7D58	93/49 - 93/56
			_T("4/5"),		_T("1/6"),		_T("5/6"),		_T("1/7"),		_T("1/8"),		_T("1/9"),		_T("1/10"),		_T("☀"),			// 0x7D59 - 0x7D60	93/57 - 93/64
			_T("☁"),		_T("☂"),		_T("☃"),		_T("☖"),		_T("☗"),		_T("▽"),		_T("▼"),		_T("♦"),			// 0x7D61 - 0x7D68	93/65 - 93/72
			_T("♥"),		_T("♣"),		_T("♠"),		_T("⌺"),		_T("⦿"),		_T("‼"),		_T("⁉"),		_T("(曇/晴)"),		// 0x7D69 - 0x7D70	93/73 - 93/80
			_T("☔"),		_T("(雨)"),		_T("(雪)"),		_T("(大雪)"),	_T("⚡"),		_T("(雷雨)"),	_T("　"),		_T("・"),			// 0x7D71 - 0x7D78	93/81 - 93/88
			_T("・"),		_T("♬"),		_T("☎")																							// 0x7D79 - 0x7D7B	93/89 - 93/91
		};

	static const TCHAR *aszSymbolsTable4[] =
		{
			_T("Ⅰ"),		_T("Ⅱ"),		_T("Ⅲ"),		_T("Ⅳ"),		_T("Ⅴ"),		_T("Ⅵ"),		_T("Ⅶ"),		_T("Ⅷ"),			// 0x7E21 - 0x7E28	94/01 - 94/08
			_T("Ⅸ"),		_T("Ⅹ"),		_T("Ⅺ"),		_T("Ⅻ"),		_T("⑰"),		_T("⑱"),		_T("⑲"),		_T("⑳"),			// 0x7E29 - 0x7E30	94/09 - 94/16
			_T("⑴"),		_T("⑵"),		_T("⑶"),		_T("⑷"),		_T("⑸"),		_T("⑹"),		_T("⑺"),		_T("⑻"),			// 0x7E31 - 0x7E38	94/17 - 94/24
			_T("⑼"),		_T("⑽"),		_T("⑾"),		_T("⑿"),		_T("㉑"),		_T("㉒"),		_T("㉓"),		_T("㉔"),			// 0x7E39 - 0x7E40	94/25 - 94/32
			_T("(A)"),		_T("(B)"),		_T("(C)"),		_T("(D)"),		_T("(E)"),		_T("(F)"),		_T("(G)"),		_T("(H)"),			// 0x7E41 - 0x7E48	94/33 - 94/40
			_T("(I)"),		_T("(J)"),		_T("(K)"),		_T("(L)"),		_T("(M)"),		_T("(N)"),		_T("(O)"),		_T("(P)"),			// 0x7E49 - 0x7E50	94/41 - 94/48
			_T("(Q)"),		_T("(R)"),		_T("(S)"),		_T("(T)"),		_T("(U)"),		_T("(V)"),		_T("(W)"),		_T("(X)"),			// 0x7E51 - 0x7E58	94/49 - 94/56
			_T("(Y)"),		_T("(Z)"),		_T("㉕"),		_T("㉖"),		_T("㉗"),		_T("㉘"),		_T("㉙"),		_T("㉚"),			// 0x7E59 - 0x7E60	94/57 - 94/64
			_T("①"),		_T("②"),		_T("③"),		_T("④"),		_T("⑤"),		_T("⑥"),		_T("⑦"),		_T("⑧"),			// 0x7E61 - 0x7E68	94/65 - 94/72
			_T("⑨"),		_T("⑩"),		_T("⑪"),		_T("⑫"),		_T("⑬"),		_T("⑭"),		_T("⑮"),		_T("⑯"),			// 0x7E69 - 0x7E70	94/73 - 94/80
			_T("❶"),		_T("❷"),		_T("❸"),		_T("❹"),		_T("❺"),		_T("❻"),		_T("❼"),		_T("❽"),			// 0x7E71 - 0x7E78	94/81 - 94/88
			_T("❾"),		_T("❿"),		_T("⓫"),		_T("⓬"),		_T("㉛")															// 0x7E79 - 0x7E7D	94/89 - 94/93
		};

	static const TCHAR *aszSymbolsTable5[] =
		{
			_T("㐂"),		_T("亭"),		_T("份"),		_T("仿"),		_T("侚"),		_T("俉"),		_T("傜"),		_T("儞"),			// 0x7521 - 0x7528	85/01 - 85/08
			_T("冼"),		_T("㔟"),		_T("匇"),		_T("卡"),		_T("卬"),		_T("詹"),		_T("吉"),		_T("呍"),			// 0x7529 - 0x7530	85/09 - 85/16
			_T("咖"),		_T("咜"),		_T("咩"),		_T("唎"),		_T("啊"),		_T("噲"),		_T("囤"),		_T("圳"),			// 0x7531 - 0x7538	85/17 - 85/24
			_T("圴"),		_T("塚"),		_T("墀"),		_T("姤"),		_T("娣"),		_T("婕"),		_T("寬"),		_T("﨑"),			// 0x7539 - 0x7540	85/25 - 85/32
			_T("㟢"),		_T("庬"),		_T("弴"),		_T("彅"),		_T("德"),		_T("怗"),		_T("恵"),		_T("愰"),			// 0x7541 - 0x7548	85/33 - 85/40
			_T("昤"),		_T("曈"),		_T("曙"),		_T("曺"),		_T("曻"),		_T("桒"),		_T("・"),		_T("椑"),			// 0x7549 - 0x7550	85/41 - 85/48
			_T("椻"),		_T("橅"),		_T("檑"),		_T("櫛"),		_T("・"),		_T("・"),		_T("・"),		_T("毱"),			// 0x7551 - 0x7558	85/49 - 85/56
			_T("泠"),		_T("洮"),		_T("海"),		_T("涿"),		_T("淊"),		_T("淸"),		_T("渚"),		_T("潞"),			// 0x7559 - 0x7560	85/57 - 85/64
			_T("濹"),		_T("灤"),		_T("・"),		_T("・"),		_T("煇"),		_T("燁"),		_T("爀"),		_T("玟"),			// 0x7561 - 0x7568	85/65 - 85/72
			_T("・"),		_T("珉"),		_T("珖"),		_T("琛"),		_T("琡"),		_T("琢"),		_T("琦"),		_T("琪"),			// 0x7569 - 0x7570	85/73 - 85/80
			_T("琬"),		_T("琹"),		_T("瑋"),		_T("㻚"),		_T("畵"),		_T("疁"),		_T("睲"),		_T("䂓"),			// 0x7571 - 0x7578	85/81 - 85/88
			_T("磈"),		_T("磠"),		_T("祇"),		_T("禮"),		_T("・"),		_T("・")											// 0x7579 - 0x757E	85/89 - 85/94
		};

	static const TCHAR *aszSymbolsTable6[] =
		{
			_T("・"),		_T("秚"),		_T("稞"),		_T("筿"),		_T("簱"),		_T("䉤"),		_T("綋"),		_T("羡"),			// 0x7621 - 0x7628	86/01 - 86/08
			_T("脘"),		_T("脺"),		_T("・"),		_T("芮"),		_T("葛"),		_T("蓜"),		_T("蓬"),		_T("蕙"),			// 0x7629 - 0x7630	86/09 - 86/16
			_T("藎"),		_T("蝕"),		_T("蟬"),		_T("蠋"),		_T("裵"),		_T("角"),		_T("諶"),		_T("跎"),			// 0x7631 - 0x7638	86/17 - 86/24
			_T("辻"),		_T("迶"),		_T("郝"),		_T("鄧"),		_T("鄭"),		_T("醲"),		_T("鈳"),		_T("銈"),			// 0x7639 - 0x7640	86/25 - 86/32
			_T("錡"),		_T("鍈"),		_T("閒"),		_T("雞"),		_T("餃"),		_T("饀"),		_T("髙"),		_T("鯖"),			// 0x7641 - 0x7648	86/33 - 86/40
			_T("鷗"),		_T("麴"),		_T("麵")																							// 0x7649 - 0x764B	86/41 - 86/43
		};

	// シンボルを変換する
	if((wCode >= 0x7A50U) && (wCode <= 0x7A74U)){
		strcpy(lpszDst, aszSymbolsTable1[wCode - 0x7A50U]);
	}
	else if((wCode >= 0x7C21U) && (wCode <= 0x7C7BU)){
		strcpy(lpszDst, aszSymbolsTable2[wCode - 0x7C21U]);
	}
	else if((wCode >= 0x7D21U) && (wCode <= 0x7D7BU)){
		strcpy(lpszDst, aszSymbolsTable3[wCode - 0x7D21U]);
	}
	else if((wCode >= 0x7E21U) && (wCode <= 0x7E7DU)){
		strcpy(lpszDst, aszSymbolsTable4[wCode - 0x7E21U]);
	}
	else if((wCode >= 0x7521U) && (wCode <= 0x757EU)){
		strcpy(lpszDst, aszSymbolsTable5[wCode - 0x7521U]);
	}
	else if((wCode >= 0x7621U) && (wCode <= 0x764BU)){
		strcpy(lpszDst, aszSymbolsTable6[wCode - 0x7621U]);
	}
	else{
		strcpy(lpszDst, TEXT("・"));
	}

	return strlen(lpszDst);
}

void ProcessEscapeSeq(const BYTE byCode)
{
	// エスケープシーケンス処理
	switch(m_byEscSeqCount){
		// 1バイト目
	case 1U	:
		switch(byCode){
			// Invocation of code elements
		case 0x6EU	: LockingShiftGL(2U);	m_byEscSeqCount = 0U;	return;		// LS2
		case 0x6FU	: LockingShiftGL(3U);	m_byEscSeqCount = 0U;	return;		// LS3
		case 0x7EU	: LockingShiftGR(1U);	m_byEscSeqCount = 0U;	return;		// LS1R
		case 0x7DU	: LockingShiftGR(2U);	m_byEscSeqCount = 0U;	return;		// LS2R
		case 0x7CU	: LockingShiftGR(3U);	m_byEscSeqCount = 0U;	return;		// LS3R

			// Designation of graphic sets
		case 0x24U	:	
		case 0x28U	: m_byEscSeqIndex = 0U;		break;
		case 0x29U	: m_byEscSeqIndex = 1U;		break;
		case 0x2AU	: m_byEscSeqIndex = 2U;		break;
		case 0x2BU	: m_byEscSeqIndex = 3U;		break;
		default		: m_byEscSeqCount = 0U;		return;		// エラー
		}
		break;

		// 2バイト目
	case 2U	:
		if(DesignationGSET(m_byEscSeqIndex, byCode)){
			m_byEscSeqCount = 0U;
			return;
		}
			
		switch(byCode){
		case 0x20	: m_bIsEscSeqDrcs = true;	break;
		case 0x28	: m_bIsEscSeqDrcs = true;	m_byEscSeqIndex = 0U;	break;
		case 0x29	: m_bIsEscSeqDrcs = false;	m_byEscSeqIndex = 1U;	break;
		case 0x2A	: m_bIsEscSeqDrcs = false;	m_byEscSeqIndex = 2U;	break;
		case 0x2B	: m_bIsEscSeqDrcs = false;	m_byEscSeqIndex = 3U;	break;
		default		: m_byEscSeqCount = 0U;		return;		// エラー
		}
		break;

		// 3バイト目
	case 3U	:
		if(!m_bIsEscSeqDrcs){
			if(DesignationGSET(m_byEscSeqIndex, byCode)){
				m_byEscSeqCount = 0U;
				return;
			}
		}
		else{
			if(DesignationDRCS(m_byEscSeqIndex, byCode)){
				m_byEscSeqCount = 0U;
				return;
			}
		}

		if(byCode == 0x20U){
			m_bIsEscSeqDrcs = true;
		}
		else{
			// エラー
			m_byEscSeqCount = 0U;
			return;
		}
		break;

		// 4バイト目
	case 4U	:
		DesignationDRCS(m_byEscSeqIndex, byCode);
		m_byEscSeqCount = 0U;
		return;
	}

	m_byEscSeqCount++;
}

void LockingShiftGL(const BYTE byIndexG)
{
	// LSx
	m_pLockingGL = &m_CodeG[(int)byIndexG];
}

void LockingShiftGR(const BYTE byIndexG)
{
	// LSxR
	m_pLockingGR = &m_CodeG[(int)byIndexG];
}

void SingleShiftGL(const BYTE byIndexG)
{
	// SSx
	m_pSingleGL  = &m_CodeG[(int)byIndexG];
}

const bool DesignationGSET(const BYTE byIndexG_arg, const BYTE byCode)
{
	int byIndexG = (int)byIndexG_arg;

	// Gのグラフィックセットを割り当てる
	switch(byCode){
	case 0x42U	: m_CodeG[byIndexG] = CODE_KANJI;				return true;	// Kanji
	case 0x4AU	: m_CodeG[byIndexG] = CODE_ALPHANUMERIC;		return true;	// Alphanumeric
	case 0x30U	: m_CodeG[byIndexG] = CODE_HIRAGANA;			return true;	// Hiragana
	case 0x31U	: m_CodeG[byIndexG] = CODE_KATAKANA;			return true;	// Katakana
	case 0x32U	: m_CodeG[byIndexG] = CODE_MOSAIC_A;			return true;	// Mosaic A
	case 0x33U	: m_CodeG[byIndexG] = CODE_MOSAIC_B;			return true;	// Mosaic B
	case 0x34U	: m_CodeG[byIndexG] = CODE_MOSAIC_C;			return true;	// Mosaic C
	case 0x35U	: m_CodeG[byIndexG] = CODE_MOSAIC_D;			return true;	// Mosaic D
	case 0x36U	: m_CodeG[byIndexG] = CODE_PROP_ALPHANUMERIC;	return true;	// Proportional Alphanumeric
	case 0x37U	: m_CodeG[byIndexG] = CODE_PROP_HIRAGANA;		return true;	// Proportional Hiragana
	case 0x38U	: m_CodeG[byIndexG] = CODE_PROP_KATAKANA;		return true;	// Proportional Katakana
	case 0x49U	: m_CodeG[byIndexG] = CODE_JIS_X0201_KATAKANA;	return true;	// JIS X 0201 Katakana
	case 0x39U	: m_CodeG[byIndexG] = CODE_JIS_KANJI_PLANE_1;	return true;	// JIS compatible Kanji Plane 1
	case 0x3AU	: m_CodeG[byIndexG] = CODE_JIS_KANJI_PLANE_2;	return true;	// JIS compatible Kanji Plane 2
	case 0x3BU	: m_CodeG[byIndexG] = CODE_ADDITIONAL_SYMBOLS;	return true;	// Additional symbols
	default		: return false;		// 不明なグラフィックセット
	}
}

const bool DesignationDRCS(const BYTE byIndexG_arg, const BYTE byCode)
{
	int byIndexG = (int)byIndexG_arg;

	// DRCSのグラフィックセットを割り当てる
	switch(byCode){
	case 0x40U	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-0
	case 0x41U	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-1
	case 0x42U	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-2
	case 0x43U	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-3
	case 0x44U	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-4
	case 0x45U	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-5
	case 0x46U	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-6
	case 0x47U	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-7
	case 0x48U	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-8
	case 0x49U	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-9
	case 0x4AU	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-10
	case 0x4BU	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-11
	case 0x4CU	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-12
	case 0x4DU	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-13
	case 0x4EU	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-14
	case 0x4FU	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// DRCS-15
	case 0x70U	: m_CodeG[byIndexG] = CODE_UNKNOWN;				return true;	// Macro
	default		: return false;		// 不明なグラフィックセット
}
}
