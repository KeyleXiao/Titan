/*******************************************************************
** �ļ���:	SchemeWarPersonConfig.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	3/10/2016
** ��  ��:	1.0
** ��  ��:	ʵ��ͨ������
********************************************************************/

#pragma once
#include "ISchemeCenter.h"
#include "ISchemeEngine.h"
#include "WarDef.h"

// ��������������ñ�
#define WAR_SCHEME_PERSON_CONFIG				"Scp\\WarPersonConfig.xml"

class CSchemeWarPersonConfig:public ISchemeUpdateSink, public ISchemeWarPersonConfig
{
public:
    /////////////////////////ISchemeUpdateSink/////////////////////////
    virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
    virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////ISchemeWarPersonConfig/////////////////////
    virtual SWarPersonConfig* CSchemeWarPersonConfig::GetSchemeWarPersonConfigInfo();
	virtual const SchemeWarPersonConfigInfo* GetSchemeWarPersonConfigInfo(int WarID);

    /////////////////////////CSchemeWarPersonConfig////////////////////
    bool                    LoadScheme(void);
    void					Close(void);

    CSchemeWarPersonConfig(void);

	virtual ~CSchemeWarPersonConfig(void);

private:
	// ����ս������
	bool loadWarData(TiXmlDocument *pXMLReader);

	// ������������
	bool loadOtherData(TiXmlDocument *pXMLReader);

private:

    SWarPersonConfig m_warPersonConfig;

	std::map<int, SchemeWarPersonConfigInfo> m_mapSchemeWarConfig;
};