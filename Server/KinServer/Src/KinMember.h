/*******************************************************************
** 文件名:	e:\Rocket\Server\KinServer\Src\KinMember.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2016/3/9
** 版  本:	1.0
** 描  述:	战队成员对象类
** 应  用:	
**************************************1******************************/

#ifndef __KinMember_h__
#define __KinMember_h__

#include "IKinScene.h"

// 结算的共享经验
#define FULL_SHATE_EXP	1000

// 战队对象实现类
class CKinMember
{
public:
	/// purpose: 构造函数
	CKinMember(void);

	/// purpose: 析构函数
	virtual ~CKinMember(void);

public:
	/// purpose: 创建
	bool create(SSceneKinMember* pSceneKinMember);

	/// purpose: 关闭
	virtual void release(void);

	/// purpose: 更新现成
	void updateData(SSceneKinMember* pSceneKinMember);

	/// purpose: 取得成员数据
	SSceneKinMember* getContext();

private:
	// 成员现场
	SSceneKinMember m_Context;

};

#endif // __Kin_h__