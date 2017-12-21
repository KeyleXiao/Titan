/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MatchServer\Include\IKin.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/15/2015	17:50
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once

#include "KinDef.h"

struct IKin
{
    virtual void release() = 0;

    /// purpose: 取得战队的整型数据
    virtual int getNumProp(DWORD dwPropID) = 0;

    /// purpose: 取得战队名字
    virtual LPCSTR getName() = 0;

    /// purpose: 删除成员
    //@ param  : nPDBID 玩家PDBID
    //@ param  : szName 玩家名字
    virtual void delMember(DWORD dwPDBID) = 0;

    /// purpose: 取得战队成员个数
    virtual int getMemberCount() = 0;

    /** 获取战队总战力
	@param dwKinID		:战队ID 
	*/
    virtual DWORD getTotalFightScore() = 0;

    /** 获取战队总战力
	@param dwKinID		:战队ID 
	*/
    virtual SAutoAccept* getAutoAccept() = 0;
};