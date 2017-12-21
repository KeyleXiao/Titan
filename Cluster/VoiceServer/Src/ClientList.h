/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceServer\Src\ClientList.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武
** 日  期:	2012-11-21
** 版  本:	1.0
** 描  述:	语音客户端用户列表
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#ifndef __CLIENT_LIST_H__
#define __CLIENT_LIST_H__

#include "VoiceDef.h"
#include "ClientUser.h"
#include "Singleton.h"
#include <list>
  

/**
@name : 客户端列表
@brief:
*/
class CClientList : public Singleton<CClientList>
{
public:
	typedef std::map<DWORD,CClientUser*> CLIENT_LIST;

	// 用PDBID 索引 clientID
	typedef std::map<DWORD,DWORD> TMAP_IDInfo;

	/**
	@name        : 添加一个客户端用户
	@param client: 用户指针
	@return      : 返回添加后的指针
	*/
    CClientUser * AddUser(DWORD	dwClientID);

	/**
	@name        : 从用户列表中删除一个客户端用户
	@param client: 用户指针
	@return      : 如果找不到该用户则返回false
	*/
	bool DelUser( DWORD dwClientID, DWORD dwPDBID, DWORD dwUDBID );

	/**
	@name        : 根据ID索引一个用户,更快
	@return      : 如果找不到该用户则返回0
	*/
	CClientUser * GetUser(const DWORD dwClientID);

	/**
	@name        : 根据ID查找一个用户,用Map找查
	@return      : 如果找不到该用户则返回0
	*/
	CClientUser * FindUser(const DWORD dwClientID);

	// 获得用户列表，如果网关ID为VOICE_INVALID_ID，dwServerID = VOICE_INVALID_ID则返回所有的用户
	bool GetUserList( CClientUser ** pUserList, DWORD dwCount, DWORD & dwGetCount, DWORD dwGatewayID = VOICE_INVALID_ID, DWORD dwServerID = VOICE_INVALID_ID );

	// 获得用户ID列表，如果网关ID为VOICE_INVALID_ID，dwServerID = VOICE_INVALID_ID则返回所有的用户
	DWORD GetClientIDListByServer( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts, DWORD dwGatewayID = VOICE_INVALID_ID, DWORD dwServerID = VOICE_INVALID_ID, bool bSameServer = true );

	// 根据类型和类型ID值，获取用户ID列表
	DWORD GetClientIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts, DWORD dwChannelType = VoiceChannel_GM, LONGLONG nID = 0 );

	// 获取某个玩家的所有粉丝ID列表
	DWORD GetFansIDList( DWORD * pClientIDArray, DWORD dwIDArrayCount, DWORD & dwGetCounts, CClientUser & user, bool bLocal );
	
	// 清除指定网关上的用户,默认全清
	void Clear( DWORD dwGatewayID=VOICE_INVALID_ID );

	/**
	@name        : 统计当前所有用户数
	@return      : 返回用户数
	*/
	DWORD CountUser();

	// 保存用户信息列表到csv文件
	void SaveUserListToCSV();

	/** 写入数据到文件
	@param  BYTE* pData:要保存的数据
	@param  int nLens: 数据大小
	@param  char * szFileName:要保存的文件名
	@return  
	*/	
	void WriteData2File(BYTE* pData,int nLens,const _TCHAR * szFileName);

	/// 设置客户列表数组
	void SetClientArray( DWORD dwClientID, CClientUser * pUser );

	// 添加一个PDBID索引
	DWORD AddPDBIDInfo( DWORD dwPDBID, DWORD dwClientID );

	// 删除一个PDBID索引
	void DelPDBIDInfo( DWORD dwPDBID, DWORD dwClientID );

	// 添加一个UDBID索引
	DWORD AddUDBIDInfo( DWORD dwUDBID, DWORD dwClientID );

	// 删除一个UDBID索引
	void DelUDBIDInfo( DWORD dwUDBID, DWORD dwClientID );

	// 通过PDBID查找clientID
	DWORD GetClientIDByPDBID( DWORD dwPDBID );

	// 通过UDBID查找clientID
	DWORD GetClientIDByUDBID( DWORD dwUDBID );

	// 通过PDBID查找client对象
	CClientUser *GetClientByPDBID( DWORD dwPDBID );

	// 通过UDBID查找client对象
	CClientUser *GetClientByUDBID( DWORD dwUDBID );

	// 网关用户离开
	void OnGatewayLeave( DWORD dwGatewayID );

	// 服务器连入时，要更新该游戏世界下所有用户的社会服务器ID
	bool UpdateUserServerInfo( DWORD dwServerID, DWORD dwWorldID );

	// 更新用户列表，网关同步用户到语音服务器用
	void OnVoiceGatewayUpdateUserList( DWORD dwGatewayID, SMsgVoiceGatewayClientNode * pClientNode, DWORD dwCount );

	// 同步指定数量的正在语音用户列表给GM
	void UpdateVoiceListToClient( CClientUser & user, DWORD dwCount, DWORD dwSecond );

	CClientList();

	virtual ~CClientList();

protected:

	/** 取得时间字串
	@param   DWORD dwTime:时间
	@param   
	@return  LPCSTR
	*/
	LPCSTR GetTimeString(DWORD dwTime);

//protected:
// 测试用
public:
	// 用数组提高查找效率
	CClientUser *m_ClientArray[VOICE_MAX_GATEWAYID][VOICE_MAX_CLIENT_COUNT];	// 分类的客户端

private:
	// PDBID索引clientID列表
	TMAP_IDInfo m_mapIDInfo;

	// UDBID索引clientID列表
	TMAP_IDInfo m_mapUDBIDInfo;

	// 客户端列表
	CLIENT_LIST m_list[VOICE_MAX_GATEWAYID];

};

#endif//__CLIENT_LIST_H__