/*******************************************************************
** �ļ���:	E:\speed\Server\LegendCupServer\Src\LegendCupSceneService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	���а�
********************************************************************/

#pragma once
#include "ILegendCupSceneService.h"
#include "NetMessageDef.h"
#include "IMessageHandler.h"
#include "IMessageDispatch.h"
#include "IDBRetSink.h"

class LegendCupScenService:public ILegendCupSceneService, public ITransmitHandler,public IMessageHandler, public IDBRetSink
{

public:

	LegendCupScenService();
     //
	////////////////////////////////////////ITransmitHandler//////////////////////////////////
	/** ��Ϣ����
	@param server	Դ������ID
	@param uMsgID	��ϢID
	@param head		��Ϣͷ
	@param data		��Ϣ
	@param len		��Ϣ����
	*/
	virtual void	onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);

	/**
	@name         : ֪ͨ�������б��Ѿ�����
	@brief        : �������б����ֻ�����з����������˳�ʱ����
	@note         : ������б����ICenterServerConnector::GetServerList���
	*/
	virtual void	onServerListUpdated();

	/** ֪ͨ��������Ϣ����
	@param nState		: SERVER_STATE
	@param pServerData	: ServerData
	@brief        : ��������������仯�ȵ�
	*/
	virtual void	onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData);

	// 
	///////////////////////////////////IMessageHandler///////////////////////////////////////
	/** ��Ϣ����
	@param msg ������Ϣ
	*/
	virtual void	onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

    ///////////////////////////////////ILegendCupSceneService///////////////////////////////////////
    // �����������������͹�������Ϣ
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, void* pData, size_t nLen);

    // ����ͻ��˷��͹�������Ϣ
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, void* pData, size_t nLen);

    //////////////////////////////////IDBRetSink///////////////////////
    // ���ݿ����󷵻ػص�����
    virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);


	// LegendCupSceneServer
	//////////////////////////////////////////////////////////////////////////
	// _Stub::release() call
	void release();
	// _Stub::on_start call
	bool create();
private:
    
    // purpose: ���ɱ���ID
    LONGLONG createDBLegendCupId(int nWorld, int nServerID );

    // purpose: �����������Ϣ
    void	onMsgCreateCup( ClientID clientID, void* pData, size_t stLen );

    // purpose: �������󴴽���������
    void	onMsgCreateCupType( ClientID clientID, void* pData, size_t stLen );

    // purpose: �����������Ϣ
    void	onMsgReqJoinLegendcupRoom( ClientID clientID, void* pData, size_t stLen );
    
    // purpose: ��������				-- �������ݿ�
	void    onReturnCreateLegendCup(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);

    //  purpose: У�鴴�������ɹ�
    void	onMsgCreateCupResult( void* pData, size_t nLen );

    // purpose: ������뱭���۳����
    void	onMsgReqJoinCup( void* pData, size_t nLen );

private:
	unsigned short m_u16ServerID;
	volatile	LONG  m_mutex;
};