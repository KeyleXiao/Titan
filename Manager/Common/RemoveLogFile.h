/*******************************************************************
** 文件名:	RemoveLogFile.h 
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

#ifndef __RemoveLogFile_H__
#define __RemoveLogFile_H__

#pragma once

#include <list>
#include <map>
using namespace std;


// 清除Log文件节点信息
struct SRemoveLogFileInfo
{
	DWORD dwID;						// 序号ID
	ULONGLONG dwFileSize;			// 文件大小
	string strFileName;				// 文件名

	SRemoveLogFileInfo(void)
	{
		clear();
	};
	void clear(void)
	{
		dwID = 0;
		dwFileSize = 0;
		strFileName.clear();
	};
};

// 清除Log文件类
class CRemoveLogFile
{

#define MG_RemoveLogGetFileListMax		2000		// 能取的最多文件数

public:


	CRemoveLogFile();
	
	virtual ~CRemoveLogFile();

	// 创建,dwPreTimes 处理在些时间前的,要处理的文件扩展名列表,无时默认为[.html,.txt,.log]
	bool Create(DWORD dwPreTimes=0,std::list<string> *checkExtList=NULL);
	// 释放
	void Release();
	// 设置显示信息
	void SetTitle(const char * szTitle);
	// 从指定目录中加载清除Log列表信息,默认最多10层子目录,bAddMode=true为增加模式
	bool LoadFromPath(const char * szPathName,bool bAddMode=false,DWORD dwMaxLayer=10);
	// 删除列表中的文件
	bool RemoveAllFile(bool bShowLog=true,const char * szLogFileName=NULL);

private:
	// 方法

	// 从指定目录得到所有文件列表
	bool GetFileList(const char * szPathName,DWORD dwMaxLayer=10);
	// 从指定目录得到文件列表
	bool DoGetFileList(string strRootPathName,string strPathName,DWORD &dwLayer,DWORD dwMaxLayer=10);

public:

	ULONGLONG m_dwTotalFileSize;		// 总文件内容数据长度
	ULONGLONG m_dwRemoveFileSize;		// 删除总文件内容数据长度
	DWORD m_dwRemoveOKCounts;		// 删除成功个数
	DWORD m_dwRemoveFailCounts;		// 删除失败个数
	bool m_bLoad;					// 文件已加载
	DWORD m_dwState;				// 状态
	DWORD m_dwMaxIndex;				// 最大序号
	DWORD m_dwPreTimes;				// 处理在些时间前的
	std::string m_strTitle;			// 文件信息说明
	std::map<string,bool> m_checkExtList;		// 要处理的文件扩展名
	std::list<SRemoveLogFileInfo> m_fileList;	// 所有的文件列表
};

#endif//__RemoveLogFile_H__