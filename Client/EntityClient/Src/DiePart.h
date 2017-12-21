/*******************************************************************
** 文件名:	DiePart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	1/8/2015
** 版  本:	1.0
** 描  述:	实体死亡部件

********************************************************************/

#pragma once

#include "IDiePart.h"
#include "PathFindDef.h"
#include "IPathFindService.h"
#include "IPathFindManager.h"
#include "GameViewDef.h"
// 实体死亡部件
class DiePart : public IDiePart, public rkt::IEventExecuteSink
{
public:
	////////////////////IEntityPart/////////////////////////////////////////////
	virtual bool		onLoad( IEntity* pEntity);

	virtual void		release();

	virtual short		getPartID();

	virtual IEntity *	getHost();

	// 消息通讯处理
	virtual bool		onMessage(void * pEntityHead, int msgId, void * data, size_t len);

	// 显示层的命令处理
	virtual bool 		onCommand( int cmdid,void * data,size_t len );
    
    virtual void        restore();

	////////////////////ISerializable/////////////////////////////////////////////
	virtual bool		onSerialize(UID uid,  rkt::ibuffer & in,int flag );

	virtual bool		deSerialize( rkt::obuf & out,int flag );

	////////////////////DiePart/////////////////////////////////////////////
    // 爆炸死亡后 不显示尸体
    virtual bool isHideBody();

	virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	DiePart();
	~DiePart();
    void addDynamicObstacle(SObstacleNode& node);
    void removeDynamicObstacle();
	void updateDynamicObstacleMsg(const SObstacleNode& node);
private:
	void				onRelive(void * data, int32 len);
	void				onDie(void * data, int32 len);
    // 显示层加载完成
    void                onCommand_viewObjectLoadCompleted(void);
    void updateHideBody(int nDeadType);

	// 是否有动态障碍
	bool                IsObstacle();

private:
    BYTE                    m_serState;
    DWORD                   m_dwDeadTick;	// 死亡时刻
    int                     m_nReliveTick;
	IEntity*				m_pMaster;			// 实体对象

    SObstacleNode*			m_SObstacleNode;

    bool                    m_bHideBody;
};