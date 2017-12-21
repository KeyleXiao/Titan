/*******************************************************************
** 文件名:	SchemeMobaGoods.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	1/9/2015
** 版  本:	1.0
** 描  述:	

			
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeMobaGoods.h"
#include "IServerGlobal.h"
#include "MobaGoodsSchemeDef.h"
#include <string>
using namespace std;

/** 
@param   
@param   
@return  
*/
CSchemeMobaGoods::CSchemeMobaGoods(void)
{	

}

/** 
@param   
@param   
@return  
*/
CSchemeMobaGoods::~CSchemeMobaGoods(void)
{	
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeMobaGoods::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	m_mapGoodsScheme.clear();
	m_mapRecommendGoodScheme.clear();
	m_mapGoodsSkillFreeze.clear();

	// 载入MOBA物品文件
    string stringPath = SCP_PATH_FORMAT( MOBA_GOODS_SCHEME_FILENAME );
	if(!pSchemeEngine->LoadScheme(stringPath.c_str(),this,true))
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	// 载入推荐装备文件
    stringPath = SCP_PATH_FORMAT( RECOMMEND_GOOD_SCHEME_FILENAME );
	if(!pSchemeEngine->LoadScheme(stringPath.c_str(),this,true))
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	// 载入物品效果类文件
    stringPath = SCP_PATH_FORMAT( GOOD_EFFECT_GROUPTYPE_SCHEME_FILENAME );
	if(!pSchemeEngine->LoadScheme(stringPath.c_str(),this,true))
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeMobaGoods::Close(void)
{	
	// 清空
	m_mapGoodsScheme.clear();
	m_mapRecommendGoodScheme.clear();
	m_mapGoodsSkillFreeze.clear();
}

/** CSV配置载入时通知
@param   pCSVReader：读取CSV的返回接口
@param   szFileName：配置文件名
@param   
@return  
@note     
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/ 
bool CSchemeMobaGoods::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{

	bool bRet = false;

    string strMGoodsPath = SCP_PATH_FORMAT( MOBA_GOODS_SCHEME_FILENAME );
    string strRecomdPath = SCP_PATH_FORMAT( RECOMMEND_GOOD_SCHEME_FILENAME );
    string strEffectPath = SCP_PATH_FORMAT( GOOD_EFFECT_GROUPTYPE_SCHEME_FILENAME );

	if(rkt::StringHelper::casecmp(szFileName, strMGoodsPath.c_str()) == 0)
	{
		bRet = LoadMobaGoodsScheme(reader);
	}
	else if(rkt::StringHelper::casecmp(szFileName, strRecomdPath.c_str()) == 0)
	{
		bRet = LoadRecommendGoodScheme(reader);
	}
	else if(rkt::StringHelper::casecmp(szFileName, strEffectPath.c_str()) == 0)
	{
		bRet = LoadGoodEffectGroupTypeScheme(reader);
	}

	if(!bRet)
	{
		return false;
	}

	AfterSchemeLoaded();

	return true;
}

/** 配置动态更新时通知
@param   pCSVReader：读取CSV的返回接口
@param   szFileName：配置文件名
@param   
@return  
@note     
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/
bool CSchemeMobaGoods::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeMobaGoods* pNewInstance = NewInstance();
	if(pNewInstance == NULL)
	{
		return false;
	}

	if(pNewInstance->OnSchemeLoad(reader, szFileName))
	{
		return true;
	}
	else
	{
		safeDelete(pNewInstance);
		return false;
	}
}


/////////////////////////ISchemeMobaGoods/////////////////////////
/** 取得怪物配置信息
@param nMonsterId:	怪物ID
@return  
*/
SSchemeMobaGoods* CSchemeMobaGoods::getMobaGoodsShemeInfo(int nGoodId)
{
	TMAP_GOODSCHEME::iterator iter = m_mapGoodsScheme.find(nGoodId);
	if (iter == m_mapGoodsScheme.end())
	{
		return NULL;
	}
	return &(iter->second);
}

SSchemeRecommendGood* CSchemeMobaGoods::getRecommandGoods(int nHeroID, int nMatchType)
{
	TMAP_RECOMMENDGOODSCHEME::iterator iter = m_mapRecommendGoodScheme.find(nHeroID);
	if (iter == m_mapRecommendGoodScheme.end())
		return NULL;

	list<SSchemeRecommendGood>::iterator iter2 = iter->second.begin();
	for (; iter2 != iter->second.end(); ++iter2)
	{
		for (int i = 0; i < MatchType_MaxID; ++i)
		{
			if (iter2->nMatchTypeList[i] == nMatchType)
			{
				return &(*iter2);
			}
		}
	}

	return NULL;
}

bool CSchemeMobaGoods::LoadMobaGoodsScheme(SCRIPT_READER reader)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_mapGoodsScheme.clear();

	int nLen = 0;
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		// 当前列数
		int nCol = 0;

		SSchemeMobaGoods item;
		ZeroMemory(&item,sizeof(SSchemeMobaGoods)); 

		// 物品ID
		item.nGoodsID = pCSVReader->GetInt(nRow, nCol++, 0);

		// 名称
		nLen = sizeof(item.szName);
		pCSVReader->GetString(nRow, nCol++, item.szName, nLen);

		// 物品ID 1、装备 2、物品
		item.nTypeID = (emMobaGoodType)pCSVReader->GetInt(nRow, nCol++, 0);

		// 装备图标ID
		item.nIconID = pCSVReader->GetInt(nRow, nCol++, 0);
	
		// 装备组标签
		item.nLabelGourp = pCSVReader->GetInt(nRow, nCol++, 0);

		// 装备组子标签
		item.nLabel = pCSVReader->GetInt(nRow, nCol++, 0);

		//是否英雄专属	
		item.bIsHeroLimit = pCSVReader->GetInt(nRow, nCol++, 0)> 0? true: false;

		//英雄ID
		item.nHeroID = pCSVReader->GetInt(nRow, nCol++, 0);

		//职业限制
		item.nVocationTypeLimit = pCSVReader->GetInt(nRow, nCol++, 0);

		// 合成需要装备
		char strNeedComposeList[128] = {0};
		nLen = sizeof(strNeedComposeList);
		pCSVReader->GetString(nRow, nCol++, strNeedComposeList, nLen);
		parseIntArray(strNeedComposeList, item.NeedComposeList, COMPOSE_MAX_COUNT, ';');

		// 能合装备
		char strCanComposeList[128] = {0};
		nLen = sizeof(strCanComposeList);
		pCSVReader->GetString(nRow, nCol++, strCanComposeList, nLen);
		parseIntArray(strCanComposeList, item.CanComposeList, COMPOSE_MAX_COUNT, ';');

		// 绝对价格
		item.nGoodCost = pCSVReader->GetInt(nRow, nCol++, 0);

		// 出售价格
		item.nSoldOutCost = pCSVReader->GetInt(nRow, nCol++, 0);

		// 基础效果ID组 可以叠加的效果ID
		char strEffectList[128] = {0};
		int EffectList[MAX_EFFECT_COUNT] = {0};
		nLen = sizeof(strEffectList);
		pCSVReader->GetString(nRow, nCol++, strEffectList, nLen);
		int nEffectCount = parseIntArray(strEffectList, EffectList, MAX_EFFECT_COUNT, ';');

		// 效果列表类型: 0 基础属性 1 为同ID不叠加, 2 为同类型不叠加
		char strEffectListType[128] = {0};
		int EffectListType[MAX_EFFECT_COUNT] = {0};
		nLen = sizeof(strEffectListType);
		pCSVReader->GetString(nRow, nCol++, strEffectListType, nLen);
		int nEffectTypeCount = parseIntArray(strEffectListType, EffectListType, MAX_EFFECT_COUNT, ';');

		if (nEffectCount != nEffectTypeCount)
		{
			ErrorLn("CSchemeMobaGoods::LoadMobaGoodsScheme() -- nEffectCount != nEffectTypeCount");
			return false;
		}

		//最大数量限制
		item.nEffectCount = nEffectCount > MAX_EFFECT_COUNT ? MAX_EFFECT_COUNT: nEffectCount;
		for (int n = 0; n < nEffectCount; ++n)
		{
			SGoodEffectInfo GoodEffectInfo;
			GoodEffectInfo.nEffectID = EffectList[n];
			GoodEffectInfo.nEffectType = (emMobaGoodEffectType)EffectListType[n];
			item.GoodEffectInfo[n] = GoodEffectInfo;
		}

		// 物品移除调用效果
		char strRemoveEffect[128] = {0};
		int RemoveEffectList[MAX_EFFECT_COUNT] = {0};
		nLen = sizeof(strRemoveEffect);
		pCSVReader->GetString(nRow, nCol++, strRemoveEffect, nLen);
		int nRemoveEffectCount = parseIntArray(strRemoveEffect, RemoveEffectList, MAX_EFFECT_COUNT, ';');
		if (MAX_EFFECT_COUNT < nRemoveEffectCount)
		{
			nRemoveEffectCount = MAX_EFFECT_COUNT;
		}

		for (int n = 0; n < nRemoveEffectCount; ++n)
		{
			item.GoodRemoveEffectInfo[n] = RemoveEffectList[n];
		}

        // 继承效果ID组
        char strInheritEffectList[128] = {0};
        int InheritEffectList[MAX_EFFECT_COUNT] = {0};
        nLen = sizeof(strInheritEffectList);
        pCSVReader->GetString(nRow, nCol++, strInheritEffectList, nLen);
        int nInheritEffectCount = parseIntArray(strInheritEffectList, InheritEffectList, MAX_EFFECT_COUNT, ';');

        // 效果列表类型: 0 基础属性 1 为同ID不叠加, 2 为同类型不叠加
        char strInheritEffectListType[128] = {0};
        int InheritEffectListType[MAX_EFFECT_COUNT] = {0};
        nLen = sizeof(strInheritEffectListType);
        pCSVReader->GetString(nRow, nCol++, strInheritEffectListType, nLen);
        int nInheritEffectTypeCount = parseIntArray(strInheritEffectListType, InheritEffectListType, MAX_EFFECT_COUNT, ';');

        if (nInheritEffectCount != nInheritEffectTypeCount)
        {
            ErrorLn("CSchemeMobaGoods::LoadMobaGoodsScheme() -- nInheritEffectCount != nInheritEffectTypeCount");
            return false;
        }

        //最大数量限制
        item.nInheritEffectCount = nInheritEffectCount > MAX_EFFECT_COUNT ? MAX_EFFECT_COUNT: nInheritEffectCount;
        for (int n = 0; n < nInheritEffectCount; ++n)
        {
            SGoodEffectInfo GoodEffectInfo;
            GoodEffectInfo.nEffectID = InheritEffectList[n];
            GoodEffectInfo.nEffectType = (emMobaGoodEffectType)InheritEffectListType[n];
            item.InheritGoodEffectInfo[n] = GoodEffectInfo;
        }

        // 物品移除调用效果
        char strInheritRemoveEffect[128] = {0};
        int InheritRemoveEffectList[MAX_EFFECT_COUNT] = {0};
        nLen = sizeof(strInheritRemoveEffect);
        pCSVReader->GetString(nRow, nCol++, strInheritRemoveEffect, nLen);
        int nInheritRemoveEffectCount = parseIntArray(strInheritRemoveEffect, InheritRemoveEffectList, MAX_EFFECT_COUNT, ';');
        if (MAX_EFFECT_COUNT < nInheritRemoveEffectCount)
        {
            nInheritRemoveEffectCount = MAX_EFFECT_COUNT;
        }

        for (int n = 0; n < nRemoveEffectCount; ++n)
        {
            item.InheritGoodRemoveEffectInfo[n] = InheritRemoveEffectList[n];
        }
		
		// 物品战场限制
		char strWarLimit[128] = {0};
		nLen = sizeof(strWarLimit);
		pCSVReader->GetString(nRow, nCol++, strWarLimit, nLen );
		parseIntArray(strWarLimit, item.WarLimit, WAR_LIMIT_MAX, ';');

		//自动使用(格子不够则自动使用)
		item.eAutoUseType = (emGoodAutoUseType)pCSVReader->GetInt(nRow, nCol++, 0);

		//是否特殊槽位物品
		item.bIsSpecialSlotGood = pCSVReader->GetInt(nRow, nCol++, 0)> 0? true: false;

		//能否使用
		item.bCanUse = pCSVReader->GetInt(nRow, nCol++, 0)> 0? true: false;
		
		//使用是否移除	
		item.bUsedRmove = pCSVReader->GetInt(nRow, nCol++, 0)> 0? true: false;

		//是否能叠加
		item.bCanOverlay = pCSVReader->GetInt(nRow, nCol++, 0)> 0? true: false;

		//最大叠加数量
		item.nOverlayCount = pCSVReader->GetInt(nRow, nCol++, 0);

		// 最大拥有数量
		item.nMaxCount = pCSVReader->GetInt(nRow, nCol++, 0);

		//使用BuffID
		item.nUseBuffID = pCSVReader->GetInt(nRow, nCol++, 0);

		//使用Buff技能冷却ID
		item.nUseBuffFreezeID = pCSVReader->GetInt(nRow, nCol++, 0);

		// 使用技能ID
		item.nUseSkillID = pCSVReader->GetInt(nRow, nCol++, 0);

		//生成物品技能ID 对应的FreezeID
		if (item.nUseSkillID != 0)
		{
			m_mapGoodsSkillFreeze.insert(pair<int,int>(item.nUseSkillID, item.nUseBuffFreezeID));
		}
		

		// 可使用次数
		item.nCountCanUse = pCSVReader->GetInt(nRow, nCol++, 0);

		// 可使用次数对应图标组
		char strCanUseIconList[256] = {0};
		nLen = sizeof(strCanUseIconList);
		pCSVReader->GetString(nRow, nCol++, strCanUseIconList, nLen);
		parseIntArray(strCanUseIconList, item.nCanUseIconList, MAX_USE_ICON_LIST_COUNT, ';');	

		// 购买界面装备描述
		nLen = sizeof(item.szDescrption);
		pCSVReader->GetString(nRow, nCol++, item.szDescrption, nLen);

		// 物品栏描述
		nCol++;

		// 贵重度
		item.nPreciousDegree = pCSVReader->GetInt(nRow, nCol++, 0);

        // 标识 MOBA_GOODS_FLAG
        item.nFlag = pCSVReader->GetInt(nRow, nCol++, 0);

		// 插入到列表中
		m_mapGoodsScheme[item.nGoodsID] = item;		
	}

	return true;
}

