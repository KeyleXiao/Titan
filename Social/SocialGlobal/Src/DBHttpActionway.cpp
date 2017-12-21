/*******************************************************************
** 文件名:	DBHttpActionway.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/14/2017
** 版  本:	1.0
** 描  述:	HTTPSERVICE的Action

********************************************************************/

#include "stdafx.h"
#include "ISocialGlobal.h"
#include "ICenterConnectorService.h"
#include "ITimerService.h"
#include "IMailManager.h"
#include "IMailService.h"
#include "IShareServer.h"
#include "DBHttpActionway.h"


// 获取用户数组阀值
#define PERSON_ARRAY_SIZE       20480

///////////////////////////////////////DBHttpActionNotity//////////////////////////////////////////////////
// 构造函数
DBHttpActionNotity::DBHttpActionNotity() : m_dwIndex(0)
{
    m_PlayerList.clear();
}

// 析构函数
DBHttpActionNotity::~DBHttpActionNotity()
{
    ITimerService * pTimerService = gSocialGlobal->getTimerService();
    if (pTimerService) {
        pTimerService->KillTimer(ONLINE_USERS_READ_MAIL_TIMER, this);
    }
}



void
DBHttpActionNotity::OnExectue(
    DWORD pdbid,
    SMsgDBHttp_Request& msg,
    std::string& value,
    std::vector<std::string>& valueParams
 )
{
	if ( INVALID_PDBID == pdbid)
	{
		return;
	}

	if (value.size() > 7000)
	{
		WarningLn( _GT("DBHttpService:DB请求内容太长了(最大7000字节)!len=") << value.c_str());
		return ;
	}
    
	switch(msg.sub)
	{
	case DBHttpSubAction_Notity_KICKOUT:	//踢人
		{
			SMsgDBHttp_Kickout msg;
			msg.pdbid = pdbid;
			sstrcpyn(msg.szReason, "服务器繁忙，请稍候1分钟重新登录即可，如有疑问请与客服联系。", sizeof(msg.szReason) );

			sendDataToScene(MSG_MODULEID_DBHTTP, MSG_DBHTTP_KICKOUT, reinterpret_cast<LPCSTR>(&msg), sizeof(msg) );
		}
        break;

	case DBHttpSubAction_Notity_GETEMAIL:	//通知 获取邮件
		{
            playerReadMail(pdbid);
		}
        break;

	default: break;
	}
}

void DBHttpActionNotity::OnGodExectue(SMsgDBHttp_Request& msg, std::string& value,std::vector<std::string>& valueParams)
{
    switch (msg.sub)
    {
    case DBHttpSubAction_Notity_ALLGetMail:     // 通知所有玩家读取邮件
        {
            m_PlayerList.clear();
            m_dwIndex = 0;
            int nHttpRetValue = StringHelper::toInt(value);

            //SMsgDBHttp_NotityMultiPlayersGetEmail msg;
            //msg.nHttpRetValue = nHttpRetValue;
            //msg.nReserve = 0;           // 保留数据
            //sendDataToScene(MSG_MODULEID_DBHTTP, MSG_DBHTTP_NOTITYMULTIPLAYER_GETEMAIL, reinterpret_cast<const char*>(&msg), sizeof(msg));

            ITimerService * pTimerService = gSocialGlobal->getTimerService();
            if (nullptr == pTimerService) {
                break;
            }

            IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
            if (nullptr == pShareReceiver) {
                break;
            }

            SPersonInfo* pPersonArray = new SPersonInfo[PERSON_ARRAY_SIZE];
            if (pPersonArray) {
                DWORD dwPersonCount = pShareReceiver->GetALLPersonInfo(pPersonArray, PERSON_ARRAY_SIZE);

                for (DWORD i = 0; i < dwPersonCount; ++i) {
                    SPersonInfo & person = pPersonArray[i];
                    m_PlayerList.push_back(person.dwPlayerID);
                }

                // 判断玩家数
                if (m_PlayerList.size() > 0) {
                    pTimerService->KillTimer(ONLINE_USERS_READ_MAIL_TIMER, this);
                    // 保存在线处理定时器
                    pTimerService->SetTimer(ONLINE_USERS_READ_MAIL_TIMER, 5000, static_cast<ITimerHandler*>(this) );
                }

                SAFE_DELETEARRAY(pPersonArray);
            }
        }
        break;

    default: break;
    }
}

