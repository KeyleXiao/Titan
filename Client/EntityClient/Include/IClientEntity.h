/*******************************************************************
** 文件名:	IClientEntity.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	7/8/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/


#ifndef __I_CLIENTENTITY_H__
#define __I_CLIENTENTITY_H__


#include "platform.h"
#include "EntityViewDef.h"
#include "NetMessageDef.h"
#include "IRenderView.h"
#include "EntityDef.h"
#include "AOI_Define.h"


// 实体客户端逻辑对象
struct IClientEntity : public IEntity
{
	// 创建显示层实体
	virtual bool		createView(int flag) = 0;

	// 取得实体显示层ID
	virtual ENTITY_ID	getViewID() = 0;

	// 改变当前状态 
	virtual bool		changeState(int nState) = 0;

	// 取得当前状态 
	virtual int			getState(void) = 0; 

	// 取得数值属性
	virtual int			getIntProperty(int nPropID) = 0;

	// 取得字符串属性
	virtual char*		getStringProperty(int nPropID) = 0;

	// 取得来源游戏世界ID
	virtual	int			getFromGameWorldID() = 0;
};

struct IEntityFactory
{
	// 创建实体
	// @param type   : 实体类型
	// @param context: 创建现场
	// @param len    : 长度
	// @param flag   : 现场序列化标识，比如从数据库现场创建，从服务器发给客户端的现场创建
	virtual IClientEntity * createEntity( UID uid, SEntiyExportShadowContext &shadowData,void * context,int len) = 0;

	// 根据UID查找实体
	virtual IClientEntity * getEntity( UID uid) = 0;

    // 根据PDBID查找实体
    virtual IClientEntity * getEntityByPDBID( PDBID playerID) = 0;

	// 根据显示ID查找实体
	virtual IClientEntity * getEntityByView( ENTITY_ID viewID) = 0;

	// 将这个实体对应的UID归还
	virtual void destroyEntity( UID uid, EntityDestroyReason eReason = DESTROY_REASON_NORMAL, void* pParam = NULL) = 0;

    /** 获取当前场景中的所有玩家
    @name				: 
    @param				: 
    @return
    */
    virtual size_t getAllPlayer( IClientEntity* entityArray[], size_t nArraySize ) = 0;

    // 取得实体数量
    virtual DWORD getEntityCount(BYTE entity_type) = 0;

    // 删除所有实体
    virtual void destroyAllEntity() = 0;

    // 实体执行更新命令后操作
    virtual void onUpdateView(IClientEntity * pEntity, int cmdID, int nParam, const char* strParam, void* ptrParam, int len) = 0;

    /** 获取指定阵营玩家
    @name				: 
    @param				: 
    @return
    */
    virtual size_t getCampPlayers(IClientEntity* entityArray[], size_t nArraySize, int campMask=CAMP_MASK_ALL) = 0;

    /** 获取周围实体
    @name				: 
    @param				: 
    @return
    */
    virtual size_t getAroundEntities(UID uidMaster, IClientEntity* entityArray[], size_t nArraySize, int nDistance=32, int campMask=CAMP_MASK_ALL
        , int typeMask=(MASK_CREATURE|MASK_MONSTER|MASK_NPC|MASK_ACTOR|MASK_TANK) ) = 0;

    
};

#endif // __I_CLIENTENTITY_H__