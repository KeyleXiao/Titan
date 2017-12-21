/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MentorshipServer\Include\IMentorshipPerson.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	易耀辉
** 日  期:	11/28/2016
** 版  本:	1.0
** 描  述:	
********************************************************************/

#pragma once

struct IMentorshipPerson
{

    /// purpose: 取得战队的整型数据
    virtual int getNumProp(DWORD dwPropID) = 0;

};