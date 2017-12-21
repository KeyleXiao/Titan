/*******************************************************************
** �ļ���:	SchemeHeroRecommendPath.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	ljw
** ��  ��:	3/22/2017
** ��  ��:	1.0
** ��  ��:	

			Ӣ���Ƽ���·(����֧��AI��ʼѡ��)
********************************************************************/
#pragma once
#include "Singleton.h"
#include "ISchemeEngine.h"
#include "SchemeCache.h"
#include "ISchemeCenter.h"
#include <map>

using namespace rkt;

class CHeroRecommendPathScheme : public ISchemeHeroRecommendPath, public ISchemeUpdateSink, public CSchemeCache<CHeroRecommendPathScheme>
{
public:
    virtual bool Load();

    // ��ȡӢ�۵��Ƽ���·
    virtual int getRecommendPath(int nHeroID, int nBattleField);

    // ���ĳ��ս����pathID�б�
    virtual bool getPathList(int nBattleField, PathRecommendHeroNum* pResultBuf, int& nRefCount);

    virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

    virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

    void Close();

private:
    // <ս��ID, <Ӣ��ID, �Ƽ���·>>
    std::map<int, std::map<int, int>> m_mapRecommendPath;

    // <ս��ID, <��·ID, �Ƽ�����>>
    std::map<int, std::map<int, int>> m_mapBattleFieldPath;
};