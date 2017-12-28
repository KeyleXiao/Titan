#pragma once
#include "ISessionUser.h"


template<typename TypeID = DWORD>
struct IUserList
{
	virtual void DelUser(ISessionUser<TypeID>* pUser) = 0;
};