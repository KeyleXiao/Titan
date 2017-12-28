/*******************************************************************
** 文件名:	e:\Rocket\Server\EntityServer\Src\Tank.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2015-2-26
** 版  本:	1.0
** 描  述:	载具
** 应  用:
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IEntity.h"
#include "DTankModule.h"
#include "ITank.h"
#include "EntityEvent.h"

#include "EntityStandState.h"
#include "EntityDieState.h"
#include "EntityFightState.h"
#include "EntityUseFlagMgr.h"

class CTank : public ITank
{
public:
	////////////////////////IEntity////////////////////////////////////
protected:
	__IEntityPart *		m_pPartList[ENTITY_PART_MAX];		// 记录实体Part指针
	EntityEvent   *		m_pEventEngine;
	int					m_nMapID;
    char                m_szName[ACTORNAME_MAXSIZE];

	// 当前控制状态
	BYTE				m_byCurControlState;
	// 当前状态
	CEntityState*		m_pCurrentState;
	// 站立状态
	CEntityStandState	m_EntityStandState;
	// 死亡状态
	CEntityDieState		m_EntityDieState;
	// 战斗死亡状态
	CEntityFightState	m_EntityFightState;

public:
	CTank();

	virtual ~CTank();

	virtual bool create(UID uid,void * context,int len,int flag);

	virtual void release();

	void destroy();

	virtual UID getUID() const;

	virtual ENTITY_TYPE getType();

	virtual char * getName();

	virtual int getSceneID() const;

	// 取得当前所在地图ID
	virtual int getMapID() const;

	virtual Vector3 getLocation() const;

	virtual void setLocation(Vector3 vLoc);

    virtual Vector3 getAngle() const;

	virtual __IEntityPart * getEntityPart(ENTITY_PART id);

	virtual void handleMessage( void * data,int32 len );

	// 改变当前状态 
	virtual bool changeState(int nState);

	// 取得当前状态 
	virtual int getState(void);

	// 取得数值属性
	virtual int getIntProperty(int nPropID);

    // 设置数值属性
    virtual bool setIntProperty(int nPropID, int nValue);

	// 取得字符串属性
	virtual std::string getStringProperty(int nPropID);

	// 取得来源游戏世界ID
	virtual	int	getFromGameWorldID();

	virtual bool onSerialize( rkt::ibuffer & in,int flag );

	virtual bool deSerialize( rkt::obuf & out,int flag );

	virtual __IEntityEvent * getEntityEvent() { return m_pEventEngine;};
	/* 处理 属性事件
	@parm:	bBefore为 true时表示 属性写入前调用此接口  
					  false 表示 属性写入后调用此接口
	@parm:  data 写入前为 相应要修改的属性的值的地址写入后没有使用
	*/
	virtual void OnPropertyChange(bool bBefore,int prop_id, void * data,int32 len){};

	////////////////////////ITank////////////////////////////////////
	/** 取得载具称号
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR				GetTitle(void);

	/** 设置载具名字
	@param   
	@param   
	@return  
	*/
	virtual void				SetTitle(LPCSTR pszTitle);

	/** 取得本载具配置信息
	@param   
	@param   
	@return  
	*/
	virtual STankSchemeInfo *	GetSchemeInfo(void);

	/** 解散自已
	@param   
	@param   
	@return  
	*/
	//virtual void				KillSelf(void);

	/** 测试上次移除的tile位置
	@param   
	@param   
	@return  
	*/
	virtual void				DebugLastRemoveTile(POINT ptLastTile);

	/** 导出数据库属性现场
	@param   
	@param   
	@return  
	*/
	virtual bool				ExportMapContext(LPSTR pszContext, int &nLen);

	/** 读配置文件，添加buf
	@param   
	@param   
	@return  
	*/
	virtual bool				AddBuffFromScheme(void);

	/** 判断是否能随时跳出载具
	@name				: 
	@param				: 
	@return	
	*/
	virtual bool				CanJumpOut(void);

	/** 判断是否禁用操作提示
	@name				: 
	@param				: 
	@return
	*/
	virtual bool				IsForbidTip(void);

	/** 是否移动时打断进度条
	@name				: 
	@param				: 
	@return
	*/
	virtual bool				IsMoveBreakProgBar(void);


	/** 取得扩展数据
	@param   
	@param   
	@return  
	*/
	LPCSTR						GetExtendData(void);

	/** 设置扩展数据
	@param szExtendData 扩展数据
	@param nLen			扩展数据长度  
	@return  
	*/
	bool						SetExtendData(LPCSTR szExtendData, int nLen);

	////////////////////////CTank///////////////////////////////////
	/** 释放,会释放内存
	@param   
	@param   
	@return  
	*/
	void						Release(void);

	/** 创建
	@param   
	@param   
	@return  
	*/
	bool						Create(void);

	/** 清空数据
	@param   
	@param   
	@return  
	*/
	void						Clean(void);


	/** 是否PK载具
	@return  
	*/
	virtual bool				IsPKTank();

	/** PK载具是否有当前技能
	@return  
	*/
	virtual bool				IfHavePKTankSkill(int SpellID);

	// 设置地图ID
	void						setMapID(int nMapID);

    // 添加效果 返回效果ID记录
    virtual	int                 mobaGoodsAddEffect(int nEffectID,void * pAddContext, size_t len);

    // 移除效果
    virtual	void                mobaGoodsRemoveEffect(int nID);

    // 获取附属实体管理对象
    virtual IBelongEntityMgr*   getBelongEntityMgr(void) { return NULL; }

    // 获取用途标识对象
    virtual IUseFlag*           getUseFlag() { return NULL; }

private:


	// 是否回收利用的，调试用
	bool						m_bRecycle;

	// 是否在场景上
	bool						m_bOnZoneFlag;

	// 是否在实体世界内
	bool						m_bInEntityWorldFlag;

	// 本载具配置信息
	STankSchemeInfo	*			m_pTankSchemeInfo;

	//PK技能表
	std::map<ushort, ushort>    m_mPKSkills;
};