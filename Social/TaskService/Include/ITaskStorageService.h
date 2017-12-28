/*******************************************************************
** 文件名:	ITaskService.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	任务数据服务接口

********************************************************************/


#ifndef __TASKSERVICE_INCLUDE_ITASKSERVICE_H__
#define __TASKSERVICE_INCLUDE_ITASKSERVICE_H__

// 接口
struct ITaskStorageService
{
	// 创建
	virtual bool						create() = 0;

	// 释放
	virtual void						release() = 0;

    ///////////////////////ITaskStorage///////////////////////////////////////////////////
    /* 注册键值
    @param nType: DATA_SET_TYPE
    @param nKey : 键值
    @param bSave: 数值保存到数据库中（非临时）
    */
    virtual void                        registerKey(BYTE nType, WORD wKey, bool bSave) = 0;

    /* 清除某个拥有者的数据
    @param nType: DATA_SET_TYPE
    @param dwHolder : 拥有者ID
    */
    virtual void                        clearHolder(BYTE nType, DWORD dwHolder) = 0;

    /* 增加数据点
    @param nType: DATA_SET_TYPE
    @param dwHolder : 拥有者ID
    @param nKey : 键值
    @param nData: 数值
    */
    virtual void                        setData(BYTE nType, DWORD dwHolder, WORD wKey, int nData) = 0;

    /* 移除数据点
    @param nType: DATA_SET_TYPE
    @param dwHolder : 拥有者ID
    @param nKey : 键值
    */
    virtual void                        removeData(BYTE nType, DWORD dwHolder, WORD wKey) = 0;

    /* 取得值
    @param nType: DATA_SET_TYPE
    @param dwHolder : 拥有者ID
    @param nKey : 键值
    */
    virtual int	                        getValue(BYTE nType, DWORD dwHolder, WORD wKey) = 0;

	///////////////////////IContactTask///////////////////////////////////////////////////
	// 处理服务器消息
	virtual void						handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len) = 0;

	// 处理客户端消息
	virtual void						handleClientMsg(DWORD clientID, SNetMsgHead head, void * data, size_t len) = 0;
};


#endif  // __TASKSERVICE_INCLUDE_ITASKSERVICE_H__