#include "MobaGoods.h"
#include "ISchemeCenter.h"
#include "IEffect.h"
#include "IMobaGoodsPart.h"

CMobaGoods::CMobaGoods():m_pMaster(NULL),m_nGoodID(INVALID_MOBA_GOOD_ID),m_nSlotIndex(GOODS_AUTOUSE_SLOT),
	m_nCurOverlayCount(0),m_nCanUseCount(0),m_GoodsType(MGT_NoneType),m_bIsNewAdd(false),m_bMultipleCDGoods(false),m_pMultiCDGoods(NULL)
{

}



CMobaGoods::~CMobaGoods()
{
}

bool CMobaGoods::CanUse()
{
	__IEntity *pMaster = GetMaster();
	if (pMaster == NULL)
	{
		return false;
	}

	SSchemeMobaGoods *pSchemeMobaGoods = GetSchemeInfo();
	if (pSchemeMobaGoods == NULL)
	{
		ErrorLn(__FUNCTION__" -- pSchemeMobaGoods == NULL");
		return false;
	}

	// ʹ����ȴ���
	__IFreezePart *pFreezePart = (__IFreezePart*)pMaster->getEntityPart(PART_FREEZE);
	if (pFreezePart == NULL)
	{
		return false;
	}
	// ������ȴ�ļ��״̬����ʹ��,����״̬������
    if (m_pMultiCDGoods)
    {
        // ������Ʒʹ��
        if (m_nCanUseCount <= 0 || !m_pMultiCDGoods->canUse())
        {
            return false;
        }

        return true;
    }
    else
    {
        // ����ʹ�ü��
        if (pSchemeMobaGoods->nUseSkillID != 0)
        {
            if (!pFreezePart->CanWork(EFreeze_ClassID_Goods, pSchemeMobaGoods->nUseBuffFreezeID))
            {
                return false;
            }
        }

        // buffʹ�ü��
        if (pSchemeMobaGoods->nUseBuffID != 0)
        {
            if (!pFreezePart->CanWork(EFreeze_ClassID_Goods, pSchemeMobaGoods->nUseBuffFreezeID))
            {
                return false;
            }
        }

        // ʹ��֮����Ҫ�Ƴ��ļ���ۼ�����
        if (pSchemeMobaGoods->bUsedRmove == true)
        {
            if (m_nCurOverlayCount <= 0)
            {
                return false;
            }
        }
        else
        {
            // ʹ��֮����Ҫ�Ƴ��ļ�����ʹ�ô���
            if(pSchemeMobaGoods->nUseSkillID > 0)
            {
                return true;
            }

            if (m_nCanUseCount <= 0)
            {
                return false;
            }
        }
    }

	return true;
}

bool CMobaGoods::Use()
{
	SSchemeMobaGoods* pSchmemInfo = GetSchemeInfo();
	if (pSchmemInfo == nullptr)
	{
		ErrorLn(__FUNCTION__" -- pSchemeMobaGoods == NULL SlotIndex ="<<GetSlotIndex()<<" GoodID = "<<GetGoodID());
		return false;
	}
	// ������ȴʱ�����ͬʱ�ڼ����ʱ��������ʹ�ã���������ԭ������canUse����false������£�����ȴ�����������Ȼ�����Use�����bug
    if (m_pMultiCDGoods)
    {
        if (!m_pMultiCDGoods->canUse())
        {
            return false;
        }
    }

	bool bDelete = false;
	if (pSchmemInfo->bUsedRmove == true)
	{
		m_nCurOverlayCount--;
		if (m_nCurOverlayCount <= 0)
		{
			bDelete = true;
		}
	}
	else
	{
		// ʹ��֮����Ҫ�Ƴ��ļ�����ʹ�ô���
		if(pSchmemInfo->nUseSkillID == 0)
		{
			// ʹ�ô���Ϊ����
			if (m_nCanUseCount <= 0)
			{
				return false;
			}
			else
			{
				bDelete = false;
				m_nCanUseCount--;
			}
		}
		else
		{
		// ʹ�ô���Ϊ����
			if (m_nCanUseCount <= 0)
			{
				return false;
			}
			else if (m_pMultiCDGoods)
            {
                m_nCanUseCount--;
                // ������Ʒ�仯
                UpdateGoodSlotInfo();
            }
		}
	}

	if (pSchmemInfo->nUseBuffID != 0)
	{
		// ʹ����Ʒ���buff
		bool bAddSucess = AddBuff( CMobaGoods::GetMaster()->getUID(), pSchmemInfo->nUseBuffID, 1, CMobaGoods::GetMaster()->getUID());
		if(bAddSucess == true)
		{
			//�����ȴ
			AddFreezeTime(EFreeze_ClassID_Goods, pSchmemInfo->nUseBuffFreezeID);
		}
	}

	//ʹ�óɹ��������ȴ��Ϣ
	if (pSchmemInfo->nUseBuffFreezeID != 0)
	{
		AddFreezeTime(EFreeze_ClassID_Goods, pSchmemInfo->nUseBuffFreezeID);
	}

	return bDelete;
}

