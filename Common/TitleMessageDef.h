/*******************************************************************
** 文件名:	TitleMessageDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	3/23/2015
** 版  本:	1.0
** 描  述:	Title消息定义
********************************************************************/

#ifndef __TITLE_MESSAGE_DEF_H__
#define __TITLE_MESSAGE_DEF_H__

#include "NetMessageDef.h"
#include "TitleDef.h"

#pragma pack(1)


/////////////////////////// Title模块消息  ////////////////////////////
// 消息码定义键值
enum SC_TITLE_NET_MESSAGE
{
    SC_NET_MSG_TITLE = SC_MSG_TITLE_START,		// 战场消息

    SC_MSG_TITLE_ADD,		            // 添加title
    SC_MSG_TITLE_DELETE,				// 删除title
};

struct msg_title_add
{
    // 数据流的方式进行传输
    int     ID;	                    // TitleID
    STitleIndexData IndexData;      // 添加相关

    

    msg_title_add(void)
    {
        ID = TITLE_INDEX_INVALID;
    }
};

struct msg_title_delete
{
    // 数据流的方式进行传输
    int     ID;	        // TitleID

    msg_title_delete(void)
    {
        ID = TITLE_INDEX_INVALID; 
    }
};


#pragma pack()
#endif	// __BUFF_MESSAGE_DEF_H__