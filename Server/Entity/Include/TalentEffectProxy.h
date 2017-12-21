/*******************************************************************
** 文件名:	E:\speed\Server\Entity\Src\vTalentInfoVec.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	3/14/2016	
** 版  本:	1.0
** 描  述:	
天赋效果 代理
********************************************************************/

#pragma once


#include "EntityDef.h"
#include "SchemeTalentPage.h"
#include "SchemeTalentPageEffect.h"
#include "IEffect.h"
#include "IServerGlobal.h"

class TalentEffectProxy 
{
public:
#ifdef FUNCTION_TALENTPAGE_OPEN
    virtual void                AddTalentEffect(__IEntity *pTarget,TALENT_INFO_VECTOR& vTalentInfoVec);
#endif

    virtual void                AddEffect(int nEffectID, __IEntity *pTarget);
    
    virtual void				RemoveAllEffect();

#ifdef FUNCTION_TALENTPAGE_OPEN
    virtual bool				RfficacyTalentPoints(TALENT_INFO_VECTOR& singleTalentInfo);
#endif

#ifdef FUNCTION_RUNE_OPEN
    virtual void                AddRuneEffect(__IEntity *pTarget,Entity_Rune_Page_Info& runeInfo);
#endif

	virtual void				AddCrownEffect(__IEntity *pTarget,int* propertyList, ETIME_TO_ADD_EFFECT timeToAddEffect);
public:

    // 构造函数
    TalentEffectProxy();

    // 析构函数
    ~TalentEffectProxy();

    typedef list<IEffect*> T_LIST_TALENT_EFFECT_ALL;

    T_LIST_TALENT_EFFECT_ALL m_TalentEffectAll;

};

