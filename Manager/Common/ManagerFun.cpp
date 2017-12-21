/*******************************************************************
** 文件名:	ManagerFun.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2009/8/25
** 版  本:	1.0
** 描  述:	服务器管理器共用函数
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "ManagerFun.h"
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
		sprintf_s(szBuf, sizeof(szBuf), "%I64d BYTE",dwSize);
	}
	else if(dwSize<1048576)
	{
		fSize = fSize/1024.0;
		sprintf_s(szBuf, sizeof(szBuf), "%.2f KB",fSize);
	}
	else if (dwSize<1073741824)
	{
		fSize = fSize/1048576.0;
		sprintf_s(szBuf, sizeof(szBuf), "%.2f MB",fSize);
	}
	else
	{
		fSize = fSize/1073741824.0;
		sprintf_s(szBuf, sizeof(szBuf), "%.2f GB",fSize);
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
		sprintf_s(szBuf, nBufSize, _GT("%d秒"),nRemainSeconds);
	}
	else if(nRemainSeconds<3600)
	{// 小于1小时
		bNums=(int)(nRemainSeconds / 60) ;
		sNums =(int)(nRemainSeconds % 60) ;
		if (sNums==0)
		{
			sprintf_s(szBuf, nBufSize, _GT("%d分钟"),bNums );
		} 
		else
		{
			sprintf_s(szBuf, nBufSize, _GT("%d分钟%d秒"),bNums,sNums);
		}

	}
	else if (nRemainSeconds<86400)
	{ // 小于1天
		bNums=(int)(nRemainSeconds / 3600) ;
		sNums =(int)((nRemainSeconds % 3600)/60) ;	
		if (sNums==0)
		{
			sprintf_s(szBuf, nBufSize, _GT("%d小时"),bNums );
		} 
		else
		{
			sprintf_s(szBuf, nBufSize, _GT("%d小时%d分钟"),bNums,sNums);
		}
	} 
	else
	{//大于等于1天
		bNums=(int)(nRemainSeconds / 86400) ;
		sNums =(int)((nRemainSeconds % 86400)/3600) ;
		if (sNums==0)
		{
			sprintf_s(szBuf, nBufSize, _GT("%d天"),bNums );
		} 
		else
		{
			sprintf_s(szBuf, nBufSize, _GT("%d天%d小时"),bNums,sNums);
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
	static const char *_gszGameSoftServerName[MSG_MG_SERVERID_MAXID] =
	{
		_GT("未知游戏服务器"),     // 未定义	
		_GT("场景服务器"),           // 场景服务器	    ［简称：S］
		_GT("网关服务器"),           // 网关服务器	    ［简称：G］
		_GT("登录服务器"),           // 登录服务器	    ［简称：L］
		_GT("世界服务器"),           // 世界服务器	    ［简称：W］
		_GT("游戏客户端"),           // 客户端			［简称：C］
		_GT("中心服务器"),           // 中心服务器	    ［简称：E］
		_GT("验证码服务器"),         // 验证服务器	    ［简称：Y］
		_GT("社会服务器"),           // 社会服务器	    ［简称：O］
		_GT("数据监护服务器"),       // 数据监护服务器	［简称：G］
		_GT("跨区桥服务器"),         // 跨区桥服务器	［简称：B］
		_GT("DB应用服务器"),         // DB应用服务器	［简称：D］
		_GT("通行证服务器"),         // 通行证服务器	［简称：P］
		_GT("语音服务器"),           // 语音服务器	    ［简称：V］	
		_GT("语音网关"),             // 语音网关		 [简称：VG］
		_GT("社交服务器"),           // 社交服务器       [简称：N]
		_GT("中控服务器"),           // 中控服务器       [简称：R]
        _GT("时间同步服务器"),       // 时间同步服务器   [简称：T]
	};
    

	//管理软件服务器
	static const char *_gszManagerSoftServerName[MSG_MG_SERVERID_MGMAXID-MSG_MG_ENDPOINT_ROOT] =
	{
		_GT("根服务器"),
		_GT("主服务器"),
		_GT("子服务器"),
		_GT("管理器客户端")
	};
	if (dwServerType<MSG_MG_SERVERID_MAXID)
	{
		return _gszGameSoftServerName[dwServerType];
	}
	else if (dwServerType>=MSG_MG_ENDPOINT_ROOT && dwServerType< MSG_MG_SERVERID_MGMAXID)
	{
		return _gszManagerSoftServerName[dwServerType-MSG_MG_ENDPOINT_ROOT];
	}
	return _gszGameSoftServerName[0];
}

// 取得软件服务器状态监听类型名称
LPCSTR GetRealTimeVisitorTypeString(DWORD dwType)
{
	//用户类型名称
	static const char *_gszGameRealTimeVisitorTypeName[MG_RTVisitorType_MaxID+1] =
	{
		_GT("未知状态监听类型"),
		_GT("在线用户城市地图"),
	};

	if (dwType<MG_RTVisitorType_MaxID)
	{
		return _gszGameRealTimeVisitorTypeName[dwType+1];
	}
	return _gszGameRealTimeVisitorTypeName[0];
}


// 取得用户类型名称
LPCSTR GetManagerUserTypeString(DWORD dwUserType)
{
	//用户类型名称
	static const char *_gszGameUserTypeName[MANAGER_TYPE_MAXID+1] =
	{
		_GT("未知管理员"),
		_GT("普通管理员"),
		_GT("管理员"),
		_GT("超级管理员")
	};

	if (dwUserType<MANAGER_TYPE_MAXID)
	{
		return _gszGameUserTypeName[dwUserType+1];
	}
	return _gszGameUserTypeName[0];
}


// 服务器脚本状态名称
LPCSTR GetServerScriptStateString(DWORD dwState)
{
	//用户类型名称
	static const char *_gszGameServerScriptStateName[MANAGER_SCRIPTFLAG_MAXID] =
	{
		_GT("没有脚本"),
		_GT("没加载脚本"),
		_GT("正在加载"),
		_GT("加载完成")
	};
	if (dwState<MANAGER_SCRIPTFLAG_MAXID)
	{
		return _gszGameServerScriptStateName[dwState];
	}
	return _gszGameServerScriptStateName[0];
}

// 上传文件状态状态名称
LPCSTR GetFileUploadStateString(DWORD dwState)
{
	/*
	MG_FileUploadState_None = 0,			// 未知
	MG_FileUploadState_UnLoadFile,			// 未加载文件
	MG_FileUploadState_LoadFileOK,			// 已加载文件
	MG_FileUploadState_LoadFileFail,		// 加载文件失败
	MG_FileUploadState_Request,				// 请求上传文件
	MG_FileUploadState_RequestFail,			// 请求上传文件失败
	MG_FileUploadState_FileData,			// 传送文件数据
	MG_FileUploadState_FileDataOK,			// 传送文件数据成功
	MG_FileUploadState_FileDataFail,		// 传送文件数据失败
	MG_FileUploadState_UnPackOK,			// 解包成功
	MG_FileUploadState_VerifyPackOK,		// 校验成功
	MG_FileUploadState_VerifyPackFail,		// 校验失败
	*/
	static const char *_gszFileUploadStateName[MG_FileUploadState_Max] =
	{
		_GT("未知"),
		_GT("未加载文件"),
		_GT("已加载文件"),
		_GT("加载文件失败"),
		_GT("请求上传中"),
		_GT("请求上传失败"),
		_GT("传送文件数据中"),
		_GT("传送文件成功"),
		_GT("传送文件失败"),
		_GT("解包成功"),
		_GT("校验成功"),
		_GT("校验失败")
	};
	if (dwState<MG_FileUploadState_Max)
	{
		return _gszFileUploadStateName[dwState];
	}
	return _gszFileUploadStateName[0];
}