SSchemeMobaGoods* CMobaGoods::GetSchemeInfo()
{
	ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
	if(pSchemeCenter == NULL)
	{
		return nullptr;
	}
	ISchemeMobaGoods * pSchemeMobaGoods = pSchemeCenter->getSchemeMobaGoods();
	if(pSchemeMobaGoods == NULL)
	{
		return nullptr;
	}

	return pSchemeMobaGoods->getSchemeMobaGoodsInfo(m_nGoodID);
}


void CMobaGoods::AddGoodsEffect()
{

	//��ӽ���Ʒ�������Թ�����
	__IMobaGoodsPart* pMobaGoodsPart = (__IMobaGoodsPart*)m_pMaster->getEntityPart(PART_MOBAGOODS);
	if(pMobaGoodsPart == NULL ) 
	{
		return;
	}

	SSchemeMobaGoods *pSchemeMobaGoods = GetSchemeInfo();
	if (pSchemeMobaGoods == NULL)
	{
		ErrorLn(__FUNCTION__" -- pSchemeMobaGoods == NULL SlotIndex ="<<GetSlotIndex()<<" GoodID = "<<GetGoodID());
		return;
	}

	T_MAP_EFFECT::iterator iterBase = m_mapEffect.begin();
	for (;iterBase != m_mapEffect.end(); ++iterBase)
	{
		//��ӽ�������Ʒ�������Թ�����
		iterBase->second.bIsOpen = pMobaGoodsPart->AddEffect(m_nSlotIndex, iterBase->second.nEffectID, iterBase->second.nEffectType);
	}

    T_MAP_EFFECT::iterator iterInheritBase = m_mapInheritEffect.begin();
    for (;iterInheritBase != m_mapInheritEffect.end(); ++iterInheritBase)
    {
        //��ӽ��̳���Ʒ�������Թ�����
        iterInheritBase->second.bIsOpen = true;
        pMobaGoodsPart->AddMonsterInheritEffect(m_nSlotIndex, iterInheritBase->second.nEffectID, iterInheritBase->second.nEffectType);
    }

	return;
}

// ����´�������ּ̳�����
void CMobaGoods::addCreateMonsterInheritEffect(UID uid)
{
    __IMobaGoodsPart* pMobaGoodsPart = (__IMobaGoodsPart*)m_pMaster->getEntityPart(PART_MOBAGOODS);
    if(pMobaGoodsPart == NULL ) 
    {
        return;
    }
    T_MAP_EFFECT::iterator iterInheritBase = m_mapInheritEffect.begin();
    for (;iterInheritBase != m_mapInheritEffect.end(); ++iterInheritBase)
    {
        //��ӽ�������Ʒ�������Թ�����
        iterInheritBase->second.bIsOpen = true;
        pMobaGoodsPart->AddCreateMonsterInheritEffect(uid, m_nSlotIndex, iterInheritBase->second.nEffectID, iterInheritBase->second.nEffectType);
    }
}

