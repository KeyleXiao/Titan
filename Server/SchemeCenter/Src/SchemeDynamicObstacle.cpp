#include "stdafx.h"
#include "SchemeDynamicObstacle.h"
#include "IServerGlobal.h"
#include <string>

/** 
@param   
@param   
@return  
*/
CSchemeDynamicObstacle::CSchemeDynamicObstacle(void)
{
	// ���е�ͼ��������	
	m_mapSchemeInfo.clear();
}

/** 
@param   
@param   
@return  
*/
CSchemeDynamicObstacle::~CSchemeDynamicObstacle(void)
{
	// ���е�ͼ��������	
	m_mapSchemeInfo.clear();
}

/** 
@param   
@param   
@return  
*/
bool CSchemeDynamicObstacle::LoadScheme(void)
{
	ISchemeEngine * pSchemeEngine = gServerGlobal->getSchemeEngine();
	if(pSchemeEngine == NULL )
	{
		return false;
	}

    string stringPath = SCP_PATH_FORMAT( DYNAMICOBSTACLE_SCHEME_FILENAME );
    bool bResult = pSchemeEngine->LoadScheme(stringPath.c_str(), (ISchemeUpdateSink *)this, false);
	if(!bResult)
	{
		ErrorLn("config file load failed! filename=" << stringPath.c_str());
		return false;
	}

	return true;
}

/** �ر�
@param   
@param   
@return  
*/
void CSchemeDynamicObstacle::Close(void)
{	
	// ���е�ͼ��������	
	m_mapSchemeInfo.clear();
}

/** ȡ�õ�ͼ������Ϣ
@param   
@param   
@return  
*/
DynamicObstacleScheme * CSchemeDynamicObstacle::GetSchemeInfo(int nDynamicMonsterID)
{
	T_SchemeInfo::iterator itM = m_mapSchemeInfo.find( nDynamicMonsterID );
	if(itM != m_mapSchemeInfo.end())
	{
		return &((*itM).second);
	}

	return NULL;
}

/** CSV��������ʱ֪ͨ
@param   type   : ��ȡ������
		        : �����READER_CSV : �Ϳ��԰�pReaderת����ICSVReader*
				: READER_XML : �Ϳ��԰�pReaderת����TiXmlDocument*
@param   pReader����ȡCSV�ķ��ؽӿ�
@param   szFileName�������ļ���
@warning ��Ҫ�ڴ˷����ͷ�pCSVReader����pTiXmlDocument,��Ϊһ���ļ���Ӧ���sink
@retval buffer 
*/ 
bool CSchemeDynamicObstacle::OnSchemeLoad(SCRIPT_READER reader, LPCSTR szFileName)
{
    ICSVReader * pCSVReader = reader.pCSVReader;
    if(pCSVReader == NULL || szFileName == NULL || reader.type!=READER_CSV)
    {
        return false;
    }

    // ���
    m_mapSchemeInfo.clear();

    int nLen = 0;
    // ��ȡ
    int nRecordCount = pCSVReader->GetRecordCount();
    for (int nRow=0; nRow<nRecordCount; ++nRow)
    {
        DynamicObstacleScheme item;
        ZeroMemory(&item,sizeof(DynamicObstacleScheme));

        // ��̬�ϰ���ID
        item.nDynamicMonsterID = pCSVReader->GetInt(nRow, OBS_COL_ID, 0);
        // ��״����
        item.nShape = pCSVReader->GetInt(nRow, OBS_COL_SHAPE, 0);
        // ������ı������ǳ��ȣ���������������������ε��ڽ�Բ�뾶
        item.fLength = pCSVReader->GetFloat(nRow, OBS_COL_LENGTH, 0);
        // ������ı������ǿ�ȣ���������������������ε��ڽ�Բ�뾶,�������ֵһ��
        item.fWidth = pCSVReader->GetFloat(nRow, OBS_COL_WIDTH, 0);
        // ��̬�ϰ�����ת�Ƕ�
        item.fAngle = pCSVReader->GetFloat(nRow, OBS_COL_ANGLE, 0);
        // ��̬�ϰ���ʵ���Ƿ�ɴ�Խ��־λ
        item.nEntityCampFlag = pCSVReader->GetInt(nRow, OBS_COL_ENTITY_CAMP_FLAG, 0);
        // ��̬�ϰ�������Ӫ��־λ
        item.nSpellCampFlag = pCSVReader->GetInt(nRow, OBS_COL_SPELL_CAMP_FLAG, 0);
        // ��̬�ϰ������Ƿ�ɴ�Խ��־λ
        item.nSpellFlag = pCSVReader->GetInt(nRow, OBS_COL_SPELL_FLAG, 0);
		// ��̬�ϰ��Ƿ�Ԥ���أ������ڿͻ��˴���ǰ���ɣ�
		item.nPreloadFlag = pCSVReader->GetInt(nRow, OBS_COL_PRELOAD_FLAG, 0);
		

        m_mapSchemeInfo.insert(make_pair<unsigned int, DynamicObstacleScheme>(item.nDynamicMonsterID, item));
    }

	AfterSchemeLoaded();

	return true;
}

/** ���ö�̬����ʱ֪ͨ
@param   type   : ��ȡ������
				: �����READER_CSV : �Ϳ��԰�pReaderת����ICSVReader*
				: READER_XML : �Ϳ��԰�pReaderת����TiXmlDocument*
@param   pReader����ȡCSV�ķ��ؽӿ�
@param   szFileName�������ļ���
@warning ��Ҫ�ڴ˷����ͷ�pCSVReader����pTiXmlDocument,��Ϊһ���ļ���Ӧ���sink
@retval buffer 
*/
bool CSchemeDynamicObstacle::OnSchemeUpdate(SCRIPT_READER reader, LPCSTR szFileName)
{
	return false;
}