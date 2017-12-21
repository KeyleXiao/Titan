/*******************************************************************
** 文件名:	UploadFileList.cpp 
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

#include "stdafx.h"
#include "UploadFileList.h"



// 构造函数
UploadFileList::UploadFileList(void):
	m_dwMaxID(0)
	,m_dwAllDataLens(0)
{

}

// 析构函数
UploadFileList::~UploadFileList(void)
{
	Clear();
}

bool UploadFileList::AddUser(CUploadFile * pFile)
{
	if (pFile==NULL)
	{
		return false;
	}
	m_dwMaxID++;

	DWORD id = m_dwMaxID;
	pFile->SetID(id);

	m_mapList[id] = pFile;

	//WarningLn("上传文件管理列表["<<m_mapList.size()<<"]Add");

	return true;
}

bool UploadFileList::DelUser(CUploadFile * pFile)
{
	if (pFile==NULL)
	{
		return false;
	}
    m_mapList.erase(pFile->GetID());

	//WarningLn("上传文件管理列表["<<m_mapList.size()<<"]Del");

	return true;
}

void UploadFileList::Clear(void)
{
	list<CUploadFile *> deletelist;

	CUploadFile * pUser = NULL;
	for(TMAP_FileList::iterator it = m_mapList.begin(); it != m_mapList.end(); it++)
	{
		pUser = it->second;
		deletelist.push_back(pUser);
	}

	for (list<CUploadFile *>::iterator it0 = deletelist.begin(); it0 != deletelist.end(); it0++)
	{
		SafeRelease((*it0));
	}

	m_mapList.clear();

}

CUploadFile * UploadFileList::GetUser(DWORD dwID)
{
	TMAP_FileList::iterator it = m_mapList.find(dwID);
	if ( it==m_mapList.end())
		return NULL;
	else 
		return it->second;
}