/*******************************************************************
** 文件名:	DownloadFileList.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-4-27
** 版  本:	1.0
** 描  述:	下载文件管理列表
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __DownloadFileList_H__
#define __DownloadFileList_H__

#pragma once

#include "DownloadFile.h"
#include "Singleton.h"

#include <map>
using namespace std;



/**
@name : 下载文件管理列表
@brief:
*/
class DownloadFileList : public Singleton<DownloadFileList>
{
public:

	typedef std::map<DWORD,CDownloadFile*> TMAP_FileList;

	TMAP_FileList m_mapList;

	bool AddUser(CDownloadFile * pFile);

	bool DelUser(CDownloadFile * pFile);

	int  Count() { return m_mapList.size(); }

	void Clear(void);

	CDownloadFile * GetUser(DWORD dwID);

	// 构造函数
	DownloadFileList(void);

	// 析构函数
	virtual ~DownloadFileList(void);

protected:

	DWORD m_dwMaxID;

public:
	DWORD m_dwAllDataLens;
};

#endif//__DownloadFileList_H__
