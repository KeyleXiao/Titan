//==========================================================================
/**
* @file	  : ExternalHelper.h
* @author : xiaobao
* created : 2009-3-7
* purpose : 战队模块公用函数实现类
*/
//==========================================================================
#pragma once
#include "Chat_ManagedDef.h"

struct SMailConfig;
struct SKinConfig;

// 外部接口外观类
class CExternalHelper
{
public:
	// 启用定时器
	bool SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo);

	// 销毁定时器
	bool KillTimer(DWORD timerID, ITimerHandler * handler);

	// 发送数据到场景服，ServerID不填，则发送到所有的场景服
	void SendDataToSceneSvr(LPCSTR pData, size_t nLen, CSID ServerID = INVALID_SERVER_ID);

	// 发送数据到某个玩家
	void SendDataToPerson(DWORD dwPDBID, LPCSTR pData, size_t nLen);

	// 取得某种身份的权限
	int GetIdentityPopedom(int nIdentity);

	// 发送客户端事件
	void FireClientEvent(DWORD dwPDBID, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, size_t nLen);

	// 取得当前时间的时间字符串
	LPCSTR GetCurTimeString(void);

	// 取得金钱字符串
	LPCSTR GetMoneyString(int nTong);

	/// purpose: 根据字符串取得时间值
	long GetLongTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSecond);
	
	/// purpose: 2个时间点是不是在同一天
	bool InOneDay(time_t time1, time_t time2);

	/// purpose: 其它数值数据记录
	//@ param dwMain     : 主ID
	//@ param nTarget	   : 与主ID相关的保留数据
	//@ param nNum	   : 数值大小
	//@ param byOpType   : 操作类型
	//@ param byApproach : 途径
	//@ param nTime      : 记录的时间
	//@ param szRemark   : 备注
	void TrackAddNumeric( DWORD dwMain, int nTarget, int nNum, BYTE byNumType, BYTE byOpType, int byApproach, const char* szRemark);

	/// purpose: 运行客户端一个函数
	void ClientRunScript(int nActorDBID,const char * line);

	/// purpose: 返回时间长度
	//@ return : 格式 x天x小时x分钟
	LPCSTR GetSecondString(int nDay, int nHour, int nMinute, int nSecond);

	/// purpose: 返回时间长度
	//@ return : 格式 x天x小时x分钟
	LPCSTR GetSecondString(int nSecond);

	/// purpose: 计算2个日期之间的天数
	int	DiffDay(int time1, int time2);

    /** 判断两个时间点之间相差几周
	@return
	*/
	int DiffWeek(DWORD dwTime1, DWORD dwTime2);

	/// purpose: 检查2个数的和是否合法
	bool CheckSumValid(int a, int b);

	// 根据世界ID判断只有压测才开放
	bool IsLimitKinTank();

	/// purpose: 往公共区发送数据
	bool SendToPublicServer(void* pData, size_t nLen);

	/// purpose: 往公共区发送数据
	bool SendToPublicServer(BYTE wKeyAction, void* pData, size_t nLen);

    void showSystemMessage(PDBID PlayerID, EMChatTipID nTipID, LPCSTR szParam);

	SMailConfig* getMailConfig(int mailID);

	// 取得战队常量信息
	SKinConfig* getConfig();
};
extern CExternalHelper		g_EHelper;