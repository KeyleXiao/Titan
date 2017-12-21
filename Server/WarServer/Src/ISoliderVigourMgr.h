/*******************************************************************
** 文件名: ISoliderVigourMgr.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/5
** 版  本: 1.0
** 描  述: 战场士气接口
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  
** 日  期: 
** 描  述:  
********************************************************************/
#pragma once

#include "IWarMgr.h"

class ISoliderVigourMgr : public __IWarMgr
{
public:
	// 获得阵营路径绝对气势
	virtual int getCampPathTypeRelativeVigour(int nCamp, EWarSoliderPathType eType) = 0;

	// 增加阵营路径气势（相对和绝对气势都处理下）
	virtual bool addCampPathTypeVigour(int nCamp, EWarSoliderPathType eType, int nVigourValue) = 0;
};