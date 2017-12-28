//==========================================================================
/**
* @file	  : LegendCup.h
* @author : 秦其勇
* created : 2010-4-29   
* purpose : 杯赛相关
*/
//==========================================================================

#ifndef __LEGENDCUP_H__
#define __LEGENDCUP_H__

#include "LegendCupDef.h"
#include <vector>
#include <hash_map>
#include "ISocialGlobal.h"
#include "IDBEngineService.h"
#include "ITimerService.h"
#include "LegendCupService.h"
#include "ISchemeCenter.h"
#include "LegendCupSchemeDef.h"
#include "ICompetitionRoundBase.h"
#include "MailHelper.h"
#include "DateHandler.h"
using namespace LegendCup_ManagedDef;

class CLegendCup : public IDBRetSink, public DateHandler
{
    enum
    {
        ETIMER_END_REGISTER_TRIGGER,			// 报名时间触发器
        ETIMER_CONFIRM_REGISTER_TRIGGER,		// 最终确定报名触发器
        ETIMER_CURRENT_ROUND_BEGIN_TRIGGER,	// 当前第几轮开始触发器
        ETIMER_CURRENT_ROUND_END_TRIGGER,		// 当前第几轮结束触发器
    };

public:
	CLegendCup();
	virtual ~CLegendCup();
	bool Create(ILegendCupService* pService, SLegendCupBaseInfo* createInfo);
	void Release();

    ///////////////////////////////DateHandler///////////////////////////////////////////
    /// purpose: 日期达到
    virtual void        DateCome(unsigned long nTriggerID);  

    ///////////////////////////////IDBRetSink///////////////////////////////////////////
    virtual void		OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

public:

    /// purpose: 数据创建
    //@ pszContext 创建现场（DBREQ_TABLE_KIN结构）
    bool                createFromDB(ILegendCupService *pService, LPCSTR pszContext, int nLen);

    // 从DB读取附加列表相关(黑名单列表 申请列表..)
    bool                setCupDBEXlistContext(LPCSTR pszContext, int nLen);

    // 从DB读取参赛者
    bool                setCupDBMemberContext(LPCSTR pszContext, int nLen, obuf *pNodebuf, int& nMemberCount);

    // 从DB读取战队记录
    bool                setCupDBKinDataContext(LPCSTR pszContext, int nLen);

    // 从DB读取比赛信息
    bool                setCupDBCompetitionContext(LPCSTR pszContext, int nLen);

    // 从DB读取每轮战队位置信息
    bool                setCupDBRoundKinPos(LPCSTR pszContext, int nLen);

    // 从DB读取每轮时间信息
    bool                setCupDBRoundTimeInfo(LPCSTR pszContext, int nLen);

    // 获取当前状态
    ECupState           getLegendCupState();

    // 杯赛改变状态
    void                changeLegendCupState(ECupState eState);

    // 处理其它服务器发送过来的消息
    void                handleServerMsg(DWORD serverID, SNetMsgHead head, PACKAGE_PTR msg);

    // 处理客户端发送过来的消息
    void                handleClientMsg(DWORD client, SNetMsgHead head, PACKAGE_PTR msg);

    // 设置当前数据库读取完成标示
    void                setLegendCupDBReadFlag(ELegendDBReadFlag eReadFlag);

    // 获取当前数据库读取完成标示
    int                 getLegendCupDBReadFlag();

    // 继续下一轮比赛
    void                toNextRound(int nEndRoundID, list<DWORD> dwPromotionList);

    // 结束比赛
    void                endLegendCup(list<DWORD> finalList);  

    // 得到比赛战队信息
    SCupKinInfo*        getCupKinInfo(DWORD dwKinID);

    // 得到战队列表
    void                getRegisterKinList(map<DWORD,SCupRegisterInfo> &RegisterKinList);

    // 得到杯赛基础信息
    SLegendCupBaseInfo* getLegendCupBaseInfo();

    // 得到比赛ID
    LONGLONG            getLegendCupID();

    // 比赛是否结束
    bool                isEnd();

    // 结束报名状态
    bool                preEndRegisterState(DWORD dwActorID);

    // 申请参加比赛检查
    bool                registerCheck(SMsgJoinCup* pMsgInfo);

    // 申请参加比赛
    bool                addRegister(SMsgJoinCup* pMsgInfo);