bool CMobaGoods::bIsMultipleCDGoods()
{
	return m_bMultipleCDGoods;
}

void CMobaGoods::RecomputeGoodsEffect()
{
    if(m_pMaster == NULL)
    {
        return;
    }

	//������Ʒ����
	__IMobaGoodsPart* pMobaGoodsPart = (__IMobaGoodsPart*)m_pMaster->getEntityPart(PART_MOBAGOODS);
	if(pMobaGoodsPart == NULL ) 
	{
		return;
	}

	SSchemeMobaGoods *pSchemeMobaGoods = GetSchemeInfo();
	if (pSchemeMobaGoods == NULL)
	{
		ErrorLn(__FUNCTION__" -- pSchemeMobaGoods == NULL SlotIndex ="<<GetSlotIndex()<<" GoodID = "<<GetGoodID());
		return;
	}

	T_MAP_EFFECT::iterator iterBase = m_mapEffect.begin();
	for (;iterBase != m_mapEffect.end(); ++iterBase)
	{
        // �����Ψһ���Ե� ������Ψһ�� ���Ƴ�������
        if (iterBase->second.bIsOpen == true && iterBase->second.nEffectType != MGET_BaseEffect)
        {
            pMobaGoodsPart->RemoveEffect(m_nSlotIndex, iterBase->second.nEffectID, iterBase->second.nEffectType);
            iterBase->second.bIsOpen = pMobaGoodsPart->AddEffect(m_nSlotIndex, iterBase->second.nEffectID, iterBase->second.nEffectType);
        }

		//��ӽ�������Ʒ�������Թ���
		if (iterBase->second.bIsOpen ==  false )
		{
			iterBase->second.bIsOpen = pMobaGoodsPart->AddEffect(m_nSlotIndex, iterBase->second.nEffectID, iterBase->second.nEffectType);
		}
	}

    T_MAP_EFFECT::iterator iterInheritBase = m_mapInheritEffect.begin();
    for (;iterInheritBase != m_mapInheritEffect.end(); ++iterInheritBase)
    {
        //��ӽ��̳�����Ʒ�������Թ�����
        if (iterBase->second.bIsOpen ==  false )
        {
            iterInheritBase->second.bIsOpen = pMobaGoodsPart->AddMonsterInheritEffect(m_nSlotIndex, iterInheritBase->second.nEffectID, iterInheritBase->second.nEffectType);
        }
    }

	return;
}


void CMobaGoods::RemoveGoodsEffect()
{
    if(m_pMaster == NULL)
    {
        return;
    }
	__IMobaGoodsPart* pMobaGoodsPart = (__IMobaGoodsPart*)m_pMaster->getEntityPart(PART_MOBAGOODS);
	if(pMobaGoodsPart == NULL) 
	{
		return;
	}

	T_MAP_EFFECT::iterator iterBase = m_mapEffect.begin();
	for (;iterBase != m_mapEffect.end(); ++iterBase)
	{
		if (true == iterBase->second.bIsOpen)
		{
			pMobaGoodsPart->RemoveEffect(m_nSlotIndex,iterBase->second.nEffectID,iterBase->second.nEffectType);
		}
	}

    T_MAP_EFFECT::iterator iterInheritBase = m_mapInheritEffect.begin();
    for (;iterInheritBase != m_mapInheritEffect.end(); ++iterInheritBase)
    {
        if (true == iterInheritBase->second.bIsOpen)
        {
            pMobaGoodsPart->RemoveMonsterInheritEffect(m_nSlotIndex,iterInheritBase->second.nEffectID,iterInheritBase->second.nEffectType);
        }
    }

	return;
}

