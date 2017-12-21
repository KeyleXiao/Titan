/*******************************************************************
** 文件名:	BuffClient.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/16  10:58
** 版  本:	1.0
** 描  述:	BUFF 客户端
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "BuffClient.h"
#include "BuffPart.h"

CBuffClient * g_pBuffClient = NULL;
/** 
@param   
@param   
@return  
*/
CBuffClient::CBuffClient(void)
{

}

/** 
@param   
@param   
@return  
*/
CBuffClient::~CBuffClient(void)
{

}

/** 
@param   
@param   
@return  
*/
bool CBuffClient::create(void)
{
	if(!m_SchemeBuff.LoadScheme())
	{
		return false;
	}

	return true;
}


/** 释放
@param   
@param   
@param   
@return  
@note     释放所有资源，并且销毁此对像
@warning 
@retval buffer 
*/
void CBuffClient::release(void)
{
	// buff配置
	m_SchemeBuff.Close();

	// 全局指针NULL
	g_pBuffClient = NULL;

	delete this;
}


/** 给实体创建buff部件，重新new的部件才调此方法。如果是回收的，则不需要
*/
IBuffPart *CBuffClient::CreatePart(void)
{
	IBuffPart *pBuffPart = new CBuffPart();
	return pBuffPart;
}


/** 取得BUFF配置信息
@param   
@param   
@return  
*/
SBuffSchemeInfo * CBuffClient::GetBuffSchemeInfo(DWORD dwBuffID)
{
	return m_SchemeBuff.GetBuffSchemeInfo(dwBuffID);
}

/** 取得某个buff的描述
@param   
@param   
@return  
*/
LPCSTR CBuffClient::GetBuffDesc(int nBuffID, int nBuffLevel)
{
	SBuffSchemeInfo * pSchemeInfo = GetBuffSchemeInfo(nBuffID);
	if(pSchemeInfo == NULL)
	{
		return NULL;
	}

	list<SBuffSchemeInfo::SLevelInfo>::iterator it;
	it = pSchemeInfo->listLevelInfo.begin();
	for( ; it != pSchemeInfo->listLevelInfo.end(); ++it)
	{
		if((*it).dwBuffLevel == nBuffLevel)
		{
			return ((*it).szDesc);
		}
	}

	return NULL;
}

/** 取得buff名字
@param   
@param   
@return  
*/
LPCSTR CBuffClient::GetBuffName(DWORD dwBuffID)
{
	SBuffSchemeInfo * pSchemeInfo = GetBuffSchemeInfo(dwBuffID);
	if(pSchemeInfo == NULL)
	{
		return NULL;
	}

	return pSchemeInfo->szName;
}

/** 取得buff图标ID
@param dwBuffID: BUFF的ID  
@return  
*/
DWORD CBuffClient::GetBuffIconID(DWORD dwBuffID)
{
	SBuffSchemeInfo * pSchemeInfo = GetBuffSchemeInfo(dwBuffID);
	if(pSchemeInfo == NULL)
	{
		return 0;
	}

	return (DWORD)pSchemeInfo->nIconID;
}


/** 输出函数
@param   
@param   
@return  
*/
API_EXPORT IBuffClient *CreateBuffClient(void)
{
	// 如果创建了，则直接返回
	if(g_pBuffClient != NULL)
	{
		return g_pBuffClient;
	}

	// 实例化BUFF
	CBuffClient *pBuffClient = new CBuffClient();
	if(pBuffClient == NULL || !pBuffClient->create())
	{
		safeRelease(pBuffClient);
		return NULL;
	}

	g_pBuffClient = pBuffClient;

	return g_pBuffClient;
}
