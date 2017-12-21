﻿/*******************************************************************
** 文件名:	NPCFuncTrigger.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/2/14
** 版  本:	1.0
** 描  述:	NPC功能出发器
********************************************************************/

#ifndef __NPCFUNC_TRIGGER_H__
#define __NPCFUNC_TRIGGER_H__

#include "ITrigger.h"
#include <string>


class NPCFuncTrigger : public ITrigger, public IEventExecuteSink
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
    NPCFuncTrigger(int nSceneID, const char *szCallFunc);

    ~NPCFuncTrigger();

private:
    int               m_nSceneID;
    std::string       m_szCallFunc;
    int               m_nTriggerID;
};

#endif//__NPCFUNC_TRIGGER_H__