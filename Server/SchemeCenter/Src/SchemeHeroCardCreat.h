/*******************************************************************
** �ļ���:	SchemeHeroCardCreate.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	����
** ��  ��:	2016/8/3
** ��  ��:	1.0
** ��  ��:	

********************************************************************/
#pragma once
#include "SchemeDef.h"
#include "SchemeCache.h"
#include "ISchemeCenter.h"

using namespace rkt;
class CSchemeHeroCardCreat : public ISchemeHeroCardCreat, public ISchemeUpdateSink, public CSchemeCache<CSchemeHeroCardCreat>
{
public:
    /////////////////////////////////ISchemeUpdateSink/////////////////////////////////////////
    virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    /////////////////////////////////ISchemeHeroCardCreat/////////////////////////////////////////
    virtual SHeroCardCreatPro* getHeroCardCreatPro(int nPlayerLV);

    virtual SHeroCardCreatCount* getHeroCardCreateCount(int nPlayerLV, int nWarType);

public:
    /////////////////////////////////CSchemeHeroCardCreat/////////////////////////////////////////
    bool                    LoadScheme();

    void                    Close();

protected:
    bool                    loadHeroCardCreatProScheme(SCRIPT_READER reader);
    bool                    loadHeroCardCreatCountScheme(SCRIPT_READER reader);

private:
    // ��ҵȼ�Ϊ����
    std::map<int, SHeroCardCreatPro>        m_mapHeroCardCreatePro;

    // ��ҵȼ�Ϊ������  ս������Ϊ������
    typedef std::map<int, std::map<int, SHeroCardCreatCount>> MAPHEROCARDCREATCOUNT;
    MAPHEROCARDCREATCOUNT m_mapHeroCardCreateCount;

};