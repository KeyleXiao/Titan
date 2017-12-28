/*******************************************************************
** 文件名:	WarService_Stub.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	ljw
** 日  期:
** 版  本:	1.0
** 描  述:	本文件为根据IWarService接口定义，自动生成的WarService接口
			代理文件，该文件是为符合Framework对接口进行的包装


********************************************************************/

#pragma once

/**
	服务容器:
	1.框架根据配置创建容器，容器负责与框架进行底层通讯，收到调用请求再转交给应用层提供的服务存根进行处理
	2.代理/存根，从COM架构借鉴过来的名词，应用层调用某个服务的接口，实际交由代理封包成消息，再提交给框架进行转发
				而框架里的容器收到请求消息，则需要调用存根进行解释和真正处理
*/

#include "IServiceContainer.h"
#include "IWarService.h"
#include "buffer.h"

// 这个是服务实现类，你需要自己实现
#include "WarService.h"

using namespace rkt;
using namespace std;

/**
   代理对象:
   该代理实现IWarService接口，把接口函数调用封装为框架系统的消息转发
   对于外界使用接口的其他类来说，这个过程是无缝对接的
*/
class WarService_Proxy : public IWarService
{
public:
	SERVICE_PTR    m_pContainer;


	WarService_Proxy() {
	}

	virtual ~WarService_Proxy() {
	}

	////////////////////////////////////////////////////////////////////////////
	// 以下自动生成的消息打包函数，你需要检查
	////////////////////////////////////////////////////////////////////////////
	// 开启战场逻辑
	virtual void Start()
	{
		BUILD_MSG_CONTEXT_VOID(IWarService::Start);


		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}

	//////////////////////////////////////////////////////////////////////////////////
	virtual bool  PrepareEnterWar(SActorPreaperEnterWar sActorPreaperEnterWar)
	{
		obuf128 t_data;
		t_data << sActorPreaperEnterWar;

		BUILD_MSG_BUFFER(IWarService::PrepareEnterWar, t_data);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
		RETURN_FROM_MSG_BUF(bool);

		return false;     // 请检查默认返回值是否正确
	};

	//////////////////////////////////////////////////////////////////////////////////
	virtual void  setLegendCupInfo(SMsgLegendCupRoomInfo sLegendCupInfo)
	{
		obuf128 t_data;
		t_data << sLegendCupInfo;

		BUILD_MSG_BUFFER(IWarService::setLegendCupInfo, t_data);

		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	};

	// 玩家进入战场
	virtual void onActorEnterWar(SActorEnterWarBaseInfo sActorEnterWarBaseInfo)
	{
		obuf128 t_data;
		t_data << sActorEnterWarBaseInfo;
		BUILD_MSG_BUFFER(IWarService::onActorEnterWar, t_data);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
	}

	// 保留原来接口!!
	virtual void onEntityDie(sEntityDieInfo entityDieInfo)
	{
		obuf128 t_data;
		//创建战场需要的参数
		t_data << entityDieInfo;

		BUILD_MSG_BUFFER(IWarService::onEntityDie, t_data);

		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}

	virtual void onEntityDieEx(sEntityDieInfo entityDieInfo, SEntityReliveInfo& info)
	{
		obuf256 t_data;

		t_data << entityDieInfo;
		BUILD_MSG_BUFFER(IWarService::onEntityDieEx, t_data);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
		if (resultBuf.good() && resultBuf.size() > 0)
		{
			ibuffer t_in(resultBuf.data(), resultBuf.size());
			t_in >> info;
		}
	}

	virtual void onEntityRelive(UID uid)
	{
		BUILD_MSG_CONTEXT_1(IWarService::onEntityRelive, UID, uid)
		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}

	//////////////////////////////////////////////////////////////////////////////////
	virtual void  onWarClientMsg(UID uidActor, byte byKeyAction, const char* pData, size_t nLen)
	{
		obuf256 t_data;
		t_data << uidActor << byKeyAction << nLen;
		t_data.push_back(pData, nLen);
		BUILD_MSG_BUFFER(IWarService::onWarClientMsg, t_data)

		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	};


	virtual int getAllPerson(UID *PersonArray, int nArraySize)
	{
		BUILD_MSG_CONTEXT_2(IWarService::getAllPerson, UID *, PersonArray, int, nArraySize);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
		RETURN_FROM_MSG_BUF(int);

		return 0;
	}

	virtual int getWarSceneID()
	{
		BUILD_MSG_CONTEXT_VOID(IWarService::getWarSceneID);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
		RETURN_FROM_MSG_BUF(int);

		return 0;
	}

