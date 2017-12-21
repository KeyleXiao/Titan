/*******************************************************************
** 文件名:	EntityBase.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			实体基类声明
********************************************************************/


#pragma once

#include "IEntityProperty.h"
#include "IClientEntity.h"
#include "Vector3.h"
#include "EntityStandState.h"
#include "EntityDieState.h"
#include "EntityFightState.h"

class EntityBase : public IClientEntity
{
protected:
    IEntityPart *   m_pPartList[ENTITY_PART_MAX];		// 记录实体Part指针
    ENTITY_ID       m_viewID;                           // 显示层ID

    // 是否在视野内
    bool            m_bOnView;

    BYTE            m_byteEntityType;

    // 当前控制状态
    BYTE				m_byCurControlState;
    // 当前状态
    CEntityState*		m_pCurrentState;
    // 站立状态
    CEntityStandState	m_EntityStandState;
    // 死亡状态
    CEntityDieState		m_EntityDieState;
	// 战斗状态
	CEntityFightState	m_EntityFightState;

public:
    EntityBase();

    virtual~ EntityBase() {}

    virtual bool isOnView() { return m_bOnView; }

    /************************************************************************/
    /* IEntity                                                              */
    /************************************************************************/
    virtual bool create( UID uid,SEntiyExportShadowContext &shadowData,void * context,int len );

    // 销毁
    virtual void destroy(EntityDestroyReason eReason = DESTROY_REASON_NORMAL, void* pParam = NULL);

    virtual UID getUID() const;

    virtual bool isHero() const;

    virtual ENTITY_MASK getTypeMask() { return (ENTITY_MASK)(MASK_MONSTER | MASK_CREATURE); }

    virtual const char * getName() const;

    virtual int getMapID() const;

    virtual Vector3 getLocation() const;

    virtual Vector3 getAngle() const;

    virtual IEntityPart * getEntityPart( ENTITY_PART id );

    virtual void sendCommand( int cmdid,void * data,int32 len );

    // 处理实体消息
    virtual void handMessage(SNetMsgHead* head, void * pEntityHead, BYTE byPartID, int msgId,void * data,int len );

    virtual void fireEvent( int eventID,void *data,int len );

    virtual bool fireVote( int eventID,void *data,int len );

    virtual bool onSerialize(UID uid, rkt::ibuffer & in,int flag );

    virtual bool deSerialize(rkt::obuf & out,int flag );

    /************************************************************************/
    /* IClientEntity                                                        */
    /************************************************************************/
    /** 创建一个显示层的实体
    @name				: NOTE!此函数只负责创建一个最基础的实体
    @param				: 
    @return
    */
    virtual bool        createView( int flag );

    /** 更新实体显示层的参数
    @name				: 
    @param				: 
	@param				: 
    @return
    */
    virtual void        updateView( bool bOnView ,bool bUseTranSport = true);

    virtual ENTITY_ID   getViewID() { return m_viewID; }

    // 改变当前状态 
    virtual bool        changeState( int nState );

    // 取得当前状态 
    virtual int	        getState();

	// 发送命令到显示层实体,用于封装getRenderView()->sendEntityCommand
	virtual void		sendCommandToEntityView(int cmdID, int nParam, const char* strParam, void* ptrParam, int len);

    /** 根据部件ID创建一个新的部件
    @name				: 
    @param				: 部件ID
    @return
    */
    virtual IEntityPart * createEntityPart( ENTITY_PART id );

    /** 获取实体显示层的基础信息(用于createView)
    @name				: 
    @param				: 
    @return
    */
    virtual bool getBasicViewInfo( EntityViewItem& item )
    {
        return false;
    }

    inline IEntityProperty * getProperty() const
    {
        // 注意：多继承时有可能会将基类空指针转成非空的派生类指针，所以这里先判断基类指针是否为空
        IEntityPart * pPropertyPart = m_pPartList[PART_PROPERTY];
        return ( pPropertyPart==NULL )? NULL : (IEntityProperty *)pPropertyPart;
    }
    inline IRenderViewProxy* getRender()
    {
        return gClientGlobal->getRenderView();
    }
};
