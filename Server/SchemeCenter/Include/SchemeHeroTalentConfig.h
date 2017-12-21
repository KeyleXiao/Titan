/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\SchemeCenter\Src\SchemeHeroTalentConfig.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	14/7/2016	
** ��  ��:	1.0
** ��  ��:	
            ���߻����������ļ�
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "SchemeCache.h"

enum TalentActiveType
{
	TalentActiveType_DefualtActive = 0,
	TalentActiveType_SystemActive
};

class CSchemeHeroTalentConfig : public ISchemeHeroTalentConfig,
                        public ISchemeUpdateSink,
                        public CSchemeCache<CSchemeHeroTalentConfig>
{
    // ���ñ��е��� 							 					
    enum TC
    {
        TC_HERO_ID,         // Ӣ��ID
        TC_TALENT_ID,       // �츳ID
        TC_LEARN_LEVEL,     // �츳ѧϰ�ȼ�
        TC_ICON,            // �츳ͼ��
        TC_RECOMMAND_SHOW,  // �Ƿ��Ƽ���ʾ
        TC_ACTIVE_TYPE,     // �������ͣ�0Ĭ���Զ�;1ϵͳ��
        TC_ACTIVE_LEVEL,    // ����ȼ�
        TC_INNER_MUTEX,     // �����Ƿ�ȼ�����(0������1����)
        TC_SHOW_TYPE,       // ��ʾ���ͣ�0δ�����ʾ1δ������ʾ��
        TC_EFFECT_SLOT,     // Ӱ�켼�ܲ�λ
        TC_NAME,            // �츳����
        TC_PRIZE_NAME,      // �츳��������
        TC_DESC,            // �츳����
        TC_UNACTIVE_DESC,   // δ��������
        TC_INNER_DESC,      // �����츳����
        TC_PRE_TALENTID,    // ǰ���츳
        TC_WAR_TYPE,        // ս������ID
        
    };
	typedef std::map<int, std::vector<Single_Hero_Talent_InfoEx> > T_HeroTalentConfigInfo;

public:
	
	///////////////////////////////IHeroTalentInfo///////////////////	
    // ��ȡָ����ҷ��ĵ���������
    virtual std::map<int, std::vector<Single_Hero_Talent_InfoEx> >*  GetHeroTalentInfo();

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
	CSchemeHeroTalentConfig(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeHeroTalentConfig(void);
                           
	// ���ر�Ӣ��[0,nStarLv]�Ǽ������м����츳�б�
	// @param nStarLv: ���ǵȼ�
	virtual vector<int> getAllHeroStarActiveTalent(int nHeroID, int nStarLv);

	// ��ȡӢ���츳��Ϣ
	virtual Single_Hero_Talent_InfoEx* getHeroTalent(int nHeroID, int nTalentID);

private:
	T_HeroTalentConfigInfo m_SchemeInfo;
};
