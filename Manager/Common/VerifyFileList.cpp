/*******************************************************************
** 文件名:	VerifyFileList.cpp 
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

#include "stdafx.h"
#include "VerifyFileList.h"



// 构造函数
VerifyFileList::VerifyFileList(void):m_dwMaxID(0)
{

}

// 析构函数
VerifyFileList::~VerifyFileList(void)
{
	Clear();
}

bool VerifyFileList::AddUser(CVerifyFile * pFile)
{
	if (pFile==NULL)
	{
		return false;
	}
	m_dwMaxID++;

	DWORD id = m_dwMaxID;
	pFile->SetID(id);

	m_mapList[id] = pFile;

	return true;
}

bool VerifyFileList::DelUser(CVerifyFile * pFile)
{
	if (pFile==NULL)
	{
		return false;
	}
    m_mapList.erase(pFile->GetID());

	return true;
}

void VerifyFileList::Clear(void)
{
	list<CVerifyFile *> deletelist;

	CVerifyFile * pUser = NULL;
	for(TMAP_FileList::iterator it = m_mapList.begin(); it != m_mapList.end(); it++)
	{
		pUser = it->second;
		deletelist.push_back(pUser);
	}

	for (list<CVerifyFile *>::iterator it0 = deletelist.begin(); it0 != deletelist.end(); it0++)
	{
		SafeRelease((*it0));
	}

	m_mapList.clear();

}

CVerifyFile * VerifyFileList::GetUser(DWORD dwID)
{
	TMAP_FileList::iterator it = m_mapList.find(dwID);
	if ( it==m_mapList.end())
		return NULL;
	else 
		return it->second;
}