    // 申请退出比赛
    bool                exitRegister(DWORD dwActorID, DWORD dwKinID);

    // 移动到黑名单
    bool                addToBlackList(DWORD dwActorID, DWORD dwKinID);

    // 移除黑名单
    bool                removeBlackList(DWORD dwActorID, DWORD dwKinID);

    // 调整节点比赛开始时间
    bool                eidtCompetitionNodeTime(DWORD dwChangeTime);

    // 调整当前轮的比赛结算时间
    bool                eidtCurRoundEndTime(DWORD dwActorID,DWORD dwChangeTime);

    // 准备开始比赛（确定完所有参赛队之后调用接口 线上系统自动调用 线下 手动调用）
    bool                competitionStart(); 

    // 给客户端发送比赛战队信息
    void                sendRegisterList2AllViewer();

     // 给客户端发送比赛战队信息
    void                sendRegisterList2Client(ClientID clientID);

    // 给客户端发送比赛黑名单信息
    void                sendBlackList2Client(ClientID clientID);

    // 给客户端
    void                sendKinMember2Client(ClientID clientID, DWORD dwKinID);

    // 发送每轮比赛信息到客户端
    void                sendAllRoundInfo2Client(ClientID clientID);

    // 发送所有参赛成员信息到客户端
    void                sendAllKinMemberInfo2Client(ClientID clientID);

    // 发送所有参赛成员信息到客户端
    void                sendAllRoundTimeInfo2Client(ClientID clientID);

    // 所有比赛节点信息相关
    void                sendAllCompetionNodeInfo2Client(ClientID clientID);

    // 申请参加比赛
    bool                havePassword();

    // 发布比赛(线下比赛外部接口调用)
    bool                publishCompetition();

    // 获取某一轮的开赛时间
    SRoundTimeInfo*     getRoundTimeInfo(int nRoundID);

    // 记录杯赛结果
    bool                recordCupWarResult(SMsgRecordNodeWarResult* pMsgInfo);

    // 改变杯赛节点状态
    bool                setLegendCupNodeState(SMsgSetCompetitionNodeState* sNodeState);

    // 裁定比赛结果
    bool                judgeNodeWarResult(SMsgJudgeWarResult* pJudgeInfo);

    // 进入杯赛房间
    bool                joinLegendCupRoom(SMsgJoinLegendCupRoom* pJoinInfo);

    // 查看比赛节点
    bool                lookUpCupNodeWarInfo(SMsgLookUpCupNodeWarInfo* pJoinInfo);

    // 是否在申请列表中
    bool                bKinInRegister(DWORD dwKinID);

    // 改变到新一轮比赛状态
    void                changeNewRoundCupState();

    // 是否有权限加入杯赛房间
    bool                haveRightJoinCupRoom(SMsgJoinLegendCupRoom *pJoinedRoom);

    // 是否有权限判定比赛
    bool                haveRightJudge(ClientID clientID);

    // 删除参赛玩家
    void                deleteKinMember(DWORD dwKinMemberID);

    // 最终确定报名列表和人员信息
    bool                bIsCanPublic(DWORD dwActorID);

    // 得到报名战队数量
    int                 getRegisterCount();

    // 获取可以重置比赛时间点
    DWORD               getRoundEndResetTime(int nRoundID);

    // 获得当前轮
    int                 getCurrentRoundID();

    // 更新单个杯赛信息到客户端
    void                updateSingleCupBaseInfo2Client(ClientID clientID, DWORD dwClientKinID, bool bNewCreate = false);

    // 更新单个杯赛信息到客户端
    void                updateSingleCupBaseInfoAllView( bool bNewCreate = false );

    // 退出开始比赛的节点
    void                exitStartCompetitionNode(int nSerchID, DWORD dwNewStartTime);

	// 弃权战队邮件提示
	void				sendAbstentionKinMail(DWORD dwFailedKinID);

    // 获取指定参赛战队成员列表
    void                getCupKinMemberList(DWORD dwKinID, list<DWORD> &KinMember);

    // 更新战队排名
    void                updatRankToKinRecord(DWORD dwKinID, int nRankID);

    // 保存本次杯赛排名信息
    void                saveRankListToDB();