void CMobaGoods::Initial(__IEntity * pPerson, int nSlotIndex, int nGoodID)
{
	if (pPerson == NULL || nGoodID == INVALID_MOBA_GOOD_ID)
	{
		return;
	}

	// ��Ʒ����
	m_bIsNewAdd = true;
	m_pMaster = pPerson;
	m_nGoodID = nGoodID;
	m_nSlotIndex = nSlotIndex;
	m_nCurOverlayCount = 1;
	SSchemeMobaGoods *pSchemeMobaGoods = GetSchemeInfo();
	if (pSchemeMobaGoods == NULL)
	{
		ErrorLn(__FUNCTION__" -- pSchemeMobaGoods == NULL SlotIndex ="<<GetSlotIndex()<<" GoodID = "<<GetGoodID());
		return;
	}

	m_GoodsType = pSchemeMobaGoods->nTypeID;
	m_nCanUseCount = pSchemeMobaGoods->nCountCanUse;

	for (int n = 0 ; n < pSchemeMobaGoods->nEffectCount; ++n)
	{
		SMobaGoodEffectInfo MobaGoodEffectInfo;
		MobaGoodEffectInfo.nEffectID = pSchemeMobaGoods->GoodEffectInfo[n].nEffectID;
		MobaGoodEffectInfo.nEffectType= pSchemeMobaGoods->GoodEffectInfo[n].nEffectType;
		MobaGoodEffectInfo.bIsOpen = false;

		m_mapEffect.insert( pair<int,SMobaGoodEffectInfo>(pSchemeMobaGoods->GoodEffectInfo[n].nEffectID, MobaGoodEffectInfo));
	}

    for (int n = 0 ; n < pSchemeMobaGoods->nInheritEffectCount; ++n)
	{
		SMobaGoodEffectInfo MobaGoodEffectInfo;
		MobaGoodEffectInfo.nEffectID = pSchemeMobaGoods->InheritGoodEffectInfo[n].nEffectID;
		MobaGoodEffectInfo.nEffectType= pSchemeMobaGoods->InheritGoodEffectInfo[n].nEffectType;
		MobaGoodEffectInfo.bIsOpen = false;

		m_mapInheritEffect.insert( pair<int,SMobaGoodEffectInfo>(pSchemeMobaGoods->InheritGoodEffectInfo[n].nEffectID, MobaGoodEffectInfo));
	}

	// ��ʼ����ȴ���
	if (pSchemeMobaGoods->nUseBuffFreezeID > 0)
	{
		__IFreezePart *pFreezePart = (__IFreezePart *)m_pMaster->getEntityPart(PART_FREEZE);
		if (pFreezePart == NULL)
		{
			return;
		}

		// ������ȴʱ��
		int nFreezeTime = pFreezePart->GetFreezeLeftTime(EFreeze_ClassID_Goods, pSchemeMobaGoods->nUseBuffFreezeID);
		// ������ȴʱ��
		pFreezePart->SetFreezeTime(EFreeze_ClassID_Goods, pSchemeMobaGoods->nUseBuffFreezeID, nFreezeTime);
	}

	//��ʼ�����֮�����Ч��
	AddGoodsEffect();
}

void CMobaGoods::Release()
{

	// TODO Ϊ�β��Ƴ�Ч������
	//RemoveGoodsEffect();

	m_bIsNewAdd = false;
	m_GoodsType = MGT_NoneType;
	m_nGoodID =INVALID_MOBA_GOOD_ID;
	m_pMaster = NULL;
	m_nSlotIndex = GOODS_AUTOUSE_SLOT;
	m_nCurOverlayCount= 0;
	m_nCanUseCount=0;

	if(m_pMultiCDGoods)
    {
        m_pMultiCDGoods->release();
        m_pMultiCDGoods = NULL;
        m_bMultipleCDGoods = false;
    }

	delete this;
}

int CMobaGoods::GetGoodID()
{
	return m_nGoodID;
}

int CMobaGoods::GetSlotIndex()
{
	return m_nSlotIndex;
}

