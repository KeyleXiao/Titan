/*******************************************************************
** 文件名:	UploadFileList.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-4-27
** 版  本:	1.0
** 描  述:	上传文件管理列表
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __UploadFileList_H__
#define __UploadFileList_H__

#pragma once

#include "UploadFile.h"
#include "Singleton.h"

#include <map>
using namespace std;



/**
@name : 上传文件管理列表
@brief:
*/
class UploadFileList : public Singleton<UploadFileList>
{
public:

	typedef std::map<DWORD,CUploadFile*> TMAP_FileList;

	TMAP_FileList m_mapList;

	bool AddUser(CUploadFile * pFile);

	bool DelUser(CUploadFile * pFile);

	int  Count() { return m_mapList.size(); }

	void Clear(void);

	CUploadFile * GetUser(DWORD dwID);

	// 构造函数
	UploadFileList(void);

	// 析构函数
	virtual ~UploadFileList(void);

protected:

	DWORD m_dwMaxID;

public:
	DWORD m_dwAllDataLens;
};

#endif//__UploadFileList_H__