	virtual LONGLONG getWarDBKey()
	{
		BUILD_MSG_CONTEXT_VOID(IWarService::getWarDBKey);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
		RETURN_FROM_MSG_BUF(LONGLONG);
		return 0;
	}


	virtual int getWarTypeId()
	{
		BUILD_MSG_CONTEXT_VOID(IWarService::getWarTypeId);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
		RETURN_FROM_MSG_BUF(int);

		return 0;
	}

	virtual DWORD getMatchTypeID()
	{
		BUILD_MSG_CONTEXT_VOID(IWarService::getMatchTypeID);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
		RETURN_FROM_MSG_BUF(DWORD);

		return 0;
	}

	virtual void setChargedValue(UID uidTarget, int nEffectID, int nValue)
	{
		BUILD_MSG_CONTEXT_3(IWarService::setChargedValue, UID, uidTarget, int, nEffectID, int, nValue);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
	}

	virtual int getChargedValue(UID uidTarget, int nEffectID)
	{
		BUILD_MSG_CONTEXT_2(IWarService::getChargedValue, UID, uidTarget, int, nEffectID);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
		RETURN_FROM_MSG_BUF(int);

		return 0;
	}

	virtual void setWarEndReason(BYTE byEndReason)
	{
		BUILD_MSG_CONTEXT_1(IWarService::setWarEndReason, BYTE, byEndReason);
		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}

	virtual void destroy()
	{
		BUILD_MSG_CONTEXT_VOID(IWarService::destroy);

		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}

	// 战场结束。(七龙珠功能)
	virtual void DragonBallEndWar(int nFailedCamp, bool bNormalEnd)
	{
		BUILD_MSG_CONTEXT_2(IWarService::DragonBallEndWar, int, nFailedCamp, bool, bNormalEnd);

		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}

	// 场景已创建
	virtual void onSceneCreated(int nWarID)
	{
		BUILD_MSG_CONTEXT_1(IWarService::onSceneCreated, int, nWarID);

		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}

	// 用于通知指定阵营野怪死亡状况
	virtual void msgToSpecCampWildInfo(UID uMonsterUid, int nCamp)
	{
		BUILD_MSG_CONTEXT_2(IWarService::msgToSpecCampWildInfo, UID, uMonsterUid, int, nCamp);

		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}



	/** 获取战场指定类型UID
	 @param byType		:战队ID
	 */
	virtual DWORD warSerchTypeUIDList(BYTE bySelfCamp, BYTE byGetCampType, BYTE bySerchType, PDBID* pReturnArray, DWORD dwArrayMaxSize)
	{
		BUILD_MSG_CONTEXT_5(IWarService::warSerchTypeUIDList, BYTE, bySelfCamp, BYTE, byGetCampType, BYTE, bySerchType, PDBID*, pReturnArray, DWORD, dwArrayMaxSize);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);
		RETURN_FROM_MSG_BUF(DWORD);
		return 0;

		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}

	virtual void warAddSerchTypeBuff(SWarEffectAddBuff sData)
	{
		BUILD_MSG_CONTEXT_1(IWarService::warAddSerchTypeBuff, SWarEffectAddBuff, sData);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf, MSG_FLAG_NO_BLOCK);
	}
	


	/** 更新玩家BUFF信息列表
	@param data
	@param len
	*/
	virtual void updateActorBuffList(char* data, size_t len)
	{
		obuf t_data;
		t_data << len;
		t_data.push_back(data, len);

		BUILD_MSG_BUFFER(IWarService::updateActorBuffList, t_data);
		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}

	/** 序列化战场内玩家的信息
	@param uid		: 玩家UID
	@param out		: 序列化的数据
	@param flag		: 序列化标识
	*/
	virtual void deSerialzed(UID uid, rkt::obuf & out, int flag)
	{
		obuf t_data;
		t_data << uid << flag;
		BUILD_MSG_BUFFER(IWarService::deSerialzed, t_data);

		rkt::obuf resultBuf;
		m_pContainer->post_message(pMsg, nMsgLen, &resultBuf);

		if (resultBuf.good() && resultBuf.size() > 0)
		{
			out.push_back(resultBuf.data(), resultBuf.size());
		}
	}

	// 设置Ban选英雄
	virtual void setBanHeroList(char* data, size_t len)
	{
		obuf t_data;
		t_data << len;
		t_data.push_back(data, len);

		BUILD_MSG_BUFFER(IWarService::setBanHeroList, t_data);

		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}

	// 救治队友
	virtual void cureMember(UID uid)
	{
		obuf t_data;
		t_data << uid;

		BUILD_MSG_BUFFER(IWarService::cureMember, t_data);

		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}

	// 救治队友
	virtual void changeVocation(UID uid, int nSlotID)
	{
		obuf t_data;
		t_data << uid;
		t_data << nSlotID;

		BUILD_MSG_BUFFER(IWarService::changeVocation, t_data);

		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}

	// 救治队友
	virtual void collectHeroGenic(UID uid, int nAddHeroID, int nAddSkinID)
	{
		obuf t_data;
		t_data << uid;
		t_data << nAddHeroID;
		t_data << nAddSkinID;

		BUILD_MSG_BUFFER(IWarService::collectHeroGenic, t_data);

		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}

	// 假死者复活
	virtual void onFakeDeadRelive(UID uid)
	{
		obuf t_data;
		t_data << uid;

		BUILD_MSG_BUFFER(IWarService::onFakeDeadRelive, t_data);

		m_pContainer->post_message(pMsg, nMsgLen, 0, MSG_FLAG_NO_BLOCK);
	}
};

