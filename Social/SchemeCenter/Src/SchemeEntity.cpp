/*******************************************************************
** 文件名:	SchemeEntity.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	实体通用配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeEntity.h"
#include "ISocialGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeEntity::CSchemeEntity(void)
{
	memset(&m_EntityXml, 0, sizeof(m_EntityXml));
}

/** 
@param   
@param   
@return  
*/
CSchemeEntity::~CSchemeEntity(void)
{
}

SEntityXml_Server *CSchemeEntity::getEntityXml(void)
{
    return &m_EntityXml;
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeEntity::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gSocialGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( ENTITY_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("config file load failed! filename=" << stringPath.c_str() << endl);
		return false;
	}

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeEntity::Close(void)
{
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
bool CSchemeEntity::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
	TiXmlDocument *pXMLReader = reader.pXMLReader;
	if(pXMLReader == NULL || szFileName == NULL || reader.type!=READER_XML)
	{
		return false;
	}

	TiXmlElement *pRootElement = pXMLReader->FirstChildElement("scheme");
	if(pRootElement == NULL)
	{
         ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find scheme");
		return false;
	}
    TiXmlElement *pSubElement = nullptr;
 //   // 新手引导转镜头加1级BUFF
 //   pSubElement = pRootElement->FirstChildElement("TurnCamera");
 //   if(pSubElement)
 //   {
 //       const char* pText = pSubElement->Attribute("BuffIDList");
 //       if (pText)
 //       {
 //           vector<string> vecText;
 //           StringHelper::split(vecText, pText, ";");
 //           int nCount = 0;
 //           for (vector<string>::iterator iter = vecText.begin(); nCount < CAMERA_BUFF_COUNT && iter != vecText.end(); ++iter)
 //           {
 //               m_EntityXml.nTurnCameraBuffID[nCount++] = StringHelper::toInt(*iter);
 //           }
 //       }
 //   }

	//// 蓄力
	//pSubElement = pRootElement->FirstChildElement("foco_energia");
	//if(pSubElement == NULL)
	//{
 //       ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find foco_energia");
	//	return false;
	//}
	//pSubElement->QueryFloatAttribute("MinFocoEnergiaPCT", &m_EntityXml.fMinFocoEnergiaPCT);
	//if (equalto0(m_EntityXml.fMinFocoEnergiaPCT))
	//{
	//	ErrorLn("CSchemeEntity::OnSchemeLoad fMinFocoEnergiaPCT is 0.0f");
	//	return false;
	//}
	//pSubElement->QueryFloatAttribute("MaxFocoEnergiaPCT", &m_EntityXml.fMaxFocoEnergiaPCT);
	//if (equalto0(m_EntityXml.fMaxFocoEnergiaPCT))
	//{
	//	ErrorLn("CSchemeEntity::OnSchemeLoad fMaxFocoEnergiaPCT is 0.0f");
	//	return false;
	//}
 //   // 角色后退速度因子
 //   pSubElement = pRootElement->FirstChildElement("hero_backward_speed_factor");
 //   if(pSubElement == NULL)
 //   {
 //       ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find hero_backward_speed_factor");
 //       return false;
 //   }
 //   pSubElement->QueryFloatAttribute("factor", &m_EntityXml.fHeroBackwardSpeedFactor);
 //   // 角色斜向速度因子
 //   pSubElement = pRootElement->FirstChildElement("hero_sideway_speed_factor");
 //   if(pSubElement == NULL)
 //   {
 //       ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find hero_sideway_speed_factor");
 //       return false;
 //   }
 //   pSubElement->QueryFloatAttribute("factor", &m_EntityXml.fHeroSidewaySpeedFactor);

 //   pSubElement = pRootElement->FirstChildElement("show_damage");
 //   if(pSubElement == NULL)
 //   {
 //       ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find show_damage");
 //       return false;
 //   }
 //   pSubElement->QueryIntAttribute("DamageInfo", &m_EntityXml.nDamageInfo);

 //   pSubElement = pRootElement->FirstChildElement("show_changematchrank");
 //   if(pSubElement == NULL)
 //   {
 //       ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find show_changematchrank");
 //       return false;
 //   }
 //   pSubElement->QueryIntAttribute("ChangeRankInfo", &m_EntityXml.nChangeRankInfo);
 //	
 //   pSubElement = pRootElement->FirstChildElement("updated_info");
 //   if (pSubElement == NULL )
 //   {
 //       ErrorLn("szFileName = " << szFileName << ", FirstChildElement not find updated_info")
 //       return false;
 //   }
 //   pSubElement->QueryIntAttribute("RecordInfo", &m_EntityXml.nRecordInfo);
 //   pSubElement->QueryIntAttribute("Time", &m_EntityXml.nTime);

 //   pSubElement = pRootElement->FirstChildElement("show_warinfo");
 //   if(pSubElement == NULL)
 //   {
 //       ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find show_warinfo");
 //       return false;
 //   }
 //   pSubElement->QueryIntAttribute("WarShowAddEXPAndMoney", &m_EntityXml.nWarShowAddEXPAndMoney);

 //   pSubElement = pRootElement->FirstChildElement("firstWin_Award");
 //   if ( pSubElement == NULL )
 //   {
 //       ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find show_warinfo");
 //       return false;
 //   }
 //   pSubElement->QueryIntAttribute("AwardPlayerExp",    &m_EntityXml.nFirstWinPlayerExp);   // 首勝玩家經驗
 //   pSubElement->QueryIntAttribute("AwardPlayerGold",   &m_EntityXml.nFirstWinPlayerGold);  // 首勝玩家金幣
 //   pSubElement->QueryIntAttribute("AwardHeroExp",      &m_EntityXml.nFirstWinHeroExp);     // 首勝英雄經驗
 //   pSubElement->QueryIntAttribute("AwardCardCount",    &m_EntityXml.nFirstWinCardCount);   // 首勝卡牌數量
 //   pSubElement->QueryIntAttribute("AwardPrizeID",      &m_EntityXml.nFirstWinPrizeID);     // 首勝奖励ID

	//pSubElement = pRootElement->FirstChildElement("xp_skill_time");
 //   if ( pSubElement == NULL )
 //   {
 //       ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find xp_skill_time");
 //       return false;
 //   }
 //   pSubElement->QueryIntAttribute("CD_Time", &m_EntityXml.nXPCdTime);
 //   pSubElement->QueryIntAttribute("dead_is_disappear", &m_EntityXml.nDeadIsDisappear);

 //   pSubElement = pRootElement->FirstChildElement("spell_out_time");
 //   if ( pSubElement == NULL )
 //   {
 //       ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find spell_out_time");
 //       return false;
 //   }
 //   pSubElement->QueryIntAttribute("SpellAlertOut", &m_EntityXml.nSpellAlertOut);
 //   pSubElement->QueryIntAttribute("SpellAlertOutTime", &m_EntityXml.nSpellAlertOutTime);
 //   pSubElement->QueryIntAttribute("SpellOutTime", &m_EntityXml.nSpellOutTime);

 //   pSubElement = pRootElement->FirstChildElement("mobaAddExpOrTalentLog");
 //   if ( pSubElement == NULL )
 //   {
 //       ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find mobaAddExpOrTalentLog");
 //       return false;
 //   }
 //   pSubElement->QueryIntAttribute("TimeInterval", &m_EntityXml.nAddExpOrTalentLogTimeInterval);
	//pSubElement->QueryIntAttribute("PlanLogIsShow", &m_EntityXml.nIsShowPlanLog);

 //   // 人物每天活跃度上限
	//pSubElement = pRootElement->FirstChildElement("activity");
	//if (pSubElement == NULL)
	//{
	//	ErrorLn("szFileName=" << szFileName << ",FirstChildElement not find activity");
	//	return false;
	//}
	//pSubElement->QueryIntAttribute("MaxDayActivity", &m_EntityXml.nMaxDayActivity);

 //   // 最低穿透系数
 //   pSubElement = pRootElement->FirstChildElement("Penetration");
 //   if (pSubElement)
 //   {
 //       pSubElement->QueryIntAttribute("min", &m_EntityXml.nMinPenetCelt);
 //   }

    pSubElement = pRootElement->FirstChildElement("MatchBaseScore");
    if ( pSubElement == NULL )
    {
        ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find MatchBaseScore");
        return false;
    }
    pSubElement->QueryIntAttribute("MatchTypeBaseScore", &m_EntityXml.nMatchTypeBaseScore);
    
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
bool CSchemeEntity::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeEntity* pNewInstance = NewInstance();
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