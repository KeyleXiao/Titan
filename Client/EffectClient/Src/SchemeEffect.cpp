/*******************************************************************
** 文件名:	e:\Rocket\Server\EffectServer\Src\SchemeEffect.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/19  15:08
** 版  本:	1.0
** 描  述:	效棵配置
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "stdafx.h"
#include "SchemeEffect.h"
#include "IClientGlobal.h"

/** 
@param   
@param   
@return  
*/
CSchemeEffect::CSchemeEffect(void)
{
	// 所有效果配置属性	
	m_mapEffectSchemeInfo.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemeEffect::~CSchemeEffect(void)
{
	// 所有效果配置属性	
	m_mapEffectSchemeInfo.clear();
}

/** 
@param   
@param   
@return  
*/
bool CSchemeEffect::LoadScheme(void)
{
	ISchemeEngine *pSchemeEngine = gClientGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
	string stringPath = EFFECT_SCHEME_FILENAME;	
	bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
	if(!bResult)
	{
		Error("加载配置文件失败。文件名 = " << stringPath.c_str() << endl);

		return false;
	}

	return true;
}

/** 关闭
@param   
@param   
@return  
*/
void CSchemeEffect::Close(void)
{	
	// 所有效果配置属性	
	m_mapEffectSchemeInfo.clear();
}

/** 取得地图配置信息
@param   
@param   
@return  
*/
SEffectSchemeInfo * CSchemeEffect::GetEffectSchemeInfo(DWORD dwEffectID)
{
	TMAP_EFFECTSCHEMEINFO::iterator itM = m_mapEffectSchemeInfo.find(dwEffectID);
	if(itM != m_mapEffectSchemeInfo.end())
	{
		return &((*itM).second);
	}

	return NULL;
}

const char * CSchemeEffect::GetEffectGroupName(int nGroupID)
{
	GroupNameMap::iterator it = m_mapCheckGroupMap.find(nGroupID);
	if(it == m_mapCheckGroupMap.end())
	{
		return "";
	}

	return it->second.c_str();
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
bool CSchemeEffect::OnSchemeLoad(ICSVReader * pCSVReader,LPCSTR szFileName)
{
	return false;	
}

/** XML配置载入时通知
@param   pTiXmlDocument：读取XML的返回接口
@param   szFileName：配置文件名
@param   
@return  
@note     
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/
bool CSchemeEffect::OnSchemeLoad(TiXmlDocument * pTiXmlDocument,LPCSTR szFileName)
{
	if(pTiXmlDocument == NULL)
	{
		return false;
	}

	// 所有效果配置属性	
	m_mapEffectSchemeInfo.clear();

	// 根节点
	TiXmlElement * pRootElement = pTiXmlDocument->FirstChildElement("scheme");
	if(pRootElement == NULL)
	{
		return false;
	}

	// [effectgroup]
	TiXmlElement * pSubElement = pRootElement->FirstChildElement("effectgroup");
	if(pSubElement == NULL)
	{
		return false;
	}

	// 此配置的xml格式文件太大，将属性名改短一些，这样，文件小一点
	LPCSTR pszAttrProp = NULL; char szAttributeName[128] = { 0 };
	TiXmlElement * pChildElement = NULL;

	while(pSubElement)
	{
		// GROUP ID
		int nGroupID = 0;
		pszAttrProp = pSubElement->Attribute("gid", &nGroupID);
		if(pszAttrProp == NULL || nGroupID <= 0)
		{
			return false;
		}

		int nFlag = 0;
		pszAttrProp = pSubElement->Attribute("flag", &nFlag);

		// 描述
		pszAttrProp = pSubElement->Attribute("gname");
		if(pszAttrProp == NULL)
		{
			return false;
		}
		
		// 检查同一组的每一处定义组名是否相同，如果不同表示策划配错了
		GroupNameMap::iterator iter = m_mapCheckGroupMap.find(nGroupID);
		if (iter == m_mapCheckGroupMap.end())
		{
			// 未找到则增加
			m_mapCheckGroupMap.insert(std::make_pair(nGroupID, std::string(pszAttrProp)));
		}
		else
		{
			// 找到了则判断和原来的是否相同
			std::string &str = iter->second;
			if (str != pszAttrProp)
			{
				ErrorLn("组的两个定义名字不一样，groupID = " << nGroupID);
				return false;
			}
		}

		// [sub]
		pChildElement = pSubElement->FirstChildElement("effect");
		if(pChildElement == NULL)
		{
			return false;
		}

		while(pChildElement)
		{
			SEffectSchemeInfo effectschemeinfo;
			effectschemeinfo.nGroupID = nGroupID;
			effectschemeinfo.nFlag = nFlag;

			// 效果id
			pszAttrProp = pChildElement->Attribute("eid", &effectschemeinfo.nEffectID);
			if(pszAttrProp == NULL || effectschemeinfo.nEffectID <= 0)
			{
				ErrorLn("effectid不能小于等于0! effectid="<<effectschemeinfo.nEffectID);
				return false;
			}

			// 效果id不能大于0xffff，否则存数据库会出问题
			if(effectschemeinfo.nEffectID >= 0xFFFF)
			{
				ErrorLn("effectid不能大于等于0xFFFF! effectid="<<effectschemeinfo.nEffectID);
				return false;
			}

			// 是否有重复的
			if(GetEffectSchemeInfo(effectschemeinfo.nEffectID) != NULL)
			{
				ErrorLn("重复的effectid.effectid="<<effectschemeinfo.nEffectID);
				return false;
			}

			// 描述
			pszAttrProp = pChildElement->Attribute("desc");
			if(pszAttrProp == NULL)
			{
				return false;
			}
			sstrcpyn(effectschemeinfo.szDesc, pszAttrProp, sizeof(effectschemeinfo.szDesc));

			// 参数			
			for(int i = 0; i < 128; i++)
			{
				sprintf(szAttributeName, "p%d", i); int nTempValue = 0;
				pszAttrProp = pChildElement->Attribute(szAttributeName, &nTempValue);
				if(pszAttrProp == NULL)
				{
					break;
				}

				effectschemeinfo.listParam.push_back(nTempValue);
			}

			// map
			m_mapEffectSchemeInfo[effectschemeinfo.nEffectID] = effectschemeinfo;

			// 读取下一个
			pChildElement = pChildElement->NextSiblingElement("effect");
		}	
	
		// 读取下一个
		pSubElement = pSubElement->NextSiblingElement("effectgroup");
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
bool CSchemeEffect::OnSchemeUpdate(ICSVReader * pCSVReader, LPCSTR szFileName)
{
	return false;
}

/** 配置动态更新时通知
@param   pTiXmlDocument：读取XML的返回接口
@param   szFileName：配置文件名
@param   
@return  
@note     
@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
@retval buffer 
*/
bool CSchemeEffect::OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, LPCSTR szFileName)
{
	return OnSchemeLoad(pTiXmlDocument, szFileName);
}