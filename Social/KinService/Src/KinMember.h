/*******************************************************************
** 文件名:	e:\Rocket\Server\KinService\Src\KinMember.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2009/3/9
** 版  本:	1.0
** 描  述:	战队成员对象类
** 应  用:	
**************************************1******************************/

#ifndef __KinMember_h__
#define __KinMember_h__

#include "IKinService.h"

// 战队对象实现类
class CKinMember
{
public:
	/// purpose: 构造函数
	CKinMember::CKinMember();
	/// purpose: 构造函数
	CKinMember(SKinMember* pSceneKinMember);

	/// purpose: 析构函数
	virtual ~CKinMember(void);

public:
	/// purpose: 创建
	bool Create();

	/// purpose: 关闭
	virtual void Close(void);

	/// purpose: 取得现场
	SKinMember* getContext();

private:
	// 成员现场
	SKinMember m_Context;
};

#endif // __Kin_h__