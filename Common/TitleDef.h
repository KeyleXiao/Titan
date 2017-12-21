/*******************************************************************
** 文件名:	TitleDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-6-27
** 版  本:	1.0
** 描  述:	称号定义

********************************************************************/


#pragma once


/// 头顶称号定义
enum TITLE_INDEX_TYPE
{
    TITLE_INDEX_INVALID = -1,
    TITLE_INDEX_NAME,		    // 名字
    TITLE_INDEX_KIN,		    // 战队标题
    TITLE_INDEX_CLAN,		    // 帮会标题


    TITLE_INDEX_MAX,			 // 最大称号数
};

// 称号名字长度
#define TITLE_NAME_MAXSIZE						32

// 各行称号数据
struct STitleIndexData
{
    int 	nTitleID;					        // 称号ID,0为此行无称号
    char	szTitleName[TITLE_NAME_MAXSIZE];	// 称号名字

    STitleIndexData()
    {
        memset(this, 0, sizeof(*this));
    };

    void clear(void)
    {
        memset(this, 0, sizeof(*this));
    };
};