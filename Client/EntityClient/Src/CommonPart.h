/*******************************************************************
** �ļ���:	CommonPart.h
** ��  Ȩ: (C) ���ڱ������缼�����޹�˾
** ������:	���к�
** ��  ��: 2017/1/22
** ��  ��: 1.0
** ��  ��: ͨ�ò���
** Ӧ  ��:  	
	
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
    // ���յ��������·�NavGroup����
    void onMessageSendNavGroup(void * data, size_t len);
    // ���յ��������·�WebUrl����
    void onMessageSendWebUrl(void * data, size_t len);
    // ����Url֪ͨ����ʾ��
    void processWebUrlToView(int nWebUrlID, const char* szUrl);
private:
	// ����ʵ��
	IEntity* m_pMaster;
};

