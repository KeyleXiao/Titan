#pragma once

class ISessionUser;

class IUserList
{
public:
	virtual void DelUser(ISessionUser* pUser) = 0;
	//virtual void OnRecv(DWORD dwDataLen) = 0;
protected:
private:
};