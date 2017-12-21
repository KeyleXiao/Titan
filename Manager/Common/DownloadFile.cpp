/*******************************************************************
** 文件名:	DownloadFile.cpp 
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

#include "stdafx.h"
#include "DownloadFile.h"
#include "DownloadFileList.h"
#include <strstream>
#include "sha1.h"

///////////////////////////////////////////////////////////////////////////////////////////////
CDownloadFile::CDownloadFile(void):
	m_dwID(0)
	,m_dwFileSize(0)
	,m_dwMemorySize(0)
	,m_pFileData(0)
	,m_dwRecvFileSize(0)
	,m_dwTypeID(0)
	,m_strFileName("")
	,m_strRecvFileName("")
	,m_strTitle("")
	,m_bLoad(false)
	,m_bVerify(false)
	,m_bVerifySave(false)
	,m_bSave(false)
	,m_bBroadcastFile(false)
	,m_dwState(0)
	,m_nServerType(0)
	,m_dwUploadID(0)
	,m_dwServerData1(0)
	,m_dwServerData2(0)
	,m_dwServerData3(0)
	,m_dwFileEditTime(0)
{

	memset(m_Md5Data,0,sizeof(m_Md5Data));
	memset(m_RecvMd5Data,0,sizeof(m_RecvMd5Data));
	memset(m_SaveMd5Data,0,sizeof(m_SaveMd5Data));
}

///////////////////////////////////////////////////////////////////////////////////////////////
CDownloadFile::~CDownloadFile()
{
	// 释放
	FreeDataMemory();
}
// 创建
bool CDownloadFile::Create(const char * szFileName,const char * szRecvFileName)
{
	if (szFileName==NULL || szRecvFileName==NULL || strlen(szFileName)<2)
	{
		return false;
	}
	m_strFileName = szFileName;
	m_strRecvFileName = szRecvFileName;

	return true;
}
void CDownloadFile::Release()
{

	DownloadFileList::getInstance().DelUser(this);

	delete this;
}

DWORD CDownloadFile::GetID()
{
	return m_dwID;
}

void CDownloadFile::SetID(DWORD dwID)
{
	m_dwID = dwID;
}


std::string CDownloadFile::ToString()
{
	char szBuf[256] = {0};
	string_to_hex((const unsigned char *)m_Md5Data,sizeof(m_Md5Data),(char *)szBuf);
	ostrstream buffer;
	//buffer << "下载文件[ID=" << m_dwID << ",文件名="<<m_strFileName<<",占内存="<<m_dwMemorySize<<",长度="<<m_dwFileSize<<",MD5="<<szBuf<<",校验="<<m_bVerify<<":"<<m_bVerifySave<<",保存="<<m_bSave<<",状态="<<m_dwState<<":"<<GetFileDownloadStateString(m_dwState)<<"]["<<m_strTitle<<"]" << std::ends;
	buffer << _GT("下载文件[ID=") << m_dwID << _GT(",文件名=")<<m_strFileName<<_GT(",长度=")<<m_dwFileSize<<_GT(",校验=")<<m_bVerify<<":"<<m_bVerifySave<<_GT(",保存=")<<m_bSave<<_GT(",状态=")<<m_dwState<<":"<<GetFileDownloadStateString(m_dwState)<<"]["<<m_strTitle<<"]" << std::ends;
	return buffer.str();
}

// 取得内存
void CDownloadFile::GetDataMemory(DWORD dwSize)
{
	// 释放内存
	if (m_pFileData!=NULL)
	{
		delete [](m_pFileData);
		m_pFileData = NULL;
		DownloadFileList::getInstance().m_dwAllDataLens -=m_dwMemorySize;
		m_dwMemorySize = 0;
	}
	m_pFileData = (BYTE*)new char[dwSize];
	m_dwMemorySize = dwSize;
	DownloadFileList::getInstance().m_dwAllDataLens += m_dwMemorySize;
}

// 释放内存
void CDownloadFile::FreeDataMemory()
{
	// 释放内存
	if (m_pFileData!=NULL)
	{
		delete [](m_pFileData);
		m_pFileData = NULL;
		DownloadFileList::getInstance().m_dwAllDataLens -=m_dwMemorySize;
		m_dwMemorySize = 0;
	}
}

// 清除数据
void CDownloadFile::Clear()
{
	// 释放内存
	if (m_pFileData!=NULL)
	{
		delete [](m_pFileData);
		m_pFileData = NULL;
		DownloadFileList::getInstance().m_dwAllDataLens -=m_dwMemorySize;
		m_dwMemorySize = 0;
	}
	m_dwFileSize = 0;
	m_dwTypeID = 0;
	m_strFileName = "";
	m_strRecvFileName = "";
	m_dwRecvFileSize = 0;
	m_bLoad = false;
	m_bVerify = false;
	m_bVerifySave = false;
	m_bSave = false;
	memset(m_Md5Data,0,sizeof(m_Md5Data));
	memset(m_RecvMd5Data,0,sizeof(m_RecvMd5Data));
	memset(m_SaveMd5Data,0,sizeof(m_SaveMd5Data));
}

// 清除内存数据
void CDownloadFile::ClearFileData()
{
	// 释放内存
	FreeDataMemory();
}
// 设置显示信息
void CDownloadFile::SetTitle(const char * szTitle)
{
	if (szTitle==NULL)
	{
		m_strTitle = "";
	}
	m_strTitle = szTitle;
}

// 取得数据
const BYTE * CDownloadFile::GetFileData()
{
	return m_pFileData;
}
// 取得文件数据长度
DWORD CDownloadFile::GetFileSize()
{
	return m_dwFileSize;
}

// 计算MD5码,用来比对文件用
void CDownloadFile::MakeMD5Data()
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
// 设定文件数据 pData:文件内容,nSize:内容长度
bool CDownloadFile::SetFileData(const BYTE * pData,DWORD dwSize)
{
	m_dwFileSize = dwSize;

	m_bLoad	= true; // 文件已加载

	// 释放内存
	FreeDataMemory();
	if (pData!=NULL && dwSize>0)
	{
		// new内存
		GetDataMemory(dwSize);
		// copy内存
		memcpy(m_pFileData,pData,dwSize);
		// 计算MD5码
		MakeMD5Data();
	}

	// 校验比对文件是否一致
	CheckVerifyData();

	return true;
}
// 校验比对文件是否一致
bool CDownloadFile::CheckVerifyData()
{
	m_bVerify = true;

	if (m_dwRecvFileSize != m_dwFileSize)
	{
		m_bVerify = false;
	}
	else
	{
		for (int i=0;i<20;i++)
		{
			// 收到的文件MD5码数据 校验比对
			if (m_Md5Data[i]!=m_RecvMd5Data[i])
			{
				m_bVerify = false;
				break;
			}
		}
	}

	return m_bVerify;
}


// 保存数据到文件
bool CDownloadFile::SaveFile()
{
	// 文件是否保存
	m_bSave = false;

	char szBuf[512] = {0};

	if (m_dwFileSize>MANAGER_UPLOADFILE_MAXSIZE)
	{
		DWORD dwMaxFileSize = MANAGER_UPLOADFILE_MAXSIZE;
		ostrstream buffer;
		buffer << _GT("文件大小(")<<m_dwFileSize<<_GT(")不能大于(")<< dwMaxFileSize <<")" << std::ends;
		SetTitle(buffer.str());
	}
	else if (!m_bLoad || (m_pFileData==NULL && m_dwFileSize>0))
	{
		ostrstream buffer;
		buffer << _GT("无文件相关数据") << std::ends;
		SetTitle(buffer.str());
	}
	else if (!m_bVerify)
	{
		ostrstream buffer;
		buffer << _GT("文件数据校验失败") << std::ends;
		SetTitle(buffer.str());
	}
	else
	{
		// 打开文件
		CFile file;
		CFileException ex;
		if (file.Open(my_utf82t(m_strFileName.data()),CFile::modeWrite|CFile::modeCreate,&ex))
		{
			if (m_dwFileSize>0 && m_pFileData!=NULL)
			{
				// 将数据包写入文件
				file.Write(m_pFileData,m_dwFileSize);
			}
			m_bSave = true;

		}
		else
		{
			TCHAR szError[1024] = {0};
			ex.GetErrorMessage(szError,1024);
			m_strTitle = my_t2utf8(szError);
			SetTitle(m_strTitle.c_str());
		}

		// 修改文件修改时间
		CFileStatus fileStatus;
		BOOL bGetStatusOK = FALSE;

		if (m_bSave && m_dwFileEditTime>0)
		{
			bGetStatusOK = file.GetStatus(fileStatus);
		}
		file.Close();

		// 修改文件修改时间
		if (m_bSave && m_dwFileEditTime>0 && bGetStatusOK)
		{
			fileStatus.m_mtime = m_dwFileEditTime;
			CFile::SetStatus(my_utf82t(m_strFileName.data()),fileStatus);
		}
	}

	// 已保存，校验比对保存文件是否一致
	if (m_bSave)
	{
		m_bSave = CheckSaveVerifyData();
	}

	return m_bSave;
}

// 校验比对保存文件是否一致
bool CDownloadFile::CheckSaveVerifyData()
{
	m_bVerifySave = false;

	if (!m_bLoad)
	{
		ostrstream buffer;
		buffer << _GT("没收到文件数据") << std::ends;
		SetTitle(buffer.str());
	}
	else if(!m_bSave)
	{
		ostrstream buffer;
		buffer << _GT("文件未保存") << std::ends;
		SetTitle(buffer.str());
	}
	else if(!m_bVerify)
	{
		ostrstream buffer;
		buffer << _GT("收到数据校验失败") << std::ends;
		SetTitle(buffer.str());
	}
	else
	{
		// 读出存入的文件数据
		DWORD dwReadFileSize = 0;
		// 打开文件
		CFile file;
		CFileException ex;
		if (file.Open(my_utf82t(m_strFileName.data()),CFile::modeRead|CFile::shareDenyNone,&ex))
		{
			ULONGLONG nFileSize = file.GetLength();
			if (nFileSize>MANAGER_UPLOADFILE_MAXSIZE)
			{
				DWORD dwMaxFileSize = MANAGER_UPLOADFILE_MAXSIZE;
				ostrstream buffer;
				buffer << _GT("校验保存文件大小(")<<nFileSize<<_GT(")不能大于(")<< dwMaxFileSize <<")" << std::ends;
				SetTitle(buffer.str());
			}
			else
			{
				DWORD dwReadSize = (DWORD)nFileSize;
				// 与原数据长度
				if (dwReadSize!=m_dwFileSize)
				{
					ostrstream buffer;
					buffer << _GT("保存文件大小(")<<dwReadSize<<_GT(")与原大小(")<<m_dwFileSize<<_GT(")不符") << std::ends;
					SetTitle(buffer.str());
				}
				else if (dwReadSize==0)	// 0长度,不用校验保存文件MD5码
				{
					m_bVerifySave = true;
				}
				else if (dwReadSize>0)
				{
					// new内存
					BYTE * pReadData = (BYTE*)new char[dwReadSize];
					dwReadFileSize = file.Read(pReadData,dwReadSize);
					// 计算MD5码
					// SHA1加密
					memset(m_SaveMd5Data,0,sizeof(m_SaveMd5Data));
					if (pReadData!=NULL && dwReadFileSize>0)
					{
						ctx_sha1 hash;
						hash.update((const unsigned char *)(pReadData),dwReadFileSize);
						hash.finish(m_SaveMd5Data);
					}

					// 释放内存
					if (pReadData!=NULL)
					{
						delete [](pReadData);
						pReadData = NULL;
					}
					
					bool bCheckPass = true;
					// 校验保存文件MD5码
					for (int i=0;i<20;i++)
					{
						// 收到的文件MD5码数据 校验比对
						if (m_SaveMd5Data[i]!=m_RecvMd5Data[i])
						{
							bCheckPass = false;
							break;
						}
					}

					if (!bCheckPass)
					{
						ostrstream buffer;
						buffer << _GT("保存文件数据MD5码校验失败") << std::ends;
						SetTitle(buffer.str());
					}

					m_bVerifySave = bCheckPass;
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

	}

	return m_bVerifySave;

}