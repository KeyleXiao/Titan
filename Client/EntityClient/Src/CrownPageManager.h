#pragma once
#include "IGameDatabaseDef.h"
#include <map>
#include "IEntity.h"
#include "EntityViewDef.h"
//#define  DefaulCrownPageCount 2   //默认创建2页皇冠页。最多20页
class CCrownPageManager
{
public:
	CCrownPageManager();
	~CCrownPageManager();

public:
	bool onSerialize(rkt::ibuffer& in);

	bool deSerialize(rkt::obuf& out);

	bool onLoad( IEntity* pEntity);

	void onMsgCrownAllGemstoneItem(LPCSTR pszMsg, int nLen);

	void onMsgCrownAllCrownPage(LPCSTR pszMsg, int nLen);

	void onMsgCrownGemstoneProduceInfo(LPCSTR pszMsg,int nLen);

	void onMsgUpdateCrownGemstoneItem(LPCSTR pszMsg, int nLen);

	void onMsgAppraisalCrownGemstoneRet(LPCSTR pszMsg, int nLen);

	IEntity *		                            m_pMaster;
};
