/*******************************************************************
** �ļ���:	AntiAddictClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	11/15/2016
** ��  ��:	1.0
** ��  ��:	

			�����Կͻ���ģ��: �����������صĿͻ����߼�
********************************************************************/

#pragma once

#include "IMessageDispatch.h"
#include "IAntiAddictClient.h"
#include "IEntityClient.h"

class CAntiAddictClient : public IAntiAddictClient, public IMessageHandler, public IGameViewEventHandler
{
public:
	////////////////////////////////////IGameViewEventHandler///////////////////////////////////////
	/** 
	@name   : ��ʾ���¼�������
	@param
	@param   
	@return  
	*/
	virtual bool		onViewEvent( int eventID,int nParam,const char * strParam,void * ptrParam );


	///////////////////////////////////////IMessageHandler//////////////////////////////////////////
	/** 
	@name   : ��Ϣ����
	@param
	@param   
	@return  
	*/
	virtual void		onMessage( SNetMsgHead *head, void *data, size_t len );


	//////////////////////////////////////IAntiAddictClient/////////////////////////////////////////
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


	//////////////////////////////////////CAntiAddictClient/////////////////////////////////////////
	/** 
	@name   : ����
	@param
	@param   
	@return  
	*/
	CAntiAddictClient(void);

	/** 
	@name   : ����
	@param
	@param   
	@return  
	*/
	virtual ~CAntiAddictClient(void);

private:
	/** 
	@name   : ��������֤
	@param
	@param   
	@return  
	*/
	void			AntiAddictAuth(void * ptrParam);

	/** 
	@name   : ��������֤���
	@param
	@param   
	@return  
	*/
	void			OnMsgAuthResult(void *data, size_t len );

	/** 
	@name   : ��������Ϣ����
	@param
	@param   
	@return  
	*/
	void			OnMsgAddictTip(void *data, size_t len );
};
extern CAntiAddictClient *			g_pAntiAddictClient;