/**
   存根对象:
   1.处理收到的消息调用请求，然后把他转发给真正的服务接口函数
*/
class WarService_Stub : public IServiceStub
{
	WarService_Proxy  m_proxy;
	WarService *      m_real_service;
	obuf              m_return_buffer;
public:
	WarService_Stub(WarService * pReal) : m_real_service(pReal) {
	}

	// 启动
	virtual bool on_start(SERVICE_PTR pContainer, void * data, int len)
	{
		m_proxy.m_pContainer = pContainer;

		return m_real_service->on_start(pContainer, data, len);
	}

	// 停止
	virtual bool on_stop()
	{
		m_real_service->on_stop();
		m_proxy.m_pContainer = SERVICE_PTR(0);
		return true;
	}

	// 处理消息
	virtual bool handle_message(SERVICE_MESSAGE * pMsg, rkt::obuf * resultBuf)
	{
		if (TEST_SAME_FUNCTION(IWarService::Start))
		{
			m_real_service->Start();
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::PrepareEnterWar))
		{
			SActorPreaperEnterWar sActorPreaperEnterWar;
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
			in >> sActorPreaperEnterWar;
			const bool & vReturn = m_real_service->PrepareEnterWar(sActorPreaperEnterWar);
			if (resultBuf) *resultBuf << vReturn;
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::setLegendCupInfo))
		{
			SMsgLegendCupRoomInfo sLegendCupInfo;
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
			in >> sLegendCupInfo;
			m_real_service->setLegendCupInfo(sLegendCupInfo);
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::onActorEnterWar))
		{
			SActorEnterWarBaseInfo sActorEnterWarBaseInfo;
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
			in >> sActorEnterWarBaseInfo;
			m_real_service->onActorEnterWar(sActorEnterWarBaseInfo);
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::onEntityDie))
		{
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
			sEntityDieInfo entityDieInfo;
			in >> entityDieInfo;

			m_real_service->onEntityDie(entityDieInfo);
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::onEntityDieEx))
		{
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
			SEntityReliveInfo sReliveInfo;
			sEntityDieInfo entityDieInfo;
			in >> entityDieInfo;

			m_real_service->onEntityDieEx(entityDieInfo, sReliveInfo);
			if (resultBuf) *resultBuf << sReliveInfo;
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::onEntityRelive))
		{
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
			UID uid;
			in >> uid;

			m_real_service->onEntityRelive(uid);

			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::onWarClientMsg))
		{
			UID uidActor; byte byKeyAction; size_t nLen;
			ibuffer in(pMsg->context, pMsg->context_len);
			in >> uidActor >> byKeyAction >> nLen;

			m_real_service->onWarClientMsg(uidActor, byKeyAction, in.current(), nLen);
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::getAllPerson))
		{
			UID* pReturnArray; int nArraySize;
			GET_MSG_PARAM_2(UID*, pReturnArray, int, nArraySize);
			const int & vReturn = m_real_service->getAllPerson(pReturnArray, nArraySize);
			if (resultBuf) *resultBuf << vReturn;
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::getWarSceneID))
		{
			const int & vReturn = m_real_service->getWarSceneID();
			if (resultBuf) *resultBuf << vReturn;
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::getWarDBKey))
		{
			if (resultBuf) *resultBuf << m_real_service->getWarDBKey();
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::getWarTypeId))
		{
			const int & vReTurn = m_real_service->getWarTypeId();
			if (resultBuf) *resultBuf << vReTurn;
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::getMatchTypeID))
		{
			const DWORD & vReTurn = m_real_service->getMatchTypeID();
			if (resultBuf) *resultBuf << vReTurn;
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::setChargedValue))
		{
			UID uidTarget; int nEffectID; int nValue;
			GET_MSG_PARAM_3(UID, uidTarget, int, nEffectID, int, nValue);

			m_real_service->setChargedValue(uidTarget, nEffectID, nValue);
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::getChargedValue))
		{
			UID uidTarget; int nEffectID;
			GET_MSG_PARAM_2(UID, uidTarget, int, nEffectID);
			const int & vReturn = m_real_service->getChargedValue(uidTarget, nEffectID);
			if (resultBuf) *resultBuf << vReturn;
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::setWarEndReason))
		{
			BYTE byEndReason;
			GET_MSG_PARAM_1(BYTE, byEndReason);
			m_real_service->setWarEndReason(byEndReason);
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::destroy))
		{
			m_real_service->destroy();
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::DragonBallEndWar))
		{
			int nFailedCamp;
			bool bNormalEnd;
			GET_MSG_PARAM_2(int, nFailedCamp, bool, bNormalEnd);
			m_real_service->DragonBallEndWar(nFailedCamp, bNormalEnd);
			return true;
		}

		// 场景已创建
		if (TEST_SAME_FUNCTION(IWarService::onSceneCreated))
		{
			int nWarID;
			GET_MSG_PARAM_1(int, nWarID);
			m_real_service->onSceneCreated(nWarID);
			return true;
		}

		// 场景已创建
		if (TEST_SAME_FUNCTION(IWarService::msgToSpecCampWildInfo))
		{
			UID uMonsterUid;
			int nCamp;
			GET_MSG_PARAM_2(UID, uMonsterUid, int, nCamp);
			m_real_service->msgToSpecCampWildInfo(uMonsterUid, nCamp);
			return true;
		}



		if (TEST_SAME_FUNCTION(IWarService::warSerchTypeUIDList))
		{
			BYTE bySelfCamp; BYTE byGetCampType; BYTE bySerchType; PDBID* pReturnArray; DWORD dwArrayMaxSize;
			GET_MSG_PARAM_5(BYTE, bySelfCamp, BYTE, byGetCampType, BYTE, bySerchType, PDBID*, pReturnArray, DWORD, dwArrayMaxSize);

			*resultBuf << m_real_service->warSerchTypeUIDList(bySelfCamp, byGetCampType, bySerchType, pReturnArray, dwArrayMaxSize);
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::warAddSerchTypeBuff))
		{
			SWarEffectAddBuff sData;
			GET_MSG_PARAM_1(SWarEffectAddBuff, sData);

			m_real_service->warAddSerchTypeBuff(sData);
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::updateActorBuffList))
		{
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
			size_t buf_len = 0;
			in >> buf_len;

			m_real_service->updateActorBuffList(in.current(), buf_len);
			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::deSerialzed))
		{
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
			UID uid;
			int flag;
			in >> uid >> flag;

			obuf out;
			m_real_service->deSerialzed(uid, out, flag);
			if (resultBuf)
				resultBuf->push_back(out.data(), out.size());

			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::setBanHeroList))
		{
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
			size_t buf_len = 0;
			in >> buf_len;

			m_real_service->setBanHeroList(in.current(), buf_len);

			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::cureMember))
		{
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
			UID uid = 0;
			in >> uid;

			m_real_service->cureMember(uid);

			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::changeVocation))
		{
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
			UID uid = 0;
			int nSlotID = 0;
			in >> uid >> nSlotID;

			m_real_service->changeVocation(uid, nSlotID);

			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::collectHeroGenic))
		{
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
			UID uid = 0;
			int nHeroID = 0;
			int nSkinID;
			in >> uid >> nHeroID >> nSkinID;

			m_real_service->collectHeroGenic(uid, nHeroID, nSkinID);

			return true;
		}

		if (TEST_SAME_FUNCTION(IWarService::onFakeDeadRelive))
		{
			rkt::ibuffer in(pMsg->context, pMsg->context_len);
			UID uid = 0;
			in >> uid;

			m_real_service->onFakeDeadRelive(uid);

			return true;
		}

		ErrorLn(__FUNCTION__":can not found function name:" << MsgFunctionName<SERVICE_MESSAGE>()(pMsg));
		return false;
	}

	// 返回代理对象即可
	virtual void * query_interface(int iid)
	{
		return &m_proxy;
	}

	// 销毁
	virtual void release()
	{
		if (m_real_service)
		{
			m_real_service->release();
			m_real_service = 0;
		}

		delete this;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) IServiceStub * CreateWarService()
{
	return new WarService_Stub(new WarService);
}