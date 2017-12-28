/*******************************************************************
** �ļ���:	UserContainer.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/12/06
** ��  ��:	1.0
** ��  ��:	���������ӵ�ID���ⲿ��������
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "UserList.h"
#include "ByteRecord.h"


template<typename TYPE, typename TypeID = DWORD>
class UserContainer : public UserList<TYPE, TypeID>
{
public:
	// �������Ӷ���
	TYPE* CreateUser(IConnection * pIncomingConn, TimerAxis& timerAxis)
	{
		TYPE* pNewUser = new TYPE(pIncomingConn, *this, m_Processer, timerAxis, &m_ByteRecord);
		m_unHandshakeSet.insert(pNewUser);

		return pNewUser;
	}

	// ɾ���û�
	virtual void	DelUser(ISessionUser<TypeID>* pUser)
	{
		UserList::DelUser(pUser);
		m_unHandshakeSet.erase(pUser);
	}

	//************************************
	// Returns:   bool �Ƿ�ɹ�������UserMap
	// Qualifier: ���յ�������Ϣʱ���Է��ᷢ����ID.��ʱ�Ὣ�����Ӵ���ʱ�������Ƴ���������UserMap
	// Parameter: TYPE * pSession
	// Parameter: TypeID dwID
	//************************************
	bool OnSessionRcvID(TYPE* pSession, TypeID dwID)
	{
		pSession->SetID(dwID);
		m_unHandshakeSet.erase(pSession);
		return AddUser(pSession, dwID);
	}

	// �Ƿ�����������
	virtual	bool IsFull()
	{
		size_t currCount = this->m_unHandshakeSet.size() + this->m_UserMap.size();
		return currCount >= this->GetMaxID();
	}

protected:
	typedef std::set<ISessionUser<TypeID>*>	SessionSet;

	SessionSet		m_unHandshakeSet;	// ���滹δ���������֡���Ϣ������
	ByteRecord		m_ByteRecord;		// ������¼��
};