#pragma once
#include "singleton.h"
#include "DGlobalMessage.h"

enum EMMsgView;


// 缓存一些全局消息对象，适用于单线程
class MsgFactory : public Singleton<MsgFactory>
{
public:
	MsgFactory();

	const SGameMsgHead&	BuildHead_MV(EMMsgView emMsg);
	const SGameMsgHead&	BuildHead_MG(EMMsgView emMsg);
	const SGameMsgHead&	BuildHead_MC(EMMsgView emMsg);
	const SGameMsgHead&	BuildHead_MS(EMMsgView emMsg);

private:
	SGameMsgHead m_mvHeader;
	SGameMsgHead m_mgHeader;
	SGameMsgHead m_mcHeader;
	SGameMsgHead m_msHeader;
};
extern MsgFactory& gMsg;
