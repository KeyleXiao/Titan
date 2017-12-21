/*******************************************************************
** 文件名:	ImageDataManager.cpp 
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	李界华
** 日  期:	2015-08-19
** 版  本:	1.0
** 描  述:	镜像数据管理器
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "stdafx.h"
#include "ImageDataManager.h"



/** 取得镜像数据银行
@param   
@return  
*/
CImageDataBank * CImageDataManager::GetImageDataBank(BYTE endPoint)
{
	if (endPoint >= MSG_SERVERID_MAXID)
	{
		return NULL;
	}

	return &(m_ImageBank[endPoint]);
}