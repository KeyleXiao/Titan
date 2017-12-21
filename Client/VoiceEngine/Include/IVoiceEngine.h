/*******************************************************************
** 文件名:	IVoiceEngine.h
** 版  权:	(C) 深圳冰川网络技术股份公司
** 创建人:	宋扬
** 日  期:	2012/09/24
** 版  本:	1.0
** 描  述:	语音引擎模块
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 VOICE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// VOICE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。


#ifndef __IVoiceEngine__H__
#define __IVoiceEngine__H__

#ifndef  VOICE_API
#	ifdef VOICE_EXPORTS
#		define  VOICE_API  extern "C" __declspec(dllexport)
#	else
#		define  VOICE_API  extern "C" __declspec(dllimport)
#		pragma comment(lib, MAKE_DLL_LIB_NAME(VoiceEngine))
#	endif
#endif


#include "VoiceDeviceDef.h"
#include "VoiceDef.h"
#include "VoiceModuleDef.h"

//struct TimerAxis;
//struct ILuaEngine;
//struct IEventEngine;
//struct ISchemeEngine;
struct IVoiceEngine;
struct IVoice;
struct IVoiceClient;
struct IVoiceDevice;
struct IVoiceControl;
struct IChannelClient;
struct IRoom;
struct IChat;
struct IVideoDevice;

// tolua_begin

// 上层应用接口提供接口
struct IVoiceEngineProvider
{
	/// 获取时间轴
	virtual TimerAxis* GetTimerAxis()  = 0;

	/// 获取事件服务器
	virtual IEventEngine* GetEventEngine()  = 0;

	/// 获取配置引擎
	virtual ISchemeEngine* GetSchemeEngine()  = 0;

    /// 发送数据
    virtual bool SendData(const char* data, size_t size) = 0;

    /// 是否游戏网络连接中
    virtual bool IsConnected() = 0;

    virtual HWND getGameHWND() = 0;

	// 语音引擎的事件回调
	virtual void OnVoiceEvent( DWORD nEventID, void *pEventHead, void *pData, DWORD dwLen ) = 0;
};


// 语音引擎对象接口
struct IVoiceEngine
{
	// 初始化对象
	virtual bool Create(void) = NULL;
	// 释放对象
	virtual void Release(void) = NULL;

	// 获得语音引擎设备接口
	virtual IVoiceDevice* GetVoiceDevice() = NULL;

	// 取得应用实例接口 dwVoiceType : 应用类型 EMVoiceType 
	virtual IVoice* GetVoice( DWORD dwVoiceType = VoiceType_Game ) = NULL;

	// 取得语音实例应用设置相关接口 dwVoiceType : 应用类型 EMVoiceType 
	virtual IVoiceControl* GetVoiceControl( DWORD dwVoiceType = VoiceType_Game ) = NULL;

	// 取得播放和采集视频引擎设备接口 dwVoiceType : 应用类型 EMVoiceType 
	virtual IVideoDevice* GetVideoDevice( DWORD dwVoiceType = VoiceType_Game ) = NULL;

	// 取得逻辑处理实例接口 dwVoiceType : 应用类型 EMVoiceType 
	virtual IVoiceClient* GetVoiceClient( DWORD dwVoiceType = VoiceType_Game ) = NULL;

	// 取得逻辑处理实例接口 dwVoiceType : 应用类型 EMVoiceType 
	virtual IVoiceEngineProvider* GetVoiceEngineProvider() = NULL;

	// 接受游戏网络消息处理
	virtual void OnGameMessage( DWORD actionId, void* head, void* data, size_t len ) = NULL;

	// 抛事件到上层
	virtual void FireProviderEvent( DWORD dwEventID, DWORD dwData1 = 0, DWORD dwData2 = 0, DWORD dwData3 = 0, const char * pData = NULL, DWORD dwLen = 0 ) = NULL;

	// 按键来开麦或闭麦
	virtual void OnKeyOpenMic( bool bOpen ) = NULL;

	// 压力测试 初始化
	virtual bool TestCreate(  DWORD dwClientNum  ) = NULL;

	// 选择设备接口
	virtual bool SelectDevice( int nInputDevice ,int nOutputDevice )= NULL;

	// 选择混音设备接口
	virtual bool SelectMixDevice() = NULL;

	// 停止立体声混音设备
	virtual void StopMixDevice() = NULL;
};

// 语音引擎设备接口
struct	IVoiceDevice	
{
	// 初始化
	virtual bool Create() = NULL;

	// 释放
	virtual void Release() = NULL;

	// 取得应用实例接口 dwVoiceType : 应用类型 EMVoiceType 
	virtual IVoice* GetVoice( DWORD dwVoiceType = VoiceType_Game ) = NULL;

	// 获得输入设备索引
	virtual int GetInputDevice()=NULL;

	// 获得输出设备索引
	virtual int GetOutputDevice()=NULL;

	// 指向设备最前端
	virtual void MoveHead()=NULL;

	// 指向下一个, true:没到结尾 false:到结尾
	virtual bool MoveNext(void) = NULL;

	// 取得列表当前指向的数据
	virtual const SVoiceDeviceInfo * GetListData(void) =NULL;

	// 获得混音输入设备索引
	virtual int GetMixInputDevice() = NULL;

};

// 语音使用者接口
struct IVoiceUser
{
	// 获得压缩录音帧数据事件 dwParam 为用户设置的流畅度系数 用来设置听众的语音流畅机制
	virtual void OnRecordAudioFrame( char* pAudioFrame,DWORD dwLen, char* pVideoFrame,DWORD dwVideoLen,DWORD dwIndex = 0,DWORD dwKey = 0 ,DWORD dwParam =0 ) = NULL;

};
// 语音设备应用实例接口
struct IVoice
{

	// 释放对象
	virtual void Release(void) = NULL;

	// 语音专用线程调度回调任务,主要是来发送网络包
	virtual void DipatchVoiceTask() = NULL;

	// 主逻辑线程调度回调任务
	virtual void DipatchTask() = NULL;

	// 派发向逻辑主线程的发送出的事件队列事件
	virtual void DispatchOutEvent(void) = NULL;

	// 向设备写入语音帧数据 dwParam 为声音来源用户设置的流畅度系数 用来设置自己的语音流畅机制
	virtual bool WriteAudioFrame( DWORD dwChannelID,char* pAudioFrame, DWORD dwLen ,char* pVideoFrame,DWORD dwVideoLen,DWORD dwIndex = 0,DWORD dwKey = 0 ,DWORD dwParam=0) = NULL;

	// 设置语音用户
	virtual bool SetVoiceUser( IVoiceUser* pVoiceUser ) = NULL;

	// 开始录音到本地文件中 参数pSaveDir为 存储路径 pSzPreName为存储文件的前缀名
	virtual void StartRecordingToFile( const char* pSaveDir  ,const char* pSzPreName )=NULL;

	// 停止录音到本地文件中
	virtual void StopRecordingToFile()=NULL;

	// 获得设置相关
	virtual IVoiceControl* GetVoiceControl() = NULL;

	// 获得播放和采集视频引擎设备
	virtual IVideoDevice* GetVideoDevice() = NULL;

	// 服务器发来 更新语音压缩结构信息 更新后要重启播放和采集线程 才生效
	virtual void UpdateCodecInfo( SVoiceCodecInfo* pCodecInfo ) = NULL;

	// 是否有人在频道说话
	virtual bool IsSomeOneTalking() =NULL;

	// 设置是否可以传输语音数据
	virtual void SetTransmiting( bool bIsTransmit ) = NULL;

	// 获得是否正在传输语音数据
	virtual bool GetTransmiting()= NULL;

	// 获得私聊是否正在传输语音数据
	virtual void SetChatTransmiting( bool bTransmit ) = NULL;

	// 获得私聊是否正在传输语音数据
	virtual bool GetChatTransmiting(void) = NULL;


	// 获得是否在录音
	virtual bool GetIsRecording() = NULL;

	// 设置缓冲buffer大小 单位秒
	virtual void SetFramesToBuffer(int nFrames) = NULL;

	// 打开性能测试Ping包
	virtual void OpenPerformancePing(bool bOpen = false,DWORD dwDelays = 0) = NULL;

	// 重启预处理器
	virtual void ReStartPPState() = NULL;

	// 设置淡入播放标识,dwFadeInFrames 淡入的语音帧数,为0表示用内置的默认参数6秒
	virtual bool SetFadeInPlay(DWORD dwFadeInFrames = 0) = NULL;

	// 获得是否 启用了采集
	virtual bool IsVoiceTransmited() = NULL ;

	// 重新选择设备
	virtual void ReSelectDevice() = NULL;

	// 动态修改比特率
	virtual bool SetCodecBitrate( DWORD dwBitrate ) = NULL;

	// 获得音频通道数,立体声为2,单声道为1
	virtual DWORD GetChannelCount() = NULL;

	// 抛事件给语音处理线程,支持多线程调用
	virtual void FireEvent( DWORD dwEventID, DWORD dwData1 = 0, DWORD dwData2 = 0, DWORD dwData3 = 0 ,const char * pData = NULL, DWORD dwLen = 0 ) = NULL;

	// 向主线程抛抛事件,支持多线程调用
	virtual void FireOutEvent(DWORD dwEventID, DWORD dwData1 = 0, DWORD dwData2 = 0, DWORD dwData3 = 0 ,const char * pData = NULL, DWORD dwLen = 0 ) = NULL;

	// 重新选择立体声混音设备
	virtual void ReSelectMixDevice() = NULL;

	// 停止立体声混音设备
	virtual void StopMixDevice() = NULL;

};


// 语音实例应用设置相关 接口
struct IVoiceControl
{
	// 获得当前音量大小 用于动态显示音量条
	virtual int GetVoiceVolume()=NULL;

	// 获得输入音量大小
	virtual int GetInboundVolume()=NULL;

	// 设置输入音量大小 
	virtual void SetInboundVolume( int nVol )=NULL;

	// 获得输出音量大小
	virtual int GetOutboundVolume()=NULL;

	// 设置输出音量大小
	virtual void SetOutboundVolume( int nVol )=NULL;

	// 获得是否静音自己
	virtual bool GetMuteSelf()=NULL;

	// 设置静音自己的标志
	virtual void SetMuteSelf( bool bMute )=NULL;

	// 获得是否静音频道声音
	virtual bool GetMuteSound()=NULL;

	// 设置静音频道声音的标志
	virtual void SetMuteSound( bool bMute )=NULL;

	// 设置激活语音传输与关闭的音量大小
	virtual void SetActivateLevel( int nVolume  )=NULL;

	// 获得激活语音传输与关闭的音量大小
	virtual int GetActivateLevel()=NULL;

	// 设置语音等待帧数系数
	virtual void SetVoiceWaitParam( DWORD dwVoiceWaitParam ) = NULL;

	// 获取语音等待帧数系数
	virtual DWORD GetVoiceWaitParam(void) = NULL;

	// 设置麦克风增益的 系数(0~100)
	virtual void SetAgcLevel( DWORD dwParam ) = NULL;

	// 获得麦克风增益的 数值
	virtual DWORD GetAgcLevel() = NULL ;

	// 设置录音控制 麦克风音量大小
	virtual void SetSystemMicVolume( DWORD dwVolume ) = NULL;

	// 获得录音控制 麦克风的音量大小
	virtual DWORD GetSystemMicVolume() = NULL;

	// 设置录音控制 选中类型 传参为MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE（3） 为选中麦克风 MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT(8) 为选中混音
	virtual bool  SetOutSource( DWORD dwComponentType ) =NULL;

	// 获得上次设置混音时的错误字符串
	virtual const char* GetLastSetSourceError()= NULL;

	// 设置软件麦克风增益的 基数
	virtual void SetAgcLevelBase( DWORD dwParam )= NULL;

	// 频道中实时音量大小
	virtual int GetListenVolume() = NULL;

	// 设置麦克风加强
	virtual bool StrengthenMic( bool bStrengthen ) = NULL;

	// 是否过滤环境噪音
	virtual bool IsFilterBgNoise() = NULL;

	// 设置是否过滤环境噪音
	virtual void SetFilterBgNoise( bool bOpen ) = NULL;

	// 设置上次设置混音时的错误字符串
	virtual void SetLastSetSourceError(  const char* szError ) = NULL;
};


// 游戏世界信息
struct IVoiceServerWorldInfo
{
	virtual const char * GetWorldName( DWORD dwWorldID )  = 0;

	// 服务器个数
	virtual DWORD GetServerCounts(void) = 0;

	// 指向列表最前端
	virtual void MoveHead(void) = 0;

	// 指向列表最末端
	virtual void MoveEnd(void) = 0;

	// 指向下一个, true:没到结尾 false:到结尾
	virtual bool MoveNext(void) = 0;

	// 取得列表当前指向的数据
	virtual const SVoiceWorldData * GetListData(void) = 0;
};


// 语音客户端逻辑处理接口
struct IVoiceClient
{
	/// 创建，初始化
	virtual bool Create( DWORD dwVoiceType, IVoice* pVoice ) = NULL;

	// 释放对象
	virtual void Release(void) = NULL;

	// 连接服务器
	virtual bool ConnectToServer( const char* address, unsigned short port ) = NULL;

	// 断开服务器
	virtual void Disconnect() = NULL;

	//是否连接上服务器
	virtual bool IsConnected() = NULL;


	// 发送数据
	virtual bool SendData( const char * pData,DWORD dwLen ) = NULL;

	// 发送数据给语音服务器
	virtual bool SendDataToServer(WORD byKeyAction,DWORD dwMsgCode,DWORD dwUserData1=0,
								  DWORD dwUserData2=0,DWORD dwUserData3=0, DWORD dwUserData4 = 0, DWORD dwUserData5 = 0, const char * pData=0,DWORD dwLen=0) = NULL;

	// 发送游戏网络数据
	virtual bool SendGameData( BYTE nDestEndPoint, WORD byKeyAction, DWORD dwMsgCode, DWORD dwUserData1 = 0, 
							   DWORD dwUserData2 = 0, DWORD dwUserData3 = 0, const char * pData = NULL, DWORD dwLen = 0 ) = NULL;
	
	// 获得客户端ID
	virtual DWORD GetClientID() = NULL;

	// 获得当前频道ID
	virtual LONGLONG GetChannelID(void) = NULL;

	// 获得当前频道类型
	virtual DWORD GetChannelType(void) = NULL;

	// 获得当前频道ID
	virtual const SVoiceChannelID * GetChannelIDInfo(void) = NULL;

	// 获得当前所在频道对象
	virtual IChannelClient * GetChannel(void) = NULL;

	// 获得当前所在房间对象
	virtual IRoom * GetRoom( DWORD dwRoomID = VoiceRoomType_Max ) = NULL;

	// 获得状态标记 dwFlag为 EMVoiceUserStateFlag
	virtual bool GetStateFlag( DWORD dwFlag ) = NULL;

	// 获得语音服务器的时间
	virtual DWORD GetServerTime(void) = NULL;

    virtual SVoiceUserInfo* getVoiceUser() = NULL;

	// 用ClientID获得用户
	virtual const SVoiceUserInfo_Table * GetUserByClientID( DWORD dwClientID ) = NULL;

	// 根据PDBID查找成员
	virtual const SVoiceUserInfo_Table * GetUserByPDBID( DWORD dwPDBID ) = NULL;

	// 根据PDBID查找成员
	virtual const SVoiceUserInfo_Table * GetUserByUDBID( DWORD dwUDBID ) = NULL;


	// 操作话筒 bOpen 为true开启话筒，false关闭话筒
	virtual void OpenMic( bool bOpen = true ) = NULL;

	// 操作听筒 bOpen 为true开启听筒，false关闭听筒
	virtual void OpenHeadset( bool bOpen = true ) = NULL;

	// 请求观察，bForce为true的话，强制获得用户列表，和当前麦序列表
	virtual void RequestVisitor( bool bForce = false ) = NULL;

	// 改变房间麦序模式 麦类型dwMicState为EMVoiceMicState
	virtual void ChangeMicState( DWORD dwMicState ) = NULL;

	// 设置房间状态标记 dwRoomFlag 为 EMVoiceRoomStateFlag，bSet为true时设置标记，false取消标记
	virtual void SetRoomFlag( DWORD dwRoomFlag, bool bSet ) = NULL;

	// 进入某个频道
	virtual void EnterRoom( DWORD dwType, DWORD dwRoomID, DWORD	dwWorldID = 0, DWORD dwID = 0, const char * szPassword = NULL ) = NULL;

	// GM进入某个频道
	virtual void GMEnterRoom( DWORD dwType, DWORD dwRoomID, DWORD dwWorldID, DWORD dwID ) = NULL;

	// GM进人玩家所在频道
	virtual void GMEnterRoomByPDBID( DWORD dwPDBID ) = NULL;

	// 设置房间锁，bLock为true时，加锁
	virtual void SetRoomLock( DWORD dwRoomID, bool bLock, const char * szPassword = NULL ) = NULL;

	// 抢/下麦，bAdd为true时抢麦，false为下麦
	virtual void AddMicOrder( bool bAdd = true ) = NULL;

	// 增加/减少当前麦上的人的时间，bAdd为true时增加时间，dwTimeCount次数
	virtual void AddMicOrderTime( bool bAdd, DWORD dwTimeCount = 1 ) = NULL;

	// 设置麦序时间，秒
	virtual void SetMicTime( DWORD dwRoomID, DWORD dwTime ) = NULL;

	// 把一个用户添加到麦序，或从麦序上移除，bAdd为true时添加到麦序
	virtual void AddClientToMicOrder( DWORD dwPDBID, bool bAdd = true ) = NULL;

	// 移动一个用户的麦序，dwMoveType为EMVoiceMoveMicOrderType
	virtual void MoveMicOrder( DWORD dwClientID, DWORD dwMoveType ) = NULL;

	// 清空麦序
	virtual void ClearAllMicOrder(void) = NULL;

	// 把成员调度到自己所在频道，bMoveRoom为true时，dwData为房间ID，否则dwData为PDBID
	virtual void Move( bool bMoveRoom, DWORD dwData ) = NULL;

	// 开启频道广播，bOpen为true时开启
	virtual void OpenBroadcast( bool bOpen ) = NULL;

	// 请求拉人AD到当前所在频道 szText 玩家输入，dwBroadcastType 广播类型参考EMVoiceADBroadcastType GM频道才有效, dwADType 附加AD效果 0 - 无, 1 - 弹DIDA
	virtual bool RequestInviteAD(int nChannelType, int nRoomID, const char* szText, DWORD dwBroadcastType = VoiceBroadcast_SelfWorld, DWORD dwADType = 0 ) = NULL;

	// 封PDBID 禁言，10分钟，bBan为false时，解禁
	virtual void BanPDBID( DWORD dwPDBID, bool bBan = true ) = NULL;

	// 踢人
	virtual void Kick( DWORD dwPDBID ) = NULL;

	// 清除频道信息
	virtual void ClearChannelInfo(void) = NULL;

	// 获取一个频道的配置 dwChannelType为VoiceChannel_Max时，取当前频道
	virtual const SVoiceChannelConfig * GetChannelConfig( DWORD dwChannelType = VoiceChannel_Max ) = NULL;

	// 设置频道备注信息
	virtual bool SetChannelRemark( const char* szRemark ) = NULL;

	// 设置称号
	virtual bool SetTitleName( const char* szTitleName ) = NULL;


	// 用户是否在说话
	virtual bool IsTalking( DWORD dwClientID, DWORD dwTimeout = 0 ) = NULL;

	// 指向说话列表最前端
	virtual void VoiceMoveHead(void) = NULL;

	// 指向说话列表最末端
	virtual void VoiceMoveEnd(void) = NULL;

	// 指向说话下一个, true:没到结尾 false:到结尾
	virtual bool VoiceMoveNext(void) = NULL;

	// 取得说话当前指向的数据
	virtual const SVoiceRoomVoiceDataResult * VoiceGetListData(DWORD dwTimeout = 0) = NULL;

	// 设置是否激活F2按键发音
	virtual void SetActiveF2( bool bActive ) = NULL;

	// 获得是否激活F2按键发音
	virtual bool IsActiveF2() = NULL;

	// 获得是否要按键发音标志 此返回值 为房间是否要按键发音 和玩家自己设置 属性的综合标志
	virtual bool IsTalkingByKey() = NULL;

	// 添加拒听
	virtual void AddToBlackList( DWORD dwClientID ) = NULL;

	// 取消拒听 为0清空所有
	virtual void DelFromBlackList( DWORD dwClientID = 0 ) = NULL;

	// 是否拒听
	virtual bool InBlackList( DWORD dwClientID ) = NULL;

	// 开启/关闭用户列表
	virtual void OpenUserList( bool bOpen ) = NULL;

	// 开启/关闭当前频道，当关闭时，需要带上理由szMsg和关闭延时dwTimeDelay秒
	virtual bool OpenChannel( bool bOpen, const char * szMsg = NULL, DWORD dwTimeDelay = 10 ) = NULL;

	// 发送文字聊天信息 
	virtual void sendChatMessage( ulong channel, const char* text, DWORD target = 0 ) = NULL;

	// 获取游戏世界信息对象
	virtual IVoiceServerWorldInfo * GetWorldInfo(void) = NULL;

	// 根据worldID得到世界名
	virtual const char * GetWorldName( DWORD dwWorldID ) = NULL;

	// GM获取正在语音的列表
	virtual void GMGetVoiceList( DWORD dwCount, DWORD dwSecond ) = NULL;

	// GM获取私聊列表
	virtual void GMGetChatList(void) = NULL;

	// 修改语音包发送时，每个包多少帧
	virtual void SetPacketFrames( DWORD dwCount ) = NULL;

	// 频道是否有人在说话
	virtual bool IsChannelOnTalking( DWORD dwChannelType ) = NULL;

	// 房间里是否有人在说话
	virtual bool IsRoomOnTalking( DWORD dwRoomID ) = NULL;

	// 获取语音在线时长
	virtual void GetVoiceTimeLen(void) = NULL;

	// 是否是我的个人频道
	virtual bool IsMyPersonalChannel(void) = NULL;

	// 获取我的个人频道ID信息
	virtual const SVoiceChannelID * GetMyPersonalChannelIDInfo(void) = NULL;

	// 进入他所在的频道
	virtual void EnterRoomByPDBID( DWORD dwPDBID ) = NULL;

	// 添加马甲 bAdd 为false时移除
	virtual void AddVest( DWORD dwUDBID, bool bAdd = true, DWORD dwAddType = 0 ) = NULL;

	// 从服务器请求临时马甲(临时管理)列表
	virtual void RequestVestList(void) = NULL;

	// 指向个人排行榜最前端
	virtual void TopListMoveHead(void) = NULL;

	// 指向个人排行榜最末端
	virtual void TopListMoveEnd(void) = NULL;

	// 指向个人排行榜下一个, true:没到结尾 false:到结尾
	virtual bool TopListMoveNext(void) = NULL;

	// 取得个人排行榜当前指向的数据
	virtual const SVoicePersonalTopData * TopListGetListData() = NULL;

	// 修改个人频道名称
	virtual void SetPersonalChannelName( const char * szNewChannelName ) = NULL;

	// 公开频道，参加排行榜
	virtual void SetPersonalChannelPublic( bool bPublic ) = NULL;

	// 上传用户个人频道配置
	virtual void SendPersonalConfig( bool bPersonalPublic, const char * szChannelName ) = NULL;

	// 向服务器请求个人排行榜
	virtual void RequestPersonalTopList(void) = NULL;

	// 发送私聊邀请
	virtual void SendChatInvite( DWORD dwAccepterPDBID ) = NULL;

	// 接受私聊邀请 dwInviter 客户端ID
	virtual void AcceptChatInvite( DWORD dwInviter, bool bAccept = true ) = NULL;

	// 关闭私聊
	virtual void CloseChat(void) = NULL;

	// 获取私聊对象
	virtual IChat * GetChat(void) = NULL;

	// 操作视频 bOpen 为true开启，false关闭
	virtual void OpenVideo( bool bOpen = true ) = NULL;

	// 操作私聊话筒 bOpen 为true开启话筒，false关闭话筒
	virtual void OpenChatMic( bool bOpen = true ) = NULL;

	// 私聊设置
	virtual void SetChatConfig( DWORD dwConfig = VoiceChatConfig_Defualt ) = NULL;

	// GM旁听私聊
	virtual void GMEnterChat( DWORD dwChatID ) = NULL;

	// 设置连麦
	virtual void SetShareMic( DWORD dwClientID, bool bShare ) = NULL;

	// 添加/取消关注 bCount 是否需要计数
	virtual void AddAtten( DWORD dwClientID, bool bAdd, bool bCount = false ) = NULL;

	// 接受粉丝邀请
	virtual void AcceptFansCall( DWORD dwClientID ) = NULL;

	// 召集粉丝
	virtual void CallFans( const char * szMsg, bool bLocal = false,int nParam1 = 0,int nParam2 = 0  ) = NULL;

	// 请求在线粉丝数
	virtual void RequestFansOnlineCount(void) = NULL;

	// 请求召集粉丝结果
	virtual void RequestFansCallResult(void) = NULL;

	// 获取收到的语音召集信息
	virtual const SVoiceCallFansData * GetCallFansInfo(void) = NULL;

	// 获取房间的名称
	virtual const char * GetRoomName( DWORD dwRoomID ) = NULL;

	// 设置房间名称
	virtual void SetRoomName( DWORD dwRoomID, const char * szNewRoomName ) = NULL;

	// 获取今日粉丝增加数
	virtual DWORD GetFansCountAddToday(void) = NULL;

	// 设置频道最大人数上限
	virtual void SetChannelMaxCount( DWORD dwCount ) = NULL;

	// 设置当前频道语音品质类型  EMVoiceQualityType
	virtual void SetChannelQuality( DWORD dwQuality ) = NULL;

	// 获取当前频道语音品质类型  EMVoiceQualityType
	virtual DWORD GetChannelQuality(void) = NULL;

	// 获取默认语音品质  EMVoiceQualityType
	virtual DWORD GetDefaultQuality(void) = NULL;

	// 设置房间文字聊天冷却时间
	virtual void SetRoomChatCoolDown( DWORD dwRoomID, DWORD dwCoolTime ) = NULL;

	// 关闭房间文字聊天
	virtual void CloseRoomChat( DWORD dwRoomID, bool bClose = true ) = NULL;

	// 赠送礼物 dwFlag EMVoiceSendGiftFlag
	virtual void SendGift( DWORD dwWorldID, DWORD dwPDBID, DWORD dwGiftID, const char * szMsg ,DWORD dwFlag = 0,DWORD dwData1 = 0,DWORD dwData2 = 0,DWORD dwData3 = 0) = NULL;

	// 向服务器请求蓝钻等信息，GM可查询别的角色蓝钻信息
	virtual void RequestDiamondInfo( DWORD dwPDBID = 0 ) = NULL;

	// 获取蓝钻信息
	virtual const SVoiceDiamondInfo * GetDiamondInfo(void) = NULL;

	// 向服务器请求粉丝排行榜
	virtual void RequestFansTopList(void) = NULL;

	// 指向粉丝排行榜最前端
	virtual void FansTopListMoveHead(void) = NULL;

	// 指向粉丝排行榜最末端
	virtual void FansTopListMoveEnd(void) = NULL;

	// 指向粉丝排行榜下一个, true:没到结尾 false:到结尾
	virtual bool FansTopListMoveNext(void) = NULL;

	// 取得粉丝排行榜当前指向的数据
	virtual const SVoiceFansTopListData * FansTopListGetListData() = NULL;

	// 是否能发送视频用来控制只有麦上的人或私聊才能发视频
	virtual bool CanSendVideo() = NULL;

    // 获取语音用户类型(当前频道)
    virtual int getVoiceUserType(SVoiceUserInfo* pUser = NULL) = NULL;

    // 计算语音用户权限(当前频道)
    virtual DWORD calcVoiceUserRight(SVoiceUserInfo* pUser = NULL) = NULL;
};


// 频道对象
struct IChannelClient
{
	// 获取频道ID信息
	virtual const SVoiceChannelID * GetIDInfo(void) = NULL;

	// 获取频道ID
	virtual LONGLONG GetID(void) = NULL;

	// 获得频道内总人数
	virtual DWORD Count(void) = NULL;

	// 获取房间
	virtual IRoom * GetRoom( DWORD dwRoomID = 0 ) = NULL;

	// 获取类型
	virtual DWORD GetType(void) = NULL;

	// 获取状态标记 EMVoiceChannelStateFlag
	virtual bool GetStateFlag( DWORD dwFlag ) = NULL;

	// 基本信息
	virtual const SVoiceChannelBaseInfo * GetBaseInfo(void) = NULL;

	// 拥有者名称
	virtual const char * GetOwnerName(void) = NULL;

	// 指向马甲列表最前端
	virtual void VestMoveHead(void) = NULL;

	// 指向马甲列表最末端
	virtual void VestMoveEnd(void) = NULL;

	// 指向马甲下一个, true:没到结尾 false:到结尾
	virtual bool VestMoveNext(void) = NULL;

	// 取得马甲当前指向的数据
	virtual const SVoiceVestData * VestGetListData(void) = NULL;

	// 用UDBID获取信息
	virtual const SVoiceVestData * GetVestDataByUDBID( DWORD dwUDBID ) = NULL;
};


// 房间对象
struct IRoom
{
    virtual void Add(SVoiceUserInfo_Table& user) = NULL;

    virtual void Del(DWORD dwClientID) = 0;

	// 获取房间ID
	virtual DWORD GetID(void) = NULL;

	// 指向用户列表最前端
	virtual void MoveHead(void) = NULL;

	// 指向用户列表最末端
	virtual void MoveEnd(void) = NULL;

	// 指向下一个, true:没到结尾 false:到结尾
	virtual bool MoveNext(void) = NULL;

	// 取得用户列表当前指向的数据
	virtual const SVoiceUserInfo * GetListData(void) = NULL;

	// 用ClientID获得用户
	virtual SVoiceUserInfo_Table * GetUserByClientID( DWORD dwClientID ) = NULL;

	// 根据PDBID查找成员
	virtual SVoiceUserInfo_Table * GetUserByPDBID( DWORD dwPDBID ) = NULL;

	// 根据UDBID查找成员
	virtual SVoiceUserInfo_Table * GetUserByUDBID( DWORD dwUDBID ) = NULL;

	// 获取总人数
	virtual DWORD Count(void) = NULL;

	// 获取总人数，参考值
	virtual DWORD UserCount(void) = NULL;


	// 指向麦序列表最前端
	virtual void MicMoveHead(void) = NULL;

	// 指向麦序列表最末端
	virtual void MicMoveEnd(void) = NULL;

	// 指向麦序下一个, true:没到结尾 false:到结尾
	virtual bool MicMoveNext(void) = NULL;

	// 取得麦序当前指向的数据
	virtual const SVoiceRoomMicData * MicGetListData(void) = NULL;

	// 获取麦序总数
	virtual DWORD MicCount(void) = NULL;


	// 获取麦控类型 EMVoiceMicState
	virtual DWORD GetMicState(void) = NULL;

	// 获取房间状态标记 EMVoiceRoomStateFlag
	virtual bool GetStateFlag( DWORD dwFlag ) = NULL;

	// 获得用户状态标记 dwFlag为 EMVoiceUserStateFlag
	virtual bool GetUserStateFlag( DWORD dwUserState, DWORD dwFlag ) = NULL;

	// dwClientID 的用户是否在麦序上
	virtual bool GetIsOnMicListByClientID( DWORD dwClientID ) = NULL;

	// dwPDBID的用户是否在麦序上
	virtual bool GetIsOnMicListByPDBID( DWORD dwPDBID ) = NULL;

	// 获得麦序模式下，当前在麦上的人的信息
	virtual const SVoiceCurMicUserInfo * GetCurMicUser(void) = NULL;

	// 麦序模式下，ClientID用户是否在麦上
	virtual bool IsClientOnMic( DWORD dwClientID ) = NULL;

	// 麦序模式下，PDBID用户是否在麦上
	virtual bool IsPDBIDOnMic( DWORD dwPDBID ) = NULL;

	// 是否是2号麦
	virtual bool IsSecondMicUser( DWORD dwPDBID ) = NULL;

	// 是否是最后一个麦
	virtual bool IsLastMicUser( DWORD dwPDBID ) = NULL;

	// 获取频道备注信息
	virtual const char * GetRemark(void) = NULL;

	// 是否有人在说话
	virtual bool IsTalking(void) = NULL;

	// 获取房间名称
	virtual const char * GetName(void) = NULL;

	// 获取基本信息
	virtual const SVoiceRoomBaseInfo * GetBaseInfo(void) = NULL;
};

// 私聊对象
struct IChat
{
	// 获取正在私聊用户数据
	virtual const SVoiceUserInfo * GetChatUserData(void) = NULL;

	// 获取邀请者的用户数据
	virtual const SVoiceUserInfo * GetChatInviterData(void) = NULL;
};






////// 视频 ////////////////////////////////////////////////////////////////////

// 采集视频摄像头输入接口
struct ICameraInput
{
	// 连接视频采集驱动
	virtual bool Connect() = NULL;

	// 断开连接视频采集驱动
	virtual bool Disconnect() = NULL;

	// 启动视频采集
	virtual bool StartCapture() = NULL;

	// 停止视频采集
	virtual bool StopCapture() = NULL;

	// 是否已连接视频采集驱动
	virtual bool IsConnected() = NULL;

	// 是否正在视频采集
	virtual bool IsCapture() = NULL;

	// 从已采集的视频帧数据队列取得一帧数据,char* pVideoOut:输出内存,DWORD dwSize:内存长度,返回读取的实际长度,支持其他固定一个线程线程读取的无锁队列
	virtual DWORD ReadVideoFrame( char* pVideoOut,DWORD dwSize) = NULL;

	// 视频输出格式选择
	virtual bool VideoFormatDlg() = NULL;

	// 视频来源选择
	virtual bool VideoSourceDlg() = NULL;

	// 发送异步控制命令,用来其他线程要调用指定功能 dwCode:摄像头控制命令 EMVideoCameraControlCode
	virtual bool SendControlCode(DWORD dwCode,DWORD dwData1=0,DWORD dwData2=0,DWORD dwData3=0) = NULL;

};

// 采集视频屏幕录像输入接口
struct IScreenInput
{
	// 启动视频采集
	virtual bool StartCapture() = NULL;

	// 停止视频采集
	virtual bool StopCapture() = NULL;

	// 是否已连接视频采集驱动
	virtual bool IsConnected() = NULL;

	// 是否正在视频采集
	virtual bool IsCapture() = NULL;

	// 从已采集的视频帧数据队列取得一帧数据,char* pVideoOut:输出内存,DWORD dwSize:内存长度,返回读取的实际长度,支持其他固定一个线程线程读取的无锁队列
	virtual DWORD ReadVideoFrame( char* pVideoOut,DWORD dwSize) = NULL;

	// 设置屏幕录像大小类型 EMVideoCaptureScreenSizeType,bool bStretch:是否拉伸,默认不拉伸保持长宽比
	virtual bool SetCaptureSizeType(DWORD dwSizeType = 0,bool bStretch = false) = NULL;

	// 取得屏幕录像大小类型 EMVideoCaptureScreenSizeType
	virtual DWORD GetCaptureSizeType() = NULL;

};

//	读取时回调的接口,用来接数据,直接用队列中内存来拷贝数据
struct IVideoShowHandler
{
	// 当调用
	virtual void OnReadShowVideoFrame(DWORD dwVideoShowChannel,BYTE * pVideoData,DWORD dwLen ) = NULL;
};

// 播放视频输出接口
struct IVideoShow
{
	// 写入视频帧数据,DWORD dwClientID:视频来源ID,一般是用户dwClientID,内部会处理显示在具体的通道上,BYTE * pVideoData:视频帧数据,DWORD dwLen:视频帧数据长度,DWORD dwFlag:EMVoiceStreamDataFlag
	virtual bool WriteChannelVideoFrame(DWORD dwClientID,BYTE * pVideoData,DWORD dwLen,DWORD dwFlag = 0) = NULL;

	// 设置指定显示通道显那个视频来源ID用户视频,本地通道除外,dwClientID为0时,会智能判断并显示
	virtual bool SetShowClientID(DWORD dwVideoShowChannel,DWORD dwClientID = 0) = NULL;

	// 取得指定显示通道显那个视频来源ID用户视频
	virtual DWORD GetShowClientID(DWORD dwVideoShowChannel) = NULL;

	// 写入视频帧数据到显示列表,DWORD dwVideoShowChannel:显示视频的通道类型,BYTE * pVideoData:视频帧数据,DWORD dwLen:视频帧数据长度
	virtual bool WriteVideoFrame(DWORD dwVideoShowChannel,BYTE * pVideoData,DWORD dwLen) = NULL;

	// 从视频帧数据队列取得一帧数据,DWORD dwVideoShowChannel:显示视频的通道类型,char* pVideoOut:输出内存,DWORD dwSize:内存长度,返回读取的实际长度,支持其他固定一个线程线程读取的无锁队列
	virtual DWORD ReadVideoFrame(DWORD dwVideoShowChannel,BYTE* pVideoOut,DWORD dwSize) = NULL;

	// 从视频帧数据队列取得一帧数据,DWORD dwVideoShowChannel:显示视频的通道类型 IVideoShowHandler * pHandler: 读取时回调的接口,用来接数据,直接用队列中内存来拷贝数据
	virtual bool ReadVideoData(DWORD dwVideoShowChannel,IVideoShowHandler * pHandler) = NULL;

};


// 视频码率控制接口
struct IVideoSpeed
{
	// 开启视频码率控制 bool bEnable: true 开启
	virtual void Enable(bool bEnable = true) = NULL;

	// 是否开启视频码率控制
	virtual bool IsEnable() = NULL;

	// 设置视频码率控制模式分类 DWORD dwSpeedType : EMVideoNetSpeedType 
	virtual bool SetSpeedType(DWORD dwSpeedType = VideoNetSpeed_Smooth) = NULL;

	// 取得视频码率控制模式分类
	virtual DWORD GetSpeedType() = NULL;

	// 开启包括音频码率控制 bool bEnable: true 开启,默认已开启
	virtual void IncludeAudio(bool bEnable = true) = NULL;

	// 是否开启包括音频码率控制
	virtual bool IsIncludeAudio() = NULL;

	// 当前实时网络速度
	virtual DWORD GetSendSpeed() = NULL;
	// 当前实时平均网络速度
	virtual DWORD GetAvgSendSpeed() = NULL;
	// 当前视频实时网络速度
	virtual DWORD GetVideoSendSpeed() = NULL;
	// 当前音频实时网络速度
	virtual DWORD GetAudioSendSpeed() = NULL;
	// 当前视频实时Fps,音频是固定的不用算
	virtual DWORD GetVideoFPS() = NULL;


	// 接收当前实时网络速度
	virtual DWORD GetRecvSpeed() = NULL;
	// 接收当前实时平均网络速度
	virtual DWORD GetAvgRecvSpeed() = NULL;
	// 接收当前视频实时网络速度
	virtual DWORD GetVideoRecvSpeed() = NULL;
	// 接收当前音频实时网络速度
	virtual DWORD GetAudioRecvSpeed() = NULL;
	// 接收当前视频实时Fps,音频是固定的不用算
	virtual DWORD GetVideoRecvFPS() = NULL;

};


// 播放和采集视频引擎设备接口
struct	IVideoDevice
{
	// 取得采集视频屏幕录像输入接口
	virtual IScreenInput* GetScreenInput() = NULL;

	// 取得采集视频摄像头输入接口
	virtual ICameraInput* GetCameraInput() = NULL;

	// 取得视频码率控制接口
	virtual IVideoSpeed* GetVideoSpeed() = NULL;

	// 取得播放视频输出接口
	virtual IVideoShow* GetVideoShow() = NULL;

	// 设置输入源类型 EMVideoSourceType
	virtual bool SetVideoSourceType(DWORD dwVideoSourceType = 0) = NULL;

	// 取得输入源类型 EMVideoSourceType
	virtual DWORD GetVideoSourceType() = NULL;

	// 设置是否打开显示本地预览视频
	virtual void PreviewLocal(bool bOpen = true) = NULL;

	// 是否打开显示本地预览视频
	virtual bool IsPreviewLocal() = NULL;

};

//////////////////////////////////////////////////////////////////////////



// tolua_end



VOICE_API IVoiceEngine* CreateVoiceEngine(IVoiceEngineProvider* pProvider);

//
#if defined(VOICE_STATIC_LIB)/// 静态库版本
#	pragma comment(lib, "libVoiceEngine")
extern "C" IVoiceEngine* CreateVoiceEngine(IVoiceEngineProvider* pProvider);
#	define	CreateVoiceEngineProc	CreateVoiceEngine
#else /// 动态库版本
typedef IVoiceEngine* (__stdcall *procCreateVoiceEngine)(IVoiceEngineProvider* pProvider);
//#	define	CreateVoiceProc	DllApi<procCreateVoice>::load(MAKE_DLL_NAME(VoiceEngine), "CreateVoiceEngine")
#endif

#endif //__VOICE__H__