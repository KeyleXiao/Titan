/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\SchemeCenter\Src\SchemeRunePage.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����ΰ
** ��  ��:	14/3/2016	18:36
** ��  ��:	1.0
** ��  ��:	
            ���߻�����ҳ�����ļ�
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "SchemeCache.h"

#ifdef FUNCTION_RUNE_OPEN
class CSchemeRunePage : public ISchemeRunePage,
                        public ISchemeUpdateSink,
                        public CSchemeCache<CSchemeRunePage>
{
    // <����ID, struct Rune>
	typedef std::map<int, RUNE_INFO> T_MapRunePage;
public:
	///////////////////////////////IRuneInfo///////////////////	
    // ��ȡָ����ҷ��ĵ���������
	virtual void GetRunePageByID(DWORD dPlayerID,
                                 // <RUNE_INFO, �����ͷ�������>
                                 std::map<int, Entity_Rune_Page_Info> &pRunePage);

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
	CSchemeRunePage(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeRunePage(void);

private:
	T_MapRunePage m_SchemeInfo;
};

#endif