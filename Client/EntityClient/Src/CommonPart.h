/*******************************************************************
** 文件名:	CommonPart.h
** 版  权: (C) 深圳冰川网络技术有限公司
** 创建人:	李有红
** 日  期: 2017/1/22
** 版  本: 1.0
** 描  述: 通用部件
** 应  用:  	
	
*******************************************************************/

#pragma once
#include "ICommonPart.h"
#include "IMessageDispatch.h"
#include <map>

class CommonPart:public ICommonPart
{
public:
	// IEntityPart
	//////////////////////////////////////////////////////////////////////////
	virtual short  getPartID();

	virtual bool onLoad( IEntity* pEntity);

	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);

	virtual bool onCommand( int cmdid,void * data,size_t len );

	virtual IEntity * getHost();

	virtual void release();

    virtual void restore();
	// ISerializable
	//////////////////////////////////////////////////////////////////////////
	virtual bool onSerialize( UID uid, rkt::ibuffer & in,int flag );

	virtual bool deSerialize( rkt::obuf & out,int flag );

	virtual void onMeetNPC(UID uidNPC);

private:
    // 接收到服务器下发NavGroup数据
    void onMessageSendNavGroup(void * data, size_t len);
    // 接收到服务器下发WebUrl数据
    void onMessageSendWebUrl(void * data, size_t len);
    // 处理Url通知到显示层
    void processWebUrlToView(int nWebUrlID, const char* szUrl);
private:
	// 所属实体
	IEntity* m_pMaster;
};

