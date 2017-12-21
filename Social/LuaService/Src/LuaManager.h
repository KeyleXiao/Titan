/*******************************************************************
** �ļ���:	LuaManager.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	3/31/2016	17:19
** ��  ��:	1.0
** ��  ��:	������
********************************************************************/

#ifndef __LUAMANAGER_H__
#define __LUAMANAGER_H__

#include "IServiceContainer.h"
#include "ILuaManager.h"

class LuaManager : public ILuaManager
{
public:
	////////////////////////////////ILuaManager//////////////////////////////////////////
	virtual SERVICE_PTR getLuaService(){return m_LuaServices;};
	virtual bool		load();
	virtual void		release();

private:
	SERVICE_PTR m_LuaServices;

private:
	bool createLuaService();
};

#endif // __LUAMANAGER_H__