bool CSchemeMobaGoods::LoadRecommendGoodScheme(SCRIPT_READER reader)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_mapRecommendGoodScheme.clear();

	int nLen = 0;
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		// 当前列数
		int nCol = 0;

		SSchemeRecommendGood item;
		ZeroMemory(&item,sizeof(SSchemeRecommendGood)); 

		//英雄ID 
		item.nHeroID = pCSVReader->GetInt(nRow, nCol++, 0);

		//出门装 
		char strEerlyStage[128] = {0};
		nLen = sizeof(strEerlyStage);
		pCSVReader->GetString(nRow, nCol++, strEerlyStage, nLen);
		parseIntArray(strEerlyStage, item.EerlyStage, RECOMMEND_GOODS_MAX_COUNT, ';');


		//中期核心
		char strMiddleStage[128] = {0};
		nLen = sizeof(strMiddleStage);
		pCSVReader->GetString(nRow, nCol++, strMiddleStage, nLen);
		parseIntArray(strMiddleStage, item.MiddleStage, RECOMMEND_GOODS_MAX_COUNT, ';');

		//顺风神装
		char strDownWindStage[128] = {0};
		nLen = sizeof(strDownWindStage);
		pCSVReader->GetString(nRow, nCol++, strDownWindStage, nLen);
		parseIntArray(strDownWindStage, item.DownWindStage, RECOMMEND_GOODS_MAX_COUNT, ';');

		//逆风神装 
		char strUpWindStage[128] = {0};
		nLen = sizeof(strUpWindStage);
		pCSVReader->GetString(nRow, nCol++, strUpWindStage, nLen);
		parseIntArray(strUpWindStage, item.UpWindStage, RECOMMEND_GOODS_MAX_COUNT, ';');

		//消耗品
		char strConsumeGoods[128] = {0};
		nLen = sizeof(strConsumeGoods);
		pCSVReader->GetString(nRow, nCol++, strConsumeGoods, nLen);
		parseIntArray(strConsumeGoods, item.ConsumeGoods, RECOMMEND_GOODS_MAX_COUNT, ';');

		// 默认页
		nCol++;

		// 比赛类型组合
		char strMatchTypeList[128] = { 0 };
		nLen = sizeof(strMatchTypeList);
		pCSVReader->GetString(nRow, nCol++, strMatchTypeList, nLen);
		parseIntArray(strMatchTypeList, item.nMatchTypeList, MatchType_MaxID, ';');

		// 插入到列表中
		m_mapRecommendGoodScheme[item.nHeroID].push_back(item);		
	}
	return true;
}

