/*******************************************************************
** 文件名:	ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/5/2016	11:14
** 版  本:	1.0
** 描  述:	辅助功能类
********************************************************************/

#ifndef __EXTERNALHELPER_H__
#define __EXTERNALHELPER_H__

#include "ClanDef.h"
#include "Chat_ManagedDef.h"

struct SMailConfig;
struct SKinMember;
// 外部接口外观类
class CExternalHelper
{
public:
	// 启用定时器
	bool setTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo = 0);
	// 销毁定时器
	bool killTimer(DWORD timerID, ITimerHandler * handler);

	// 发送数据到场景服，ServerID不填，则发送到所有的场景服
	void sendDataToZoneSvr(LPCSTR pData, size_t nLen, CSID ServerID = INVALID_SERVER_ID);
	// 发送数据到某个玩家
	void sendDataToPerson(DWORD dwPDBID, LPCSTR pData, size_t nLen);

    // 取得帮会职位
    int getClanIdentity(DWORD dwPDBID, int nShaikhID, int nKinIdentity);

    // 取得帮会每级信息
    SClanLevel* getLevelInfo(int nClanLevel);
    // 取得帮会常量信息
    SClanConfig *getConfig();
    // 帮会卡牌信息
    SClanCardSchemeInfo* getClanCardInfo(int nLevel);

    // 发送系统提示到单人
    void showSystemMessage(DWORD dwPDBID, EMChatTipID nTipID, LPCSTR szParam = NULL);
	// 发送系统提示到帮会
	void broadcastClanSystemTips(DWORD dwClanID, EMChatTipID nTipID, LPCSTR szParam = NULL);
    // 发送系统提示到战队
    void broadcastKinSystemTips(DWORD dwKinID, EMChatTipID nTipID, LPCSTR szParam);

	// 获取角色战队信息
	SKinMember* getActorKinMemberInfo(DWORD dwPDBID);

    // 取得某个身份的名字
    string getIdentityName(int nIdentity);

    // 取得某个玩家的帮会身份
    int getIdentity(DWORD dwPDBID);

    // 增加帮会日志
    void addClanGameNoteLog(DWORD dwClanID, const char *szParam);

	/** 判断两个时间点之间相差几周
	* @return
	*/
	int DiffWeek(DWORD dwTime1, DWORD dwTime2);

	// 获取邮件配置
	SMailConfig* getMailConfig(int mailID);
};
extern CExternalHelper		g_EHelper;

#endif // __EXTERNALHELPER_H__