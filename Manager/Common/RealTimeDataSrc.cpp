/*******************************************************************
** 文件名:	RealTimeDataSrc.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-9-11
** 版  本:	1.0
** 描  述:	服务器实时数据源处理类
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#include "stdafx.h"
#include "RealTimeDataSrc.h"
#include "Intl.h"
using namespace rkt;



/** 取得指定服务器类型实时数据ID最大值
WORD wServerType		: 服务器类型
@return  DWORD 数据ID最大值
*/
DWORD CRealTimeDataSrc::GetRTDataMaxID(WORD wServerType)
{
	//游戏软件服务器
	static DWORD _gszGameRTDataMaxID[MSG_MG_SERVERID_MAXID] =
	{
		0,		                    // 未知游戏服务器
		MG_ZoneRTData_Max,		    // 场景服务器
		MG_GatewayRTData_Max,		// 网关服务器
		MG_LoginRTData_Max,		    // 登录服务器
		0,		                    // 世界服务器
		0,		                    // 游戏客户端
		MG_CenterRTData_Max,		// 中心服务器
		0,		                    // 验证码服务器
		MG_SocialRTData_Max,		// 社会服务器
		0,		                    // 数据监护服务器
		0,		                    // 跨区桥服务器
		0,		                    // DB应用服务器
		0,		                    // 通行证服务器

		MG_VoiceRTData_Max,			// 语音服务器
		MG_VoiceGatewayRTData_Max,	// 语音网关
		0,							// 社交服务器
		0,							// 中控服务器
        0,                          // 时间同步服务器
	};
	//管理软件服务器
	static DWORD _gszManagerRTDataMaxID[MSG_MG_SERVERID_MGMAXID-MSG_MG_ENDPOINT_ROOT] =
	{
		0,		// 根服务器
		0,		// 主服务器
		MG_ComputerRTData_Max,		// 子服务器
		0,		// 管理器客户端
	};
	if (wServerType<MSG_MG_SERVERID_MAXID)
	{
		return _gszGameRTDataMaxID[wServerType];
	}
	else if (wServerType>=MSG_MG_ENDPOINT_ROOT && wServerType< MSG_MG_SERVERID_MGMAXID)
	{
		return _gszManagerRTDataMaxID[wServerType-MSG_MG_ENDPOINT_ROOT];
	}
	return 0;
}



/** 取得实时数据ID是否开启
WORD wServerType	: 服务器类型
DWORD dwDataID		: 实时数据类型ID
@return  bool 是否开启
*/
bool CRealTimeDataSrc::IsRTDataIDEnable(DWORD dwServerType,DWORD dwDataID)
{

	// 场景服务器
	static bool _gIsDataIDEnable_Zone[MG_ZoneRTData_Max+1] =
	{
		true,		// 进程CPU占用
		true,		// 进程已用物理内存
		true,		// 进程已用虚拟内存
		true,		// 在线人数
		false,		//  无
	};
	//  网关服务器
	static bool _gIsDataIDEnable_Gateway[MG_GatewayRTData_Max+1] =
	{
		true,		// 进程CPU占用
		true,		// 进程已用物理内存
		true,		// 进程已用虚拟内存
		true,		// 在线人数
		true,		// 平均网络延时
		true,		// 网关到大型网站的PING值
		true,		// 延迟率高的玩家占的百分比
		true,		// 客户端每秒发包速度
		true,		// 客户端每秒收包速度
		true,		// 服务器每秒发包速度
		true,		// 服务器每秒收包速度
		false,		//  无
	};
	// 登录服务器
	static bool _gIsDataIDEnable_Login[MG_LoginRTData_Max+1] =
	{
		true,		// 进程CPU占用
		true,		// 进程已用物理内存
		true,		// 进程已用虚拟内存
		true,		// 在线人数
		false,		//  无
	};
	// 中心服务器
	static bool _gIsDataIDEnable_Center[MG_CenterRTData_Max+1] =
	{
		true,		// 进程CPU占用
		true,		// 进程已用物理内存
		true,		// 进程已用虚拟内存
		true,		// 每秒发包速度
		true,		// 每秒收包速度
		false,		//  无
	};
	//  社会服务器
	static bool _gIsDataIDEnable_Social[MG_SocialRTData_Max+1] =
	{
		true,		// 进程CPU占用
		true,		// 进程已用物理内存
		true,		// 进程已用虚拟内存
		true,		// 在线人数
		true,		// 跨服在线数
		false,		// 无
	};
	//  子服务器管理器
	static bool _gIsDataIDEnable_Computer[MG_ComputerRTData_Max+1] =
	{
		true,		// 物理内存
		true,		// 已用物理内存
		true,		// 虚拟内存
		true,		// 已用虚拟内存
		true,		// CPU占用
		true,		// 进程CPU占用
		false,		// 无
	};

	//  语音服务器
	static bool _gIsDataIDEnable_Voice[MG_VoiceRTData_Max+1] =
	{
		true,		// 进程CPU占用
		true,		// 进程已用物理内存
		true,		// 进程已用虚拟内存
		true,		// 在线人数
		true,		// 客户端每秒发包速度
		true,		// 客户端每秒收包速度
		true,		// 服务器每秒发包速度
		true,		// 服务器每秒收包速度
		false,		//  无
	};

	//  语音网关
	static bool _gIsDataIDEnable_VoiceGateway[MG_VoiceGatewayRTData_Max+1] =
	{
		true,		// 进程CPU占用
		true,		// 进程已用物理内存
		true,		// 进程已用虚拟内存
		true,		// 在线人数
		true,		// 客户端每秒发包速度
		true,		// 客户端每秒收包速度
		true,		// 服务器每秒发包速度
		true,		// 服务器每秒收包速度
		false,		//  无
	};


	bool bRet = false;

	DWORD dwMaxDataID = GetRTDataMaxID((WORD)dwServerType);
	if (dwDataID<dwMaxDataID)
	{
		// 根据服务器类型检查 dwDataID 是否正确
		switch(dwServerType)
		{
		case MSG_MG_ENDPOINT_SCENE:		// 场景服务器
			{
				bRet = _gIsDataIDEnable_Zone[dwDataID];
			}
			break;
		case MSG_MG_ENDPOINT_GATEWAY:	// 网关服务器
			{
				bRet = _gIsDataIDEnable_Gateway[dwDataID];
			}
			break;
		case MSG_MG_ENDPOINT_LOGIN:		// 登录服务器
			{
				bRet = _gIsDataIDEnable_Login[dwDataID];
			}
			break;
		case MSG_MG_ENDPOINT_CENTER:	// 中心服务器
			{
				bRet = _gIsDataIDEnable_Center[dwDataID];
			}
			break;
		case MSG_MG_ENDPOINT_SOCIAL:	// 社会服务器
			{
				bRet = _gIsDataIDEnable_Social[dwDataID];
			}
			break;
		case MSG_MG_ENDPOINT_MANAGER:	// 子服务器管理器
			{
				bRet = _gIsDataIDEnable_Computer[dwDataID];
			}
			break;
		case MSG_MG_ENDPOINT_VOICE:	// 语音服务器
			{
				bRet = _gIsDataIDEnable_Voice[dwDataID];
			}
			break;
		case MSG_MG_ENDPOINT_VOICEGATE:	// 语音网关
			{
				bRet = _gIsDataIDEnable_VoiceGateway[dwDataID];
			}
			break;
		}
	}

	return bRet;
}



