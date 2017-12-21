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

struct SPELL_CONTEXT;
struct CONDITION_CONTEXT;
struct SHandleCommand_Attack;
struct SSlotSkillInfo;
struct STargetInfo;
struct SNewTargetInfo;

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
	virtual void onLoad(IEntity *pEntity) = 0;

	// 取得技能ID
	virtual int getID() = 0;

	// 克隆一个技能对象，因为每个玩家都需要一个
	virtual ISpell * clone() = 0;

	// 获取状态标志，例如攻击计数,是否在吟唱状态
	virtual int getFlag(int nIndex) = 0;

	// 设置状态标志: 这个状态标志可以有很多功能，例如攻击计数
	virtual void setFlag( int nIndex, int nFlag ) = 0;

	// 获取某个技能数值
	virtual int getSpellData( SPELL_DATA_ID nIndex ) = 0;

	// 获取某个技能数值
	virtual const char * getSpellDataStr( SPELL_DATA_ID nIndex ) = 0;

	// 添加天赋数据: 天赋数据可对技能数值进行影响
	// @param nTalentID : 天赋ID，可以用这个ID查到天赋说明,同一个天赋ID是不能重复添加的
	// @param pTalentEffect : 增加的天赋效果
	// @param nToken: 令牌（删除时传入同样令牌可移除天赋)
	virtual bool addTalent( int nTalentID) = 0;

	// 移除天赋数据
	// @param nToken : 添加时传入的令牌
	virtual bool removeTalent( int nTalentID) = 0;

	// 更新技能数据
	// @param pData : 除技能ID外，其他都可改变，连招用时，目前只支持同类型的技能连招
	virtual void updateSpellData( SPELL_DATA * pData ) = 0;

	// 处理施法事件: 
	virtual bool onSpellEvent( SPELL_EVENT nEventID,SPELL_CONTEXT * context ) = 0;

	// 该技能是否已经发动
	virtual bool isWorking() = 0;

	// 施法结果
	virtual SpellResult castResult(CONDITION_CONTEXT *pContext) = 0;

    // 获取技能配置数据
    virtual SPELL_DATA *getSpellData() = 0;

    // 设置某个技能数值
    virtual void setSpellData(SPELL_DATA_ID nIndex, int nIndexData, const char* pData) = 0;

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
struct ISpellPart : public IEntityPart
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
	virtual bool addSpell( int nSpellID ) = 0;

    // 移除技能
    virtual void removeSpell(int nSpellID) = 0;

	// 施法技能
	virtual bool castSpell( SPELL_CONTEXT * context ) = 0;

	// 升级技能
	virtual bool upgradeSpell( int nSpellID ) = 0;

    // 激活天赋
    virtual bool activeTalent( int nTalentID ) = 0;

	// 取得最后施法时间
	virtual DWORD getLastCastTime(void) = 0;

	// 能否施法
	virtual SpellResult castResult(const SHandleCommand_Attack &Attack) = 0;

	// 取得技能槽等级
	virtual int getSpellSlotLevel(int nSlotIndex) = 0;

	// 技能询问
	virtual void spellAttribute(const SSlotSkillInfo &sSlotInfo) = 0;

    // 取得目标信息
    virtual STargetInfo *getTargetInfo(void) = 0;

    // 取得打断循环攻击
    virtual bool getBreakLoopAttack(void) = 0;

    // 设置打断循环攻击
    virtual void setBreakLoopAttack(bool bBreak) = 0;

    // 增加取新目标信息
    virtual void addNewTargetInfo(const SNewTargetInfo &newTargetInfo) = 0;

    virtual void restore(){};
};

// ================================================================================//
/**
   技能工厂: 加载技能模版，并创建技能
*/
// ================================================================================//
struct ISpellFactory
{
	/** 装载脚本
	@param szDataFile : 技能数据脚本
	@param szLogicFile: 技能逻辑脚本
	*/
	virtual bool Load(const char * szDataFile,const char * szLogicFile) = 0;

	// 获取技能配置数据
	virtual SPELL_DATA *getSpellData( int nSpellID ) = 0;

	// 创建一个技能
	virtual ISpell *createSpell( int nSpellID ) = 0;

	// 创建一个技能部件
	virtual ISpellPart *createSpellPart() = 0;

	virtual void release() = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
#include "LoadLib.h"

#if defined(_LIB) || defined(SPELLFACTORY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(SpellClient))
extern "C" ISpellFactory * CreateSpellFactory();
#	define	CreateSpellFactoryProc	CreateSpellFactory
#else													/// 动态库版本
typedef ISpellFactory * (RKT_CDECL * procCreateSpellFactory)();
#	define	CreateSpellFactoryProc	DllApi<procCreateSpellFactory>::load(MAKE_DLL_NAME(SpellClient), "CreateSpellFactory")
#endif
