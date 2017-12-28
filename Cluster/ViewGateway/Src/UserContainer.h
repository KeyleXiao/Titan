/*******************************************************************
** 文件名:	UserContainer.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/06
** 版  本:	1.0
** 描  述:	适用于连接的ID由外部传入的情况
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "UserList.h"
#include "ByteRecord.h"


template<typename TYPE, typename TypeID = DWORD>
class UserContainer : public UserList<TYPE, TypeID>
{
public:
	// 创建连接对象
	TYPE* CreateUser(IConnection * pIncomingConn, TimerAxis& timerAxis)
	{
		TYPE* pNewUser = new TYPE(pIncomingConn, *this, m_Processer, timerAxis, &m_ByteRecord);
		m_unHandshakeSet.insert(pNewUser);

		return pNewUser;
	}

	// 删除用户
	virtual void	DelUser(ISessionUser<TypeID>* pUser)
	{
		UserList::DelUser(pUser);
		m_unHandshakeSet.erase(pUser);
	}

	//************************************
	// Returns:   bool 是否成功加入了UserMap
	// Qualifier: 当收到握手消息时，对方会发来其ID.此时会将本连接从临时集合中移除，并加入UserMap
	// Parameter: TYPE * pSession
	// Parameter: TypeID dwID
	//************************************
	bool OnSessionRcvID(TYPE* pSession, TypeID dwID)
	{
		pSession->SetID(dwID);
		m_unHandshakeSet.erase(pSession);
		return AddUser(pSession, dwID);
	}

	// 是否连接数已满
	virtual	bool IsFull()
	{
		size_t currCount = this->m_unHandshakeSet.size() + this->m_UserMap.size();
		return currCount >= this->GetMaxID();
	}

protected:
	typedef std::set<ISessionUser<TypeID>*>	SessionSet;

	SessionSet		m_unHandshakeSet;	// 储存还未发来“握手”消息的连接
	ByteRecord		m_ByteRecord;		// 流量记录器
};