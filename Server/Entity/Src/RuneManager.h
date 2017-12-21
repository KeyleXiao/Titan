/*******************************************************************
** �ļ���:	RuneManager.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��绪
** ��  ��:	8/16/2017
** ��  ��:	1.0
** ��  ��:	����ϵͳ

********************************************************************/
#ifndef __RUNEMANAGER_H__
#define __RUNEMANAGER_H__

#include "IBankPart.h"
#include "EntityDef.h"
#include "IDBEngineService.h"
#include "IRuneManager.h"

class CPlayerBankPart;


#ifdef FUNCTION_RUNE_OPEN
// ����ϵͳ������
class CRuneManager : public IRuneManager, public IDBRetSink
{
public:
	/////////////////////////////////IDBRetSink/////////////////////////////////////////
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

public:
	///////////////////////////IRuneManager//////////////////////////
    // ���ӵ�з�������
    virtual bool addRuneStoreInfo(int nRuneId,int nCount = 1);
    // �ж����Ƿ�����ӷ���
    virtual bool canAddRuneStoreInfo(int nRuneId,RUNE_BUY_ERROR_TYPE& eErrorType,int nCount = 1);
    // ��ȡ��ǰ��������
    virtual bool getOtherRuneData(obuf & outData);
    // ��ȡ��ǰ����ҳ
    virtual int getCurRunePage();

	// ���캯��
	CRuneManager();
	// ��������
	virtual ~CRuneManager();

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

    size_t getStore(rkt::obuf& out);

    void setRuneTalentPageIndex(int nTalentEffectPageIndex);
    ////////////////////////////////////// ���� ////////////////////////////////////
    // �ͻ�������ͬ��������Ϣ
    void requestDataBaseRuneInlayInfo();
    // �ͻ������������˵ķ�����Ϣ
    void onClientRequestOtherSyncRuneInfo(void * pData, size_t nLen);
    // ����ͻ������󱣴����ҳ����
    bool onClientRequestSaveRunePage(LPCSTR pszMsg, size_t nLen);
    // ����ͻ�������ɾ������ҳ����
    bool onClientRequestDeleteRunePage(void * data, size_t len);
    // ���ݿⷵ�ط�������
    void onDBReturnRuneInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
    // ���÷���buff��Ч����Ч
    bool setRuneEffect();
    // ���ͻ��˷��������˵ķ�����Ϣ
    void sendOtherRuneInfoToClient(void *pOutData,int nOutLen );
    // ���ͳ�ʼ����ҳ����Ϣ���ͻ���
    void sendRunePageInfoToClient();

    // �����̵꣺�Լ�ӵ�еķ���
    // ���¼���ķ��ĵ��ͻ���
    void updateActiveRuneStoreItemToClient(int nRuneId,int bActive);	

    bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);
private:
	__IEntity *                     m_pMaster;

	CPlayerBankPart *               m_pBankPart;

	TalentEffectProxy*              m_pEffectProxy;


    /////////////////////////////////// Rune ����ϵͳ/////////////////////////////////////
    // ����ҷ���ҳ�з��ĵ����,һ����������20ҳ����
    typedef std::map<int, Entity_Rune_Page_Info>         RUNE_PAGE_MAP;
    RUNE_PAGE_MAP                           m_mapRunePage;
    // ��ǰ����ҳ��
    int                                     m_nCurRunePageIdx;                             

    // ���Ŀ� <����ID�� struct Rune>

    RUNESTORE_MAP                           m_mapRuneStore;
};

#endif 
#endif	// __RUNEMANAGER_H__