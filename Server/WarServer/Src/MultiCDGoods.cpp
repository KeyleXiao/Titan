#include "MultiCDGoods.h"


CMultiCDGoods::CMultiCDGoods(void)
{
    m_dwChargeFreezeTick=0;
    m_eCDState = State_UseInterval;
    m_nLeftChargeFreezeTime=0;
    m_MultipleOverlayCountLimit=0;
    m_nUseInterval=0;
    m_bIsExistLeftChargedTime=true;    
    m_bMultipleResetUseCount = false;
    m_pMaster = NULL;
    m_pMobaGoods = NULL;
    m_nNeedSubFreezeTime = 0;
}


CMultiCDGoods::~CMultiCDGoods(void)
{
}

bool CMultiCDGoods::canUse(void)
{
    if(m_eCDState == State_UseInterval && m_nLeftChargeFreezeTime > 0)
    {
        return false;
    }
    return true;
}

void CMultiCDGoods::release()
{
    ITimerService * pTimerService = gServerGlobal->getTimerService();
    if(pTimerService)
    {
        pTimerService->KillTimer(ETimerEventID_GoodsMultipleChargedFreeze,this);
        pTimerService->KillTimer(ETimerEventID_GoodsMultipleIntervalFreeze,this);
    }

    m_dwChargeFreezeTick=0;
    m_eCDState=State_UseInterval;
    m_nLeftChargeFreezeTime=0;
    m_MultipleOverlayCountLimit=0;
    m_nUseInterval=0;
    m_bIsExistLeftChargedTime=true;    
    m_bMultipleResetUseCount = false;
    m_pMaster = NULL;
    m_pMobaGoods = NULL;
    delete this;
}

bool CMultiCDGoods::inital(__IEntity* pEntity, IMobaGoods* pMobaGood,int nInterval,int OverlayCountLimit,bool bResetUseCount, int bInitalUseCount)
{
    if (pEntity == NULL || pMobaGood == NULL)
    {
        ErrorLn(__FUNCTION__"-- CMultiCDGoods::inital()! pEntity == NULL || pMobaGood == NULL");
        return false;
    }
    m_pMaster = pEntity;
    m_pMobaGoods = pMobaGood;

    IFreezeServer *pFreezeServer = gServerGlobal->getFreezeServer();
    if (pFreezeServer == NULL)
    {
        ErrorLn(__FUNCTION__" -- pFreezeServer == NULL");
        return false;
    }
    SSchemeMobaGoods *pSchemeMobaGoods = m_pMobaGoods->GetSchemeInfo();
    if (pSchemeMobaGoods == NULL)
    {
        ErrorLn(__FUNCTION__" -- pSchemeMobaGoods == NULL SlotIndex ="<<m_pMobaGoods->GetSlotIndex()<<" GoodID = "<<m_pMobaGoods->GetGoodID());
        return false;
    }

    // ��ȡ��Ʒ����
    __IMobaGoodsPart *pMobaGoodsPart = (__IMobaGoodsPart *)m_pMaster->getEntityPart(PART_MOBAGOODS);
    if (pMobaGoodsPart == NULL)
    {
        return false;
    }
    m_eCDState = State_UseInterval;
    m_nLeftChargeFreezeTime = pFreezeServer->GetSchemeFreezeTime(EFreeze_ClassID_Goods, pSchemeMobaGoods->nUseBuffFreezeID);
    // ����ǰ��¼��ʣ��ʱ��
    SEffectCDOverlay cdData = pMobaGoodsPart->getCDOverlayInfo(pSchemeMobaGoods->nUseBuffFreezeID);
    pMobaGoodsPart->removeCDOverlayInfo(pSchemeMobaGoods->nUseBuffFreezeID);
    if (cdData.nLeftCDTime > 0)
    {
        m_bIsExistLeftChargedTime = true;
        m_nLeftChargeFreezeTime = cdData.nLeftCDTime;
    }
    // ����ǰ�ļ�¼ ����ǰ�ļ�¼��ʼ��
    if (cdData.nLeftCanUseCount > 0)
    {
        m_eCDState = State_Charge;
        m_pMobaGoods->SetCanUseCount(cdData.nLeftCanUseCount);
    }
    else if (bInitalUseCount > 0)
    {
        m_eCDState = State_Charge;
        m_pMobaGoods->SetCanUseCount(bInitalUseCount);

    }

    // ʹ�ü���ͳ��ܱ�־λ��������true����������ʹ�ã������false����������ʹ��
    m_nUseInterval = nInterval;
    m_MultipleOverlayCountLimit = OverlayCountLimit;
    m_bMultipleResetUseCount = bResetUseCount;

    if (m_pMobaGoods->GetCanUseCount() < OverlayCountLimit)
    {
        gServerGlobal->getTimerService()->SetTimer(ETimerEventID_GoodsMultipleChargedFreeze, m_nLeftChargeFreezeTime, this,INFINITY_CALL,__FUNCTION__);
        // ������ȴ�����ÿ�ʼ��ȴtick��
        __IFreezePart *pFreezePart = (__IFreezePart *)m_pMaster->getEntityPart(PART_FREEZE);
        if (pFreezePart == NULL)
        {
            return false;
        }
        pFreezePart->SetFreezeTime(EFreeze_ClassID_Goods, pSchemeMobaGoods->nUseBuffFreezeID, m_nLeftChargeFreezeTime, true);
        // �����³�����ȴtick
        m_dwChargeFreezeTick = getTickCountEx();
    }

    return true;
}

