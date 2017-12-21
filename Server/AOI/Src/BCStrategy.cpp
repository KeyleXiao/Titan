/*******************************************************************
** 文件名:	BCStrategy.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	3/09/2015
** 版  本:	1.0
** 描  述:	广播策略类
            
********************************************************************/

#include "stdafx.h"
#include "BCStrategy.h"
#include "IActorService.h"

DWORD BCStrategy::s_dwSendCounts = 0;	// 当前进程发包总数量
DWORD BCStrategy::s_dwSustainedTime = 0;// 持续进程发包持续时间

void BCStrategy::Dispatch(AOI_Proxy* pClient, AOI_Proxy* pSrcObj, PACKAGE_PTR package)
{
	// 对于pSrcObj==NULL的情况，由于根本无法得到到底广播的是哪个玩家的数据，这里直接发送
	if (pSrcObj == NULL)
	{
		pClient->sendBCMessage(package);
		return;
	}

	// 如果找到对应实体的则直接覆盖
	bool bmatch = false;
	list<PreparedPackage>::iterator itrend = m_packagelist.end();
	for (list<PreparedPackage>::iterator itr = m_packagelist.begin(); itr != itrend; ++itr)
	{
		// 同一个人的广播数据直接覆盖
		if (itr->srcobj == pSrcObj)
		{
			itr->package = package;
			itr->sendtick = GetCoolTick(pClient, pSrcObj) + GetTickCount();
			bmatch = true;
			break;
		}
	}
	if (!bmatch)
	{
		PreparedPackage prep;
		prep.srcobj = pSrcObj; prep.package = package;
		prep.sendtick = GetCoolTick(pClient, pSrcObj) + GetTickCount();
		m_packagelist.push_back(prep);
	}

	// 主动调用一次
	Run(pClient);
}

void BCStrategy::Run(AOI_Proxy* pClient)
{
	if (m_packagelist.size() == 0 || pClient == NULL)
	{
		return;
	}

	DWORD dwCurTick = GetTickCount();

	list<PreparedPackage>::iterator itrend = m_packagelist.end();
	for (list<PreparedPackage>::iterator itr = m_packagelist.begin(); itr != itrend; )
	{
		if (itr->sendtick >= dwCurTick)
		{
			pClient->sendBCMessage(itr->package);
			itr = m_packagelist.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

void BCStrategy::RemoveMessage(AOI_Proxy* obj)
{
	list<PreparedPackage>::iterator itrend = m_packagelist.end();
	for (list<PreparedPackage>::iterator itr = m_packagelist.begin(); itr != itrend; )
	{
		if (itr->srcobj == obj)
		{
			itr = m_packagelist.erase(itr);
		}
		else
		{
			++itr;
		}
	}
}

int BCStrategy::GetCoolTick(AOI_Proxy* pClient, AOI_Proxy* pSrcObj)
{
	float distance = GetDistance(pClient, pSrcObj);
	int cool = (int)((pow((double)distance, 0.5)-1)*75);
	if (cool < 0) cool = 0;
	return cool;
}

float BCStrategy::GetDistance(AOI_Proxy* pClient, AOI_Proxy* pSrcObj)
{
	// 根据factor获取冷却时间
	Vector3 locClient = pClient->getLocation();
	locClient.y = 0.0f;

	float distance = 0.0f;
	// NOTE: 无法获取到距离参数，则按照0来计算
	bool validloc = true;
	Vector3 locSrc;
	if (pSrcObj != NULL)
	{
		locSrc = pSrcObj->getLocation();
		validloc = true;
	}

	if (validloc)
		distance = locSrc.getDistance(locClient);
	return distance;
}

int BCStrategyMove::GetCoolTick(AOI_Proxy* pClient, AOI_Proxy* pSrcObj)
{
	float distance = GetDistance(pClient, pSrcObj);
	// 根据factor获取冷却时间
	int cool = (int)((pow((double)distance, 0.5)-1)*50);
	if (cool < 0) cool = 0;
	return cool;
}

int BCStrategyMP::GetCoolTick(AOI_Proxy* pClient, AOI_Proxy* pSrcObj)
{
	float distance = GetDistance(pClient, pSrcObj);
	// 根据factor获取冷却时间
	int cool = (int)((pow((double)distance, 0.5)-1)*50);
	if (cool < 0) cool = 0;
	return cool;
}
