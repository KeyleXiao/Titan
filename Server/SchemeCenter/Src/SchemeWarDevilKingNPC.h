/*******************************************************************
** �ļ���:	SchemeWarDevilKingNPC.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2015 - All Rights Reserved
** ������:	���к�
** ��  ��:	2017/11/30
** ��  ��:	1.0
** ��  ��:	
            ս��ħ������NPC����
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
class CSchemeWarDevilKingNPC: public ISchemeWarDevilKingNPC, public ISchemeUpdateSink, public CSchemeCache<CSchemeWarDevilKingNPC>
{

public:
	CSchemeWarDevilKingNPC(void);
	virtual ~CSchemeWarDevilKingNPC(void);

	// ����ű�
	bool LoadScheme(void);

	// �ر�
	void Close(void);

    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
	// CSV��������ʱ֪ͨ
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	// ���ö�̬����ʱ֪ͨ
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	//////////////////////////////////////////ISchemeWarDevilKing///////////////////////
	virtual SWarDevilKingNPC* getWarDevilKingNPC(int nHeroID) override;

public:

private:
	// <Ӣ��ID,����>
	std::map<int,SWarDevilKingNPC> m_mapScheme;
};