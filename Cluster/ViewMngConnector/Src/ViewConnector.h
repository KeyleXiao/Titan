/*******************************************************************
** 文件名:	ViewConnector.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2009/8/25
** 版  本:	1.0
** 描  述:	语音服务器连接器
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "IViewConnector.h"
using namespace View;

#include "net.h"
using namespace rkt;

#include <string>
#include <map>
using namespace std;

class ViewConnector : public IViewConnector,public IConnectionEventHandler,public TimerHandler,public IVoiceMessageHandler
{
	enum
	{
		ETimerEventID_ReconnectNet = 0,
		ETimerEventID_Keepalive,
	};

	// 事件处理者列表,以事件处理者实例地址为关键字
	typedef map<DWORD,IVoiceEventHandler*> TMAP_VoiceEventHandlerInfo;
	// 语音服务器上的服务器扩展信息类列表,以服务器ID为关键字
	typedef map<DWORD,IVoicePart*> TMAP_VoicePartList;
	// 本地序号ID与ID对应表,以dwID为关键字
	typedef map<DWORD,DWORD> TMAP_VoiceID2IDInfo;

	//游戏世界信息数据结构
	struct SVoiceWorldNode : public IVoiceWorldInfo
	{
		DWORD dwWorldID;				// 游戏世界ID
		string strWorldName;			// 游戏世界名称
		TMAP_VoiceID2IDInfo mapList;	// 服务器ID对应列表

		SVoiceWorldNode(void){dwWorldID = 0;}
		virtual ~SVoiceWorldNode(void){ }
		// 游戏世界ID
		virtual DWORD GetWorldID(void){ return dwWorldID;}
		// 游戏世界名称
		virtual const char * GetWorldName(void)	{ return strWorldName.c_str();}
		// 服务器个数
		virtual DWORD GetServerCounts(void)	{ return mapList.size();}
	};

	// 游戏世界ID分类列表,以dwWorldID为关键字
	typedef map<DWORD,SVoiceWorldNode> TMAP_VoiceWorldIDInfo;

public:

	ViewConnector(TimerAxis * pTimerAxis);

	virtual ~ViewConnector();


	///////////////////////// IVoiceMessageHandler 处理语音服务器发过来的消息 /////////////////////////
	virtual void HandleVoiceMessage(unsigned long actionId,SMsgVoiceHead &voiceHead,SGameMsgHead* head, LPSTR data, size_t len);

	//////////////////////////////////////////////////////////////////////////
	/************************IConnectionEventHandler*************************/
	virtual void OnAccept( IConnection * pIncomingConn,IConnectionEventHandler ** ppHandler );

	virtual void OnConnected( IConnection * conn );

	virtual void OnDisconnected( IConnection * conn,DWORD reason );

	virtual void OnRecv( IConnection * conn,LPVOID pData,DWORD dwDataLen );

	virtual void OnError( IConnection * conn,DWORD dwErrorCode );

	virtual void OnTimer( unsigned long dwTimerID );

	//////////////////////////////////////////////////////////////////////////
	/***********************IViewConnector***************************/
	virtual bool Create(DWORD dwGameWorldID,WORD wServerType,WORD wSubID,const char * szServerName,const char * szWorldName);

	virtual bool Connect(const char * szCSIP,WORD nCSPort,int nThreadMode);

	virtual void  Close();

	virtual bool  IsConnected();

	// 主动重连
	virtual bool ReConnect();

	virtual bool RegisterServer(DWORD dwGameWorldID,WORD wServerType,WORD wSubID);

	virtual DWORD GetServerID();

	virtual void  Release();
	// 发给语音服务器数据
	virtual void  SendData(const char * pData,DWORD dwLen);

	std::string ToString();

	/** 注册消息处理者
	@param moduleId 模块ID
	@param hander 消息处理者实例
	*/
	virtual bool registerMessageHandler(DWORD moduleId, IVoiceMessageHandler* handler);

	/** 注销消息处理者
	@param moduleId 模块ID
	*/
	virtual bool unregisterMessageHandler(DWORD moduleId);

	/** 注册事件处理者
	@param wEventID 事件ID	参考 VoiceDef.h   EMEventVoice
	@param hander 事件处理者实例
	*/
	virtual bool registerEventHandler(WORD wEventID,IVoiceEventHandler* handler);

	/** 注销事件处理者
	@param wEventID 事件ID	参考 VoiceDef.h   EMEventVoice
	@param  hander 事件处理者实例
	*/
	virtual bool unregisterEventHandler(WORD wEventID,IVoiceEventHandler* handler);

	/** 触发事件
	@param wEventID 事件ID	参考 VoiceDef.h   EMEventVoice
	*/
	virtual bool FireEventExecute(WORD wEventID, LPSTR pszContext, size_t nLen);

	////// 常用发数据接口 ////////////////////////////////////////////////////////////////////

	// 发数据给指定游戏服务器 dwServerID:服务器ID 为0时广播给所有服务器,bZeroBroadcast:为true时打开为0广播,否则无效
	virtual void SendDataToServer(DWORD dwServerID,const char * pData,DWORD dwLen,bool bZeroBroadcast=false);
	// 广播数据给指定列表游戏服务器 pServerArray:服务器ID列表,wServerNum:列表个数
	virtual void BroadcastDataToServer(DWORD * pServerArray,WORD wServerNum,const char * pData,DWORD dwLen);


	////// 常用服务器列表相关接口 ////////////////////////////////////////////////////////////////////

	/**  取得服务器ID列表
	@param   DWORD* pServerArray	: 保存找到的ID数组
	@param   WORD dwArrayLen		: 数组长度
	@param   WORD &dwGetCounts		: 找到的个数
	@param   DWORD dwWorldID		: 游戏世界ID，0为全部
	@param   WORD wServerType		: 服务器类型,0为全部
	@return  找到的个数
	*/
	virtual DWORD GetGameServerIDList(DWORD * pServerArray,DWORD dwArrayLen,DWORD &dwGetCounts,DWORD dwWorldID=0,WORD wServerType=0);

	/**  取得指定游戏世界的指定类型的服务器ID
	@param   DWORD dwWorldID		: 游戏世界ID
	@param   WORD wServerType		: 服务器类型
	@param   WORD wSubID			: 服务器序号
	@return  找到的服务器ID,为0表示没找到
	*/
	virtual DWORD GetGameServerIDByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID);

	/** 列出所有服务器信息
	@param   DWORD dwWorldID		: 游戏世界ID
	@param   WORD wServerType		: 服务器类型
	@return 
	*/
	virtual void PrintGameServerListInfo(DWORD dwWorldID=0,WORD wServerType=0);

	////// 常用游戏世界列表相关接口 ////////////////////////////////////////////////////////////////////

	/**  取得游戏世界ID列表
	@param   DWORD* pWorldArray	: 保存找到的ID数组
	@param   WORD dwArrayLen		: 数组长度
	@param   WORD &dwGetCounts		: 找到的个数
	@return  找到的个数
	*/
	virtual DWORD GetGameWorldIDList(DWORD * pWorldArray,DWORD dwArrayLen,DWORD &dwGetCounts);

	/**  取得指定游戏世界信息
	@param   DWORD dwWorldID		: 游戏世界ID
	@return  找到的服务器ID,为0表示没找到
	*/
	virtual IVoiceWorldInfo * GetGameWorldInfo(DWORD dwWorldID);

	/** 列出所有游戏世界信息
	@return 
	*/
	virtual void PrintGameWorldListInfo();

	////// 常用扩展信息类列表相关接口 ////////////////////////////////////////////////////////////////////

	/** 用服务器ID 取得扩展信息类
	@param   DWORD dwServerID		: 服务器ID
	@return  IVoicePart*	:服务器扩展信息类
	*/
	virtual IVoicePart* GetGameServerPart(DWORD dwServerID);

	/** 用TypeSubID取得扩展信息类
	@param   DWORD dwWorldID		: 游戏世界ID
	@param   WORD wServerType		: 服务器类型
	@param   WORD wSubID			: 服务器序号
	@return  const IVoicePart*	:服务器扩展信息类
	*/
	virtual IVoicePart* GetGameServerPartByTypeSubID(DWORD dwWorldID,WORD wServerType,WORD wSubID);

	////// 语音功能类相关接口 ////////////////////////////////////////////////////////////////////
	/** 添加蓝钻
	@param   DWORD dwUDBID		: 用户UDBID
	@return  DWORD dwCount		: 蓝钻数
	@param	 DWORD dwPDBID		: 用户PDBID 可以为0
	@param	 DWORD dwPDBIDSend	: 赠送者PDBID 可以为0
	@param	 DWORD dwSpare1
	@param	 DWORD dwSpare2
	@param	 DWORD dwSpare3 备用
	@param	 const char * szReason : 备注
	*/
	virtual bool AddVoiceBlueDiamond( DWORD dwUDBID, DWORD dwCount, DWORD dwPDBID = 0, DWORD dwPDBIDSend = 0, DWORD dwSpare1 = 0, DWORD dwSpare2 = 0, DWORD dwSpare3 = 0, const char * szReason = NULL );

	/** 广播数据到客户端
	@param	pData		: 数据
	@param	dwLen		: 数据长度
	@param	nBroadcastType : 广播类型 EMVoiceChannelType
	@param	dwPDBID1/dwPDBID2 : 用户PDBID
	@param	dwWorldID1/dwWorldID2 : 世界ID，
	@param  dwValueID1/dwValueID2 : ID值,参考SVoiceChannelID
	@param  bRoom : 是否广播用户dwPDBID1所在房间
	*/
	virtual bool BroadcastDataToClient( BYTE nBroadcastType, void * pData, DWORD dwLen, DWORD dwPDBID1 = 0, DWORD dwPDBID2 = 0, DWORD dwWorldID1 = 0, DWORD dwWorldID2 = 0, DWORD dwValueID1 = 0, DWORD dwValueID2 = 0, bool bRoom = false );

	/** 取定时器
	@param   
	@param   
	@return  
	*/
	virtual TimerAxis* GetTimerAxis();

