/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceGateway\Src\ClientList.h
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
	/**
	@name        : 添加一个客户端用户
	@param client: 用户指针
	@return      : 如果当前已达到最大用户数则返回false
	*/
    bool AddUser(CClientUser * pClient);

	/**
	@name        : 从用户列表中删除一个客户端用户
	@param client: 用户指针
	@return      : 如果找不到该用户则返回false
	*/
	bool DelUser(CClientUser * pClient);

	/**
	@name        : 根据ID查找一个用户
	@return      : 如果找不到该用户则返回0
	*/
	CClientUser * FindUser(const DWORD dwID);

	/**
	@name        : 根据ID查找一个用户
	@return      : 如果找不到该用户则返回0
	*/
	CClientUser * FindUser( const char * szIP, const WORD nPort );

	/**
	@name        : 统计当前所有用户数
	@return      : 返回用户数
	*/
	DWORD CountUser();

	void Clear(void);

	/**
	@name        : 根据索引取得用户指针
	@warning     : 注意：索引值的上限是MAX_CLIENT_COUNT而不是CountUser
	@param index : 用户索引
	@return      : 如果该索引无用户则返回false,
	*/
	CClientUser * GetUserByIndex(DWORD dwIndex);

	DWORD GetGatewayID();

	void  SetGatewayID(DWORD dwID);

	// 保存用户信息列表到csv文件
	void SaveUserListToCSV();

	/** 写入数据到文件
	@param  BYTE* pData:要保存的数据
	@param  int nLens: 数据大小
	@param  char * szFileName:要保存的文件名
	@return  
	*/	
	void WriteData2File(BYTE* pData,int nLens,const char * szFileName);

	CClientList() : m_dwGatewayID(0),m_dwCounts(0)
	{
		memset(&m_list,0,sizeof(m_list));
	}

protected:
	/// 产生一个客户端ID
	inline bool GenerateID(DWORD & dwID);

	/// 归还一个客户端ID
	inline void RestoreID(const DWORD & dwID);

	/// 从ID转成索引
	inline DWORD ID2Index(const DWORD & dwID);

	/// 从索引转成ID
	inline DWORD  Index2ID(DWORD dwIndex);

	/** 取得时间字串
	@param   DWORD dwTime:时间
	@param   
	@return  LPCSTR
	*/
	LPCSTR GetTimeString(DWORD dwTime);

protected:
	// 用数组提高效率
	CClientUser*		m_list[VOICE_MAX_CLIENT_COUNT];
	DWORD				m_dwCounts;	// 客户端数

	DWORD				m_dwGatewayID;
	std::list<DWORD>	m_IDStock;
};

#endif//__CLIENT_LIST_H__