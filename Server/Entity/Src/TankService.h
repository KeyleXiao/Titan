/*******************************************************************
** 文件名:	TankService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	12/24/2014
** 版  本:	1.0
** 描  述:	

			Tank服务，管理所有非玩家对象，包括怪物和Tank(一个协程)
********************************************************************/

#pragma once

#include "ITankService.h"
#include "EntityDef.h"
#include "Tank.h"
#include "TankProperty.h"
#include "IEntityAOI.h"
#include "IEnmityServer.h"
#include "IDiePart.h"
#include <map>

/**
	职责: 
	1.管理某个场景的所有怪物，Tank的创建销毁
	2.管理某个场景的所有怪物AI
	3.管理某个场景的所有Tank对话
*/
class TankService : public ITankService
{
	typedef std::map<UID,__IEntity *>  Tank_MAP;
	Tank_MAP   m_TankMap;
	
    int       m_nMapID;           // 地图ID
	int       m_nSceneID;         // 场景ID
	int       m_nAllotIndex;      // UID分配索引


public:
	TankService() : m_nMapID(0),m_nSceneID(0),m_nAllotIndex(0) {}

	virtual ~TankService(){}

	bool on_start(int nMapID, int scene )
	{
		m_nMapID = nMapID;
		m_nSceneID = scene;

		return true;
	}

	bool on_stop()
	{
        destroyAll();
		return true;
	}

	// 手动创建Tank
	virtual UID createTank( int id,Vector3 loc,void * context,int len )
	{
		// 总数已经到达上限
		if ( m_TankMap.size()>=MAX_TANK_COUNT )
			return INVALID_UID;

		// todo 这个是临时的，不为最优的分配，分配一个合法ID
		do 
		{
			++m_nAllotIndex;

			if ( m_nAllotIndex>=MAX_TANK_COUNT )
			{
				m_nAllotIndex = 0;
			}
		} while (m_TankMap.find(m_nAllotIndex)!=m_TankMap.end() );

		TANK_UID_DATA uid_data;
		uid_data.type = TYPE_TANK;
		uid_data.scene= m_nSceneID;
		uid_data.serial = m_nAllotIndex;

		__IEntity * pTank = new CTank;
		if ( pTank==0 )
			return INVALID_UID;

		UID uid = *((UID *)&uid_data);

		// 假设从数据库读取到角色数据
		TankProperty Tank_data;
		Tank_data.nID		= id;			// 载具ID
		sstrcpyn( Tank_data.szName,"怪物",sizeof(Tank_data.szName) );
		Tank_data.nSceneID	= 1;         // 所在地图
		Tank_data.location	= loc;       // 在地图上的位置
		//Tank_data.nVocation = id;       // 预设英雄,玩家职业
		Tank_data.nLevel = 1;		   // 等级  
		Tank_data.nHP    = 100;		   // 血量
		Tank_data.nMaxHP = 100;		   // 最大血量
		Tank_data.nMP    = 100;		   // 法术值
		Tank_data.nMaxMP = 100;		   // 最大法术值
		Tank_data.uid    = uid;

		unsigned short nflag =  TO_CLIENT_OTHER;
		unsigned short partid = PART_PROPERTY;

		obuf o;
		o << partid;
        int nLenOffset = o.size();
        int nLen = 0;
        o << nLen;  // 压入部件长度
        int nBegin = o.size();
		Tank_data.getDataBuffer( o,nflag );
        nLen = o.size() - nBegin;
        *((int*)(o.data() + nLenOffset)) = nLen;

		// 设置地图ID
		((CTank*)pTank)->setMapID(m_nMapID);

		// 创建实体
		if ( pTank->create(uid,o.data(),o.size(),nflag )==false )
		{
			pTank->release();
			return INVALID_UID;
		}

		m_TankMap[uid] = pTank;
		return uid;
	}

	// 手动销毁Tank
	virtual bool destroyTank( UID uid )
	{
		__IEntity * pTank = findTank(uid);
		if ( pTank==0 )
			return false;

		pTank->release();

		// 结构中释放掉
		m_TankMap.erase(uid);

		return true;
	}


	__IEntity * findTank( UID uid )
	{
		Tank_MAP::iterator it = m_TankMap.find(uid);
		if ( it==m_TankMap.end() )
			return 0;

		return it->second;
	}