/** 取得状态监听是否开启
WORD wServerType	: 服务器类型
DWORD dwRTVisitorType	: 状态监听类型
@return  bool 是否开启
*/
bool CRealTimeDataSrc::IsRTVisitorEnable(DWORD dwServerType,DWORD dwRTVisitorType)
{
	if (dwRTVisitorType>=MG_RTVisitorType_MaxID)
	{
		return false;
	}
	//  社会服务器
	static bool _gIsRTVisitorEnable_Social[MG_RTVisitorType_MaxID+1] =
	{
		true,		// // 在线IP所有城市坐标信息
		false,		// 无
	};

	bool bRet = false;

	// 根据服务器类型检查 dwDataID 是否正确
	switch(dwServerType)
	{
	case MSG_MG_ENDPOINT_SOCIAL:	// 社会服务器
		{
			bRet = _gIsRTVisitorEnable_Social[dwRTVisitorType];
		}
		break;
	}

	return bRet;
}


//////////////////////////////////////////////////////////////////////////

/**
@name : 服务器实时数据源处理类
@brief:
*/


CRealTimeDataSrc::CRealTimeDataSrc()
{
	m_wServerType	= 0;			// 服务器类型
	m_wSubID		= 0;			// 第n号服务器
	m_dwBufLen		= 0;			// 状态监听数据长度
	m_dwRTVisitorType = 0;			// 状态监听类型

	memset(m_nDataValue, 0, sizeof(m_nDataValue));				// 实时数据
	memset(m_szBufData, 0, sizeof(m_szBufData));				// 状态监听数据
}

CRealTimeDataSrc::~CRealTimeDataSrc() 
{
}

/** 创建扩展部件
DWORD dwID			: 服务器ID
DWORD dwWorldID		: 服务器所在游戏世界ID
WORD wServerType	: 服务器类型
WORD wSubID			: 第n号服务器
bool bIsLocal		: 本地对像
@return  
*/
bool CRealTimeDataSrc::Create(WORD wServerType,WORD wSubID)
{
	m_wServerType	= wServerType;			// 服务器类型
	m_wSubID		= wSubID;				// 第n号服务器
	m_dwBufLen		= 0;					// 状态监听数据长度
	m_dwRTVisitorType = 0;			// 状态监听类型

	memset(m_nDataValue, 0, sizeof(m_nDataValue));				// 实时数据
	memset(m_szBufData, 0, sizeof(m_szBufData));				// 状态监听数据
	m_bitDataFlag.reset();

	return true;
}



