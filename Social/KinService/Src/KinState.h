//==========================================================================
/**
* @file	  : KinState.h
* @author : xiaobao
* created : 2009-3-7
* purpose : 家族状态类
*
* 修改记录:
* 1			增加家族移民状态	penglu	2013-12-19
*/
//==========================================================================

#ifndef __KinState_H__
#define __KinState_H__

class CKin;

// 家族状态基类
class CKinState
{
public:
	/// purpose: 构造函数
	CKinState();

	/// purpose: 虚构函数
	virtual ~CKinState();

	/// purpose: 开始状态
	virtual void Start(CKin* pKin) = NULL;

	/// purpose: 结束状态
	virtual void Stop() = NULL;

	/// purpose: 对家族对象消息的过滤
	virtual bool VoteMessage(DWORD dwMsgID, DWORD dwSrcPointID) = NULL;

protected:
	// 家族对象指针
	CKin* m_pKin;
};

// 非正式状态
class CKinInformalState : public CKinState, public TimerHandler
{
public:
	/// purpose: 构造函数
	CKinInformalState();

	/// purpose: 虚构函数
	virtual ~CKinInformalState();

	/// purpose: 开始状态
	virtual void Start(CKin* pKin);

	/// purpose: 结束状态
	virtual void Stop();

	/// purpose: 对家族对象消息的过滤
	virtual bool VoteMessage(DWORD dwMsgID, DWORD dwSrcPointID);

	//////////////////////////TimerHandler///////////////////////////
	/// purpose: 定时器触发后回调,你可以在这里编写处理代码
	virtual void OnTimer(unsigned long dwTimerID);

};

// 正常状态
class CKinFormalState : public CKinState
{
public:
	/// purpose: 构造函数
	CKinFormalState();

	/// purpose: 虚构函数
	virtual ~CKinFormalState();

	/// purpose: 开始状态
	virtual void Start(CKin* pKin);

	/// purpose: 结束状态
	virtual void Stop();

	/// purpose: 对家族对象消息的过滤
	virtual bool VoteMessage(DWORD dwMsgID, DWORD dwSrcPointID);
};

// 解散状态
class CKinDismissingState : public CKinState, public TimerHandler
{
public:
	/// purpose: 构造函数
	CKinDismissingState();

	/// purpose: 虚构函数
	virtual ~CKinDismissingState();

	/// purpose: 开始状态
	virtual void Start(CKin* pKin);

	/// purpose: 结束状态
	virtual void Stop();

	/// purpose: 对家族对象消息的过滤
	virtual bool VoteMessage(DWORD dwMsgID, DWORD dwSrcPointID);

	//////////////////////////TimerHandler///////////////////////////
	/// purpose: 定时器触发后回调,你可以在这里编写处理代码
	virtual void OnTimer(unsigned long dwTimerID);
};

#endif // __KinState_H__