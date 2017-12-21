/*******************************************************************
** 文件名:	BuffPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/16  11:01
** 版  本:	1.0
** 描  述:	BUFF部件
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IBuffClient.h"
#include <list>
using namespace std;
#include "IBuffClient.h"
#include "BuffMessageDef.h"
#include "BitMask.h"

class CBuffPart : public IBuffPart,public IEventExecuteSink
{
    // 输入间隔时间
    #define INPUT_SPACE_TIME    1000
public:
	/////////////////////////////////////////ISerializable/////////////////////////////////////////
	virtual bool onSerialize(UID uid, rkt::ibuffer &in, int flag);

	virtual bool deSerialize(rkt::obuf &out, int flag);

	/////////////////////////////////////////IEntityPart/////////////////////////////////////////
	virtual short  getPartID();

	virtual bool onLoad(IEntity* pEntity);

	// 消息通讯
	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);

	// 显示层的命令(true表示此部件处理,其他不处理,false交由其他部件继续处理)
	virtual bool onCommand(int nCmdID, void *pData, size_t nLen);

	virtual IEntity * getHost();

	virtual void release();

	/////////////////////////////////////////IBuffPart/////////////////////////////////////////
	/** 通过buff index取得buff
	@param   
	@param   
	@return  
	*/
	virtual IBuff *GetBuff(DWORD dwBuffIndex);

	/** 某个BUFF是否存在
	@param   
	@param  
	@return  
	*/
	virtual bool IsExist(DWORD dwBuffID);

	/** 获得所有BUFF的index
	@param   pBuffIndex ：buff index的数组
	@param   nCount ：IN = 数组大小，OUT = buff数
	@return  成功：true, 失败：false，表示数组过小
	*/
	virtual bool GetAllBuff(DWORD * pBuffIndex, int &nCount);

	/** 右键取消某个buff
	@param   
	@param   
	@return  
	*/
	virtual bool RightClickRemoveBuff(int nBuffIndex);

	/** 通过buff id取得buff
	@param   
	@param   
	@return  
	*/
	virtual IBuff *GetBuffByID(DWORD dwBuffID);

	/** 某个effectID是否存在
	@param   
	@param  
	@return  
	*/
	virtual bool IsEffectIDExist(int nEffectID);

	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	// 订阅属性改变事件，然后同步到显示层
	virtual void OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	/////////////////////////////////////////CBuffPart/////////////////////////////////////////
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

	/** 
	@param   
	@param   
	@return  
	*/
	bool						AddBuff(IBuff * pBuff);

	/** 
	@param   
	@param   
	@return  
	*/
	bool						RemoveBuff(IBuff * pBuff);

private:
	// 注册事件
	void subscibeEvents(void);

	// 注销事件
	void unSubscibeEvents(void);

	// 服务器消息增加buff
	void OnServerMessageAddBuff(void *pData, int nLen, bool bAppear = false);

	// 服务器消息移除buff
	void OnServerMessageRemoveBuff(void *pData, int nLen);

	// 服务器消息改变充能层数
	void OnServerMessageChangeChargeCount(void *pData, int nLen);

    // 复活事件处理
    void onEventReliev();
    // 死亡事件处理
    void onEventDie();

	// 是否可以增加该Buff
	bool canAddBuff(SBuffMsgAddBuff_SC* pBuffData);

private:
	// 主人
	IEntity *					m_pMaster;

	typedef list<IBuff*>		TLIST_BUFF;
	// BUFF列表
	TLIST_BUFF					m_listBuff;

	// 保存序列化时Buff表,进入视野序列化，不然有些BUFF光效不播放问题
	typedef list<SBuffMsgAddBuff_SC>		SERIALIZEBUFF;
	SERIALIZEBUFF				m_SerializeBuff;

    // 上次输入时间
    DWORD                       m_dwLastInputTime;

	// 快速查询掩码
	BitMask					    m_AccQueryMask;
};