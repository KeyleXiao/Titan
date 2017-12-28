/*******************************************************************
** 文件名:	EntityUseFlagMgr.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/25/2017
** 版  本:	1.0
** 描  述:	实体用途管理类

********************************************************************/

#include "stdafx.h"
#include "EntityUseFlagMgr.h"



/////////////////////////////////////////////////////////////////

CEntityUseFlagMgr::CEntityUseFlagMgr(void)
{
    ZeroMemory(m_nUseFlags, sizeof(m_nUseFlags));
}

bool CEntityUseFlagMgr::add(int nFlag, const char* szInfo)
{
    if (nFlag >= USE_FLAG_MAX)
    {
        ErrorLn(__FUNCTION__": flag is invalid, flag="<< nFlag << szInfo);
        return false;
    }

    ++m_nUseFlags[nFlag];

    return true;
}

bool CEntityUseFlagMgr::remove(int nFlag, const char* szInfo)
{
    if (nFlag >= USE_FLAG_MAX)
    {
        ErrorLn(__FUNCTION__": flag is invalid, flag=" << nFlag << szInfo);
        return false;
    }

    if (m_nUseFlags[nFlag] > 0)
    {
        --m_nUseFlags[nFlag];
    }

    return true;
}

bool CEntityUseFlagMgr::check(int nFlag)
{
    if (nFlag >= USE_FLAG_MAX)
    {
        return false;
    }

    return (m_nUseFlags[nFlag] > 0);
}

int CEntityUseFlagMgr::getAll(void)
{
    int nFlags = 0;
    for (int i = 0; i < USE_FLAG_MAX; i++)
    {
        if (m_nUseFlags[i] > 0)
        {
            nFlags |= (1 << i);
        }
    }
    return nFlags;
}
