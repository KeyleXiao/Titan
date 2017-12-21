#pragma once
#include "singleton.h"
#include "DGlobalMessage.h"

enum EMMsgView;


// ����һЩȫ����Ϣ���������ڵ��߳�
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
