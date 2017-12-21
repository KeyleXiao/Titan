/*******************************************************************
** 文件名:	ICommonPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李有红
** 日  期:	2017/1/22
** 版  本:	1.0
** 描  述:	通用部件
** 应  用:  	
	
*******************************************************************/

#pragma once

#include "IEntity.h"

// 通用部件
struct ICommonPart: public IEntityPart
{
	virtual void onMeetNPC(UID uidNPC) = 0;
};