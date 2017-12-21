/*******************************************************************
** 文件名:	DataGuardAPI.cpp
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

#include "stdafx.h"
#include "GuardAPI.h"
#include <time.h>
#include <string>
#include "tchar.h"
#include "StringHelper.h"
using namespace std;
using namespace rkt;


/** 取得字节大小显示字串
@param  ULONGLONG dwSize:字节大小
@return  LPCSTR
*/	
LPCSTR GetFileSizeString(ULONGLONG dwSize)
{
	static char szBuf[64] = {0};

	double fSize = (double)dwSize;

	memset(szBuf,0,sizeof(szBuf));

	if(dwSize<1024)
	{
		sprintf_s(szBuf, sizeof(szBuf), "%I64d 字节",dwSize);
	}
	else if(dwSize<1048576)
	{
		fSize = fSize/1024.0;
		sprintf_s(szBuf, sizeof(szBuf), "%.1f KB",fSize);
	}
	else if (dwSize<1073741824)
	{
		fSize = fSize/1048576.0;
		sprintf_s(szBuf, sizeof(szBuf), "%.1f MB",fSize);
	}
	else
	{
		fSize = fSize/1073741824.0;
		sprintf_s(szBuf, sizeof(szBuf), "%.1f GB",fSize);
	}
	return szBuf;
}

/** 取得剩余时间字串
@param  char * szBuf:字串buffer
@param  int nBufSize:字串buffer长度
@param  DWORD nRemainSeconds:剩余时间(秒)
@return  
*/	

void GetRemainTimeString(char * szBuf,int nBufSize,DWORD nRemainSeconds)
{
	// 剩余时间计数
	int bNums=0,sNums=0;

	// 小于等于60秒
	if (nRemainSeconds<60)
	{
		sprintf_s(szBuf, nBufSize, "%d秒",nRemainSeconds);
	}
	else if(nRemainSeconds<3600)
	{// 小于1小时
		bNums=(int)(nRemainSeconds / 60) ;
		sNums =(int)(nRemainSeconds % 60) ;
		if (sNums==0)
		{
			sprintf_s(szBuf, nBufSize, "%d分钟",bNums );
		} 
		else
		{
			sprintf_s(szBuf, nBufSize, "%d分钟%d秒",bNums,sNums);
		}

	}
	else if (nRemainSeconds<86400)
	{ // 小于1天
		bNums=(int)(nRemainSeconds / 3600) ;
		sNums =(int)((nRemainSeconds % 3600)/60) ;	
		if (sNums==0)
		{
			sprintf_s(szBuf, nBufSize, "%d小时",bNums );
		} 
		else
		{
			sprintf_s(szBuf, nBufSize, "%d小时%d分钟",bNums,sNums);
		}
	} 
	else
	{//大于等于1天
		bNums=(int)(nRemainSeconds / 86400) ;
		sNums =(int)((nRemainSeconds % 86400)/3600) ;
		if (sNums==0)
		{
			sprintf_s(szBuf, nBufSize, "%d天",bNums );
		} 
		else
		{
			sprintf_s(szBuf, nBufSize, "%d天%d小时",bNums,sNums);
		}
	}
}

/** 取得剩余时间字串
@param  DWORD nRemainSeconds:剩余时间(秒)
@return  LPCSTR
*/	
LPCSTR GetRemainTimeString(DWORD nRemainSeconds)
{
	static char szTimebuf[10][32] = {0};
	static BYTE nBufIndex = 0;

	nBufIndex++;
	if (nBufIndex>=10)
	{
		nBufIndex =0;
	}

	memset(szTimebuf[nBufIndex],0,sizeof(szTimebuf[nBufIndex]));
	GetRemainTimeString(szTimebuf[nBufIndex],sizeof(szTimebuf[nBufIndex]),nRemainSeconds);
	return szTimebuf[nBufIndex];
}

