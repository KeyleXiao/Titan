/*******************************************************************
** 文件名:	BCStrategyMgr.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	3/09/2015
** 版  本:	1.0
** 描  述:	广播策略管理类
            
********************************************************************/

#include "stdafx.h"
#include "EntityDef.h"
#include "BCStrategyMgr.h"

int BCClient::s_keynum = 0;
BCPrefabStg BCClient::s_id2key[MAX_BC_ID] = {0};

BCStrategy* BCClient::GetStrategy(BROADCAST_ID id)
{
	if (id >= MAX_BC_ID)
	{
		return NULL;
	}

	BCPrefabStg stg = s_id2key[id];
	// 不存在则克隆一份
	if (m_strategyTable[stg.key] == NULL)
	{
		m_strategyTable[stg.key] = stg.strategy->Clone();
	}
	return m_strategyTable[stg.key];
}

// 删除有关玩家obj的消息
void BCClient::RemoveMessage(AOI_Proxy* obj)
{
	for (int i = 0; i < s_keynum; ++i)
	{
		if (m_strategyTable[i])
			m_strategyTable[i]->RemoveMessage(obj);
	}
}

void BCClient::Run()
{
	for (int i = 0; i < s_keynum; ++i)
	{
		if (m_strategyTable[i])
			m_strategyTable[i]->Run(m_pClient);
	}
}

void BCClient::Init()
{
	if (s_keynum != 0)
		return;

	s_id2key[BROADCAST_ID(BROADCAST_ENTER_VIEWPORT)]	                = BCPrefabStg(s_keynum++, new BCStrategyEnter);
	s_id2key[BROADCAST_ID(BROADCAST_LEAVE_VIEWPORT)]	                = BCPrefabStg(s_keynum++, new BCStrategyLeave);
	s_id2key[BROADCAST_ID(BROADCAST_MOVE)]								= BCPrefabStg(s_keynum++, new BCStrategyMove);
	s_id2key[BROADCAST_ID(BROADCAST_DO_ACTION)]							= BCPrefabStg(s_keynum++, new BCStrategyAction);
	s_id2key[BROADCAST_ID(BROADCAST_CHANGE_STATE)]						= BCPrefabStg(s_keynum++, new BCStrategyState);
	s_id2key[BROADCAST_ID(BROADCAST_CHANGE_PROPERTY)]					= BCPrefabStg(s_keynum++, new BCStrategyProperty);
	s_id2key[BROADCAST_ID(BROADCAST_CHANGE_PROPERTY, PROPERTY_LEVEL)]	= BCPrefabStg(s_keynum++, new BCStrategyLevel);
	s_id2key[BROADCAST_ID(BROADCAST_CHANGE_PROPERTY, PROPERTY_EXP)]		= BCPrefabStg(s_keynum++, new BCStrategyExp);
	s_id2key[BROADCAST_ID(BROADCAST_CHANGE_PROPERTY, PROPERTY_MAX_HP)]	= BCPrefabStg(s_keynum++, new BCStrategyMaxHP);
	s_id2key[BROADCAST_ID(BROADCAST_CHANGE_PROPERTY, PROPERTY_HP)]		= BCPrefabStg(s_keynum++, new BCStrategyHP);
	s_id2key[BROADCAST_ID(BROADCAST_CHANGE_PROPERTY, PROPERTY_MAX_MP)]	= BCPrefabStg(s_keynum++, new BCStrategyMaxMP);
	s_id2key[BROADCAST_ID(BROADCAST_CHANGE_PROPERTY, PROPERTY_MP)]		= BCPrefabStg(s_keynum++, new BCStrategyMP);
	s_id2key[BROADCAST_ID(BROADCAST_PROPERTY_SET)]						= BCPrefabStg(s_keynum++, new BCStrategyPropertySet);
	s_id2key[BROADCAST_ID(BROADCAST_PLAY_EFFECT)]						= BCPrefabStg(s_keynum++, new BCStrategyEffect);
	s_id2key[BROADCAST_ID(BROADCAST_BUFF)]								= BCPrefabStg(s_keynum++, new BCStrategyBuff);
	s_id2key[BROADCAST_ID(BROADCAST_SPELL)]								= BCPrefabStg(s_keynum++, new BCStrategySpell);
    s_id2key[BROADCAST_ID(BROADCAST_SCENE)]								= BCPrefabStg(s_keynum++, new BCStrategyScene);
    s_id2key[BROADCAST_ID(BROADCAST_CREATE)]							= BCPrefabStg(s_keynum++, new BCStrategyCreate);
    s_id2key[BROADCAST_ID(BROADCAST_DESTROY)]							= BCPrefabStg(s_keynum++, new BCStrategyDestroy);

	assert(s_keynum < TBL_SIZE);
}

BCStrategyMgr::BCStrategyMgr()
{
	BCClient::Init();
}

BCStrategyMgr::~BCStrategyMgr()
{
	for (MAP_OBJS::iterator itr = m_mapObjs.begin(); itr != m_mapObjs.end(); ++itr)
	{
		delete itr->second;
	}
}

void BCStrategyMgr::InsertEntity(AOI_Proxy* pSrcObj)
{
	assert(pSrcObj != NULL);
	int serial = UID_2_SERIAL(pSrcObj->getID());
	BCClient* &pClient = m_mapObjs[serial];
	if (pClient) delete pClient;
	pClient = new BCClient(pSrcObj);
}

void BCStrategyMgr::RemoveEntity(AOI_Proxy* pSrcObj)
{
	assert(pSrcObj != NULL);
	int serial = UID_2_SERIAL(pSrcObj->getID());
	BCClient* &pClient = m_mapObjs[serial];
	if (pClient)
	{
		delete pClient;
		m_mapObjs.erase(serial);
	}
}

/**
@name				: 向pClientObj对象派遣pSrcObj的广播包
@param				: 
@return
*/
void BCStrategyMgr::Dispatch(AOI_Proxy* pClientObj, AOI_Proxy* pSrcObj, BROADCAST_ID id, PACKAGE_PTR package)
{
	if (pClientObj == NULL)
		return;

	MAP_OBJS::iterator itr = m_mapObjs.find(UID_2_SERIAL(pClientObj->getID()));
	BCClient* pBCClient = NULL;

	if (itr == m_mapObjs.end())
	{
		ErrorLn(__FUNCTION__ << "cannot find uid= " << pClientObj->getID() << " entity, id.type=" << id.type);
		return;
	}

	pBCClient = itr->second;
	assert(pBCClient != NULL);

	switch (id.type)
	{
		// 通知pClientObj，玩家pSrcObj进入视野了
	case BROADCAST_ENTER_VIEWPORT:
	case BROADCAST_LEAVE_VIEWPORT:
    case BROADCAST_CREATE:
    case BROADCAST_DESTROY:
        package = pSrcObj->getPackage(id.type);
        break;

	default: break;
	}

	pBCClient->GetStrategy(id)->Dispatch(pClientObj, pSrcObj, package);
}

// 运行广播策略, 在定时器或者线程主循环里面调用
// todo:如果该函数很耗时，后续可以把主循环放到ActorService里面实现
void BCStrategyMgr::Run()
{
	for (MAP_OBJS::iterator itr = m_mapObjs.begin(); itr != m_mapObjs.end(); ++itr)
	{
		itr->second->Run();
	}
}
