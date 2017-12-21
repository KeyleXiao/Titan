/*******************************************************************
** 文件名:	ISpell.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	1/8/2015
** 版  本:	1.0
** 描  述:	

			技能系统接口
********************************************************************/


#pragma once
#include "IEntity.h"
#include "SpellDef.h"
using namespace SPELL;

struct ATTACK_DATA;
struct IAttackObject;
struct SPELL_CONTEXT;

// ================================================================================//
/** 
   技能接口:
   1.单个技能接口，负责维护技能施放逻辑,及技能相关的天赋数据
*/
// ================================================================================//
struct ISpell
{
	// 载入技能
	// @param pEntity: 技能实体指针(属于某实体,同步方便)
	virtual void onLoad(__IEntity *pEntity, SLOT_TYPE nSlotType, int nSlotIndex, int nAddCoolTime) = 0;

	// 取得技能ID
	virtual int getID() = 0;

	// 克隆一个技能对象，因为每个玩家都需要一个
	virtual ISpell * clone() = 0;

	// 获取状态标志，例如攻击计数,是否在吟唱状态
	virtual int getFlag(int nIndex) = 0;

	// 设置状态标志: 这个状态标志可以有很多功能，例如攻击计数
	virtual void setFlag( int nIndex, int nFlag  ) = 0;

	// 获取某个技能数值
	virtual int getSpellData( SPELL_DATA_ID nIndex ) = 0;

	// 获取某个技能数值
	virtual const char * getSpellDataStr( SPELL_DATA_ID nIndex ) = 0;

    virtual bool addTalent(int nTalentID) = 0;

	// 添加天赋数据: 天赋数据可对技能数值进行影响
	// @param nTalentID : 天赋ID，可以用这个ID查到天赋说明,同一个天赋ID是不能重复添加的
	// @param isAdd : 是增加数值还是直接设置数值
	// @param nIndex: 技能数值索引
	// @param nValue: 数值大小
	// @param nToken: 令牌（删除时传入同样令牌可移除天赋)
	//virtual bool addTalent( int nTalentID,bool isAdd,SPELL_DATA_ID nIndex,int nValue) = 0;

	// 添加天赋数据: 天赋数据可对技能数值进行影响,同一个天赋ID是不能重复添加的
	// @param nTalentID : 天赋ID,可以用这个ID查到天赋说明
	// @param nIndex: 技能数值索引
	// @param nValue: 数值大小
	// @param nToken: 令牌（删除时传入同样令牌可移除天赋)
	//virtual bool addTalent( int nTalentID,SPELL_DATA_ID nIndex,const char * szValue ) = 0;

	// 移除天赋数据
	// @param nToken : 添加时传入的令牌
	virtual bool removeTalent( int nTalentID ) = 0;

	// 更新技能数据
	// @param pData : 除技能ID外，其他都可改变，连招用时，目前只支持同类型的技能连招
	virtual void updateSpellData( SPELL_DATA * pData ) = 0;

	// 处理施法事件: 
	virtual bool onSpellEvent( SPELL_EVENT nEventID,SPELL_CONTEXT * context ) = 0;

	// 该技能是否已经发动
	virtual bool isWorking() = 0;

	// 取得天赋数量
	virtual int getTalentCount(void) = 0;

	// 取得天赋数据
	// @param nIndex : 天赋索引位置
	virtual SPELL_TALENT *getTalentData(int nIndex) = 0;

	// 更新技能现场
	// @param uidTarget :		目标ID
	// @param ptTargetTile :	目标点
	virtual void updateSpellContext(UID uidTarget, Vector3 ptTargetTile) = 0;

	// 增加实体(需要管理的对象实体，调用此接口保存)
	// 因为lol中有道具有召唤怪物功能，所以此放到技能里面，对应技能管理召唤对象(一个英雄可能多个这种技能)
	// @param uidEntity :		实体UID
	// @param nMaxCount :		最大实体数量
	virtual void addEntity(UID uidEntity, int nMaxCount) = 0;

	// 移除实体(移除管理的对象实体)
	// @param uidEntity :		实体UID
	virtual void removeEntity(UID uidEntity) = 0;

	// 取得槽位类型
	virtual SLOT_TYPE getSlotType(void) = 0;

    // 取得槽位索引
    virtual int getSlotIndex(void) = 0;

    // 获得技能数据
    virtual SPELL_DATA *getSpellData(void) = 0;

    // 设置某个技能数值
    virtual void setSpellData(SPELL_DATA_ID nIndex, int nIndexData, const char* pData) = 0;

    // 获取当前技能上下文
    virtual SPELL_CONTEXT* getCurSpellContext() = 0;

    // 获取技能状态
    virtual int getState() = 0;

    // 设置Key值
    virtual void setKey(DWORD dwKey) = 0;

    // 获取Key值
    virtual DWORD getKey(void) = 0;

