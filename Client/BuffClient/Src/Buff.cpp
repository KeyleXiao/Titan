/*******************************************************************
** 文件名:	Buff.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/15  22:46
** 版  本:	1.0
** 描  述:	BUFF
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include "Buff.h"
#include "BuffClient.h"
#include "ExternalFacade.h"
#include "IClientGlobal.h"
#include "IClientEntity.h"
/** 
@param   
@param   
@return  
*/
CBuff::CBuff(void)
{
	// buff index
	m_dwBuffIndex = 0;

	// buff id
	m_dwBuffID = INVALID_BUFF_ID;

	// 等级
	m_dwBuffLevel = 0;

    // 标志
    m_dwSchemeFlag = 0;

	// 总时间
	m_dwTime = 0;

	// 启动tick
	m_dwStartTick = 0;

	// 图标id
	m_nIconID = 0;

	// buff名字 
	ZeroMemory(m_szName,sizeof(m_szName));	

	// 描述
	ZeroMemory(m_szDesc, sizeof(m_szDesc));	

	// BUFF部件
	m_pBuffPart = NULL;

	// 所有效果
	m_listEffect.clear();

	// 光效ID
	m_dwFlashID = 0;

	// 效果数据
	memset(&m_effectContext, 0, sizeof(m_effectContext));
}

/** 
@param   
@param   
@return  
*/
CBuff::~CBuff(void)
{
}


/** 
@param   
@param   
@return  
*/
bool CBuff::Create(CBuffPart * pBuffPart, DWORD dwIndex, DWORD dwBuffID, DWORD dwBuffLevel, DWORD dwTime, EFFECT_CONTEXT *pContext)
{
	if(pBuffPart == NULL)
	{
		return false;
	}

	IEffectFactory *pEffectFactory = g_ExternalFacade.GetEffectFactory();
	if(pEffectFactory == NULL)
	{
		return false;
	}

	SBuffSchemeInfo * pSchemeInfo = g_pBuffClient->GetBuffSchemeInfo(dwBuffID);
	if(pSchemeInfo == NULL)
	{
		return false;
	}

	SBuffSchemeInfo::SLevelInfo * pLevelInfo = NULL;
	list<SBuffSchemeInfo::SLevelInfo>::iterator it = pSchemeInfo->listLevelInfo.begin();
	for( ; it != pSchemeInfo->listLevelInfo.end(); ++it)
	{
		if((*it).dwBuffLevel == dwBuffLevel)
		{
			pLevelInfo = &(*it);
			break;
		}
	}

	if(pLevelInfo == NULL)
	{
		return false;
	}

	// buff index
	m_dwBuffIndex = dwIndex;

	// buff id
	m_dwBuffID = dwBuffID;

	// 等级
	m_dwBuffLevel = dwBuffLevel;

    // 标志
    m_dwSchemeFlag = pSchemeInfo->dwFlag;

	// 总时间
	m_dwTime = dwTime;

	// 启动tick
	m_dwStartTick = 0;

	// BUFF部件
	m_pBuffPart = pBuffPart;

	// 图标id
	m_nIconID = pSchemeInfo->nIconID;

	// buff名字 
	sstrcpyn(m_szName, pSchemeInfo->szName, sizeof(m_szName));	

	// 描述
	sstrcpyn(m_szDesc, pLevelInfo->szDesc, sizeof(m_szDesc));	

	// 光效ID	
	if(pLevelInfo->dwFlashIDEx != 0)
	{
		m_dwFlashID = pLevelInfo->dwFlashIDEx;
	}
	if(m_dwFlashID == 0)
	{
		m_dwFlashID = pLevelInfo->dwFlashID;
	}

	// 创建效果
	list<DWORD>::iterator itL = pLevelInfo->listEffect.begin();
	for( ; itL != pLevelInfo->listEffect.end(); ++itL)
	{
		// 没创建成功不要紧的，有些效果客户端本来就不需要创建
		IEffect * pEffect = pEffectFactory->BuildEffect((*itL));
		if(pEffect != NULL)
		{
			m_listEffect.push_back(pEffect);
		}
	}

	// 效果数据
	if (pContext == NULL)
	{
		memset(&m_effectContext, 0, sizeof(m_effectContext));
	}
	else
	{
		memcpy(&m_effectContext, pContext, sizeof(m_effectContext));
	}

	return true;
}


