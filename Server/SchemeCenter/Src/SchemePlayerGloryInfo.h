/*******************************************************************
** �ļ���:	SchemePlayerGloryInfo.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2016/8/3
** ��  ��:	1.0
** ��  ��:	

********************************************************************/
#pragma once
#include "SchemeDef.h"
#include "SchemeCache.h"
#include "ISchemeCenter.h"

class CSchemePlayerGloryInfo:public ISchemePlayerGloryInfo,public ISchemeUpdateSink, public CSchemeCache<CSchemePlayerGloryInfo>
{
public:
    /////////////////////////////////ISchemePlayerGloryInfo/////////////////////////////////////////

    virtual std::map<int, SGlorySchemeInfo>* getGlorySchemeInfo();

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
    virtual bool            OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool            OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);
    /////////////////////////////////CSchemePlayerGloryInfo/////////////////////////////////////////
    bool											LoadScheme(void);
    void											Close(void);

protected:
    // ������óƺŵ�����
    void parseConditionInfo(char* pValue, std::map<int, long>& item);

private:
    // �ƺ����ñ���Ϣ
    std::map<int, SGlorySchemeInfo>      m_mapGloryInfo;
};