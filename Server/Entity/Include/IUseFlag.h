/*******************************************************************
** 文件名:	IUseFlag.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/25/2017
** 版  本:	1.0
** 描  述:	实体用途管理接口

********************************************************************/

#pragma once

// 用途定义
enum ENTITY_USE_FLAG
{
    USE_FLAG_DAMAGE_NOTIFY_OPERATOR,        // 目标伤害通知操作者
    USE_FLAG_DIE_NOTIFY_OPERATOR,           // 目标死亡通知操作者
    USE_FLAG_MAX,
};

// 用途标识位，位要对应上面定义
enum ENTITY_USE_FLAG_BIT
{
    USE_BIT_INVALID                = 0 << 0,    // 无效
    USE_BIT_DAMAGE_NOTIFY_OPERATOR = 1 << 0,   // 目标伤害通知操作者
    USE_BIT_DIE_NOTIFY_OPERATOR    = 1 << 1,   // 目标死亡通知操作者
};



// 实体用途管理接口
struct IUseFlag
{
    // 增加标识
    virtual bool add(int nFlag, const char* szInfo="") = 0;

    // 删除标识
    virtual bool remove(int nFlag, const char* szInfo = "") = 0;

    // 校验标识是否有效
    virtual bool check(int nFlag) = 0;

    // 获取所有flag
    virtual int  getAll(void) = 0;
};
