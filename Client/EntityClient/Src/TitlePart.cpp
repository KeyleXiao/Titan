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
#include "EntityViewDef.h"
#include "TitleMessageDef.h"

////////////////////* TitlePart */////////////////////////////////////////////
TitlePart::TitlePart()
{
    m_pMaster = NULL;	
}

TitlePart::~TitlePart()
{

}

////////////////////* IEntityPart */////////////////////////////////////////////
bool TitlePart::onLoad( IEntity* pEntity)
{
    if (!pEntity)
    {
        return false;
    }
	m_pMaster = pEntity;

	UID uid = m_pMaster->getUID();
	BYTE type = UID_2_TYPE(uid);
	gClientGlobal->getEventEngine()->Subscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CREATE_VIEW, type, uid, __FUNCTION__);
    
    return true;
}

void TitlePart::release()
{
	if (m_pMaster != NULL)
	{
		UID uid = m_pMaster->getUID();
		BYTE type  = UID_2_TYPE(uid);
		gClientGlobal->getEventEngine()->UnSubscibe(static_cast<IEventExecuteSink*>(this), EVENT_ENTITY_CREATE_VIEW, type, uid);
	}

    m_pMaster = 0;

    delete this;
}

short TitlePart::getPartID()
{
    return PART_TITLE;
}

IEntity * TitlePart::getHost()
{
    return m_pMaster;
}

// 消息通讯处理
bool TitlePart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    switch( msgId )
    {
    case SC_MSG_TITLE_ADD:						// 添加称号
        {
            // 实体移动
            if(data != NULL && len >= sizeof(msg_title_add))
            {
                // 修改并更新
                msg_title_add *pData = (msg_title_add *)data;
                if (pData->ID <= TITLE_INDEX_INVALID || pData->ID >= TITLE_INDEX_MAX)
                {
                    return false;
                }
                m_mapTitleData[pData->ID] = pData->IndexData;
                
                showTitle();
            }
        }
        break;
    case SC_MSG_TITLE_DELETE:					// 移除称号
        {
            // 实体移动
            if(data != NULL && len >= sizeof(msg_title_delete))
            {
                // 发给显示层进行移动 todo.
                msg_title_delete *pData = (msg_title_delete *)data;
                if (pData->ID <= TITLE_INDEX_INVALID || pData->ID >= TITLE_INDEX_MAX)
                {
                    return false;
                }
                m_mapTitleData.erase(pData->ID);
                showTitle();
            }
        }
        break;
   
    default:
        return false;
    }
    return true;
}

// 显示层的命令处理
bool TitlePart::onCommand( int cmdid,void * data,size_t len )
{
    return false;
}

bool TitlePart::onSerialize( UID uid, rkt::ibuffer & in,int flag )
{
    int nCount = 0;
    in >> nCount;

	m_mapTitleData.clear();
    for (int i = 0; i < nCount; ++i)
    { 
        STitleIndexData node;
        in >> node;
		m_mapTitleData[node.nTitleID] = node;
    }

    return true;
}

bool TitlePart::deSerialize( rkt::obuf & out,int flag )
{
    return true;
}

///////////////////IEventExecuteSink////////////////////////////////////
void TitlePart::OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	switch(wEventID)
	{
	case EVENT_ENTITY_CREATE_VIEW:
		{
			if (m_pMaster == NULL)
				return;

			showTitle();
		}
		break;
	default:
		break;
	}
}

void TitlePart::showTitle()
{
    obuf obData;
    cmd_count cmdCount;
    cmdCount.nCount = m_mapTitleData.size();
    obData.push_back(&cmdCount, sizeof(cmdCount));

    map<int, STitleIndexData>::iterator iter = m_mapTitleData.begin();
    for ( ; iter != m_mapTitleData.end(); ++iter)
    {
        cmd_entity_show_title_node cmdNode;
        cmdNode.nTitleID = iter->second.nTitleID;
        sstrcpyn(cmdNode.szTitleName, iter->second.szTitleName, sizeof(cmdNode.szTitleName));
        obData.push_back(&cmdNode, sizeof(cmdNode));
    }

    //发送到客户端显示
    m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_SHOW_TITLE, 0, 0, obData.data(), obData.size());
}

void TitlePart::restore()
{

}

////////////////////* ITitlePart */////////////////////////////////////////////