void CMultiCDGoods::AddFreezeTime(EFREEZE_CLASS ecFreezeClass, int nFreezeID)
{
    if (m_pMaster == NULL)
    {
        return;
    }
    __IFreezePart *pFreezePart = (__IFreezePart *)m_pMaster->getEntityPart(PART_FREEZE);
    if (pFreezePart == NULL)
    {
        return;
    }
    IFreezeServer *pFreezeServer = gServerGlobal->getFreezeServer();
    if (pFreezeServer == NULL)
    {
        return;
    }
   
    if(m_eCDState == State_UseInterval)
	{
		// �Ƕ�����ȴ��ȡ������ȴʱ�䣬һ����ȴ
		int nFreezeTime = pFreezeServer->GetSchemeFreezeTime(ecFreezeClass, nFreezeID);
		// ������ȴʱ��
		pFreezePart->SetFreezeTime(ecFreezeClass, nFreezeID, nFreezeTime, true);
        // �����³�����ȴtick
        m_dwChargeFreezeTick = getTickCountEx();
	}
	else
    {
        // ���أ�ʹ��֮������ȴʱ��
        int nFreezeTime = getMultipleFreezeIntervalTime();
        // ��ʱʱ������Ϊ0����������
        if(nFreezeTime==0)
        {
            return;
        }
        // ʹ��֮��������һ��ʹ�ü����ʱ��
        gServerGlobal->getTimerService()->SetTimer(ETimerEventID_GoodsMultipleIntervalFreeze,nFreezeTime,this,1, __FUNCTION__);

        ErrorLn(__FUNCTION__ << "nFreezeTime="<< nFreezeTime);
        bool bBreak = nFreezeTime == m_nUseInterval;
        pFreezePart->SetFreezeTime(ecFreezeClass, nFreezeID, nFreezeTime,true, bBreak);
    }
}

void CMultiCDGoods::OnTimer(unsigned long dwTimerID)
{
    switch (dwTimerID)
    {
    case ETimerEventID_GoodsMultipleChargedFreeze:
        {
            // ������ȴ�����߼����ܶ�ʱ��
            OnGoodsMultipleCDTimeTimer();
        }
        break;
    case ETimerEventID_GoodsMultipleIntervalFreeze:
        {
            // ������ȴʹ�ü���߼����ܶ�ʱ��
            OnGoodsMultipleIntervalTimeTimer();
        }
        break;
    default:
        break;
    }
}

// �趨�����ʣ�������ȴ�ͳ��������ȴ
void CMultiCDGoods::OnGoodsMultipleIntervalTimeTimer() 
{
    if(m_pMaster==NULL || m_pMobaGoods == NULL)
    {
        return;
    }
    __IFreezePart *pFreezePart = (__IFreezePart *)m_pMaster->getEntityPart(PART_FREEZE);
    if (pFreezePart == NULL)
    {
        return;
    }
    // �����ȴ��� ��������
    m_eCDState=State_Charge;
    int nBuffFreezeID= m_pMobaGoods->GetSchemeInfo()->nUseBuffFreezeID;
    // ������ȴ��ʼ�����ڹ���ʱ��
    int nPassTime = getTickCountEx() - m_dwChargeFreezeTick;
    // ��ȴʣ��ʱ��
    m_nLeftChargeFreezeTime = m_nLeftChargeFreezeTime - nPassTime;
    if(m_bIsExistLeftChargedTime)
    {
        // �г��ܵ�ʣ��ʱ��
        pFreezePart->SetFreezeTime(EFreeze_ClassID_Goods, nBuffFreezeID, m_nLeftChargeFreezeTime, true, false);
        m_dwChargeFreezeTick = getTickCountEx();
        
    }
    else
    {
        // ������ȴ��ʣ��ʱ��Ϊ��
        if(m_nLeftChargeFreezeTime < 0)
        {
            // �Ѿ��ǳ�����ȴ�� Ҫ����ȴ��ʱ�䲹����һ�εĳ�����ȴ
            m_nNeedSubFreezeTime =m_nLeftChargeFreezeTime;
            m_nLeftChargeFreezeTime = 0;
        }

        pFreezePart->SetFreezeTime(EFreeze_ClassID_Goods, nBuffFreezeID, m_nLeftChargeFreezeTime, true, false);
        // ����������ܶ�ʱ��
        gServerGlobal->getTimerService()->SetTimer(ETimerEventID_GoodsMultipleChargedFreeze, m_nLeftChargeFreezeTime, this, 1, __FUNCTION__);
        m_dwChargeFreezeTick = getTickCountEx();
    }

}

