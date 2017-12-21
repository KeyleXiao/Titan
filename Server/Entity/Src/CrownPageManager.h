/*******************************************************************
** �ļ���:	CrownPageManager.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	12/1/2016
** ��  ��:	1.0
** ��  ��:	�ʹ�ϵͳ

********************************************************************/
#ifndef __CROWNPAGEMANAGER_H__
#define __CROWNPAGEMANAGER_H__

#include "IBankPart.h"
#include "IShareServer.h"
#include "EntityDef.h"
#include "IDBEngineService.h"
class CPlayerBankPart;



// �ʹ�ϵͳ������
class CCrownPageManager: public IDBRetSink,public TimerHandler
{
    enum
    {
        ETIMERID_READ_CROWNDATA,
    };

    enum
    {
        READ_CROWNDATA_TIME = 500,
    };

public:
	// ���캯��
	CCrownPageManager();

	bool onSerialize(rkt::ibuffer& in);

	bool deSerialize(rkt::obuf& out);

	/** ����
	*/
	bool onLoad( __IEntity* pEntity, CPlayerBankPart *pBankPart);

	/** ���� 
	*/
	void restore();

    // �ͷ�
    void release(void);

	/////////////////////////////////////��ʯ�߼�/////////////////////////////////////
    // �Ƿ�ʯ����
    bool isFullForGemsone(void);
	// �ж����Ƿ�����ӻʹڱ�ʯ
	bool canAddCrownGemstone(int nGemstoneID,BYTE byGrade=1,int nCount = 1);
	// ��ӻʹڱ�ʯ����
	bool addCrownGemstone(int nGemstoneID,int nCount = 1, BYTE byGrade = 1, int nExp = 0, bool bAppraisal = false);
    // �Ƿ����Ƴ��ʹڱ�ʯ
    bool canDeleteCrownGemstone(int nGemstoneDBID);
    // �Ƴ��ʹڱ�ʯ
    bool deleteCrownGemstone(int nGemstoneDBID);

    //��ʯ����
    bool onMessageRequestSaleGemstone(LPCSTR pszMsg, size_t nLen);

    // ���俪��ʯ TODO ���ڹ��ܹر��� 
    void onMessageOpenGemstoneChest(int nChestType);
	// �ͻ���ѡ��ԭʯ����ʯ�齱�� TODO ���ڹ��ܹر���
	bool onMessageGemstoneSelectCobblestone(LPCSTR pszMsg, size_t nLen);
	// �ͻ��˻��ѵ�ȯ���� TODO ���ڹ��ܹر���
	bool onMessageGemstoneOpenWindow(LPCSTR pszMsg, size_t nLen);
	// �ͻ��˿������ѡ��ʯ TODO ���ڹ��ܹر���
	bool onMessageGemstoneWindowOpenToSelecet(LPCSTR pszMsg, size_t nLen);

    // �ͻ��˼���
    bool onMessageRequestAppraisalGemstone(LPCSTR pszMsg, size_t nLen);

	// ������Ƕ��ʯ�ľ�����ս����
	void addCrownInlayGemstoneExp( LONG lgValue );

private:
    // �������б�ʯ
    void readCrownGemstonesFromDatabase();
    // ����ʯ����
    void readCrownGemstoneProductFromDatabase();
    // ���ݿⷵ�����б�ʯ	
    void onDBReturnCrownAllGemstoneInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
    // ���ݿⷵ�ز���	
    void onDBReturnCrownGemstoneProductInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
    // ���浥����ʯ�����ݿ�
    void saveCrownGemstoneToDatabase(GEMSTONE_ITEM* pItem);
    // �����ݿ����Ƴ������ʹڱ�ʯ
    bool deleteCrownGemstoneFromDatabase(int nGemstoneDBID);

    // �����µı�ʯ������Ϣ
    void newGemstoneProduceDistribution();
    // ���²�����Ϣ�����ݿ�
    void saveGemstoneDistributionToDatabase();
	// ��ʯ�������
	bool decreaseGemstoneChest();
    // ���±�ʯ��ֵ������
    void updateCrownGemstoneFaceScoreAndPropValue(GEMSTONE_ITEM* pItem);

    // ���Ͳ���
    bool doCrownAppraisalGemstone(LPCSTR pszMsg, size_t nLen);
    // ���ͱ�ʯ
    bool canAppraisalCrownGemstone(ISchemeCrownGemstoneInfo *pCrownInfoService, int nMainStoneDBID, int nAssistantStoneDBID, int& nCostMoney, int& nCostTicket);
    // ���ͱ�ʯ
    bool appraisalCrownGemstone(ISchemeCrownGemstoneInfo *pCrownInfoService, int nMainStoneDBID);
	// ������־
	void writeGemstoneAppraisalLog( LPCSTR pszMsg, size_t nLen, bool bAppraisalSucess );
    // Ƶ���㲥
    bool broadCastGemstoneLevelUpInChatChannel(int nGemstoneGrade);

    // ���ӱ�ʯ����
    bool addCrownGemstoneExp(int nGemstoneDBID, int nExp);
    // ���ӱ�ʯ����
    bool canAddCrownGemstoneExp(int nGemstoneDBID, int nExp);

