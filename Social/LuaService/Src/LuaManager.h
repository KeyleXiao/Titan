/*******************************************************************
** 文件名:	LuaManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	3/31/2016	17:19
** 版  本:	1.0
** 描  述:	帮会管理
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