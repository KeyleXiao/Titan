#include "stdafx.h"
#include "TalentEffectProxy.h"
#include "SchemeCenter.h"
#include "SchemeDef.h"

// 构造函数
TalentEffectProxy:: TalentEffectProxy()
{
	m_TalentEffectAll.clear();
}
// 析构函数
TalentEffectProxy:: ~TalentEffectProxy()
{
	RemoveAllEffect();
}

#ifdef FUNCTION_TALENTPAGE_OPEN
void TalentEffectProxy::AddTalentEffect(__IEntity *pTarget,TALENT_INFO_VECTOR& vTalentInfoVec)
{

	if(pTarget==NULL)
	{
		return ;
	}

	if(vTalentInfoVec.size()==0)
	{
		return ;
	}

	ISchemeTalentPage *pSchemeTalentPage = gServerGlobal->getSchemeCenter()->getSchemeTalentPage();
	if (pSchemeTalentPage == NULL)
	{
		ErrorLn("TalentEffectProxy::AddTalentEffect() -- getSchemeTalentPage == NULL")
			return ;
	}

	ISchemeTalentPageEffect *pSchemeTalentPageEffect = gServerGlobal->getSchemeCenter()->getSchemeTalentPageEffect();
	if (pSchemeTalentPageEffect == NULL)
	{
		ErrorLn("TalentEffectProxy::AddTalentEffect() -- getSchemeTalentPageEffect == NULL")
			return ;
	}

	TALENT_INFO_VECTOR::iterator iter=vTalentInfoVec.begin();
	vector<string> vTalentPageVector;
	vector<string> vTalentPageEffectVector;
	for (;iter!=vTalentInfoVec.end();++iter)
	{
		int nEffectArrayIndex=iter->byPoints;
		if(nEffectArrayIndex<=0)
		{

			continue;
		}
		int nTalentId=iter->byTalentID;

		STalentPageData*pTalentPage=pSchemeTalentPage->getTalentPageSchemeInfo(nTalentId);
		if(pTalentPage==NULL)
		{
			ErrorLn("TalentEffectProxy::AddTalentEffect() -- getTalentPageShemeInfo == NULL")
				continue;
		}

		vTalentPageVector.clear();
		StringHelper::split(vTalentPageVector, pTalentPage->szEffectIdArray, ';', " ");
		if(nEffectArrayIndex>vTalentPageVector.size())
		{
			continue;
		} 

		int nEffectArrayId=atoi(vTalentPageVector[nEffectArrayIndex-1].c_str());
		STalentPageEffect*pTalentPageEffect=pSchemeTalentPageEffect->getTalentEffectSchemeInfo(nEffectArrayId);
		if(pTalentPageEffect==NULL)
		{
			ErrorLn("TalentEffectProxy::AddTalentEffect() -- getTalentEffectShemeInfo == NULL")
				continue;
		}

		vTalentPageEffectVector.clear();
		StringHelper::split(vTalentPageEffectVector, pTalentPageEffect->stEffectArray, ';', " ");
		for (UINT nCount=0; nCount<vTalentPageEffectVector.size(); ++nCount)
		{
			int  nEffectId=(StringHelper::toInt(vTalentPageEffectVector[nCount]));
			AddEffect(nEffectId,pTarget);
		}

	}
}
#endif

void TalentEffectProxy::AddEffect(int nEffectID, __IEntity *pTarget)
{
	if (nEffectID == 0 || pTarget == nullptr)
	{
		return ;
	}

	IEffectFactory *pEffectFactory = gServerGlobal->getEffectFactory();
	if(pEffectFactory == NULL)
	{
		ErrorLn("TalentEffectProxy::AddEffect() -- pEffectFactory == NULL")
			return ;
	}

	IEffect *pEffect = pEffectFactory->BuildEffect(nEffectID);
	if (pEffect == NULL)
	{
		// 处理效果加不上的情况
		ErrorLn("TalentEffectProxy::AddEffect() -- EffectID = "<< nEffectID);
		return;

	}

	UID uid = pTarget->getUID();
	EFFECT_CONTEXT context;
	context.pEntity = pTarget;
	context.uidOperator = uid;
	context.uidTarget = uid;
	context.nID=nEffectID;
	pEffect->Start(&context, NULL);    
	m_TalentEffectAll.push_back(pEffect);
	return ;
}

