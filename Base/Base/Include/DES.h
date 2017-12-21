/*******************************************************************
** 文件名:	DES.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2010/6/29 14:38
** 版  本:	1.0
** 描  述:	DES加密解密
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#pragma once

#ifndef	RKT_DES_H__DEF
#define RKT_DES_H__DEF

#include "Common.h"

#ifdef SUPPORT_DES

namespace rkt {

class RKT_EXPORT CDESEncoder
{

public:
	CDESEncoder();
	virtual ~CDESEncoder();


/** DES加密字符串,返回的加密后的十六进制字符串
@param	const char *szIn: 需要加密的明文
@param	DWORD dwInSize	: 字符串的size
@param	char * szOut	: 加密后输出的字符串
@param	DWORD dwOutSize	: 存放密文字符串的size,最小为 (dwInSize+8)*2,调用后dwOutSize为实际密文长度
@param	const char *szKey: Key值字符串,为NULL是默认将使用内置的Key
@param	DWORD dwKeySize	: key字符串size
*/
static	bool DES_Encode(const char *szIn, DWORD dwInSize, char * szOut, DWORD & dwOutSize, const char *szKey=NULL, DWORD dwKeySize=0);

/** DES解密字符串,返回的解密后的ASCII字符串
@param	const char *szIn: 需要解密的密文
@param	DWORD dwInLen	: 字符串的Len
@param	char * szOut	: 解密后输出的字符串
@param	DWORD dwOutSize	: 存放解文字符串的size,最小为 dwInSize/2-8,调用后dwOutSize为实际密文长度
@param	const char *szKey: Key值字符串,为NULL是默认将使用内置的Key
@param	DWORD dwKeySize	: key字符串size
*/
static	bool DES_Decode(const char *szIn, DWORD dwInLen, char * szOut, DWORD & dwOutSize, const char *szKey=NULL, DWORD dwKeySize=0);

// DES加密解密 Type: 0.加,1.解
//bool Des_Go(char *In, DWORD datalen, char *Out,  DWORD &nOutLen, const char *Key, DWORD keylen, bool Type);

};

} // rkt

#endif //SUPPORT_DES

#endif	// RKT_DES_H__DEF