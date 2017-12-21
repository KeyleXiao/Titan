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
#include "KinScene.h"
#include "KinMember.h"
#include "ExternalHelper.h"

/// purpose: 构造函数
CKinMember::CKinMember(void)
{

}

/// purpose: 析构函数
CKinMember::~CKinMember(void)
{

}


/// purpose: 创建
bool CKinMember::create(SSceneKinMember* pSceneKinMember)
{
	if (pSceneKinMember == NULL)
	{
		return false;
	}

	memcpy(&m_Context, pSceneKinMember, sizeof(m_Context));

	return true;
}

/// purpose: 关闭
void CKinMember::release(void)
{

	delete this;
}

/// purpose: 更新现成
void CKinMember::updateData(SSceneKinMember* pSceneKinMember)
{
	if (pSceneKinMember == NULL)
	{
		return;
	}

	memcpy(&m_Context, pSceneKinMember, sizeof(m_Context));
}

SSceneKinMember* CKinMember::getContext()
{
    return &m_Context;
}