	__IEntityProperty * getProperty( UID uid )
	{
		__IEntity * pTank= findTank(uid);
		if ( pTank==0 )
			return 0;

		return (__IEntityProperty*)pTank->getEntityPart(PART_PROPERTY);
	}

	__IEntityAOI * getAOI( UID uid )
	{
		__IEntity * pTank= findTank(uid);
		if ( pTank==0 )
			return 0;

		return (__IEntityAOI*)pTank->getEntityPart(PART_AOI);
	}

	__IEnmityPart * getEnmity( UID uid )
	{
		__IEntity * pTank= findTank(uid);
		if ( pTank==0 )
			return 0;

		return (__IEnmityPart*)pTank->getEntityPart(PART_ENMITY);
	}
	__IDiePart * getDiePart( UID uid )
	{
		__IEntity * pTank= findTank(uid);
		if ( pTank==0 )
			return 0;

		return (__IDiePart*)pTank->getEntityPart(PART_DIE);
	}
	__IBuffPart * getBuffPart( UID uid )
	{
		__IEntity * pTank= findTank(uid);
		if ( pTank == 0 )
			return 0;

		return (__IBuffPart*)pTank->getEntityPart(PART_BUFF);
	}
	__IMovePart * getMovePart( UID uid )
	{
		__IEntity * pTank= findTank(uid);
		if ( pTank == 0 )
			return 0;

		return (__IMovePart*)pTank->getEntityPart(PART_MOVE);
	}

	//////////////////////////////// IEntity ///////////////////////////////////////////////
	// 取得实体名称
	virtual string getName(UID uid)
	{
		__IEntity * pTank = findTank(uid);
		if ( pTank==0 )
			return "Unknown";

		return pTank->getName();
	}

	// 取得当前所在场景ID
	virtual int getSceneID()
	{
		return m_nSceneID;
	}

	// 取得实体位置
	virtual Vector3 getLocation(UID uid)
	{
		__IEntity * pTank = findTank(uid);
		if ( pTank==0 )
			return Vector3();

		return pTank->getLocation();
	}

	// 设置实体位置
	virtual void setLocation(UID uid, Vector3 vLoc)
	{
		__IEntity * pTank = findTank(uid);
		if ( pTank==0 )
			return;

		pTank->setLocation(vLoc);
	}

	// 取得实体角度
	virtual Vector3 getAngle(UID uid)
	{
		__IEntity * pTank = findTank(uid);
		if ( pTank==0 )
			return Vector3();

		return pTank->getAngle();
	}

	// 处理消息
	virtual void handleMessage( UID uid,void *pData,int nLen )
	{
		__IEntity * pTank = findTank(uid);
		if ( pTank==0 )
			return;

		return pTank->handleMessage(pData,nLen);
	}

    virtual void handleReplaceableMessage(UID uid, unsigned long ulKey,void * pMsg,int nLen)
    {

    }

	virtual void changeState(UID uidTank, int nState)
	{
		__IEntity * pTank= findTank(uidTank);
		if ( pTank == 0 )
			return ;
		pTank->changeState(nState);
	}
	virtual int getState(UID uidTank)
	{
		__IEntity * pTank= findTank(uidTank);
		if ( pTank == 0 )
			return 0;

		return pTank->getState();
	}

	/////////////////////// IEntityProperty ///////////////////////////////////////////////
	// 取得某个属性的类型
	virtual string getPropertyType( UID uid,int prop_id )
	{
		__IEntityProperty * property = getProperty(uid);
		if ( property==0 )
			return "";

		return property->getPropertyType(prop_id);
	}

	// 取得某个属性的标志数据(通常是序列化标志)
	virtual int getPropertyFlag( UID uid,int prop_id )
	{
		__IEntityProperty * property = getProperty(uid);
		if ( property==0 )
			return 0;

		return property->getPropertyFlag(prop_id);
	}

	// 获取实体属性(布尔类型）
	virtual bool getProperty_Bool( UID uid,int prop_id)
	{
		__IEntityProperty * property = getProperty(uid);
		if ( property==0 )
			return false;

		return property->getProperty_Bool(prop_id);
	}

