/*******************************************************************
** 文件名:	IUrlResponse.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/14/2017
** 版  本:	1.0
** 描  述:	HTTP响应接口

********************************************************************/

#pragma once

#ifndef __I_URL_RESPONSE_H__
#define __I_URL_RESPONSE_H__


// HTTP响应接口
struct IUrlResponse
{
    /**
    @name    : 请求url调用（外部接口调用）
    @param
    @param
    */
    virtual void onUrlResponse(const char* pContext, unsigned short nLen) = 0;
};


#endif // __I_URL_RESPONSE_H__