/** 释放所有资源，并且销毁此对像
@param   
@param   
@return    	
*/
void CBuff::Release(void)
{
	// kill定时器
	g_ExternalFacade.KillTimer(ETimerEventID_Life, static_cast<ITimerHandler *>(this));

	// buff index
	m_dwBuffIndex = 0;

	// buff id
	m_dwBuffID = INVALID_BUFF_ID;

	// 等级
	m_dwBuffLevel = 0;

	// 总时间
	m_dwTime = 0;

	// 启动tick
	m_dwStartTick = 0;

	// 图标id
	m_nIconID = 0;

	// buff名字 
	ZeroMemory(m_szName, sizeof(m_szName));	

	// 描述
	ZeroMemory(m_szDesc, sizeof(m_szDesc));	

	// BUFF部件
	m_pBuffPart = NULL;

	// 光效ID
	m_dwFlashID = 0;

	// 所有效果
	TLIST_EFFECT::iterator it = m_listEffect.begin();
	for( ; it != m_listEffect.end(); ++it)
	{
		SAFE_RELEASE((*it));
	}
	m_listEffect.clear();

	// 效果数据清空
	memset(&m_effectContext, 0, sizeof(m_effectContext));

	delete this;
}


/** 启动效果
@param   
@param   
@return  
*/
bool CBuff::Start(void)
{
	// 启动效果
	TLIST_EFFECT::iterator it = m_listEffect.begin();
	for( ; it != m_listEffect.end(); ++it)
	{
		(*it)->Start(&m_effectContext);
	}

	// 启动定时器
	if(m_dwTime > 0)
	{	
		// 启动tick
		m_dwStartTick = ::GetTickCount();

		g_ExternalFacade.SetTimer(ETimerEventID_Life,
								  m_dwTime,
								  static_cast<ITimerHandler *>(this),
								  "CBuff::Start");
	}

	return true;
}


/** 
@param   
@param   
@return  
*/
bool CBuff::Stop(void)
{
	// 结束效果
	TLIST_EFFECT::iterator it = m_listEffect.begin();
	for( ; it != m_listEffect.end(); ++it)
	{
		(*it)->Stop();
	}	

	// kill定时器
	g_ExternalFacade.KillTimer(ETimerEventID_Life, static_cast<ITimerHandler *>(this));

	return true;
}


/** buff id
@param   
@param   
@return  
*/
DWORD CBuff::GetBuffID(void)
{
	return m_dwBuffID;
}

/** 等级
@param   
@param   
@return  
*/
DWORD CBuff::GetLevel(void)
{
	return m_dwBuffLevel;
}

/** 标志
@param   
@param   
@return  
*/
DWORD CBuff::GetFlag(void)
{
    return m_dwSchemeFlag;
}

/** 剩余时间，外面要自已处理时间倒数,如果返回0,表示永久状态
@param   
@param   
@return  
*/
DWORD CBuff::GetLeftTime(void)
{
	if(m_dwTime > 0)
	{		
		 int nLeftTime = m_dwTime - (GetTickCount() - m_dwStartTick);
		 nLeftTime <= 0 ? nLeftTime = 1 : 0;

		 return nLeftTime;
	}

	return m_dwTime;
}

/** 图标ID
@param   
@param   
@return  
*/
int CBuff::GetIconID(void)
{
	return m_nIconID;
}

/** buff名字
@param   
@param   
@return  
*/
LPCSTR CBuff::GetBuffName(void)
{
	return m_szName;
}

/** buff描述
@param   
@param   
@return  
*/
LPCSTR CBuff::GetDesc(void)
{
	return m_szDesc;
}

/** 
@param   
@param   
@return  
*/
DWORD CBuff::GetIndex(void)
{
	return m_dwBuffIndex;
}

/** 
@param   
@param   
@return  
*/
DWORD CBuff::GetFlashID(void)
{
	return m_dwFlashID;
}

/** 
@param   
@param   
@return  
*/
EFFECT_CONTEXT *CBuff::GetEffectContext(void)
{
	return &m_effectContext;
}


/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void CBuff::OnTimer(unsigned long dwTimerID)
{
	if (m_pBuffPart == NULL)
	{
		return;
	}

	// 移除状态
	m_pBuffPart->RemoveBuff(this);
}


/** 取得总时间
@param   
@param   
@return  
*/
int	 CBuff::GetTotalTime(void)
{
	return m_dwTime;
}

int CBuff::GetEffectCount( void )
{
	return m_listEffect.size();
}

int CBuff::GetEffectIDByIndex(int nIndex)
{
	if (nIndex < 0 || nIndex >= (int)m_listEffect.size())
	{
		return 0;
	}

	TLIST_EFFECT::iterator iter = m_listEffect.begin();
	while(nIndex && iter != m_listEffect.end())
	{
		iter++;
		nIndex--;
	}
	
	if (iter == m_listEffect.end())
	{
		return 0;
	}

	return (*iter)->GetEffectID();
}

/** 判断某个效果是否存在
@param   
@param   
@return  
*/
bool CBuff::IsEffectIDExist(int nEffectID)
{
	TLIST_EFFECT::iterator iter = m_listEffect.begin();
	for (; iter != m_listEffect.end(); ++iter)
	{
		if ((*iter)->GetEffectID() == nEffectID)
		{
			return true;
		}
	}

	return false;
}