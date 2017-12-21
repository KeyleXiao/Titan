/*******************************************************************
** 文件名:	TitlePart.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-6-27
** 版  本:	1.0
** 描  述:	实体称号部件接口

********************************************************************/

#pragma once

#include "ITitlePart.h"

// 实体称号部件
class TitlePart : public __ITitlePart, public IEventExecuteSink
{
public:
    ////////////////////* IEntityPart */////////////////////////////////////////////
    // 取得部件ID
    virtual short			getPartID();
    virtual bool			onLoad( __IEntity* pEntity, int nLoginMode);
    virtual bool			onMessage(void * pEntityHead, int msgId, void * data, size_t len);
    virtual __IEntity *		getHost();
    virtual void			release();
    virtual bool			onSerialize( rkt::ibuffer & in,int flag );
    virtual bool			deSerialize( rkt::obuf & out,int flag );
    // 切换英雄
	virtual void            onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore) { }

    virtual void            restore();

	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	virtual void			OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    ////////////////////* __ITitlePart */////////////////////////////////////////////
	/** 加入称号
	@param nTitleID: 称号ID
	@param szTitleName: 称号  
	@param bCoverTitle: 是否能覆盖 
	@return  
	*/
	virtual bool				addTitle(int nTitleID, const char * szTitleName, bool bCoverTitle=true);

	// 删除称号
	virtual bool				deleteTitle(int nTitleID);



    ////////////////////* TitlePart */////////////////////////////////////////////
    TitlePart();
    virtual ~TitlePart();
private:
    void				        SendEntityMessage(UID uid, short byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen);

private:
    __IEntity *			m_pMaster;

    // BuildClientSelf数据
    obuf				m_BuildBuf;

    typedef std::map<int, STitleIndexData>    TITLE_INDEXDATA_MAP;
    TITLE_INDEXDATA_MAP         m_mapIndexData;

};