/*******************************************************************
** 文件名:	PlayerMoneyManager.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	7/4/2017
** 版  本:	1.0
** 描  述:  玩家金钱管理类

********************************************************************/

#include "stdafx.h"
#include <functional>

#include "OssLogDef.h"
#include "IGameDatabaseDef.h"
#include "PlayerBankPart.h"
#include "IAntiAddictServer.h"
#include "IOSSLogService.h"
#include "ExternalHelper.h"
#include "PlayerMoneyManager.h"

//////////////////////////////////////////////////////////////////////////

std::function<bool(int,int)> check_money = [](int nCurValue, int nOptValue) -> bool
{
    if ( nOptValue == 0)
        return true;

    // 不够了，不能操作
    if (nCurValue + nOptValue < 0)
    {
        return false;
    }
    // 数据库有上限限制，这里先判断，防止数据库才出错
    if (nCurValue + nOptValue > 100000000)
    {
        return false;
    }
    return true;
};

//////////////////////////////////////////////////////////////////////////

// 构造函数
CPlayerMoneyManager::CPlayerMoneyManager()
    : m_pBankPart(nullptr)
    , m_isOperatingToDB(false)    /*  是否正在操作金钱（点券、金币）给DB，DB返回后自动设置false */
{

}

// 析构函数
CPlayerMoneyManager::~CPlayerMoneyManager()
{
	m_pBankPart = nullptr;
}

// 初始化
bool CPlayerMoneyManager::init(CPlayerBankPart *pBankPart)
{
	m_pBankPart = pBankPart;

	return true;
}

// 取得主角对象
inline __IEntity * CPlayerMoneyManager::getMaster() const
{
    return m_pBankPart ? m_pBankPart->getHost() : nullptr;
}

/** 取得点券兑换金币的汇率
@return : 
*/
double CPlayerMoneyManager::getExchangeRate(void)
{
    return 1.5;
}

/** 是否可增加或扣除货币
@param nMoney:      操作点券/金币数量，注：负数为扣除，正数为增加
@param nMoneyType:  货币类型 GAME_MONEY_TYPE,如果为
@return : 
*/
bool CPlayerMoneyManager::canAddMoneyEx(int nMoney, BYTE nMoneyType)
{
    // 如果在执行数据库操作，而本次将不能进行扣除操作
    if ( m_isOperatingToDB && (nMoney < 0) )
    {
        return false;
    }
    __IEntity * pMaster = getMaster();
    if(nullptr == pMaster)
    {
        return false;
    }
    
    int nCurrentTicket  = pMaster->getIntProperty(PROPERTY_TICKET);
    int nCurrentGlod    = pMaster->getIntProperty(PROPERTY_GOLD);

    // 点券
    switch(nMoneyType)
    {
    case MONEY_TICKET:  // 只消耗点券
        {
            return check_money(nCurrentTicket, nMoney);
        }
        break;

    case MONEY_GOLD:    // 只消耗金币
        {
             return check_money(nCurrentGlod, nMoney);
        }
        break;

    case MONEY_ALL:     // TODO 点券可以转换金币
        {
            // TODO先校验金币
            // 折算点券转换

        }
        break;

    default: break;
    }
    
    return false;
}


/** 是否可增加或扣除货币
@param nTicket:  操作点券数量，注：负数为扣除，正数为增加
@param nGlod  :  操作金币数量，注：负数为扣除，正数为增加
@return : 
*/
bool CPlayerMoneyManager::canAddMoney(int nTicket, int nGlod)
{
    // 校验点券
    if(nTicket != 0)
    {
        if( !canAddMoneyEx(nTicket, MONEY_TICKET) )
        {
            return false;
        }
    }
    // 校验金币
    if(nGlod != 0)
    {
        if( !canAddMoneyEx(nGlod, MONEY_GOLD) )
        {
            return false;
        }
    }

    return true;
}

