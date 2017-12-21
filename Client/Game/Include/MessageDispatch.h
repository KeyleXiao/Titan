/*******************************************************************
** 文件名:	MessageDispatch.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	6/21/2014
** 版  本:	1.0
** 描  述:	

			消息分发器
********************************************************************/

#pragma once


#include "IMessageDispatch.h"
#include <list>
#include <map>

#ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
#include "SpellMessageDef.h"
#endif

/**
	网络消息处理者
	用法: 由各模块实现该接口，然后调用IMessageDispatch的注册接口,由此接管属于该模块的包
	举例: 例如任务系统，设定moduleId=4,则任务系统创建一个类，继承IMessageHandler，并调用IMessageDispatch注册该消息
	      则以后碰到消息头中标明模块为4的消息，都发给该类处理
*/ 
class MessageDispatch : public IMessageDispatch
{
public:
	///////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void dispatchMessage(void* buf, size_t len)
	{
		if ( buf==NULL || len<sizeof(SNetMsgHead) )
		{
			ErrorLn("dispatchMessage failed! message length is too short! [len]" << len );
			return;
		}

		// 已阻塞
		if ( m_bChocke==true )
		{
			void * data = malloc(len+sizeof(int));
			memcpy( (int *)data+1,buf,len);
			*(int *)data = len;

			m_cacheList.push_back(data);
			return;
		}

		SNetMsgHead * head = (SNetMsgHead *)buf;
		void * data = (void*)(head+1);
		len -= sizeof(SNetMsgHead);

#ifdef ENTITY_MESSAGE_ROUTE_TIME_HACK
        if(head->byKeyModule == MSG_MODULEID_ENTITY && head->byKeyAction==SPELL_MSG_PREPARE)
        {
            SMsgEntityHead * pEntityHead = (SMsgEntityHead*)(head+1);
            if(pEntityHead->byPartID == PART_SPELL)
            {
                SSpellContextImp_SC* pSpellContext = (SSpellContextImp_SC*)((char*)data+sizeof(SMsgEntityHead));
                pSpellContext->dwCRecvTick = getTickCountEx();
            }
        }
#endif

		HANDLER_MAP::iterator mit = m_DispatchMap.find( head->byKeyModule );
		if ( mit==m_DispatchMap.end() )
		{
			ErrorLn("dispatchMessage failed! message Module id is not registered! [ModuleID]=" << head->byKeyModule );
			return;
		}

		HANDLER_LIST & msg_list = mit->second;
		
		HANDLER_LIST::iterator it = msg_list.begin();
		for ( ;it!=msg_list.end(); )
		{
			if ( *it==0 )
			{
				it = msg_list.erase(it);
			}
			else
			{
                DWORD startTick = getTickCount();

				(*it)->onMessage( head,data,len );
				++it;

                DWORD dwRunTick = getTickCount() - startTick;
                // 处理时间太长
                if(dwRunTick> 30)
                {
                    TraceLn("MSG: "<<head->bySrcEndPoint<<"->"<<head->byDestEndPoint<<", Module="<<head->byKeyModule<<", ActionId="<<head->byKeyAction<<", cost "<<dwRunTick <<"ms");
                }
			}
		}
	}

	virtual void registerMessageHandler(ulong moduleId, IMessageHandler* handler)
	{
		// 去重
		unregisterMessageHandler( moduleId,handler );

		// 添加
		// 只能用push_front 添加到前面 若是添加到后面 会导致消息给后面刚添加的再处理一次 状态切换时会发生
		m_DispatchMap[moduleId].push_front(handler);
	}

	virtual void unregisterMessageHandler(ulong moduleId, IMessageHandler* handler)
	{
		HANDLER_MAP::iterator mit = m_DispatchMap.find( moduleId );
		if ( mit==m_DispatchMap.end() )
		{
			return;
		}

		HANDLER_LIST & msg_list = mit->second;

		HANDLER_LIST::iterator it = msg_list.begin();
		for ( ;it!=msg_list.end();++it )
		{
			if (*it==handler )
			{
				*it = 0;
			}
		}
	}

	/**
	阻塞派发： 暂时不要调度网络包
	比如Unity3d加载地图时无法做到真正的同步加载，此时创建了人物，加载完成后又会卸载掉
	*/
	virtual void choke( bool bEnable )
	{
		m_bChocke = bEnable;

		if ( m_bChocke==false )
		{
			// 取消阻塞就要把之前的都调度一下
			CACHE_LIST::iterator it = m_cacheList.begin();
			for ( ;it!=m_cacheList.end();++it )
			{
				void * data = *it;
				int len = *(int *)data;

				dispatchMessage( (int *)data+1,len );
				free(data);
			}

			m_cacheList.clear();
		}
	}



	MessageDispatch() : m_bChocke(false){}

	~MessageDispatch()
	{
		if (!m_cacheList.empty())
		{
			CACHE_LIST::iterator it = m_cacheList.begin();
			for ( ;it!=m_cacheList.end();++it )
			{
				void * data = *it;
				free(data);
			}
			m_cacheList.clear();
		}
	}

private:
	typedef std::list<IMessageHandler*>		HANDLER_LIST;
	typedef std::map<int,HANDLER_LIST>		HANDLER_MAP;

	HANDLER_MAP                             m_DispatchMap;

	typedef std::list<void*>			    CACHE_LIST;
	CACHE_LIST                              m_cacheList;
	bool                                    m_bChocke;
};