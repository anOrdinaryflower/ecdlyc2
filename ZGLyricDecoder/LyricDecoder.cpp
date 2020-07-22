#include "stdafx.h"
#include "LyricDecoder.h"
#include "zlib/zconf.h"
#include "zlib/zlib.h"
#include "stdio.h"
#include <sys/stat.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

const char key[] = {(char)64, (char)71, (char)97, (char)119, (char)94, (char)50, (char)116, (char)71, (char)81, (char)54, (char)49, (char)45,(char)206, (char)210,(char)110, (char)105};

unsigned char QQKey1[] = { 0x21, 0x40, 0x23, 0x29, 0x28, 0x4E, 0x48, 0x4C, 0x69, 0x75, 0x79, 0x2A, 0x24, 0x25, 0x5E, 0x26 };
unsigned char QQKey2[] = { 0x31, 0x32, 0x33, 0x5A, 0x58, 0x43, 0x21, 0x40, 0x23, 0x29, 0x28, 0x2A, 0x24, 0x25, 0x5E, 0x26 };
unsigned char QQKey3[] = { 0x21, 0x40, 0x23, 0x29, 0x28, 0x2A, 0x24, 0x25, 0x5E, 0x26, 0x61, 0x62, 0x63, 0x44, 0x45, 0x46 };
typedef int(*QQdes)(unsigned char *, unsigned char *, int);
typedef int(*QQDdes)(unsigned char *, unsigned char *, int);
typedef int(*QQUncompressCommon)(unsigned char *, unsigned long *, unsigned char const *, unsigned long);
unsigned char * left(unsigned char *dst, unsigned char *src, int n);
int _ustrlen(unsigned char *s);
FileNode krcdecode(char *src,int src_len) {
	if (!src) return {};
	if (memcmp(src, "krc1", 4) != 0) return {};
	for (int i = 4; i < src_len; i++) {
		src[i] = src[i] ^ key[(i-4) % 16];
	}
	uLongf dstsize = src_len * 100;
	uLongf *dstsizep = &dstsize;
	unsigned char *result = (unsigned char *)malloc(dstsize);
	if (Z_OK != uncompress(result, dstsizep, (const Bytef*)(src + 4), src_len-4)) return {};
	result[dstsize] = '\0';
	return FileNode((char *)result , dstsize, true);
}

FileNode qrcdecode(char *src, int src_len) {
	char qrc_hearder[] = "[offset:0]";
	HMODULE hDLL = LoadLibrary(L"QQMusicCommon.dll");
	if (!hDLL) return {};
	if (memcmp(src, qrc_hearder, 10) != 0)return {};
	unsigned long out_len = src_len * 16;
	unsigned long *out_lenp = &out_len;
	QQdes des = QQdes(GetProcAddress(hDLL, "?des@qqmusic@@YAHPAE0H@Z"));
	QQDdes Ddes = QQDdes(GetProcAddress(hDLL, "?Ddes@qqmusic@@YAHPAE0H@Z"));
	Ddes((unsigned char *)(src + 11), QQKey1, src_len - 11);
	des( (unsigned char *)(src + 11), QQKey2, src_len - 11);
	Ddes((unsigned char *)(src + 11), QQKey3, src_len - 11);
	unsigned char *result = (unsigned char *)malloc(out_len);
	if (Z_OK != uncompress(result, out_lenp, (const Bytef*)(src + 11), src_len - 11)) return {};
	result[out_len] = '\0';
	int con=0;
	short con_ = 0;
		goback:
	while (con[result] != '\n')
		*++result;
	*++result;
	con_++;
	if(con_!=5)
	goto goback;
	FreeLibrary(hDLL);
	con = _ustrlen(result);
	while (con[result] != ')'){
		con[result] = '\0';
			con--;
	}
	return FileNode((char *)result,out_len,true);
}
unsigned char * left(unsigned char *dst, unsigned char *src, int n)
{
	unsigned	char *p = src;
	unsigned	char *q = dst;
	int len = _ustrlen(src);
	if (n > len) n = len;
	while (n--) *(q++) = *(p++);
	*(q++) = '\0'; 
	return dst;
}
int _ustrlen(unsigned char *s) {
	int v=0;
	while (v[s] != '\0')
		v++;
	return v;
}