/*******************************************************************
** 文件名: BelongEntityMgr.h 
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 7/3/2017
** 版  本: 1.0
** 描  述: 附属实体管理类（分身、宠物。。。）

********************************************************************/

#ifndef __BELONG_ENTITY_MGR_H__
#define __BELONG_ENTITY_MGR_H__
#pragma once

#include <map>
#include "IBelongEntityMgr.h"


// 附属实体管理类（分身、宠物。。。）
class CBelongEntityMgr : public IBelongEntityMgr
{
    typedef std::map<UID, EntityBelongContext> ENTITY_TABLE;

public:
    CBelongEntityMgr();

    ~CBelongEntityMgr();

    bool onload(__IEntity  * pMaster);

    void release(void);

    // 重置（清空并移除实体）
    virtual void restore(void);

    // 插入一个附属实体信息
    virtual void push_back(EntityBelongContext context);

    // 删除实体并移除附属信息
    virtual void remove(UID uid);

    // 获取附属实体信息
    virtual EntityBelongContext *  get(UID uid);


private:
    __IEntity *             m_pMaster;

    ENTITY_TABLE            m_mapEntity;
};


#endif // __BELONG_ENTITY_MGR_H__