    // 参赛玩家登陆
    void                onKinMemberLogin(DWORD dwActorID);

    // 参赛玩家登陆
    void                onKinMemberLogout(DWORD dwActorID);

    // 添加观察者
    void                addLegendCupViewer(DWORD dwActorID);

    // 移除观察者
    void                removeLegendCupViewer(DWORD dwActorID);

    // 发送到客户端
    void                send2Client( ClientID clientID, char *pData, int nSize );

    // 广播给所有观察者
    void                sendDataToAllViewer(char *pData, int nSize);

    // 广播给所有观察者
    void                getAllViewerList(map<DWORD,bool> &mapViewer);

    // 奖励每场参赛奖励
    void                distributionEveryValidWarPrize(list<DWORD> dwKinList);

    // 取消杯赛
    void				cancelLegendCup();

    // 获取报名截止时间
    DWORD               getRegisterLeftTime();

private:
    // 发放奖励相关
    bool                distributionOfRewards();

    // 分配奖金
    bool                distributionTotalBonus();

    // 分配奖励物品
    bool                distributionPrizeList();

    // 推算每轮比赛的相关时间
    bool                calculateRoundTimeInfo(int nFirstRoundID);

    // 保存申请列表
    void                saveRegisterToDB();

    // 保存黑名单列表
    void                saveBlackListToDB();

    // DB读取完成之后继续杯赛
    void                continueLegendCup();

    // 保存杯赛开始战队信息（退出战队无法参加 ）
    bool                saveCupKinData();

    // 创建每轮的比赛节点
    bool                createCompetitionRound();

    // 更新基础信息到数据库
	bool                writeCupBaseDataToDB();

    // 获取最终确定参赛列表时间
    DWORD               getConfirmRegisterLeftTime();

    // 获取本轮开始时间
    DWORD               getStartRoundLeftTime();

    // 获取本轮结束时间
    DWORD               getEndRoundLeftTime();

    // 从创建初始化
    bool                initalFromCreate();

    // 结束报名
    void                onTriggerEndRegister();

    // 最终确定报名列表和人员信息
    void                onTriggerConfirmRegister();

    // 结束报名
    void                onTriggerBeginRound();

    // 最终确定报名列表和人员信息
    void                onTriggerEndRound();

    // 重新设置本轮开始结束定时器
    void                resetTriggerRoundBeginAndEnd();

    // 重新设置本轮开始结束定时器
    int                 getRoundIDBySerchID(int nSerchID);

    // 退还申请杯赛资金
    void                sendBackRegisterNeedTicket(DWORD dwKinID, DWORD dwRegisterPDBID);

    // 记录最终名次
    void                recordKinFinalRank(list<DWORD> finalList);

	// 更新单个杯赛节点信息到客户端
	void				updateSingleCupNodeInfo2AllView( SCompetitionNode* pNodeInfo);

    // 更新本轮杯赛节点信息到客户端
    void				sendCurRoundInfo2Viewer();

	// 通知观察者移除杯赛信息
	void				send2AllViewerRemoveCupInfo();

private:
    ILegendCupService*          m_pLegendCupService;
    SLegendCupBaseInfo          m_LegendCupBaseInfo;
    
    map<DWORD, SCupRegisterInfo>  m_mapRegisterKin;           // 报名列表 战队ID 杯赛排名
    
    map<DWORD, bool>            m_mapRequstBlack;           // 该比赛黑名单          

    map<DWORD, SCupKinInfo>     m_KinInfoListDetail;        // 正式参赛战队详细信息
    map<DWORD, SLegendCupMemberInfo> m_CupMemberInfo;       // 正式参赛玩家详细信息

    int                         m_eDBReadFlag;              // ELegendDBReadFlag
    
     
    // 通过m_LegendCupBaseInfo.dwConfirmRegisterDeadline 以及定位的轮数 确定每轮比赛的时间
    // 由于开赛时的定位到第几轮是由参赛队伍数量确定的 所以不一定所有比赛轮数都有开赛时间
    map<int, SRoundTimeInfo>         m_mapRoundBeginTime;    // 每轮比赛开始时间记录
    map<int,ICompetitionRoundBase *> m_mapCompetitionRound;  // 比赛轮数相关

    map<PDBID,bool>                 m_mapCupViewActor;               // 杯赛观察者
};

#endif