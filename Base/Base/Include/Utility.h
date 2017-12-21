/*******************************************************************
** 文件名:	Utility.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	11/22/2011
** 版  本:	1.0
** 描  述:	使用函数封装
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "Common.h"

// 取得文件ID
// 算法设计: 文件ID = 文件哈西值 = BKDRHash( 文件路径 + 文件大小 )
RKT_API unsigned int GetFileID( const char * szPath,int nSize=0 );

/**
Hash函数 数据1 数据2 数据3 数据4 数据1得分 数据2得分 数据3得分 数据4得分 平均分 
BKDRHash 2 0 4774 481 96.55 100 90.95 82.05 92.64 
APHash 2 3 4754 493 96.55 88.46 100 51.28 86.28 
DJBHash 2 2 4975 474 96.55 92.31 0 100 83.43 
JSHash 1 4 4761 506 100 84.62 96.83 17.95 81.94 
RSHash 1 0 4861 505 100 100 51.58 20.51 75.96 
SDBMHash 3 2 4849 504 93.1 92.31 57.01 23.08 72.41 
PJWHash 30 26 4878 513 0 0 43.89 0 21.95 
ELFHash 30 26 4878 513 0 0 43.89 0 21.95 

其中数据1为100000个字母和数字组成的随机串哈希冲突个数。数据2为100000个有意义的英文句子哈希冲突
*/

// BKDR Hash Function
RKT_API unsigned int BKDRHash(char *str);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
RAM requirements for LZMA:
  for compression:   (dictSize * 11.5 + 6 MB) + state_size
  for decompression: dictSize + state_size
    state_size = (4 + (1.5 << (lc + lp))) KB
    by default (lc=3, lp=0), state_size = 16 KB.

LZMA properties (5 bytes) format
    Offset Size  Description
      0     1    lc, lp and pb in encoded form.
      1     4    dictSize (little endian).
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct LZMA_PROP
{
	unsigned char    state;
	unsigned char    dicsize[4];
};

RKT_API std::string _URLEncode(const char* pszUrl);
RKT_API std::string PathEncode(const char* pszUrl);

// 压缩文件后缀
#define   COMPRESS_EXTERN				".zip"