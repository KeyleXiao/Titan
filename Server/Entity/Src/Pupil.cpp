/*******************************************************************
** 文件名:	Pupil.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	11/8/2016
** 版  本:	1.0
** 描  述:	防沉迷-未成年

********************************************************************/

#include "stdafx.h"
#include "Pupil.h"
#include "IServerGlobal.h"
#include "EntityHelper.h"
#include "IServiceContainer.h"
#include "ITimerService.h"
#include "ILoginServerProtocol.h"
#include "ICenterConnectorService.h"


#define	DINTERVAL_KICKOUT			10					// 在10秒后被踢对象将被送出场景服

atomic<hprecord*> hprecord::head(NULL);
atomic<hazard*> hazard::s_p(NULL);


CPupil::CPupil()
    : m_nState(emAddictState_Health)
    , m_nOneHourTips(0)
    , m_dwIdCard(0)
{

}

CPupil::~CPupil()
{
    m_mapActorList.clear();
    m_mapKickList.clear();
}


/** 创建
*/
bool CPupil::create(SUserAddictInfo* pAddictInfo)
{
    if (pAddictInfo != NULL)
    {
        memcpy(&m_AddictInfo, pAddictInfo, sizeof(m_AddictInfo));
        m_dwIdCard = m_AddictInfo.dwIdCard;
    }

    ITimerService * pTimerService = gServerGlobal->getTimerService();
    if(pTimerService)
    {
        // 启动定时器（5分钟）
        pTimerService->SetTimer(TIP_ACTOR_TIMER, ADDICT_FIVE_MIN, this, INFINITY_CALL, __FUNCTION__);
        pTimerService->SetTimer(KICK_ACTOR_TIMER, ADDICT_KICK_TIME, this, INFINITY_CALL, __FUNCTION__);

        OnTimer(TIP_ACTOR_TIMER);
    }
    return true;
}

/** 释放
*/
void CPupil::release(void)
{
    // 关闭定时器
    ITimerService * pTimerService = gServerGlobal->getTimerService();
    if(pTimerService)
    {
        pTimerService->KillTimer(TIP_ACTOR_TIMER, this);
        pTimerService->KillTimer(KICK_ACTOR_TIMER, this);
    }

    delete this;
}


/** 加入角色
@ return
*/
bool CPupil::addActor(PDBID dwPDBID, UID uid)
{
	UID uid_temp = INVALID_UID;
	if( m_mapActorList.find(dwPDBID, uid_temp) == false)
	{
		m_mapActorList.insert(dwPDBID, uid);
	}

	return true;
}

/** 移除角色
@ return
*/
bool CPupil::removeActor(PDBID dwPDBID, UID uid)
{
    // 这里不用检测，强制删除内存数据
    m_mapActorList.erase(dwPDBID);

    return true;
}

/** 没有角色数据
@ return
*/
bool CPupil::empty()
{
    return (m_mapActorList.size()==0);
}

/** 加入被踢角色
@ return
*/
void  CPupil::addKickActor(PDBID pdbid)
{
	DWORD dwTime = 0;
	if( !m_mapKickList.find(pdbid, dwTime) )
	{
		m_mapKickList.insert(pdbid, ((DWORD)time(NULL)) );
	}
}

/** 移除被踢角色
@ return
*/
void  CPupil::removeKickActor(PDBID pdbid)
{
    m_mapKickList.erase(pdbid);
}

/** 更新在线账号下的角色为成年人状态
@ return
*/
void  CPupil::setAdult()
{
    for (MAP_ACTOR::iterator iter = m_mapActorList.begin(); iter != m_mapActorList.end(); ++iter)
    {
        UID & uid = iter->second;
        ActorHelper helper(uid, ACTOR_UID);
        IActorService * pActorService = helper.m_ptr;

        if( pActorService == nullptr)
        {
            continue;
        }

        // 重设在线UID玩家的bIsAdult和IdCard信息，用于解除玩家沉迷信息
        pActorService->setAdult();
    }

    // 清空踢人列表对象
    m_mapKickList.clear();
}

