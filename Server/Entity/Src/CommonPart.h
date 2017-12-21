/*******************************************************************
** 文件名:	CommonPart.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/1/22
** 版  本:	1.0
** 描  述:	

********************************************************************/


#pragma once
#include "ICommonPart.h"
#include "IEventEngine.h"
#include "GuideStep.h"

class CProgressBarObserver;

// 实体状态部件
class CommonPart:public __ICommonPart, public IEventExecuteSink
{
public:
	CommonPart();
	~CommonPart();
public:
	/////////////////////////////////////////IEntityPart/////////////////////////////////////////
	virtual short getPartID();
	virtual bool onLoad( __IEntity* pEntity, int nLoginMode);
	virtual __IEntity * getHost();
	virtual bool onMessage(void * pEntityHead, int msgId, void * data, size_t len);
	virtual void release();

	virtual bool onSerialize( rkt::ibuffer & in,int flag );

	virtual bool deSerialize( rkt::obuf & out,int flag );

	virtual void onChangeHero(int nOldHeroID, int nNewHeroID, bool isBefore){};

    virtual void restore();

	/////////////////////////////////////////__ICommonPart/////////////////////////////////////////
    /*@name 通知客户端数据
    @param nNotifyType: 通知数据类型NOTICE_DATA_TYPE
    @param nValue:      数值内容
    @param pText:       文本内容
    @return 
    */
    virtual bool notifyClientData(int nNotifyType, int nValue, char* pText=0);

    /*@name 保存引导步骤
    @param nStep: 引导步骤
    @return 
    */
    virtual bool saveGuideStep(int nStep);

    /*@name 获取实体自身当前充能值
    @return 
    */
    virtual int getChargeValue(void);

    /*@name 设置实体自身当前充能值
    @param nValue: 充能数值
    @return 
    */
    virtual void setChargeValue(int nValue);    
	/////////////////////////////////////////IEventExecuteSink/////////////////////////////////////////
	/** 
	@param   wEventID ：事件ID
	@param   bSrcType ：发送源类型
	@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
	@param   pszContext : 上下文
	@param   nLen : 上下文长度
	@return  
	@note     
	@warning 
	@retval buffer 
	*/
	virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

private:
	void SendEntityMessage(UID uid, short byPartID, BYTE byKeyAction, LPCSTR pszData, int nDataLen);

    // 触发NPC
    void onMessageTriggerNpc(void * data, size_t len);
    // 触发NPC函数功能
    void onMessageFireNpcFunction(void * data, size_t len);
    // 右键点击
    void onMessageRightButtonClick(void * data, size_t len);
	// 玩家游戏状态是否离开
	void onMessageGameStateIsLeave(void* data, size_t len);
	// 施放进度条技能
	void onMessageCastProcessbarSpell(void* data, size_t len);
    // 请求下发Web导航组
    void onMessageRequestNavGroup(void* data, size_t len);
    // 请求下发Web页信息
    void onMessageRequestWebUrl(void* data, size_t len);
private:
	__IEntity	* 		m_pMaster;
	CGuideStep 	* 		m_pGuideStep;
    int                 m_nChargeValue;     // 当前的充能数值

	CProgressBarObserver* m_pProgressBarChangeVoc;
};