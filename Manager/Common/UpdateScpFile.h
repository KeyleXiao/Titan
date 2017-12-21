/*******************************************************************
** 文件名:	UpdateScpFile.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-6-8
** 版  本:	1.0
** 描  述:	刷新脚本对象类
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __UpdateScpFile_H__
#define __UpdateScpFile_H__

#pragma once


// 刷新脚本对象类
class CUpdateScpFile
{

public:

	CUpdateScpFile();
	
	virtual ~CUpdateScpFile();

	// 创建
	bool Create(const char * szRemoteFileName);
	// 释放
	void Release();
	// 取得ID
	DWORD GetID();
	// 设置ID
	void SetID(DWORD dwID);
	// 输出描述
	std::string ToString();
	// 设置显示信息
	void SetTitle(const char * szTitle);

private:

	DWORD m_dwID;			// 本地ID

public:

	std::string m_strFileName;		// 文件名
	bool m_bAutoRelease;			// 传完自动释放
	DWORD m_dwState;				// 状态 EMManagerFileUpdateScpState
	std::string m_strTitle;			// 文件信息说明
									// 用来标识目标服务器位置，FileManager用来发数据到指定服务器
	BYTE m_nServerType;				// 服务器类型
	DWORD m_dwServerData1;			// 服务器标识数据1
	DWORD m_dwServerData2;			// 服务器标识数据2
	DWORD m_dwServerData3;			// 服务器标识数据3
};

#endif//__UpdateScpFile_H__