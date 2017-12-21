/*******************************************************************
** 文件名:	DownloadFile.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-4-27
** 版  本:	1.0
** 描  述:	下载文件类
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __DownloadFile_H__
#define __DownloadFile_H__

#pragma once

// 下载文件类
class CDownloadFile
{
public:

	CDownloadFile(void);

	virtual ~CDownloadFile();

	// 创建
	bool Create(const char * szFileName,const char * szRecvFileName);
	// 释放
	void Release();
	// 取得ID
	DWORD GetID();
	// 设置ID
	void SetID(DWORD dwID);
	// 输出描述
	std::string ToString();
	// 设定文件数据 pData:文件内容,nSize:内容长度
	bool SetFileData(const BYTE * pData,DWORD dwSize);
	// 保存数据到文件
	bool SaveFile();
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

private:
	// 取得内存
	void GetDataMemory(DWORD dwSize);
	// 释放内存
	void FreeDataMemory();
	// 计算MD5码,用来比对文件用
	void MakeMD5Data();
	// 校验比对文件是否一致
	bool CheckVerifyData();
	// 校验比对保存文件是否一致
	bool CheckSaveVerifyData();
private:

	DWORD m_dwID;			// 本地ID
	DWORD m_dwFileSize;		// 文件内容数据长度
	DWORD m_dwMemorySize;	// 内存长度
	BYTE * m_pFileData;		// 数据,析构函数释放

public:

	std::string m_strFileName;			// 文件名
	std::string m_strRecvFileName;		// 收到的文件名
	DWORD m_dwFileEditTime;				// 文件修改时间
	DWORD m_dwRecvFileSize;				// 收到的文件内容数据长度
	bool m_bLoad;						// 文件已加载
	bool m_bVerify;						// 文件校验通过
	bool m_bVerifySave;					// 存入文件校验通过
	bool m_bSave;						// 文件是否保存
	bool m_bBroadcastFile;				// 是广播文件
	DWORD m_dwTypeID;					// 类型ID
	DWORD m_dwState;					// 状态
	std::string m_strTitle;				// 文件信息说明
	unsigned char m_Md5Data[20];		// MD5码数据
	unsigned char m_RecvMd5Data[20];	// 收到的文件MD5码数据
	unsigned char m_SaveMd5Data[20];	// 保存的文件MD5码数据
										// 用来标识目标服务器位置，FileManager用来发数据到指定服务器
	BYTE m_nServerType;					// 服务器类型
	DWORD m_dwUploadID;					// 远程上传数据对象ID
	DWORD m_dwServerData1;				// 服务器标识数据1
	DWORD m_dwServerData2;				// 服务器标识数据2
	DWORD m_dwServerData3;				// 服务器标识数据3

};

#endif//__DownloadFile_H__