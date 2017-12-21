/*******************************************************************
** 文件名:	SchemeMatchRoomRule.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	4/01/2015
** 版  本:	1.0
** 描  述:	实体通用配置
********************************************************************/

#include "StdAfx.h"
#include "SchemeMatchRoomRule.h"
#include "ISocialGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeMatchRoomRule::CSchemeMatchRoomRule(void)
{
	
}

/** 
@param   
@param   
@return  
*/
CSchemeMatchRoomRule::~CSchemeMatchRoomRule(void)
{
}

/** 载入脚本
@param   
@param   
@return  
*/
bool CSchemeMatchRoomRule::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gSocialGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
    string stringPath = SCP_PATH_FORMAT( DMATCHROOM_SCHEME_RULE );
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		ErrorLn(__FUNCTION__":config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	return true;	
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeMatchRoomRule::Close(void)
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
bool CSchemeMatchRoomRule::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
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

	TiXmlElement * pChildElement = NULL;
	TiXmlElement * pGrandchildElement = NULL;
	LPCSTR pszAttrProp = NULL;

	TiXmlElement * pSubElement = pRootElement->FirstChildElement("selectherorule");
	if(pSubElement == NULL)
	{
		return false;
	}

	while(pSubElement)
	{
		SchemeMatchRoomRule RuleInfo;

		// RuleID
		pszAttrProp = pSubElement->Attribute("RuleID", (int *)&RuleInfo.nRuleID);
		if(pszAttrProp == NULL || RuleInfo.nRuleID <= 0)		
		{
			ErrorLn("nRuleID < = 0! nWarID="<<RuleInfo.nRuleID);
			return false;
		}

		// 不能有重复的RuleID
		if(getSchemeMatchRoomRule(RuleInfo.nRuleID) != NULL)
		{
			ErrorLn("repeated nRuleID="<<RuleInfo.nRuleID);
			return false;
		}

		// [step]
		int i = 0;
		TiXmlElement * pStepElement = pSubElement->FirstChildElement("step");
		while (pStepElement)
		{
			if (i >= MAX_RULE_STEP)
			{
				ErrorLn("step count over MAX_RULE_STEP");
				return false;
			}
			SRuleStep * pSRuleSetp = &(RuleInfo.RuleStep[i]);
			pStepElement->Attribute("StepId", (int *)&(pSRuleSetp->nStepId));
			pStepElement->Attribute("TypeID", (int *)&(pSRuleSetp->nTypeID));
			pStepElement->Attribute("TimeInterval", (int *)&(pSRuleSetp->nTimeInterval));
			pStepElement->Attribute("PosCount", (int *)&(pSRuleSetp->nPosCount));

			int nEnd = 0;
			pStepElement->Attribute("End", (int *)&(nEnd));
			pSRuleSetp->bEnd = (nEnd == 1)? true: false;

			//读取控制位置
			TiXmlElement * pPosElement = pStepElement->FirstChildElement("pos");
            int t = 0;
			if (pPosElement != NULL)
			{
				// 设置step 控制哪些位置
				
				while (pPosElement)
				{
					if (t >= MAX_ROOM_POS)
					{
						ErrorLn("pos count over MAX_ROOM_POS");
						return false;
					}
					pPosElement->Attribute("PosId", (int *)&(pSRuleSetp->nPos[t]));
					t++;
					pPosElement = pPosElement->NextSiblingElement("pos");
				}
				
                if (pSRuleSetp->nPosCount != t)
                {
                    ErrorLn("pos count not Match!please check the config "<<"step id ="<< i<< " file = "<<SCP_PATH_FORMAT( szFileName ));
                    return false;
                }
			}

			// 读取下一个
			i++;
			pStepElement = pStepElement->NextSiblingElement("step");
		}
		  
		// 压入map
		m_mapSchemeRule[RuleInfo.nRuleID] = RuleInfo;

		// 读取下一个
		pSubElement = pSubElement->NextSiblingElement("selectherorule");
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
bool CSchemeMatchRoomRule::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
	CSchemeMatchRoomRule* pNewInstance = NewInstance();
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

SchemeMatchRoomRule* CSchemeMatchRoomRule::getSchemeMatchRoomRule( int RuleID )
{
	map<int,SchemeMatchRoomRule>::iterator iter = m_mapSchemeRule.find(RuleID);
	if (iter != m_mapSchemeRule.end())
	{
		return &(iter->second);
	}
	return NULL;
}
