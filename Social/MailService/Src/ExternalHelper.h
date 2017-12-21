/*******************************************************************
** 文件名:	ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	2/6/2017
** 版  本:	1.0
** 描  述:	辅助功能类
********************************************************************/
#pragma once
#include "Chat_ManagedDef.h"
#include "MailDef.h"

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

	// 取得当前时间的时间字符串
	LPCSTR GetCurTimeString(void);

	/// purpose: 根据字符串取得时间值
	long GetLongTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSecond);
	
	/// purpose: 2个时间点是不是在同一天
	bool InOneDay(time_t time1, time_t time2);

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

	// 是否是电脑PDBID
	bool IsComputer(DWORD dwPDBID);

	/// purpose: 系统消息提示
    void showSystemMessage(PDBID PlayerID, EMChatTipID nTipID, LPCSTR szParam);

	// 获取邮件配置脚本数据
	SMailConfig* getMailConfig(int nMailFillID);

	// 获取邮件填充邮件内容
	string getFillMailContent(string strMailContent, LPCSTR szParamList);
};
extern CExternalHelper		g_EHelper;