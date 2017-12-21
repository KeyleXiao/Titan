/*******************************************************************
** �ļ���:	SchemeSlotMapedShortcut.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2015/12/8
** ��  ��:	1.0
** ��  ��:	��ݼ�ӳ��

			
********************************************************************/
#pragma once
#include "SchemeDef.h"
#include "SchemeCache.h"
#include "ISchemeCenter.h"
using namespace std;
// ����
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
    // ������Ӧ�Ĳ�λ��ݼ���Ϣ
    typedef map<int,SSlotShortcutScheme> T_MAP_SLOT_INDEX_MAPED_INFO;
    typedef map<int,T_MAP_SLOT_INDEX_MAPED_INFO> T_MAP_SLOT_TYPE_MAPED_INFO;
    T_MAP_SLOT_TYPE_MAPED_INFO m_AllSlotMapedShortcut;
};