/*******************************************************************
** 文件名:	E:speed\Server\SchemeCenter\Src\CSchemePrizeUnlock.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	11/8/2016	
** 版  本:	1.0
** 描  述:	
            读策角色英雄解锁配置文件
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
    // 获取指定英雄等级解锁的配置数据
	virtual SHeroLevelPrizeUnlockMap* getHeroLevelPrizeUnlockInfo() ;
    ///////////////////////////////SActorPrizeUnlockVec///////////////////	
    // 获取指定角色解锁的配置数据
    virtual SActorPrizeUnlockVec* getActorPrizeUnlockInfo();
	///////////////////////////////SHeroStarPrizeUnlockMap///////////////////	
	// 获取指定英雄星级解锁的配置数据
    virtual SHeroStarPrizeUnlockMap*  getHeroStarPrizeUnlockInfo();
	///////////////////////////////ISchemeUpdateSink///////////////////
    /** CSV配置载入时通知
    @param   type   : 读取器类型
    : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
    : READER_XML : 就可以把pReader转换成TiXmlDocument*
    @param   pReader：读取CSV的返回接口
    @param   szFileName：配置文件名
    @warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
    @retval buffer
    */
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);

    /** 配置动态更新时通知
    @param   type   : 读取器类型
				    : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
				    : READER_XML : 就可以把pReader转换成TiXmlDocument*
    @param   pReader：读取CSV的返回接口
    @param   szFileName：配置文件名
    @warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
    @retval buffer
    */
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	///////////////////////////////CSchemePathFindServiceInfo//////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool LoadScheme(void);

	/** 关闭
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
