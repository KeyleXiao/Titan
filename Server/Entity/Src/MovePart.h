/*******************************************************************
** 文件名:	MovePart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	rjc
** 日  期:	4/23/2015
** 版  本:	1.0
** 描  述:	
modifyied by ljw at 2015.8.18
			移动部件
********************************************************************/

#pragma once
#include "IMovePart.h"
#include "IEventEngine.h"
#include "IMessageDispatch.h"
#include "TimerAxis.h"
#include "ITimerService.h"
#include "MovePath.h"
#include "Barrier.h"
#include "EntityDef.h"

struct ISingleMapArea;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 玩家移动部件
class CMovePart : public __IMovePart, public TimerHandler, public IEventExecuteSink, public IEventVoteSink
{
public:
	enum {
		MOVE_UPDATE_TIMER,		// 移动更新Timer
		MOVE_ACCELERATE_TIMER,	// 加速奔跑Timer
	};

    CMovePart();
    ///////////////////////////////// __IEntityPart //////////////////////////

    virtual void release();

    virtual short getPartID();

    virtual __IEntity * getHost();

    virtual bool onLoad( __IEntity* pEntity, int nLoginMode);

    virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);

    // 切换英雄
    virtual void  onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore) { }

    virtual void  restore();

    ///////////////////////////////// ISerializable ////////////////////////////
    virtual bool onSerialize( rkt::ibuffer & in,int flag );

    virtual bool deSerialize( rkt::obuf & out,int flag );	

    ///////////////////////////////// IMovePart //////////////////////////
    // 可以在一次移动中改变速度  cmd
    virtual bool  setSpeed(float speed);

    // 移动到某点 cmd
    virtual bool  moveTo(Vector3 &loc, bool bInitiatively = false);

	// 移动
	virtual bool move(Vector3* pPointList, int nPointCount, bool bInitiatively = false);

    virtual bool jumpTo(Vector3& loc, bool bAutoCheckGround, float fGroundY);

    // 旋转朝向
    virtual void rotateTo(Vector3 dirNew);

    // 停止移动 cmd
    virtual bool  stopMoving(bool bInitiatively = false);

    // 停在某个位置 cmd nSceneID = 0 表示本地图传送
    virtual bool  stopAt(Move3& move, int nSceneID, int nReason);

    virtual bool changeMoveState(Move3& move, bool bNotifySelf = false, bool bVerify = false);
    
    virtual bool    isMoving();

    // 是否正在转向
    virtual bool isRotating();

    virtual Vector3 getMovingTarget();

    virtual float   getSpeed();

    virtual Vector3 getAngle();

    virtual Move3   getMoveState();

    // 冲锋   
    virtual bool shift(void *pData, int nDataLen);

	// 检测两点之间是否可以通过
	virtual bool canPass(Vector3 locFrom, Vector3 locTo, float fAgentHeight);

    // 设置是否可以通过结界
    virtual void setCanPassBarrier(bool bCanPass);

    // 当前的移动状态
    // 当前的移动状态
    virtual Behaviour* getCurrentBehaviour();

    virtual WORD& getSendPkgSeq(){return m_wSendPkgSeq;}

    virtual WORD& getSendKeyPkgSeq(){return m_wSendKeyPkgSeq;}

    ///////////////////////////////// TimerHandler //////////////////////////
    virtual void OnTimer( unsigned long dwTimerID );	

    //////////////////// IEventExecuteSink /////////////////////////////////////////////
    void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    virtual bool OnVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);
	
    // 设置僵直  dwEndTick:僵直结束时间
    virtual void setRigidity(DWORD dwEndTick)
	{
		m_dwRigidityEndTick = dwEndTick;
	}

    // 丢失位置(动作决定位移，之后客户端会重新上报位置)
    virtual void lostLocation();

    virtual bool isLostLocation();

    

