/*******************************************************************
** �ļ���: E:\speed\Server\KinServer\Src\KinScene.h
** ��  Ȩ: (C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	ս��ϵͳ������
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
    // ִ���ͷ�Service
    virtual void            onStop();
	virtual void			release();
	virtual SERVICE_PTR		getKinSceneService();

private:
	/// purpose:  ȡ��IMatchSceneService�ӿ�ָ��
	IKinSceneService*		getIKinSceneService();

private:
	SERVICE_PTR m_KinSceneServices;

private:
	bool createKinSceneService();

};