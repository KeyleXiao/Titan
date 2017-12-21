/*******************************************************************
** 文件名:	DBHttpService.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/14/2017
** 版  本:	1.0
** 描  述:	HTTPSERVICE


**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#include "stdafx.h"
#include "ISocialGlobal.h"
#include "IMessageDispatch.h"
#include "DBHttpActionway.h"
#include "DBHttpService.h"
#include "IShareServer.h"

using namespace DBHttp;

DBHttpService* g_pDBHttpService = NULL;

/*
@brief：构造
*/
DBHttpService::DBHttpService()
{
	memset(m_ActionwayFun, 0, sizeof(m_ActionwayFun));
}

/*
@brief：析构
*/
DBHttpService::~DBHttpService()
{

}

//////////////////////////////////////////////////////////////////////////
void DBHttpService::Release(void)
{


	delete this;
}

//////////////////////////////////////////////////////////////////////////
bool DBHttpService::Create(void)
{
    IMessageDispatch* pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if (pMessageDispatch) {
        pMessageDispatch->registerTransmitHandler(MSG_MODULEID_DBHTTP, static_cast<ITransmitHandler*>(this));
    }

	RegisterActionwayHandler(DBHttpAction_Notity,           new DBHttpActionNotity);
    RegisterActionwayHandler(DBHttpAction_SystemMessage,    new ActionwaySystemMessage);

	return true;
}


/** 执行Service的on_stop
@return
*/
void DBHttpService::onStop(void)
{
    UnRegisterActionwayHandler(DBHttpAction_Notity);
    UnRegisterActionwayHandler(DBHttpAction_SystemMessage);

    IMessageDispatch* pMessageDispatch = gSocialGlobal->getMessageDispatch();
    if (pMessageDispatch) {
        pMessageDispatch->unregisterTransmitHandler(MSG_MODULEID_DBHTTP);
    }
}

//////////////////////////////////////////////////////////////////////////
bool DBHttpService::RegisterActionwayHandler(DWORD actionway, IActionway* handler)
{
	if(actionway < 0 && actionway >= DBHttpAction_NUM) return false;
	if (m_ActionwayFun[actionway])
	{
		return false;
	}

	m_ActionwayFun[actionway] = handler;
	return true;
}

//////////////////////////////////////////////////////////////////////////
void DBHttpService::UnRegisterActionwayHandler(DWORD actionway)
{
	if(actionway < 0 && actionway >= DBHttpAction_NUM) return;
	if (m_ActionwayFun[actionway])
	{
		delete m_ActionwayFun[actionway];
		m_ActionwayFun[actionway] = 0;
	}
}

/*
@Brief: 解析请求消息
*/
bool DBHttpService::ParseParam(ibuffer& buf)
{
	buf >> m_msg;
	if( !buf.good() )
		return false;

	for (DWORD i=0; i<m_msg.num; i++)
	{
		DWORD ID;
		buf >> ID;
		m_PDBIDList.push_back(ID);
		if (!buf.good())
			return false;
	}

	std::string temp;
	buf >> temp;
	std::vector<std::string> thingslist;
	StringHelper::split(thingslist, temp.c_str() ,"|||");
	if (thingslist.size() == 0)
		return false;

	m_value = thingslist[0];
	if (thingslist.size() == 2)
	{
		StringHelper::split(m_valueParams,thingslist[1].c_str(),',');
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
void DBHttpService::onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len)
{
	switch(uMsgID)
	{
	case MSG_DBHTTP2ZONE_REQUEST:
		{
			HandleDBHttpRequest(data,len);
		}
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
void DBHttpService:: onServerListUpdated()
{

}

//////////////////////////////////////////////////////////////////////////
void DBHttpService:: onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData)
{

}

//////////////////////////////////////////////////////////////////////////
/*
@brief：处理请求，核心函数
*/
void DBHttpService::HandleDBHttpRequest(void * data, size_t len)
{
	ibuffer buf(data,len);

	//解析参数
	ClearParam();
	if( !ParseParam(buf) )
		return;

	if (m_msg.action <= 0 || m_msg.action >= DBHttpAction_NUM || m_ActionwayFun[m_msg.action] == NULL)
	{
		return ;
	}

	if (m_msg.num <= 0)
	{
		m_ActionwayFun[m_msg.action]->OnGodExectue(m_msg, m_value,m_valueParams);
	}
	else
	{
		//根据module action actionway 进行操作
		for (DWORD n=0; n<m_msg.num; n++)
		{
            DWORD & dwPDBID = m_PDBIDList[n];
            SSharePersonProperty person = gSocialGlobal->getShareReceiver()->GetSharePerson(dwPDBID);
			if (person.dwPDBID != dwPDBID)
				continue;

			m_ActionwayFun[m_msg.action]->OnExectue(dwPDBID, m_msg, m_value,m_valueParams);
		}
	}
}

// 输出函数
API_EXPORT IDBHttpService* CreateDBHttpService(void)
{
	// 如果创建了，则直接返回
	if(g_pDBHttpService != NULL)
	{
		return g_pDBHttpService;
	}

	// 实例化系统模块
	DBHttpService * pDBHttpService = new DBHttpService();
	if(pDBHttpService == NULL || !pDBHttpService->Create())
	{
		SAFE_RELEASE(pDBHttpService);
		return NULL;
	}

	g_pDBHttpService = pDBHttpService;

	return g_pDBHttpService;
}