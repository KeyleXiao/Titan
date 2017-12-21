/*******************************************************************
** �ļ���:	CSchemeWarForceEnd.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/3/28
** ��  ��:	1.0
** ��  ��:	
            ս��ǿ�ƽű�
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeDef.h"
#include <map>

using namespace rkt;

// ϵͳ��ʾ����
class CSchemeWarForceEnd: public ISchemeWarForceEnd, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarForceEnd>
{
	typedef std::map< int, SWarForceEndInfo > WAR_FORCEEND_MAP;
	typedef WAR_FORCEEND_MAP::iterator WAR_FORCEEND_MAP_ITER;

    // �����ļ���Ӧ����
    enum SCHEME_COLUMN
    {
        COLNUM_MatchType,		// ��������
        COLNUM_IsEnable,		// �Ƿ���
        COLNUM_Time,			// �����¼����룩
        COLNUM_KillDiffWeight,	// ��ͷ��Ȩ��
        COLNUM_EcmDiffWeight,	// ���ò�Ȩ��
        COLNUM_TowerDiffWeight, // ������Ȩ��
    };

public:
	CSchemeWarForceEnd(void);
	virtual ~CSchemeWarForceEnd(void);

    /////////////////////////////////CSchemeChatCD/////////////////////////////////////////
	// ����ű�
	bool LoadScheme(void);

	// �ر�
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV��������ʱ֪ͨ
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// ���ö�̬����ʱ֪ͨ
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarForceEnd///////////////////////
	SWarForceEndInfo* getWarForceEndInfo(int nMatchType);

	// �Ƿ�����ǿ�ƽ���
	virtual bool isEnableForceEnd(int nMatchType);
public:

private:
	WAR_FORCEEND_MAP     m_mapScheme;
};