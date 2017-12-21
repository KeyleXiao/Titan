/*******************************************************************
** 文件名:	BCStrategyMgr.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	3/09/2015
** 版  本:	1.0
** 描  述:	MMO广播策略管理类
            
********************************************************************/

#pragma once

#include "ISceneManager.h"
#include "BCStrategy.h"
#include "EntityHelper.h"
#include <hash_map>
#include <vector>

using stdext::hash_map;
using std::vector;

// 预制策略对象
struct BCPrefabStg
{
	int key;
	BCStrategy* strategy;
	BCPrefabStg(int k = 0, BCStrategy* stg = NULL)
		: key(k)
		, strategy(stg)
	{

	}
};

/*
* 每个客户端角色对应一个BCClient对象, 该对象用于维护角色自身广播数据的派遣和发送
*
*/
class BCClient
{
	enum
	{
		TBL_SIZE = 32,
		MAX_BC_ID = 1024
	};

public:
	BCClient(AOI_Proxy* obj)
		: m_pClient(obj)
	{
		memset(m_strategyTable, 0, sizeof(m_strategyTable));
	}

	~BCClient()
	{
		for (int i = 0; i < s_keynum; ++i)
		{
			delete m_strategyTable[i];
		}
	}

	BCStrategy* GetStrategy(BROADCAST_ID id);

	void RemoveMessage(AOI_Proxy* obj);

	void Run();

	static void Init();

private:
	static int s_keynum;
	static BCPrefabStg s_id2key[MAX_BC_ID];

private:
	AOI_Proxy* m_pClient;
	BCStrategy* m_strategyTable[TBL_SIZE];
};

class BCStrategyMgr
{
public:
	BCStrategyMgr();
	~BCStrategyMgr();

	void InsertEntity(AOI_Proxy* pSrcObj);

	void RemoveEntity(AOI_Proxy* pSrcObj);

	/**
	@name				: 分发广播包
	@param				: 
	@return
	*/
	void Dispatch(AOI_Proxy* pClientObj, AOI_Proxy* pSrcObj, BROADCAST_ID id, PACKAGE_PTR package = PACKAGE_PTR(NULL));

	// 运行广播策略, 在定时器或者线程主循环里面调用
	void Run();

private:
	typedef hash_map<int, BCClient*> MAP_OBJS;
	MAP_OBJS m_mapObjs;
};
