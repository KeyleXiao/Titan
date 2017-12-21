#pragma once
#include "singleton.h"
#include "DGlobalMessage.h"

enum EMMsgView;


// 缓存一些全局消息对象，适用于单线程
class MsgFactory : public Singleton<MsgFactory>
{
public:
	MsgFactory();

	const SGameMsgHead&	BuildHead_GM(EMMsgView emMsg);
	const SGameMsgHead&	BuildHead_GV(EMMsgView emMsg);
	const SGameMsgHead&	BuildHead_GC(EMMsgView emMsg);

private:
	SGameMsgHead m_gmHeader;
	SGameMsgHead m_gvHeader;
	SGameMsgHead m_gcHeader;
};
extern MsgFactory& gMsg;


//MsgFactory::~MsgFactory()
//{
//}