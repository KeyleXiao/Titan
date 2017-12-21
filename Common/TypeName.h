/*******************************************************************
** 文件名:	TypeName.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/1/2014
** 版  本:	1.0
** 描  述:	

			数据集合模版
********************************************************************/

#pragma once

template<typename T>
const char * TypeName(T)
{
	return "unknown";
}

inline const char * TypeName(int)
{
    return "int";
}

inline const char * TypeName(unsigned char)
{
    return "byte";
}

inline const char * TypeName(short)
{
    return "short";
}

inline const char * TypeName(bool)
{
    return "bool";
}

inline const char * TypeName(char *)
{
    return "string";
}

inline const char * TypeName(void *)
{
    return "point";
}

inline const char*  TypeName(Vector3)
{
    return "Vector3";
}

inline const char*  TypeName(unsigned long)
{
    return "unsigned long";
}
