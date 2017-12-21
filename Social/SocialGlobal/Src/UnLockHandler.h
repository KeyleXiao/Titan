/*******************************************************************
** 文件名: UnLockHandler.h 
** 版  权: (C) 深圳冰川网络技术有限公司
** 创建人: 李界华
** 日  期: 4/27/2017
** 版  本: 1.0
** 描  述: 默认数据库处理
** 应  用:    
    
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "IDBEngine.h"

class CUnLockHandler : public IDBRetSink
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
	virtual void        OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);

    ///////////////////////////////////////////////////////////////////
    /** 
    @param   
    @param   
    @return  
    */
    CUnLockHandler(void);

    /** 
    @param   
    @param   
    @return  
    */
    virtual ~CUnLockHandler(void);

    /** 创建
    @return  
    */
    bool create(void);

    /** 取消注册数据库处理
    @return  
    */
    bool onStop(void);

    /** 请求执行解锁
    @return  
    */
    bool requstExecuteUnlock(void);

    /** 执行解锁
    @return  
    */
    bool handleExecute(char* pData, size_t nDataSize);
private:
    /** 执行强制解锁
    @param   dwOperateWorldID：数据记录对应游戏世界ID
    @param   dwSinkWorldID   ：要操作的目标游戏世界ID
    @param   pszInData       ：输入数据
    @param   nInDataLen      ：输入数据长度
    @return         
    @retval buffer 
    */
    bool executeUnLock(DWORD dwOperateWorldID, DWORD dwSinkWorldID, int nActorIDCount, LPCSTR pszInData, int nInDataLen);


};
