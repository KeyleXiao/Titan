/******************************************************************
** 文件名:	HandleCommandAttack.h
** 版  权:	(C)  深圳冰川网络技术有限公司
** 创建人:
** 日  期:
** 版  本:	1.0
** 描  述:	攻击命令		
** 应  用:  
******************************************************************/
#ifndef __HandleCommandAttack_H__
#define __HandleCommandAttack_H__

#include "IHandleCommand.h"
/**
@name : 攻击命令对象
@brief: 1.攻击失败要尝试自动调整(例如距离太远要自动靠近,MP不够要自动加蓝)
        2.如果是连发技能要自动连发
*/
class HandleCommandAttack : public IHandleCommand
{
	// 攻击放弃距离
	#define ATTACK_GIVE_UP_DISTANCE				64
	// 跳跃技能槽位索引
	#define ATTACK_JUMP_SPELL_SLOT_INDEX		13
public:
	/////////////////////////////////////////IHandleCommand/////////////////////////////////////////
	virtual void release();
	virtual bool run();
	virtual void update();
	virtual void printInfo(ulong time)
	{
		WarningLn("HandleCommandAttack更新超时 200 毫秒" << time << ", spellid=" << m_Data.nID);
	}

	virtual DWORD getTimeOut();

    // 取得命令类型
    virtual int getComandType()
    {
        return CommandType_Attack;
    }

public:
	/////////////////////////////////////////HandleCommandAttack/////////////////////////////////////////
	// 构造函数
	HandleCommandAttack(const SHandleCommand_Attack &data);
	// 析构函数
	~HandleCommandAttack();

	// 取得攻击数据
	const SHandleCommand_Attack *getAttackData(void);

private:
	// 发送攻击数据
	void SendAttackData();

    // 设置停止移动
    void setStopMove(void);

    // 镜头自动转向攻击对象
    void rotateCameraToTarget(void);

    // 循环攻击
    bool loopAttack(void);

    // 绘制技能贴花
    void drawSpellDecal(void);

    // 处理发送 以及循环技能逻辑  运行完返回true 没有运行完返回false
    bool doCommand(ISpellPart* pSpellPart, STargetInfo* pTargetInfo);
    
private:
	SHandleCommand_Attack			m_Data;

	DWORD							m_dwTimeOut;		// 超时时间

    DWORD                           m_dwCount;          // 当前攻击次数
};

// 1.维护一个技能队列，用来顺序处理技能释放命令
// 2.根据技能的优先级，让优先级高的技能打断优先级低的技能
class AttackCmdQueue:public SingletonEx<AttackCmdQueue>, public IEventExecuteSink
{
    friend class HandleCommandAttack;
protected:
	int						m_nRunningSpell;		    /// 当前在命令处理队列里的技能
    typedef list<SHandleCommand_Attack> COMMAND_LIST;   /// 缓存命令队列
    COMMAND_LIST            m_CommandAttackList;        /// 缓存命令队列
   
    bool                    m_bBlocking;
    DWORD                   m_dwBlockTick;

public:
	AttackCmdQueue();
	~AttackCmdQueue();

	bool create();
	void release();

	void addAttackCommand(const SHandleCommand_Attack &data);
	void clearAttackCommand();
	bool isAttackFinished() const;
    void setNextAttack();
    // 检测后续技能是否已冷却好
    bool checkNextAttackCommand();

    // 从队列中移除指定技能
    void removeCommandBySpellID(int nSpellID);

private:
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);
};

#endif // __HandleCommandAttack_H__