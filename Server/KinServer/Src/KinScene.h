/*******************************************************************
** 文件名: E:\speed\Server\KinServer\Src\KinScene.h
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	战队系统场景服
********************************************************************/

#pragma once
#include "IMessageHandler.h"
#include "IServiceContainer.h"
#include "KinSceneService.h"
#include "IKinScene.h"

class KinScene:public IKinScene
{
public:
	///////////////////////////////IKinScene///////////////////////////////////////////
	virtual bool			load();
    // 执行释放Service
    virtual void            onStop();
	virtual void			release();
	virtual SERVICE_PTR		getKinSceneService();

private:
	/// purpose:  取得IMatchSceneService接口指针
	IKinSceneService*		getIKinSceneService();

private:
	SERVICE_PTR m_KinSceneServices;

private:
	bool createKinSceneService();

};