#ifndef UTIL_H
#define UTILH 1

#define MAXSECLEN 4096

#ifdef __cplusplus
extern "C"{
#endif /* __cplusplus */

	int   getBit(unsigned char *byte, int *pbit, int gbit);
	void  getStr(char *tostr, unsigned char *byte, int *pbit, int len);
	int   parseOTHERdesc(unsigned char *data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
