/*******************************************************************
** �ļ���:	ClanHelper.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	��ɭ
** ��  ��:	2015-05-31
** ��  ��:	1.0
** ��  ��:	
********************************************************************/

#pragma once

#include "IServerGlobal.h"
#include "IClanScene.h"
#include "IClanSceneService.h"

//////////////////////////////////////////////////////////////////////////////////////////////////
// ��ð��ϵͳָ��
// @param ������������󣬿��Ա�֤�ں���ִ�й�����m_ptr��ֵ�����ͷ�
//////////////////////////////////////////////////////////////////////////////////////////////////
class ClanHelper
{
public:
	SERVICE_PTR m_pContainer;
	IClanSceneService *m_ptr;

	ClanHelper() : m_pContainer(0),m_ptr(0)
	{
		IClanScene *pClanScene = gServerGlobal->getClanScene();
		if (pClanScene == 0)
		{
			return;
		}

		m_pContainer = pClanScene->getClanSceneService();
		if (m_pContainer == 0)
		{
			return;
		}

		m_ptr = (IClanSceneService*)m_pContainer->query_interface();
	}
};
