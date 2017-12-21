/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\SchemeCenter\Src\SchemeRune.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����ΰ
** ��  ��:	14/3/2016	18:36
** ��  ��:	1.0
** ��  ��:	
            ���Ľű���ȡ
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "SchemeCache.h"

using namespace std;

#ifdef FUNCTION_RUNE_OPEN

class CSchemeRuneInfo : public ISchemeRuneInfo,
                        public ISchemeUpdateSink,
                        public CSchemeCache<CSchemeRuneInfo>
{
	typedef map<unsigned int, RuneInfoScheme>	T_MapRuneInfo;
public:
	///////////////////////////////IRuneInfo///////////////////	
    // ��ȡĳ��ָ�����ĵ���������
	virtual RuneInfoScheme * GetRuneSchemeInfo(int nRuneID);

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
	CSchemeRuneInfo(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeRuneInfo(void);

private:
    /** ��ȡ������ 
	@param   
	@param   
	@return  
	*/
	bool GetRuneProperty(ICSVReader* pCSVReader, size_t nRow, int nIndex, RuneInfoScheme& schemeinfo);

private:
	// ����Ѱ·������������	
	T_MapRuneInfo m_SchemeInfo;
};

#endif