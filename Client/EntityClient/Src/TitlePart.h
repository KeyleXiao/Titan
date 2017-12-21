/*******************************************************************
** 文件名:	TitlePart.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-6-27
** 版  本:	1.0
** 描  述:	实体称号部件接口

********************************************************************/

#pragma once

#include "ITitlePart.h"


// 实体死亡部件
class TitlePart : public ITitlePart, public IEventExecuteSink
{
public:
    ////////////////////IEntityPart/////////////////////////////////////////////
    virtual bool		onLoad( IEntity* pEntity);

    virtual void		release();

    virtual short		getPartID();

    virtual IEntity *	getHost();

    // 消息通讯处理
    virtual bool		onMessage(void * pEntityHead, int msgId, void * data, size_t len);

    // 显示层的命令处理
    virtual bool 		onCommand( int cmdid,void * data,size_t len );

    virtual void        restore();

    ////////////////////ISerializable/////////////////////////////////////////////
    virtual bool		onSerialize( UID uid, rkt::ibuffer & in,int flag );

	virtual bool		deSerialize( rkt::obuf & out,int flag );

	///////////////////IEventExecuteSink////////////////////////////////////
	virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ////////////////////TitlePart/////////////////////////////////////////////
    TitlePart();
    ~TitlePart();
private:
    void                showTitle();

private:
	IEntity*			m_pMaster;			// 实体对象

	map<int, STitleIndexData> m_mapTitleData;
};