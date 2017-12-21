/*******************************************************************
** 文件名:	e:\speed\Client\Game\Src\Stage_MatchRoom.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/23/2015	11:48
** 版  本:	1.0
** 描  述:	
            比赛大厅 房间
********************************************************************
            这个状态下的数据移动到
********************************************************************/

#pragma once


#include "stdafx.h"
#include "Stage_Base.h"
#include "IMessageDispatch.h"
#include "MatchDef.h"
#include "EntityViewDef.h"
#include "StateManage.h"


struct SSchemeMatchRoom;

class Stage_MatchRoom : public Stage_Base
{

public:
	/////////////////////////////Stage_Base///////////////////////////////////////////////////////////////
	virtual void		onEnter( void * context,int len );
	virtual void		update();
	virtual void		onLeave();

public:
	Stage_MatchRoom();

private:

};