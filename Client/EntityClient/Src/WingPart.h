/*******************************************************************
** 文件名:	WingPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	1/21/2016
** 版  本:	1.0
** 描  述:	翅膀部件实现
********************************************************************/

#pragma once

#include "IWingPart.h"

class WingPart : public IWingPart, public IEventExecuteSink
{
public:
    ////////////////////IEntityPart//////////////////////////////////////////////////////
    virtual bool onSerialize( UID uid, rkt::ibuffer & in,int flag );
    virtual bool deSerialize( rkt::obuf & out,int flag );

    virtual short  getPartID() { return PART_WING; }

    virtual bool onLoad( IEntity* pEntity);

    // 消息通讯
    virtual bool onMessage(void *pEntityHead, int nMsgID, void *pData, size_t nLen);

    // 显示层的命令
    virtual bool onCommand(int nCmdID,void *pData,size_t nLen);

    virtual IEntity * getHost(){ return m_pEntity; }

    virtual void release();
    virtual void restore();
    ////////////////////IEventExecuteSink//////////////////////////////////////////////////////
    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ////////////////////WingPart//////////////////////////////////////////////////////
     virtual SWing* getWing()
     {
         return &m_Wing;
     }
public:
    WingPart();
    ~WingPart();

private:
    /** 客户端消息设置翅膀信息
    */
    void OnClientMessageSetWingInfo(void *pData, size_t nLen);

    /** 客户端消息更新翅膀信息
	*/
	void OnClientMessageUpdateWingInfo(void *pData, size_t nLen);

    /** 更新翅膀信息
	*/
	void UpdateWingInfo(void);

    /** 客户端消息更新飞行时间
	*/
	void OnClientMessageUpdateFlyTime(void *pData, size_t nLen);

    /** 是否在飞行中
    */
    bool IsFly();

private:
	IEntity*		m_pEntity;

    // 翅膀信息
    SWing           m_Wing;

    // 翅膀状态
    int             m_nWingState;
};

