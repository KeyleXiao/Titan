/*******************************************************************
** 文件名: E:\speed\Server\LegendCupServer\Src\LegendCupScene.h
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/19/2015
** 版  本:	1.0
** 描  述:	排行榜系统场景服
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
    // 执行释放Service
    virtual void            onStop();
	virtual void			release();
	virtual SERVICE_PTR		getLegendCupSceneService();

private:
	/** 取得IMatchSceneService接口指针
	@return  
	*/
	ILegendCupSceneService*		getILegendCupSceneService();

private:
	SERVICE_PTR m_LegendCupSceneServices;

private:
	bool createLegendCupSceneService();

};