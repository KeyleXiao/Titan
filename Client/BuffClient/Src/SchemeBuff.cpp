/*******************************************************************
** 文件名:	SchemeBuff.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/12  21:53
** 版  本:	1.0
** 描  述:	buff配置脚本
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "SchemeBuff.h"
#include "ExternalFacade.h"

/** 
@param   
@param   
@return  
*/
CSchemeBuff::CSchemeBuff(void)
{
	// 所有BUFF配置属性	
	m_mapBuffSchemeInfo.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemeBuff::~CSchemeBuff(void)
{
	// 所有地图配置属性	
	m_mapBuffSchemeInfo.clear();
}

/** 
@param   
@param   
@return  
*/
bool CSchemeBuff::LoadScheme(void)
{
	ISchemeEngine * pSchemeEngine = g_ExternalFacade.GetSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( BUFF_SCHEME_FILENAME );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
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
void CSchemeBuff::Close(void)
{	
	// 所有地图配置属性	
	m_mapBuffSchemeInfo.clear();
}


/** 取得地图配置信息
@param   
@param   
@return  
*/
SBuffSchemeInfo * CSchemeBuff::GetBuffSchemeInfo(DWORD dwBuffID)
{
	TMAP_BUFFSCHEMEINFO::iterator itM = m_mapBuffSchemeInfo.find(dwBuffID);
	if(itM != m_mapBuffSchemeInfo.end())
	{
		return &((*itM).second);
	}

	return NULL;
}


/** 取得所有配置信息
@param   
@param   
@return  
*/
TMAP_BUFFSCHEMEINFO * CSchemeBuff::GetBuffSchemeInfo(void)
{
	return &m_mapBuffSchemeInfo;
}


/////////////////////////ISchemeUpdateSink/////////////////////////
bool CSchemeBuff::OnSchemeLoad(SCRIPT_READER reader, const char* szFileName)
{
	TiXmlDocument *pXMLReader = reader.pXMLReader;
	if(pXMLReader == NULL || szFileName == NULL || reader.type!=READER_XML)
	{
		return false;
	}

	// 所有地图配置属性	
	m_mapBuffSchemeInfo.clear();

	TiXmlElement * pRootElement = pXMLReader->FirstChildElement("scheme");
	if(pRootElement == NULL)
	{
		return false;
	}

	// [buff]
	TiXmlElement * pSubElement = pRootElement->FirstChildElement("buff");
	if(pSubElement == NULL)
	{
		return false;
	}

	DWORD dwTempValue = 0;
	LPCSTR pszAttrProp = NULL;
	char szAttributeName[256] = { 0 }; 
	TiXmlElement * pChildElement = NULL;
	TiXmlElement * pGrandchildElement = NULL;
	int nQueryAccelerateCount = 0;
	while(pSubElement)
	{
		SBuffSchemeInfo buffschemeinfo;

		// BUFF ID
		pszAttrProp = pSubElement->Attribute("buffid", (int *)&buffschemeinfo.dwBuffID);
		if(pszAttrProp == NULL || buffschemeinfo.dwBuffID <= INVALID_BUFF_ID)		
		{
			ErrorLn("buffid Can not be less than or equal to 0! buffid="<<buffschemeinfo.dwBuffID);
			return false;
		}

		// BUFF ID不能大于0xffff，否则存数据库会出问题
		if(buffschemeinfo.dwBuffID >= 0xFFFF)
		{
			ErrorLn("buffid Cannot be greater than or equal to 0xFFFF! buffid="<<buffschemeinfo.dwBuffID);
			return false;
		}

		// 不能有重复的buff id
		if(GetBuffSchemeInfo(buffschemeinfo.dwBuffID) != NULL)
		{
			ErrorLn("repeated buffid.buffid="<<buffschemeinfo.dwBuffID);
			return false;
		}

		// buff名字
		pszAttrProp = pSubElement->Attribute("buffname");
		if(pszAttrProp == NULL)
		{
			return false;
		}
		sstrcpyn(buffschemeinfo.szName, pszAttrProp, sizeof(buffschemeinfo.szName));

		// [base]
		pChildElement = pSubElement->FirstChildElement("base");
		if(pChildElement == NULL)
		{
			return false;
		}

		// 叠加规则
		pszAttrProp = pChildElement->Attribute("replacerule", (int *)&buffschemeinfo.dwReplaceRule);
		if(pszAttrProp == NULL)		
		{
			return false;
		}

		// 叠加数量
		pszAttrProp = pChildElement->Attribute("pileqty", (int *)&buffschemeinfo.nPileQty);
		if(pszAttrProp == NULL || buffschemeinfo.nPileQty < 0 || buffschemeinfo.nPileQty >= 1024)
		{
			ErrorLn("Invalid number of stacked.buffid="<<buffschemeinfo.dwBuffID);
			return false;
		}	

		// 标志
		pszAttrProp = pChildElement->Attribute("flag", (int *)&buffschemeinfo.dwFlag);
		if(pszAttrProp == NULL)
		{
			return false;
		}

		// 结束条件
		for(int i = 0; i < 32; i++)
		{
			sprintf(szAttributeName, "endflag%d", i);

			// endflag%d
			pszAttrProp = pChildElement->Attribute(szAttributeName, (int *)&dwTempValue);
			if(pszAttrProp == NULL)
			{
				break;
			}
			buffschemeinfo.listEndFlag.push_back(dwTempValue);
		}

		// 图标
		pszAttrProp = pChildElement->Attribute("iconid", &buffschemeinfo.nIconID);
		if(pszAttrProp == NULL)
		{
			return false;
		}

		// [sub]
		pChildElement = pSubElement->FirstChildElement("sub");
		if(pChildElement == NULL)
		{
			return false;
		}

		while(pChildElement)
		{
			SBuffSchemeInfo::SLevelInfo levelinfo;

			// 等级
			pszAttrProp = pChildElement->Attribute("level", (int *)&levelinfo.dwBuffLevel);
			if(pszAttrProp == NULL)
			{
				return false;
			}

			// 不能有重复等级
			list<SBuffSchemeInfo::SLevelInfo>::iterator itL = buffschemeinfo.listLevelInfo.begin();
			for( ; itL != buffschemeinfo.listLevelInfo.end(); ++itL)
			{
				SBuffSchemeInfo::SLevelInfo * pLevelInfo = &(*itL);
				if(pLevelInfo->dwBuffLevel == levelinfo.dwBuffLevel)
				{
					ErrorLn("Having the same level of buff.buffid="<<buffschemeinfo.dwBuffID<<". bufflevel="<<levelinfo.dwBuffLevel);
					return false;
				}
			}

			// 描述
			pszAttrProp = pChildElement->Attribute("desc");
			if(pszAttrProp == NULL)
			{
				return false;
			}
			sstrcpyn(levelinfo.szDesc, pszAttrProp, sizeof(levelinfo.szDesc));

			// [flasheffect]
			pGrandchildElement = pChildElement->FirstChildElement("flasheffect");
			if(pGrandchildElement == NULL)
			{
				return false;
			}

			// 光效ID
			pszAttrProp = pGrandchildElement->Attribute("id", (int *)&levelinfo.dwFlashID);
			if(pszAttrProp == NULL)
			{
				return false;
			}

			//  光效ID 扩展
			pszAttrProp = pGrandchildElement->Attribute("idex", (int *)&levelinfo.dwFlashIDEx);
			if(pszAttrProp == NULL)
			{
				return false;
			}

			// [time]
			pGrandchildElement = pChildElement->FirstChildElement("time");
			if(pGrandchildElement == NULL)
			{
				return false;
			}

			// 持续时间
			pszAttrProp = pGrandchildElement->Attribute("long", (int *)&levelinfo.dwTimeLong);
			if(pszAttrProp == NULL)
			{
				return false;
			}

			// 持续时间 扩展
			pszAttrProp = pGrandchildElement->Attribute("longex", (int *)&levelinfo.dwTimeLongEx);
			if(pszAttrProp == NULL)
			{
				return false;
			}

			// [effect]
			pGrandchildElement = pChildElement->FirstChildElement("effectClient");
			if(pGrandchildElement == NULL)
			{
				return false;
			}

			// 效果表
			for(int i = 0; i < 128; i++)
			{
				sprintf(szAttributeName, "id%d", i);

				// id%d
				pszAttrProp = pGrandchildElement->Attribute(szAttributeName, (int *)&dwTempValue);
				if(pszAttrProp == NULL)
				{
					break;
				}

				// 检查是否有些效果ID
				if(!g_ExternalFacade.IsExistEffectID(dwTempValue))
				{
					Error("The effect of the BUFF configuration tables" << dwTempValue << "ID number effect does not exist!" << endl);
					return false;
				}

				levelinfo.listEffect.push_back(dwTempValue);
			}

			// [startrule]
			pGrandchildElement = pChildElement->FirstChildElement("startrule");
			if(pGrandchildElement == NULL)
			{
				return false;
			}

			// BUFF条件
			for(int i = 0; i < 128; i++)
			{
				sprintf(szAttributeName, "hasbuffid%d", i);

				// hasbuffid%d
				pszAttrProp = pGrandchildElement->Attribute(szAttributeName, (int *)&dwTempValue);
				if(pszAttrProp == NULL)
				{
					break;
				}

				if(dwTempValue != INVALID_BUFF_ID)
				{
					levelinfo.listHasBuff.push_back(dwTempValue);
				}
			}

			// BUFF条件 检查方式
			pszAttrProp = pGrandchildElement->Attribute("hascheck", (int *)&levelinfo.dwHasCheck);
			if(pszAttrProp == NULL)
			{
				return false;
			}

			// 非BUFF条件
			for(int i = 0; i < 128; i++)
			{
				sprintf(szAttributeName, "nobuffid%d", i);

				// nobuffid%d
				pszAttrProp = pGrandchildElement->Attribute(szAttributeName, (int *)&dwTempValue);
				if(pszAttrProp == NULL)
				{
					break;
				}

				if(dwTempValue != INVALID_BUFF_ID)
				{
					levelinfo.listNoBuff.push_back(dwTempValue);
				}
			}

			// 非BUFF条件 检查方式
			pszAttrProp = pGrandchildElement->Attribute("nocheck", (int *)&levelinfo.dwNoCheck);
			if(pszAttrProp == NULL)
			{
				return false;
			}

			// [dispelbuff]
			pGrandchildElement = pChildElement->FirstChildElement("dispelbuff");
			if(pGrandchildElement == NULL)
			{
				return false;
			}

			// 驱散BUFF
			for(int i = 0; i < 128; i++)
			{
				sprintf(szAttributeName, "buffid%d", i);

				// buffid%d
				pszAttrProp = pGrandchildElement->Attribute(szAttributeName, (int *)&dwTempValue);
				if(pszAttrProp == NULL)
				{
					break;
				}

				if(dwTempValue != INVALID_BUFF_ID)
				{
					levelinfo.listDispel.push_back(dwTempValue);
				}
			}

			// 驱散概率
			pszAttrProp = pGrandchildElement->Attribute("per", (int *)&levelinfo.dwDispelPer);
			if(pszAttrProp == NULL)
			{
				return false;
			}

			// [rand]
			pGrandchildElement = pChildElement->FirstChildElement("rand");
			if(pGrandchildElement == NULL)
			{
				return false;
			}

			// 随机效果
			for(int i = 0; i < 128; i++)
			{
				sprintf(szAttributeName, "effectid%d", i);

				// effectid%d
				pszAttrProp = pGrandchildElement->Attribute(szAttributeName, (int *)&dwTempValue);
				if(pszAttrProp == NULL)
				{
					break;
				}

				// 检查是否有些效果ID
				if(!g_ExternalFacade.IsExistEffectID(dwTempValue))
				{
					Error("BUFF configuration of random effects in the table" << dwTempValue << "ID number effect does not exist!" << endl);
					return false;
				}

				levelinfo.listRandEffect.push_back(dwTempValue);
			}

			// 随机效果概率
			pszAttrProp = pGrandchildElement->Attribute("per", (int *)&levelinfo.dwRandPer);
			if(pszAttrProp == NULL)
			{
				return false;
			}

			// 随机效果光效
			pszAttrProp = pGrandchildElement->Attribute("flasheffectid", (int *)&levelinfo.dwRandFlashID);
			if(pszAttrProp == NULL)
			{
				return false;
			}

			// [endarose]
			pGrandchildElement = pChildElement->FirstChildElement("endarose");
			if(pGrandchildElement == NULL)
			{
				return false;
			}

			// 后续触发BUFF：id, level,id, level, id, level
			for(int i = 0; i < 128; i++)
			{
				sprintf(szAttributeName, "id%d", i);

				// id%d
				pszAttrProp = pGrandchildElement->Attribute(szAttributeName, (int *)&dwTempValue);
				if(pszAttrProp == NULL || dwTempValue == INVALID_BUFF_ID)
				{
					break;
				}
				levelinfo.listEndAroseBuff.push_back(dwTempValue);

				sprintf(szAttributeName, "le%d", i);

				// le%d
				pszAttrProp = pGrandchildElement->Attribute(szAttributeName, (int *)&dwTempValue);
				if(pszAttrProp == NULL)
				{
					return false;
				}
				levelinfo.listEndAroseBuff.push_back(dwTempValue);
			}

			// 后续触发概率
			pszAttrProp = pGrandchildElement->Attribute("per", (int *)&levelinfo.dwEndArosePer);
			if(pszAttrProp == NULL)
			{
				return false;
			}

			// 配了查询加速 分配查询索引
			levelinfo.nAccQueryIndex = (buffschemeinfo.dwFlag & tBuff_TypeFlag_QueryAccelerate) ? nQueryAccelerateCount++ : -1;

			// buff各等级配置
			buffschemeinfo.listLevelInfo.push_back(levelinfo);

			// 读取下一个
			pChildElement = pChildElement->NextSiblingElement("sub");
		}


		// 压入map
		m_mapBuffSchemeInfo[buffschemeinfo.dwBuffID] = buffschemeinfo;

		// 读取下一个
		pSubElement = pSubElement->NextSiblingElement("buff");
	}

	return true;
}


bool CSchemeBuff::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}