/** 取得时间字串
@param   DWORD dwTime:时间
@param   
@return  LPCSTR
*/
LPCSTR GetTimeString(DWORD dwTime)
{
	static char szTimebuf[10][32] = {0};
	static BYTE nBufIndex = 0;

	nBufIndex++;
	if (nBufIndex>=10)
	{
		nBufIndex =0;
	}

	memset(szTimebuf[nBufIndex],0,sizeof(szTimebuf[nBufIndex]));
	if (dwTime==0)
	{
		return szTimebuf[nBufIndex];
	}
	time_t t = (time_t) dwTime;
	tm local;
	memset(&local,0,sizeof(local));
	localtime_s(&local,&t);

	sprintf_s(szTimebuf[nBufIndex], sizeof(szTimebuf[nBufIndex]), "%d-%.2d-%.2d %.2d:%.2d:%.2d",
		1900+local.tm_year,local.tm_mon+1,local.tm_mday,local.tm_hour,local.tm_min,local.tm_sec);

	return szTimebuf[nBufIndex];
}


// 取得服务器类型名称
LPCSTR GetServerTypeString(DWORD dwServerType)
{
	//游戏软件服务器
	static const char *_gszGameSoftServerName[MSG_SERVERID_MAXID] =
	{
		_GT("未知游戏服务器"),
		_GT("场景服务器"),
		_GT("网关服务器"),
		_GT("登录服务器"),
		_GT("游戏客户端"),
		_GT("中心服务器"),
		_GT("社会服务器"),
		_GT("数据监护服务器"),
		_GT("数据监护服务器"),
	};
	if (dwServerType<MSG_SERVERID_MAXID)
	{
		return _gszGameSoftServerName[dwServerType];
	}
	return _gszGameSoftServerName[0];
}


/** 写入数据到文件
@param  BYTE* pData:要保存的数据
@param  int nLens: 数据大小
@param  char * szFileName:要保存的文件名
@return  
*/	
void WriteData2File(BYTE* pData,int nLens,char * szFileName)
{
	BOOL   bSuccess ;
	DWORD  dwBytesWritten ;
	HANDLE hFile= CreateFile (my_utf82t(szFileName), GENERIC_WRITE, 0, NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) ;

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return ;
	}

	bSuccess = WriteFile (hFile, pData, nLens, &dwBytesWritten, NULL) ;

	CloseHandle (hFile) ;
}

/** 写入数据到日志文件
@param  BYTE* pData:要保存的数据
@param  int nLens: 数据大小
@param  char * szFileName:要保存的文件名
@return  
*/	
void SaveLog2File(BYTE* pData,int nLens,char * szFileName)
{
	BOOL   bSuccess ;
	DWORD  dwBytesWritten ;
	HANDLE hFile= CreateFile (my_utf82t(szFileName), GENERIC_WRITE, 0, NULL,OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) ;

	if (hFile == INVALID_HANDLE_VALUE)
	{
		return ;
	}

	bSuccess = WriteFile (hFile, pData, nLens, &dwBytesWritten, NULL) ;

	CloseHandle (hFile) ;
}


// 生成固定长度字串
void AddStringSpace(char * szBuf,int nBufSize,int nStringWidth)
{
	if (szBuf==NULL||nBufSize<=0||nStringWidth<=0)
	{
		return;
	}

	// 在名字后加空格，做成等长字串
	int szlen=strlen(szBuf);
	int nStrWidth = nStringWidth;
	if (nStringWidth>=nBufSize)
	{
		nStrWidth = nBufSize-1;
	}
	int i=0;
	for (i=0;i<nStrWidth-szlen;i++)
	{
		szBuf[szlen+i]=' ';
	}
	szBuf[szlen+i]='\0';
}


/**  取得用分号分开的数据串，存入数组
@param   int * pArray: 数组
@param   int nArrayLen:数组长度
@param	char * szStringBuf : 分析的字串
@param   int nBufLen:字串Buffer长度
@return  bool :成功否
*/
bool GetCsvIntArray(int * pArray,int nArrayLen,const char * szStringBuf,int nBufLen)
{
	if (pArray==NULL || szStringBuf==NULL || nArrayLen<=0 || nBufLen<=0)
	{
		return false;
	}

	// 1;2;3;4;5

	int nGetInt = 0;
	int nGetCount = 0;
	vector<string> strList;
	StringHelper::split(strList, szStringBuf, ';', " ");

	for (WORD i=0;i<strList.size();i++)
	{
		nGetInt = StringHelper::toInt(strList[i]);
		if (nGetCount<nArrayLen)
		{
			(*pArray) = nGetInt;
			pArray++;
			nGetCount++;
		}
	}

	return true;
}