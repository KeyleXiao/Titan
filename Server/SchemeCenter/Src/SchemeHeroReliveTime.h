/*******************************************************************
** �ļ���:	SchemeHeroReliveTime.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/5/10
** ��  ��:	1.0
** ��  ��:	
            Ӣ�۸���ʱ��
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
class CSchemeHeroReliveTime: public ISchemeHeroReliveTime, public ISchemeUpdateSink, public CSchemeCache<CSchemeHeroReliveTime>
{
	typedef std::map<int, std::map<int, int> > HERO_RELIVETIME_MAP;
	typedef HERO_RELIVETIME_MAP::iterator HERO_RELIVETIME_MAP_ITER;

    // �����ļ���Ӧ����
    enum SCHEME_COLUMN
    {
		COLNUM_ConfigWarID,		// ��������
        COLNUM_Level,			// �ȼ�
        COLNUM_ReliveTime,		// ����ʱ��
    };

public:
	CSchemeHeroReliveTime(void);
	virtual ~CSchemeHeroReliveTime(void);

	// ����ű�
	bool LoadScheme(void);

	// �ر�
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV��������ʱ֪ͨ
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// ���ö�̬����ʱ֪ͨ
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeReliveTime///////////////////////
	// ��ȡ����ʱ��
	virtual int getHeroReliveTime(int nMatchTypeID, int nLevel);

private:
	HERO_RELIVETIME_MAP     m_mapScheme;
};