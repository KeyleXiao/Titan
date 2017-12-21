/*******************************************************************
** 文件名: ITalentPageManager.h 
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李有红
** 日  期: 2017/8/16
** 版  本: 1.0
** 描  述: 天赋页管理接口

********************************************************************/
#ifndef __I_TALENTPAGEMANAGER_H__
#define __I_TALENTPAGEMANAGER_H__

class ITalentPageManager
{
	// 获取当前天赋页
	virtual int GetCurTalentPage() = 0;
};

#endif // __I_TALENTPAGEMANAGER_H__