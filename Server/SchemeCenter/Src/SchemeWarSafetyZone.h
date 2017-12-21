/*******************************************************************
** �ļ���:	CSchemeWarSafetyZone.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/11/1
** ��  ��:	1.0
** ��  ��:	
			����ɱ-��ȫ��/����������
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeDef.h"
#include "SchemeWarDef.h"
#include "SchemeCache.h"
#include <map>

using namespace rkt;

// ϵͳ��ʾ����
class CSchemeWarSafetyZone: public ISchemeWarSafetyZone, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarSafetyZone>
{
	typedef std::map< int, SWarSafetyZone> WAR_SAFETYZONE_MAP;
	typedef WAR_SAFETYZONE_MAP::iterator WAR_SAFETYZONE_MAP_ITER;

public:
	CSchemeWarSafetyZone(void);
	virtual ~CSchemeWarSafetyZone(void);

	// ����ű�
	bool LoadScheme(void);

	// �ر�
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV��������ʱ֪ͨ
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// ���ö�̬����ʱ֪ͨ
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarSafetyZone///////////////////////
	// ��ȡ��ȫ����Ϣ
	virtual SWarSafetyZone* getWarSafetyZone(int nRound);

public:

private:
	WAR_SAFETYZONE_MAP     m_mapScheme;
};