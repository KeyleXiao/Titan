/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\SchemeCenter\Src\SchemeRuneConfig.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����ΰ
** ��  ��:	14/3/2016	18:36
** ��  ��:	1.0
** ��  ��:	
���߻����������ļ�
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "SchemeCache.h"

#ifdef FUNCTION_RUNE_OPEN

class CSchemeRuneConfig : public ISchemeRuneConfig,
    public ISchemeUpdateSink,
    public CSchemeCache<CSchemeRuneConfig>
{
    // <����ID, struct Rune>
    typedef std::map<int, EntityHeroRuneStore> T_RuneConfigInfo;
public:
    ///////////////////////////////IRuneInfo///////////////////	
    // ��ȡָ����ҷ��ĵ���������
    virtual void GetRuneInfoByID(DWORD dPlayerID,
        // <RUNE_INFO, �����ͷ�������>
        std::map<int, EntityHeroRuneStore> &mPlayerRuneConfig);

    ///////////////////////////////ISchemeUpdateSink///////////////////
    /** CSV��������ʱ֪ͨ
    @param   type   : ��ȡ������
    : �����READER_CSV : �Ϳ��԰�pReaderת����ICSVReader*
    : READER_XML : �Ϳ��԰�pReaderת����TiXmlDocument*
    @param   pReader����ȡCSV�ķ��ؽӿ�
    @param   szFileName�������ļ���
    @warning ��Ҫ�ڴ˷����ͷ�pCSVReader����pTiXmlDocument,��Ϊһ���ļ���Ӧ���sink
    @retval buffer
    */
    virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);

    /** ���ö�̬����ʱ֪ͨ
    @param   type   : ��ȡ������
    : �����READER_CSV : �Ϳ��԰�pReaderת����ICSVReader*
    : READER_XML : �Ϳ��԰�pReaderת����TiXmlDocument*
    @param   pReader����ȡCSV�ķ��ؽӿ�
    @param   szFileName�������ļ���
    @warning ��Ҫ�ڴ˷����ͷ�pCSVReader����pTiXmlDocument,��Ϊһ���ļ���Ӧ���sink
    @retval buffer
    */
    virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    ///////////////////////////////CSchemePathFindServiceInfo//////////////////////
    /** ����ű�
    @param   
    @param   
    @return  
    */
    bool LoadScheme(void);

    /** �ر�
    @param   
    @param   
    @return  
    */
    void Close(void);

    /** 
    @param   
    @param   
    @return  
    */
    CSchemeRuneConfig(void);

    /** 
    @param   
    @param   
    @return  
    */
    virtual ~CSchemeRuneConfig(void);

private:
    T_RuneConfigInfo m_SchemeInfo;
};

#endif