/*******************************************************************
** �ļ���:	LogoutPart.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	
** ��  ��:	2/26/2016
** ��  ��:	1.0
** ��  ��:	��ɫ�ǳ�������

********************************************************************/

#pragma once
#include "IDBEngine.h"
#include "ILogoutPart.h"

// ʵ��ǳ�����
class LogoutPart : public __ILogoutPart, public IDBRetSink
{

public:
    LogoutPart();

    ////////////////////* IEntityPart */////////////////////////////////////////////
    /** ȡ�ò���ID
    @return  
    */
    virtual short			getPartID();
    virtual bool			onLoad( __IEntity* pEntity, int nLoginMode);
    virtual bool			onMessage(void* pEntityHead, int msgId, void* data, size_t len);
    virtual __IEntity*		getHost();
    virtual void			release();

    virtual bool			onSerialize( rkt::ibuffer& in,int flag );
    virtual bool			deSerialize( rkt::obuf& out,int flag );
    
    // �л�Ӣ��
    virtual void            onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore) { }

    virtual int             getLogoutReason();

    virtual void            restore();

    ///////////////////////IDBRetSink////////////////////
    virtual void			OnReturn(IDBRetSink* pRealDBRetSink, int nCmdID, int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

private:
    bool                    doLogout(int nReason);

    void                    updatePlayerMapInfo(int nReason);

    // д�����ռ�
    void                    writeLogoutLog(int nReason);
private:
    __IEntity*			m_pMaster;
    bool                m_bLogouting;  // �Ƿ����ڴ�������

    int                 m_nLogoutReason;
};