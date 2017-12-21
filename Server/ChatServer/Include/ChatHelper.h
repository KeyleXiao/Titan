/*******************************************************************
** �ļ���:	ChatHelper.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��ɭ
** ��  ��:	2016-07-18
** ��  ��:	1.0
** ��  ��:	
********************************************************************/

#pragma once

#include "IServerGlobal.h"
#include "IServiceContainer.h"
#include "IChatScene.h"
#include "IChatSceneService.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// �������ϵͳָ��
// @param ������������󣬿��Ա�֤�ں���ִ�й�����m_ptr��ֵ�����ͷ�
//////////////////////////////////////////////////////////////////////////////////////////////////
class ChatHelper
{
public:
	SERVICE_PTR m_pContainer;
	IChatSceneService *m_ptr;

	ChatHelper() : m_pContainer(0),m_ptr(0)
	{
		IChatScene *pChatScene = gServerGlobal->getChatScene();
		if (pChatScene == 0)
		{
			return;
		}

		m_pContainer = pChatScene->getChatSceneService();
		if (m_pContainer == 0)
		{
			return;
		}

		m_ptr = (IChatSceneService*)m_pContainer->query_interface();
	}
};
