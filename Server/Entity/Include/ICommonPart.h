/*******************************************************************
** 文件名:	ICommonPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/1/22
** 版  本:	1.0
** 描  述:	

********************************************************************/

#pragma once
#include "IEntity.h"

struct __ICommonPart:public __IEntityPart
{
    /*@name 通知客户端数据
    @param nNotifyType: 通知数据类型NOTICE_DATA_TYPE
    @param nValue:      数值内容
    @param pText:       文本内容
    @return 
    */
    virtual bool notifyClientData(int nNotifyType, int nValue, char* pText=0) = 0;

    /*@name 保存引导步骤
    @param nStep: 引导步骤
    @return 
    */
    virtual bool saveGuideStep(int nStep) = 0; 

    /*@name 获取实体自身当前充能值
    @return 
    */
    virtual int getChargeValue(void) = 0;

    /*@name 设置实体自身当前充能值
    @param nValue: 充能数值
    @return 
    */
    virtual void setChargeValue(int nValue) = 0;
};

//////////////////////////////////////////////////////////////////////////
#if defined(_LIB) || defined(ENTITY_STATIC_LIB)		/// 静态库版本
#	pragma comment(lib, MAKE_LIB_NAME(ENTITY))
extern "C" __ICommonPart * CreateCommonPart();
#	define	CreateCommonPartProc	CreateCommonPart
#else													/// 动态库版本
typedef __ICommonPart * (RKT_CDECL * procCreateCommonPart)();
#	define	CreateCommonPartProc	DllApi<procCreateCommonPart>::load(MAKE_DLL_NAME(ENTITY), "CreateCommonPart")
#endif
