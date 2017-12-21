/*******************************************************************
** 文件名:	MiscManager.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016-12-9
** 版  本:	1.0
** 描  述:	辅助全局对象的一个杂项管理器

********************************************************************/


#ifndef __MISC_MANAGER_H__
#define __MISC_MANAGER_H__

#include "IDBRetSink.h"
#include "IMiscManager.h"


// 辅助全局对象的一个杂项管理器
class CMiscManager : public IMiscManager, public IDBRetSink
{
public:
    ////////////////// IMiscManager ////////////////////////////////////////////////////////
	/** 加载
	@return  
	*/
    virtual bool	        load();

	/** 执行Service的on_stop
	@return  
	*/
    virtual void            onStop(void);

	/** 释放
	@note     释放所有资源，并且销毁此对像
	@warning 
	*/
	virtual void			release(void);

	/* 用Service配置名称创建Service
    @param pszServiceSchemeName: Service配置名称
    */
	virtual SERVICE_PTR     create_service(const char * pszServiceSchemeName);

    /* 注册到DBEngineService，这个有点尴尬*/
    virtual bool            registerDBEngineService();

    /* 校验游戏世界ID有效性*/
    virtual bool            checkWorldID(void);

    ////////////////// IDBRetSink ////////////////////////////////////////////////////////
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
	virtual void			OnReturn(IDBRetSink * pRealDBRetSink, int nCmdID, int nDBRetCode, char * pszDBRetDesc, int nQueueIndex, char * pOutData, int nOutLen);


	////////////////// CMiscManager ////////////////////////////////////////////////////////

    // 构造函数
	CMiscManager();

    // 析构函数
	virtual ~CMiscManager();

private:

};



#endif //__MISC_MANAGER_H__ 

