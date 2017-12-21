/*******************************************************************
** 文件名:	SchemeHeroCard.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-5-10
** 版  本:	1.0
** 描  述:	英雄卡牌配置 HeroCard.csv

			
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"


// 定义
class CSchemeHeroCard : public ISchemeHeroCard, public ISchemeUpdateSink
{
    enum EMHEROCARD_SCHEME_COLUMN
    {
        COL_CARDID = 0,					    // 卡牌ID
		COL_CARDGRADE,                      // 卡牌等级
        COL_CARDNAME,						// 卡牌名称
        COL_CARDICON,					    // 卡牌图标
        COL_CARDFRAMEICON,                  // 卡牌框图标
        COL_HEROLIST,						// 使用卡牌目标英雄ID列表，0为所有英雄
        COL_REMARK,                         // 卡牌描述
        COL_FLAG,                           // 控制标识
    };

public:
	//////////////////////////////// ISchemeHeroCard ////////////////////////////////
	/** 取得指定卡牌的配置信息
	@param nCardID: 卡牌ID 
	@return  
	*/
    virtual SHeroCardScheme*                        getByCardID( int nCardID );

	/** 取得指定卡牌的颜色名字
	@param nCardID: 卡牌ID 
	@return  
	*/
	virtual string getUBBNameByCardID( int nCardID );

	virtual string getUBBNameBySHeroCardScheme( SHeroCardScheme* pSHeroCardScheme );

	/** 取得某个英雄配置的所有卡牌配置信息
	@param nHeroID: 英雄ID 
	@param arrayCard: 输输出值，所有符合条件的卡牌
	@param nArrayCount: 数组大小
	@return  
	*/
    virtual int                                     getCardsByHeroID(int nHeroID, const SHeroCardScheme** arrayCard, const int nArrayCount);

	//////////////////////////////// ISchemeUpdateSink ////////////////////////////////
	/** CSV配置载入时通知
	@param   type   : 读取器类型
		            : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
					: READER_XML : 就可以把pReader转换成TiXmlDocument*
	@param   pReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/ 
	virtual bool			                        OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	/** 配置动态更新时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			                        OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	/////////////////////////CSchemeMonster//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool											LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void											Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeHeroCard(void);

private:
    typedef int CARD_ID;
	typedef std::map<CARD_ID, SHeroCardScheme>	    TMAP_HEROCARD;

	TMAP_HEROCARD                                   m_mapHeroCard;
};