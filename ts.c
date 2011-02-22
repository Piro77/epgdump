#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ts.h"

static unsigned int CalcCrc(unsigned int crc, unsigned char *buf, int len);
static int checkcrc(SECcache *secs);

SECcache *readTS(FILE *in, SECcache secs[], int size) {
	static int rcount = 0;
	static int ridx = -1;

	TSpacket pk;

	unsigned char buf[1024];

	int boff;
	int len;
	unsigned char *payptr;

	int inchar;
	int i;

	/* syncバイトまで読み飛ばし */
	if(rcount == 0) {
		while((inchar = fgetc(in)) != EOF) {
			if((inchar & 0xFF) == 0x47) {
				//fseek(in, -1, SEEK_CUR);
				ungetc(inchar, in);
				break;
			}
		}
		if(inchar == EOF) {
			return NULL;
		}
	}

retry:

	/* 戻すべき残りがあるか? */
	if(ridx >= 0 && secs[ridx].cont) {
		/* バッファチェック */
		if((secs[ridx].cur.payload[secs[ridx].curlen] & 0xFF) == 0xFF) {
			secs[ridx].cont = 0;
			secs[ridx].seclen = 0;
			secs[ridx].setlen = 0;
			secs[ridx].curlen = 0;
		} else {
			len = secs[ridx].cur.payloadlen - secs[ridx].curlen;
			/* 全部設定済みチェック */
			if(len == 0) {
				secs[ridx].cont = 0;
				secs[ridx].seclen = 0;
				secs[ridx].setlen = 0;
				secs[ridx].curlen = 0;
			} else {
				/* ここでseclenが跨るようにTS分割されていると困るな @@
				   if(secs[ridx].pid == 0x12) {
				   int check = secs[ridx].cur.payload[secs[ridx].curlen] & 0xFF;
				   if(!(check == 0x4E ||
				   check == 0x4F ||
				   (check >= 0x50 && check <= 0x6F))) {
				   secs[ridx].curlen -= 3;
				   }
				   }
				*/

				boff = 12;
				secs[ridx].seclen = getBit(&secs[ridx].cur.payload[secs[ridx].curlen], &boff, 12) + 3; // ヘッダ

				/*
				  if(secs[ridx].seclen == 2334) {
				  printf("aa");
				  }
				*/

				/* TSデータ長-設定済みデータ長 */
				if(secs[ridx].seclen > len) {
					memcpy(secs[ridx].buf, &secs[ridx].cur.payload[secs[ridx].curlen], len);
					secs[ridx].setlen = len;
					secs[ridx].curlen = 0;
					secs[ridx].cont   = 1;
					/* 次のレコード読み込み */
				} else {
					memcpy(secs[ridx].buf,
						   &secs[ridx].cur.payload[secs[ridx].curlen], secs[ridx].seclen);
					secs[ridx].setlen  = secs[ridx].seclen;
					secs[ridx].curlen += secs[ridx].seclen;
					secs[ridx].cont = 1;

					/* CRCのチェック */
					if(checkcrc(&(secs[ridx]))) {
						return &(secs[ridx]); /* 戻る */
					}
					goto retry; /* もう一回 */
				}
			}
		}
	}

	int roffset = 0;
	while(1) {
		if(fread(buf+roffset, 188-roffset, 1, in) != 1) {
			/* 残りの処理? */
			return NULL;
		}
		roffset = 0;
		rcount++;

		if((buf[0] & 0xFF) != 0x47) {
			/* 最初はbuf中に0x47があるかチェック */
			for(i = 1; i < 188; i++) {
				if((buf[i] & 0xFF) == 0x47) {
					break;
				}
			}

			if(i < 188) {
				/* そこから再読み込みして欲しいのでseek */
				//fseek(in, (188 - i) * -1, SEEK_CUR);
				roffset = i;
				memmove(buf, buf + i, 188 - i);
				continue;
			}

			while((inchar = fgetc(in)) != EOF) {
				if((inchar & 0xFF) == 0x47) {
					//fseek(in, -1, SEEK_CUR);
					ungetc(inchar, in);
					break;
				}
			}
			if(inchar == EOF) {
				return NULL;
			}
			continue;
		}

		/*
		  if(rcount == 406502) {
		  printf("aa");
		  }
		*/


		pk.rcount = rcount;

		boff = 0;
		pk.sync = getBit(buf, &boff, 8);
		pk.transport_error_indicator = getBit(buf, &boff, 1);
		pk.payload_unit_start_indicator = getBit(buf, &boff, 1);
		pk.transport_priority = getBit(buf, &boff, 1);
		pk.pid = getBit(buf, &boff, 13);
		pk.transport_scrambling_control = getBit(buf, &boff, 2);
		pk.adaptation_field_control = getBit(buf, &boff, 2);
		pk.continuity_counter = getBit(buf, &boff, 4);

		/*
		  adaptation_field_control 2 bslbf
		  continuity_counter 4 uimsbf
		  if(adaptation_field_control = = '10' || adaptation_field_control = = '11'){
		  adaptation_field()
		  }
		  ...
		  adaptation_field() {
		  adaptation_field_length 8 uimsbf
		  if (adaptation_field_length > 0) {
		  discontinuity_indicator....

		  00 Reserved for future use by ISO/IEC
		  01 No adaptation_field, payload only
		  10 Adaptation_field only, no payload
		  11 Adaptation_field followed by payload

		*/

		pk.payloadlen = 184;

		if(pk.adaptation_field_control == 2) {
			continue;
		}

		if(pk.adaptation_field_control == 3) {
			len = getBit(buf, &boff, 8);
			payptr = buf + (boff / 8) + len;
			pk.payloadlen -= (len + 1);
		} else {
			payptr = buf + (boff / 8);
		}
		if(pk.payloadlen < 0){
			continue ;
		}

		/*
		   if the Transport Stream packet carries the first byte of a PSI section, the payload_unit_start_indicator value
		   shall be '1', indicating that the first byte of the payload of this Transport Stream packet carries the pointer_field.
		*/
		if(pk.payload_unit_start_indicator == 1) {
			/* pointer_fieldはいらない */
			payptr += 1;
			pk.payloadlen -= 1;
		}
		memset(pk.payload, 0xFF, sizeof(pk.payload));
		if(pk.payloadlen > sizeof(pk.payload))
			continue;
		memcpy(pk.payload, payptr, pk.payloadlen);


		/*
		  if(pk.rcount == 62) {
		  printf("62\n");
		  }

		  if(pk.rcount == 63) {
		  printf("63\n");
		  }
		*/

		/* 興味のあるpidか確認 */
		for(int i = 0;i < size; i++) {
			if(secs[i].pid == pk.pid) {
				secs[i].cur = pk;
				/* 途中処理中か最初か? */
				if(!secs[i].cont) {
					/* 最初 セクション長を調べる */
					boff = 12;
					secs[i].seclen = getBit(secs[i].cur.payload, &boff, 12) + 3; // ヘッダ;
					/*
					  if(secs[i].seclen == 2334) {
					  printf("aa");
					  }
					*/

					if(secs[i].seclen > secs[i].cur.payloadlen) {
						memcpy(secs[i].buf, secs[i].cur.payload, secs[i].cur.payloadlen);
						secs[i].setlen = secs[i].cur.payloadlen;
						secs[i].cont = 1;
						continue;
					}
					memcpy(secs[i].buf, secs[i].cur.payload, secs[i].seclen);
					secs[i].setlen = secs[i].seclen;
					secs[i].curlen = secs[i].seclen;
					secs[i].cont = 1;
					ridx = i;
					/* CRCのチェック */
					if(checkcrc(&(secs[ridx]))) {
						return &(secs[i]); /* 取り合えず戻る */
					}
					goto retry; /* 残り処理へ */
				}
				/* セクション長-設定済み長 */
				len = secs[i].seclen - secs[i].setlen;
				if(len > secs[i].cur.payloadlen) {
					/* 全体転送 */
					memcpy(&secs[i].buf[secs[i].setlen],
						   secs[i].cur.payload, secs[i].cur.payloadlen);
					secs[i].setlen += secs[i].cur.payloadlen;
					continue;
				}
				/* セクション長の残りを設定 */
				memcpy(&secs[i].buf[secs[i].setlen], secs[i].cur.payload, len);
				secs[i].setlen  = secs[i].seclen;
				secs[i].curlen += len;
				secs[i].cont    = 1;
				ridx = i;
				/* CRCのチェック */
				if(checkcrc(&(secs[ridx]))) {
					return &(secs[i]);
				}
				goto retry; /* 残り処理へ */
			}
		}
	}

	//return NULL;
}

