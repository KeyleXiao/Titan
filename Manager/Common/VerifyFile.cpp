/*******************************************************************
** 文件名:	VerifyFile.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-4-27
** 版  本:	1.0
** 描  述:	校检文件类
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "VerifyFile.h"
#include "VerifyFileList.h"
#include <strstream>
#include "sha1.h"

///////////////////////////////////////////////////////////////////////////////////////////////
CVerifyFile::CVerifyFile():
	m_dwID(0)
	,m_dwFileSize(0)
	,m_dwMemorySize(0)
	,m_pFileData(0)
	,m_dwTypeID(0)
	,m_strFileName("")
	,m_strName("")
	,m_strTitle("")
	,m_bLoad(false)
	,m_dwState(0)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////
CVerifyFile::~CVerifyFile()
{
	// 释放
	FreeDataMemory();
}
// 创建
bool CVerifyFile::Create()
{
	return true;
}
void CVerifyFile::Release()
{
	VerifyFileList::getInstance().DelUser(this);

	delete this;
}

DWORD CVerifyFile::GetID()
{
	return m_dwID;
}

void CVerifyFile::SetID(DWORD dwID)
{
	m_dwID = dwID;
}

std::string CVerifyFile::ToString()
{
	ostrstream buffer;
	buffer << _GT("校检文件列表[ID=") << m_dwID << _GT(",文件个数=")<<m_mapList.size()<<"]["<<m_strTitle<<"]" << std::ends;
	return buffer.str();
}


// 清除数据
void CVerifyFile::Clear()
{
	// 释放
	FreeDataMemory();
	m_dwTypeID = 0;
	m_dwState = 0;
	m_strFileName = "";
	m_bLoad = false;
	m_mapList.clear();
}

// 清除内存数据
void CVerifyFile::ClearFileData()
{
	// 释放内存
	FreeDataMemory();
}
// 取得内存
void CVerifyFile::GetDataMemory(DWORD dwSize)
{
	// 释放内存
	if (m_pFileData!=NULL)
	{
		delete [](m_pFileData);
		m_pFileData = NULL;
		m_dwMemorySize = 0;
	}
	m_pFileData = (BYTE*)new char[dwSize];
	m_dwMemorySize = dwSize;
}

// 释放内存
void CVerifyFile::FreeDataMemory()
{
	// 释放内存
	if (m_pFileData!=NULL)
	{
		delete [](m_pFileData);
		m_pFileData = NULL;
		m_dwMemorySize = 0;
	}
}

// 设置显示信息
void CVerifyFile::SetTitle(const char * szTitle)
{
	if (szTitle==NULL)
	{
		m_strTitle = "";
	}
	m_strTitle = szTitle;
}
// 取得打包数据
const BYTE * CVerifyFile::GetFileData()
{
	return m_pFileData;
}
// 取得打包数据长度
DWORD CVerifyFile::GetFileSize()
{
	return m_dwFileSize;
}

// 从文件中加载校检列表信息
bool CVerifyFile::LoadFromFile(const char * szFileName)
{
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
			buffer << _GT("从文件中加载校检列表信息文件大小(")<<nFileSize<<_GT(")不能大于(")<< dwMaxFileSize <<")" << std::ends;
			SetTitle(buffer.str());
			WarningLn(buffer.str());
		}
		else
		{
			DWORD dwReadSize = (DWORD)nFileSize;
			if (dwReadSize>0)
			{
				// new内存
				GetDataMemory(dwReadSize);
				DWORD dwGetFileSize = file.Read(m_pFileData,dwReadSize);
				if (!LoadFromMemory(m_pFileData,dwGetFileSize))
				{
					// 释放内存
					FreeDataMemory();
				}
				else
				{
					m_dwFileSize = dwGetFileSize;
					m_bLoad = true;	// 文件已加载
				}
			}
		}
	}
	else
	{
		TCHAR szError[1024] = {0};
		ex.GetErrorMessage(szError,1024);
		SetTitle(my_t2utf8(szError));
		WarningLn(_GT("从文件中加载校检列表信息出错：")<<my_t2utf8(szError));
	}
	file.Close();

	return m_bLoad;
}
// 从指定目录中加载校检列表信息,checkList为指定列表,removeList为指定排除列表,为空列出目录所有,默认最多10层子目录,bAddMode=true为增加模式
bool CVerifyFile::LoadFromPath(const char * szPathName,bool bAddMode,std::list<string> *checkList,std::list<string> *removeList,DWORD dwMaxLayer)
{
	if (NULL==szPathName)
	{
		return false;
	}

	// bAddMode=true为增加模式
	if (!bAddMode)
	{
		m_mapList.clear();
	}

	// 指定排除列表
	bool bHaveRemoveList = false;
	std::map<string,bool> mapRemoveFileList;
	if (removeList!=NULL && removeList->size()>0)
	{
		bHaveRemoveList = true;
		for(std::list<string>::iterator it0 = removeList->begin(); it0!= removeList->end();it0++)
		{
			// 文件名
			string strFileName = (*it0);
			MStringHelper::toLower(strFileName);
			mapRemoveFileList[strFileName] = true;
		}
	}

	std::list<string> * pFileList = checkList;

	std::list<string> myFileList;

	if (NULL==pFileList)
	{
		pFileList = &myFileList;
		// 列出目录所有
		GetFileList(szPathName,myFileList);
	} 

	// 按列表加载
	for(std::list<string>::iterator it = pFileList->begin(); it!= pFileList->end();it++)
	{
		// 文件名
		string strFileName = (*it);
		string strCheckFileName = szPathName;
		strCheckFileName += "\\";
		strCheckFileName += strFileName;

		MStringHelper::toLower(strFileName);

		// 有指定排除列表
		if (bHaveRemoveList)
		{
			std::map<string,bool>::iterator iter1 = mapRemoveFileList.find(strFileName);
			if ( iter1!=mapRemoveFileList.end())
			{
				continue;
			}
		}

		SVerifyFileInfo verifyInfo;
		verifyInfo.strFileName = (*it);
		
		// 从指定文件加载校检信息
		if (GetVerifyInfo(strCheckFileName.c_str(),verifyInfo))
		{
			m_mapList[strFileName] = verifyInfo;
		}
	}

	m_bLoad = true;

	return true;
}
// 从内存中加载校检列表信息
bool CVerifyFile::LoadFromMemory(const BYTE * pBufData,DWORD dwSize)
{
	if (NULL==pBufData || dwSize<1)
	{
		return false;
	}

	SSaveVerifyFileHead * pData = (SSaveVerifyFileHead *)pBufData;

	DWORD nListCounts = pData->dwCounts;

#ifdef MANAGER_PRINT_DEBUG_INF
	// 调式代码
	char szBuf[512]={0};
	DWORD nRetSize= nListCounts * sizeof(SSaveVerifyFileNode) + sizeof(SSaveVerifyFileHead);
	sprintf_s(szBuf, sizeof(szBuf), _GT("数据大小 nOutLen =%d： nListCounts=%d * sizeof(SSaveVerifyFileNode)=%d + sizeof(SSaveVerifyFileHead)=%d,nRetSize=%d "),
		dwSize,nListCounts,sizeof(SSaveVerifyFileNode),sizeof(SSaveVerifyFileHead),nRetSize);
	TraceLn(szBuf);
#endif

	if(dwSize <(DWORD)(nListCounts * sizeof(SSaveVerifyFileNode) + sizeof(SSaveVerifyFileHead)))
	{
		ErrorLn(_GT("从内存中加载校检列表信息数据条数与大小不符，nLen = ")<<dwSize);
		return false;
	}
	if (pData->dwHeadFlag!=SAVEVERIFYFILEHEADFLAG)
	{
		ErrorLn(_GT("从内存中加载校检列表信息数据头标识不正确,无效数据！"));
		return false;
	}

	if (pData->dwDataSize!=dwSize)
	{
		WarningLn(_GT("从内存中加载校检列表信息数据头标记数据长度[")<<pData->dwDataSize<<_GT("]与实际[")<<dwSize<<_GT("]不符"));
	}

	m_mapList.clear();
	m_bLoad = false;

	SSaveVerifyFileNode * pInfo = (SSaveVerifyFileNode *)(pBufData + sizeof(SSaveVerifyFileHead));

	DWORD dwNameStrOffset = nListCounts * sizeof(SSaveVerifyFileNode) + sizeof(SSaveVerifyFileHead);
	LPSTR pNameStr = (LPSTR)pBufData + dwNameStrOffset;

	// 文件名
	char szNodeFileName[_MAX_PATH] = {0};

	for (DWORD i=0;i< nListCounts; i++)
	{
		pNameStr = (LPSTR)pBufData + dwNameStrOffset;

		if (pInfo->nStrLens>0)
		{
			sstrcpyn(szNodeFileName,pNameStr,pInfo->nStrLens+1);
		}
		else
		{
			memset(szNodeFileName,0,sizeof(szNodeFileName));
		}

		SVerifyFileInfo addFileNode;
		addFileNode.dwID			= i+1;						// 序号ID
		addFileNode.dwFileSize		= pInfo->dwFileSize;		// 文件大小
		addFileNode.strFileName		= szNodeFileName;			// 文件名
		// 校检码数据
		memcpy(addFileNode.VerifyData,pInfo->VerifyData,sizeof(addFileNode.VerifyData));

		string strLowKeyName = szNodeFileName;
		MStringHelper::toLower(strLowKeyName);

		m_mapList[strLowKeyName] = addFileNode;

#ifdef MANAGER_PRINT_DEBUG_INF
		// 测试代码			
		sprintf_s(szBuf, sizeof(szBuf), _GT("%d.文件大小=%d,文件名长=%d,文件=%s"),
			i,pInfo->dwFileSize,pInfo->nStrLens,szNodeFileName);
		TraceLn(szBuf);
#endif

		dwNameStrOffset += pInfo->nStrLens;
		pInfo++;
	}

	m_bLoad = true;

	return true;
}
// 从另外的校检文件对象加载校检列表信息
bool CVerifyFile::LoadFromOther(CVerifyFile &OtherVerify)
{
	if (!OtherVerify.m_bLoad)
	{
		return false;
	}

	m_mapList.clear();
	m_bLoad = false;

	for(TMAP_FileList::iterator iter = OtherVerify.m_mapList.begin(); iter!=  OtherVerify.m_mapList.end();iter++)
	{
		m_mapList[iter->first] = iter->second;
	}

	m_bLoad = true;

	return true;
}
// 保存校检列表信息到指定文件
bool CVerifyFile::SaveDataToFile(const char * szFileName)
{
	if (szFileName==NULL)
	{
		return false;
	}
	// 取得保存文件数据内容,并分配内存
	GetSaveVerifyData();

	if (NULL==m_pFileData || m_dwFileSize<1)
	{
		return false;
	}

	m_strFileName = szFileName;

	// 文件是否保存
	char szBuf[512] = {0};
	bool bSave = false;

	// 打开文件
	CFile file;
	CFileException ex;
	if (file.Open(my_utf82t(szFileName),CFile::modeWrite|CFile::modeCreate,&ex))
	{
		// 将数据包写入文件
		file.Write(m_pFileData,m_dwFileSize);
		bSave = true;
	}
	else
	{
		TCHAR szError[1024] = {0};
		ex.GetErrorMessage(szError,1024);
		SetTitle(my_t2utf8(szError));
		WarningLn(_GT("保存校检列表信息到指定文件出错：")<< my_t2utf8(szError));
	}
	file.Close();

	return bSave;
}
// 从指定文件加载校检信息
bool CVerifyFile::GetVerifyInfo(const char * szFileName,SVerifyFileInfo &verifyNode)
{
	if (szFileName==NULL)
	{
		return false;
	}

	bool bLoad = false;
	DWORD dwFileSize = 0;

	// 打开文件
	CFile file;
	CFileException ex;
	if (file.Open(my_utf82t(szFileName),CFile::modeRead|CFile::shareDenyNone,&ex))
	{
		ULONGLONG nFileSize = file.GetLength();
		if (nFileSize>MANAGER_VERIFYFILE_MAXSIZE)
		{
			DWORD dwMaxFileSize = MANAGER_VERIFYFILE_MAXSIZE;
			ostrstream buffer;
			buffer << _GT("加载校检文件大小(")<<nFileSize<<_GT(")不能大于(")<< dwMaxFileSize <<")" << std::ends;
			WarningLn(buffer.str());
		}
		else
		{
			DWORD dwReadSize = (DWORD)nFileSize;
			bLoad = true;	// 文件已加载
			if (dwReadSize>0)
			{
				// new内存
				BYTE* pFileData = (BYTE*)new char[dwReadSize+1024];
				// 读出数据
				dwFileSize = file.Read(pFileData,dwReadSize);
			
				verifyNode.dwFileSize = dwFileSize;
				// 计算MD5码,SHA1加密
				memset(verifyNode.VerifyData,0,sizeof(verifyNode.VerifyData));
				if (pFileData!=NULL && dwFileSize>0)
				{
					ctx_sha1 hash;
					hash.update((const unsigned char *)(pFileData),dwFileSize);
					hash.finish(verifyNode.VerifyData);
				}

				// 释放内存
				if (pFileData!=NULL)
				{
					delete [](pFileData);
					pFileData = NULL;
				}
			}
		}
	}
	else
	{
		TCHAR szError[1024] = {0};
		ex.GetErrorMessage(szError,1024);
		ostrstream buffer;
		buffer << _GT("加载校检文件出错：")<< szError << std::ends;
		WarningLn(buffer.str());
	}
	file.Close();

	m_bLoad = bLoad;

	return bLoad;
}

// 从指定目录得到所有文件列表
bool CVerifyFile::GetFileList(const char * szPathName,std::list<string> &checkList,DWORD dwMaxLayer)
{
	if (szPathName==NULL || strlen(szPathName)<=0)
	{
		return false;
	}
	checkList.clear();
	string strFindRootPath = szPathName;
	string strSubPath;
	DWORD dwLayer = 0;

	return DoGetFileList(strFindRootPath,strSubPath,checkList,dwLayer,dwMaxLayer);
}
// 从指定目录得到文件列表
bool CVerifyFile::DoGetFileList(string strRootPathName,string strPathName,std::list<string> &checkList,DWORD &dwLayer,DWORD dwMaxLayer)
{
	//WarningLn(strPathName.c_str());
	if (strRootPathName.size()<=0)
	{
		return false;
	}

	// 要列出的目录名
	string strFind = strRootPathName;
	strFind += "\\*.*";

	CFileFind file;
	BOOL bContinue = file.FindFile(my_utf82t(strFind.c_str()));
	int  nCount = 0;
	bool bRetOK = true;

	while(bContinue)
	{
		bContinue = file.FindNextFile();
		if(!file.IsDots())
		{
			nCount++;
			if (file.IsDirectory())
			{
				if (dwLayer<=dwMaxLayer)
				{
					// 下一层目录
					string strPath = strRootPathName;
					strPath += "\\";
					strPath += my_t2utf8(file.GetFileName().GetString());
					string strSubPath = strPathName;
					if (strPathName.size()>0)
					{
						strSubPath += "\\";
					}
					strSubPath += my_t2utf8(file.GetFileName().GetString());

					dwLayer++;
					bRetOK = DoGetFileList(strPath,strSubPath,checkList,dwLayer,dwMaxLayer);
					dwLayer--;
					if (!bRetOK)
					{
						bContinue = false;
						break;
					}
				}
				else
				{
					WarningLn(_GT("取文件列表当前层数[")<<dwLayer<<_GT("]超过最大层数[")<<dwMaxLayer<<"]:"<<strPathName.c_str());
					bRetOK = false;
					bContinue = false;
					break;
				}
			} 
			else
			{
				if (checkList.size()<=MG_VerifyGetFileListMax)
				{
					string strFileName = strPathName;
					if (strPathName.size()>0)
					{
						strFileName += "\\";
					}
					strFileName += my_t2utf8(file.GetFileName().GetString());

					checkList.push_back(strFileName);

					//WarningLn(strFileName.c_str());
				}
				else
				{
					WarningLn(_GT("取文件列表当前列表数[")<<checkList.size()<<_GT("]超过最大")<<MG_VerifyGetFileListMax<<":"<<strPathName.c_str());
					bRetOK = false;
					bContinue = false;
					break;
				}
			}			
		}
	}
	return bRetOK;
}

// 从列表中找到指定文件名称校检信息
SVerifyFileInfo * CVerifyFile::GetUser(const char * szFileName)
{
	if (NULL==szFileName)
	{
		return NULL;
	}

	string strKey = szFileName;
	MStringHelper::toLower(strKey);

	TMAP_FileList::iterator it = m_mapList.find(strKey);
	if ( it==m_mapList.end())
		return NULL;
	else 
		return &(it->second);
}

// 比较两个校检文件节点信息是否一致,srcNode源,destNode目标
bool CVerifyFile::CheckVerifyNode(SVerifyFileInfo &srcNode,SVerifyFileInfo &destNode)
{
	bool bVerify = true;

	if (srcNode.dwFileSize != destNode.dwFileSize)
	{
		bVerify = false;
	}
	else
	{
		for (int i=0;i<20;i++)
		{
			// 收到的文件MD5码数据 校验比对
			if (srcNode.VerifyData[i]!=destNode.VerifyData[i])
			{
				bVerify = false;
				break;
			}
		}
	}

	return bVerify;
		
}
// 导出所有文件列表
bool CVerifyFile::ExportFileList(std::list<string> &outList)
{
	if (!m_bLoad)
	{
		return false;
	}
	for(TMAP_FileList::iterator iter = m_mapList.begin(); iter!= m_mapList.end();iter++)
	{
		outList.push_back(iter->second.strFileName);
	}
	return true;
}


// 比较两个校检信息是否一致,srcNode源,destNode目标
bool CVerifyFile::CheckVerify(CVerifyFile &srcNode,CVerifyFile &destNode,const char * szLogFileName)
{
	string strSrcName = srcNode.m_strName;
	string strDestName = destNode.m_strName;	

	bool bVerifyOK = true;

	std::list<string> sameList;		// 一样的列表
	std::list<string> diffList;		// 不一样的列表	
	std::list<string> srcList;		// 只有源才有的	
	std::list<string> destList;		// 只有目标才有的

	// 比较所有
	for(TMAP_FileList::iterator iter = srcNode.m_mapList.begin(); iter!= srcNode.m_mapList.end();iter++)
	{
		TMAP_FileList::iterator itFind = destNode.m_mapList.find(iter->first);
		if (itFind==destNode.m_mapList.end())
		{
			srcList.push_back(iter->second.strFileName);
		}
		else 
		{
			// 比较两个校检文件节点信息是否一致,srcNode源,destNode目标
			if (CheckVerifyNode(iter->second,itFind->second))
			{
				sameList.push_back(iter->second.strFileName);
			}
			else
			{
				diffList.push_back(iter->second.strFileName);
			}
		}
	}
	// 只有目标才有的
	for(TMAP_FileList::iterator iter1 = destNode.m_mapList.begin(); iter1!= destNode.m_mapList.end();iter1++)
	{
		TMAP_FileList::iterator itFind = srcNode.m_mapList.find(iter1->first);
		if (itFind==srcNode.m_mapList.end())
		{
			destList.push_back(iter1->second.strFileName);
		}
	}
	
	if (diffList.size()>0 || srcList.size()>0 || destList.size()>0)
	{
		bVerifyOK = false;
	}

	// 为空不建日志
	if (NULL!=szLogFileName)
	{
		WarningLn(_GT("保存比较日志文件:[")<<szLogFileName<<"]");

		char szBuf[512] = {0};
		string strAllLog;
		// 保存日志文件
		CFile file;
		CFileException ex;
		if (file.Open(my_utf82t(szLogFileName),CFile::modeWrite|CFile::modeCreate,&ex))
		{
			char szBuf[512] = {0};
			string strAllLog;
			sprintf_s(szBuf,sizeof(szBuf),_GT("\r\n校检比较日志\r\n源(%d个文件)：%s\r\n目标(%d个文件)：%s\r\n校检结果：%s\r\n"),
				srcNode.m_mapList.size(),strSrcName.c_str(),destNode.m_mapList.size(),strDestName.c_str(),bVerifyOK?_GT("成功"):_GT("失败"));
			strAllLog += szBuf;
			sprintf_s(szBuf,sizeof(szBuf),_GT("\r\n不同的(%d个文件)：\r\n"),diffList.size());
			strAllLog += szBuf;

			for(std::list<string>::iterator iter2 = diffList.begin(); iter2!= diffList.end();iter2++)
			{
				strAllLog += "\r\n\t";
				strAllLog += (*iter2);
			}
			sprintf_s(szBuf,sizeof(szBuf),_GT("\r\n只有源[%s]才有的(%d个文件)：\r\n"),strSrcName.c_str(),srcList.size());
			strAllLog += szBuf;
			
			for(std::list<string>::iterator iter3 = srcList.begin(); iter3!= srcList.end();iter3++)
			{
				strAllLog += "\r\n\t";
				strAllLog += (*iter3);
			}

			sprintf_s(szBuf,sizeof(szBuf),_GT("\r\n只有目标[%s]才有的(%d个文件)：\r\n"),strDestName.c_str(),destList.size());
			strAllLog += szBuf;

			for(std::list<string>::iterator iter4 = destList.begin(); iter4!= destList.end();iter4++)
			{
				strAllLog += "\r\n\t";
				strAllLog += (*iter4);
			}

			sprintf_s(szBuf,sizeof(szBuf),_GT("\r\n一样的(%d个文件)：\r\n"),sameList.size());
			strAllLog += szBuf;

			for(std::list<string>::iterator iter5 = sameList.begin(); iter5!= sameList.end();iter5++)
			{
				strAllLog += "\r\n\t";
				strAllLog += (*iter5);
			}

			// 将数据包写入文件
			file.Write(strAllLog.c_str(),strAllLog.size());
		}
		else
		{
			TCHAR szBuf[512] = {0};
			ex.GetErrorMessage(szBuf,sizeof(szBuf));
			WarningLn(_GT("保存比较日志文件出错: ")<<my_t2utf8(szBuf));
		}
		file.Close();
	}

	return bVerifyOK;
}

// 取得保存文件数据内容,并分配内存
void CVerifyFile::GetSaveVerifyData()
{
	m_dwFileSize = 0;

	obuf2048 obufNode;
	obuf2048 obufName;

	DWORD dwCounts = 0;
	for(TMAP_FileList::iterator iter = m_mapList.begin(); iter!= m_mapList.end();iter++)
	{
		dwCounts++;
		// 保存校检文件节点信息
		SSaveVerifyFileNode addNode;
		addNode.dwFileSize		= iter->second.dwFileSize;				// 文件大小
		addNode.nStrLens		= iter->second.strFileName.size();		// 文件名长度
		// 校检码数据
		memcpy(addNode.VerifyData,iter->second.VerifyData,sizeof(addNode.VerifyData));

		obufNode.push_back(&addNode, sizeof(SSaveVerifyFileNode));
		if (addNode.nStrLens>0)
		{
			obufName.push_back(iter->second.strFileName.c_str(),addNode.nStrLens);
		}
	}


	// 文件内容数据长度
	DWORD dwMaxSendLens = obufNode.size()+obufName.size()+sizeof(SSaveVerifyFileHead);

	// 取得内存
	GetDataMemory(dwMaxSendLens);

	if (NULL==m_pFileData)
	{
		return;
	}

	ofixbuf obufData((char *)m_pFileData, dwMaxSendLens);

	// 保存校检文件头信息
	SSaveVerifyFileHead saveHead;
	saveHead.dwCounts			= dwCounts;					// 节点数
	saveHead.dwDataSize			= dwMaxSendLens;			// 总数据大小
	saveHead.dwDateTime			= (DWORD)time(NULL);		// 时间
	saveHead.dwType				= 0;						// 类型

	obufData.push_back(&saveHead, sizeof(saveHead));
	if (obufNode.size()>0)
	{
		obufData.push_back(obufNode.data(),obufNode.size());
	}
	if (obufName.size()>0)
	{
		obufData.push_back(obufName.data(),obufName.size());
	}

	m_dwFileSize = obufData.size();

}