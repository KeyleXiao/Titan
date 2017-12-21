/*******************************************************************
** 文件名:	RealTimeDataSrc.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-9-11
** 版  本:	1.0
** 描  述:	服务器实时数据源处理类
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#ifndef __RealTimeDataSrc_H__
#define __RealTimeDataSrc_H__

#include "DManagerModule.h"
#include <bitset>



// 软件服务器实时数据监听用户
struct SManagerRealTimeDataUser
{
	DWORD dwClientID;		// 客户端ID
	DWORD dwTimeout;		// 超时时间

	SManagerRealTimeDataUser(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// 软件服务器实时数据监听用户,以客户端ID为关键字
typedef map<DWORD,SManagerRealTimeDataUser> TMAP_RealTimeDataUser;

/**
@name : 服务器实时数据源处理类
@brief:
*/
class CRealTimeDataSrc
{

private:
	WORD m_wServerType;			// 服务器类型
	WORD m_wSubID;				// 第n号服务器
	bitset<MANAGER_REALTIMEDATA_MAXID> m_bitDataFlag;			// 有数据的数据项的映射信息
	int m_nDataValue[MANAGER_REALTIMEDATA_MAXID];				// 实时数据
	char m_szBufData[MANAGER_RTBUFDATA_MAXSIZE];				// 状态监听数据
	DWORD m_dwRTVisitorType;									// 状态监听类型
	DWORD m_dwBufLen;											// 状态监听数据长度
public:

	CRealTimeDataSrc();

	virtual ~CRealTimeDataSrc();

	/** 取得指定服务器类型实时数据ID最大值
	WORD wServerType		: 服务器类型
	@return  DWORD 数据ID最大值
	*/
	static DWORD GetRTDataMaxID(WORD wServerType);

	/** 取得实时数据ID是否开启
	WORD wServerType	: 服务器类型
	DWORD dwDataID		: 实时数据类型ID
	@return  bool 是否开启
	*/
	static bool IsRTDataIDEnable(DWORD dwServerType,DWORD dwDataID);

	/** 取得状态监听是否开启
	WORD wServerType	: 服务器类型
	DWORD dwRTVisitorType	: 状态监听类型
	@return  bool 是否开启
	*/
	static bool IsRTVisitorEnable(DWORD dwServerType,DWORD dwRTVisitorType);

	/** 创建服务器实时数据源处理类
	WORD wServerType	: 服务器类型
	WORD wSubID			: 第n号服务器
	@return  
	*/
	bool Create(WORD wServerType,WORD wSubID);

	/** 设置实时数据
	DWORD dwDataID		: 实时数据类型ID,不同的服务器类型用不用的ID 参考 EMManagerRealTimeDataLogin
	int nValue			: 数值
	@return  是否成功设置
	*/
	bool SetData(DWORD dwDataID,int nValue);

	/** 取得实时数据
	DWORD dwDataID		: 实时数据类型ID
	@return  int nValue	: 数值
	*/
	int GetData(DWORD dwDataID);

	/** 检查实时数据ID是否有效
	DWORD dwDataID		: 实时数据类型ID
	@return  bool 是否有效
	*/
	bool IsGoodRTDataID(DWORD dwDataID);

	/** 检查实时数据是否变动
	DWORD dwDataID		: 实时数据类型ID
	@return  bool 是否有变动
	*/
	bool IsDataChanged(DWORD dwDataID);

	// 清除所有数据变更标识
	void ReSetDataFlag();

	// 取得所有数据变更标识
	DWORD GetDataChangedFlag();

	// 取得服务器类型
	WORD GetServerType();

	// 取得第n号服务器序号
	WORD GetServerSubID();

	/** 导出实时数据数据现场
	@param   LPSTR pszContext	: 导出buffer
	@param   int &nLen			: buffer长度,返回时是数据长度
	@return  BYTE 导出数据个数
	*/
	BYTE ExportContext(LPSTR pszContext, int &nLen);

	/** 设置状态监听数据
	DWORD dwRTVisitorType	: 软件服务器状态监听类型 参考 EMManagerRealTimeVisitorType
	LPSTR pszContext	: 设置buffer
	int nLen			: buffer长度
	@return  是否成功设置
	*/
	bool SetVisitorData(DWORD dwRTVisitorType,LPSTR pszContext, int nLen);

	/** 取得状态监听数据
	DWORD &dwRTVisitorType	: 取得数据的,软件服务器状态监听类型 参考 EMManagerRealTimeVisitorType
	LPSTR &pszContext	: 取得数据buffer指针
	int &nLen			: 取得数据buffer长度
	@return  是否成功取得
	*/
	bool GetVisitorData(DWORD &dwRTVisitorType,LPSTR &pszContext,int &nLen);

};

#endif//__RealTimeDataSrc_H__