/** 增加或扣除货币
@param nTicket: 添加的点券，负数表示扣除点卷
@param nGlod  : 添加的金币，负数表示扣除金币
@param  : nReason 添加的原因（产生OSS_RESOURCE_ADD_RANGE/消耗的途径OSS_RESOURCE_OPT_DEL/转进OSS_RESOURCE_OPT_TRANSIN/转出OSS_RESOURCE_OPT_TRANSOUT）
@param  : pszRemark 添加的点券日志的备注（记日志用）
@param  : nSubclassID, 子类ID，购买物品则填物品ID，没有子类默认0
@param  : nNum 个数，子类是物品ID则填物品个数
@param  : bAddictLimit，是否考虑沉迷限制
@ note  : 在添加点券操作提交数据库（包括其他模块提交的）还没有返回前，这时候你提交点券是会失败的
*/
bool CPlayerMoneyManager::addMoney(int nTicket, int nGlod, int nReason, const char * pszRemark/*=0*/, int nSubclassID/*=0*/, short nNum/*=0*/, bool bAddictLimit/*=true*/)
{
    __IPlayerRole * pMaster = CAST_TYPE(__IPlayerRole*, getMaster());
    if(pMaster == 0)
    {
        return false;
    }
    UID uid = pMaster->getUID();
    // 防沉迷衰减
    IAntiAddictServer * pAntiAddict = gServerGlobal->getAntiAddictServer();
    if ( pAntiAddict && nGlod > 0 && bAddictLimit)
    {
        int reduce = pAntiAddict->getReduceRate(uid, pMaster->getPlayerLevel());
        if ( reduce < 100 )
        {
            nGlod = ( nGlod * reduce ) / 100;
            char szParam[256];
            ssprintf_s(szParam, sizeof(szParam), a2utf8("%d;防沉迷状态,收益衰减至%d％,实际获得金币%d"), CHAT_CHANNEL_SYSTEM, reduce, nGlod);
            g_EHelper.showSystemMessage(uid, CHAT_TIP_ANTIADDICT_INCOME_REDUCE, szParam);
        }
    }
    if (nTicket == 0 && nGlod==0)
    {
        return true;
    }
    // 不能在游戏库中增加点券
    if(nTicket > 0)
    {
        return false;
    }

    // 上一轮还在处理中就抛弃本次操作
    if (m_isOperatingToDB && (nTicket < 0 || nGlod < 0) )
    {
        return false;
    }

    int nCurrentTicket = pMaster->getIntProperty(PROPERTY_TICKET);
    int nCurrentGlod   = pMaster->getIntProperty(PROPERTY_GOLD);

    // 只消耗点券或金币
    if(nTicket==0 || nGlod==0)
    {
        bool checked = true;
        EMChatTipID nMsgID = CHAT_TIP_ENTITY_BEGIN;
        if (nTicket != 0)
        {
            if (!check_money(nCurrentTicket, nTicket))
            {
                nMsgID = CHAT_TIP_ENTITY_TICKET_OPERATE_ERROR;
                checked = false;
            }
        }
        
        if (nGlod != 0)
        {
            if (!check_money(nCurrentGlod, nGlod))
            {
                nMsgID = CHAT_TIP_ENTITY_GOLD_OPERATE_ERROR;
                checked = false;
            }
        }

        // 不合法
        if(!checked)
        {
            if (nMsgID != CHAT_TIP_ENTITY_BEGIN)
            {
                g_EHelper.showSystemMessage(uid, nMsgID, "");
            }
            return false;
        }

        executeMoneyDataBase(nTicket,nReason,nGlod,nReason,pszRemark,nSubclassID,nNum,0);
    }
    else    // 同时要消耗点券和金币（同时使用两种货币，以金币为单位结算）
    {
        // 不合法校验
        if (!check_money(nCurrentTicket, nTicket))
        {
            g_EHelper.showSystemMessage(uid, CHAT_TIP_ENTITY_TICKET_OPERATE_ERROR, "");
            return false;
        }
        
        if (!check_money(nCurrentGlod, nGlod))
        {
            g_EHelper.showSystemMessage(uid, CHAT_TIP_ENTITY_GOLD_OPERATE_ERROR, "");
            return false;
        }

        // 增加金币操作
        if(nGlod > 0)
        {
            executeMoneyDataBase(nTicket,nReason,nGlod,nReason,pszRemark,nSubclassID,nNum,0);
        }
        else    // 消耗金币操作
        {
            // 1、按比例把点券转换成金币(写一条点券消耗记录(认为是兑换记录），一条金币增加记录)（游戏库）DBREQ_PARAM_GAMETICKET_ADDMINE
            int nExhGlod = abs(nTicket) * getExchangeRate(); // 入口消耗点券数折算出的金币数
            int nAddGlod = nExhGlod;                         // 点券折算出来的金币数

            // TODO 下面都是异步操作，异常时还是蛮麻烦的
            executeMoneyDataBase(nTicket,OSSRES_DEL_TICKET_TO_GLOD,nAddGlod,OSSRES_ADD_GLOD_FROM_TICKET,pszRemark,nSubclassID,nNum,0);

            // 折算出的金币 + 入口参数要扣除的金币
            int nCostGlod = -1*nAddGlod + nGlod;

            // 2、 消耗金币记录（游戏库）DBREQ_PARAM_GAMETICKET_ADDMINE
            executeMoneyDataBase(0,0,nCostGlod,OSSRES_DEL_GLOD_EXCHAGE,pszRemark,nSubclassID,nNum,0);

            // 商城购买逻辑自己要执行这个（3、 商城购买物品金币记录（日志库）DBREQ_OSS_MONITOR_ACTORTICKET_ITEM）
        }
    }

    // 设置请求状态，为true时候，不能进行商城购买操作
    m_isOperatingToDB = true;

    // TODO 把用户信息发给客户端
    return true;
}