// 上传文件状态状态名称
LPCSTR GetFileDownloadStateString(DWORD dwState)
{
	/*
	MG_FileDownloadState_None = 0,			// 未知
	MG_FileDownloadState_WaitFileData,		// 等待文件数据
	MG_FileDownloadState_FileDataOK,		// 收到数据成功
	MG_FileDownloadState_FileDataFail,		// 收到数据失败
	MG_FileDownloadState_SaveFileOK,		// 保存文件成功
	MG_FileDownloadState_SaveFileFail,		// 保存文件失败
	*/
	static const char *_gszFileDownloadStateName[MG_FileDownloadState_Max] =
	{
		_GT("未知"),
		_GT("等待文件数据"),
		_GT("收到数据成功"),
		_GT("收到数据失败"),
		_GT("保存文件成功"),
		_GT("保存文件失败")
	};
	if (dwState<MG_FileDownloadState_Max)
	{
		return _gszFileDownloadStateName[dwState];
	}
	return _gszFileDownloadStateName[0];
}


// 刷脚本文件状态名称
LPCSTR GetFileUpdateScpStateString(DWORD dwState)
{
	// 刷新脚本状态定义
	/*
	MG_FileUpdateScpState_None = 0,			// 未知
	MG_FileUpdateScpState_Request,			// 请求刷新
	MG_FileUpdateScpState_UpdateOK,			// 刷新成功
	MG_FileUpdateScpState_UpdateFail,		// 刷新失败
	// 最大
	MG_FileUpdateScpState_Max
	*/

	static const char *_gszFileUpdateScpStateName[MG_FileUpdateScpState_Max] =
	{
		_GT("未知"),
		_GT("请求刷新脚本"),
		_GT("刷新脚本成功"),
		_GT("刷新脚本失败")
	};
	if (dwState<MG_FileUpdateScpState_Max)
	{
		return _gszFileUpdateScpStateName[dwState];
	}
	return _gszFileUpdateScpStateName[0];
}