#ifdef FUNCTION_RUNE_OPEN
void TalentEffectProxy::AddRuneEffect(__IEntity *pTarget,Entity_Rune_Page_Info& runeInfo)
{
	if ( pTarget == nullptr)
	{
		return ;
	}

	IEffectFactory *pEffectFactory = gServerGlobal->getEffectFactory();
	if(pEffectFactory == NULL)
	{
		ErrorLn("TalentEffectProxy::AddEffect() -- pEffectFactory == NULL")
			return ;
	}

	ISchemeRuneInfo *pRuneInfoService = NULL;
	if (!(pRuneInfoService = gServerGlobal->getSchemeCenter()->getSchemeRuneInfo()))
	{
		ErrorLn(__FUNCTION__ << " Query RuneInfo Failed !!!");
		return ;
	}
	vector<string> vRuneEffectIdVec;
	vector<string> vRuneEffectValueVec;
	for (int idx = 0; idx < runeInfo.nRuneNum; idx++)
	{
		int nRuneId=runeInfo.nRuneGroove[idx];
		if(nRuneId==0)
		{
			continue;
		}

		RuneInfoScheme *pRuneInfoScheme= pRuneInfoService->GetRuneSchemeInfo(nRuneId);
		if(pRuneInfoScheme==NULL)
		{
			continue;
		}

		int nRuneNum = 0; 
		for (int i=0; i < runeInfo.nRuneNum; i++)    //遍历找到同id个数，这个记录下有点耗性能，后面改(因为改动有点大)
		{
			if(nRuneId == runeInfo.nRuneGroove[i])
			{
				runeInfo.nRuneGroove[i]=0;
				nRuneNum++;
			}
		}

		int nEffectValueIndex=0;
		EFFECT_VALUE_LIST ListValue;

		vRuneEffectIdVec.clear();
		vRuneEffectValueVec.clear();

		StringHelper::split(vRuneEffectIdVec, pRuneInfoScheme->stEffectArray, ';', " ");
		StringHelper::split(vRuneEffectValueVec, pRuneInfoScheme->stEffectValueArray, ';', " ");

		vector<string>::iterator iterEffectId= vRuneEffectIdVec.begin();
		for (;iterEffectId!=vRuneEffectIdVec.end();++iterEffectId)
		{    
			int nEffectId= atoi((*iterEffectId).c_str());
			IEffect *pEffect = pEffectFactory->BuildEffect(nEffectId);
			if (pEffect == NULL)
			{
				// 处理效果加不上的情况
				ErrorLn("TalentEffectProxy::AddRuneEffect() -- EffectID = "<< nEffectId);
				continue;
			}

			int nEffectType = (int)pEffect->GetEffectType();
			if(nEffectType!=(int)EFFECT_CHANGE_VALUE)
			{
				continue;
			}

			if(vRuneEffectValueVec.size()<=0||vRuneEffectValueVec.size()<=nEffectValueIndex)
			{
				continue;
			}

			int nEffectValue =atoi( vRuneEffectValueVec[nEffectValueIndex].c_str());
			int nTotalEffectValue = nRuneNum*nEffectValue;

			ListValue.clear();
			ListValue.push_back(nTotalEffectValue);
			IEffectChangeValue*  pEffectChangeValue= ( IEffectChangeValue* )pEffect;
			pEffectChangeValue->SetValue(&ListValue);

			UID uid = pTarget->getUID();
			EFFECT_CONTEXT context;
			context.pEntity = pTarget;
			context.uidOperator = uid;
			context.uidTarget = uid;
			context.nID=nEffectId;
			pEffect->Start(&context, NULL);    
			m_TalentEffectAll.push_back(pEffect); // 名称目前先这样，后面改

			nEffectValueIndex++;
		}
	}
}
#endif

void TalentEffectProxy::RemoveAllEffect()
{
	IEffectFactory *pEffectFactory = gServerGlobal->getEffectFactory();
	if(pEffectFactory == NULL)
	{
		return;
	}

	T_LIST_TALENT_EFFECT_ALL::iterator iter =  m_TalentEffectAll.begin();
	for (;iter != m_TalentEffectAll.end();++iter)
	{
		//移除效果 后续添加
		(*iter)->Stop();
		(*iter)->Release();
	}
	m_TalentEffectAll.clear();
	return;
}

