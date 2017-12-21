/*******************************************************************
** 文件名:	DBRetLogSink.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华 (Carl Chen)
** 日  期:	2016-7-4
** 版  本:	1.0
** 描  述:  记录日志数据库返回
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/


#ifndef __OSS_SRC_DBRETLOGSINK_H__
#define __OSS_SRC_DBRETLOGSINK_H__


#include "IDBEngine.h"
#include <strstream>


//log默认数据库请求返回回调
class CDBRetLogSink : public IDBRetSink
{
public:
	//////////////////////////////////IDBRetSink///////////////////////
	/** 数据库请求返回回调方法
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
	virtual void		OnReturn(int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

	///////////////////////////////////////////////////////////////////
	/** 
	@param   
	@param   
	@return  
	*/
	CDBRetLogSink(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CDBRetLogSink(void);
};

//extern  CDBRetLogSink    g_DBRetLogSink;


#endif// __OSS_SRC_DBRETLOGSINK_H__