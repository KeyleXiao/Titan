/*******************************************************************
** �ļ���:	TalentPageManager.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/8/16
** ��  ��:	1.0
** ��  ��:	�츳ҳϵͳ

********************************************************************/
#ifndef __TALENTPAGEMANAGER_H__
#define __TALENTPAGEMANAGER_H__

#include "IBankPart.h"
#include "EntityDef.h"
#include "IDBEngineService.h"
#include "ITalentPageManager.h"

class CPlayerBankPart;
class TalentEffectProxy;

#ifdef FUNCTION_TALENTPAGE_OPEN
// ����ϵͳ������
class CTalentPageManager : public ITalentPageManager, public IDBRetSink
{
public:
	/////////////////////////////////IDBRetSink/////////////////////////////////////////
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	//////////////////////////////////ITalentPageManager//////////////////////
	// ��ȡ��ǰ�츳ҳ
	int virtual GetCurTalentPage();

public:
	// ���캯��
	CTalentPageManager();
	// ��������
	virtual ~CTalentPageManager();

public:
	bool onSerialize(rkt::ibuffer& in);

    bool onSerializePage(rkt::ibuffer& in);


	bool deSerialize(rkt::obuf& out,int flag );

	bool deSerializePage(rkt::obuf& out);

	/** ����
	*/
	bool onLoad( __IEntity* pEntity, CPlayerBankPart *pBankPart, TalentEffectProxy* pEffectProxy);

	/** ���� 
	*/
	void restore();

    // �ͷ�
    void release(void);

    bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);

public:
	// �����ݿ����������츳��
	void RequestDataBaseAllTalentPageInfo();

	// ���õ�ǰѡ��ҳ��Ч��
	void SetTalentPageEffect();

private:
	//////////////////////////////////////�츳ҳ////////////////////////////////////
	// �ͻ������󱣴�ĳ���츳ҳ�ļӵ���Ϣ
	bool OnClientRequestSetTalentPageAddPointsInfo(void* pData, int nLen);
	// �ͻ�������ɾ����ǰѡ��ҳ
	void OnClientRequestDeleteTalentPage(int nCurTalentPage);

	// ���ݿ������츳ҳ�ļӵ�����
	void OnDBReturnTalentPagesPoints(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	// �����츳ҳ���ݵ��ͻ���
	void SendTalentPageInfoAllToClient(void * pData, int nOutLen);

private:
	__IEntity *                     m_pMaster;

	CPlayerBankPart *               m_pBankPart;

	TalentEffectProxy*              m_pEffectProxy;

	// �츳ҳ�ӵ������Ч������
	typedef map<int, TALENT_INFO_VECTOR> TALENTPAGE_MAP;
	TALENTPAGE_MAP                  m_mapTalentPage;

	int								m_nTalentEffectPageIndex;
};

#endif

#endif	// __TALENTPAGEMANAGER_H__