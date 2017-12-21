/*******************************************************************
** 文件名:	IMovePart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	9/9/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once

/**
	移动部件: 
	1.把实体的移动信息定时发送给服务器
	2.接收服务器发过来的其他实体的移动信息,在本客户端上模拟显示
*/

#include "IMovePart.h"
#include "IEventEngine.h"
#include "EntityViewDef.h"
#include <math.h>
#include "TimerHandler.h"
#include "MovePath.h"
#include "EntityDef.h"

class MovePart : public IMovePart, public IEventExecuteSink, public rkt::TimerHandler
{
public:
    enum
    {
        Timer_Move_Update,
    };

	MovePart();

    ////////////////////IEntityPart//////////////////////////////////////////////////////
    virtual void release();
	virtual short getPartID() { return PART_MOVE;}

	virtual bool onLoad( IEntity* pEntity);

	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);

	virtual bool onCommand( int cmdid,void * data,size_t len );
	
	virtual IEntity * getHost(){ return m_pMaster; }


	virtual bool onSerialize( UID uid, rkt::ibuffer & in,int flag );
	virtual bool deSerialize( rkt::obuf & out,int flag );

    virtual void restore();

    ////////////////////IMovePart/////////////////////////////////////////////////////
    virtual void onSyncPostion( DWORD dwTickcount, const Move3 & move );

    virtual void onForceMove( const cmd_force_move & move );

	// 向目标移动
	virtual void onMovePos( const cmd_creature_move_pos &movePos );

    // 获取实体当前移动三元组
    virtual Move3 getMoveContext();

    // 切换操作模式(假人，影响假人后退攻击时朝向)  
    virtual void setOperateMode(int nMode, Vector3& vTarget);

    virtual void setRobotMode(bool bSet);

    ////////////////////IEventExecuteSink//////////////////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
    virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ////////////////////ITimerHandler//////////////////////////////////////////////////////
    virtual void OnTimer( unsigned long dwTimerID );
private:
	/************************************************************************/
	/* 处理显示层的各种命令                                                    */
	/************************************************************************/
	void onCmdMove(cmd_creature_rigidbody_sync& pos);

	void onCmdCtrl(cmd_creature_act_sync& data);

	void onCmdLand(cmd_creature_act_landspeed& data);

    void onCmdSetFormType(cmd_creature_act_set_formtype& data);

    // 触发转镜头
	void onCmdTurnCamera(int nCameIndex);
private:
	/************************************************************************/
	/* 处理各种网络消息                                                        */
	/************************************************************************/
	void onMsgMove(void* data, int len);

	void onMsgCtrl(msg_creature_act_sync& data);

	// 传送
	void onMsgTransport(void* data, int len);

    void onMsgReportLocLater(msg_request_report_loc_later& data);

private:
    bool gotoBehaviour(BEHAVIOURTYPE eType);

    void updateMoveState(Move3& move, bool bNotifyView = true);

    /** 同步非主角位置到显示层
	*/
    void syncMoveState2View( DWORD dwTickcount, const Move3 & move );

    void update();

    void reportAILoc();

    // 是否丢弃包
    bool isDropPkg(WORD wPkgSeq, WORD wPkgPrecondSeq);

protected:
	IEntity*		m_pMaster;
    unsigned int    m_nSession;

    BEHAVIOURTYPE   m_eBehaviourType;        
    Behaviour*      m_Behaviours[BEHAVIOUR_TYPE_MAX];

    // 上次给显示层更新位置的时刻
    DWORD           m_dwLastSynPos2ViewTick;

    unsigned int    m_uReportLocSerial;

	int             m_mode;
	Vector3         m_target;

    bool            m_bRobotMode;

    int             m_nCreatureState;

    WORD            m_wSendPkgSeq;           // 向服务器同步的包的序号
    WORD            m_wSendKeyPkgSeq;        // 向服务器同步的关键包的序号(普通包的前置包)

    WORD            m_wRcvPkgSeq;         // 服务器同步的包序号    
};

