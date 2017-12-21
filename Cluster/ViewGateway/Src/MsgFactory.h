#pragma once
#include "singleton.h"
#include "DGlobalMessage.h"

enum EMMsgView;


// ����һЩȫ����Ϣ���������ڵ��߳�
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