// 子服务器安装更新进行状态
LPCSTR GetManagerWorldSetupUpdateStatusString(DWORD dwFlag)
{
	/*//  子服务器安装更新进行状态
	enum EMWorldSetupUpdateStatus
	{
	MG_WorldSetupUpdateStatus_None = 0,		// 未开始
	MG_WorldSetupUpdateStatus_GetVer,		// 正在取得版本信息
	MG_WorldSetupUpdateStatus_BackUp,		// 正在备份
	MG_WorldSetupUpdateStatus_ReName,		// 正在重命名
	MG_WorldSetupUpdateStatus_Setup,		// 正在安装更新
	MG_WorldSetupUpdateStatus_Verify,		// 正在校验
	MG_WorldSetupUpdateStatus_OK,			// 完成
	// 最大
	MG_WorldSetupUpdateStatus_MaxID
	};
	*/

	//取得子服务器安装更新进行状态名称
	static const char *_gszGameWorldSetupUpdateStatusName[MG_WorldSetupUpdateStatus_MaxID] =
	{
		_GT("未开始"),
		_GT("取版本"),
		_GT("备份中"),
		_GT("重命名中"),
		_GT("安装更新"),
		_GT("正在校验"),
		_GT("安装完成")
	};

	if (dwFlag<MG_WorldSetupUpdateStatus_MaxID)
	{
		return _gszGameWorldSetupUpdateStatusName[dwFlag];
	}
	return _gszGameWorldSetupUpdateStatusName[0];

}


// 报警等级名称
LPCSTR GetManagerGameAlertLevelNameString(DWORD dwAlertLevel)
{
	/*
	AlertLevel_None		= 0x0,			// 0	:无
	AlertLevel_Info		= 0x1,			// 1	:信息
	AlertLevel_Prompt	= 0x2,			// 2	:提示
	AlertLevel_Emphasis	= 0x4,			// 4	:强调
	AlertLevel_Warning	= 0x8,			// 8	:警告
	AlertLevel_Error	= 0x10,			// 16	:错误
	AlertLevel_Bad		= 0x20,			// 32	:严重
	*/
	//取得举报信息状态名称
	static const char *_gszGameAlertLevelName[AlertLevel_MAXID] =
	{
		"",
		_GT("信息"),
		_GT("提示"),
		_GT("强调"),
		_GT("警告"),
		_GT("错误"),
		_GT("严重")
	};

	static DWORD _gszGameAlertLevelFlag[AlertLevel_MAXID] =
	{
		AlertLevel_None,
		AlertLevel_Info,
		AlertLevel_Prompt,
		AlertLevel_Emphasis,
		AlertLevel_Warning,
		AlertLevel_Error,
		AlertLevel_Bad
	};

	static string _gstrOutGameAlertLevel;

	_gstrOutGameAlertLevel = "";

	for (DWORD i=0;i<AlertLevel_MAXID;i++)
	{
		if ( (_gszGameAlertLevelFlag[i] & dwAlertLevel)!=0)
		{
			if (_gstrOutGameAlertLevel.size()>0)
			{
				_gstrOutGameAlertLevel += " ";
				_gstrOutGameAlertLevel += _gszGameAlertLevelName[i];
			}
			else
			{
				_gstrOutGameAlertLevel += _gszGameAlertLevelName[i];
			}
		}
	}

	return _gstrOutGameAlertLevel.c_str();
}