	// 设置实体属性(布尔类型）
	virtual bool setProperty_Bool( UID uid,int prop_id,bool value)
	{
		__IEntityProperty * property = getProperty(uid);
		if ( property==0 )
			return false;

		return property->setProperty_Bool(prop_id,value);
	}

	// 获取实体属性(数字,实际内部支持用int,byte,short三种类型存)
	virtual int getProperty_Integer( UID uid,int prop_id)
	{
		__IEntityProperty * property = getProperty(uid);
		if ( property==0 )
			return 0;

		return property->getProperty_Integer(prop_id);
	}

	// 设置实体属性(数字,实际内部支持用int,byte,short三种类型存)
	virtual bool setProperty_Integer( UID uid,int prop_id,int value)
	{
		__IEntityProperty * property = getProperty(uid);
		if ( property==0 )
			return false;

		return property->setProperty_Integer(prop_id,value);
	}

	// 增加属性数值(注意内部需要控制上下限)
	virtual bool addProperty_Integer( UID uid,int prop_id,int delta)
	{
		__IEntityProperty * property = getProperty(uid);
		if ( property==0 )
			return false;

		return property->addProperty_Integer(prop_id,delta);
	}

	// 获取实体属性(字符串)
	virtual string getProperty_String( UID uid,int prop_id )
	{
		__IEntityProperty * property = getProperty(uid);
		if ( property==0 )
			return "";

		return property->getProperty_String(prop_id);
	}

	// 设置实体属性(字符串)
	virtual bool setProperty_String( UID uid,int prop_id,string value)
	{
		__IEntityProperty * property = getProperty(uid);
		if ( property==0 )
			return false;

		return property->setProperty_String(prop_id,value);
	}

	// 获取实体属性(自定义结构,如果属性存在返回)
	virtual string getProperty_custom( UID uid,int prop_id)
	{
		__IEntityProperty * property = getProperty(uid);
		if ( property==0 )
			return "";

		return property->getProperty_String(prop_id);
	}

	// 设置实体属性(自定义结构)
	virtual bool setProperty_custom( UID uid,int prop_id,string value)
	{
		__IEntityProperty * property = getProperty(uid);
		if ( property==0 )
			return false;

		return property->setProperty_custom(prop_id,(void*)value.c_str(),value.size());
	}

	/////////////////////// IEntityAOI ///////////////////////////////////////////////
	// 取得视野半径
	virtual float getSight(UID uid)
	{
		__IEntityAOI * pAOI = getAOI(uid);
		if ( pAOI==0 )
			return 0.0f;

		return pAOI->getSight();
	}

	// 调整视野半径
	virtual void setSight( UID uid,float radii )
	{
		__IEntityAOI * pAOI = getAOI(uid);
		if ( pAOI==0 )
			return ;

		return pAOI->setSight(radii);
	}

	// 是否隐身
	virtual bool isHide(UID uid)
	{
		__IEntityAOI * pAOI = getAOI(uid);
		if ( pAOI==0 )
			return false;

		return pAOI->isHide();
	}

	// 设置隐身状态
	virtual void setHide( UID uid,bool state )
	{
		__IEntityAOI * pAOI = getAOI(uid);
		if ( pAOI==0 )
			return ;

		return pAOI->setHide(state);
	}

	/////////////////////// IEnmityPart ///////////////////////////////////////////////
	// 添加仇恨数据
	virtual void AddEnmityValue(UID uidTank, UID uidDes, int nVal)
	{
		__IEnmityPart * pPart = getEnmity(uidTank);
		if ( pPart==0 )
			return ;

		pPart->AddEnmityValue(uidDes, nVal);
		//*/
	}
	virtual void AddBroadEnmityValue(UID uidTank, UID uidDes, int nVal)
	{
		__IEnmityPart * pPart = getEnmity(uidTank);
		if ( pPart==0 )
			return ;

		pPart->AddBroadEnmityValue(uidDes, nVal);
		//*/
	}

