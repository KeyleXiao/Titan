/*******************************************************************
** 文件名: IBelongEntityMgr.h 
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 7/3/2017
** 版  本: 1.0
** 描  述: 附属实体管理接口（分身、宠物。。。）

********************************************************************/
#ifndef __I_BELONG_ENTITY_MGR_H__
#define __I_BELONG_ENTITY_MGR_H__

struct EntityBelongContext;

struct IBelongEntityMgr
{
    // 重置（清空并移除实体）
    virtual void restore(void) = 0;

    // 插入一个附属实体信息
    virtual void push_back(EntityBelongContext context) = 0;

    // 删除实体并移除附属信息
    virtual void remove(unsigned long uid) = 0; 

    // 获取附属实体信息
    virtual EntityBelongContext *  get(UID uid) = 0;
};

#endif // __I_BELONG_ENTITY_MGR_H__