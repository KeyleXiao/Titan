/*******************************************************************
** 文件名:	UpdateScpFileList.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-6-8
** 版  本:	1.0
** 描  述:	刷新脚本对象管理列表
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __UpdateScpFileList_H__
#define __UpdateScpFileList_H__

#pragma once

#include "UpdateScpFile.h"
#include "Singleton.h"

#include <map>
using namespace std;



/**
@name : 刷新脚本对象管理列表
@brief:
*/
class UpdateScpFileList : public Singleton<UpdateScpFileList>
{
public:

	typedef std::map<DWORD,CUpdateScpFile*> TMAP_FileList;

	TMAP_FileList m_mapList;

	bool AddUser(CUpdateScpFile * pFile);

	bool DelUser(CUpdateScpFile * pFile);

	int  Count() { return m_mapList.size(); }

	void Clear(void);

	CUpdateScpFile * GetUser(DWORD dwID);

	// 构造函数
	UpdateScpFileList(void);

	// 析构函数
	virtual ~UpdateScpFileList(void);

protected:

	DWORD m_dwMaxID;

};

#endif//__UpdateScpFileList_H__
