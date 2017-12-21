/*******************************************************************
** 文件名:	Handler.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/27
** 版  本:	1.0
** 描  述:	具体的消息处理类请继承此基类
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "IHandler.h"
#include "ISerializableData.h"

// TYPE为连接的持有者；MsgT为要处理的消息类(MsgT中不能有虚函数)
template <class TYPE, class MsgT>
class Handler : public IHandler<TYPE>
{
public:
	// 模块号
	virtual	BYTE	GetModuleId() { return MsgT::GetModuleId(); }
	// 消息号
	virtual	BYTE	GetActionId() { return MsgT::GetActionId(); }

	virtual void	_Handle(TYPE* pSession, SGameMsgHead* head, void* data, size_t len)
	{
		Assert(pSession);
		Assert(head);
		Assert(data);
		if (pSession==NULL || head ==NULL || data == NULL)
			return;

		if (std::is_base_of<ISerializableData, MsgT>::value)
		{
			/*
			这里如果抛异常，有如下可能：
			1.toBytes、fromBytes里面的代码写错了（如果已经改为由工具生成代码，则不可能写错）；
			2.对方发来的恶意消息；
			因此，这里考虑加try...catch	TODO
			*/
			MsgT tMsgT;
			ISerializableData* pISD = (ISerializableData*)(&tMsgT);
			pISD->fromBytes((char*)data, len);
			Handle(pSession, head, &tMsgT);
		}
		else
		{
			MsgT * pMsg = (MsgT *)(data);	// 只要不调用MsgT的虚函数，这里就是安全的
			if (len != sizeof(MsgT))
			{
				Assert(len == sizeof(MsgT));
				return;
			}
			Handle(pSession, head, pMsg);
		}
	}

protected:
	//************************************
	// Method:    Handle
	// FullName:  ModuleViewHandler<TYPE, MsgT>::Handle
	// Access:    virtual protected 
	// Returns:   void
	// Qualifier:	这些参数在实现类中，都不需要再判NULL了，因为基类中已经判断过了
	// Parameter: TYPE * pSession		消息来源于此连接
	// Parameter: SGameMsgHead * head	通用消息头
	// Parameter: MsgT * pMsg			具体的消息对象
	//************************************
	virtual void	Handle(TYPE* pSession, SGameMsgHead* head, MsgT* pMsg) = 0;
};