    // ���±�ʯ���ͻ���
    void sendCrownGemstoneToClient(GEMSTONE_ITEM* pItem);
    // �������б�ʯ���ͻ���
    void sendCrownGemstonesToClient();
    // ���²�����Ϣ���ͻ���
    void sendGemstoneDistributionToClient();
    // ��ʯ����������ͻ���
    void sendGemstoneAppraisalRetToClient(int nGemstoneDBID, int nCritNum, bool bAppraisalRet);

    // ��ñ�ʯ
    GEMSTONE_ITEM* getGemstonePossessItem(int nGemstoneDBID);
	// ������ʯdbid
	int  getGemstoneDBMaxIndex();
    // ��ñ�ʯӵ������
    int  getGemstoneCount();
    // ��ñ�ʯ��λ��
    int getGemstoneListEmptyIndex(void);
    // ��ʯ���� 
    void sortCrownGemstoneList();

	/////////////////////////////////////�ʹ��߼�/////////////////////////////////////
public:
    // �ܷ�����ҳ��
    bool canAddPage(DWORD nNum=1);
    // ������ҳ��
    bool createNewPage(DWORD nNum=1);
	// �ͻ������� ����ʹ�ҳ
	bool onMessageRequestSaveCrownPage(LPCSTR pszMsg, size_t nLen);
	// �ͻ������� ɾ���ʹ�ҳ
	bool onMessageRequestDeleteCrownPage(LPCSTR pszMsg, size_t nLen);
	// �ͻ������� �л��ʹ�ҳ
	bool onMessageRequestSwitchCrownPage(int nPageDBID);
    // �ͻ������� ����������лʹ�ҳ����
    void onMessageRequestOtherPlayerCrownPages(void * data, size_t nLen);

	// ��ȡ������ҵ�ǰ�ʹ�ҳ����
	virtual bool getOtherCrownPageData(obuf & outData);
	// ��ȡ������ҵ�ǰ��ʯ����
	bool getOtherCrownGemstoneData(obuf & outData);
	// ս������Ч�� timeToAddEffect:ETIME_TO_ADD_EFFECT
	void setCrownEffect(int timeToAddEffect);

private:
    // �Զ�����Ĭ��ҳ
    void autoCreateDefaultPage();
    // ����һҳ
    int innerCreateOnePage();
    // ���ݿ��ж�ȡ�Լ����лʹ�ҳ����,�ɶ�ʱ��������ȡ
    void readCrownPagesFromDatabase();
	// ���ݿⷵ�� �����Լ��Ļʹ�ҳ����
	void onReturnReadCrownPages(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
    // ���ݿⷵ�� ���»ʹ�ҳ����
    void onReturnUpdateCrownPage(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
    // ���ݿⷵ�� ɾ���ʹ�ҳ����
    void onReturnDeleteCrownPage(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

    // ����Ч��
    bool deSerializeCrownPropertyList(rkt::obuf& out);
    // �����Լ��ʹ���Ƕ������ͻ���
    void sendCrownCrownPagesToClient();

	// ��ûʹ�ҳ
	CROWN_PAGE_ITEM* getGemstoneCrownPageItem(int nPageDBID);
	// ��ûʹ����ҳdbid
	int  getCrownDBPageMaxIndex();
	// ��ûʹ�ҳ����
	int  getCrownPageCount();
	// ��ûʹ�ҳ��λ��
    int getCrownPageListEmptyIndex(void);
    // �ʹ�ҳ���� 
    void sortCrownPageList();

	///////////////////////////////// IDBRetSink ////////////////////////////
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
	/////////////////////////////////TimerHandler/////////////////////////////////////////
	virtual void OnTimer( unsigned long dwTimerID );

	// Ŀǰû�����ݿ����� �Ȳ�����
	//void doTest();


private:
    // ��ǰӵ��ҳ������
    BYTE                            m_nPageCount;
    // �ʹ�ҳ���ݿ��м�¼�����������ֵ
    int				                m_nDBPageMaxIndex;
    // ��ǰ�ʹ�ҳ
    int                             m_nCurrentCrownPage;

    // ��ʯ���ݿ��м�¼�����������ֵ
    int				                m_nDBGemstoneMaxIndex;
	// ÿҳ��Ƕ��ϸ��Ϣ
	CROWN_PAGE_ITEM                 m_crownPageList[CROWN_PAGE_MAX_COUNT];    
	// ӵ�б�ʯ��Ϣ
	GEMSTONE_ITEM                   m_crownGemstoneList[CROWN_GEMSTONE_MAX_COUNT];

	// ��ʯ������Ϣ
	GEMSTONE_PRODUCE_DISTRIBUTION   m_gemstoneProduce;

	// �ʹ������б�
	int			                    m_crownPropertyList[CROWN_PROPERTY_COUNT_MAX];
	// ����id �������
	int				                m_nGemstoneChestID;

	__IEntity *                     m_pMaster;

	CPlayerBankPart *               m_pBankPart;
};

#endif	// __CrownPageMANAGER_H__