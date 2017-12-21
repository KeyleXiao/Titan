/*******************************************************************
** 文件名:	IGameSceneService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/30/2014
** 版  本:	1.0
** 描  述:	

			场景管理器
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "AOI_Define.h"
#include "share_ptr.h"
#include "buffer.h"
#include "Ref_Count_Obj.h"
#include <string>
#include "PackagePtr.h"
#include "EntityDef.h"
#include "Simple_Atom_Lock.h"

struct IGameSceneService;

struct AOI_Proxy;
typedef share_ptr<AOI_Proxy>     AOI_PROXY_PTR;   // 这个不用智能指针了，各种序列化等问题避免不清晰

#pragma pack(1)
struct AOI_PROXY_DATA
{
	UID		uid;
	Vector3	loc;
};
#pragma pack()


#define MAX_ATTACKING_TARGET_NUM 32

struct AttackingTarget
{
	UID uidTarget;
	unsigned int nAttackTick;
};

// 实体快照
struct Entity_Snapshot
{
    UID             m_uid;                                
    int             m_nVocation;                // 职业
	int				m_nVocationType;			// 职业类型:坦克 战士 法师 射手 辅助
    uint8           m_nCamp;                    // 阵营
    Vector3         m_Loc;                      // 位置
    int             m_nState;                   // 状态（死亡、复活等）
    uint16          m_nVisibaleFlag;			// 相对各个阵营的可见标志
    int             m_PathID;                   // 路径ID(小兵和英雄的) 
    int             m_nLevel;                   // 等级
    int             m_nCurHp;                   // 当前血量
    int             m_nMaxHp;                   // 最大血量
    int             m_nCurMp;                   // 当前蓝量
    int             m_nMaxMp;                   // 最大蓝量
    int             m_nCurStamina;              // 当前体力
    bool            m_bCanSelected;             // 是否可以被选中    
    int             m_nWhere;                   // 地面 高台 空中...
	int				m_nEcm;						// 经济总量
	int				m_nPower;					// 战力值
	int				m_nDandgerHp;				// 残血值
	AttackingTarget m_AttackingTargets[MAX_ATTACKING_TARGET_NUM];    // 正在攻击的目标

    Entity_Snapshot() : m_uid(INVALID_UID), m_PathID(-1), m_bCanSelected(true), m_nWhere(0), m_nEcm(0), m_nPower(0), m_nVocationType(0), m_nDandgerHp(200)
    {
		memset(m_AttackingTargets, 0, sizeof(m_AttackingTargets));
    }

    Entity_Snapshot(const Entity_Snapshot& rhs)
    {
        m_uid = rhs.m_uid;
        m_nVocation = rhs.m_nVocation;
		m_nVocationType = rhs.m_nVocationType;
        m_nCamp = rhs.m_nCamp; 
        m_Loc = rhs.m_Loc;
        m_nVisibaleFlag = rhs.m_nVisibaleFlag;
        m_PathID = rhs.m_PathID;
        m_nLevel = rhs.m_nLevel;
        m_nCurHp = rhs.m_nCurHp;
        m_nMaxHp = rhs.m_nMaxHp;
        m_nCurMp = rhs.m_nCurMp;
        m_nMaxMp = rhs.m_nMaxMp;
        m_nState = rhs.m_nState;
        m_nCurStamina = rhs.m_nCurStamina;
        m_bCanSelected = rhs.m_bCanSelected;
        m_nWhere = rhs.m_nWhere;
		m_nEcm = rhs.m_nEcm;
		m_nPower = rhs.m_nPower;
		m_nDandgerHp = rhs.m_nDandgerHp;
		memcpy(m_AttackingTargets, rhs.m_AttackingTargets, sizeof(m_AttackingTargets));
    }
};

// 视野代理对象，由应用层实现
struct AOI_Proxy : public Entity_Snapshot
{
    int             m_nForestID;                // 所在草丛ID
    DWORD           m_ExposedEndTick[3];        // 对各个阵营暴露截止时间(在草丛ID变化时需要清理)
	uint16          m_nKeepInsightFlag;         // 永久性对某个阵营暴露视野
    bool            m_bIsStealing;              // 是否是隐形单位
    bool            m_bIsAntiStealth;           // 是否是反隐形单位
    uint8           m_nSight;                   // 搜索视野
    uint8           m_nSightInGrid;             // 以网格为单位的视野
    bool            m_bGhostOld;
	bool            m_bGhost;                   // 这个状态下任何人都看不到，包括自己
    bool            m_bFlying;                  // 是否飞行
    DWORD           m_dwCurStateStartTick;      // 当前状态开始时间 
    Vector3         m_LookLoc;                  // 观察点,上帝视角时有值
    UID             m_uidPKTarget;              // pk目标
    bool            m_bObserver;                // 是否是观察者

	const AOI_PROXY_PTR  *  m_pSharePtr;        // 反查智能指针，临时做法

	volatile	LONG m_MutexBlock;		// 互斥标志

	void setSharePtr( const AOI_PROXY_PTR * ptr )
	{
		Simple_Atom_Lock lock(&m_MutexBlock);
		m_pSharePtr = ptr;
	}

	AOI_PROXY_PTR getSharePtr()
	{
		Simple_Atom_Lock lock(&m_MutexBlock);
		if ( m_pSharePtr==0 )
			return AOI_PROXY_PTR(0);

		return *m_pSharePtr;
	}

	// 收到广播数据
	virtual void onBroadcast( BROADCAST_ID id,AOI_Proxy * pSrc,PACKAGE_PTR package ) = 0;

	// 收到进出视野事件
	virtual void onInOutSight( int camp,bool bInSight, UID uidTarget=INVALID_UID ) = 0;

    // 收到进出草丛事件(客户端透明处理)
    virtual void onInOutForest( int camp, int nForestID ) = 0;

	// 向某个玩家或者某个阵营通知自己创建的消息
	virtual void onCreateNotify( UID uidAcceptor,int nAcceptCamp=-1 ) = 0;

	// 向某个玩家或者某个阵营通知自己消失的消息
	virtual void onDestroyNotify( UID uidAcceptor,int nAcceptCamp=-1 ) = 0;

	virtual ~AOI_Proxy(){};
};


// 场景更新共享信息
struct SceneUpdateShareInfo
{
    int     nMapID;             // 静态地图ID
    int     nSceneID;           // 地图地图ID
    int     nPlayerQty;         // 人物数量
    int     nMonsterQty;        // 怪物数量

    SceneUpdateShareInfo()
    {
        memset(this, 0, sizeof(*this));
    }
};
/**
	游戏场景对象
*/
struct  IGameSceneService
{
	// 创建场景,传入ID和场景配置信息
	virtual bool onCreate( int mapID,int sceneID, bool isCopy, int nWarID=INVALID_WAR_ID ) = 0;

