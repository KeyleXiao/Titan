/*******************************************************************
** 文件名:	GlobalServerDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015/7/9
** 版  本:	1.0
** 描  述:	全局服务器定义
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once
#pragma pack(1)


// 服务器性监控结构体
struct SServerCapability 
{
	int nGameWorldID;	// 游戏世界ID
	WORD wServerType;	// 服务器类型
	WORD wServerNo;		// 服务器编号（同类型时）
	int nCheckTime;		// 开始检测采集的时间点
	int nInterval;		// 毫秒

	SServerCapability()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 完成检测的结点
typedef list<SServerCapability> TCAPABILITYNODE;

// 最新监控结点数据 LONGLONG = gameworldID(4) + serverType(2) + serverNo(2)
typedef map<LONGLONG, SServerCapability> TMONITORNODE;





#pragma pack()