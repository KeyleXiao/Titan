/*******************************************************************
** 文件名:	e:\Rocket\Server\WarServer\Src\SchemeWar.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/27  16:57
** 版  本:	1.0
** 描  述:	冷却配置脚本
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "stdafx.h"
#include "SchemeWar.h"
#include "ExternalFacade.h"
#include "ExternalFacade.h"

/** 
@param   
@param   
@return  
*/
CSchemeWar::CSchemeWar(void)
{
	// 所有冷却配置信息	
	m_vectorWarClass.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemeWar::~CSchemeWar(void)
{
	// 所有冷却配置信息	
	m_vectorWarClass.clear();
}

/** 
@param   
@param   
@return  
*/
bool CSchemeWar::LoadScheme(void)
{
	ISchemeEngine * pSchemeEngine = g_ExternalFacade.GetSchemeEngine();
	if(pSchemeEngine == NULL)
	{
		return false;
	}

	// 加载脚本
	string stringPath = FREEZE_SCHEME_FILENAME;
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
void CSchemeWar::Close(void)
{	
	// 所有冷却配置信息	
	m_vectorWarClass.clear();
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
bool CSchemeWar::OnSchemeLoad(SCRIPT_READER reader,LPCSTR szFileName)
{
	TiXmlDocument * pTiXmlDocument = reader.pXMLReader;
	if ( pTiXmlDocument==0 || reader.type!=READER_XML)
		return false;

	// 所有冷却配置信息
	m_vectorWarClass.clear();

	TiXmlElement * pRootElement = pTiXmlDocument->FirstChildElement("scheme");
	if(pRootElement == NULL)
	{
		return false;
	}

	// [freezegroup]
	TiXmlElement * pSubElement = pRootElement->FirstChildElement("freezeclass");
	if(pSubElement == NULL)
	{
		return false;
	}

	while(pSubElement)
	{
		SWarClass freezeclass;

		// classid
		LPCSTR pszAttrProp = pSubElement->Attribute("classid", &freezeclass.nClassID);
		if(pszAttrProp == NULL || freezeclass.nClassID <= 0)		
		{
			return false;
		}

		// [group]
		TiXmlElement * pGroupElement = pSubElement->FirstChildElement("group");
		while(pGroupElement)
		{
			// groupid
			int nGroupID = 0;
			pszAttrProp = pGroupElement->Attribute("groupid", &nGroupID);
			if(pszAttrProp == NULL || nGroupID <= 0)
			{
				return false;
			}

			// 重复检测			
			if(GetWarGroup(freezeclass.nClassID, nGroupID) != NULL)
			{
				return false;
			}

			int nWarID = 0;
			list<int> listWarID;
			char szAttributeName[64] = { 0 };			
			for(int i = 0; i < 128; i++)
			{
				sprintf(szAttributeName, "id%d", i);
				pszAttrProp = pGroupElement->Attribute(szAttributeName, &nWarID);
				if(pszAttrProp == NULL || nWarID <= 0)
				{
					break;
				}

				// 压入列表中
				listWarID.push_back(nWarID);
			}

			// 压入
			freezeclass.mapWarGroup[nGroupID] = listWarID;

			// 读取下一个			
			pGroupElement = pGroupElement->NextSiblingElement("group");
		}


		// [freeze]
		TiXmlElement * pWarElement = pSubElement->FirstChildElement("freeze");
		while(pWarElement)
		{
			SWarScheme freezescheme;

			// freeze id
			pszAttrProp = pWarElement->Attribute("id", &freezescheme.nWarID);
			if(pszAttrProp == NULL || freezescheme.nWarID <= 0)
			{
				return false;
			}

			// 重复检查
			if(GetWarScheme(freezeclass.nClassID, freezescheme.nWarID) != NULL)
			{
				return false;
			}

			// time
			pszAttrProp = pWarElement->Attribute("time", &freezescheme.nTime);
			if(pszAttrProp == NULL || freezescheme.nTime < 0)
			{
				return false;
			}

			// savedb
			pszAttrProp = pWarElement->Attribute("savedb", &freezescheme.nSaveDB);
			if(pszAttrProp == NULL)
			{
				return false;
			}

			// 冷却时长	客户端显示用
			pszAttrProp = pWarElement->Attribute("time1", &freezescheme.nShowTime);
			if(pszAttrProp == NULL || freezescheme.nShowTime < 0)
			{
				return false;
			}

			// [together]
			TiXmlElement * pTogetherElement = pWarElement->FirstChildElement("together");
			while(pTogetherElement)
			{
				SWarScheme::SWarGroup freezegroup;

				// groupid
				pszAttrProp = pTogetherElement->Attribute("groupid", &freezegroup.nGroupID);
				if(pszAttrProp == NULL || freezegroup.nGroupID <= 0)
				{
					return false;
				}

				// time
				pszAttrProp = pTogetherElement->Attribute("time", &freezegroup.nTime);
				if(pszAttrProp == NULL || freezegroup.nTime < 0)
				{
					return false;
				}

				// 压入
				freezescheme.listGroup.push_back(freezegroup);

				// 读取下一个
				pTogetherElement = pTogetherElement->NextSiblingElement("together");
			}

			// 压入
			freezeclass.mapWarScheme[freezescheme.nWarID] = freezescheme;

			// 读取下一个
			pWarElement = pWarElement->NextSiblingElement("freeze");
		}

		// 压入
		m_vectorWarClass.push_back(freezeclass);

		// 读取下一个
		pSubElement = pSubElement->NextSiblingElement("freezeclass");
	}

	return true;
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
bool CSchemeWar::OnSchemeUpdate(SCRIPT_READER reader,LPCSTR szFileName)
{
	return OnSchemeLoad(reader, szFileName);
}

/** 
@param   
@param   
@return  
*/
SWarScheme * CSchemeWar::GetWarScheme(DWORD dwClassID, DWORD dwWarID)
{
	TVECTOR_FREEZECLASS::iterator it = m_vectorWarClass.begin();
	for( ; it != m_vectorWarClass.end(); ++it)
	{
		SWarClass * pWarClass = &(*it);
		if(pWarClass->nClassID == dwClassID)
		{
			TMAP_FREEZESCHEME::iterator itM = pWarClass->mapWarScheme.find(dwWarID);
			if(itM != pWarClass->mapWarScheme.end())
			{
				return &((*itM).second);
			}
		}
	}

	return NULL;
}

/** 
@param   
@param   
@return  
*/
list<int> *	CSchemeWar::GetWarGroup(DWORD dwClassID, DWORD dwGroupID)
{
	TVECTOR_FREEZECLASS::iterator it = m_vectorWarClass.begin();
	for( ; it != m_vectorWarClass.end(); ++it)
	{
		SWarClass * pWarClass = &(*it);
		if(pWarClass->nClassID == dwClassID)
		{
			TMAP_FREEZEGROUP::iterator itG = pWarClass->mapWarGroup.find(dwGroupID);
			if(itG != pWarClass->mapWarGroup.end())
			{
				return &((*itG).second);
			}
		}
	}

	return NULL;
}
