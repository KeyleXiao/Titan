/*******************************************************************
** �ļ���:	SchemeWarEvaluate.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2015/12/4
** ��  ��:	1.0
** ��  ��:	ս������������ñ�
********************************************************************/
#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeWarDef.h"

class CSchemeWarEvaluate:public ISchemeWarEvaluate, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarEvaluate>
{
public:
    CSchemeWarEvaluate();
    ~CSchemeWarEvaluate();

public:
    ///////////////////////////ISchemeUpdateSink///////////////////////////////////////////////
    virtual bool            OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool            OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    ///////////////////////////CSchemeWarEvaluate///////////////////////////////////////////////
    //����ű�
	bool LoadScheme(void);

	//�ر�
	void Close(void);

    ////////////////////////////ISchemeWarEvaluate//////////////////////////////////////////////
    virtual SWarEvaluathInfo* getEvaluathInfo(int nMatchTypeID = 1);

private:
    std::map<int, SWarEvaluathInfo> m_warEvaluateMap;
};
