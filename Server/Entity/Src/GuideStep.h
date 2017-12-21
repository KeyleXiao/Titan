/*******************************************************************
** 文件名:	GuideStep.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	13/4/2017
** 版  本:	1.0
** 描  述:	引导步骤数据库回调接口

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IDBEngineService.h"


class CommonPart;

class CGuideStep : public IDBRetSink
{
public:
	///////////////////////////////// IDBRetSink ////////////////////////////
	/** 数据库请求返回回调方法
	@param   pDBRetSink		：结果返回接口	
	@param   nCmdID ：命令ＩＤ
	@param   nDBRetCode：数据库请求返回值，参考上面定义
	@param   pszDBRetDesc：数据库请求返回描述，由ＳＰ返回
	@param   nQueueIndex：队列定义
	@param   pOutData：输出数据
	@param   nOutLen：输出数据长度
	@return  
	@note     
	@warning 此对像千万不能在数据库返回前释放，否则会非法！
	@retval buffer 
	*/	
	virtual void OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	///////////////////////////////////////////////////////////////////
	/** 
	@param   
	@param   
	@return  
	*/
	CGuideStep(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CGuideStep(void);

	/** 
	@param   
	@param   
	@return  
	*/
	bool create(CommonPart * pCommonPart);

	/** 
	@param   
	@param   
	@return  
	*/
	void release(void);

	/** 
	@param   
	@param   
	@return  
	*/
	bool save(int nStep);

private:
	CommonPart *	m_pCommonPart;
};
