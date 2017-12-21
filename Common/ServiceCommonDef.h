/*******************************************************************
** 文件名:	ServiceCommonDef.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	2/23/2016
** 版  本:	1.0
** 描  述:	服务返回公共定义
            如EntityHelper.h中不支持返回多个数值，如返回复杂类型的时候
            在此定义
********************************************************************/

#ifndef __SERVICE_COMMONDEF_H__
#define __SERVICE_COMMONDEF_H__

#define BUFF_TYPE_MAX_COUNT 32
// Buff类型列表
struct BuffTypeList
{
    int     nCount;
    DWORD   dwBuffID[BUFF_TYPE_MAX_COUNT];
    DWORD   dwBuffLevel[BUFF_TYPE_MAX_COUNT];
    int     nBuffTime[BUFF_TYPE_MAX_COUNT];

    BuffTypeList()
    {
        memset(this, 0, sizeof(*this));
    }
};

#pragma pack()
#endif	// __SERVICE_COMMONDEF_H__