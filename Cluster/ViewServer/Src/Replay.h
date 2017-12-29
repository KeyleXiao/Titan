#pragma once
#include "ViewRedisData.h"
#include "LoadBattleInfoTask.h"
#include "ViewNode.h"
#include "ViewMsgDef_Client.h"
#include "STime.h"


struct ClientUser;
struct ViewParam;
using ViewRedisData::BattleInfo;
using ViewRedisData::BattlePrepInfo;

class Replay
{
	friend struct LoadPrepInfoTask;
	friend struct LoadBattleInfoTask;
public:
	Replay()
		: m_rID(0)
		, m_lReaded(0)
		, m_bStart(false)
		, m_bFinished(false)
		, m_nRefCount(0)
		, m_pTask(nullptr)
	{}

	~Replay()
	{
		safeDelete(m_pTask);
	}

	bool Start();
	// ����һ����ս��
	void Insert(ClientUser* pClientUser, const ViewParam& sViewParam);
	// �Ƴ���ս��
	void Remove(PlayerID dwPlayerID);
	// ÿ��N���룬��ReplayContainer������
	void HeartBeat();

	// ���ü��� +1
	void IncRef() { m_nRefCount += 1; }
	// ���ü��� -1
	void DecRef() { m_nRefCount -= 1; }
	// ���ü����Ƿ�Ϊ0
	bool IsRefEmpty() { return m_nRefCount == 0; }

private:
	// ����ս�������й�ս��
	void BroadCastBattleInfo();

public:
	ReplayID	m_rID;		// ս��ID

private:
	typedef	std::vector<SMsgView_VC_BattleInfo>	Vec_VC_BattleInfo;

	string					m_prepKey;		// Ԥ��key
	SMsgView_VC_BattlePrep	m_battlePrepMsg;// Ԥ����Ϣ
	STime					m_timeBegin;	// ��ʼʱ��
	STime					m_timeEnd;		// ����ʱ��

	string					m_strKey;		// ս����Ӧ��Key
	Vec_VC_BattleInfo		m_vecBattleInfo;// ս����Ϣvector

	LoadBattleInfoTask*		m_pTask;	// ѭ��ʹ�õĶ�ȡ����
	long					m_lReaded;	// �Ѿ��������Ŀ

	int			m_nRefCount;	// ���ü���������Ϊ0���˶�����ܱ��Ƴ�����
	bool		m_bStart;		// ս���Ƿ��Ѿ�����(����Ϊtrueʱ����Ҳſɶ�ȡս����Ϣ)
	bool		m_bFinished;	// ս���Ƿ��Ѿ������������ˣ��Ͳ�����ȥ��ս���ˣ�

	typedef std::map<PlayerID, ViewNode>	UserMap;
	UserMap		m_UserMap;	// ��ս��map
	
};