	// 获取唯一场景ID
	virtual int getSceneID() = 0;

	// 获取地图ID，同一张地图可以创建多个副本
	virtual int getMapID() = 0;

	// 插入实体
	virtual bool insertEntity( AOI_PROXY_PTR proxy,Vector3 loc, int flag = MSG_FLAG_DEFAULT ) = 0;

	// 移除实体
	virtual bool removeEntity( AOI_PROXY_PTR proxy,Vector3 loc, int flag = MSG_FLAG_DEFAULT ) = 0;

	// 移动实体
	virtual bool moveEntity( AOI_PROXY_PTR proxy,Vector3 newLoc  ) = 0;

    // 添加观察者
    virtual bool setObserver( AOI_PROXY_PTR proxy, bool bObserver ) = 0;

    // 更新实体可见性
    virtual void updateObjVisible( AOI_PROXY_PTR proxy ) = 0;

    // 反隐标示发生了变化
    virtual void onUpdateObjAntiStealth( AOI_PROXY_PTR proxy ) = 0;

    // 更新视野里的实体
    virtual void updateObjsInSight(AOI_PROXY_PTR proxy) = 0;

    // 设置实体视野距离
    virtual void setSight( AOI_PROXY_PTR proxy,int nSight ) = 0;

	// 取某点周围一定范围内的格子内的所有实体 
	// @param fDist : 距离半径(注意，这里不是精确匹配的，是把一个单元格内的所有对象都返回,误差为单元格大小)
	virtual int k_nearest( Vector3 loc,float fDist,AOI_PROXY_PTR * pReturnArray,int nArraySize,int campMask=CAMP_MASK_ALL, bool bCheckDist=false ) = 0;