SSchemeMobaGoods* CSchemeMobaGoods::getSchemeMobaGoodsInfo(int nGoodID)
{
	if (INVALID_MOBA_GOOD_ID == nGoodID)
	{
		return nullptr;
	}
	
	TMAP_GOODSCHEME::iterator iter = m_mapGoodsScheme.find(nGoodID);
	if (iter != m_mapGoodsScheme.end())
	{
		return &(iter->second);
	}

	return nullptr;
	 
}

bool CSchemeMobaGoods::LoadGoodEffectGroupTypeScheme(SCRIPT_READER reader)
{
	ICSVReader * pCSVReader = reader.pCSVReader;
	if(pCSVReader == NULL || reader.type!=READER_CSV)
	{
		return false;
	}

	// 清空
	m_mapGoodEffectGroupTypeScheme.clear();

	int nLen = 0;
	// 读取
	int nRecordCount = pCSVReader->GetRecordCount();
	for (int nRow=0; nRow<nRecordCount; ++nRow)
	{
		// 当前列数
		int nCol = 0;

		SSchemeGoodEffectGroupType item;
		ZeroMemory(&item,sizeof(SSchemeGoodEffectGroupType)); 

		// 名称
		nLen = sizeof(item.szName);
		pCSVReader->GetString(nRow, nCol++, item.szName, nLen);

		// 效果所属于ID组
		item.nEffectGroupID = pCSVReader->GetInt(nRow, nCol++, 0);

		//效果ID 
		item.nEffectID = pCSVReader->GetInt(nRow, nCol++, 0);
		// 插入到列表中
		m_mapGoodEffectGroupTypeScheme[item.nEffectID] = item;		
	}
	return true;
}