// 执行数据库操作
inline bool CPlayerMoneyManager::executeMoneyDataBase(int nTicket, int nTickReasonID, int nGold, int nGlodReasonID, const char * pszRemark, int nSubclassID, short nNum, int nReserverID)
{
    __IPlayerRole * pMaster = CAST_TYPE(__IPlayerRole*, getMaster());
    if(pMaster == 0)
    {
        return false;
    }
    IDBEngineService* pDBEngine = (IDBEngineService*)gServerGlobal->getDBEngineService();
    if (pDBEngine == NULL)
    {
        WarningLn(__FUNCTION__": failed pDBEngine == NULL, name="<< pMaster->getName());
        return false;
    }
    IOSSLogService * pOSSLogServer = gServerGlobal->getOSSLogService();
    if (pOSSLogServer == NULL)
    {
        WarningLn(__FUNCTION__": failed pOSSLogServer== NULL, name="<< pMaster->getName());
        return false;
    }

    int nPDBID          = pMaster->getIntProperty(PROPERTY_ID);
    int nCurrentTicket  = pMaster->getIntProperty(PROPERTY_TICKET);
    int nCurrentGlod    = pMaster->getIntProperty(PROPERTY_GOLD);
    DWORD dwWorldID     = m_pBankPart->getFromGameWorldID();
    DWORD dwUserID      = m_pBankPart->getUserID();

    // 消耗金币记录
    DBREQ_PARAM_GAMETICKET_ADDMINE dbParam;
    dbParam.dwWorldID       = dwWorldID;        // 游戏世界ID	
    dbParam.dwUserID		= dwUserID;         // 帐号ID	
    dbParam.dwActorID		= nPDBID;           // 角色ID
    dbParam.nAddTick		= nTicket;		    // 点券数 正数为加，负数为减
    dbParam.nReasonID		= nTickReasonID;    // 加减原因
    dbParam.nAddBindTick    = nGold;			// 金币数 正数为加，负数为减
    dbParam.nBindReasonID   = nGlodReasonID;    // 加减原因
    dbParam.nSubclassID	    = nSubclassID;		// 子类ID，购买物品则填物品ID，没有子类默认0
    dbParam.nNum			= nNum;	            // 个数，子类是物品ID则填物品个数

    if(!pDBEngine->executeSP(GAMEDB_REQUEST_GAMETICKET_ADDMINE, dbParam.dwActorID, (LPCSTR)&dbParam, sizeof(dbParam), m_pBankPart, dwWorldID))
    {
        WarningLn(__FUNCTION__": GAMEDB_REQUEST_GAMETICKET_ADDMINE execute failed,dwUserID ="<< dbParam.dwUserID <<",name="<< pMaster->getName()<<",PDBID="<<nPDBID);
        return false;
    }
    // 数值日记
    if(nTickReasonID > 0 && nTicket != 0) pOSSLogServer->addTicketLog(dwWorldID, nPDBID, nPDBID, nTicket, nTickReasonID, 0, pszRemark);
    if(nGlodReasonID > 0 && nGold != 0) pOSSLogServer->addMoneyLog(dwWorldID, nPDBID, nPDBID, nGold, nGlodReasonID, 0, pszRemark);

    int nNewTicket      = nCurrentTicket + nTicket;
    int nNewBindTicket  = nCurrentGlod + nGold; 

    // 先修改下内存数据，后面还是以数据库返回为准
    pMaster->setIntProperty(PROPERTY_TICKET, nNewTicket);
    pMaster->setIntProperty(PROPERTY_GOLD, nNewBindTicket);
    // 修改PLAYER内存数据
    SPlayerInfo *  pPlayerInfo = m_pBankPart->getPlayerInfo();
    if(pPlayerInfo)
    {
        pPlayerInfo->dwTicket       = nNewTicket;
        pPlayerInfo->dwBindTicket   = nNewBindTicket;
    }
    return true;
}


