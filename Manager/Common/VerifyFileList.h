/*******************************************************************
** 文件名:	VerifyFileList.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-4-27
** 版  本:	1.0
** 描  述:	校检文件管理列表
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __VerifyFileList_H__
#define __VerifyFileList_H__

#pragma once

#include "VerifyFile.h"
#include "Singleton.h"

#include <map>
using namespace std;



/**
@name : 校检文件管理列表
@brief:
*/
class VerifyFileList : public Singleton<VerifyFileList>
{
public:

	typedef std::map<DWORD,CVerifyFile*> TMAP_FileList;

	TMAP_FileList m_mapList;

	bool AddUser(CVerifyFile * pFile);

	bool DelUser(CVerifyFile * pFile);

	int  Count() { return m_mapList.size(); }

	void Clear(void);

	CVerifyFile * GetUser(DWORD dwID);

	// 构造函数
	VerifyFileList(void);

	// 析构函数
	virtual ~VerifyFileList(void);

protected:

	DWORD m_dwMaxID;

};

#endif//__VerifyFileList_H__
