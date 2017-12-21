#pragma once

#include "IEntityClient.h"
#include "IMessageDispatch.h"
#include "MigrateDef.h"
#include "Singleton.h"

using namespace rkt;

class Migrate : public IMessageHandler, public IGameViewEventHandler, public Singleton<Migrate>
{
public :
    void create();

    void release();

    ///////////////////////////IMessageHandler//////////////////////////////////////////////
    /** 消息处理
	@param head : 消息头，通常为一个消息包的前4个字节
    @param data : 消息的实际内容，除去了消息头的
	@param len  : 内容长度
	*/
	virtual void onMessage( SNetMsgHead* head,void* data,size_t len );

    ///////////////////////////IGameViewEventHandler//////////////////////////////////////////////
	// 显示层事件处理器
	virtual bool onViewEvent(int32 eventID, int nParam, const char * strParam, void * ptrParam);

	// 切换场景
	void onViewSwitchScene(int nParam, const char * strParam, void * ptrParam);
};
