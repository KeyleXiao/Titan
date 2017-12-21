#pragma once
#include "singleton.h"
#include "DGlobalMessage.h"

enum EMMsgView;


// ����һЩȫ����Ϣ���������ڵ��߳�
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