/*******************************************************************
** 文件名:	UploadFile.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-4-27
** 版  本:	1.0
** 描  述:	上传文件类
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __UploadFile_H__
#define __UploadFile_H__

#pragma once


// 上传文件类
class CUploadFile
{

public:

	CUploadFile();
	
	virtual ~CUploadFile();

	// 创建
	bool Create(const char * szFileName);
	// 释放
	void Release();
	// 取得ID
	DWORD GetID();
	// 设置ID
	void SetID(DWORD dwID);
	// 输出描述
	std::string ToString();
	// 从文件中加载
	bool OpenFile(const char * szFileName);
	// 从内存中加载 szFileName:文件名,pData:文件内容,nSize:内容长度
	bool LoadMemoryFile(const char * szFileName,const BYTE * pData,DWORD dwSize);
	// 清除数据
	void Clear();
	// 清除内存数据
	void ClearFileData();
	// 设置显示信息
	void SetTitle(const char * szTitle);
	// 取得数据
	const BYTE * GetFileData();
	// 取得文件数据长度
	DWORD GetFileSize();
	// 设置上传的文件名
	bool SetRemoteFileName(const char * szFileName);

private:
	// 取得内存
	void GetDataMemory(DWORD dwSize);
	// 释放内存
	void FreeDataMemory();
	// 计算MD5码,用来比对文件用
	void MakeMD5Data();

private:

	DWORD m_dwID;			// 本地ID
	DWORD m_dwFileSize;		// 文件内容数据长度
	DWORD m_dwMemorySize;	// 内存长度
	BYTE * m_pFileData;		// 数据,析构函数释放

public:

	std::string m_strFileName;		// 文件名
	std::string m_strRemoteFileName;		// 上传的文件名
	DWORD m_dwFileEditTime;			// 文件修改时间
	bool m_bLoad;					// 文件已加载
	bool m_bAutoRelease;			// 传完自动释放
	bool m_bBroadcastFile;			// 是广播文件
	DWORD m_dwTypeID;				// 类型ID
	DWORD m_dwState;				// 状态
	std::string m_strTitle;			// 文件信息说明
	unsigned char m_Md5Data[20];	// MD5码数据
									// 用来标识目标服务器位置，FileManager用来发数据到指定服务器
	BYTE m_nServerType;				// 服务器类型
	DWORD m_dwDownloadID;			// 远程下载数据对象ID
	DWORD m_dwServerData1;			// 服务器标识数据1
	DWORD m_dwServerData2;			// 服务器标识数据2
	DWORD m_dwServerData3;			// 服务器标识数据3
};

#endif//__UploadFile_H__