/*******************************************************************
** �ļ���:	ISchemeCenter.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	���к�
** ��  ��:	11/17/2016
** ��  ��:	1.0
** ��  ��:	���濨�ű�����
********************************************************************/


#pragma once
#include "ISchemeCenter.h"
#include "ISchemeEngine.h"
#include "SchemeCache.h"
#include "SchemeDef.h"

// �������ñ�
class CSchemeBenefitCard : public ISchemeBenefitCard, public ISchemeUpdateSink, public CSchemeCache<CSchemeBenefitCard>
{
public:
	// ISchemeUpdateSink
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char *szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName);

	// ISchemeBenefitCard
	// ������濨������Ϣ
	virtual SBenefitCardConfig_S* getBenefitCardConfig(int nChestID);

public:
	bool LoadScheme();
	void Close();

private:
	// �������濨�ű�
	bool LoadBenefitCardScheme(ICSVReader *pCSVReader,const char *szFileName);

private:
	map<int, SBenefitCardConfig_S> m_mapBenefitCardConfig;
};