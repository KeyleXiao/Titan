/*******************************************************************
** 文件名:	e:\Rocket\Server\KinServer\Src\Kin.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	xiaobao
** 日  期:	2009/3/9
** 版  本:	1.0
** 描  述:	战队对象
** 应  用:	
********************************************************************/

#include "StdAfx.h"
#include "KinService.h"
#include "KinMember.h"
#include "ExternalHelper.h"

CKinMember::CKinMember()
{

}

/// purpose: 构造函数
CKinMember::CKinMember(SKinMember* pKinMember)
{
	memcpy(&m_Context, pKinMember, sizeof(SKinMember));
}

/// purpose: 析构函数
CKinMember::~CKinMember(void)
{

}


/// purpose: 创建
bool CKinMember::Create()
{
	return true;
}

/// purpose: 关闭
void CKinMember::Close(void)
{
	
}

/// purpose: 取得现场
SKinMember* CKinMember::getContext()
{
	return &m_Context;
}