// 取得游戏服务器报警类型名称
LPCSTR GetManagerGameAlertTypeNameString(DWORD dwAlertType)
{
	/*
	MG_AlertSrcType_None = 0,				// 未知
	MG_AlertSrcType_DataBase,				// 数据库
	MG_AlertSrcType_Performance,			// 性能
	MG_AlertSrcType_Memory,					// 内存
	MG_AlertSrcType_Network,				// 网络
	MG_AlertSrcType_Lua,					// Lua
	MG_AlertSrcType_Script,					// 脚本
	MG_AlertSrcType_Crash,					// 崩溃
	// 最大
	MG_AlertSrcType_MaxID
	*/

	//取得游戏服务器报警类型名称
	static const char *_gszGameAlertSrcTypeName[MG_AlertSrcType_MaxID] =
	{
		_GT("未知"),
		_GT("数据库"),
		_GT("性能"),
		_GT("内存"),
		_GT("网络"),
		"Lua",
		_GT("脚本"),
		_GT("崩溃")
	};

	if (dwAlertType<MG_AlertSrcType_MaxID)
	{
		return _gszGameAlertSrcTypeName[dwAlertType];
	}
	return _gszGameAlertSrcTypeName[0];
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

// 取得服务器状态名称
LPCSTR GetManagerServerStateFlagString(DWORD dwFlag)
{
	// 服务器状态类型定义
	/*enum EMManagerServerStateFlag
	{
		// 未定义
		MG_SERVERSTATE_UNKNOW = 0,		// 未知
		MG_SERVERSTATE_UNSTART,			// 未启动
		MG_SERVERSTATE_STARTING,		// 正在启动
		MG_SERVERSTATE_STARTFAIL,		// 启动失败
		MG_SERVERSTATE_RUNING,			// 已启动，正在远行
		MG_SERVERSTATE_NOANSWER,		// 已启动，无响应
		MG_SERVERSTATE_STOPING,			// 正在关闭
		MG_SERVERSTATE_STOPED,			// 已关闭
		// 最大
		MG_SERVERSTATE_MAXID
	};*/

	//取得服务器状态名称
	static const char *_gszGameServerStateFlagName[MG_SERVERSTATE_MAXID] =
	{
		_GT("未知"),
		_GT("未启动"),
		_GT("启动中..."),
		_GT("启动失败"),
		_GT("运行中"),
		_GT("无响应"),
		_GT("关闭中..."),
		_GT("已关闭")
	};

	if (dwFlag<MG_SERVERSTATE_MAXID)
	{
		return _gszGameServerStateFlagName[dwFlag];
	}
	return _gszGameServerStateFlagName[0];
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


// 获取星期几
DWORD GetDayOfWeekByNow()
{
	SYSTEMTIME systemTime;
	::GetLocalTime(&systemTime);

	DWORD dwDayofWeek = (DWORD)systemTime.wDayOfWeek;

	// 星期天
	if (dwDayofWeek==0)
	{
		dwDayofWeek = 7;
	}

	return dwDayofWeek;
}



// 日期字符串转换成日期结构
struct tm StringConvetToStructTime(char * szBuf, int nBufSize)
{
	int nYear = 0; 
	int nMonth=0; 
	int nDate=0; 
	int nHour=0; 
	int nMin=0; 
	int nSec=0;  

	sscanf_s(szBuf,"%d-%d-%d %d:%d:%d",&nYear,&nMonth,&nDate,&nHour,&nMin,&nSec); 

	time_t time0 = time(NULL);
	struct tm * tmNow = localtime(&time0);

	struct tm  mytime;
	mytime.tm_year = nYear-1900;
	mytime.tm_mon = nMonth-1;
	mytime.tm_mday = nDate;
	mytime.tm_hour = nHour;
	mytime.tm_min = nMin;
	mytime.tm_sec =  nSec;
	mytime.tm_isdst = tmNow->tm_isdst;	

	return mytime;	
}


// 日期字符串转换成日期
DWORD StringConvetToTime(char * szBuf, int nBufSize)
{
	struct tm mytime = StringConvetToStructTime(szBuf, nBufSize);
	time_t time0= mktime(&mytime);

	return (DWORD)time0;
}