/** 更新在线账号下的角色实名认证状态
@ return
*/
void  CPupil::setRealNameAuther()
{
	for (MAP_ACTOR::iterator iter = m_mapActorList.begin(); iter != m_mapActorList.end(); ++iter)
	{
		UID & uid = iter->second;
		ActorHelper helper(uid, ACTOR_UID);
		IActorService * pActorService = helper.m_ptr;

		if( pActorService == nullptr)
		{
			continue;
		}

		// 重设在线UID玩家的实名认证状态标识
		pActorService->setRealNameAutherFlag();

		ClientID clientID = pActorService->getClientID();
		// 发送到登陆服上重设认证信息
		notifyLoginServerForPassport(clientID);
	}
}

/** 更新在线账号下的角色身份证号
@ return
*/
void  CPupil::updateIdCard(DWORD dwIdCard)
{
	for (MAP_ACTOR::iterator iter = m_mapActorList.begin(); iter != m_mapActorList.end(); ++iter)
	{
		UID & uid = iter->second;
		ActorHelper helper(uid, ACTOR_UID);
		IActorService * pActorService = helper.m_ptr;
		if (pActorService == nullptr)
		{
			continue;
		}

		pActorService->setIdCard(dwIdCard);
	}

	m_AddictInfo.dwIdCard = dwIdCard;
	m_dwIdCard = dwIdCard;
}

/** 取得玩家的衰减比例
@ return : 返回衰减比例（x/100）,即返回，表示衰减一半
*/
int CPupil::getReduceRate(void)
{
    // 不在线？不可能吧
    if (m_AddictInfo.nOnlineCount == 0)
    {
        return 100;
    }

    int nCurTime = (int)time(NULL);
    int nOnlineTime = nCurTime - m_AddictInfo.nLoginTime + m_AddictInfo.nPreOnlineTime;
    if (nOnlineTime <= ADDICT_TIRE_TIME)
    {
        return 100;
    }

    if (nOnlineTime <= ADDICT_ILL_TIME)
    {
        return 50;
    }

    return 0;
}

/** 更新
*/
void CPupil::update(SUserAddictInfo* pAddictInfo)
{
    if (pAddictInfo != NULL)
    {
        memcpy(&m_AddictInfo, pAddictInfo, sizeof(m_AddictInfo));
        m_dwIdCard = m_AddictInfo.dwIdCard;

#ifdef SHOW_ANTI_ADDICT_LOG
		time_t dwLoginTime = m_AddictInfo.nLoginTime;
		tm *pLoginTime = localtime(&dwLoginTime);
		char tLoginTime[56] = {0};													// 登陆时间
		sprintf_s(tLoginTime, sizeof(tLoginTime), "%d-%.2d-%.2d %.2d:%.2d:%.2d", 
			1900+pLoginTime->tm_year,
			pLoginTime->tm_mon+1,
			pLoginTime->tm_mday,
			pLoginTime->tm_hour,
			pLoginTime->tm_min,
			pLoginTime->tm_sec);

		time_t dwLogoutTime = m_AddictInfo.nLogoutTime;
		tm *pLogoutTime = localtime(&dwLogoutTime);
		char tLogoutTime[56] = {0};													// 登出时间
		sprintf_s(tLogoutTime, sizeof(tLogoutTime), "%d-%.2d-%.2d %.2d:%.2d:%.2d", 
			1900+pLogoutTime->tm_year,
			pLogoutTime->tm_mon+1,
			pLogoutTime->tm_mday,
			pLogoutTime->tm_hour,
			pLogoutTime->tm_min,
			pLogoutTime->tm_sec);

		char tPreOnlineTime[56] = {0};												// 之前在线时间
		sprintf_s(tPreOnlineTime, sizeof(tPreOnlineTime), "%.2dHour %.2dMin %.2dSec", 
			m_AddictInfo.nPreOnlineTime / 3600,
			(m_AddictInfo.nPreOnlineTime % 3600) / 60 ,
			m_AddictInfo.nPreOnlineTime % 60);

		char tPreOfflineTime[56] = {0};												// 之前离线时间
		sprintf_s(tPreOfflineTime, sizeof(tPreOfflineTime), "%.2dHour %.2dMin %.2dSec", 
			m_AddictInfo.nPreOffLineTime / 3600,
			(m_AddictInfo.nPreOffLineTime % 3600) / 60 ,
			m_AddictInfo.nPreOffLineTime % 60);

		TraceLn("XXXXXAddict, ActorLogin LoginTime = " << tLoginTime << ", LogoutTime = " << tLogoutTime << ",  PreOnlineTime = " << tPreOnlineTime << ", PreOfflineTime = " << tPreOfflineTime << ", ActorID = " << pAddictInfo->nPDBID << ", nCardID = " << pAddictInfo->dwIdCard);                     
#endif

		// 更新沉迷数据，做一次状态判定
		OnTimer(TIP_ACTOR_TIMER);
    }
}

