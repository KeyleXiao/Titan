/*******************************************************************
** 文件名:	ExternalHelper.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/3/29
** 版  本:	1.0
** 描  述:	辅助功能类
********************************************************************/

#pragma once

#include "IActorService.h"
#include "Chat_ManagedDef.h"

struct SWarSafetyZone;

// 限定v的范围为[a,b]
#define range(v, a, b) ((v) < (a) ? (a) : ((v) > (b) ? (b) : (v)))

#define DropT2ChangeT(a)  g_EHelper.talentDropType2TalentChangeType((a))
#define ChangeT2DropT(a)  g_EHelper.talentChangeType2TalentDropType((a))

// 定义
class CExternalHelper
{
public:
	// 构造函数
	CExternalHelper(void);
	// 虚构函数
	~CExternalHelper(void);

	// 启用定时器
	bool SetTimer(DWORD dwTimerID, DWORD dwInterval, ITimerHandler *pHandler, DWORD dwCallTimes = INFINITY_CALL, const char *debugInfo = 0);

	// 销毁定时器
	bool KillTimer(DWORD dwTimerID, ITimerHandler *pHandler);

    // 给“战场所有人”广播提示文字
	void showSystemMessage(int nSceneID, EMChatTipID nTipID, LPCSTR szParam = NULL, WORD nReserveData=0);

	// 给单个玩家发送系统提示文字
	void showSystemMessageToClient(PDBID pdbid, EMChatTipID nTipID, LPCSTR szParam = NULL);

    // 在“战场频道”显示提示文字
	void showSystemMessageToChatBox(int nSceneID, EMChatTipID nTipID, LPCSTR szParam = NULL, WORD nReserveData = 0);
	
	// 是否小件物品
	bool isLowGoods(int nGoodID);

	// 是否大件物品
	bool isHighGoods(int nGoodID);

	// 获取物品名字
	std::string getGoodsName(int nGoodID);

	// 本战场是否可以使用英雄星级
	bool isCanUseHeroStarLv(int nMatchType);

	// 发送实体消息到客户端WarPart
	void sendEntityMessage(UID uid, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen);
	// 发送消息到客户端clientWar
	void sendMessageToClientWar(UID uid, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen, bool bToView = false);
	// 构造直接发送消息到客户端显示层的数据
	void fillDataToClientView(obuf& ob, BYTE byKeyAction, LPCSTR pszData, size_t nDataLen);

	// 注册场景事件
	void Subscribe(int nSceneID, IEventExecuteSink* pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPSTR pszDesc);

	// 注册场景事件
	void UnSubscribe(int nSceneID, IEventExecuteSink* pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID);

	// 发场景事件
	void FireExecute(int nSceneID, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

	// 获取安全区脚本数据
	SWarSafetyZone* getWarSafetyZoneInfo(int nRound);

	SEntityXml_Server* getEntityXml();

	// 天赋改变类型转换成天赋掉落类型
	ETalentDropType talentChangeType2TalentDropType(ENUM_TALENT_CHANGE_TYPE changeType);

	// 天赋掉落类型转换成天赋改变类型
	ENUM_TALENT_CHANGE_TYPE talentDropType2TalentChangeType(ETalentDropType dropType);

	// EWarMedal转换成EDataType
	EDataType warMedal2DataType(EWarMedal warMedal);
};

extern CExternalHelper				g_EHelper;