void CMultiCDGoods::OnGoodsMultipleCDTimeTimer()
{
    // ���ܳɹ�
    if(m_pMaster==NULL)
    {
        return;
    }
    int nBuffFreezeID = m_pMobaGoods->GetSchemeInfo()->nUseBuffFreezeID;
    int nCanUseCount = m_pMobaGoods->GetCanUseCount();
    m_pMobaGoods->SetCanUseCount(++nCanUseCount);
    m_eCDState = State_Charge;
    
    if (m_pMobaGoods->GetCanUseCount() >= m_MultipleOverlayCountLimit)
    {
        // ������ȴ���� �Ѿ����� ��رն�ʱ��
        gServerGlobal->getTimerService()->KillTimer(ETimerEventID_GoodsMultipleChargedFreeze,this);
        m_eCDState = State_Full;
    }
    else
    {
        // ������ȴ���� û�г���
        __IFreezePart *pFreezePart = (__IFreezePart *)m_pMaster->getEntityPart(PART_FREEZE);
        if (pFreezePart == NULL)
        {
            return;
        }

        IFreezeServer *pFreezeServer = gServerGlobal->getFreezeServer();
        if (pFreezeServer == NULL)
        {
            return;
        }
        
        m_nLeftChargeFreezeTime = pFreezeServer->GetSchemeFreezeTime(EFreeze_ClassID_Goods, nBuffFreezeID);
        // ����ʣ��ĳ���ʱ��
        if(!m_bIsExistLeftChargedTime)
        {   
            // ������ʱ�� ��Ϊʹ����ȴ����ʣ��ĳ�����ȴʱ�� ����Ҫ����
            if (m_nNeedSubFreezeTime < 0)
            {
                m_nLeftChargeFreezeTime += m_nNeedSubFreezeTime;
                m_nNeedSubFreezeTime = 0;
            }
            gServerGlobal->getTimerService()->SetTimer(ETimerEventID_GoodsMultipleChargedFreeze, m_nLeftChargeFreezeTime, this, INFINITY_CALL, __FUNCTION__);
            m_bIsExistLeftChargedTime = true;
        }
        // ������ȴ�����ÿ�ʼ��ȴtick��
        pFreezePart->SetFreezeTime(EFreeze_ClassID_Goods, nBuffFreezeID, m_nLeftChargeFreezeTime, true);
        m_dwChargeFreezeTick = getTickCountEx();

    }
}
// ʹ����Ʒʱ�����ȴʱ��
int CMultiCDGoods::getMultipleFreezeIntervalTime()
{
    // ������״̬��ʹ�ó�����Ʒ  1����������״̬��ʹ�� 2�����ڳ���״̬��ʹ��
    // 1����������ʹ�õ�ʱ�����ߵ��θ߲���˱俪����ʱ���������3�㣬ʹ�ú�2�㣬������ʱ����
    int nCanUseCount = m_pMobaGoods->GetCanUseCount();
    // 1������������ʹ�ã�����һ�Σ�
    if((m_MultipleOverlayCountLimit - nCanUseCount)==1)
    {
        IFreezeServer *pFreezeServer = gServerGlobal->getFreezeServer();
        if (pFreezeServer == NULL)
        {
            ErrorLn(__FUNCTION__" -- pFreezeServer == NULL");
            return false;
        }
        int nBuffFreezeID = m_pMobaGoods->GetSchemeInfo()->nUseBuffFreezeID;
        m_nLeftChargeFreezeTime = pFreezeServer->GetSchemeFreezeTime(EFreeze_ClassID_Goods, nBuffFreezeID);
        gServerGlobal->getTimerService()->SetTimer(ETimerEventID_GoodsMultipleChargedFreeze, m_nLeftChargeFreezeTime, this, INFINITY_CALL, __FUNCTION__);
        m_dwChargeFreezeTick = getTickCountEx();
    }

    // 2�����ڳ���״̬��ʹ��
    int nPassTime = getTickCountEx() - m_dwChargeFreezeTick;
    // �õ��Ӷ�����ȴЧ����������ȴ���ʱ��,ʹ�ü��������ȴ
    // ���ⳬCD�������:ʣ��ʱ��Ϊ����˵���Ѿ���һ����ȴ�ˣ���������ʣ����ȴ����Ҫ������һ�εĳ���ʣ����ȴ
    // ������ʱ��60����ܣ��ѳ���50�룬ʹ�ü��15���Ͳ�������ʣ����ȴ��ֱ�������´γ���ʱ�䣬��ʱ�г�����ȴ
    if(m_nLeftChargeFreezeTime < nPassTime + m_nUseInterval)
    {
        // ����󲻴���ʣ�����ʱ��
        m_bIsExistLeftChargedTime=false;
        // �ر�ԭ���س��ܶ�ʱ������Ϊ�����ˣ��´ζ�ʱҪ�ı䶨ʱ��ֵ
        gServerGlobal->getTimerService()->KillTimer(ETimerEventID_GoodsMultipleChargedFreeze, this);
    }


    // ʹ��֮������Ϊʹ�ü��״̬
    m_eCDState = State_UseInterval;
    // ����ʹ����ȴ���
    return m_nUseInterval;
}

