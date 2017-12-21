/*******************************************************************
** 文件名:	UploadFile.cpp 
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

#include "stdafx.h"
#include "UploadFile.h"
#include "UploadFileList.h"
#include <strstream>
#include "sha1.h"

///////////////////////////////////////////////////////////////////////////////////////////////
CUploadFile::CUploadFile():
	m_dwID(0)
	,m_dwFileSize(0)
	,m_dwMemorySize(0)
	,m_pFileData(0)
	,m_dwTypeID(0)
	,m_strFileName("")
	,m_strRemoteFileName("")
	,m_strTitle("")
	,m_bLoad(false)
	,m_bAutoRelease(false)
	,m_bBroadcastFile(false)
	,m_dwState(0)
	,m_nServerType(0)
	,m_dwDownloadID(0)
	,m_dwServerData1(0)
	,m_dwServerData2(0)
	,m_dwServerData3(0)
	,m_dwFileEditTime(0)
{
	memset(m_Md5Data,0,sizeof(m_Md5Data));
}

///////////////////////////////////////////////////////////////////////////////////////////////
CUploadFile::~CUploadFile()
{
	// 释放
	FreeDataMemory();

}
// 创建
bool CUploadFile::Create(const char * szFileName)
{
	return OpenFile(szFileName);
}
void CUploadFile::Release()
{

	UploadFileList::getInstance().DelUser(this);

	delete this;
}

DWORD CUploadFile::GetID()
{
	return m_dwID;
}

void CUploadFile::SetID(DWORD dwID)
{
	m_dwID = dwID;
}

std::string CUploadFile::ToString()
{
	char szBuf[256] = {0};
	string_to_hex((const unsigned char *)m_Md5Data,sizeof(m_Md5Data),(char *)szBuf);
	ostrstream buffer;
	//buffer << "上传文件[ID=" << m_dwID << ",文件名="<<m_strFileName<<",上传名="<<m_strRemoteFileName<<",占内存="<<m_dwMemorySize<<",长度="<<m_dwFileSize<<",MD5="<<szBuf<<",状态="<<m_dwState<<":"<<GetFileUploadStateString(m_dwState)<<"]["<<m_strTitle<<"]" << std::ends;
	buffer << _GT("上传文件[ID=") << m_dwID << _GT(",文件名=")<<m_strFileName<<_GT(",长度=")<<m_dwFileSize<<_GT(",状态=")<<m_dwState<<":"<<GetFileUploadStateString(m_dwState)<<"]["<<m_strTitle<<"]" << std::ends;
	return buffer.str();
}

// 取得内存
void CUploadFile::GetDataMemory(DWORD dwSize)
{
	// 释放内存
	if (m_pFileData!=NULL)
	{
		delete [](m_pFileData);
		m_pFileData = NULL;
		UploadFileList::getInstance().m_dwAllDataLens -=m_dwMemorySize;
		m_dwMemorySize = 0;
	}
	m_pFileData = (BYTE*)new char[dwSize];
	m_dwMemorySize = dwSize;
	UploadFileList::getInstance().m_dwAllDataLens += m_dwMemorySize;
}

// 释放内存
void CUploadFile::FreeDataMemory()
{
	// 释放内存
	if (m_pFileData!=NULL)
	{
		delete [](m_pFileData);
		m_pFileData = NULL;
		UploadFileList::getInstance().m_dwAllDataLens -=m_dwMemorySize;
		m_dwMemorySize = 0;
	}
}

// 清除数据
void CUploadFile::Clear()
{
	// 释放内存
	if (m_pFileData!=NULL)
	{
		delete [](m_pFileData);
		m_pFileData = NULL;
		UploadFileList::getInstance().m_dwAllDataLens -=m_dwMemorySize;
		m_dwMemorySize = 0;
	}
	m_dwFileSize = 0;
	m_dwTypeID = 0;
	m_dwState = 0;
	m_strFileName = "";
	m_bLoad = false;
	memset(m_Md5Data,0,sizeof(m_Md5Data));
}

// 清除内存数据
void CUploadFile::ClearFileData()
{
	// 释放内存
	FreeDataMemory();
}

// 设置显示信息
void CUploadFile::SetTitle(const char * szTitle)
{
	if (szTitle==NULL)
	{
		m_strTitle = "";
	}
	m_strTitle = szTitle;
}

// 取得数据
const BYTE * CUploadFile::GetFileData()
{
	return m_pFileData;
}
// 取得文件数据长度
DWORD CUploadFile::GetFileSize()
{
	return m_dwFileSize;
}

// 设置上传的文件名
bool CUploadFile::SetRemoteFileName(const char * szFileName)
{
	if (szFileName==NULL)
	{
		return false;
	}
	m_strRemoteFileName = szFileName;
	return true;
}


// 计算MD5码,用来比对文件用
void CUploadFile::MakeMD5Data()
{
	// SHA1加密
	memset(m_Md5Data,0,sizeof(m_Md5Data));
	if (m_pFileData!=NULL && m_dwFileSize>0)
	{
		ctx_sha1 hash;
		hash.update((const unsigned char *)(m_pFileData),m_dwFileSize);
		hash.finish(m_Md5Data);
	}
}

// 从文件中加载
bool CUploadFile::OpenFile(const char * szFileName)
{
	m_dwState = MG_FileUploadState_LoadFileFail;	// 加载文件失败

	if (szFileName==NULL)
	{
		return false;
	}
	m_strFileName = szFileName;
	m_bLoad = false;
	m_dwFileSize = 0;

	// 释放内存
	FreeDataMemory();

	// 打开文件
	CFile file;
	CFileException ex;
	if (file.Open(my_utf82t(szFileName),CFile::modeRead|CFile::shareDenyNone,&ex))
	{
		ULONGLONG nFileSize = file.GetLength();
		if (nFileSize>MANAGER_UPLOADFILE_MAXSIZE)
		{
			DWORD dwMaxFileSize = MANAGER_UPLOADFILE_MAXSIZE;
			ostrstream buffer;
			buffer << _GT("文件大小(")<<nFileSize<<_GT(")不能大于(")<< dwMaxFileSize <<")" << std::ends;
			SetTitle(buffer.str());
		}
		else
		{
			DWORD dwReadSize = (DWORD)nFileSize;
			// 文件修改时间
			CFileStatus fileStatus;
			if (file.GetStatus(fileStatus))
			{
				m_dwFileEditTime = (DWORD)fileStatus.m_mtime.GetTime();
			}
			m_bLoad = true;	// 文件已加载
			if (dwReadSize>0)
			{
				// new内存
				GetDataMemory(dwReadSize);
				m_dwFileSize = file.Read(m_pFileData,dwReadSize);
				// 计算MD5码
				MakeMD5Data();
			}
		}
	}
	else
	{
		TCHAR szError[1024] = {0};
		ex.GetErrorMessage(szError,1024);
		m_strTitle = my_t2utf8(szError);
		SetTitle(m_strTitle.c_str());
	}
	file.Close();

	if (m_bLoad)
	{
		m_dwState = MG_FileUploadState_LoadFileOK;	// 已加载文件
	}

	return m_bLoad;

}
// 从内存中加载 szFileName:文件名,pData:文件内容,nSize:内容长度
bool CUploadFile::LoadMemoryFile(const char * szFileName,const BYTE * pData,DWORD dwSize)
{
	m_dwState = MG_FileUploadState_LoadFileFail;	// 加载文件失败

	if (szFileName==NULL || pData==NULL )
	{
		return false;
	}
	m_strFileName = szFileName;
	m_dwFileSize = dwSize;

	// 释放内存
	FreeDataMemory();
	if (dwSize>0)
	{
		// new内存
		GetDataMemory(dwSize);
		// copy内存
		memcpy(m_pFileData,pData,dwSize);
		// 计算MD5码
		MakeMD5Data();
	}

	m_dwState = MG_FileUploadState_LoadFileOK;	// 已加载文件

	return true;
}