/*******************************************************************
** 文件名:	GetDiskSN.cpp 
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

#include "stdafx.h"
#include "GetDiskSN.h"

#ifdef SUPPORT_SAFE_SSPRINTF_S
#pragma push_macro("sprintf_s") 
#undef sprintf_s
#endif

// 取得硬盘序号
BOOL GetCDriveSerialNumber(LPDWORD pdwSerialNumber)
{
	//Again as with the NIC address, the serial number of the C drive
	//could be used as the basis of a security scheme for a networked 
	//product of yours
	BOOL bSuccess = (GetVolumeInformation(_T("C:\\"), NULL, 0, pdwSerialNumber,
					NULL, NULL, NULL, 0) != 0);
	return bSuccess;
}

// 取得硬盘序号字串
const char * GetCDriveSerialNumberString(void)
{
	static char szSerialNumber[32]={0};
	DWORD dwSerialNumber = 0;
	//Again as with the NIC address, the serial number of the C drive
	//could be used as the basis of a security scheme for a networked 
	//product of yours
	BOOL bSuccess = (GetVolumeInformation(_T("C:\\"), NULL, 0, &dwSerialNumber,
		NULL, NULL, NULL, 0) != 0);

	sprintf_s(szSerialNumber, sizeof(szSerialNumber),"%X",dwSerialNumber);

	return szSerialNumber;
}
#ifdef SUPPORT_SAFE_SSPRINTF_S
#pragma pop_macro("sprintf_s") 
#endif
