/*******************************************************************
** �ļ���:	CSchemeAIEcmExpCtrl.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/10/28
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
class CSchemeAIEcmExpCtrl: public ISchemeAIEcmExpCtrl, public ISchemeUpdateSink, public CSchemeCache<CSchemeAIEcmExpCtrl>
{
	// AI���ÿ���<configWarID,<lineID,�����б�>>
	typedef std::map< int,map<int, list<SAIEcmCtrlConfig_S>> > ECM_CTRL_MAP;
	typedef ECM_CTRL_MAP::iterator ECM_CTRL_MAP_ITER;

	// AI�������<configWarID,<lineID,�����б�>>
	typedef std::map< int,map<int, list<SAIExpCtrlConfig_S>> > EXP_CTRL_MAP;
	typedef EXP_CTRL_MAP::iterator EXP_CTRL_MAP_ITER;

    // �����ļ���Ӧ����
    enum SCHEME_COLUMN
    {
        COLNUM_CONFIG_WAR_ID = 0,// ս��ID�����������ļ���
		COLNUM_MATCHSCORE_RANGE,
        COLNUM_ID,				// ���ò�����ID
		COLNUM_A_RATIO,			// aϵ��
        COLNUM_B_RATIO,			// bϵ��
        COLNUM_SOLDIER,			// ʿ���ӳ�
        COLNUM_MONSTER,			// Ұ�ּӳ�
        COLNUM_SALARY,			// ���ʼӳ�
        COLNUM_HERO,			// Ӣ�ۼӳ�
    };

public:
	CSchemeAIEcmExpCtrl(void);
	virtual ~CSchemeAIEcmExpCtrl(void);

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

private:
	// AI���ÿ���
	bool LoadAIEcmCtrlScheme(SCRIPT_READER reader);
	// AI�������
	bool LoadAIExpCtrlScheme(SCRIPT_READER reader);

	//////////////////////////////////////////ISchemeAIEcmExpCtrl///////////////////////
	// ��ȡAI���ÿ�������
	virtual SAIEcmCtrlConfig_S* getAIEcmCtrl(int nConfigWarID, int nMatchScore, int nLineID);

	// ��ȡAI�����������
	virtual SAIExpCtrlConfig_S* getAIExpCtrl(int nConfigWarID, int nMatchScore, int nLineID);
public:

private:
	ECM_CTRL_MAP     m_mapEcmCtrlScheme;
	EXP_CTRL_MAP     m_mapExpCtrlScheme;
};