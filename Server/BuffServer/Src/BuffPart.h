/*******************************************************************
** 文件名:	BuffPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/13  17:05
** 版  本:	1.0
** 描  述:	BUFF部件
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include <list>
using namespace std;
#include "IBuffServer.h"
class CBuff;
#include "IEventEngine.h"
#include "BuffMessageDef.h"
#include "BitMask.h"

#pragma pack(1)

// 导出数据库现场
struct SExportDBContext
{
	WORD		wBuffID;	// buff ID
	BYTE		byLevel;	// 等级
	WORD		wLeftTime;	// 剩余时间（分钟）

	SExportDBContext(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 导出切换地图现场
struct SExportMapContext
{
	WORD		wBuffID;	// buff ID
	BYTE		byLevel;	// 等级
	int			nLeftTime;	// 剩余时间（毫秒）

	SExportMapContext(void)
	{
		memset(this, 0, sizeof(*this));
	}		
};

#pragma pack()


class CBuffPart : public __IBuffPart, public TimerHandler, public IEventExecuteSink
{
	typedef list< CBuff *>				TLIST_BUFF;
    typedef map<DWORD,TLIST_BUFF>       MAP_BUFF;
	typedef list< SExportMapContext >	TLIST_OTHERHUN;

	struct SInitBuff
	{
		DWORD		dwBuffID;
		DWORD		dwBuffLevel;
		UID			uidAdd;
		DWORD		dwTime;
		SBuffContext BuffContext;
		int			nContextLen;

		SInitBuff(void)
		{
			memset(this, 0, sizeof(*this));
		}
	};
	typedef list< SInitBuff >			TLIST_INITBUFF;

	enum
	{
		ETimerEventID_CleanInvalidBuff = 0,	// 清除无效BUFF
		ETimerEventID_DelayRemoveBuffTimer,	// 延迟删除buff定时器
	};
public:

	///////////////////ISerializable////////////////////////////////////
	/** 序列化接口
	@return  
	*/
	virtual bool			onSerialize( rkt::ibuffer & in,int flag );

	/** 反序列化接口
	@return  
	*/
	virtual bool			deSerialize( rkt::obuf & out,int flag );

	///////////////////__IEntityPart////////////////////////////////////
	/** 取是部件ID
	*/
	virtual short			getPartID(void);

	/** 创建，重新启用，也会调此接口
	*/
	virtual bool			onLoad(__IEntity *pMaster, int nLoginMode);

	/** 释放,会释放内存
	*/
	virtual void			release(void);

	/** 取是主人
	*/
	virtual __IEntity*		getHost(void);

	/** 消息处理
	*/
	virtual bool			onMessage(void * pEntityHead, int msgId, void * data, size_t len);

    // 切换英雄
    virtual void            onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore);

    virtual void            restore();

	///////////////////IBuffPart//////////////////////////////////////
	/** 给实体添加buff前，需调用此方法检测
	@param   dwBuffID ：buffID
	@param   dwBuffLevel ：buff等级
	@param   uidAdd ：添加者
	@return  
	*/
	virtual bool			CanAdd(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd);

	/** 给实体添加buff
	@param   dwBuffID ：buffID
	@param   dwBuffLevel ：buff等级
	@param   uidAdd ：添加者 
	@param   dwTime ：BUFF持续时间，填0表示取配置时间
	@return  
	*/
	virtual bool			Add(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0);

	/** 给实体移除buff
	@param   dwBuffID ：buffID
	@param   uidAdd ：添加者，不能把别人添加的移除掉;为INVALID_UID时，移除所有dwBuffID
	@param   uidRemove ：移除者
	@return  
	*/
	virtual bool			Remove(DWORD dwBuffID, UID uidAdd, UID uidRemove);

	/** 给实体移除同类型的buff
	@param  dwBuffFlag ：buff类型 
	@param  bAllFlag ：是否移除所有的同类型的
	@param  uidRemove ：移除者
	@return  
	*/
	virtual bool			Remove(DWORD dwBuffType, bool bAllFlag, UID uidRemove);

	/** 延迟移除buff
	@param   dwBuffIndex  Buff索引
	@param   dwTime		  延迟删除时间
	*/
	virtual void			DelayRemove(DWORD dwBuffIndex, DWORD dwTime);

	/** 某个BUFF是否存在
	@param   
	@param   dwBuffID ：buffID
	@param   dwBuffLevel ：buff等级
	@param   uidAdd ：如果不管添加者，则填INVALID_UID
	@return  
	*/
	virtual bool			IsExist(DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd);

	/** 某种BUFF是否存在
	@param   
	@param   
	@return  
	*/
	virtual bool			IsExist(DWORD dwBuffType);

	/** 清除所有BUFF
	@param   
	@param   
	@return  
	*/
	virtual bool			Clean(void);

	/** 右键取消某个buff
	@param   
	@param   
	@return  
	*/
	virtual bool			RightClickRemoveBuff(DWORD dwBuffIndex);

	/** 移除对方添加的某种类型的Buff
	@param   dwBuffType ：Buff的类型，填0表示移除所有类型
	@param   uidAdd		：添加者，填0表示移除所有生物添加的
	@param   uidRemove	：移除者
	@return  
	*/
	virtual void			CleanBuff(DWORD dwBuffType, UID uidAdd, UID uidRemove);

	/** 取玩家某个BUFF的等级(如果有多个相同ID的BUFF取等级最高的)
	@param   dwBuffID：buff的ID
	@param   uidAdd ：如果不管添加者，则填INVALID_UID   
	@return  
	*/
	virtual DWORD			GetBuffLevel(DWORD dwBuffID, UID uidAdd);

	//////////////////////////ITimeAxis///////////////////////////////
	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	virtual void			OnTimer(unsigned long dwTimerID);

	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
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

	///////////////////CBuffPart//////////////////////////////////////
	/** 
	@param   
	@param   
	@return  
	*/
	CBuff *					GetBuff(DWORD dwBuffID, UID uidAdd);

	/** 
	@param   pOutBuff ：输出buff 
	@param   nQty ：数量
	@return  
	*/
	void					GetBuff(DWORD dwBuffID, CBuff ** ppOutBuff, int &nQty);

	/** 创建初始化的BUFF，总是在创建部件时，创建BUFF，但此时很多部件没有创建，很容易非法
	@param   
	@param   
	@return  
	*/
	bool					BuildInitBuff();

	/** 移除某个BUFF
	@param   
	@param   
	@return  
	*/
	bool					Remove(CBuff * pInBuff, UID uidRemove, bool bCreateEndArose);

	/** 
	@param   
	@param   
	@return  
	*/
	void					OnStartBuff(CBuff * pBuff);

	/** 取不能添加BUFF的原因（CanAdd后，采用这个函数可以取到具体原因）
	@param   
	@param   
	@return  
	*/
	virtual int				GetErrorID();

	/** 剩余时间
	@param   dwBuffID：buff的ID
	@param   uidAdd ：如果不管添加者，则填INVALID_UID  
	@return －1：表示时间已越过，不用通知客户端创建。0：表示永久时间。正数表示剩余时间 
	*/
	virtual int				GetLeftTime(DWORD dwBuffID, UID uidAdd);

	/** 某个effectID是否存在
	@param   dwEffectID: 指定的效果ID
	@param   lIgnoreUid：忽略指定添加者
	@return  
	*/
	virtual bool			IsEffectIDExist(DWORD dwEffectID, UID lIgnoreUid);

	/** 设置累积BUFF
	@param  dwBuffID:buffID
	@param  uidAdd ：如果不管添加者，则填INVALID_UID
	@param  bOverlay:true递增BUFF等级，直到buff等级上限，false递减buff等级，直到0
	*/
	virtual void			SetAccumulateBuff(DWORD dwBuffID, UID uidAdd, bool bOverlay, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0);

	/** 索引移除buff
	@param   dwBuffIndex  Buff索引
	@param   dwTime		  延迟删除时间
	*/
	virtual bool            RemoveBuffByIndex(DWORD dwIndex);

    

    /** 取得类型buff列表
    @param   dwBuffType ：buff类型
	@param   pBuffID ：输出buffID列表
    @param   pBuffLevel ：输出buffLevel列表
    @param   pBuffTime ：输出buffTime列表
	@param   nOutCount ：数量
	@return  
	*/
	void					GetBuff(DWORD dwBuffType, DWORD *pBuffID, DWORD *pBuffLevel, int *pBuffTime, int &nOutCount);

	/** 
	@param   
	@param   
	@return  
	*/
	CBuffPart(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CBuffPart(void);

private:
	// 服务器消息增加buff
	void OnServerMessageAddBuff(void *pData, size_t nLen);

	// 服务器消息移除buff
	void OnServerMessageRemoveBuff(void *pData, size_t nLen);

    // 客户端消息移除buff
    void OnClientMessageRemoveBuff(void *pData, size_t nLen);

	/** 通过索引移除buff
	@param   dwBuffIndex Buff索引
	@return  
	*/
	bool Remove(DWORD dwBuffIndex);

    // 重算Flag
    void recountFlag();

    /** 通过索引获取buff
	@param   dwBuffIndex Buff索引
	@return  
	*/
	CBuff *getBuff(DWORD dwBuffIndex);

private:
	// 所属实体
	__IEntity *				m_pMaster;

	// 当前buff
	MAP_BUFF				m_BuffMap;

	// BUFF index
	DWORD					m_dwBuffIndex;

	// 是否需要备份buff
	bool					m_bBackInitBuff;

	// 在创建过程中加的buff
	TLIST_INITBUFF			m_listInitBuff;

	// 是否正在清除BUFF过程中
	bool					m_bCleaningFlag;

	// 是否启动了清除无效BUFF定时器
	bool					m_bSetClanInvalidTimer;

	// 不能添加BUFF的原因ID
	int						m_nErrorID;

	// 保存序列化时Buff表,进入视野序列化，不然有些BUFF光效不播放问题
	typedef list<SBuffMsgAddBuff_SC>		SERIALIZEBUFF;
	SERIALIZEBUFF				m_SerializeBuff;
	// 保存要打包带走的Buff表
	SERIALIZEBUFF				m_deSerializeBuff;

	// 延迟删除BUFF表
	typedef map<DWORD, DWORD>		DELAY_REMOVE_BUFF_MAP;
	DELAY_REMOVE_BUFF_MAP	m_DelayRemoveBuffMap;

	// 启用延迟删除buff定时器
	bool					m_bDelayRemoveBuffTimer;

    // 所有BUFF的FLAG标识
    DWORD                   m_dwBitFlag;

	// 可加速查询的buff掩码
	BitMask                 m_AccQueryMask;

	// 是否序列化buff
	bool					m_bDeSerializeBuff;
};