/**
@purpose          : 定时器触发后回调,你可以在这里编写处理代码
@param	 dwTimerID: 定时器ID,用于区分是哪个定时器
@return		      : empty
*/
void DBHttpActionNotity::OnTimer(unsigned long dwTimerID)
{
    switch (dwTimerID)
    {
    case ONLINE_USERS_READ_MAIL_TIMER:
        {
            DWORD dwSize = static_cast<DWORD>( m_PlayerList.size() );

            // 是不是到头了
            if (m_dwIndex >= dwSize) {
                ITimerService * pTimerService = gSocialGlobal->getTimerService();
                if (pTimerService) {
                    pTimerService->KillTimer(ONLINE_USERS_READ_MAIL_TIMER, this);
                }

                m_dwIndex = 0;
                m_PlayerList.clear();
                break;
            }

            for (DWORD i = 0; i<MAX_PLYARE_COUNT; i++, m_dwIndex++) {
                if (m_dwIndex >= dwSize) {
                    break;
                }

                DWORD & dwPDBID = m_PlayerList[m_dwIndex];
                playerReadMail(dwPDBID);
            }
        }
        break;

    default: break;
    }
}

// 发送数据到场景服
void DBHttpActionNotity::sendDataToScene(BYTE byKeyModule, BYTE byKeyAction, LPCSTR pData, size_t nLen)
{
    ICenterConnectorService* pCenterConnectorService = gSocialGlobal->getCenterConnectorService();
    if ( nullptr == pCenterConnectorService) {
        return;
    }

    SGameMsgHead gamemsghead;
    gamemsghead.SrcEndPoint = MSG_ENDPOINT_SOCIAL;
    gamemsghead.DestEndPoint= MSG_ENDPOINT_SCENE;
    gamemsghead.byKeyModule = byKeyModule;
    gamemsghead.byKeyAction = byKeyAction;

    obuf ob;
    ob << gamemsghead;
    ob.push_back(pData, nLen);
    pCenterConnectorService->sendDataToZoneSvr(ob.data(), ob.size() );
}

// 玩家读取邮件
void DBHttpActionNotity::playerReadMail(DWORD dwPDBID)
{
    //SMsgDBHttp_NotityGetEmail msg;
    //msg.pdbid = dwPDBID;

    //obuf1024 buf;
    //buf.push_back(&msg, sizeof(msg));
    //sendDataToScene(MSG_MODULEID_DBHTTP, MSG_DBHTTP_NOTITYGETEMAIL, buf.data(), buf.size());

    SERVICE_PTR pService = gSocialGlobal->getMailManager()->GetMailService();
    if (pService) {
        IMailService* pMailService = reinterpret_cast<IMailService*>(pService->query_interface());
        if (pMailService) {
            pMailService->readPlayerMailList(dwPDBID);
        }
    }
}

///////////////////////////////////////ActionwaySystemMessage//////////////////////////////////////////////////
/*
@brief：系统消息
*/
void ActionwaySystemMessage::OnExectue(DWORD pdbid, SMsgDBHttp_Request& msg, std::string& value, std::vector<std::string>& valueParams)
{
}

void ActionwaySystemMessage::OnGodExectue(SMsgDBHttp_Request& msg, std::string& value, std::vector<std::string>& valueParams)
{
    switch (msg.sub)
    {
    case DBHttpSubAction_Notity_GETUSERLIST:    // 取得玩家角色ID列表
        {
            IShareReceiver * pShareReceiver = gSocialGlobal->getShareReceiver();
            if (nullptr == pShareReceiver) {
                break;
            }

            SPersonInfo* pPersonArray = new SPersonInfo[PERSON_ARRAY_SIZE];
            if (pPersonArray) {
                DWORD dwPersonCount = pShareReceiver->GetALLPersonInfo(pPersonArray, PERSON_ARRAY_SIZE);

                obuf8192 buf;
                //DWORD dwNodeSize = sizeof(SPersonInfo::dwPlayerID);
                for (DWORD i = 0; i < dwPersonCount; ++i) {
                    SPersonInfo & person = pPersonArray[i];
                    buf.push_back(&person.dwPlayerID, sizeof(person.dwPlayerID));
                }

                //通过中心服发送个DBHttpServer
                if (buf.size() > 0) {
                    sendDataToHttpSrv(MSG_MODULEID_DBHTTP, MSG_SOCIAL2DBHTTP_USERLIST, buf.data(), buf.size());
                }

                SAFE_DELETEARRAY(pPersonArray);
            }
        }
        break;

    default:  break;
    }
}

void ActionwaySystemMessage::sendDataToHttpSrv(BYTE byKeyModule, BYTE byKeyAction, LPCSTR pData, size_t nLen)
{
    ICenterConnectorService* pCenterConnectorService = gSocialGlobal->getCenterConnectorService();
    if (nullptr == pCenterConnectorService) {
        return;
    }

    pCenterConnectorService->sendDataToDBHttpSvr(byKeyModule, byKeyAction, pData, nLen);
}
