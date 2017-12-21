/*******************************************************************
** �ļ���:	UserList.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/11/28
** ��  ��:	1.0
** ��  ��:	�����û�����
** Ӧ  ��:	�����Ϊ��������������ID
			TODO	��Ҫ����ʱ��δ���������֡���Ϣ������
**************************** �޸ļ�¼ ******************************
** �޸���:
** ��  ��:
** ��  ��:
********************************************************************/
#pragma once
#include "IUserList.h"
#include "ISessionUser.h"
#include "Processer.h"

// TYPE Ϊʵ��ӵ��IConnection����
template<class TYPE>
class UserList : public IUserList
{
public:
	UserList() :m_dwCurrIndex(0) {}
	virtual ~UserList() {}

	Processer<TYPE>& GetProcesser() { return m_Processer; }

	// �ط�ʱ����
	void	shutdown(void);

	// �û�����
	size_t	Count() { return m_UserMap.size(); }

	// ȡUser
	TYPE*	Get(DWORD id);

	//************************************
	// Returns:   bool				true �ɹ��� false ����������
	// Qualifier:
	// Parameter: TYPE * pNewUser	����û�
	// Parameter: DWORD id			idΪ0����ʾ��Ҫ����������ID
	//************************************
	bool	AddUser(TYPE* pNewUser, DWORD id = 0);
	// ɾ���û�
	virtual void	DelUser(ISessionUser* pUser);

protected:
	virtual void	onDelUser(ISessionUser* pUser) = 0;

	// ע������Ҫ�����������Ϣ����m_Processer��
	virtual void	RegisterHandlers() = 0;
	// ��ȡ�����������ͬʱҲ��������ֵ��������ID��
	virtual DWORD	GetMaxID() = 0;
	// �������Ƿ�ﵽ����
	virtual	bool	IsFull() { return this->Count() >= GetMaxID(); }

private:
	// ����ID	TODO �˷�����Ҫ��Ԫ����
	DWORD	GenerateID();

protected:
	// <ID,User>
	typedef std::map<DWORD, TYPE*> USER_MAP;

	USER_MAP			m_UserMap;
	Processer<TYPE>		m_Processer;

private:
	DWORD	m_dwCurrIndex;
};

// ����Ϊ�����ľ���ʵ��
/////////////////////////////////////////////////////////////////////////////////////

// ȡUser
template<class TYPE>
TYPE* UserList<TYPE>::Get(DWORD id)
{
	auto it = m_UserMap.find(id);
	if (it != m_UserMap.end())
		return it->second;
	else
		return nullptr;
}

// ����û�
template<class TYPE>
bool UserList<TYPE>::AddUser(TYPE* pNewUser, DWORD id)
{
	if (IsFull())
		return false;

	const DWORD tmpId = id <= 0 ? GenerateID() : id;
	pNewUser->SetID(tmpId);
	m_UserMap[tmpId] = pNewUser;
	return true;
}

template<class TYPE>
void UserList<TYPE>::DelUser(ISessionUser* pUser)
{
	DWORD id = pUser->GetID();
	m_UserMap.erase(id);
}

template<class TYPE>
void UserList<TYPE>::shutdown(void)
{
	if (!m_UserMap.empty())
	{
		list<TYPE *> deletelist;

		for (USER_MAP::iterator it = m_UserMap.begin(); it != m_UserMap.end(); ++it)
		{
			TYPE* pUser = it->second;
			deletelist.push_back(pUser);
		}
		for (list<TYPE *>::iterator it0 = deletelist.begin(); it0 != deletelist.end(); ++it0)
		{
			TYPE * pUser = *it0;
			pUser->Release();
			//SafeRelease((*it0));
		}

		deletelist.clear();
		m_UserMap.clear();
	}
	
	m_dwCurrIndex = 0;
}

template<class TYPE>
DWORD UserList<TYPE>::GenerateID()
{
	const DWORD MAX_COUNT = GetMaxID();
	do
	{
		// +1�ȣ������ܹ���֤�������øո��Ƴ���User��ID
		m_dwCurrIndex += 1;
		// ����ʹ��2�ֽ����ڵ�ID
		if (m_dwCurrIndex > MAX_COUNT && m_UserMap.size() < MAX_COUNT)
		{
			m_dwCurrIndex = 1;
		}
		// 0Ϊ�Ƿ�ID
	} while (m_dwCurrIndex <= 0 || m_UserMap.find(m_dwCurrIndex) != m_UserMap.end());

	return m_dwCurrIndex;
}