/* BonTest/TsStream.cppからのパクリ */
unsigned int CalcCrc(unsigned int crc, unsigned char *buf, int len) {
	unsigned int c = crc;
	int n;

	static const unsigned int CrcTable[256] = {
		0x00000000UL, 0x04C11DB7UL, 0x09823B6EUL, 0x0D4326D9UL,	0x130476DCUL, 0x17C56B6BUL, 0x1A864DB2UL, 0x1E475005UL,	0x2608EDB8UL, 0x22C9F00FUL, 0x2F8AD6D6UL, 0x2B4BCB61UL,	0x350C9B64UL, 0x31CD86D3UL, 0x3C8EA00AUL, 0x384FBDBDUL,
		0x4C11DB70UL, 0x48D0C6C7UL, 0x4593E01EUL, 0x4152FDA9UL,	0x5F15ADACUL, 0x5BD4B01BUL, 0x569796C2UL, 0x52568B75UL,	0x6A1936C8UL, 0x6ED82B7FUL, 0x639B0DA6UL, 0x675A1011UL,	0x791D4014UL, 0x7DDC5DA3UL, 0x709F7B7AUL, 0x745E66CDUL,
		0x9823B6E0UL, 0x9CE2AB57UL, 0x91A18D8EUL, 0x95609039UL,	0x8B27C03CUL, 0x8FE6DD8BUL, 0x82A5FB52UL, 0x8664E6E5UL,	0xBE2B5B58UL, 0xBAEA46EFUL, 0xB7A96036UL, 0xB3687D81UL,	0xAD2F2D84UL, 0xA9EE3033UL, 0xA4AD16EAUL, 0xA06C0B5DUL,
		0xD4326D90UL, 0xD0F37027UL, 0xDDB056FEUL, 0xD9714B49UL,	0xC7361B4CUL, 0xC3F706FBUL, 0xCEB42022UL, 0xCA753D95UL,	0xF23A8028UL, 0xF6FB9D9FUL, 0xFBB8BB46UL, 0xFF79A6F1UL,	0xE13EF6F4UL, 0xE5FFEB43UL, 0xE8BCCD9AUL, 0xEC7DD02DUL,
		0x34867077UL, 0x30476DC0UL, 0x3D044B19UL, 0x39C556AEUL,	0x278206ABUL, 0x23431B1CUL, 0x2E003DC5UL, 0x2AC12072UL,	0x128E9DCFUL, 0x164F8078UL, 0x1B0CA6A1UL, 0x1FCDBB16UL,	0x018AEB13UL, 0x054BF6A4UL, 0x0808D07DUL, 0x0CC9CDCAUL,
		0x7897AB07UL, 0x7C56B6B0UL, 0x71159069UL, 0x75D48DDEUL,	0x6B93DDDBUL, 0x6F52C06CUL, 0x6211E6B5UL, 0x66D0FB02UL,	0x5E9F46BFUL, 0x5A5E5B08UL, 0x571D7DD1UL, 0x53DC6066UL,	0x4D9B3063UL, 0x495A2DD4UL, 0x44190B0DUL, 0x40D816BAUL,
		0xACA5C697UL, 0xA864DB20UL, 0xA527FDF9UL, 0xA1E6E04EUL,	0xBFA1B04BUL, 0xBB60ADFCUL, 0xB6238B25UL, 0xB2E29692UL,	0x8AAD2B2FUL, 0x8E6C3698UL, 0x832F1041UL, 0x87EE0DF6UL,	0x99A95DF3UL, 0x9D684044UL, 0x902B669DUL, 0x94EA7B2AUL,
		0xE0B41DE7UL, 0xE4750050UL, 0xE9362689UL, 0xEDF73B3EUL,	0xF3B06B3BUL, 0xF771768CUL, 0xFA325055UL, 0xFEF34DE2UL,	0xC6BCF05FUL, 0xC27DEDE8UL, 0xCF3ECB31UL, 0xCBFFD686UL,	0xD5B88683UL, 0xD1799B34UL, 0xDC3ABDEDUL, 0xD8FBA05AUL,
		0x690CE0EEUL, 0x6DCDFD59UL, 0x608EDB80UL, 0x644FC637UL,	0x7A089632UL, 0x7EC98B85UL, 0x738AAD5CUL, 0x774BB0EBUL,	0x4F040D56UL, 0x4BC510E1UL, 0x46863638UL, 0x42472B8FUL,	0x5C007B8AUL, 0x58C1663DUL, 0x558240E4UL, 0x51435D53UL,
		0x251D3B9EUL, 0x21DC2629UL, 0x2C9F00F0UL, 0x285E1D47UL,	0x36194D42UL, 0x32D850F5UL, 0x3F9B762CUL, 0x3B5A6B9BUL,	0x0315D626UL, 0x07D4CB91UL, 0x0A97ED48UL, 0x0E56F0FFUL,	0x1011A0FAUL, 0x14D0BD4DUL, 0x19939B94UL, 0x1D528623UL,
		0xF12F560EUL, 0xF5EE4BB9UL, 0xF8AD6D60UL, 0xFC6C70D7UL,	0xE22B20D2UL, 0xE6EA3D65UL, 0xEBA91BBCUL, 0xEF68060BUL,	0xD727BBB6UL, 0xD3E6A601UL, 0xDEA580D8UL, 0xDA649D6FUL,	0xC423CD6AUL, 0xC0E2D0DDUL, 0xCDA1F604UL, 0xC960EBB3UL,
		0xBD3E8D7EUL, 0xB9FF90C9UL, 0xB4BCB610UL, 0xB07DABA7UL,	0xAE3AFBA2UL, 0xAAFBE615UL, 0xA7B8C0CCUL, 0xA379DD7BUL,	0x9B3660C6UL, 0x9FF77D71UL, 0x92B45BA8UL, 0x9675461FUL,	0x8832161AUL, 0x8CF30BADUL, 0x81B02D74UL, 0x857130C3UL,
		0x5D8A9099UL, 0x594B8D2EUL, 0x5408ABF7UL, 0x50C9B640UL,	0x4E8EE645UL, 0x4A4FFBF2UL, 0x470CDD2BUL, 0x43CDC09CUL,	0x7B827D21UL, 0x7F436096UL, 0x7200464FUL, 0x76C15BF8UL,	0x68860BFDUL, 0x6C47164AUL, 0x61043093UL, 0x65C52D24UL,
		0x119B4BE9UL, 0x155A565EUL, 0x18197087UL, 0x1CD86D30UL,	0x029F3D35UL, 0x065E2082UL, 0x0B1D065BUL, 0x0FDC1BECUL,	0x3793A651UL, 0x3352BBE6UL, 0x3E119D3FUL, 0x3AD08088UL,	0x2497D08DUL, 0x2056CD3AUL, 0x2D15EBE3UL, 0x29D4F654UL,
		0xC5A92679UL, 0xC1683BCEUL, 0xCC2B1D17UL, 0xC8EA00A0UL,	0xD6AD50A5UL, 0xD26C4D12UL, 0xDF2F6BCBUL, 0xDBEE767CUL,	0xE3A1CBC1UL, 0xE760D676UL, 0xEA23F0AFUL, 0xEEE2ED18UL,	0xF0A5BD1DUL, 0xF464A0AAUL, 0xF9278673UL, 0xFDE69BC4UL,
		0x89B8FD09UL, 0x8D79E0BEUL, 0x803AC667UL, 0x84FBDBD0UL,	0x9ABC8BD5UL, 0x9E7D9662UL, 0x933EB0BBUL, 0x97FFAD0CUL,	0xAFB010B1UL, 0xAB710D06UL, 0xA6322BDFUL, 0xA2F33668UL,	0xBCB4666DUL, 0xB8757BDAUL, 0xB5365D03UL, 0xB1F740B4UL
	};

	for (n = 0; n < len; n++) {
		c = (c << 8) ^ CrcTable[((((c >> 24) & 0xFF) ^ buf[n]) & 0XFF)];
	}

	return c;
}


int checkcrc(SECcache *secs) {

	/* regard a section with more than MAXSECLEN data as an error. */
	if(secs->seclen > MAXSECLEN) {
		return 0;
	}

	/* セクションの終りに置かれる4バイトのCRC32は、
	   CRC計算の結果0になるように設定される。
	   値が発生した場合は、エラーなので対象外にする */
	if(CalcCrc(0xffffffffU, secs->buf, secs->seclen)) {
//		fprintf(stderr, "tblid:0x%x CRC error\n", secs->buf[0]);
		return 0;
	}
	return 1;
}