bool CMobaGoods::CanAddOverlayCount()
{
	SSchemeMobaGoods *pSchemeMobaGoods = GetSchemeInfo();
	if (pSchemeMobaGoods == NULL)
	{
		ErrorLn(__FUNCTION__" -- pSchemeMobaGoods == NULL SlotIndex ="<<GetSlotIndex()<<" GoodID = "<<GetGoodID());
		return false;
	}

	return (pSchemeMobaGoods->bCanOverlay && m_nCurOverlayCount < pSchemeMobaGoods->nOverlayCount );
}

bool CMobaGoods::AddOverlayCount()
{
	m_nCurOverlayCount++;
	return true;
}

__IEntity * CMobaGoods::GetMaster()
{
	return m_pMaster;
}

void CMobaGoods::ConsumedOverlayCount()
{

}

void CMobaGoods::ConsumedUseCount()
{

}

int CMobaGoods::GetCanUseCount()
{
	return m_nCanUseCount;
}

void CMobaGoods::AddFreezeTime(EFREEZE_CLASS ecFreezeClass, int nFreezeID)
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

    if (m_pMultiCDGoods)
    {
        m_pMultiCDGoods->AddFreezeTime(ecFreezeClass, nFreezeID);
    }
    else
    {
        // �Ƕ�����ȴ��ȡ������ȴʱ�䣬һ����ȴ
        int nFreezeTime = pFreezeServer->GetSchemeFreezeTime(ecFreezeClass, nFreezeID);
        // ������ȴʱ��
        pFreezePart->AddSpellBreakFreezeTime(ecFreezeClass, nFreezeID, nFreezeTime);
    }
	
}

int CMobaGoods::GetCanUseTimes()
{
	SSchemeMobaGoods *pSchemeMobaGoods = GetSchemeInfo();
	if (pSchemeMobaGoods == NULL)
	{
		ErrorLn(__FUNCTION__" -- pSchemeMobaGoods == NULL SlotIndex ="<<GetSlotIndex()<<" GoodID = "<<GetGoodID());
		return 0;
	}

	// ʹ��֮����Ҫ�Ƴ��ļ���ۼ�����
	if (pSchemeMobaGoods->bUsedRmove == true)
	{
		return m_nCurOverlayCount;
	}
	else
	{
		// ʹ��֮����Ҫ�Ƴ��ļ�����ʹ�ô���
		return m_nCanUseCount;
	}
}

bool CMobaGoods::Remove()
{

    // ������Ʒ��¼����
    if (m_bMultipleCDGoods)
    {
        recordeMultipleCDInfo();
    }
    

	RemoveGoodsEffect();

	//�Ƴ���Ʒ֮����ӵ�Ч��
	IEffectFactory *pEffectFactory = gServerGlobal->getEffectFactory();
	if(pEffectFactory == NULL)
	{
		ErrorLn(__FUNCTION__" -- pEffectFactory == NULL")
			return false;
	}

	SSchemeMobaGoods *pSchemeMobaGoods = GetSchemeInfo();
	if (pSchemeMobaGoods == NULL)
	{
		ErrorLn(__FUNCTION__" -- pSchemeMobaGoods == NULL");
		return false;
	}

	if (m_pMaster == NULL)
	{
		return false;
	}
	UID uid = m_pMaster->getUID();

	for (int n = 0; n < MAX_EFFECT_COUNT; ++n)
	{
		int nRemoveEffectID = pSchemeMobaGoods->GoodRemoveEffectInfo[n];
		if(nRemoveEffectID > 0)
		{
			IEffect *pEffect = pEffectFactory->BuildEffect(nRemoveEffectID);
			if (pEffect == NULL)
			{
				// ����Ч���Ӳ��ϵ����
				ErrorLn(__FUNCTION__" -- nRemoveEffectID = "<< nRemoveEffectID);
				continue;
			}
			else
			{
				EFFECT_CONTEXT context;
				context.pEntity = m_pMaster;
				context.uidOperator = uid;
				context.uidTarget = uid;
				pEffect->Start(&context, NULL);
				pEffect->Stop();
				pEffect->Release();
			}
		}
	}

	return true;
}