	// 销毁技能
	virtual void release() = 0;
};

// ================================================================================//
/** 
   实体部件接口，继承该接口可以挂接到实体上

   注意：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：：
   1.每个玩家都是一个独立协程,所以Part的实现中如果需要访问全局数据，一定要注意线程安全
   2.一个玩家的多个Part直接互相调用是线程安全的，可以放心使用
*/
// ================================================================================//
struct __ISpellPart : public __IEntityPart
{
	// 取得技能数量
	virtual int getSpellCount() = 0;

    // 取得第一个技能
    virtual ISpell *getFirstSpell(void) = 0;

    // 取得下一个技能
    virtual ISpell *getNextSpell(void) = 0;

	// 查找技能
	virtual ISpell * findSpell( int nSpellID ) = 0;

	// 添加技能
    // @param nSpellID : 技能ID
    // @param nSlotType : 槽位类型
    // @param nSlotIndex: 槽位索引
    // @param nAddCoolTime: 增加冷却,有些技能技能上次升级技能冷却
	virtual bool addSpell(int nSpellID, SLOT_TYPE nSlotType=SLOT_TYPE_SKILL, int nSlotIndex=SPELL_SLOT_MAX_COUNT, int nAddCoolTime = 0) = 0;

	// 移除技能
	virtual bool removeSpell(int nSpellID) = 0;

	// 施法技能
	virtual bool castSpell( SPELL_CONTEXT * context ) = 0;

	// 升级技能
    // @param nSlotIndex : 槽位索引
	virtual bool upgradeSpell( int nSlotIndex ) = 0;

	// 取得最后施法时间
	virtual DWORD getLastCastTime(void) = 0;

	// 设置僵直时间
	virtual void setRigidityTime( int nTime,int nSpellEffectType, int nCastFlag ) = 0;

	// 取得是否已创建实体(此函数用来知道技能和天赋改变是否需要同步客户,
	// 未创建实体时改变了技能和天赋通过deSerialize打包技能和天赋数据)
	virtual bool getCreateEntity(void) = 0;

    // 激活天赋(用作外部接口时，作为临时天赋 并不会判断激活条件，直接激活)
    // bCost需要消耗点数，有些天赋是需要金钱激活的，但用户断线重连进来,已激活的不消耗
    virtual bool activeTalent(int nTalentID, bool bCost = true) = 0;

    // 取消天赋
    virtual void deActiveTalent(int nTalentID) = 0;

	// 创建一个攻击对象
	// @param type : 攻击类型
	// @param pAttack : 攻击数据
	// @param pContext: 攻击现场
	// @return : 返回攻击对象序号
	virtual DWORD createAttackObject( ATTACK_TYPE type,ATTACK_DATA * pAttack,SPELL_CONTEXT * pContext ) = 0;

	// 取得攻击对象
	// @param dwSerialID : 对象序列号
	// @return : 攻击对象
	virtual IAttackObject *getAttackObject(DWORD dwSerialID) = 0;

    // 关闭攻击对象
    // @param dwSerialID : 对象序列号
    virtual void closeAttackObject(DWORD dwSerialID) = 0;

	// 增加技能点数
	// @param nPoint : 技能点数
	virtual void addSpellPoint(int nPoint) = 0;

	// 获得技能点数
	// @return : 获得技能点数
	virtual int getSpellPoint() = 0;

	// 取得技能槽等级
	virtual int getSpellSlotLevel(int nSlotIndex) = 0;

    // 设置技能槽位
    virtual void setSpellSlotLevel(int nSlotIndex, int nLevel) = 0;

    // 创建攻击对象序列号
    virtual DWORD CreateAttackObjectSerialID(void) = 0;
};

// ================================================================================//
/**
   技能工厂: 加载技能模版，并创建技能
*/
// ================================================================================//
struct ISpellFactory
{
	// 装载脚本
	// @param szDataFile : 技能数据脚本
	// @param szLogicFile: 技能逻辑脚本
	virtual bool Load(const char * szDataFile,const char * szLogicFile) = 0;

	// 获取技能配置数据
	virtual SPELL_DATA * getSpellData( int nSpellID ) = 0;

	// 创建一个技能
	virtual ISpell * createSpell( int nSpellID ) = 0;

	// 创建一个技能部件
	virtual __ISpellPart * createSpellPart() = 0;

	// 执行Service的on_stop
    virtual void onStop(void) = 0;

	virtual void release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(SPELL_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(Spell))
extern "C" ISpellFactory * CreateSpellFactory();
#	define	CreateSpellFactoryProc	CreateSpellFactory
#else													/// 动态库版本
typedef ISpellFactory * (RKT_CDECL * procCreateSpellFactory)();
#	define	CreateSpellFactoryProc	DllApi<procCreateSpellFactory>::load(MAKE_DLL_NAME(Spell), "CreateSpellFactory")
#endif
