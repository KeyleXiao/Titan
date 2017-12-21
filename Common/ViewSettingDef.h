/*******************************************************************
** 文件名:	ViewSettingDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/11/25
** 版  本:	1.0
** 描  述:	观战服务所用到的Setting定义
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once


struct DefaultPort
{
	static const WORD MNG_FOR_GATE	= 14850;	// 管理服为Gate所开的端口
	static const WORD MNG_FOR_VIEW	= 14851;	// 管理服为View所开的端口
	static const WORD GATE_FOR_VIEW = 15900;	// Gate  为View所开的端口
};