/*******************************************************************
** 文件名:	MiscManager.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-12-9
** 版  本:	1.0
** 描  述:	辅助全局对象的一个杂项管理器

********************************************************************/


#ifndef __MISC_MANAGER_H__
#define __MISC_MANAGER_H__


#include "IMiscManager.h"


// 辅助全局对象的一个杂项管理器
class CMiscManager : public IMiscManager
{
public:
    ////////////////// IMiscManager ////////////////////////////////////////////////////////

	/* 用Service配置名称创建Service
    @param pszServiceSchemeName: Service配置名称
    */
	virtual SERVICE_PTR create_service(const char * pszServiceSchemeName);

	////////////////// CMiscManager ////////////////////////////////////////////////////////

    // 构造函数
	CMiscManager();

    // 析构函数
	virtual ~CMiscManager();
};



#endif //__MISC_MANAGER_H__ 

