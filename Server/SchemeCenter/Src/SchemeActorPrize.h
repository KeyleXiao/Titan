#pragma once
#include "ISchemeCenter.h"
#include "ISchemeEngine.h"
#include "SchemeCache.h"
#include "SchemeDef.h"

// �������ñ�
class CSchemeActorPrize : public ISchemeActorPrize, public ISchemeUpdateSink, public CSchemeCache<CSchemeActorPrize>
{
public:
	// ISchemeUpdateSink
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char *szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName);

	// ISchemeActorPrize
	// ��ý���������Ϣ
	virtual SActorPrizeConfig_S* getActorPrizeConfig(int nPrizeID);
	// ��ý�����������Ϣ
	virtual SActorPrizeGroupConfig* getActorPrizeGroupConfig(int nPrizeGroupID);
	// ��ý���ת��������Ϣ
	virtual SActorPrizeExchangeConfig* getPrizeExchangeConfig(int nPrizeID);
    // У���Ƿ���ڴ˽���ID
    virtual bool isExists(int nPrizeID);

public:
	bool LoadScheme();
	void Close();

private:
	// ���ؽ�ɫ�����ű�
	bool LoadActorPrizeScheme(ICSVReader *pCSVReader,const char *szFileName);
	// ���ؽ�ɫ������ű�
	bool LoadActorPrizeGroupScheme(ICSVReader *pCSVReader,const char *szFileName);
	// ���ؽ�ɫ����ת���ű�
	bool LoadActorPrizeExchangeScheme(ICSVReader *pCSVReader,const char *szFileName);

    // �ڲ�У�齱�������Ƿ�Ϸ�
    bool checkPrizeForArray(const char* szFileName, int nRow, int * pArray, int nCount);

private:
	map<int, SActorPrizeConfig_S> m_mapActorPrizeConfig;
	map<int, SActorPrizeGroupConfig> m_mapActorPrizeGroupConfig;
	map<int, SActorPrizeExchangeConfig> m_mapActorPrizeExchangeConfig;
};