/** 获取IdCard
*/
DWORD CPupil::getIdCard()
{
    return m_dwIdCard;
}

// 对当前有关的帐号进行广播消息
void CPupil::broadcastUsers(const char* szMessage)
{
    if(szMessage==NULL || strlen(szMessage) <= 0)
    {
        return;
    }

	for (MAP_ACTOR::iterator iter = m_mapActorList.begin(); iter != m_mapActorList.end(); ++iter)
    {
        // 发送客户端提示玩家
		obuf ob;

		SNetMsgHead head;
		head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
		head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
		head.byKeyModule    = MSG_MODULEID_ANTIADDICT;
		head.byKeyAction    = MSG_ADDICT_TIP_S2C;
		ob << head;

		SMsgAddictTip_S2C data;
		sstrcpyn(data.szRetDes, szMessage, sizeof(data.szRetDes));
		ob.push_back(&data, sizeof(data));

		ActorHelper helper(iter->first, ACTOR_DBID);
		if ( helper.m_ptr ) 
		{
			helper.m_ptr->send2Client( PACKAGE_PTR(new std::string(ob.data(), ob.size()) ) );
		}
    }
}


// 将通行证认证信息通知登陆服
void CPupil::notifyLoginServerForPassport(DWORD clientID)
{
    ICenterConnectorService * pCenterConnection = gServerGlobal->getCenterConnectorService();
    if (pCenterConnection)
    {
        SMsgLoginServer_RemoveAntiaddict msg;
        msg.dwClientID = clientID;

        pCenterConnection->sendToLoginServer(MSG_MODULEID_LOGIN, MSG_LOGINSERVER_REMOVEANTIADDICT, (const char*)&msg, sizeof(msg));
    }
}

