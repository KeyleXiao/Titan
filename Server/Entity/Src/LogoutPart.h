/*******************************************************************
** 文件名:	LogoutPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	2/26/2016
** 版  本:	1.0
** 描  述:	角色登出、存盘

********************************************************************/

#pragma once
#include "IDBEngine.h"
#include "ILogoutPart.h"

// 实体登出部件
class LogoutPart : public __ILogoutPart, public IDBRetSink
{

public:
    LogoutPart();

    ////////////////////* IEntityPart */////////////////////////////////////////////
    /** 取得部件ID
    @return  
    */
    virtual short			getPartID();
    virtual bool			onLoad( __IEntity* pEntity, int nLoginMode);
    virtual bool			onMessage(void* pEntityHead, int msgId, void* data, size_t len);
    virtual __IEntity*		getHost();
    virtual void			release();

    virtual bool			onSerialize( rkt::ibuffer& in,int flag );
    virtual bool			deSerialize( rkt::obuf& out,int flag );
    
    // 切换英雄
    virtual void            onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore) { }

    virtual int             getLogoutReason();

    virtual void            restore();

    ///////////////////////IDBRetSink////////////////////
    virtual void			OnReturn(IDBRetSink* pRealDBRetSink, int nCmdID, int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

private:
    bool                    doLogout(int nReason);

    void                    updatePlayerMapInfo(int nReason);

    // 写下线日记
    void                    writeLogoutLog(int nReason);
private:
    __IEntity*			m_pMaster;
    bool                m_bLogouting;  // 是否正在处理下线

    int                 m_nLogoutReason;
};