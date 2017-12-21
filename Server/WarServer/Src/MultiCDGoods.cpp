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

    // 获取物品部件
    __IMobaGoodsPart *pMobaGoodsPart = (__IMobaGoodsPart *)m_pMaster->getEntityPart(PART_MOBAGOODS);
    if (pMobaGoodsPart == NULL)
    {
        return false;
    }
    m_eCDState = State_UseInterval;
    m_nLeftChargeFreezeTime = pFreezeServer->GetSchemeFreezeTime(EFreeze_ClassID_Goods, pSchemeMobaGoods->nUseBuffFreezeID);
    // 有以前记录的剩余时间
    SEffectCDOverlay cdData = pMobaGoodsPart->getCDOverlayInfo(pSchemeMobaGoods->nUseBuffFreezeID);
    pMobaGoodsPart->removeCDOverlayInfo(pSchemeMobaGoods->nUseBuffFreezeID);
    if (cdData.nLeftCDTime > 0)
    {
        m_bIsExistLeftChargedTime = true;
        m_nLeftChargeFreezeTime = cdData.nLeftCDTime;
    }
    // 有以前的记录 按以前的记录初始化
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

    // 使用间隔和充能标志位，充能置true，可以主动使用；间隔置false；不可主动使用
    m_nUseInterval = nInterval;
    m_MultipleOverlayCountLimit = OverlayCountLimit;
    m_bMultipleResetUseCount = bResetUseCount;

    if (m_pMobaGoods->GetCanUseCount() < OverlayCountLimit)
    {
        gServerGlobal->getTimerService()->SetTimer(ETimerEventID_GoodsMultipleChargedFreeze, m_nLeftChargeFreezeTime, this,INFINITY_CALL,__FUNCTION__);
        // 设置冷却，设置开始冷却tick数
        __IFreezePart *pFreezePart = (__IFreezePart *)m_pMaster->getEntityPart(PART_FREEZE);
        if (pFreezePart == NULL)
        {
            return false;
        }
        pFreezePart->SetFreezeTime(EFreeze_ClassID_Goods, pSchemeMobaGoods->nUseBuffFreezeID, m_nLeftChargeFreezeTime, true);
        // 设置下充能冷却tick
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
		// 非多重冷却获取配置冷却时间，一般冷却
		int nFreezeTime = pFreezeServer->GetSchemeFreezeTime(ecFreezeClass, nFreezeID);
		// 设置冷却时间
		pFreezePart->SetFreezeTime(ecFreezeClass, nFreezeID, nFreezeTime, true);
        // 设置下充能冷却tick
        m_dwChargeFreezeTick = getTickCountEx();
	}
	else
    {
        // 多重：使用之后获得冷却时间
        int nFreezeTime = getMultipleFreezeIntervalTime();
        // 定时时间配置为0，不做处理
        if(nFreezeTime==0)
        {
            return;
        }
        // 使用之后都先设置一下使用间隔定时器
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
            // 多重冷却充能逻辑功能定时器
            OnGoodsMultipleCDTimeTimer();
        }
        break;
    case ETimerEventID_GoodsMultipleIntervalFreeze:
        {
            // 多重冷却使用间隔逻辑功能定时器
            OnGoodsMultipleIntervalTimeTimer();
        }
        break;
    default:
        break;
    }
}

// 设定间隔后剩余充能冷却和超额充能冷却
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
    // 间隔冷却完毕 开启充能
    m_eCDState=State_Charge;
    int nBuffFreezeID= m_pMobaGoods->GetSchemeInfo()->nUseBuffFreezeID;
    // 充能冷却开始到现在过的时间
    int nPassTime = getTickCountEx() - m_dwChargeFreezeTick;
    // 冷却剩余时间
    m_nLeftChargeFreezeTime = m_nLeftChargeFreezeTime - nPassTime;
    if(m_bIsExistLeftChargedTime)
    {
        // 有充能的剩余时间
        pFreezePart->SetFreezeTime(EFreeze_ClassID_Goods, nBuffFreezeID, m_nLeftChargeFreezeTime, true, false);
        m_dwChargeFreezeTick = getTickCountEx();
        
    }
    else
    {
        // 超额冷却，剩余时间为负
        if(m_nLeftChargeFreezeTime < 0)
        {
            // 已经是超额冷却了 要把冷却的时间补给下一次的充能冷却
            m_nNeedSubFreezeTime =m_nLeftChargeFreezeTime;
            m_nLeftChargeFreezeTime = 0;
        }

        pFreezePart->SetFreezeTime(EFreeze_ClassID_Goods, nBuffFreezeID, m_nLeftChargeFreezeTime, true, false);
        // 开启超额充能定时器
        gServerGlobal->getTimerService()->SetTimer(ETimerEventID_GoodsMultipleChargedFreeze, m_nLeftChargeFreezeTime, this, 1, __FUNCTION__);
        m_dwChargeFreezeTick = getTickCountEx();
    }

}

