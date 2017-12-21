/*******************************************************************
** 文件名:	SchemeAIBuyItem.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:	3/28/2015
** 版  本:	1.0
** 描  述:	

			AI购买物品配置
********************************************************************/
#pragma once
#include "Singleton.h"
#include "ISchemeEngine.h"
#include "SchemeCache.h"
#include "ISchemeCenter.h"
#include <map>

using namespace rkt;

class CAIBuyItemScheme : public ISchemeAIBuyItem, public ISchemeUpdateSink, public CSchemeCache<CAIBuyItemScheme>
{
public:
    virtual bool Load();

    virtual SBuyScheme* getBuyScheme(int nAISchemeID); 

    virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    void Close();

private:
    bool loadBuyItemScheme(ICSVReader* pCSVReader);


    std::map<int, SBuyScheme> m_mapBuyScheme;
};