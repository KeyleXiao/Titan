/*******************************************************************
** �ļ���:	CSchemeWarSubFuncSetting.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/4/13
** ��  ��:	1.0
** ��  ��:	
            ս���ӹ������ýű�
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeDef.h"
#include "SchemeWarDef.h"
#include <map>

using namespace rkt;

// ϵͳ��ʾ����
class CSchemeWarSubFunc: public ISchemeWarSubFunc, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarSubFunc>
{
	typedef std::map< int, SWarSubFunc> WAR_SUBFUNC_MAP;
	typedef WAR_SUBFUNC_MAP::iterator WAR_SUBFUNC_MAP_ITER;

    // �����ļ���Ӧ����
    enum SCHEME_COLUMN
    {
        COLNUM_MatchType,			// ��������
		COLNUM_XPSkill_OnOff,		// xp���ܿ���
		COLNUM_Support_OnOff,		// ���޿���
		COLNUM_SaveOftenEuip_OnOff,	// ���泣��װ��
		COLNUM_AIEcmCtrl_OnOff,		// AI���ÿ��ƿ���
		COLNUM_AIExpCtrl_OnOff,		// AI������ƿ���
    };

public:
	CSchemeWarSubFunc(void);
	virtual ~CSchemeWarSubFunc(void);

	// ����ű�
	bool LoadScheme(void);

	// �ر�
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV��������ʱ֪ͨ
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// ���ö�̬����ʱ֪ͨ
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarSubFuncSetting///////////////////////
	// �Ƿ�����ĳ���
	virtual bool isEnableSubFunc(int nMatchType, int nSubFuncType);

public:

private:
	WAR_SUBFUNC_MAP     m_mapScheme;
};