protected:

	bool SendHandshake();

	// 清除所有服务器信息列表
	void ClearAllList(void);

	///////////////////////// 处理语音服务器发过来的消息 /////////////////////////
	// 回应握手消息
	void OnMsgHandshakeResponse(unsigned long actionId,SMsgVoiceHead &voiceHead,SGameMsgHead* head, LPSTR data, size_t len);
	// 服务器列表更新消息
	void OnMsgServerListUpdate(unsigned long actionId,SMsgVoiceHead &voiceHead, SGameMsgHead* head, LPSTR data, size_t len);
	// 游戏世界列表更新
	void OnMsgWorldListUpdate(unsigned long actionId,SMsgVoiceHead &voiceHead, SGameMsgHead* head, LPSTR data, size_t len);

protected:

	IConnection   *		m_pConnection;
	TimerAxis     *		m_pTimerAxis;
	std::string			m_szRemoteIP;
	WORD				m_wRemotePort;
    int                 m_nThreadMode;
	DWORD				m_dwPublicWorldID;	// 公共游戏世界ID
	DWORD 				m_ID;
	DWORD				m_dwWorldID;	// 服务器所在游戏世界ID
	WORD				m_wServerType;
	WORD 				m_wSubID;
	bool				m_bIsConnected;
	char				m_szName[VOICE_NAME_MAXSIZE];	// 服务器名
	char				m_szWorldName[VOICE_NAME_MAXSIZE];	// 游戏世界名

	bool m_bIsAnswer;				// 是否有回应
	DWORD m_dwLastAnswerTime;		// 最后收到心跳Tick时间

	// 消息处理者列表
	IVoiceMessageHandler*	m_MsgHandlers[MSG_MODULEID_MAXID+1];
	// 事件处理者列表
	TMAP_VoiceEventHandlerInfo m_mapEventHandlers[EVENT_VOICE_MAXID+1];
	// 语音服务器上的服务器扩展信息类列表,以服务器ID为关键字
	TMAP_VoicePartList	m_mapPartList;
	// 游戏世界ID分类列表,以dwWorldID为关键字
	TMAP_VoiceWorldIDInfo m_mapWorldList;

};


// 取得服务器类型名称
const char* GetServerTypeString(DWORD dwServerType);