bool CMultiCDGoods::ResetCanUseCount()
{
    SSchemeMobaGoods *pSchemeMobaGoods = m_pMobaGoods->GetSchemeInfo();
    if (pSchemeMobaGoods == NULL)
    {
        ErrorLn(__FUNCTION__" -- pSchemeMobaGoods == NULL SlotIndex ="<<m_pMobaGoods->GetSlotIndex()<<" GoodID = "<<m_pMobaGoods->GetGoodID());
        return false;
    }
    __IFreezePart *pFreezePart = (__IFreezePart *)m_pMaster->getEntityPart(PART_FREEZE);
    if (pFreezePart == NULL)
    {
        return false;
    }

    m_pMobaGoods->SetCanUseCount(m_MultipleOverlayCountLimit);
    pFreezePart->Cancel(EFreeze_ClassID_Goods, pSchemeMobaGoods->nUseBuffFreezeID, true);
    gServerGlobal->getTimerService()->KillTimer(ETimerEventID_GoodsMultipleChargedFreeze, this);
    m_eCDState = State_Charge;
    return true;

}

bool CMultiCDGoods::onSerialize(rkt::ibuffer & in,int flag)
{
    in>>m_eCDState;      //  ���ܻ��߼����ȴ״̬��trueΪ����
    in>>m_bIsExistLeftChargedTime;   //  ʣ����ȴ����ʱ���Ƿ����     
    in>>m_nLeftChargeFreezeTime;              //  ������ȴ��ʼʱ��
    in>>m_nUseInterval;          //  ������ȴʹ�ü��
    in>>m_MultipleOverlayCountLimit; //  ������ȴ���ܴ���
    in>>m_dwChargeFreezeTick;               //  ������ȴ������ȴtick
    in>>m_bMultipleResetUseCount;

    return true;
}

bool CMultiCDGoods::deSerialize(rkt::obuf & out,int flag)
{
    out<<m_eCDState;      //  ���ܻ��߼����ȴ״̬��trueΪ����
    out<<m_bIsExistLeftChargedTime;   //  ʣ����ȴ����ʱ���Ƿ����     
    out<<m_nLeftChargeFreezeTime;              //  ������ȴ��ʼʱ��
    out<<m_nUseInterval;            //  ������ȴʹ�ü��
    out<<m_MultipleOverlayCountLimit; //  ������ȴ���ܴ���
    out<<m_dwChargeFreezeTick;               //  ���س�����ȴtick
    out<<m_bMultipleResetUseCount;

    return true;
}

bool CMultiCDGoods::onLoad(__IEntity* pEntity, IMobaGoods *pMobaGoods)
{
    if (pEntity == NULL || pMobaGoods == NULL)
    {
        ErrorLn(__FUNCTION__"-- pEntity == NULL || pMobaGoods == NULL");
        return false;
    }

    m_pMaster = pEntity;
    m_pMobaGoods = pMobaGoods;
    return true;
}