private:

    // 部件更新
    void update();

    bool gotoBehaviour(BEHAVIOURTYPE eType, bool bNotifySelf = true);


    enum CheckMoveMask
    {
        // 检测与地形的碰撞
        CHECK_MASK_TERRAIN = 1 << 0,     
        // 检测动态的结界
        CHECK_MASK_DYNAMIC_BARRIER = 1 << 1,             
    };

    enum CheckResult
    {
        CHECK_RESULT_OK,                                   // 成功
        CHECK_RESULT_INVALID_LOC,                          // 无效的坐标
        CHECK_RESULT_BELOW_LOWEST_HEIGHT,                  // 最低高度以下(需要加死亡buff)  
        CHECK_RESULT_OUT_OF_MAP_RANGE,                     // 坐标超出地图范围
        CHECK_RESULT_HIT_TERRAIN_OBSTACLE,                 // 碰到地形障碍了
        CHECK_RESULT_HIT_DYANMIC_BARRIER,                  // 碰到动态结界了
    };
    CheckResult checkPos(Vector3& loc, int nCheckMask = CHECK_MASK_TERRAIN|CHECK_MASK_DYNAMIC_BARRIER);

    // 执行停止
    void executeStopmoving();

    // 客户端同步位置并且移动
    bool handleClientMove(void* data, int len);

    // 客户端控制
    bool handleClientCtrl(void* data, int len);

    // 客户端降落
    void handleCommandLand(msg_creature_act_land* data);

    // 客户端设置立足点类型
    void handleCommandSetFormType(msg_creature_act_set_formtype* data);

    // 客户端转镜头
    void handleCommandTurnCamera(void * data, size_t len);

    // 校验移动位置
    bool verifyClientMove(Vector3& vPoint, float * fOffset=0);

    // 客户端拉回到服务器位置
    void correctClientPos(bool bBroadcast = false, int nRaiseHeight = 0);

    // 摔死
    void fall2Death();

    // 是否遇到障碍
    bool isHitBarrier(Vector3 ptFrom, Vector3 ptTo);

    // 设置位置
    void setMoveContext(Move3& move);

    // 是否丢弃包
    bool isDropPkg(WORD wPkgSeq, WORD wPkgPrecondSeq, bool bCheckPrecondPkg);
protected:
	// 是否可以移动
	bool canMove(bool bInitiatively);

	// 移动
	bool execMove(std::vector<Vector3> vecPath);
private:
    // 冲锋遇到障碍加buff
    void shiftHitTerrainAddBuff();

    // 冲锋完加buff
    void shiftFinishAddBuff();

private:
	// 删除加速Buff
	void removeSpeedupBuff();
	// 添加加速Buff
	void addSpeedupBuff();
	// 心跳，检查加速情况
	void updateSpeedup();

private:
    __IEntity *     m_pMaster;

    BEHAVIOURTYPE   m_eBehaviour;        // 当前行为
    Behaviour*      m_Behaviours[BEHAVIOUR_TYPE_MAX];

    float           m_fSpeed;
    bool            m_bCanMove;

    DWORD           m_dwKeepDirEndTick;      // 

    DWORD           m_dwLastUpdatePosTick;   // 上次与客户端同步位移时刻

    DWORD           m_dwLastLostLocationTick; // 丢失位置时刻,此时需要客户端同步一个位置到服务器。常用于一些根据动作进行位移的技能
    unsigned int    m_uLostLocationSerialNo;  // 丢失位置序列号 ,此时需要客户端同步一个位置到服务器。常用于一些根据动作进行位移的技能

    bool            m_bAutoCheckGround;
    float           m_fGroundY;

    // 传送后客户端接下来的位置必须和传送后一致 因为客户端在收到传送消息前可能会同步位置上来
    // 这时候服务器位置已经是传送后位置 客户端同步过来的还是传送前的位置  这个同步位置是不能当做瞬移接受的
    bool            m_bEnterStrictMoveCheck;  
    DWORD           m_dwStrictCheckTimeOut;

    std::list<Barrier*> m_listBarrier;  // 障碍本地记录

    bool    m_bForbitMoveInitiatively;  // 禁止主动(自发)移动 (例如被嘲讽了或者被拖拽移动)

    int     m_nFindPathFailTimes;

    bool    m_bCanPassBarrier;        // 可以通过动态障碍标志

    int     m_nCreatureState;        // 客户端状态机

	DWORD	m_dwRcvLastMoveMsgTick;	// 最后一次收到移动消息包的时间

    DWORD   m_dwRigidityEndTick;    // 移动僵直时刻

    bool    m_bHandlingDropablePkg;  // 正在处理普通包


    WORD   m_wRcvPkgSeq;              // 当前客户端同步包的序号
    WORD   m_wSendPkgSeq;            // 发往客户端的同步包序号
    WORD   m_wSendKeyPkgSeq;         // 发往客户端的关键包的序号(普通包里需标明的前置包)
};