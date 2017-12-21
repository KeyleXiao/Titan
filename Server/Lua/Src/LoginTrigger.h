/*******************************************************************
** 文件名:	LoginTrigger.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	11/28/2016
** 版  本:	1.0
** 描  述:	登入触发器
********************************************************************/

#ifndef __LOGIN_TRIGGER_H__
#define __LOGIN_TRIGGER_H__

#include "ITrigger.h"
#include <string>

/**
@name : 登入触发器
@brief: 
*/
class LoginTrigger : public ITrigger, public IEventExecuteSink
{
public:
	virtual bool Setup();

	virtual void Release();

	virtual void SetID(int nID);

	virtual int GetID() const;

	virtual int GetTriggerType(void);

	virtual std::string GetTriggerCallBack(void);

	virtual int GetSceneID();

    //////////////////////////////////////////////////////////////////////////
    /** 
    @param   wEventID ：事件ID
    @param   bSrcType ：发送源类型
    @param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
    @param   pszContext : 上下文
    @param   nLen : 上下文长度
    @return  
    @note     
    @warning 
    @retval buffer 
    */
    virtual void		OnExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

    //////////////////////////////////////////////////////////////////////////
    LoginTrigger(int nSceneID, const char *szCallFunc);

    ~LoginTrigger();

    // 设置LUA登陆事件
    void SetLuaLoginEvent(DWORD dwPDBID);

private:
    int               m_nSceneID;
    std::string       m_szCallFunc;
    int               m_nTriggerID;

    std::map<DWORD,int> m_LoginMethodMap;    // 记录上线方式,是切换地图还是跨场景
    std::map<DWORD,bool> m_ReadTaskDataMap;  // 读取任务数据表
};

#endif//__LOGIN_TRIGGER_H__