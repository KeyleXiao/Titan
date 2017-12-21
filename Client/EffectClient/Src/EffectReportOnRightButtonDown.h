/*******************************************************************
** 文件名:	EffectReportOnRightButtonDown.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	5/02/2017
** 版  本:	1.0
** 描  述:	右键按下时上报
********************************************************************/

#pragma once

#include "IEffect.h"
#include "EffectDef.h"
#include "GameViewDef.h"
#include "IClientGlobal.h"
#include "IRenderView.h"
#include "IEntityClient.h"
#include "Entity_inl.h"
using namespace EFFECT_CLIENT;

class CEffectReportOnRightButtonDown : public IEffect, public IGameViewEventHandler
{
public:
	typedef  EffectClient_ReportOnRightButtonDown    SCHEME_DATA;

	CEffectReportOnRightButtonDown( SCHEME_DATA & data ) : m_data(data),m_pEntity(NULL)
	{
	}

	// 效果启用
	virtual bool			Start( EFFECT_CONTEXT * context )
	{
        if(context == NULL || context->pEntity == NULL)
            return false;

        if(!context->pEntity->isHero())
            return false;

        m_pEntity = context->pEntity;

		// 通知显示层鼠标右键按下时上报逻辑层
        // GVIEWCMD_REPORT_ON_RIGHTBUTTON_DOWN
        gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_REPORT_ON_RIGHTBUTTON_DOWN, 1, 0, 0, 0);

        // 注册鼠标右键按下事件
        gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_REPORT_ON_RIGHTBUTTON_DOWN, this);

		return true;
	}

    virtual bool onViewEvent( int eventID,int nParam,const char * strParam,void * ptrParam )
    {
        switch(eventID)
        {
        case GVIEWCMD_REPORT_ON_RIGHTBUTTON_DOWN:
            {
                // 发送鼠标右键按下消息到服务器
                if(m_pEntity != NULL)
                {
                    SendMessageToEntityScene(m_pEntity->getUID(), PART_COMMON, CS_MSG_RIGHTBUTTON_DOWN, NULL, 0);
                    return true;
                }
            }
            break;
        }

        return false;
    }

	// 效果停止
	virtual void			Stop()
	{
        if(m_pEntity == NULL)
            return;

        // 通知显示层鼠标右键按下时不上报逻辑层
        gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_REPORT_ON_RIGHTBUTTON_DOWN, 0, 0, 0, 0);

        // 反注册鼠标右键按下事件
        gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_REPORT_ON_RIGHTBUTTON_DOWN);

        m_pEntity = NULL;
	}

	// 克隆一个新效果
	virtual IEffect *       Clone() 
	{
		return new CEffectReportOnRightButtonDown(m_data);
	}

	// 释放
	virtual void			Release()
	{
		Stop();
		delete this;
	}

	// 取得效果ID
	virtual int				GetEffectID()
	{
		return m_data.nID;
	}

private:
	SCHEME_DATA               m_data;

    IEntity	                  *m_pEntity;
};
