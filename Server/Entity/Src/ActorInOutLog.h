/*******************************************************************
** 文件名:	ActorInOutLog.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	11/22/2016
** 版  本:	1.0
** 描  述:	人物上下线日记

********************************************************************/

#pragma once

#include "IDBEngineService.h"

struct __IEntity;

// 人物上下线日记
class CActorInOutLog : public IEventExecuteSink, public IDBRetSink
{
public:
	/** 构造函数
	*/
    CActorInOutLog(void);

	/** 析构函数
	*/
    ~CActorInOutLog(void);

	/** 创建
	*/
	bool onLoad( __IEntity* pEntity, int nLoginMode);

    void restore();

	/** 释放
	*/
    void release();

	/** 序列化
	*/
    bool onSerialize( rkt::ibuffer & in,int flag );

	/** 反序列化
	*/
    bool deSerialize( rkt::obuf & out,int flag );

	///////////////////////////////// IEventExecuteSink ////////////////////////////
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
	virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	///////////////////////////////// IDBRetSink ////////////////////////////
	/** 数据库请求返回回调方法
	@param   pDBRetSink		：结果返回接口	
	@param   nCmdID ：命令ＩＤ
	@param   nDBRetCode：数据库请求返回值，参考上面定义
	@param   pszDBRetDesc：数据库请求返回描述，由ＳＰ返回
	@param   nQueueIndex：队列定义
	@param   pOutData：输出数据
	@param   nOutLen：输出数据长度
	@return  
	@note     
	@warning 此对像千万不能在数据库返回前释放，否则会非法！
	@retval buffer 
	*/	
	virtual void        OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

private:
	/** 上线事件
	*/
    void onEventActorLogin(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	/** 下线事件
	*/
    void onEventActorLogout(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

private:
    __IEntity *      m_pMaster;

    // 登录标识，用于数据库关联上下线记录用途
    int             m_nLoginIdentity;
};