int CMobaGoods::GetOverLayCount()
{
	return m_nCurOverlayCount;
}

void CMobaGoods::SetOverLayCount(int nOverLayCount)
{
	m_nCurOverlayCount = nOverLayCount;
}

emMobaGoodType CMobaGoods::GetGoodsType()
{
	return m_GoodsType;
}

int CMobaGoods::GetSkillID()
{
	SSchemeMobaGoods *pSchemeMobaGoods = GetSchemeInfo();
	if (pSchemeMobaGoods == NULL)
	{
		ErrorLn(__FUNCTION__" -- pSchemeMobaGoods == NULL SlotIndex ="<<GetSlotIndex()<<" GoodID = "<<GetGoodID());
		return 0;
	}

	return GetSchemeInfo()->nUseSkillID;
}

bool CMobaGoods::GetIsNewAddGoods()
{
	return m_bIsNewAdd;
}

void CMobaGoods::SetIsNewAddGoods(bool bState)
{
	m_bIsNewAdd = bState;
}

bool CMobaGoods::ResetCanUseCount()
{
    if (m_pMultiCDGoods)
    {
        return m_pMultiCDGoods->ResetCanUseCount();
    }
    else
    {
        SSchemeMobaGoods *pSchemeMobaGoods = GetSchemeInfo();
        if (pSchemeMobaGoods == NULL)
        {
            ErrorLn(__FUNCTION__" -- pSchemeMobaGoods == NULL SlotIndex ="<<GetSlotIndex()<<" GoodID = "<<GetGoodID());
            return false;
        }
        int resetCount = pSchemeMobaGoods->nCountCanUse;
        if (resetCount > 0 && m_nCanUseCount !=resetCount)
        {
            m_nCanUseCount = resetCount;	
            return true;
        }
    }

	return false;
}
// ���ö�����ȴ����ʼ������
void CMobaGoods::SetMultipleCoolDown(int nInterval,int OverlayCountLimit,bool bResetUseCount, int bInitalUseCount)
{
	if (m_pMaster == NULL)
	{
		return;
	}
    m_pMultiCDGoods = new CMultiCDGoods();
    if (!m_pMultiCDGoods)
    {
        SAFE_DELETE(m_pMultiCDGoods);
        return;
    }
    m_pMultiCDGoods->inital(m_pMaster, this, nInterval, OverlayCountLimit, bResetUseCount, bInitalUseCount);
    m_bMultipleCDGoods = true;
}


void CMobaGoods::UpdateGoodSlotInfo()
{
	//��ӽ���Ʒ�������Թ�����
	__IMobaGoodsPart* pMobaGoodsPart = (__IMobaGoodsPart*)m_pMaster->getEntityPart(PART_MOBAGOODS);
	if(pMobaGoodsPart == NULL ) 
	{
		return;
	}

	pMobaGoodsPart->UpdateGoodSlotInfo(SLOT_TYPE_GOODS, m_nSlotIndex);
}

