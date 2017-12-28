/*******************************************************************
** 文件名:	UserList.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/28
** 版  本:	1.0
** 描  述:	连接用户容器
** 应  用:	本类会为连接生成自增的ID
			TODO	需要清理长时间未发来“握手”消息的连接
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/
#pragma once
#include "IUserList.h"
#include "ISessionUser.h"
#include "Processer.h"

// TYPE 为实际拥有IConnection的类
template<typename TYPE, typename TypeID = DWORD>
class UserList : public IUserList<TypeID>
{
public:
	UserList() :m_dwCurrIndex(0) {}
	virtual ~UserList() {}

	Processer<TYPE>& GetProcesser() { return m_Processer; }

	// 关服时调用
	void	Shutdown(void);

	// 用户数量
	size_t	Count() { return m_UserMap.size(); }

	// 取User
	TYPE*	Get(TypeID id);

	//************************************
	// Returns:   bool				true 成功； false 连接数已满
	// Qualifier:
	// Parameter: TYPE * pNewUser	添加用户
	// Parameter: TypeID id			id为0，表示需要生成自增的ID
	//************************************
	bool	AddUser(TYPE* pNewUser, TypeID id = 0);
	// 删除用户
	virtual void	DelUser(ISessionUser<TypeID>* pUser);

protected:
	virtual void	OnDelUser(ISessionUser<TypeID>* pUser) = 0;

	// 注册这里要处理的所有消息，到m_Processer中
	virtual void	RegisterHandlers() = 0;
	// 获取最大连接数（同时也是允许出现的最大自增ID）
	virtual TypeID	GetMaxID() = 0;
	// 连接数是否达到上限
	virtual	bool	IsFull() { return this->Count() >= GetMaxID(); }

private:
	// 生成ID	TODO 此方法需要单元测试
	TypeID	GenerateID();

protected:
	// <ID,User>
	typedef std::map<TypeID, TYPE*> USER_MAP;

	USER_MAP			m_UserMap;
	Processer<TYPE>		m_Processer;

private:
	TypeID	m_dwCurrIndex;
};

// 以下为方法的具体实现
/////////////////////////////////////////////////////////////////////////////////////

// 取User
template<class TYPE, typename TypeID>
TYPE* UserList<TYPE, TypeID>::Get(TypeID id)
{
	auto it = m_UserMap.find(id);
	if (it != m_UserMap.end())
		return it->second;
	else
		return nullptr;
}

// 添加用户
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
		// +1先，这样能够保证：不重用刚刚移除的User的ID
		m_dwCurrIndex += 1;
		// 尽量使用2字节以内的ID
		if (m_dwCurrIndex > MAX_COUNT && m_UserMap.size() < MAX_COUNT)
		{
			m_dwCurrIndex = 1;
		}
		// 0为非法ID
	} while (m_dwCurrIndex <= 0 || m_UserMap.find(m_dwCurrIndex) != m_UserMap.end());

	return m_dwCurrIndex;
}