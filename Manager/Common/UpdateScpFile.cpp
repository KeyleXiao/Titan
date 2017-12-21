/*******************************************************************
** 文件名:	UpdateScpFile.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2010-6-8
** 版  本:	1.0
** 描  述:	刷新脚本对象类
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#include "stdafx.h"
#include "UpdateScpFile.h"
#include "UpdateScpFileList.h"
#include <strstream>
#include "sha1.h"

///////////////////////////////////////////////////////////////////////////////////////////////
CUpdateScpFile::CUpdateScpFile():
	m_dwID(0)
	,m_strFileName("")
	,m_strTitle("")
	,m_bAutoRelease(false)
	,m_dwState(0)
	,m_nServerType(0)
	,m_dwServerData1(0)
	,m_dwServerData2(0)
	,m_dwServerData3(0)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////
CUpdateScpFile::~CUpdateScpFile()
{

}
// 创建
bool CUpdateScpFile::Create(const char * szRemoteFileName)
{
	if (szRemoteFileName==NULL)
	{
		return false;
	}
	m_strFileName = szRemoteFileName;
	return true;
}
void CUpdateScpFile::Release()
{

	UpdateScpFileList::getInstance().DelUser(this);

	delete this;
}

DWORD CUpdateScpFile::GetID()
{
	return m_dwID;
}

void CUpdateScpFile::SetID(DWORD dwID)
{
	m_dwID = dwID;
}

std::string CUpdateScpFile::ToString()
{
	ostrstream buffer;	
	buffer << _GT("刷新脚本[ID=") << m_dwID << _GT(",文件名=")<<m_strFileName<<_GT(",状态=")<<m_dwState<<":"<<GetFileUpdateScpStateString(m_dwState)<<"]["<<m_strTitle<<"]" << std::ends;
	return buffer.str();
}


// 设置显示信息
void CUpdateScpFile::SetTitle(const char * szTitle)
{
	if (szTitle==NULL)
	{
		m_strTitle = "";
	}
	m_strTitle = szTitle;
}
