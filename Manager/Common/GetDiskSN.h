/*******************************************************************
** 文件名:	GetDiskSN.h 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	宋文武 (sww8@163.com)
** 日  期:	2009/8/25
** 版  本:	1.0
** 描  述:	取得硬盘序号
** 应  用:  	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#ifndef __GetDiskSN_H__
#define __GetDiskSN_H__

// 取得硬盘序号
BOOL GetCDriveSerialNumber(LPDWORD pdwSerialNumber);
// 取得硬盘序号字串
const char * GetCDriveSerialNumberString(void);

#endif//__GetDiskSN_H__