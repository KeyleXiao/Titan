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
#include "IClientGlobal.h"

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

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeEntity::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
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
		return false;
	}

	TiXmlElement *pSubElement = pRootElement->FirstChildElement("spell");
	if(pSubElement == NULL)
	{
        ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find spell");
		return false;
	}
	pSubElement->QueryFloatAttribute("SpellHeight", &m_EntityXml.fSpellHeight);

    // 角色后退速度系数
    pSubElement = pRootElement->FirstChildElement("hero_backward_speed_factor");
    if(pSubElement == NULL)
    {
        ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find hero_backward_speed_factor");
        return false;
    }
    pSubElement->QueryFloatAttribute("factor", &m_EntityXml.fHeroBackwardSpeedFactor);

    // 角色左右移动速度系数
    pSubElement = pRootElement->FirstChildElement("hero_sideway_speed_factor");
    if(pSubElement == NULL)
    {
        ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find hero_sideway_speed_factor");
        return false;
    }
    pSubElement->QueryFloatAttribute("factor", &m_EntityXml.fHeroSidewaySpeedFactor);

	// 伤害光效ID
	pSubElement = pRootElement->FirstChildElement("damage_Lighting");
    if(pSubElement == NULL)
    {
        ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find damage_Lighting");
        return false;
    }
	// 伤害光效
    pSubElement->QueryIntAttribute("LightingID", &m_EntityXml.nDamageLightingID);
    // 持续伤害系数
    pSubElement->QueryFloatAttribute("ContinueDamageLightingFactor", &m_EntityXml.fContinueDamageLightingFactor);
    // 持续伤害光效ID
    pSubElement->QueryIntAttribute("ContinueDamageLightingID", &m_EntityXml.nContinueDamageLightingID);

	// 天赋信息
    pSubElement = pRootElement->FirstChildElement("talent_Info");
    if(pSubElement == NULL)
    {
        ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find talent_Info");
        return false;
    }
    // 天赋最大可使用数
    pSubElement->QueryIntAttribute("Talent_MaxUseCount", &m_EntityXml.nTalentMaxUseCount);

	// XP技能时间
	pSubElement = pRootElement->FirstChildElement("xp_skill");
	if(pSubElement == NULL)
	{
		ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find xp_skill");
		return false;
	}
	// xp技能充能时间
	pSubElement->QueryIntAttribute("CD_Time", &m_EntityXml.nXPSkillTime);

	// 机器人是否战绩炫耀标志位
	pSubElement = pRootElement->FirstChildElement("ai_military_exploits");
	if(pSubElement == NULL)
	{
		ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find talent_Info");
		return false;
	}
	// 机器人是否战绩炫耀标志位
	pSubElement->QueryIntAttribute("AIMilitaryExploitsFlag", &m_EntityXml.nAIMilitaryExploitsFlag);

	// 战场结束后结束界面延迟显示时间
	pSubElement = pRootElement->FirstChildElement("WarEndWndDelayTime");
	if (pSubElement == NULL)
	{
		ErrorLn(__FUNCTION__":szFileName" << szFileName << "FirstChildElement not find WarEndWndDelayTime");
		return false;
	}
	pSubElement->QueryFloatAttribute("DelayTime", &m_EntityXml.fWarEndWndDelayTime);

	// 任务模块开关，默认关闭时0，策划测试配置为1可见，模块稳定后删除
	pSubElement = pRootElement->FirstChildElement("ContactTask");
	if (pSubElement)
	{
		pSubElement->QueryIntAttribute("Switch", &m_EntityXml.nContactTaskSwitch);
	}
	else
	{
		ErrorLn(__FUNCTION__":szFileName" << szFileName << "FirstChildElement not find LifeHero");
		m_EntityXml.nContactTaskSwitch = 0;
	}

    pSubElement = pRootElement->FirstChildElement("MatchBaseScore");
    if ( pSubElement == NULL )
    {
        ErrorLn("szFileName="<<szFileName<<",FirstChildElement not find MatchBaseScore");
    }
    pSubElement->QueryIntAttribute("MatchTypeBaseScore", &m_EntityXml.nMatchTypeBaseScore);

    pSubElement = pRootElement->FirstChildElement("MoveMsgOptimize");
    if (pSubElement)
    {
        pSubElement->Attribute("open", &m_EntityXml.nOpenMovePkgOptimize);
    }

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
	return OnSchemeLoad(reader, szFileName);
}


/////////////////////////ISchemeEntity/////////////////////////
SEntityXml_Client *CSchemeEntity::getEntityXml(void)
{
    return &m_EntityXml;
}