#ifdef FUNCTION_TALENTPAGE_OPEN
bool TalentEffectProxy::RfficacyTalentPoints(TALENT_INFO_VECTOR& vTalentInfoVec)
{
	if(vTalentInfoVec.size()==0)
	{
		return true ;
	}

	ISchemeTalentPage *pSchemeTalentPage = gServerGlobal->getSchemeCenter()->getSchemeTalentPage();
	if (pSchemeTalentPage == NULL)
	{
		ErrorLn("TalentEffectProxy::RfficacyTalentPoints() -- getSchemeTalentPage == NULL")
			return true;
	}

	TMAP_TALENTSCHEME *pMapTalentPage=((CSchemeTalentPage*)pSchemeTalentPage)->getTalentPageSchemeInfoMapAll();
	int nMaxLevelCount=0;// 最大等级（6）加点次数
	int nTotalPoint=0;// 玩家全部加点数
	// 将天赋点数和id信息存到map，map查询起来较快
	map<int,int> mapRfficacyTalent;
	TALENT_INFO_VECTOR::iterator iter=vTalentInfoVec.begin();
	for (;iter!=vTalentInfoVec.end();++iter)
	{
		int nPoints=iter->byPoints;
		if(nPoints<=0)
		{

			continue;
		}
		int nTalentId=iter->byTalentID;
		TMAP_TALENTSCHEME::iterator iter = pMapTalentPage->find(nTalentId);
		if (iter == pMapTalentPage->end())
		{   // 表里没找到天赋id，不继续找了,数据天赋id肯定有问题,效验失败
			return false;
		}
		int nCurTalentLevel=iter->second.nLevel;
		if(nCurTalentLevel==TalentDefaultMaxLevel)
		{
			nMaxLevelCount++;
		}
		nTotalPoint+=nPoints;
		mapRfficacyTalent[nTalentId]=nPoints;
	}
	// 第一种判定作弊：玩家加点数目综合大于基于等级的最大可使用数量。则效验失败
	if(nTotalPoint>TalentMaxCanUseCount)
	{
		return false;
	}
	// 第二种判定作弊: 玩家都在最大级(6)加点（目前规则只允许加一点）。则效验失败
	if(nMaxLevelCount>1)
	{
		return false;
	}
	// 第三种判定作弊：玩家加点的上一级没有加点（每个点都效验最保险）。则效验失败
	map<int,int>::iterator iterRfficacy=mapRfficacyTalent.begin();
	for (;iterRfficacy!=mapRfficacyTalent.end();++iterRfficacy)
	{    
		int nTalentId=iterRfficacy->first;
		int nCurTalentLevel=(*pMapTalentPage)[nTalentId].nLevel;
		if(nCurTalentLevel==1)
		{
			// 等级为0的不判断，上级没限制
			continue;
		}
		int nHigherLevel=nCurTalentLevel-1;//上一层等级
		bool bIsExistHigherLevel=false;
		int nCurTalentType=(*pMapTalentPage)[nTalentId].nTalentType;
		// 找寻上个点的id
		TMAP_TALENTSCHEME::iterator iterTalent=pMapTalentPage->begin();
		for (;iterTalent!=pMapTalentPage->end();++iterTalent)
		{
			// 找到同类型上一等级的id，一般有两个或者三个
			if(iterTalent->second.nTalentType!=nCurTalentType||iterTalent->second.nLevel!=nHigherLevel)
			{
				continue;

			}

			int nHigherTalentID=iterTalent->first;//上一层等级天赋id
			map<int,int>::iterator iterRfficacyPoint = mapRfficacyTalent.find(nHigherTalentID);
			if (iterRfficacyPoint == mapRfficacyTalent.end())
			{
				//没找到继续找
				bIsExistHigherLevel=false;
			}
			else
			{
				// 找到了退出
				bIsExistHigherLevel=true;
				break;
			}
		}
		// 有天赋一个效验失败就算失败
		if(bIsExistHigherLevel==false)
		{
			return false;
		}

	}   


	return true;

}
#endif

