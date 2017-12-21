/*******************************************************************
** 文件名:	AOIPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - All Rights Reserved
** 创建人:	彭璐
** 日  期:	12/9/2014
** 版  本:	1.0
** 描  述:	管理实体视野和属性、状态更新

********************************************************************/

#include "IAOIPart.h"
#include "EntityViewDef.h"

#pragma once
class AOIPart : public IAOIPart, public TimerHandler
{
	enum
	{
		ETIMER_GUARD,			// 警戒TIMER
	};
public:
	AOIPart(void);
	virtual ~AOIPart(void);

	virtual short getPartID();

	virtual bool onLoad( IEntity * pEntity );

	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);

	virtual bool onCommand( int cmdid,void * data,size_t len );

	virtual IEntity * getHost();

	virtual void release();

	virtual bool onSerialize( UID uid, rkt::ibuffer& in,int flag );
	virtual bool deSerialize( rkt::obuf& out,int flag );

    // 获取视野半径
    virtual int getSight();

    // 是否在主角焦点范围内
    virtual bool isInMasterEyeFocus(float x, float y, float z);

    virtual void restore();

    ///////////TimerHandler///////////////////////////////////////////////////////////////
    /**
	@purpose          : 定时器触发后回调,你可以在这里编写处理代码
	@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
	@return		      : empty
	*/
	virtual void OnTimer( unsigned long dwTimerID );

private:
	void onTimerGuard(void);

private:
	IEntity*    m_pMaster;
    int         m_nSignRadii;               // 视力半径
    DWORD       m_dwUnGuardTime;            // 非警戒时间点
    bool        m_bGuard;                   // 是否在警戒中

    static float m_sGuardDistance;          // 警戒距离(米)
    static int   m_sGuardBufTime;           // 警戒缓冲时间(秒)

    // 只针对主角使用聚焦功能
    cmd_entity_master_eye_focus     m_eyeFocus;
};
