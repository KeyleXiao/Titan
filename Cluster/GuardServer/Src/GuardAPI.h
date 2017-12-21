/*******************************************************************
** 文件名:	DataGuardAPI.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015-8-17
** 版  本:	1.0
** 描  述:	数据监护服务器共用函数
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


#ifndef __DATAGUARD_API_H__
#define __DATAGUARD_API_H__


/** 取得字节大小显示字串
@param  ULONGLONG dwSize:字节大小
@return  LPCSTR
*/	
LPCSTR GetFileSizeString(ULONGLONG dwSize);

/** 取得剩余时间字串
@param  char * szBuf:字串buffer
@param  int nBufSize:字串buffer长度
@param  DWORD nRemainSeconds:剩余时间(秒)
@return  
*/	
void GetRemainTimeString(char * szBuf,int nBufSize,DWORD nRemainSeconds);

/** 取得剩余时间字串
@param  DWORD nRemainSeconds:剩余时间(秒)
@return  LPCSTR
*/	
LPCSTR GetRemainTimeString(DWORD nRemainSeconds);

// 取得时间字串
LPCSTR GetTimeString(DWORD dwTime);

// 取得服务器类型名称
LPCSTR GetServerTypeString(DWORD dwServerType);

/** 写入数据到文件
@param  BYTE* pData:要保存的数据
@param  int nLens: 数据大小
@param  char * szFileName:要保存的文件名
@return  
*/	
void WriteData2File(BYTE* pData,int nLens,char * szFileName);

/** 写入数据到日志文件
@param  BYTE* pData:要保存的数据
@param  int nLens: 数据大小
@param  char * szFileName:要保存的文件名
@return  
*/	
void SaveLog2File(BYTE* pData,int nLens,char * szFileName);


// 生成固定长度字串
void AddStringSpace(char * szBuf,int nBufSize,int nStringWidth);

/**  取得用分号分开的数据串，存入数组
@param   int * pArray: 数组
@param   int nArrayLen:数组长度
@param	char * szStringBuf : 分析的字串
@param   int nBufLen:字串Buffer长度
@return  bool :成功否
*/
bool GetCsvIntArray(int * pArray,int nArrayLen,const char * szStringBuf,int nBufLen);


#endif	//__DATAGUARD_API_H__