void CMultiCDGoods::OnGoodsMultipleCDTimeTimer()
{
    // 充能成功
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
        // 充能冷却好了 已经充满 则关闭定时器
        gServerGlobal->getTimerService()->KillTimer(ETimerEventID_GoodsMultipleChargedFreeze,this);
        m_eCDState = State_Full;
    }
    else
    {
        // 充能冷却好了 没有充满
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
        // 还有剩余的充能时间
        if(!m_bIsExistLeftChargedTime)
        {   
            // 补偿的时间 因为使用冷却大于剩余的充能冷却时间 所以要补偿
            if (m_nNeedSubFreezeTime < 0)
            {
                m_nLeftChargeFreezeTime += m_nNeedSubFreezeTime;
                m_nNeedSubFreezeTime = 0;
            }
            gServerGlobal->getTimerService()->SetTimer(ETimerEventID_GoodsMultipleChargedFreeze, m_nLeftChargeFreezeTime, this, INFINITY_CALL, __FUNCTION__);
            m_bIsExistLeftChargedTime = true;
        }
        // 设置冷却，设置开始冷却tick数
        pFreezePart->SetFreezeTime(EFreeze_ClassID_Goods, nBuffFreezeID, m_nLeftChargeFreezeTime, true);
        m_dwChargeFreezeTick = getTickCountEx();

    }
}
// 使用物品时获得冷却时间
int CMultiCDGoods::getMultipleFreezeIntervalTime()
{
    // 有两种状态下使用充能物品  1、充满能量状态下使用 2、正在充能状态下使用
    // 1、充满能量使用的时候从最高到次高层的退变开启定时器：如充满3层，使用后2层，开启定时充能
    int nCanUseCount = m_pMobaGoods->GetCanUseCount();
    // 1、充满能量下使用（用了一次）
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

    // 2、正在充能状态下使用
    int nPassTime = getTickCountEx() - m_dwChargeFreezeTick;
    // 得到从多重冷却效果传来的冷却间隔时间,使用间隔设置冷却
    // 特殊超CD情况处理:剩余时间为负，说明已经过一轮冷却了，不用设置剩余冷却，需要设置下一次的充能剩余冷却
    // 比如总时间60秒充能，已充能50秒，使用间隔15，就不用设置剩余冷却，直接设置下次充能时间，暂时叫超额冷却
    if(m_nLeftChargeFreezeTime < nPassTime + m_nUseInterval)
    {
        // 间隔后不存在剩余充能时间
        m_bIsExistLeftChargedTime=false;
        // 关闭原多重充能定时器，因为超额了，下次定时要改变定时数值
        gServerGlobal->getTimerService()->KillTimer(ETimerEventID_GoodsMultipleChargedFreeze, this);
    }


    // 使用之后设置为使用间隔状态
    m_eCDState = State_UseInterval;
    // 返回使用冷却间隔
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
    in>>m_eCDState;      //  充能或者间隔冷却状态，true为充能
    in>>m_bIsExistLeftChargedTime;   //  剩余冷却充能时间是否存在     
    in>>m_nLeftChargeFreezeTime;              //  多重冷却初始时间
    in>>m_nUseInterval;          //  多重冷却使用间隔
    in>>m_MultipleOverlayCountLimit; //  多重冷却充能次数
    in>>m_dwChargeFreezeTick;               //  多重冷却充能冷却tick
    in>>m_bMultipleResetUseCount;

    return true;
}

bool CMultiCDGoods::deSerialize(rkt::obuf & out,int flag)
{
    out<<m_eCDState;      //  充能或者间隔冷却状态，true为充能
    out<<m_bIsExistLeftChargedTime;   //  剩余冷却充能时间是否存在     
    out<<m_nLeftChargeFreezeTime;              //  多重冷却初始时间
    out<<m_nUseInterval;            //  多重冷却使用间隔
    out<<m_MultipleOverlayCountLimit; //  多重冷却充能次数
    out<<m_dwChargeFreezeTick;               //  多重充能冷却tick
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
