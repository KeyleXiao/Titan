/*******************************************************************
** �ļ���:	ClientContainer.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/12/06
** ��  ��:	1.0
** ��  ��:	��ս�ͻ�������
** Ӧ  ��:  ������������ÿͻ��˹�ս������
			������Ժ�����������ͻ��˹�ս�����󣬿����ٿ�һ��Container����
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "UserContainer.h"
#include "ClientSession.h"


class ClientContainer : public UserContainer<ClientSession, PlayerID>, public Singleton<ClientContainer>
{
public:
	ClientContainer();
	virtual ~ClientContainer();

	inline void BroadCastData(obuf& obufData, const VecPlayerID& vec)
	{
		BroadCastData((const obuf&)obufData, vec);
	}
	void BroadCastData(const obuf& obufData, const VecPlayerID& vec);

	template<typename TMsg>
	void BroadCastData(TMsg& msg, const VecPlayerID& vec)
	{
		obuf obufData;
		TBuildObufMsg(obufData, msg);

		for (size_t i = 0; i < vec.size(); i++)
		{
			const PlayerID& dwPlayerID = vec.at(i);
			auto pClient= this->Get(dwPlayerID);
			if (pClient==nullptr)
				continue;
			 
			pClient->SendData(obufData.data(), obufData.size());
		}
	}

protected:
	virtual void OnDelUser(ISessionUser<PlayerID>* pUser);

	//ע������Ҫ�����������Ϣ����m_Processer��
	virtual void RegisterHandlers();

	virtual PlayerID GetMaxID();

};
extern ClientContainer& gClientContainer;