	/////////////////////// IBuffPart ///////////////////////////////////////////////
	virtual bool AddBuff(UID uidTank, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0)
	{
		__IBuffPart * pPart = getBuffPart(uidTank);
		if ( pPart==0 )
			return false;
		return pPart->Add(dwBuffID, dwBuffLevel, uidAdd, dwTime, pBuffContext, nContextLen);
	}
	virtual bool RemoveBuff(UID uidTank, DWORD dwBuffID, UID uidAdd, UID uidRemove)
	{
		__IBuffPart * pPart = getBuffPart(uidTank);
		if ( pPart==0 )
			return false;
		return pPart->Remove(dwBuffID, uidAdd, uidRemove);
	}
	virtual bool IsExistBuff(UID uidTank, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd)
	{
		__IBuffPart * pPart = getBuffPart(uidTank);
		if ( pPart==0 )
			return false;
		return pPart->IsExist(dwBuffID, dwBuffLevel, uidAdd);
	}
    virtual bool IsExistBuffType(UID uidTank, DWORD dwBuffType)
    {
        __IBuffPart * pPart = getBuffPart(uidTank);
        if ( pPart==0 )
            return false;
        return pPart->IsExist(dwBuffType);
    }
	virtual DWORD GetBuffLevel(UID uidTank, DWORD dwBuffID, UID uidAdd)
	{
		__IBuffPart * pPart = getBuffPart(uidTank);
		if ( pPart==0 )
			return 0;
		return pPart->GetBuffLevel(dwBuffID, uidAdd);
	}
	virtual int GetLeftTime(UID uidTank, DWORD dwBuffID, UID uidAdd)
	{
		__IBuffPart * pPart = getBuffPart(uidTank);
		if ( pPart==0 )
			return -1;
		return pPart->GetLeftTime(dwBuffID, uidAdd);
	}

	virtual void SetAccumulateBuff(UID uidTank, DWORD dwBuffID, UID uidAdd, bool bOverlay, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0)
	{
		__IBuffPart * pPart = getBuffPart(uidTank);
		if ( pPart==0 )
			return;
		pPart->SetAccumulateBuff(dwBuffID, uidAdd, bOverlay, dwTime, pBuffContext, nContextLen);
	}

	virtual bool RemoveBuffByIndex(UID uidTank,DWORD dwIndex)
	{
		__IBuffPart * pPart = getBuffPart(uidTank);
		if ( pPart==0 )
			return false;
		return pPart->RemoveBuffByIndex(dwIndex);
	}
	/////////////////////// IMovePart ///////////////////////////////////////////////
	virtual bool Transport(UID uidTank, Vector3 newLoc, Vector3 rotation, bool bTransport, int nSceneID, int nReason)
	{
		__IMovePart* pPart = getMovePart(uidTank);
		if(pPart == NULL)
		{
			return false;
		}

		Move3 move = pPart->getMoveState();
		move.vPosition = newLoc;
        if(rotation != Vector3(-1,-1,-1))
        {
            move.vRotation = rotation;
        }

		if (bTransport)
		{
			return pPart->stopAt( move,nSceneID,nReason );
		}
		else
		{
			pPart->changeMoveState(move);
		}
		return true;
	}

	virtual bool MoveTo(UID uidTank, Vector3 newLoc)
	{
		return false;
	}

    virtual bool isInsight(UID uid, int camp)
    {
        __IEntityAOI * pAOI = getAOI(uid);
        if ( pAOI==0 )
            return false;

        return pAOI->isInsight(camp);
    }

    virtual int getWarID(UID uid)
    {
        __IEntity * pTank= findTank(uid);
        if ( pTank == 0 )
            return 0;

        __IWarPart* pWarPart =  (__IWarPart*)pTank->getEntityPart(PART_WAR);
        if(pWarPart == NULL)
        {
            return 0;
        }

        return pWarPart->getWarID();
    }

    virtual BuffTypeList getBuff(UID uid, DWORD dwBuffType)
    {
        __IEntity * pTank= findTank(uid);
        if ( pTank == 0 )
            return BuffTypeList();

        __IBuffPart* pBuffPart =  (__IBuffPart*)pTank->getEntityPart(PART_BUFF);
        if(pBuffPart == NULL)
        {
            return BuffTypeList();
        }

        DWORD dwBuffID[BUFF_TYPE_MAX_COUNT] = {0};
        DWORD dwBuffLevel[BUFF_TYPE_MAX_COUNT] = {0};
        int nBuffTime[BUFF_TYPE_MAX_COUNT] = {0};
        int nCount = BUFF_TYPE_MAX_COUNT;

        pBuffPart->GetBuff(dwBuffType,dwBuffID,dwBuffLevel,nBuffTime,nCount);

        BuffTypeList buffType;
        buffType.nCount = nCount;
        memcpy(buffType.dwBuffID, dwBuffID, sizeof(buffType.dwBuffID));
        memcpy(buffType.dwBuffLevel, dwBuffLevel, sizeof(buffType.dwBuffLevel));
        memcpy(buffType.nBuffTime, nBuffTime, sizeof(buffType.nBuffTime));

        return buffType;
    }

