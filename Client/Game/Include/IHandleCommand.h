#ifndef __IHandleCommand_H__
#define __IHandleCommand_H__

#define MOVE_POS_TIME	64		// 移动命令间隔时间

#define COMMAND_TIME_OUT	2000	// 命令超时时间

#define COMMAND_DISTANCE    5       // 命令放弃距离

// 命令类型
enum CommandType
{
    // 攻击
    CommandType_Attack,
    // 靠近点
    CommandType_Close,
    // 靠近目标
    CommandType_Pursuit,
    // 最大值
    CommandType_Max,
};

/**
@name : 操作命令对象接口
*/
struct IHandleCommand
{
	virtual void release() = 0;

	virtual bool run() = 0;

	virtual void update() = 0;

	virtual void printInfo(ulong time) = 0;

	// 取得超时时间
	virtual DWORD getTimeOut() = 0;

    // 取得命令类型
    virtual int getComandType() = 0;
};

struct ISpell;
// 攻击命令结构
struct SHandleCommand_Attack
{
	UID			uidOperator;		// 使用技能的操作者（不一定是发起者，如载具上面的人），如果填0，表示操作者为技能部件拥有者
	UID			uidTarget;			// 使用技能时鼠标点击的位置上的实体对象
	Vector3		ptTargetTile;		// 使用技能时鼠标点击的位置
	int			nID;				// 技能ID
    ISpell*     pSpell;             // 技能对象（在命令队列中不要总查询，移除技能时维护好）
	int			nSlotIndex;			// 槽索引
	SLOT_TYPE	nSlotType;			// 槽位类型	(1、装备 2、技能)
    DWORD       dwTick;             // 时间
    Vector3     ptSelfTile;         // 使用技能自己位置
#ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
    DWORD       dwViewOptTick;      // 显示层触发时间
#endif

	SHandleCommand_Attack()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 靠近坐标点命令结构
struct SHandleCommand_Close
{
	Vector3		ptTargetTile;		// 目标点
	float		fDistance;			// 距离
    Vector3     ptSelfTile;         // 自己距离

	SHandleCommand_Close()
	{
		memset(this, 0, sizeof(*this));
	}
};

// 靠近目标命令结构
struct SHandleCommand_Pursuit
{
	UID			uidTarget;			// 目标
	float		fDistance;			// 到达距离
	float		fGiveUpDistance;	// 放弃距离
    Vector3     ptSelfTile;         // 自己距离

	SHandleCommand_Pursuit()
	{
		memset(this, 0, sizeof(*this));
	}
};


/**
@name : 操作命令工厂接口
*/
struct IHandleCommandFactory
{
	/**
	@name            : 增加攻击命令
	*/
	virtual void addAttackCommand(const SHandleCommand_Attack &data) = 0;

	/**
	@name            : 清除攻击命令
	*/
	virtual void clearAttackCommand(void) = 0;


	/**
	@name            : 创建攻击命令
	@return          : 返回攻击命令对象指针
	*/
	virtual IHandleCommand *CreateCommand_Attack(const SHandleCommand_Attack &data) = 0;

	/**
	@name            : 靠近目标点命令
	@return          : 返回靠近目标点命令对象指针
	*/
	virtual IHandleCommand *CreateCommand_Close(const SHandleCommand_Close &data) = 0;

	/**
	@name            : 靠近目标命令
	@return          : 返回靠近目标命令对象指针
	*/
	virtual IHandleCommand *CreateCommand_Pursuit(const SHandleCommand_Pursuit &data) = 0;
};


/**
@name : 操作命令管理接口
*/
struct IHandleCommandManager
{
	// 获取命令工厂接口
	virtual IHandleCommandFactory *getHandleCommandFactory() = 0;

	/** 创建其它要素，设置时钟
	*/
	virtual bool create() = 0;

	/** 显式关闭，特别是时钟，因为始终与系统有关
		系，在析构关闭时钟很危险
	*/
	virtual void close() = 0;

	virtual void update() = 0;

	virtual bool isIdle() = 0;

	/** 添加一个命令到队列尾部
	*/
	virtual void appendCommandTail(IHandleCommand *pCmd) = 0;

	/**  插入到表头部
	*/
	virtual void insertCommandFront(IHandleCommand *pCmd) = 0;

	/** 清除队列
	*/
	virtual void clear() = 0;

	/** 清除除了本指令以外的其他指令
	*/
	virtual void clearBefore(IHandleCommand *pCmd) = 0;

	/** 清除除了本指令之后的其他指令
	*/
	virtual void clearAfter(IHandleCommand *pCmd) = 0;

	// 移除某项指令
	virtual void RemoveCommand(IHandleCommand *pCommand) = 0;

    // 获取队列命令数量
    virtual int getCommandCount(void) = 0;
};

#endif __IHandleCommand_H__