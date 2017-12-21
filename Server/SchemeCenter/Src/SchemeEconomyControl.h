/*******************************************************************
** �ļ���:	CSchemeEconomyControl.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/2/23
** ��  ��:	1.0
** ��  ��:	
            ���ÿ��ƽű�
********************************************************************/


#pragma once
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "SchemeDef.h"
#include <map>

using namespace rkt;

// ϵͳ��ʾ����
class CSchemeEconomyControl: public ISchemeEconomyControl, public ISchemeUpdateSink, public CSchemeCache<CSchemeEconomyControl>
{
	typedef std::map< int,map<int, SEconomyControlConfig_S> > ECO_CTRL_MAP;
	typedef std::map< int,map<int, SEconomyControlConfig_S> >::iterator ECO_CTRL_MAP_ITER;

    // �����ļ���Ӧ����
    enum SCHEME_COLUMN
    {
        COLNUM_CONFIG_WAR_ID = 0,// ս��ID�����������ļ���
        COLNUM_ID,				// ���ò�����ID
		COLNUM_A_RATIO,			// aϵ��
        COLNUM_B_RATIO,			// bϵ��
        COLNUM_SOLDIER,			// ʿ���ӳ�
        COLNUM_MONSTER,			// Ұ�ּӳ�
        COLNUM_SALARY,			// ���ʼӳ�
        COLNUM_HERO,			// Ӣ�ۼӳ�
    };

public:
	CSchemeEconomyControl(void);
	virtual ~CSchemeEconomyControl(void);

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


	//////////////////////////////////////////ISchemeEconomyControl///////////////////////
	virtual SEconomyControlConfig_S* getEconomyCtrl(int nMapID, int nLineID);
public:

private:
    ECO_CTRL_MAP     m_mapScheme;
};