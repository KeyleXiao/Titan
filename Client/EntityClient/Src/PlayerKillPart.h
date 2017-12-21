/*******************************************************************
** 文件名:	PlayerKillPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	8/05/2016
** 版  本:	1.0
** 描  述:	PK部件实现
********************************************************************/

#pragma once

#include "IPlayerKillPart.h"
#include "PlayerKillMessageDef.h"

class PlayerKillPart : public IPlayerKillPart
{
public:
    ////////////////////IEntityPart//////////////////////////////////////////////////////
    virtual bool onSerialize( UID uid, rkt::ibuffer & in,int flag );
    virtual bool deSerialize( rkt::obuf & out,int flag );

    virtual short  getPartID() { return PART_PK; }

    virtual bool onLoad( IEntity* pEntity);

    // 消息通讯
    virtual bool onMessage(void *pEntityHead, int nMsgID, void *pData, size_t nLen);

    // 显示层的命令
    virtual bool onCommand(int nCmdID,void *pData,size_t nLen);

    virtual IEntity * getHost(){ return m_pEntity; }

    virtual void release();

    virtual void restore();

    ////////////////////IPlayerKillPart//////////////////////////////////////////////////////
    // 是否和此目标进行PK
    virtual bool isWithPK(IEntity * pEntity);

    ////////////////////PlayerKillPart//////////////////////////////////////////////////////
public:
    PlayerKillPart();
    ~PlayerKillPart();

private:
    // 检测切磋
    bool checkPK(UID uidTarget);

    // 客户端消息请求PK
    void onClientMessageRequestPK(LPCSTR pData, size_t nLen);

    /** 客户端消息开始PK
    */
    void onClientMessageStartPK(void *pData, size_t nLen);

    /** 客户端消息结束PK
    */
    void onClientMessageEndPK(void *pData, size_t nLen);

    /** 客户端消息显示消息框
    */
    void onClientMessageShowMessageBox(void *pData, size_t nLen);

    // 设置请求PK
    void setRequestPK();

private:
	IEntity*		    m_pEntity;
    // 正在和此实体PK
    UID                 m_uidPKTarget;

    // 请求PK信息容器
    typedef vector<SRequestPKInfo>  REQUEST_PK_VECTOR;
    REQUEST_PK_VECTOR   m_RequestPKVector;
};

