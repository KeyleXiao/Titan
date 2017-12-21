/*******************************************************************
** 文件名:	IAntiAddictGlobal.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	11/7/2016
** 版  本:	1.0
** 描  述:	防沉迷接口

********************************************************************/


#ifndef __I_ANTI_ADDICT_GLOBAL_H__
#define __I_ANTI_ADDICT_GLOBAL_H__

#include "AntiAddictDef.h"

// 防沉迷世界服接口
struct IAntiAddictGlobal
{	   
	// 释放对象
	virtual void Release(void) = NULL;
};

// 输出函数
API_EXPORT IAntiAddictGlobal* CreateAntiAddictGlobal(void);

#endif // __I_ANTI_ADDICT_GLOBAL_H__