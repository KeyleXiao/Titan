/*******************************************************************
** �ļ���:	E:speed\Server\SchemeCenter\Src\CSchemePrizeUnlock.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	11/8/2016	
** ��  ��:	1.0
** ��  ��:	
            ���߽�ɫӢ�۽��������ļ�
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "SchemeCache.h"

enum PrizeUnlockType
{
	PrizeUnlockType_Actor = 1 ,
	PrizeUnlockType_HeroLevel,
	PrizeUnlockType_HeroStar,
};

class CSchemePrizeUnlock : public ISchemePrizeUnlock,
                        public ISchemeUpdateSink,
                        public CSchemeCache<CSchemePrizeUnlock>
{
public:
	
	///////////////////////////////SHeroLevelPrizeUnlockMap///////////////////	
    // ��ȡָ��Ӣ�۵ȼ���������������
	virtual SHeroLevelPrizeUnlockMap* getHeroLevelPrizeUnlockInfo() ;
    ///////////////////////////////SActorPrizeUnlockVec///////////////////	
    // ��ȡָ����ɫ��������������
    virtual SActorPrizeUnlockVec* getActorPrizeUnlockInfo();
	///////////////////////////////SHeroStarPrizeUnlockMap///////////////////	
	// ��ȡָ��Ӣ���Ǽ���������������
    virtual SHeroStarPrizeUnlockMap*  getHeroStarPrizeUnlockInfo();
	///////////////////////////////ISchemeUpdateSink///////////////////
    /** CSV��������ʱ֪ͨ
    @param   type   : ��ȡ������
    : �����READER_CSV : �Ϳ��԰�pReaderת����ICSVReader*
    : READER_XML : �Ϳ��԰�pReaderת����TiXmlDocument*
    @param   pReader����ȡCSV�ķ��ؽӿ�
    @param   szFileName�������ļ���
    @warning ��Ҫ�ڴ˷����ͷ�pCSVReader����pTiXmlDocument,��Ϊһ���ļ���Ӧ���sink
    @retval buffer
    */
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);

    /** ���ö�̬����ʱ֪ͨ
    @param   type   : ��ȡ������
				    : �����READER_CSV : �Ϳ��԰�pReaderת����ICSVReader*
				    : READER_XML : �Ϳ��԰�pReaderת����TiXmlDocument*
    @param   pReader����ȡCSV�ķ��ؽӿ�
    @param   szFileName�������ļ���
    @warning ��Ҫ�ڴ˷����ͷ�pCSVReader����pTiXmlDocument,��Ϊһ���ļ���Ӧ���sink
    @retval buffer
    */
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	///////////////////////////////CSchemePathFindServiceInfo//////////////////////
	/** ����ű�
	@param   
	@param   
	@return  
	*/
	bool LoadScheme(void);

	/** �ر�
	@param   
	@param   
	@return  
	*/
	void Close(void);

	/** 
	@param   
	@param    
	@return  
	*/
	CSchemePrizeUnlock(void);

	/** 
	@param   
	@param   
    @return  
    */
    virtual ~CSchemePrizeUnlock(void);

private:
    SHeroLevelPrizeUnlockMap m_mapHeroLevelUnlockInfo;
    SActorPrizeUnlockVec m_vecActorUnlockInfo;
	SHeroStarPrizeUnlockMap m_mapHeroStarUnlockInfo;
};