bool CMobaGoods::onSerialize(rkt::ibuffer & in,int flag)
{
	if (!in.good())
	{
		return false;
	}

	in>>m_bIsNewAdd;
	in>>m_nGoodID;
	in>>m_nSlotIndex;
	in>>m_nCurOverlayCount; //	��ǰ��������
	in>>m_nCanUseCount;		//	��ʹ�ô���
	in>>m_GoodsType;
    in>>m_bMultipleCDGoods;
	
    if (m_bMultipleCDGoods)
    {
        m_pMultiCDGoods = new CMultiCDGoods();
        if (!m_pMultiCDGoods)
        {
            SAFE_DELETE(m_pMultiCDGoods);
            return false;
        }
        m_pMultiCDGoods->onSerialize(in, flag);
    }

    m_mapEffect.clear();
    SSchemeMobaGoods *pSchemeMobaGoods = GetSchemeInfo();
    if (pSchemeMobaGoods != NULL)
    {
        for (int n = 0 ; n < pSchemeMobaGoods->nEffectCount; ++n)
        {
            SMobaGoodEffectInfo MobaGoodEffectInfo;
            MobaGoodEffectInfo.nEffectID = pSchemeMobaGoods->GoodEffectInfo[n].nEffectID;
            MobaGoodEffectInfo.nEffectType= pSchemeMobaGoods->GoodEffectInfo[n].nEffectType;
            MobaGoodEffectInfo.bIsOpen = false;

            m_mapEffect.insert( pair<int,SMobaGoodEffectInfo>(pSchemeMobaGoods->GoodEffectInfo[n].nEffectID, MobaGoodEffectInfo));
        }

        for (int n = 0 ; n < pSchemeMobaGoods->nInheritEffectCount; ++n)
        {
            SMobaGoodEffectInfo MobaGoodEffectInfo;
            MobaGoodEffectInfo.nEffectID = pSchemeMobaGoods->InheritGoodEffectInfo[n].nEffectID;
            MobaGoodEffectInfo.nEffectType= pSchemeMobaGoods->InheritGoodEffectInfo[n].nEffectType;
            MobaGoodEffectInfo.bIsOpen = false;

            m_mapInheritEffect.insert( pair<int,SMobaGoodEffectInfo>(pSchemeMobaGoods->InheritGoodEffectInfo[n].nEffectID, MobaGoodEffectInfo));
        }
    }

	return true;
}

bool CMobaGoods::deSerialize(rkt::obuf & out,int flag)
{
	out<<m_bIsNewAdd;
	out<<m_nGoodID;
	out<<m_nSlotIndex;
	out<<m_nCurOverlayCount; //	��ǰ��������
	out<<m_nCanUseCount;		//	��ʹ�ô���
	out<<m_GoodsType;
    out<<m_bMultipleCDGoods;
	
    if (m_bMultipleCDGoods)
    {
        m_pMultiCDGoods->deSerialize(out, flag);
    }

	return true;
}

bool CMobaGoods::onLoad(__IEntity* pEntity, int nLoginMode)
{
	if(pEntity == NULL)
	{
		return false;
	}

	m_pMaster = pEntity;

    if (m_bMultipleCDGoods)
    {
        if (m_pMultiCDGoods != NULL)
        {
            m_pMultiCDGoods->onLoad(m_pMaster, this);
        }
    }

	// ���¼�������Ч��
	RecomputeGoodsEffect();

	return true;
}

void CMobaGoods::recordeMultipleCDInfo()
{
    __IEntity *pMaster = GetMaster();
    if (pMaster == NULL)
    {
        return;
    }

    __IMobaGoodsPart *pMobaGoodsPart = (__IMobaGoodsPart *)pMaster->getEntityPart(PART_MOBAGOODS);
    if (pMobaGoodsPart == NULL)
    {
        return;
    }

    __IFreezePart *pFreezePart = (__IFreezePart *)pMaster->getEntityPart(PART_FREEZE);
    if (pFreezePart == NULL)
    {
        return;
    }

    // ����
    int nBuffFreezeID = GetSchemeInfo()->nUseBuffFreezeID;
    SEffectCDOverlay data;
    data.nLeftCanUseCount = GetCanUseCount();
    data.nLeftCDTime = pFreezePart->GetFreezeLeftTime(EFreeze_ClassID_Goods, nBuffFreezeID);
    pMobaGoodsPart->setCDOverlayInfo(nBuffFreezeID, data);
}


void CMobaGoods::SetCanUseCount(int nCanUseCount)
{
    m_nCanUseCount = nCanUseCount;

    UpdateGoodSlotInfo();
}
