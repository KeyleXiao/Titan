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
template<typename TYPE, typename TypeID = DWORD>
class UserList : public IUserList<TypeID>
{
public:
	UserList() :m_dwCurrIndex(0) {}
	virtual ~UserList() {}

	Processer<TYPE>& GetProcesser() { return m_Processer; }

	// �ط�ʱ����
	void	Shutdown(void);

	// �û�����
	size_t	Count() { return m_UserMap.size(); }

	// ȡUser
	TYPE*	Get(TypeID id);

	//************************************
	// Returns:   bool				true �ɹ��� false ����������
	// Qualifier:
	// Parameter: TYPE * pNewUser	����û�
	// Parameter: TypeID id			idΪ0����ʾ��Ҫ����������ID
	//************************************
	bool	AddUser(TYPE* pNewUser, TypeID id = 0);
	// ɾ���û�
	virtual void	DelUser(ISessionUser<TypeID>* pUser);

protected:
	virtual void	OnDelUser(ISessionUser<TypeID>* pUser) = 0;

	// ע������Ҫ�����������Ϣ����m_Processer��
	virtual void	RegisterHandlers() = 0;
	// ��ȡ�����������ͬʱҲ��������ֵ��������ID��
	virtual TypeID	GetMaxID() = 0;
	// �������Ƿ�ﵽ����
	virtual	bool	IsFull() { return this->Count() >= GetMaxID(); }

private:
	// ����ID	TODO �˷�����Ҫ��Ԫ����
	TypeID	GenerateID();

protected:
	// <ID,User>
	typedef std::map<TypeID, TYPE*> USER_MAP;

	USER_MAP			m_UserMap;
	Processer<TYPE>		m_Processer;

private:
	TypeID	m_dwCurrIndex;
};

// ����Ϊ�����ľ���ʵ��
/////////////////////////////////////////////////////////////////////////////////////

// ȡUser
template<class TYPE, typename TypeID>
TYPE* UserList<TYPE, TypeID>::Get(TypeID id)
{
	auto it = m_UserMap.find(id);
	if (it != m_UserMap.end())
		return it->second;
	else
		return nullptr;
}

// ����û�
template<class TYPE, typename TypeID>
bool UserList<TYPE, TypeID>::AddUser(TYPE* pNewUser, TypeID id)
{
	if (IsFull())
		return false;

	const TypeID tmpId = id <= 0 ? GenerateID() : id;
	pNewUser->SetID(tmpId);
	m_UserMap[tmpId] = pNewUser;
	return true;
}

template<class TYPE, typename TypeID>
void UserList<TYPE, TypeID>::DelUser(ISessionUser<TypeID>* pUser)
{
	TypeID id = pUser->GetID();
	m_UserMap.erase(id);

	OnDelUser(pUser);
}

template<class TYPE, typename TypeID>
void UserList<TYPE, TypeID>::Shutdown(void)
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
		}

		deletelist.clear();
		m_UserMap.clear();
	}
	
	m_dwCurrIndex = 0;
}

template<class TYPE, typename TypeID>
TypeID UserList<TYPE, TypeID>::GenerateID()
{
	const TypeID MAX_COUNT = GetMaxID();
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