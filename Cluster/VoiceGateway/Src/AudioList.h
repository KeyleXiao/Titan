/*******************************************************************
** 文件名:	E:\Rocket\Cluster\VoiceGateway\Src\AudioList.h
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


#ifndef __AUDIO_LIST_H__
#define __AUDIO_LIST_H__

#include "VoiceDef.h"
#include "AudioUser.h"
#include "Singleton.h"
#include <list>
  

/**
@name : 客户端列表
@brief:
*/
class CAudioList : public Singleton<CAudioList>
{
public:
	/**
	@name        : 添加一个客户端用户
	@param client: 用户指针
	@return      : 如果当前已达到最大用户数则返回false
	*/
    bool AddUser(CAudioUser * pClient);

	/**
	@name        : 从用户列表中删除一个客户端用户
	@param client: 用户指针
	@return      : 如果找不到该用户则返回false
	*/
	bool DelUser(CAudioUser * pClient);

	/**
	@name        : 根据ID查找一个用户
	@return      : 如果找不到该用户则返回0
	*/
	CAudioUser * FindUser(const DWORD dwID);

	/**
	@name        : 根据ID查找一个用户
	@return      : 如果找不到该用户则返回0
	*/
	CAudioUser * FindUser( const char * szIP, const WORD nPort );

	/**
	@name        : 统计当前所有用户数
	@return      : 返回用户数
	*/
	DWORD CountUser();

	void Clear(void);

	/// purpose: 设置网关ID
	void  SetGatewayID(DWORD dwID);
	
	/// purpose: 获得网关ID
	DWORD GetGatewayID();


	CAudioList() : m_dwCounts(0)
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
	CAudioUser*			m_list[VOICE_MAX_CLIENT_COUNT];
	DWORD				m_dwCounts;	// 客户端数

	DWORD				m_dwGatewayID;
	std::list<DWORD>	m_IDStock;
};

#endif//__AUDIO_LIST_H__