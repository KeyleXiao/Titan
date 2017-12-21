/*******************************************************************
** �ļ���:	IEntityEvent.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���� (Carl Chen)
** ��  ��:	2/26/2015
** ��  ��:	1.0
** ��  ��:	

			ʵ���¼��ӿ�
********************************************************************/


#pragma once

#include "Event_ManagedDef.h"
#include "IEventEngine.h"
using namespace rkt;

// �¼�ID
typedef ENTITYEVENT_KEY  EventKey;

// ʵ���¼�����
struct __IEntityEvent
{
	/** ����ͶƱ�¼�   
	@param   wEventID ���¼�ID
	@param   pszContext : ������
	@param   nLen : �����ĳ���	  
	*/
	virtual bool		FireVote(EventKey eventID, LPCSTR pszContext, int nLen) = 0;

	/** ����ִ���¼�
	@param   wEventID ���¼�ID
	@param   pszContext : ������
	@param   nLen : �����ĳ���	  
	*/
	virtual void		FireExecute(EventKey eventID, LPCSTR pszContext, int nLen) = 0;

	/** ����ȫ��ִ���¼�
	@param   wEventID ���¼�ID
	@param   pszContext : ������
	@param   nLen : �����ĳ���	  
	*/
	virtual void		FireExecute(GAMEEVENT_KEY eventID, LPCSTR pszContext, int nLen) = 0;

	/** ����ͶƱ�¼�ͨ����ִ���¼�
	@param   wEventID ���¼�ID
	@param   pszContext : ������
	@param   nLen : �����ĳ���	  
	*/
	virtual bool		FireVoteExecute(EventKey eventID, LPCSTR pszContext, int nLen) = 0;

	/** ����ͶƱ�¼�
	@param   pSink : �ص�sink
	@param   wEventID : �¼�ID
	@param	 pDesc : �������������������Գ���
	@return  �ɹ�����true,ʧ�ܷ���false
	*/
	virtual bool		Subscibe(IEventVoteSink * pSink,EventKey eventID, LPCSTR pszDesc) = 0;

	/** ����ִ���¼�
	@param   pSink : �ص�sink
	@param   wEventID : �¼�ID
	@param	 pDesc : �������������������Գ���
	@return  �ɹ�����true,ʧ�ܷ���false
	*/
	virtual bool		Subscibe(IEventExecuteSink * pSink,EventKey eventID, LPCSTR pszDesc) = 0;

	/** ȡ������ͶƱ�¼�
	@param   pSink : �ص�sink
	@param   wEventID : �¼�ID
	@return  �ɹ�����true,ʧ�ܷ���false
	*/
	virtual bool		UnSubscibe(IEventVoteSink * pSink,EventKey eventID) = 0;

	/** ȡ������ִ���¼�
	@param   pSink : �ص�sink
	@param   wEventID : �¼�ID
	@return  �ɹ�����true,ʧ�ܷ���false
	*/
	virtual bool		UnSubscibe(IEventExecuteSink * pSink,EventKey eventID) = 0;
};