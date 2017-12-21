#pragma once
#include "singleton.h"
#include "DGlobalMessage.h"

enum EMMsgView;


// 缓存一些全局消息对象，适用于单线程
class MsgFactory : public Singleton<MsgFactory>
{
public:
	MsgFactory();

	const SGameMsgHead&	BuildHead_VM(EMMsgView emMsg);
	const SGameMsgHead&	BuildHead_VG(EMMsgView emMsg);
	const SGameMsgHead&	BuildHead_VC(EMMsgView emMsg);

private:
	SGameMsgHead m_vmHeader;
	SGameMsgHead m_vgHeader;
	SGameMsgHead m_vcHeader;
};
extern MsgFactory& gMsg;


//MsgFactory::~MsgFactory()
//{
//}