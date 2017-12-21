/*******************************************************************
** 文件名:	ClanState.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/5/2016	11:14
** 版  本:	1.0
** 描  述:	帮会状态类
********************************************************************/

#ifndef __CLANSTATE_H__
#define __CLANSTATE_H__

class CClan;

// 帮会状态基类
class CClanState
{
public:
	/// purpose: 构造函数
	CClanState();

	/// purpose: 虚构函数
	virtual ~CClanState();

	/// purpose: 开始状态
	virtual void start(CClan* pClan) = NULL;

	/// purpose: 结束状态
	virtual void stop() = NULL;

	/// purpose: 对帮会对象消息的过滤
	virtual bool voteMessage(DWORD dwMsgID) = NULL;

protected:
	// 帮会对象指针
	CClan* m_pClan;
};

// 正常状态
class CClanNormalState : public CClanState
{
public:
	/// purpose: 构造函数
	CClanNormalState();

	/// purpose: 虚构函数
	virtual ~CClanNormalState();

	/// purpose: 开始状态
	virtual void start(CClan* pClan);

	/// purpose: 结束状态
	virtual void stop();

	/// purpose: 对帮会对象消息的过滤
	virtual bool voteMessage(DWORD dwMsgID);
};


// 解散状态
class CClanDismissingState : public CClanState, public TimerHandler
{
public:
	/// purpose: 构造函数
	CClanDismissingState();

	/// purpose: 虚构函数
	virtual ~CClanDismissingState();

	/// purpose: 开始状态
	virtual void start(CClan* pClan);

	/// purpose: 结束状态
	virtual void stop();

	/// purpose: 对帮会对象消息的过滤
	virtual bool voteMessage(DWORD dwMsgID);

	//////////////////////////TimerHandler///////////////////////////
	/// purpose: 定时器触发后回调,你可以在这里编写处理代码
	virtual void OnTimer(unsigned long dwTimerID);
};

#endif // __CLANSTATE_H__