void TalentEffectProxy::AddCrownEffect(__IEntity *pTarget,int* propertyList, ETIME_TO_ADD_EFFECT timeToAddEffect)
{
	if ( pTarget == NULL||propertyList == NULL||propertyList[0]<=0)
	{
		return ;
	}

	ISchemeCenter* pSchemeCenter = gServerGlobal->getSchemeCenter();
	if ( pSchemeCenter == NULL )
	{
		return ;
	}

	ISchemeCrownGemstoneInfo *pCrownInfoService = pSchemeCenter->getSchemeCrownGemstoneInfo();
	if ((pCrownInfoService == NULL ))
	{
		ErrorLn(__FUNCTION__ << " Query CrownInfo Failed !!!");
		return  ;
	}

	IEffectFactory *pEffectFactory = gServerGlobal->getEffectFactory();
	if(pEffectFactory == NULL)
	{
		ErrorLn("TalentEffectProxy::AddEffect() -- pEffectFactory == NULL")
			return ;
	}

	const int nEffectCountMax = 30;
	int effectIDList[nEffectCountMax]={0};
	int effectValueList[nEffectCountMax]={0};
	int nEffectListIndex = 0;
	GEMSTONE_ITEM* pGemstoneItem = NULL;
	SCrownGemstoneConfig_S *pCrownInfoScheme = NULL;
	vector<string> effectIDVector;
	vector<string> effectValueVector;
	vector<string>::iterator iterEffectIDBegin ;
	vector<string>::iterator iterEffectValueBegin ;
	for (int i=0;i<CROWN_PROPERTY_COUNT_MAX;++i)
	{
		int nPropertyID = propertyList[i];
		if(nPropertyID<=0)
			continue;

		SCrownGemstoneEffectConfig_S* pEffectConfig = pCrownInfoService->getCrownGemstoneEffectConfig(nPropertyID);
		if(pEffectConfig == NULL)
			continue;

		effectIDVector.clear();
		StringHelper::split(effectIDVector, pEffectConfig->stEffectIDArray, ';', " ");
		iterEffectIDBegin = effectIDVector.begin();
		for (;iterEffectIDBegin!=effectIDVector.end();++iterEffectIDBegin)
		{
			int nEffectID =atoi((*iterEffectIDBegin).c_str());
			if(nEffectID<=0)
				continue;

			if(nEffectListIndex<0||nEffectListIndex>=nEffectCountMax)
				continue;

			effectIDList[nEffectListIndex] = nEffectID;
		}

		effectValueVector.clear();
		StringHelper::split(effectValueVector, pEffectConfig->stEffectValueArray, ';', " ");
		iterEffectValueBegin = effectValueVector.begin();
		for (;iterEffectValueBegin!=effectValueVector.end();++iterEffectValueBegin)
		{
			int nEffectValue =atoi((*iterEffectValueBegin).c_str());
			if(nEffectValue<=0)
				continue;

			if(nEffectListIndex<0||nEffectListIndex>=nEffectCountMax)
				continue;

			effectValueList[nEffectListIndex] = nEffectValue;
		}

		nEffectListIndex++;

	}



	UID uid = pTarget->getUID();
	if(uid<=0)
		return;

	EFFECT_VALUE_LIST ListValue;
	for (int i=0;i<(int)nEffectCountMax;++i)
	{    
		int nEffectId= effectIDList[i];
		if(nEffectId<=0)
			continue;

		IEffect *pEffect = pEffectFactory->BuildEffect(nEffectId);
		if (pEffect == NULL)
		{
			// 处理效果加不上的情况
			ErrorLn("TalentEffectProxy::AddRuneEffect() -- EffectID = "<< nEffectId);
			continue;
		}

		int nEffectType = (int)pEffect->GetEffectType();
		if(nEffectType!=(int)EFFECT_CHANGE_VALUE)
		{
			continue;
		}

		int nEffectValue = effectValueList[i];
		int nTotalEffectValue = nEffectValue;

		ListValue.clear();
		ListValue.push_back(nTotalEffectValue);
		IEffectChangeValue*  pEffectChangeValue= ( IEffectChangeValue* )pEffect;
        if (timeToAddEffect == ETIME_TO_ADD_EFFECT::ENTITY_UPGRADE)
        {
            // 核晶加体力的效果在升级的时候会被重置，所以在升级时重新设置一下体力值。
            ENTITY_PROPERTY type = (ENTITY_PROPERTY)pEffectChangeValue->GetValueType();
            if (type != PROPERTY_MAX_STAMINA)
            {
                continue;
            }
        }

        pEffectChangeValue->SetValue(&ListValue);

        EFFECT_CONTEXT context;
        context.pEntity = pTarget;
        context.uidOperator = uid;
        context.uidTarget = uid;
        context.nID = nEffectId;
        pEffect->Start(&context, NULL);
        m_TalentEffectAll.push_back(pEffect);
	}
}