////////////////////////TimerHandler//////////////////////////////////////////////////
/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void CPupil::OnTimer( unsigned long dwTimerID )
{
    switch(dwTimerID)
    {
    case KICK_ACTOR_TIMER:      // 踢人定时器
        {
            //bool bKickout = (pSchemeCenter->GetGameBaseConfig(EGameConfig_AntiAddicKickout) != 0);
            IGatewayAcceptorService * pGatewayAcceptorService = gServerGlobal->getGatewayAcceptorService();
            if(pGatewayAcceptorService == nullptr)
                return;

			vector <PDBID> vecRemove;
            DWORD dwTime = (DWORD)time(NULL);
            for (MAP_KICK::iterator iter=m_mapKickList.begin(); iter!=m_mapKickList.end(); ++iter)
            {
                if (dwTime >= iter->second + DINTERVAL_KICKOUT)
                {
                    ActorHelper helper(iter->first, ACTOR_DBID);
                    IActorService *pActorService = helper.m_ptr;
                    if(pActorService)
                    {
                        pGatewayAcceptorService->kickOutClient(pActorService->getClientID(), EKickoutClient_Reason_KickoutSelf);
                        // pTunnel->Kickout(EKickoutClient_Reason_KickoutSelf, "您当前处于未成年状态下，每一个身份证同时只允许一个角色登陆到游戏中");
                    }

                    vecRemove.push_back( iter->first );
                }
            }
			// 移除队列中的对象
			for (vector <PDBID>::iterator it = vecRemove.begin(); it != vecRemove.end(); ++it)
			{
				m_mapKickList.erase( *it );
			}
        }
        break;

    case TIP_ACTOR_TIMER:       // 提醒定时器
        {            
			int nCurTime = (int)time(NULL);
            int nOnlineTime = nCurTime - m_AddictInfo.nLoginTime + m_AddictInfo.nPreOnlineTime;

            char szHtml[512] = {0};
            if (m_nState == emAddictState_Health)
            {
                // 满5小时
                if (nOnlineTime >= ADDICT_ILL_TIME)
                {
                    m_nState = emAddictState_Ill;
                    sprintf_s(szHtml, sizeof(szHtml), "您已进入不健康游戏时间，为了您的健康，请您立即下线休息。如不下线，您的身体将受到损害，您的收益已降为零，直到您的累计下线时间满5小时后，才能恢复正常。");
                }
                // 满3小时
                else if (nOnlineTime >= ADDICT_TIRE_TIME)
                {
                    m_nState = emAddictState_Tire;
                    sprintf_s(szHtml, sizeof(szHtml), "您累计在线时间已满3小时，已经进入疲劳游戏时间，您的游戏收益将降为正常值的50％，为了您的健康，请尽快下线休息，做适当身体活动，合理安排学习生活。");
                }
                // 满2小时
                else if (nOnlineTime >= ADDICT_TWOHOUR)
                {
                    if (m_nOneHourTips < 2)
                    {
                        sprintf_s(szHtml, sizeof(szHtml), "您累计在线时间已满2小时。");
                        m_nOneHourTips = 2;
                    }
                }
                // 满1小时
                else if (nOnlineTime >= ADDICT_ONEHOUR)
                {
                    if (m_nOneHourTips < 1)
                    {
                        sprintf_s(szHtml, sizeof(szHtml), "您累计在线时间已满1小时。");
                        m_nOneHourTips = 1;
                    }
                }
            }
            else if (m_nState == emAddictState_Tire)
            {
                static int nTireCount = 0;
                if (nOnlineTime >= ADDICT_ILL_TIME)
                {
                    m_nState = emAddictState_Ill;
                    sprintf_s(szHtml, sizeof(szHtml), "您已进入不健康游戏时间，为了您的健康，请您立即下线休息。如不下线，您的身体将受到损害，您的收益已降为零，直到您的累计下线时间满5小时后，才能恢复正常。");
                }
                else
                {
                    nTireCount ++;
                    // 每30分钟提示一次
                    if (nTireCount % 6 == 0)
                    {
                        sprintf_s(szHtml, sizeof(szHtml), "已经进入疲劳游戏时间，您的游戏收益将降为正常值的50％，为了您的健康，请尽快下线休息，做适当身体活动，合理安排学习生活。");
                    }
                }
            }
            else if (m_nState == emAddictState_Ill)
            {
                static int nIllCount = 0;
                nIllCount ++;

                // 每15分钟提示一次
                if (nIllCount % 3 == 0)
                {
                    sprintf_s(szHtml, sizeof(szHtml), "您已进入不健康游戏时间，为了您的健康，请您立即下线休息。如不下线，您的身体将受到损害，您的收益已降为零，直到您的累计下线时间满5小时后，才能恢复正常。");
                }
            }

			if (szHtml[0] != 0)
			{
				broadcastUsers(szHtml);
			}
        }
        break;

    default:
        break;
    }
}


