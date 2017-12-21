/*******************************************************************
** 文件名:	CommonManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/3/20
** 版  本:	1.0
** 描  述:

********************************************************************/

#pragma once
#include "IEntity.h"
#include "IBankPart.h"
#include "EntityDef.h"
#include <list>

typedef DBREQ_TABLE_LABEL LabelInfo;

class CCommonManager
{
public:
	CCommonManager();
	~CCommonManager();

public:
	bool onSerialize(rkt::ibuffer& in, int flag);

	bool deSerialize(rkt::obuf& out, int flag);

	bool onLoad(IEntity* pEntity);

	void restore();

public:
	void onMessage(int nMsgID, LPCSTR pszMsg, int nLen);


	IEntity *		                   m_pMaster;
};
