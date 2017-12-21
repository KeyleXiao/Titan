/*******************************************************************
** �ļ���:	ClanSceneService.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	4/6/2016
** ��  ��:	1.0
** ��  ��:	��᳡������
********************************************************************/

#ifndef __CLANSCENESERVICE_H__
#define __CLANSCENESERVICE_H__

#include "IClanSceneService.h"
#include "NetMessageDef.h"
#include "Clan.h"
#include "IDBRetSink.h"
#include "ClanMessageDef.h"
#include "IMessageHandler.h"

class ClanScenService : public IClanSceneService, public ITransmitHandler, public IMessageHandler, public IEventExecuteSink, public IDBRetSink
{
public:
	// ���캯��
	ClanScenService();
	// ��������
	virtual ~ClanScenService();

	// _Stub::release() call
	void release();
	// _Stub::on_start call
	bool create();

    void onStop();

	/////////////////////////////////////////IClanSceneService/////////////////////////////////////////
	// ��ȡ�������
	virtual string getClanName(DWORD nClanID);

    // �����������������͹�������Ϣ
    virtual void    handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // ����ͻ��˷��͹�������Ϣ
    virtual void    handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

    virtual int getNumProp(DWORD dwClaneID, DWORD dwProp);

	virtual bool setNumProp(DWORD dwClanID, DWORD dwProp, int nValue);

	// ��鴴�����˱���
	virtual bool checkCreateClanLegendCup(DWORD dwClanID, int LegendCupConfigID);

	////////////////////////////////////////ITransmitHandler//////////////////////////////////////////
	// ��Ϣ����
	virtual void	onTransmit(DWORD server, ulong uMsgID, SNetMsgHead* head, void * data, size_t len);
	// ֪ͨ�������б��Ѿ�����
	virtual void	onServerListUpdated();
	// ֪ͨ��������Ϣ����
	virtual void	onServerInfoUpdated(DWORD ServerID, BYTE nState, void* pServerData);

	///////////////////////////////////////IMessageHandler/////////////////////////////////////////
	// ��Ϣ����
	virtual void	onMessage(ClientID clientID, ulong uMsgID, SNetMsgHead* head, void* data, size_t len);

    /////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    //////////////////////////////////IDBRetSink///////////////////////
    // ���ݿ����󷵻ػص�����
    virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

private:
    // ���ݰ��ID���Ұ��
    CClan* findClan(DWORD dwClanID);
    // ����ս��ID���Ұ���Ա
    SZoneClanMember* findClanKin(DWORD dwKinID);

    // ����DB�洢���̴������
	void onDBProcCreateClan(msg_clan_create_os *pInfo);
    // ���˰�����
    //@ note   : ��֪ͨ���ݿⴴ�����ɹ����أ����������ִ������������Ѿ�����������Ҫȡ����ǰ�Ĵ�������
    void backUpCreateClan(DWORD dwClanID, LPCSTR szTrackNote);

    // �����ķ��������			-- �������ݿ�
    void onReturnCenterCreateClan(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
    // �������					-- �������ݿ�
	void onReturnCreateClan(int nDBRetCode, char* pszDBRetDesc, int nQueueIndex, char* pOutData, int nOutLen);
	// ���������¼DB����
	void onReturnGetClanDonateInfo(int nDBRetCode, char *pszDBRetDesc, int nQueueIndex, char *pOutData, int nOutLen);

private:
    // �������
    void onTransmitCreateClan(LPCSTR pszMsg, int nLen);
    // ɾ�����
    void onTransmitRemoveClan(LPCSTR pszMsg, int nLen);
    // ���°�ᣨû������ӣ�
    void onTransmitUpdateClan(LPCSTR pszMsg, int nLen);
    // ���°������
    void onTransmitUpdateClanProp(LPCSTR pszMsg, int nLen);
    // ͬ������
    void onTransmitSynchroFinish(LPCSTR pszMsg, int nLen);
    // ɾ����Ա
    void onTransmitRemoveMember(LPCSTR pszMsg, int nLen);
    // ���³�Ա��û������ӣ�
    void onTransmitUpdateZoneMember(LPCSTR pszMsg, int nLen);
    // ������ҵİ��ID
    void onTransmitUpdateClanID(LPCSTR pszMsg, int nLen);
    // ת����Ϣ
    void onTransmitPostMsg(LPCSTR pszMsg, int nLen);
    // �������ǰ����Ϣ��ת
	void onTransmitUpdateSelfClanInfo(DWORD dwMsgID, LPCSTR pszMsg, int nLen);
	// ���;迨��Ϣ���ͻ���
	void onTransmitSendRequestCardInfo(LPCSTR pszMsg, int nLen);
    // ������֧Ԯ
    void onTransmitRequestCard(LPCSTR pszMsg, int nLen);
    // ��������
    void onTransmitDonateCard(LPCSTR pszMsg, int nLen);
    // ���Ӿ�������
    void onTransmitAddDonateCard(LPCSTR pszMsg, int nLen);
    // �������б�
    void onTransmitClanRequestCardList(LPCSTR pszMsg, int nLen);
    // ����������
    void onTransmitClanUpdateRequestCard(LPCSTR pszMsg, int nLen);

	// ��������
	// ���;迨��Ϣ���ͻ���
	void SendRequestCardInfo(PDBID nActorID);

private:
    map<DWORD, CClan*>          m_ClanList;				// ����б�
    map<DWORD, SZoneClanMember> m_ClanMemberList;		// ����Ա�б�

    // <����ֵ,���ID>
    multimap<int, DWORD, greater<DWORD> > m_mapPropValueClan;	// ĳ���԰�������б�  --����GetFirstPlaceActivityClanIDʹ��
    multimap<int, DWORD, greater<DWORD> >::iterator m_mapPropValueClanIter;

	struct SCreateClanInfo
	{
		PDBID nActorID;
		char szClanManifesto[CLAN_MANIFESTO_SIZE];
		int nFlag;

		SCreateClanInfo()
		{
			memset(this, 0, sizeof(*this));
		}
	};
    map<DWORD, SCreateClanInfo>			m_mapCrtClanPlayer;		// key(���봴���������)->value(�Ƿ񴴽�Ϊ��ʽ���)
};
#endif // __CLANSCENESERVICE_H__