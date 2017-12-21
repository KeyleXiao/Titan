/*******************************************************************
** 文件名:	GameVoiceClient.h
** 版  权:	(C) 深圳冰川网络技术股份公司
** 创建人:	谢晓鑫
** 日  期:	2012-11-26
** 版  本:	2.0
** 描  述:	语音系统接口
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "Singleton.h"
#include <string>
#include <vector>
#include <map>
#include <list>
#include "VoiceModuleDef.h"
#include "IVoiceEngine.h"
#include "IMessageDispatch.h"
#include "DGlobalMessage.h"

using rkt::Singleton;

struct IGameVideoShow
{

};

/**
@name : 语音模块
@brief: 
*/
class CGameVoiceClient : public IVoiceEngineProvider
					   , public IMessageHandler
					   , public Singleton<CGameVoiceClient>
					   , public ISchemeUpdateSink
                       , public IGameViewEventHandler
{
public:
	typedef map<DWORD, SVoiceGiftSchemeInfoEx> TMAP_VoiceGiftSchemeInfo;

	// 构造函数
	CGameVoiceClient(void);

	// 析构函数
	virtual ~CGameVoiceClient(void);

	// 初始化
	bool Create(void);

	// 释放操作
	void Release(void);


	///////////// IVoiceEngineProvider 上层应用接口提供接口 //////////////////////////////////////

	/// 获取时间轴
	virtual TimerAxis* GetTimerAxis();

	/// 获取事件服务器
	virtual IEventEngine* GetEventEngine();

	/// 获取配置引擎
	virtual ISchemeEngine* GetSchemeEngine();

    /// 发送数据
    virtual bool SendData(const char* data, size_t size);

    /// 是否游戏网络连接中
    virtual bool IsConnected();

    virtual HWND getGameHWND(){return NULL;}

	// 语音引擎的事件回调
	virtual void OnVoiceEvent( DWORD nEventID, void *pEventHead, void *pData, DWORD dwLen );

	/////////// IGameVoiceClient ///////////////////////////////////////////////////////////////

	// 获得语音引擎
	virtual IVoiceEngine * GetVoiceEngine(void);

	// 获得播放视频游戏处理显示接口
	virtual IGameVideoShow * GetGameVideoShow(void);

	// 获取语音配置数值
	virtual int GetVoiceSchemeInfoNum( DWORD dwNumID );

	// 获取语音礼物配置
	virtual const SVoiceGiftSchemeInfoEx * GetVoiceGiftSchemeInfo( DWORD dwID );

	// 获取打折信息
	virtual const SVoiceRebateData * GetVoiceRebateData(void);

	// 指向礼物列表最前端
	virtual void GiftMoveHead(void);

	// 指向礼物列表最末端
	virtual void GiftMoveEnd(void);

	// 指向礼物下一个, true:没到结尾 false:到结尾
	virtual bool GiftMoveNext(void);

	// 取得礼物列表当前指向的数据
	virtual const SVoiceGiftSchemeInfoEx * GiftGetListData(void);

	////////////IMessageHandler //////////////////////////////////////////////////////////////

	// 游戏网络消息处理
    virtual void onMessage( SNetMsgHead* head, void* data, size_t len);

    virtual bool onViewEvent( int eventID,int nParam,const char * strParam,void * ptrParam );


	///////////////////////// ISchemeUpdateSink /////////////////////////
	/// purpose: CSV配置载入时通知
	virtual bool OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);
	/// purpose: CSV配置动态更新时通知
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);


	///////////// 公共接口 /////////////////////////////////////////////////////////////

	//获得游戏激活状态
	virtual bool GetGameActivate();

private:
	// 频道拉人
	void OnMessageAD( ulong actionId, SNetMsgHead* head, void* data, size_t len);

	// 加载语音配置文件
	bool LoadVoiceSchemeInfo( TiXmlDocument* pTiXmlDocument );

	// 加载语音礼物配置文件
	bool LoadVoiceGiftSchemeInfo( ICSVReader * pCSVReader );

private:
	// 语音引擎
	IVoiceEngine* m_pVoiceEngine;

    IGameVideoShow* m_pVideoShow;

	// 语音配置
	SVoiceSchemeInfo m_VoiceSchemeInfo;

	// 语音礼物配置
	TMAP_VoiceGiftSchemeInfo m_VoiceGiftSchemeInfo;

	// 语音礼物配置迭代器
	TMAP_VoiceGiftSchemeInfo::iterator m_SchemeInfoItr;

};