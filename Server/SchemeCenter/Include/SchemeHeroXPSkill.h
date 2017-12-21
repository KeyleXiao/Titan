/*******************************************************************
** �ļ���:	E:\work\w1\speed\Server\SchemeCenter\Src\SchemeCrownPage.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	������
** ��  ��:	12/12/2016	
** ��  ��:	1.0
** ��  ��:	
            ��xpϵͳ�����ļ�
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "SchemeCache.h"


class CSchemeHeroXpSkill : public ISchemeHeroXpSkill,
                        public ISchemeUpdateSink,
                        public CSchemeCache<CSchemeHeroXpSkill>
{
    enum
    {
        COL_XP_HERO_ID,					    // Ӣ��id
        COL_XP_SPELL_ID,                    // ��ɱ������id
        COL_XP_SPELL_TYPE,  				// ��������(ͨ��0,ר��1)
        COL_XP_AUTO_UNLOCK,					// �Ƿ��Զ�����(��0,��1)
        COL_XP_CHIP_NUM,                    // ��ɱ������������Ƭ��
        COL_XP_ICON,						// ��ɱ��ͼ��
        COL_XP_NAME,                        // ��ɱ������
        COL_XP_LOCK_DESC,                   // ��ɱ��δ����ʱ����
        COL_XP_UNLOCK_DESC,                 // ��ɱ������ʱ����
        COL_XP_GUIDE,                       // ����ʱ�Ƿ�����(��0,��1)
        COL_XP_DRESS,                       // ����ʱ�Ƿ�Ĭ��װ��(��0,��1)
    };


    typedef list<SHeroXPSkillConfig_S>      XP_LIST;
	typedef std::map<int, XP_LIST>          HEROXP_MAP;

public:
	
	// ISchemeUpdateSink
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char *szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char *szFileName);

	// ISchemeHeroXpSkill
	// ���XP��Ϣ
	virtual SHeroXPSkillConfig_S* getHeroXpSkillConfig(int nHeroID,int nXPSkillID);

	// ����Զ�����XP�б�
	virtual bool getHeroXpActiveList(SHeroXPSkillConfig_S activeXpSkillList[],int nListCount);

public:
	bool LoadScheme();
	void Close();

private:
	bool updateXpConfigToCache( SHeroXPSkillConfig_S& sConfigInfo );

private:
	HEROXP_MAP              m_schemeInfo;
};
