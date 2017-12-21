#include "StdAfx.h"
#include "SchemeSlotMapedShortcut.h"
#include "IServerGlobal.h"

bool CSchemeSlotMapedShortcut::OnSchemeLoad(SCRIPT_READER reader,const char* szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if ( pCSVReader==0 || reader.type!=READER_CSV)
        return false;

    // ���
    m_AllSlotMapedShortcut.clear();

    int nLen = 0;
    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow=0; nRow<nRecordCount; ++nRow)
    {
        SSlotShortcutScheme item;
        ZeroMemory(&item,sizeof(SSlotShortcutScheme));
        int nCol = 0;
        // ��λ����
        item.nSlotIndex		= pCSVReader->GetInt(nRow, nCol++, 0);

        // ��λ����
        item.nSlotType		=  pCSVReader->GetInt(nRow, nCol++, 0);

        // ��λ����
        int nlen = sizeof(item.szKeyName);
        pCSVReader->GetString(nRow, nCol++, item.szKeyName, nlen);

        // ��������
        item.nAirCons		=  pCSVReader->GetInt(nRow, nCol++, 0);

        //��������
        item.nSkillType     =  pCSVReader->GetInt(nRow, nCol++, 0);

        T_MAP_SLOT_TYPE_MAPED_INFO::iterator iter = m_AllSlotMapedShortcut.find(item.nSlotType);
        if (iter == m_AllSlotMapedShortcut.end())
        {
            T_MAP_SLOT_INDEX_MAPED_INFO tmpSlotIndexMapedInfo;
            tmpSlotIndexMapedInfo.insert(pair<int,SSlotShortcutScheme>(item.nSlotIndex, item));
            m_AllSlotMapedShortcut.insert(pair<int, T_MAP_SLOT_INDEX_MAPED_INFO>(item.nSlotType, tmpSlotIndexMapedInfo));
        }
        else
        {
            iter->second.insert(pair<int,SSlotShortcutScheme>(item.nSlotIndex, item));
        }
    }

    return true;
}

bool CSchemeSlotMapedShortcut::OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName)
{
    return true;
}

int CSchemeSlotMapedShortcut::getSlotAccordSkillType( int nSkillType )
{
    T_MAP_SLOT_TYPE_MAPED_INFO::iterator iter = m_AllSlotMapedShortcut.begin();
    for (; iter != m_AllSlotMapedShortcut.end(); ++iter)
    {

        T_MAP_SLOT_INDEX_MAPED_INFO::iterator iterIndex = iter->second.begin();

        for (; iterIndex != iter->second.end(); ++iterIndex )
        {

            SSlotShortcutScheme slotInfo = iterIndex->second;
            if ( slotInfo.nSkillType == nSkillType )
            {
                return slotInfo.nSlotIndex;
            }
        }
    }
    return -1;
}

bool CSchemeSlotMapedShortcut::LoadScheme(void)
{
    ISchemeEngine* pSchemeEngine = gServerGlobal->getSchemeEngine();
    if ( pSchemeEngine == NULL )
    {
        return false;
    }

    // ����PersonModel.csv�ű�
    string stringPath = SCP_PATH_FORMAT( SLOT_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this);
    if(!bResult)
    {
        Error("config file load failed! filename=" << stringPath.c_str() << endl);
        return false;
    }
    AfterSchemeLoaded();
    return true;
}

void CSchemeSlotMapedShortcut::Close(void)
{
    m_AllSlotMapedShortcut.clear();
}