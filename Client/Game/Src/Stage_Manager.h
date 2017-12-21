/*******************************************************************
** 文件名:	Stage_Manager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/24/2014
** 版  本:	1.0
** 描  述:	

********************************************************************/


/**
	游戏状态 - 用来处理当前处在游戏的不同阶段，而需要处理的不同逻辑
	常用的阶段:
	1.Stage_Login			: 登陆阶段,处理登陆逻辑
	2.Stage_Choose			: 登陆角色/职业阶段
	3.Stage_MatchRoom       : 房间阶段(状态： 1、等待 2、选人 3、准备完成 4、战斗)
	4.Stage_MatchBattle		: 战场游戏进行阶段

*/

#pragma once


#include "Stage_Login.h"
#include "Stage_Choose.h"
#include "Stage_Game.h"
#include "Stage_LoadScene.h"
#include "Stage_MatchBattle.h"
#include "Stage_MatchRoom.h"
#include "IClientGlobal.h"
#include "net/net.h"
#include "Stage_ManagedDef.h"

using namespace Stage_ManagerDef;

class Stage_Manager 
{
	Stage_Login             m_LoginStage;           // 登陆状态
    Stage_LoadScene         m_LoadSceneStage;       // 加载地图状态
	Stage_Choose            m_ChooseStage;          // 选择状态
	Stage_Game		        m_GameStage;            // 游戏状态
	Stage_MatchRoom			m_MatchRoom;            // 比赛大厅中房间的状态
    
    Stage_MatchBattle		m_MatchBattleStage;     // 战斗状态

    Stage_Base *            m_pStates[ESS_MAX];     // 状态数组

    EStage_State            m_nPrevState;           // 前一个状态
    EStage_State            m_nCurState;            // 当前状态

    inline bool checkState( EStage_State nState )
    {
        return ( nState>ESS_NONE && nState<ESS_MAX ) && m_pStates[nState]!=0;
    }

public:
    Stage_Manager()
        : m_nCurState( ESS_NONE )
    {
        memset(m_pStates, 0, sizeof(m_pStates));

        m_pStates[ESS_LOGIN] = &m_LoginStage;
        m_pStates[ESS_CHOOSE] = &m_ChooseStage;
        m_pStates[ESS_GAME] = &m_GameStage;
        m_pStates[ESS_MATCHROOM] = &m_MatchRoom;
        m_pStates[ESS_BATTLE] = &m_MatchBattleStage;
        m_pStates[ESS_LOADSCENE] = &m_LoadSceneStage;
    }

    void release()
    {
        delete this;
    }

    Stage_Game& getGame()
    {
        return m_GameStage;
    }

	Stage_MatchRoom& getMatchRoom()
	{
		return m_MatchRoom;
	}

	Stage_Login& getLogin()
	{
		return m_LoginStage;
	}

    inline Stage_Base * getState( EStage_State nState )
    {
        return checkState(nState)? m_pStates[nState] : NULL;
    }

	// 获取当前状态ID
	inline EStage_State getCurState()
	{
		return m_nCurState;
	}

    inline EStage_State getPrevState()
    {
        return m_nPrevState;
    }

	// 切换到某一个状态 执行前一个状态离开（onLeave） 新状态进入(onEnter)
    bool gotoState( EStage_State nState,void * context=NULL,int len=0 )
    {
        if ( !checkState(nState) )
            return false;

        if ( m_nCurState==nState )
            return true;

        if ( m_pStates[m_nCurState]!=0 )
        {
            m_pStates[m_nCurState]->onLeave();
        }

		gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_GAME_STAGE_STATE,nState,0,0,0 );

        m_pStates[nState]->onEnter( context,len );
        m_nPrevState = m_nCurState;
        m_nCurState = nState;

        net_setHandler( m_pStates[nState] );

        
        return true;
    }

    // 登陆服上的状态切换本地状态
    Stage_ManagerDef::EStage_State loginServerStateSwitchLocal(unsigned long nLoginServerState)
    {
        switch(nLoginServerState)
        {
        case rkt::CLIENT_CONN_STATE_LOGIN:
            return Stage_ManagerDef::ESS_LOGIN; // 登陆状态(等待登陆,包括普通登陆和特殊方式的登陆,例如动态密码,矩阵卡等可能的登陆方式)
        case rkt::CLIENT_CONN_STATE_ACTOR:
            return Stage_ManagerDef::ESS_CHOOSE; // 选择静态角色状态
        case rkt::CLIENT_CONN_STATE_QUEUE:
            return Stage_ManagerDef::ESS_QUEUE; // 排队状态(等待进入静态地图)
        case rkt::CLIENT_CONN_STATE_GAME:
            return Stage_ManagerDef::ESS_GAME; // 游戏状态(正在游戏中)
        default:
            return Stage_ManagerDef::ESS_NONE;
        }
    }
};

 