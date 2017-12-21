/*******************************************************************
** 文件名:	SchemeSlotMapedShortcut.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	马传飞
** 日  期:	2015/12/8
** 版  本:	1.0
** 描  述:	快捷键映射

			
********************************************************************/
#pragma once
#include "SchemeDef.h"
#include "SchemeCache.h"
#include "ISchemeCenter.h"
using namespace std;
// 定义
class CSchemeSlotMapedShortcut : public ISchemeSlotMapedShortcut, public ISchemeUpdateSink, public CSchemeCache<CSchemeSlotMapedShortcut>
{
public:
    ///////////////////////////////ISchemeUpdateSink///////////////////////////////////////////
    virtual bool            OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool            OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    ///////////////////////////////ISchemeSlotMapedShortcut///////////////////////////////////////////
    virtual int             getSlotAccordSkillType( int nSkillType );

    //////////////////////////////////////////////////////////////////////////
    bool                    LoadScheme(void);

    void                    Close(void);

private:
    // 索引对应的槽位快捷键信息
    typedef map<int,SSlotShortcutScheme> T_MAP_SLOT_INDEX_MAPED_INFO;
    typedef map<int,T_MAP_SLOT_INDEX_MAPED_INFO> T_MAP_SLOT_TYPE_MAPED_INFO;
    T_MAP_SLOT_TYPE_MAPED_INFO m_AllSlotMapedShortcut;
};