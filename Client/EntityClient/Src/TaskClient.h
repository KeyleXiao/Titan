/*******************************************************************
** �ļ���:	TaskClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	7/12/2016
** ��  ��:	1.0
** ��  ��:	����ͻ���
********************************************************************/

#pragma once

#include "ITaskClient.h"
#include "IMessageDispatch.h"
#include "IEntityClient.h"

class CTaskClient : public ITaskClient, public IMessageHandler, public IGameViewEventHandler
{
public:
	////////////////////////////////////IGameViewEventHandler///////////////////////////////////////
	/** 
	@name   : ��ʾ���¼�������
	@param
	@param   
	@return  
	*/
	virtual bool		onViewEvent(int eventID, int nParam, const char *strParam, void *ptrParam);


	///////////////////////////////////////IMessageHandler//////////////////////////////////////////
	/** 
	@name   : ��Ϣ����
	@param
	@param   
	@return  
	*/
	virtual void		onMessage(SNetMsgHead *head, void *pData, size_t nLen);


	//////////////////////////////////////ITaskClient/////////////////////////////////////////
	/** 
	@name   : ���������Է���
	@param
	@param   
	@return  
	*/
	virtual bool Create();

	/** 
	@name   : �ͷŷ����Է���
	@param
	@param   
	@return  
	*/
	virtual bool Release();


	//////////////////////////////////////CTaskClient/////////////////////////////////////////
	/** 
	@name   : ����
	@param
	@param   
	@return  
	*/
	CTaskClient(void);

	/** 
	@name   : ����
	@param
	@param   
	@return  
	*/
	virtual ~CTaskClient(void);

private:
	// �ְ���������ظ�
	obuf	m_contactReplyInfo;
};
extern CTaskClient *			g_pTaskClient;
