/*******************************************************************
** 文件名:	ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	4/6/2016
** 版  本:	1.0
** 描  述:	辅助功能类
********************************************************************/

#ifndef __EXTERNALHELPER_H__
#define __EXTERNALHELPER_H__

#include "IActorService.h"
#include "ClanDef.h"
#include "Chat_ManagedDef.h"

// 定义
class CExternalHelper
{
public:
	// 构造函数
	CExternalHelper(void);
	// 虚构函数
	~CExternalHelper(void);

    // 发送消息
    void sendClanSocial(BYTE byKeyAction, int nActionID, DWORD dwClanID,void *pData, int nLen);
    // 发送数据到客户端
	void send2Client(DWORD dwPDBID, BYTE byKeyModule, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen);

    // 取得帮会每级信息
    SClanLevel* getLevelInfo(int nClanLevel);
    // 取得帮会常量信息
	SClanConfig* getConfig();
	// 帮会卡牌信息
	SClanCardSchemeInfo* getClanCardInfo(int nLevel);

	// 获取角色任务数据辅助类
	int getActorTaskData(IActorService *pActor, int nKey, int nDefault = 0);

    // 显示提示文字
	void showSystemMessage(DWORD dwPDBID, EMChatTipID nTipID, LPCSTR szParam = NULL);

    /// purpose: 计算2个日期之间的天数
    int	DiffDay(int time1, int time2);
};
extern CExternalHelper				g_EHelper;

#endif // __EXTERNALHELPER_H__