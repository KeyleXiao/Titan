/*******************************************************************
** 文件名:	Buff.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/15  22:46
** 版  本:	1.0
** 描  述:	BUFF
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

#include "IEffect.h"
#include "BuffPart.h"
// 定义
class CBuff : public IBuff,
			  public TimerHandler 
{
	typedef list<IEffect *> TLIST_EFFECT;
	
	enum
	{
		ETimerEventID_Life = 0,	// buff生命周期
	};
public:
	////////////////////////IBuff////////////////////////////////////
	/** 
	@param   
	@param   
	@return  
	*/
	bool					Create(CBuffPart * pBuffPart, DWORD dwIndex, DWORD dwBuffID, DWORD dwBuffLevel, DWORD dwTime, EFFECT_CONTEXT *pContext=NULL);

	/** 释放所有资源，并且销毁此对像
	@param   
	@param   
	@return    	
	*/
	virtual void			Release(void);

	/** 启动效果
	@param   
	@param   
	@return  
	*/
	virtual bool			Start(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual bool			Stop(void);

	/** buff id
	@param   
	@param   
	@return  
	*/
	virtual DWORD			GetBuffID(void);

	/** 等级
	@param   
	@param   
	@return  
	*/
	virtual DWORD			GetLevel(void);

    /** 标志
    @param   
    @param   
    @return  
    */
   virtual DWORD            GetFlag(void);

	/** 剩余时间，外面要自已处理时间倒数,如果返回0,表示永久状态
	@param   
	@param   
	@return  
	*/
	virtual DWORD			GetLeftTime(void);

	/** 图标ID
	@param   
	@param   
	@return  
	*/
	virtual int				GetIconID(void);

	/** buff名字
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR			GetBuffName(void);

	/** buff描述
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR			GetDesc(void);

	/** 取得总时间
	@param   
	@param   
	@return  
	*/
	virtual int				GetTotalTime(void);
	
	/** 取得效果数量
	@param   
	@param   
	@return  
	*/
	virtual int				GetEffectCount(void);

	/** 取得效果ID
	@param   
	@param   
	@return  
	*/
	virtual int				GetEffectIDByIndex(int index);

	/** 判断某个效果是否存在
	@param   
	@param   
	@return  
	*/
	virtual bool			IsEffectIDExist(int nEffectID);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual DWORD			GetFlashID(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual EFFECT_CONTEXT	*GetEffectContext(void);

	//////////////////////TimerHandler///////////////////////////////
	/**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	virtual void			OnTimer(unsigned long dwTimerID);

	////////////////////////CBuff////////////////////////////////////
	

	/** 
	@param   
	@param   
	@return  
	*/
	DWORD					GetIndex(void);

	
	/** 
	@param   
	@param   
	@return  
	*/
	CBuff(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CBuff(void);

private:
	// buff index
	DWORD					m_dwBuffIndex;

	// buff id
	DWORD					m_dwBuffID;

	// 等级
	DWORD					m_dwBuffLevel;

    // 标志
    DWORD					m_dwSchemeFlag;

	// 总时间
	DWORD					m_dwTime;

	// 启动tick
	DWORD					m_dwStartTick;	

	// 图标ID
	int						m_nIconID;	

	// buff名字 
	char					m_szName[64];

	// 描述
	char					m_szDesc[256];

	// 光效ID
	DWORD					m_dwFlashID;

	///////////////////////////////////////////
	// BUFF部件
	CBuffPart *				m_pBuffPart;

	// 所有效果
	TLIST_EFFECT			m_listEffect;

	// 效果数据
	EFFECT_CONTEXT			m_effectContext;
};