// 返回Ticket数据库操作
void CPlayerMoneyManager::onDBReturnTicketAdd( int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen)
{
    m_isOperatingToDB = false;

    __IPlayerRole * pMaster = CAST_TYPE(__IPlayerRole*, getMaster());
    if(pMaster == 0)
    {
        return;
    }
    if(m_pBankPart == 0)
    {
        return;
    }

    if ( nDBRetCode != DBRET_CODE_SUCCEED )
    {
        // 触发修改失败事件
        event_actor_change_money event_data;
        event_data.dwUserID = m_pBankPart->getUserID();
        event_data.dwActorID = (DWORD)pMaster->getIntProperty(PROPERTY_ID);
        event_data.bResult = false;  
        sstrcpyn(event_data.szParam,pszDBRetDesc, sizeof(event_data.szParam));
        pMaster->getEntityEvent()->FireExecute(EVENT_ACTOR_CHANGE_MONEY, (LPCSTR)&event_data, sizeof(event_data));

        // 写金币减少日志
        IOSSLogService * pOSSLogServer = gServerGlobal->getOSSLogService();
        if (pOSSLogServer != NULL && pszDBRetDesc != NULL )
        {
            char szText[512] = {0};
            ssprintf_s(szText, sizeof(szText), "Ticket Operate Fail:%s", pszDBRetDesc);
            pOSSLogServer->addGameNoteLog(getThisGameWorldID(), OSS_NOTE_TRACK_PERSON, nQueueIndex, szText);
        }

        ErrorLn( __FUNCTION__<<": Operate Ticket Failed, Pdbid="<<nQueueIndex<<", nCmdID="<<nCmdID<<",nDBRetCode="<<nDBRetCode<<",Desc="<<pszDBRetDesc );
        return;
    }

    if (nOutLen != sizeof(DBREQ_RESULT_GAMETICKET_ADDMINE))
    {
        return;
    }

    DBREQ_RESULT_GAMETICKET_ADDMINE* pRetData = (DBREQ_RESULT_GAMETICKET_ADDMINE*)(pOutData);
    DWORD dwPDBID = pRetData->dwActorID;
    DWORD dwUserID = pRetData->dwUserID;
    int nTotalTicket = pRetData->nTotalTicket;          // 点券
    int nTotalBindTicket = pRetData->nTotalBindTicket;  // 金币

    // 更新用户TICKET数据
    pMaster->setIntProperty(PROPERTY_TICKET, nTotalTicket);
    // 更新用户金币数据
    pMaster->setIntProperty(PROPERTY_GOLD, nTotalBindTicket);

    // 修改PLAYER内存数据
    SPlayerInfo *  pPlayerInfo = m_pBankPart->getPlayerInfo();
    if(pPlayerInfo)
    {
        int nBeforeTotalTicket = pPlayerInfo->dwTicket;
        pPlayerInfo->dwTicket = nTotalTicket;
        pPlayerInfo->dwBindTicket = nTotalBindTicket;
        // 触发修改成功事件
        event_actor_change_money event_data;
        event_data.dwUserID = dwUserID;
        event_data.dwActorID = dwPDBID;
        event_data.bResult = true;     
        event_data.nTotalTicket = pPlayerInfo->dwTicket;
        event_data.nTotalTicketBefore = nBeforeTotalTicket;
        pMaster->getEntityEvent()->FireExecute(EVENT_ACTOR_CHANGE_MONEY, (LPCSTR)&event_data, sizeof(event_data));
    }

    ENTITY_TRACELN(__FUNCTION__": PDBID="<< dwPDBID <<",PROPERTY_TICKET="<<nTotalTicket<<", PROPERTY_GOLD="<<nTotalBindTicket);
}