SSchemeGoodEffectGroupType* CSchemeMobaGoods::getSchemeMobaGoodsPropInfo(int nEffectID)
{
	if (nEffectID <= 0)
	{
		return nullptr;
	}

	TMAP_EFFECTGROUPGOODSCHEME::iterator iter = m_mapGoodEffectGroupTypeScheme.find(nEffectID);
	if (iter != m_mapGoodEffectGroupTypeScheme.end())
	{
		return &(iter->second);
	}

	return nullptr;
}

void CSchemeMobaGoods::getSchemeNeedComposeList(int nGoodID, int *NeedComposeList, int &nCount)
{
	nCount = 0;
	if (nGoodID == 0)
	{
		return;
	}

	// 找到配置

	TMAP_GOODSCHEME::iterator iter = m_mapGoodsScheme.find(nGoodID);
	if (iter == m_mapGoodsScheme.end())
	{
		return;
	}
	
	for(int i = 0; i < COMPOSE_MAX_COUNT; ++i)
	{
		if (iter->second.NeedComposeList[i] != 0)
		{
			NeedComposeList[nCount++] = iter->second.NeedComposeList[i];
		}
		 
	}
}

int CSchemeMobaGoods::getGoodsSkillFreeze(int nGoodsSkillID)
{
	TMAP_GOODSSKILLFREEZE::iterator iter =  m_mapGoodsSkillFreeze.find(nGoodsSkillID);
	if (iter != m_mapGoodsSkillFreeze.end())
	{
		return iter->second;
	}

	return 0;
}

