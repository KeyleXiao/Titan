/*******************************************************************
** 文件名:	PlayerMoneyManager.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	7/4/2017
** 版  本:	1.0
** 描  述:  玩家金钱管理类

********************************************************************/

#ifndef __PLAYER_MONEY_MANAGER_H__
#define __PLAYER_MONEY_MANAGER_H__

#include "IBankPart.h"



// 玩家金钱管理类
class CPlayerMoneyManager : public IPlayerMoneyManager
{
public:
    ///////////////////////////////// IPlayerMoneyManager //////////////////////////
    /** 取得点券兑换金币的汇率
    @return : 
    */
    virtual double getExchangeRate(void);

    /** 是否可增加或扣除货币（只操作一种货币，支持一个点券转金币功能）
    @param nMoney:      操作点券/金币数量，注：负数为扣除，正数为增加
    @param nMoneyType:  货币类型 GAME_MONEY_TYPE,如果为
    @return : 
    */
    virtual bool canAddMoneyEx(int nMoney, BYTE nMoneyType);

    /** 是否可增加或扣除货币
    @param nTicket:  操作点券数量，注：负数为扣除，正数为增加
    @param nGlod  :  操作金币数量，注：负数为扣除，正数为增加
    @return : 
    */
    virtual bool canAddMoney(int nTicket, int nGlod);

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
    virtual bool addMoney(int nTicket, int nGlod, int nReason, const char * pszRemark=0, int nSubclassID=0, short nNum=0, bool bAddictLimit=true);

    // 返回Ticket数据库操作
    virtual void onDBReturnTicketAdd(int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
    ///////////////////////////////// CPlayerMoneyManager //////////////////////////

	// 构造函数
	CPlayerMoneyManager();

	// 析构函数
	~CPlayerMoneyManager();

	// 初始化
	bool init(CPlayerBankPart *pBankPart);


private:
    // 取得主角对象
    __IEntity *  getMaster() const;

    // 执行数据库操作
    bool executeMoneyDataBase(int nTicket, int nTickReasonID, int nGold, int nGlodReasonID, const char * pszRemark=0, int nSubclassID=0, short nNum=0, int nReserverID=0);

private:

	CPlayerBankPart *   m_pBankPart;

    // 是否正在操作金钱（点券、金币）给DB，DB返回后自动设置false
    bool                m_isOperatingToDB;
};

#endif	// __PLAYER_MONEY_MANAGER_H__