/** 检查实时数据ID是否有效
DWORD dwDataID		: 实时数据类型ID
@return  bool 是否有效
*/
bool CRealTimeDataSrc::IsGoodRTDataID(DWORD dwDataID)
{
	if (dwDataID>=MANAGER_REALTIMEDATA_MAXID)
	{
		return false;
	}
	return IsRTDataIDEnable(m_wServerType,dwDataID);
}

/** 设置实时数据
DWORD dwDataID		: 实时数据类型ID,不同的服务器类型用不用的ID 参考 EMManagerRealTimeDataLogin
int nValue			: 数值
@return  是否成功设置
*/
bool CRealTimeDataSrc::SetData(DWORD dwDataID,int nValue)
{
	if (!IsGoodRTDataID(dwDataID))
	{
		ErrorLn(_GT("设置实时数据[ServerType=")<<m_wServerType<<",SubID="<<m_wSubID<<",dwDataID="<<dwDataID<<",nValue="<<nValue<<_GT("]无效，实时数据类型ID未定义! "));
		return false;
	}
	m_nDataValue[dwDataID] = nValue;		// 实时数据
	m_bitDataFlag.set(dwDataID,true);		// 有数据的数据项的映射信息
	return true;
}

/** 取得实时数据
DWORD dwDataID		: 实时数据类型ID
@return  int nValue	: 数值
*/
int CRealTimeDataSrc::GetData(DWORD dwDataID)
{
	if (!IsGoodRTDataID(dwDataID))
	{
		return 0;
	}
	return m_nDataValue[dwDataID];
}


/** 检查实时数据是否变动
DWORD dwDataID		: 实时数据类型ID
@return  bool 是否有变动
*/
bool CRealTimeDataSrc::IsDataChanged(DWORD dwDataID)
{
	if (!IsGoodRTDataID(dwDataID))
	{
		return false;
	}
	return m_bitDataFlag.test(dwDataID);
}

// 清除所有数据变更标识
void CRealTimeDataSrc::ReSetDataFlag()
{
	m_bitDataFlag.reset();		// 有数据的数据项的映射信息
}

// 取得所有数据变更标识
DWORD CRealTimeDataSrc::GetDataChangedFlag()
{
	return m_bitDataFlag.to_ulong();
}

// 取得服务器类型
WORD CRealTimeDataSrc::GetServerType()
{
	return m_wServerType;
}

// 取得第n号服务器序号
WORD CRealTimeDataSrc::GetServerSubID()
{
	return m_wSubID;
}

/** 导出数据现场
@param   LPSTR pszContext	: 导出buffer
@param   int &nLen			: buffer长度,返回时是数据长度
@return  BYTE 导出数据个数
*/
BYTE CRealTimeDataSrc::ExportContext(LPSTR pszContext, int &nLen)
{
	if (NULL==pszContext||nLen<(MANAGER_REALTIMEDATA_MAXID*sizeof(int)) )
	{
		nLen = 0;
		return 0;
	}
	nLen = 0;
	BYTE nDataCounts = 0;
	DWORD dwDataMaxID = GetRTDataMaxID(m_wServerType);

	int * pRTdata = (int *) pszContext;
	for (DWORD i=0;i<dwDataMaxID;i++)
	{
		if (m_bitDataFlag.test(i))
		{
			nDataCounts++;
			(*pRTdata) = m_nDataValue[i];
			pRTdata++;
			nLen+=sizeof(int);
		}
	}
	return nDataCounts;
}



/** 设置状态监听数据
DWORD dwRTVisitorType	: 软件服务器状态监听类型 参考 EMManagerRealTimeVisitorType
LPSTR pszContext	: 设置buffer
int nLen			: buffer长度
@return  是否成功设置
*/
bool CRealTimeDataSrc::SetVisitorData(DWORD dwRTVisitorType,LPSTR pszContext, int nLen)
{
	if (dwRTVisitorType>=MG_RTVisitorType_MaxID ||pszContext==NULL ||nLen<0 || nLen>=sizeof(m_szBufData))
	{
		return false;
	}

	m_dwRTVisitorType = dwRTVisitorType;						// 状态监听类型
	m_dwBufLen = (DWORD)nLen;									// 状态监听数据长度

	// 可传len为0表示没数据
	if (nLen>0)
	{
		memcpy(m_szBufData,pszContext,m_dwBufLen);					// 状态监听数据
	}

	return true;
}

/** 取得状态监听数据
DWORD &dwRTVisitorType	: 取得数据的,软件服务器状态监听类型 参考 EMManagerRealTimeVisitorType
LPSTR &pszContext	: 取得数据buffer指针
int &nLen			: 取得数据buffer长度
@return  是否成功取得
*/
bool CRealTimeDataSrc::GetVisitorData(DWORD &dwRTVisitorType,LPSTR &pszContext,int &nLen)
{
	dwRTVisitorType = m_dwRTVisitorType;						// 状态监听类型
	nLen			= (int)m_dwBufLen;							// 状态监听数据长度
	pszContext		= m_szBufData;
	return true;
}