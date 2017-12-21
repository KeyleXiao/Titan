/*******************************************************************
** 文件名:	RemoveLogFile.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-6-10
** 版  本:	1.0
** 描  述:	清除Log文件类
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "RemoveLogFile.h"
#include <io.h>

///////////////////////////////////////////////////////////////////////////////////////////////
CRemoveLogFile::CRemoveLogFile():
	m_dwTotalFileSize(0)
	,m_dwRemoveFileSize(0)
	,m_dwRemoveOKCounts(0)
	,m_dwRemoveFailCounts(0)
	,m_strTitle("")
	,m_bLoad(false)
	,m_dwState(0)
	,m_dwMaxIndex(0)
	,m_dwPreTimes(0)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////
CRemoveLogFile::~CRemoveLogFile()
{

}
// 创建,dwPreTimes 处理在些时间前的,要处理的文件扩展名列表,无时默认为[.html,.txt,.log]
bool CRemoveLogFile::Create(DWORD dwPreTimes,std::list<string> *checkExtList)
{
	m_dwPreTimes = dwPreTimes;
	m_checkExtList.clear();
	if (NULL==checkExtList)
	{
		string strExtKey = ".html";
		m_checkExtList[strExtKey] = true;
		strExtKey = ".txt";
		m_checkExtList[strExtKey] = true;
		strExtKey = ".log";
		m_checkExtList[strExtKey] = true;
		strExtKey = ".dmp";
		m_checkExtList[strExtKey] = true;
	}
	else
	{
		// 按列表加载
		for(std::list<string>::iterator it = checkExtList->begin(); it!= checkExtList->end();it++)
		{
			string strFileExtName = (*it);
			MStringHelper::toLower(strFileExtName);
			m_checkExtList[strFileExtName] = true;
		}
	}
	return true;
}
void CRemoveLogFile::Release()
{
	delete this;
}

// 设置显示信息
void CRemoveLogFile::SetTitle(const char * szTitle)
{
	if (szTitle==NULL)
	{
		m_strTitle = "";
	}
	m_strTitle = szTitle;
}
// 从指定目录中加载清除Log列表信息,默认最多10层子目录,bAddMode=true为增加模式
bool CRemoveLogFile::LoadFromPath(const char * szPathName,bool bAddMode,DWORD dwMaxLayer)
{
	if (NULL==szPathName)
	{
		return false;
	}

	// bAddMode=true为增加模式
	if (!bAddMode)
	{
		m_fileList.clear();
		m_dwMaxIndex = 0;
	}

	// 列出目录所有
	if (GetFileList(szPathName))
	{
		m_bLoad = true;
	}

	return true;
}

// 删除列表中的文件
bool CRemoveLogFile::RemoveAllFile(bool bShowLog,const char * szLogFileName)
{
	if (!m_bLoad)
	{
		return false;
	}
	
	char szBuf[512] = {0};
	string strAllLog;
	DWORD dwNowTimes = (DWORD)time(NULL);
	
	// 要处理的文件扩展名
	string strAllExtList;	
	int nExtIndex = 0;
	for(std::map<string,bool>::iterator iter = m_checkExtList.begin(); iter!= m_checkExtList.end();iter++)
	{
		if (nExtIndex==0)
		{
			strAllExtList = iter->first;
		}
		else
		{
			strAllExtList += ",";
			strAllExtList += iter->first;
		}
		nExtIndex++;
	}

	sprintf_s(szBuf,sizeof(szBuf),_GT("\r\n批量删除Log文件 \r\n操作时间：%s \r\n删除时间前：%s \r\n文件类型：%s \r\n"),
		GetTimeString(dwNowTimes),GetTimeString(m_dwPreTimes),strAllExtList.c_str());
	strAllLog += szBuf;

	WarningLn(szBuf);

	m_dwTotalFileSize = 0;		// 总文件内容数据长度
	m_dwRemoveFileSize = 0;		// 删除总文件内容数据长度
	m_dwRemoveOKCounts = 0;		// 删除成功个数
	m_dwRemoveFailCounts = 0;	// 删除失败个数

	// 所有的文件列表
	for(std::list<SRemoveLogFileInfo>::iterator it = m_fileList.begin(); it!= m_fileList.end();it++)
	{
		bool bRemoveOK = false;
		SRemoveLogFileInfo &fileInfo = (*it);

		m_dwTotalFileSize += fileInfo.dwFileSize;	// 总文件内容数据长度

		// 删除同名文件相关文件
		try
		{
			if (_access(fileInfo.strFileName.c_str(), 0) != -1)
			{
				if (!DeleteFile(my_utf82t(fileInfo.strFileName.c_str())))
				{
					sprintf_s(szBuf,sizeof(szBuf),_GT("\r\n删除失败 \t%s \t\t%s"),GetFileSizeString(fileInfo.dwFileSize),fileInfo.strFileName.c_str());
					strAllLog += szBuf;
					if (bShowLog)
					{
						sprintf_s(szBuf,sizeof(szBuf),_GT("删除失败 \t%s \t\t%s"),GetFileSizeString(fileInfo.dwFileSize),fileInfo.strFileName.c_str());
						WarningLn(szBuf);
					}
				}
				else
				{
					bRemoveOK = true;
					sprintf_s(szBuf,sizeof(szBuf),_GT("\r\n　　成功 \t%s \t\t%s"),GetFileSizeString(fileInfo.dwFileSize),fileInfo.strFileName.c_str());
					strAllLog += szBuf;
					if (bShowLog)
					{
						sprintf_s(szBuf,sizeof(szBuf),_GT("　　成功 \t%s \t\t%s"),GetFileSizeString(fileInfo.dwFileSize),fileInfo.strFileName.c_str());
						WarningLn(szBuf);
					}
				}
			}
		}
		catch (CException* e)
		{
			TCHAR szMsgBuf[512] = {0};
			e->GetErrorMessage(szMsgBuf,sizeof(szMsgBuf));

			sprintf_s(szBuf,sizeof(szBuf),_GT("\r\n删除出错 \t%s"),my_t2utf8(szMsgBuf));
			strAllLog += szBuf;
			if (bShowLog)
			{
				sprintf_s(szBuf,sizeof(szBuf),_GT("删除出错 \t%s"),my_t2utf8(szMsgBuf));
				WarningLn(szBuf);
			}
		}
		if (bRemoveOK)
		{
			m_dwRemoveFileSize += fileInfo.dwFileSize;	// 删除总文件内容数据长度
			m_dwRemoveOKCounts++;		// 删除成功个数
		}
		else
		{
			m_dwRemoveFailCounts++;	// 删除失败个数
		}
	}

	string strAllSize = GetFileSizeString(m_dwTotalFileSize);
	string strRemoveSize = GetFileSizeString(m_dwRemoveFileSize);

	sprintf_s(szBuf,sizeof(szBuf),_GT("\r\n\r\n删除Log文件结果: 总大小=%s,共删除=%s,删除成功个数=%d,删除失败个数=%d"),
		strAllSize.c_str(),strRemoveSize.c_str(),m_dwRemoveOKCounts,m_dwRemoveFailCounts);
	strAllLog += szBuf;

	sprintf_s(szBuf,sizeof(szBuf),_GT("删除Log文件结果: 总大小=%s,共删除=%s,删除成功个数=%d,删除失败个数=%d"),
		strAllSize.c_str(),strRemoveSize.c_str(),m_dwRemoveOKCounts,m_dwRemoveFailCounts);
	WarningLn(szBuf);


	if (NULL!=szLogFileName)
	{
		// 保存日志文件
		WarningLn(_GT("保存清除Log结果日志文件:[")<<szLogFileName<<"]");

		CFile file;
		CFileException ex;
		if (file.Open(my_utf82t(szLogFileName),CFile::modeWrite|CFile::modeCreate,&ex))
		{
			// 将数据包写入文件
			file.Write(strAllLog.c_str(),strAllLog.size());
		}
		else
		{
			TCHAR buf[512] = {0};
			ex.GetErrorMessage(buf,sizeof(buf));
			WarningLn(_GT("保存清除Log结果日志文件出错: ")<<my_t2utf8(buf));
		}
		file.Close();

	}
	return true;
}


// 从指定目录得到所有文件列表
bool CRemoveLogFile::GetFileList(const char * szPathName,DWORD dwMaxLayer)
{
	if (szPathName==NULL || strlen(szPathName)<=0)
	{
		return false;
	}
	string strFindRootPath = szPathName;
	string strSubPath;
	DWORD dwLayer = 0;

	return DoGetFileList(strFindRootPath,strSubPath,dwLayer,dwMaxLayer);
}
// 从指定目录得到文件列表
bool CRemoveLogFile::DoGetFileList(string strRootPathName,string strPathName,DWORD &dwLayer,DWORD dwMaxLayer)
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
					bRetOK = DoGetFileList(strPath,strSubPath,dwLayer,dwMaxLayer);
					dwLayer--;
					if (!bRetOK)
					{
						break;
					}
				}
				else
				{
					WarningLn(_GT("取文件列表当前层数[")<<dwLayer<<_GT("]超过最大层数[")<<dwMaxLayer<<"]:"<<strPathName.c_str());
					bRetOK = false;
					break;
				}
			} 
			else
			{
				if (m_fileList.size()<=MG_RemoveLogGetFileListMax)
				{
					string strFileName = my_t2utf8(file.GetFilePath().GetString());

					bool bCheckOK = true;

					// 时间检查条件
					if (0!=m_dwPreTimes)
					{
						CTime fileEditTime;
						file.GetLastWriteTime(fileEditTime);
						DWORD dwFileEditTime = (DWORD)fileEditTime.GetTime();
						if (dwFileEditTime>m_dwPreTimes)
						{
							bCheckOK = false;
						}
					}
					if (bCheckOK)
					{
						// 文件类型检查
						char szPath[_MAX_PATH] = {0};
						char szDrive[_MAX_DRIVE] = {0};
						char szDir[_MAX_DIR] = {0};
						char szFName[_MAX_FNAME] = {0};
						char szExt[_MAX_EXT] = {0};

						sstrcpyn(szPath,strFileName.c_str(), sizeof(szPath));
						_splitpath(szPath,szDrive,szDir,szFName,szExt);

						string strLowKeyName = szExt;
						MStringHelper::toLower(strLowKeyName);

						std::map<string,bool>::iterator iter1 = m_checkExtList.find(strLowKeyName);
						if ( iter1==m_checkExtList.end())
						{
							bCheckOK = false;
						}
					}
					// 符合条件,加入到列表
					if (bCheckOK)
					{
						m_dwMaxIndex++;
						SRemoveLogFileInfo addNode;
						addNode.dwID			= m_dwMaxIndex;						// 序号ID
						addNode.dwFileSize		= file.GetLength();					// 文件大小
						addNode.strFileName		= strFileName;						// 文件名

						m_fileList.push_back(addNode);

						//WarningLn(strFileName.c_str());
					}
				}
				else
				{
					WarningLn(_GT("取文件列表当前列表数[")<<m_fileList.size()<<_GT("]超过最大")<<MG_RemoveLogGetFileListMax<<":"<<strPathName.c_str());
					bRetOK = false;
					break;
				}
			}			
		}
	}
	return bRetOK;
}


