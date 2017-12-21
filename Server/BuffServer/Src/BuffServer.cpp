/*******************************************************************
** 文件名:	BuffServer.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/12  21:44
** 版  本:	1.0
** 描  述:	buff服务器
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "BuffServer.h"
#include "ExternalFacade.h"
#include "SchemeBuff.h"
#include "BuffPart.h"

CBuffServer * g_pBuffServer = NULL;
/** 
@param   
@param   
@return  
*/
CBuffServer::CBuffServer(void)
{
	// BUFF优化标志
	m_bOptimizeBuff = true;
}

/** 
@param   
@param   
@return  
*/
CBuffServer::~CBuffServer(void)
{

}

/** 创建
*/
bool CBuffServer::create()
{
	CSchemeBuff* pSchemeBuff = CSchemeBuff::NewInstance();
	if(pSchemeBuff == NULL || !pSchemeBuff->LoadScheme())
	{
		safeDelete(pSchemeBuff);
		return false;
	}

	return true;
}


/** 执行Service的on_stop
@return  
*/
void CBuffServer::onStop(void)
{
    // 配置器
    CSchemeBuff::CloseAll();

    // BUFF回收器
    m_BuffRecycle.Close();

    // 全局指针NULL
    g_pBuffServer = NULL;

    // BUFF优化标志
    m_bOptimizeBuff = false;
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
void CBuffServer::release(void)
{
	delete this;
}

/** 给实体创建buff部件，重新new的部件才调此方法。如果是回收的，则不需要
@param   
@param   
@param   
@return   成功: 非NULL,失败: NULL
@note     
@warning 
@retval buffer 
*/
__IBuffPart *CBuffServer::CreatePart(void)
{
	return new CBuffPart();
}

/** 取得buff名字
@param   
@param   
@return  
*/
const char* CBuffServer::GetBuffName(DWORD dwBuffID)
{
	SBuffSchemeInfo * pInfo = GetBuffSchemeInfo(dwBuffID);
	if(pInfo == NULL)
	{
		return NULL;
	}

	return pInfo->szName;
}


///////////////////////CBuffServer/////////////////////////////////
/** 取得地图配置信息
@param   
@param   
@return  
*/
SBuffSchemeInfo * CBuffServer::GetBuffSchemeInfo(DWORD dwBuffID)
{
	CSchemeBuff* pInstance = CSchemeBuff::GetWorkInstance();
	if(pInstance == NULL)
	{
		return NULL;
	}

	return pInstance->GetBuffSchemeInfo(dwBuffID);
}

/** 此方法检测是否能够添加
@param   pEntity ：实体指针
@param   dwRule ：规则
@param   dwBuffID ：buffID
@param   dwBuffLevel ：buff等级
@param   uidAdd ：添加者
@return  
*/
bool CBuffServer::CanAdd(__IEntity *pEntity, DWORD dwRule, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd)
{
	return m_ReplaceRule.CanAdd(pEntity, dwRule, dwBuffID, dwBuffLevel, uidAdd);
}

/** 压入场景对像
@param   
@param   
@return  
*/
void CBuffServer::PushToRecycle(CBuff * pBuff)
{
	m_BuffRecycle.Push(pBuff);
}

/** 弹出场景对像
@param   
@param   
@return  
*/
CBuff *	CBuffServer::PopFromRecycle(void)
{
	return m_BuffRecycle.Pop();
}

/** 取得buff回收池的信息
@param   
@param   
@return  
*/
void CBuffServer::GetBuffRecycle(int &nRecycleSize, int &nReleaseSize, int &nRestoreSize)
{
	return m_BuffRecycle.GetBuffRecycle(nRecycleSize, nReleaseSize, nRestoreSize);
}

/** 取得所有配置信息
@param   
@param   
@return  
*/
TMAP_BUFFSCHEMEINFO * CBuffServer::GetBuffSchemeInfo(void)
{
	CSchemeBuff* pInstance = CSchemeBuff::GetWorkInstance();
	if(pInstance == NULL)
	{
		return NULL;
	}


	return pInstance->GetBuffSchemeInfo();
}

/** 设置BUFF模块优化标志
@param   
@param   
@return  
*/
void CBuffServer::SetOptimizeBuffFlag(bool bOpen)
{
	m_bOptimizeBuff = bOpen;
}

int CBuffServer::getAccQueryIndex(DWORD dwBuffID, DWORD dwBuffLevel)
{
	SBuffSchemeInfo* pSchema = GetBuffSchemeInfo(dwBuffID);
	if (pSchema == NULL)
		return -1;

	for (list<SBuffSchemeInfo::SLevelInfo>::iterator it = pSchema->listLevelInfo.begin(); it != pSchema->listLevelInfo.end(); ++it)
	{
		if (it->dwBuffLevel == dwBuffLevel)
			return it->nAccQueryIndex;
	}

	return -1;
}

/** 获取BUFF模块优化标志
@param   
@param   
@return  
*/
bool CBuffServer::IsOptimizeBuffOpen()
{
	return m_bOptimizeBuff;
}

/** 输出函数
@param   
@param   
@return  
*/
API_EXPORT IBuffServer *CreateBuffServer(void)
{
	// 如果创建了，则直接返回
	if(g_pBuffServer != NULL)
	{
		return g_pBuffServer;
	}

	// 实例化BUFF服务器
	CBuffServer * pBuffServer = new CBuffServer();
	if(pBuffServer == NULL)
	{
		return NULL;
	}

	g_pBuffServer = pBuffServer;

	return g_pBuffServer;
}