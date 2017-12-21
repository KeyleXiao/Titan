/*******************************************************************
** 文件名:	E:\speed\Social\KinServer\Src\KinManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/18/2015	14:47
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once
#include "IMessageHandler.h"
#include "IServiceContainer.h"
#include "IKinManager.h"
#include <map>

class KinManager:public IKinManager
{


public:
	////////////////////////////////IKinManager//////////////////////////////////////////
	virtual SERVICE_PTR GetKinService(){return m_KinServices;};
	
	virtual bool		Load();
	virtual void		Release();

	

private:
	bool CreateKinService();

private:
    SERVICE_PTR m_KinServices;
};

