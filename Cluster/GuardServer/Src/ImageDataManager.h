/*******************************************************************
** 文件名:	ImageDataManager.h 
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
#pragma once
#include "Singleton.h"
#include "ImageDataBank.h"


class CImageDataManager : public Singleton<CImageDataManager>
{	
public:
	/** 取得镜像数据银行
	@param   
	@return  
	*/
	CImageDataBank * GetImageDataBank(BYTE endPoint);

private:
	CImageDataBank	m_ImageBank[MSG_SERVERID_MAXID];
};
