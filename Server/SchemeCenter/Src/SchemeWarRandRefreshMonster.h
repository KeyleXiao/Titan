/*******************************************************************
** �ļ���:	SchemeWarRandRefreshMonster.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/11/25
** ��  ��:	1.0
** ��  ��:	
            ս�����ˢ�ֽű�
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeDef.h"
#include <map>

using namespace rkt;

// ϵͳ��ʾ����
class CSchemeWarRandRefreshMons: public ISchemeWarRandRefreshMons, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarRandRefreshMons>
{
	// <configWarID,�������ˢ���б�>
	/*typedef std::map<int, vector<SWarRandRefreshMons>> RANDREFRESHMONSLIST_MAP;
	typedef RANDREFRESHMONSLIST_MAP::iterator RANDREFRESHMONSLIST_MAP_ITER;*/

	// <����������ID,�������ˢ���б�>
	typedef std::map<int, SWarRandRefreshMons> RANDREFRESHMONS_MAP;
	typedef RANDREFRESHMONS_MAP::iterator RANDREFRESHMONS_MAP_ITER;

public:
	CSchemeWarRandRefreshMons(void);
	virtual ~CSchemeWarRandRefreshMons(void);

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


	//////////////////////////////////////////ISchemeWarRandRefreshMons///////////////////////
	// ��ȡ���ˢ����Ϣ
	virtual vector<SWarRandRefreshMons> getRandRefreshMonsList(int nConfigWarID);

	// ��ȡ���ˢ����Ϣ
	virtual SWarRandRefreshMons* getRandRefreshMons(int nMonsterIndex);
public:

private:
	//RANDREFRESHMONSLIST_MAP m_mapListScheme;

	RANDREFRESHMONS_MAP m_mapScheme;
};