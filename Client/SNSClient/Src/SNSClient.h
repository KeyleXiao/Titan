/*******************************************************************
** �ļ���:	SNSClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	���к�
** ��  ��:	12/7/2016
** ��  ��:	1.0
** ��  ��:	SNSϵͳ�ͻ���
** Ӧ  ��:  	
	
*******************************************************************/

#pragma once
#include "ISNSClient.h"
#include "IEntityClient.h"

class SNSClient : public ISNSClient, public IMessageHandler, public IGameViewEventHandler
{
public:
	// ���캯��
	SNSClient();
	// ��������
	virtual ~SNSClient();

	// ISNSClient
	// �ͷ�
	virtual void release(void);

	// IMessageHandler
	virtual void onMessage(SNetMsgHead* head, void* data, size_t len);

	// IGameViewEventHandler
	virtual bool onViewEvent( int eventID, int nParam, const char *strParam, void *ptrParam );

public:
	// SNSClient
	bool Create(void);

protected:
	// �����Ϣͷ
	void fillSNSData(obuf& ob, int destEndPoint, BYTE msgId);

	void sendMsgToSocial(BYTE byActionID, const char *strParam = NULL, int strLen = 0, void* pData = NULL, int nLen = 0);

private:
	// ��ȡ��λ����λͼ��
	int getRankGradeIcon(int nRankGrade);
	// ��ȡ��λ����λ����
	string getRankGradeName(int nRankGrade);
private:

};

extern SNSClient* g_pSNSClient;