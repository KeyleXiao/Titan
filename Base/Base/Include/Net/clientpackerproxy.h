/*******************************************************************
** 文件名:	ClientPackerProxy.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	10/16/2011
** 版  本:	1.0
** 描  述:	客户端封包代理类
** 应  用:  

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

#pragma once

#include "ILuaEngine.h"
#include "MixEncryptPacker.h"

namespace rkt{

/**
功能:  根据客户端网络封包的特点（1.性能要求不高 2.要加强反调试能力）
       额外实现的一个代理类，该类主要是出于反跟踪的目的，去掉也是可以正常运行的

	   1.专门起一个线程负责加解密，防止直接断点WSASend API就能分析到数据加解密过程
	   2.把加解密的一部分算法放到lua脚本中，增加反调试能力
	     注意: Lua虚拟机不是多线程安全的
*/

class ClientPackerProxy : public MixEncryptPacker
{
public:
	bool RunLuaFunc( lua_State * _L,const char * func,int currenkey,int param,int & ret );

	bool Init(int nSendKey,int nRecvKey);

	// 每收到一个数据包都生成一次密钥
	virtual void GenerateEncryptKey( bool bIsSend,WORD seed );

	ClientPackerProxy() : m_pSendLuaState(0),m_pRecvLuaState(0),m_pSendFunc(0),m_pRecvFunc(0){}

	virtual ~ClientPackerProxy();

private:
	ILuaEngine   *   m_pSendLuaState;
	ILuaEngine   *   m_pRecvLuaState;

	lua_CFunction    m_pSendFunc;
	lua_CFunction    m_pRecvFunc;
};

};