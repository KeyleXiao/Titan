/*******************************************************************
** �ļ���: E:\speed\Server\LegendCupServer\Src\LegendCupScene.h
** ��  Ȩ: (C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	10/19/2015
** ��  ��:	1.0
** ��  ��:	���а�ϵͳ������
********************************************************************/

#pragma once

#include "IServiceContainer.h"
#include "LegendCupSceneService.h"
#include "ILegendCupScene.h"

class LegendCupScene:public ILegendCupScene
{
public:
	// ILegendCupScene
	//////////////////////////////////////////////////////////////////////////
	virtual bool			load();
    // ִ���ͷ�Service
    virtual void            onStop();
	virtual void			release();
	virtual SERVICE_PTR		getLegendCupSceneService();

private:
	/** ȡ��IMatchSceneService�ӿ�ָ��
	@return  
	*/
	ILegendCupSceneService*		getILegendCupSceneService();

private:
	SERVICE_PTR m_LegendCupSceneServices;

private:
	bool createLegendCupSceneService();

};