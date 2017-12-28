/*******************************************************************
** 文件名:	EntityUseFlagMgr.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/25/2017
** 版  本:	1.0
** 描  述:	实体用途管理类

********************************************************************/

#pragma once

#include "IUseFlag.h"

struct __IEntity;

// 实体用途管理类
class CEntityUseFlagMgr : public IUseFlag
{
public:
	/** 构造函数
	*/
    CEntityUseFlagMgr(void);

    // 增加标识
    virtual bool add(int nFlag, const char* szInfo) override;

    // 删除标识
    virtual bool remove(int nFlag, const char* szInfo) override;

    // 校验标识是否有效
    virtual bool check(int nFlag) override;

    // 获取所有flag
    virtual int  getAll(void) override;

private:
    // 用途
    unsigned short   m_nUseFlags[USE_FLAG_MAX];
};