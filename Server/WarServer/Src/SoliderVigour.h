/*******************************************************************
** 文件名: SoliderVigour.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/3/24
** 版  本: 1.0
** 描  述: 士兵气势
** 应  用:
**************************** 修改记录 ******************************
** 修改人:
** 日  期:
** 描  述:
********************************************************************/

#pragma once

#include "ISoliderVigourMgr.h"

class IWar;

struct SPathTypeVigour
{
    int nAbsoluteVigour;
    int nRelativeVigour;
    SPathTypeVigour()
    {
        memset( this, 0, sizeof(*this));
    }
};

class CSoliderVigour : public ISoliderVigourMgr
{
public:
	CSoliderVigour(IWar* pWar);
	~CSoliderVigour();

public:
	////////////////////////ISoliderVigourMgr start/////////////////////////
	virtual bool Init(int nMatchType);

	virtual void Release();

	// 战场正式开始
	virtual void onWarRealStart();

	// 战场结束
	virtual void onWarEnd();

	// 获得阵营路径绝对气势
	virtual int getCampPathTypeRelativeVigour(int nCamp, EWarSoliderPathType eType);

	// 增加阵营路径气势（相对和绝对气势都处理下）
	virtual bool addCampPathTypeVigour(int nCamp, EWarSoliderPathType eType, int nVigourValue);

	////////////////////////ISoliderVigourMgr end/////////////////////////

private:
	// 获得阵营路径相对气势
	int getCampPathTypeAbsoluteVigour(int nCamp, EWarSoliderPathType eType);

private:

	// 战场
	IWar*	m_pWar;
	
	// 士兵气势
	map<int, map<EWarSoliderPathType, SPathTypeVigour>>   m_mapSoliderVigour;
};