/*******************************************************************
** 文件名:	ClientContainer.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/06
** 版  本:	1.0
** 描  述:	观战客户端容器
** 应  用:  负责管理所有用客户端观战的连接
			（如果以后有其他脱离客户端观战的需求，可以再开一个Container处理）
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "UserContainer.h"
#include "ClientSession.h"


class ClientContainer : public UserContainer<ClientSession, PlayerID>, public Singleton<ClientContainer>
{
public:
	ClientContainer();
	virtual ~ClientContainer();

	inline void BroadCastData(obuf& obufData, const VecPlayerID& vec)
	{
		BroadCastData((const obuf&)obufData, vec);
	}
	void BroadCastData(const obuf& obufData, const VecPlayerID& vec);

	template<typename TMsg>
	void BroadCastData(TMsg& msg, const VecPlayerID& vec)
	{
		obuf obufData;
		TBuildObufMsg(obufData, msg);

		for (size_t i = 0; i < vec.size(); i++)
		{
			const PlayerID& dwPlayerID = vec.at(i);
			auto pClient= this->Get(dwPlayerID);
			if (pClient==nullptr)
				continue;
			 
			pClient->SendData(obufData.data(), obufData.size());
		}
	}

protected:
	virtual void OnDelUser(ISessionUser<PlayerID>* pUser);

	//注册这里要处理的所有消息，到m_Processer中
	virtual void RegisterHandlers();

	virtual PlayerID GetMaxID();

};
extern ClientContainer& gClientContainer;