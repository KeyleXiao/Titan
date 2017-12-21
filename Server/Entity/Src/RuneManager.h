/*******************************************************************
** 文件名:	RuneManager.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	8/16/2017
** 版  本:	1.0
** 描  述:	符文系统

********************************************************************/
#ifndef __RUNEMANAGER_H__
#define __RUNEMANAGER_H__

#include "IBankPart.h"
#include "EntityDef.h"
#include "IDBEngineService.h"
#include "IRuneManager.h"

class CPlayerBankPart;


#ifdef FUNCTION_RUNE_OPEN
// 符文系统管理类
class CRuneManager : public IRuneManager, public IDBRetSink
{
public:
	/////////////////////////////////IDBRetSink/////////////////////////////////////////
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

public:
	///////////////////////////IRuneManager//////////////////////////
    // 添加拥有符文数据
    virtual bool addRuneStoreInfo(int nRuneId,int nCount = 1);
    // 判断下是否能添加符文
    virtual bool canAddRuneStoreInfo(int nRuneId,RUNE_BUY_ERROR_TYPE& eErrorType,int nCount = 1);
    // 获取当前符文数据
    virtual bool getOtherRuneData(obuf & outData);
    // 获取当前符文页
    virtual int getCurRunePage();

	// 构造函数
	CRuneManager();
	// 析构函数
	virtual ~CRuneManager();

	bool onSerialize(rkt::ibuffer& in);

    bool onSerializePage(rkt::ibuffer& in);


	bool deSerialize(rkt::obuf& out,int flag );

	bool deSerializePage(rkt::obuf& out);

	/** 创建
	*/
	bool onLoad( __IEntity* pEntity, CPlayerBankPart *pBankPart, TalentEffectProxy* pEffectProxy);

	/** 重置 
	*/
	void restore();

    // 释放
    void release(void);

    size_t getStore(rkt::obuf& out);

    void setRuneTalentPageIndex(int nTalentEffectPageIndex);
    ////////////////////////////////////// 符文 ////////////////////////////////////
    // 客户端请求同步符文信息
    void requestDataBaseRuneInlayInfo();
    // 客户端请求其他人的符文信息
    void onClientRequestOtherSyncRuneInfo(void * pData, size_t nLen);
    // 处理客户端请求保存符文页操作
    bool onClientRequestSaveRunePage(LPCSTR pszMsg, size_t nLen);
    // 处理客户端请求删除符文页操作
    bool onClientRequestDeleteRunePage(void * data, size_t len);
    // 数据库返回符文数据
    void onDBReturnRuneInfo(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);
    // 调用符文buff类效果生效
    bool setRuneEffect();
    // 给客户端发送其他人的符文信息
    void sendOtherRuneInfoToClient(void *pOutData,int nOutLen );
    // 发送初始符文页数信息到客户端
    void sendRunePageInfoToClient();

    // 符文商店：自己拥有的符文
    // 更新激活的符文到客户端
    void updateActiveRuneStoreItemToClient(int nRuneId,int bActive);	

    bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);
private:
	__IEntity *                     m_pMaster;

	CPlayerBankPart *               m_pBankPart;

	TalentEffectProxy*              m_pEffectProxy;


    /////////////////////////////////// Rune 符文系统/////////////////////////////////////
    // 本玩家符文页中符文的情况,一个玩家最多有20页符文
    typedef std::map<int, Entity_Rune_Page_Info>         RUNE_PAGE_MAP;
    RUNE_PAGE_MAP                           m_mapRunePage;
    // 当前符文页号
    int                                     m_nCurRunePageIdx;                             

    // 符文库 <符文ID， struct Rune>

    RUNESTORE_MAP                           m_mapRuneStore;
};

#endif 
#endif	// __RUNEMANAGER_H__