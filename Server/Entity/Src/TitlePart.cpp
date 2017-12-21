/*******************************************************************
** 文件名:	TitlePart.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-6-27
** 版  本:	1.0
** 描  述:	实体称号部件接口

********************************************************************/

#include "stdafx.h"
#include "TitlePart.h"
#include "EventDef.h"
#include "IEntityEvent.h"
#include "ClanHelper.h"
#include "IKinScene.h"
#include "IKinSceneService.h"
#include "TitleMessageDef.h"
#include "IPlayerRole.h"
#include "ServiceHelper.h"
#include "IEntityAOI.h"
#include "ExternalHelper.h"

////////////////////* TitlePart */////////////////////////////////////////////
TitlePart::TitlePart()
{
    m_pMaster	= NULL;	
}

TitlePart::~TitlePart()
{

}

////////////////////* IEntityPart */////////////////////////////////////////////
short TitlePart::getPartID()
{
    return PART_TITLE;
}

bool TitlePart::onLoad( __IEntity* pEntity, int nLoginMode)
{
    if (!pEntity)
    {
        return false;
    }
	m_pMaster = pEntity;

	// 注册实体创建消息
	m_pMaster->getEntityEvent()->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CREATE, "TitlePart::onLoad");
    m_pMaster->getEntityEvent()->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ACTOR_CHANGE_NAME, "TitlePart::onLoad");
    return true;
}

bool TitlePart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    return true;
}

__IEntity * TitlePart::getHost()
{
    return m_pMaster;
}

void TitlePart::release()
{
	if (m_pMaster != NULL)
	{
		// 注销实体创建消息
		m_pMaster->getEntityEvent()->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CREATE);
		m_pMaster->getEntityEvent()->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_ACTOR_CHANGE_NAME);
	}

    m_pMaster = 0;

    delete this;
}

bool TitlePart::onSerialize( rkt::ibuffer & in,int flag )
{
    if ( (flag & TO_ALL_CLIENT) || (flag & TO_OTHER_SEVER) )
    {
        int nCount = 0;
        in >> nCount;

        for (int i = 0; i < nCount; ++i)
        {
            STitleIndexData node;
            in >> node;
            m_mapIndexData[node.nTitleID] = node;
        }
    }

    return true;
}

bool TitlePart::deSerialize( rkt::obuf & out,int flag )
{
    if( (flag & TO_ALL_CLIENT) || (flag & TO_OTHER_SEVER) )
	{
        int nCount = (int)m_mapIndexData.size();
        out << nCount;
        for (TITLE_INDEXDATA_MAP::iterator iter = m_mapIndexData.begin(); iter != m_mapIndexData.end(); ++iter)
        {
            STitleIndexData & node = iter->second;
            out.push_back(&node, sizeof(node));
        }
    }
    return true;
}

/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
// 容器事件触发后执行函数
void TitlePart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch (wEventID)
	{
	case EVENT_ENTITY_CREATE:
		{
			if (m_pMaster == NULL)
				return;

			addTitle(TITLE_INDEX_NAME, m_pMaster->getName());
			
			int nKinID = m_pMaster->getIntProperty(PROPERTY_KIN);
			if (nKinID > 0)
			{
				IKinScene *pKinScene = gServerGlobal->getKinScene();
				if (pKinScene != NULL)
				{
					SERVICE_PTR pContainer = pKinScene->getKinSceneService();
					if (pContainer != NULL)
					{
						IKinSceneService *pKinService = (IKinSceneService*)pContainer->query_interface();
						if (pKinService != NULL)
						{
							SKinInfo sKin = pKinService->getKinInfo(nKinID);
							if (sKin.nKinProp_ID != 0)
							{
								addTitle(TITLE_INDEX_KIN, sKin.szName,sizeof(sKin.szName));
							}
						}
					}
				}
			}

			int nClanID = m_pMaster->getIntProperty(PROPERTY_CLAN);
			if (nClanID > 0)
			{
				ClanHelper clanHelper;
				if (clanHelper.m_ptr != NULL)
				{
					string clanName = clanHelper.m_ptr->getClanName(nClanID);
					if (clanName.length() > 0)
					{
						addTitle(TITLE_INDEX_CLAN, clanName.c_str(), clanName.length());
					}
				}
			}
		}
		break;
	case EVENT_ENTITY_ACTOR_CHANGE_NAME:
		{
			if (m_pMaster == NULL)
				return;

			addTitle(TITLE_INDEX_NAME, m_pMaster->getName());
		}
		break;
	default:
		break;
	}
}

////////////////////* __ITitlePart */////////////////////////////////////////////
// 加入称号
bool TitlePart::addTitle(int nTitleID, const char * szTitleName, bool bCoverTitle)
{
	if (nTitleID < 0 || nTitleID >= TITLE_INDEX_MAX || szTitleName == NULL)
		return false;

    bool bCanAdd = true;

    TITLE_INDEXDATA_MAP::iterator iter = m_mapIndexData.find(nTitleID);
    if(iter != m_mapIndexData.end() && !bCoverTitle)
    {
        bCanAdd = false;
    }
    if(bCanAdd)
    {
        STitleIndexData node;
        node.nTitleID = nTitleID;
        sstrcpyn(node.szTitleName, szTitleName, sizeof(node.szTitleName));
        m_mapIndexData[nTitleID] = node;

        // 更新
        msg_title_add sMsgContext;
        sMsgContext.ID = nTitleID;
        sMsgContext.IndexData.nTitleID = node.nTitleID;
        sstrcpyn(sMsgContext.IndexData.szTitleName, node.szTitleName, sizeof(sMsgContext.IndexData.szTitleName));
        g_EHelper.broadCastInAOI(m_pMaster, m_pMaster->getLocation(), getPartID(), SC_MSG_TITLE_ADD, (char *)&sMsgContext, sizeof(sMsgContext));
        

    }
    return bCanAdd;
}



// 删除称号
bool TitlePart::deleteTitle(int nTitleID)
{
    m_mapIndexData.erase(nTitleID);

    msg_title_delete sMsgContext;
    sMsgContext.ID = nTitleID;
    g_EHelper.broadCastInAOI(m_pMaster, m_pMaster->getLocation(), getPartID(), SC_MSG_TITLE_DELETE, (char *)&sMsgContext, sizeof(sMsgContext));

    return true;
}


void TitlePart::SendEntityMessage(UID uid, short byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen)
{
    obuf obufData;
    SNetMsgHead head;
    head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
    head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
    head.byKeyModule	= MSG_MODULEID_ENTITY;
    head.byKeyAction	= byKeyAction;

    // 实体类统一用子头来用于转向部件
    SMsgEntityHead entityHead;
    entityHead.uidMaster = uid;
    entityHead.byPartID = byPartID;

    obufData << head << entityHead;
    obufData.push_back(pszData,nDataLen);

    ((__IPlayerRole*)m_pMaster)->send2Client(obufData.data(), obufData.size());
}

void TitlePart::restore()
{

}

//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) __ITitlePart * CreateTitlePart()
{
    return new TitlePart;
}