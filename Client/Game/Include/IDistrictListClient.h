/*******************************************************************
** 文件名:	IDistrictListClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2012/3/19
** 版  本:	1.0
** 描  述:	游戏选区接口
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#ifndef __I_DistrictListClient_H__
#define __I_DistrictListClient_H__

struct SEventSystemDefaultServerChanged
{
	DWORD nID;
	DWORD nGroup;
	DWORD nDistrict;
	DWORD nServer;

	DWORD nID0;		// 服务器索引ID
	DWORD nGroup0;		// 区组序号
	DWORD nDistrict0;	// 版本分类序号
	DWORD nServer0;		// 游戏世界区序号

	SEventSystemDefaultServerChanged(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 游戏分区索引信息
struct SClientGameGroupInfo
{
	DWORD nGroup;					// 区组序号
	DWORD nServerCounts;			// 游戏世界个数
	DWORD nRecommends;				// 推荐值数
	char szName[32];				// 分组名称

	SClientGameGroupInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 服务器索引信息
struct ServerIndexInfo
{
	DWORD		nID;			// 索引ID
	DWORD		nGroup;			// 区组序号
	DWORD		nDistrict;		// 版本分类序号
	DWORD		nServer;		// 游戏世界区序号
	int			nNumber;		// 备用数字

	ServerIndexInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 一个游戏世界区的信息
struct SClientGameServerInfo
{
	DWORD nID;				// 索引ID
	DWORD nGroup;			// 区组序号
	DWORD nDistrict;		// 版本分类序号
	DWORD nServer;			// 游戏世界区序号

	DWORD nWorldID;			// 游戏世界ID
	DWORD nLatencty;		// 网络延迟
	DWORD nState;			// 状态 0 良好 1 繁忙 2 爆满 3 维护
	DWORD nRecommend;		// 推荐值 1:推荐 0:无
	DWORD dwHid;

	char szName[32];			// 服务器名称
	char szGroupName[32];		// 分组名称
	char szVerName[32];			// 版本分类名称
	char szVerShowName[32];		// 版本分类显示名称
	char szDesc[256];			// 服务器描述
	char szAnnounce[256];		// 服务器公告
	char szSameWorld[256];		// 合成一个区的列表
	char szMainten[64];			// 维护信息

	SClientGameServerInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// 游戏分区列表
struct IDistrictListClient
{
	virtual bool Initialize(void) = NULL;

	virtual void Close(void) = NULL;

	//  按索引ID取得游戏世界区信息
	virtual const SClientGameServerInfo * GetGameServerInfoByID(DWORD nIndex) = NULL;

	//  按世界ID取得游戏世界区信息
	virtual const SClientGameServerInfo *GetGameServerInfoByWorldID(DWORD dwWorldID) = NULL;

	//  按组+区+服务器号取得游戏世界区的信息
	virtual const SClientGameServerInfo * GetGameServerInfo(DWORD nGroup,DWORD nDistrict,DWORD nServer) = NULL;

	//  按大区和游戏名取得游戏世界区的信息 szAreaName:大区名,如:电信一区,szWorldName:游戏区名,bAutoMatch:如精确找不到就只按游戏区名找,并按用户网络找到匹配游戏区
	virtual const SClientGameServerInfo * GetGameServerInfoByName(const char * szAreaName,const char * szWorldName,bool bAutoMatch=true) = NULL;

	//  取得推荐的游戏世界区的信息
	virtual const SClientGameServerInfo * GetRecommendGameServerInfo() = NULL;

	//  取得当前的游戏世界区的信息
	virtual const SClientGameServerInfo * GetSelectGameServerInfo() = NULL;

	// 取得游戏世界名称
	virtual const char* GetWorldNameByID(DWORD dwWorldID) = NULL;

	//  设定当前游戏世界区的信息 bSendEvent:是否发事件
	virtual bool SetSelectGameServerInfo(DWORD nGroup,DWORD nDistrict,DWORD nServer,bool bSendEvent=true) = NULL;

	// 进入游戏区事件处理
	virtual void OnEnterServer(DWORD nGroup,DWORD nDistrict,DWORD nServer) = NULL;

	// 游戏区名是否和输入的关键字匹配,包括汉字首字母匹配
	virtual bool IsKeyWordMatchServer(DWORD nGroup,DWORD nDistrict,DWORD nServer,const char * szKeyWord) = NULL;

	// 本地是联通还是电信 1.电信,2.联通,3.移动
	virtual DWORD GetMyISP() = NULL;

	// 设定本地是联通还是电信 1.电信,2.联通,3.移动
	virtual void SetMyISP(DWORD dwISP) = NULL;
};
#endif