	// 移除所有Tank
	virtual void destroyAll()
	{
		__IEntity *pEntity = NULL;

		Tank_MAP::iterator iterEnd = m_TankMap.end();
		for (Tank_MAP::iterator iter=m_TankMap.begin(); iter != iterEnd; ++iter)
		{
			// 删除TANK
			pEntity = iter->second;
			if (pEntity != NULL)
			{
				pEntity->release();
			}
		}

		m_TankMap.clear();
	}
    
    virtual bool chooseTarget(UID uid, int nTargetFilter, UID uidTarget)
    {
        __IEntity * pTank= findTank(uid);
        if ( pTank == 0 )
            return false;

        __IPlayerKillPart* pPlayerKillPart =  (__IPlayerKillPart*)pTank->getEntityPart(PART_PK);
        if(pPlayerKillPart == NULL)
        {
            return false;
        }

        return pPlayerKillPart->chooseTarget(nTargetFilter, uidTarget);
    }

    virtual int getPKType(UID uid)
    {
        __IEntity * pTank= findTank(uid);
        if ( pTank == 0 )
            return 0;

        __IPlayerKillPart* pPlayerKillPart =  (__IPlayerKillPart*)pTank->getEntityPart(PART_PK);
        if(pPlayerKillPart == NULL)
        {
            return 0;
        }

        return pPlayerKillPart->getPKType();
    }

    virtual int getPKState(UID uid)
    {
        __IEntity * pTank= findTank(uid);
        if ( pTank == 0 )
            return 0;

        __IPlayerKillPart* pPlayerKillPart =  (__IPlayerKillPart*)pTank->getEntityPart(PART_PK);
        if(pPlayerKillPart == NULL)
        {
            return 0;
        }

        return pPlayerKillPart->getPKState();
    }

    virtual void forceEndPK(UID uid)
    {
        __IEntity * pTank= findTank(uid);
        if ( pTank == 0 )
            return;

        __IPlayerKillPart* pPlayerKillPart =  (__IPlayerKillPart*)pTank->getEntityPart(PART_PK);
        if(pPlayerKillPart == NULL)
        {
            return ;
        }

        pPlayerKillPart->forceEndPK();
    }

	// 获取PK阵营
	virtual int getPKCamp(UID uid)
	{
		__IEntity * pTank = findTank(uid);
		if (pTank == 0)
			return 0;

		__IPlayerKillPart * pPlayerKillPart = (__IPlayerKillPart *)pTank->getEntityPart(PART_PK);
		if (pPlayerKillPart == NULL)
		{
			return 0;
		}

		return pPlayerKillPart->getPKCamp();
	}

    // 取得当前所在地图ID
    virtual int getMapID(UID uid)
    {
        return m_nMapID;
    }

    // 冲锋
    virtual void shift(UID uidTank, void *pData,int nDataLen)
    {
        __IMovePart* pMovePart = getMovePart(uidTank);
        if(pMovePart == NULL)
        {
            return;
        }

        pMovePart->shift(pData, nDataLen);
    }

    // 添加效果 返回效果ID记录
    virtual	int mobaGoodsAddEffect(UID uid, int nEffectID, void * pAddContext, size_t len)
    {
        __IEntity * pTank= findTank(uid);
        if ( pTank == 0 )
            return false;

        return pTank->mobaGoodsAddEffect(nEffectID, pAddContext, len);
    }

    // 移除效果
    virtual	void mobaGoodsRemoveEffect(UID uid, int nID)
    {
        __IEntity * pTank= findTank(uid);
        if ( pTank == 0 )
            return;

        pTank->mobaGoodsRemoveEffect(nID);
    }

    // 实体通知主人
    virtual void entityNotifyMaster(UID uid, EntityBelongContext context)
    {

    }

	// 释放
	virtual void release()
	{
		on_stop();
		delete this;
	}
};