	// 取某点周围一定范围内的格子内的所有实体,供技能使用,按点近的优化
	// @param fDist : 距离半径(注意，这里不是精确匹配的，是把一个单元格内的所有对象都返回,误差为单元格大小)
	// @param fHeight : 技能伤害高度
	virtual int k_nearestInSpell( Vector3 loc,float fDist,float fHeight,AOI_PROXY_PTR * pReturnArray,int nArraySize,int campMask=CAMP_MASK_ALL ) = 0;

	// 取某点周围直线范围内的格子内的所有实体 
	// @param fDist : 技能直线长度
	// @param fWith : 技能直线宽度(宽度范围内人为符合要求的)
	// @param fHeight : 技能伤害高度
	virtual int k_nearLineest( Vector3 loc, Vector3 dir, float fDist, float fWith, float fHeight, AOI_PROXY_PTR * pReturnArray,int nArraySize,int campMask=CAMP_MASK_ALL ) = 0;

	// 取某点是否在扇形区域内
	// @param loc : 扇形起点，一般使用技能者点
	// @param dir : 技能方向，使用技能者点击方向，或目标方向
	// @param fr  : 技能距离，扇形半径
	// @param fr  : 技能距离平方，扇形半径的平方，传此值，避免内部平方运算
	// @param cosTheta  : 半边扇形的cos值，是以dir方向各一半扇形，所以传半边扇形的cos值
	// @param fHeight  : 能攻击高度
	virtual int k_nearSectorest( Vector3 loc,Vector3 dir, float fr, float squaredR, float cosTheta,float fHeight, AOI_PROXY_PTR * pReturnArray,int nArraySize,int campMask=CAMP_MASK_ALL ) = 0;

	// 阵营视野广播
	// @param loc : 广播给所有能看见这个点的阵营玩家 
    // @param bIgnoreSelf:广播时是否忽略自己 玩家移动不能给自己广播其他玩家移动，否则客户端表现有问题
	virtual int broadcast(Vector3 loc,int dist,BROADCAST_ID id,std::string & msg,AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0), bool ignoreSelf = false) = 0;

	// 阵营视野广播(多点）
	// @param pData : 里面是多个点的数组，能看到任意点的就需要广播
	// 使用情形： 释放飞行类技能时，起始点和结束点周围的人都应该看得见，所以需要用2个点进行判断
	virtual int broadcastMultiple(char *pData,int nCount,BROADCAST_ID id,std::string & msg,AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0)) = 0;

	// 全体玩家广播
	virtual int broadcast_all( BROADCAST_ID id,std::string & msg,int campMask=CAMP_MASK_ALL ) = 0;

    // 周围的玩家广播(在点附近才广播,用于怪物使用技能等广播)
    virtual int broadcast_nearLoc(Vector3 loc,int dist,BROADCAST_ID id,std::string & msg,AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0), bool ignoreSelf = false, int campMask=CAMP_MASK_ALL) = 0;

    // 周围的玩家广播(多点广播)
	// 使用情形： 释放飞行类技能时，起始点和结束点周围的人都应该看得见，所以需要用2个点进行判断
    virtual int broadcastMultiple_nearLoc(char *pData,int nCount,BROADCAST_ID id,std::string & msg,AOI_PROXY_PTR proxy=AOI_PROXY_PTR(0), int campMask=CAMP_MASK_ALL) = 0;

	// 获取玩家总数
	virtual int getPlayerCount() = 0;

    // 获取怪物数量
    virtual int getMonsterCount() = 0;

	// 获取所有玩家的对象列表
	// @param return : 返回真正返回的玩家数目
	virtual int getAllPlayers( AOI_PROXY_PTR * pReturnArray,int nArraySize ) = 0;

    // 获取场景更新共享数据
    virtual SceneUpdateShareInfo getUpdateShareInfo() = 0;

	// 取得场景逻辑服务ID
	virtual UINT_PTR getLogicServiceID() = 0;

    // 取得给ob分配的阵营ID
    virtual int getObCampID() = 0;

	// 关闭场景
	virtual void close() = 0;

    virtual bool on_stop() = 0;

	// 销毁地图
	virtual void release() = 0;
};
