/*******************************************************************
** 文件名:	NPCService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	12/24/2014
** 版  本:	1.0
** 描  述:	

			NPC服务，管理所有非玩家对象，包括怪物和NPC(一个协程)
********************************************************************/

#pragma once

#include "INPCService.h"
#include "EntityDef.h"
#include "Monster.h"
#include "MonsterProperty.h"
#include "IEntityAOI.h"
#include "IEnmityServer.h"
#include "IBuffServer.h"
#include "ISchemeCenter.h"
#include "IPathFindManager.h"
#include "Simple_Atom_Lock.h"
#include "share_ptr.h"
#include <map>

#include "ISchemeCenter.h"
#include "DiePart.h"
#include "net/producer_consumer_queue.h"

#undef  MAX_NPC_COUNT
#define MAX_NPC_COUNT           1024
// 怪物最大序号
#define  MAX_NPC_INDEX          MAX_NPC_COUNT

/**
	职责: 
	1.管理某个场景的所有怪物，NPC的创建销毁
	2.管理某个场景的所有怪物AI
	3.管理某个场景的所有NPC对话
*/
class NPCService : public INPCService
{
    int       m_nMapID;           // 地图ID
    int       m_nSceneID;         // 场景ID

	NPC_PTR   m_npcMap[MAX_NPC_COUNT];

    typedef producer_consumer_queue_ex<WORD>      INDEX_QUEUE;
	INDEX_QUEUE  m_index_queue; // UID序号队列

    std::map<int, std::map<int, std::map<int, int>>> m_mapHandleMessageStatTable;
    DWORD m_dwStartStatHandleMessageTick;

	//////////////////////////////////////////////////////////////////////////
public:
	NPCService() : m_nSceneID(0),m_nMapID(0), m_dwStartStatHandleMessageTick(0)
	{
        // 初始化最大桶数量
        for (WORD i=0; i<MAX_NPC_INDEX; ++i) {
            m_index_queue.push_ex(i);
        }
	}

	virtual ~NPCService(){}

	// 非线程安全宏
#define INNER_FIND_NPC(uid) \
	NPC_PTR pNPC = findNPC(uid)

#define INNER_GET_MONSTER_PROPERTY_PART(uid, pPropertyPart) \
	INNER_FIND_NPC(uid);\
	pPropertyPart = NULL;\
	if (pNPC != 0)	pPropertyPart = (__IEntityProperty*)(pNPC->getEntityPart(PART_PROPERTY));

#define INNER_GET_MONSTER_AOI_PART(uid, pAOIPart) \
	INNER_FIND_NPC(uid);\
	pAOIPart = NULL;\
	if (pNPC != 0)	pAOIPart = (__IEntityAOI*)(pNPC->getEntityPart(PART_AOI));

#define INNER_GET_MONSTER_BUFF_PART(uid, pBuffPart) \
	INNER_FIND_NPC(uid);\
	pBuffPart = NULL;\
	if (pNPC != 0)	pBuffPart = (__IBuffPart*)(pNPC->getEntityPart(PART_BUFF));

#define INNER_GET_MONSTER_ENMITY_PART(uid, pEnmityPart) \
	INNER_FIND_NPC(uid);\
	pEnmityPart = NULL;\
	if (pNPC != 0)	pEnmityPart = (__IEnmityPart*)(pNPC->getEntityPart(PART_ENMITY));

#define INNER_GET_MONSTER_WAR_PART(uid, pWarPart) \
    INNER_FIND_NPC(uid);\
    pWarPart = NULL;\
    if (pNPC != 0)	pWarPart = (__IEnmityPart*)(pNPC->getEntityPart(PART_WAR));

#define INNER_GET_MONSTER_MOVE_PART(uid, pMovePart) \
    INNER_FIND_NPC(uid);\
    pMovePart = NULL;\
    if (pNPC != 0)	pMovePart = (__IMovePart*)(pNPC->getEntityPart(PART_MOVE));

	// 无锁查找NPC
	NPC_PTR findNPC(UID uid)
	{
		size_t index = UID_2_SERIAL(uid) % MAX_NPC_COUNT;

		return m_npcMap[index];  // 智能指针赋值不是原子的，所以还是有安全隐患
	}

	bool on_start( int mapID,int scene )
	{
        TraceLn( "NPCService:on_start mapID = "<<mapID<<", sceneID = "<<scene );

        m_nMapID = mapID;
		m_nSceneID = scene;

        // 创建并启动刷怪器

		return true;
	}

	bool on_stop()
	{
        TraceLn("start destroy all npc...");
        destroyAll();
		return true;
	}

    WORD pop_index()
    {
        WORD n = 0;
        return m_index_queue.pop_ex(n) ? n : MAX_NPC_INDEX;
    }
	
    bool push_index(WORD wIndex)
    {
        if(wIndex >= MAX_NPC_INDEX) {
            return false;
        }
        return m_index_queue.push_ex(wIndex);
    }


    UID genUID()
    {
        // 现在直接用队列中可用的序号，不过当队列中只剩下一个空时会出现某个怪物刚死亡又创建一个同UID的新怪，客户端会出错
        WORD wIndex = pop_index();
        if(wIndex < MAX_NPC_INDEX)
        {
            NPC_UID_DATA uid_data;
            uid_data.type   = TYPE_MONSTER;
            uid_data.scene  = m_nSceneID;
            uid_data.serial = wIndex;
            return *((UID *)&uid_data);;
        }

        return INVALID_UID;
    }

    virtual UID createNPC( int id, Vector3 loc, CreateNpcContext context)
    {
        UID uid = genUID();
        if(uid != INVALID_UID)
        {
            doCreate(id, uid, loc, context);
        }
        return uid;
    }

	// 手动创建NPC
	void doCreate( int id, UID uid, Vector3 loc, CreateNpcContext context)
	{
        COST_TIME_TRACE_START

		// 检查传入的UID是否正确
        if(findNPC(uid) != NPC_PTR(0))
        {
            ErrorLn("NPCService::doCreate id=" << id << " failed, 1");
            return;
        }

        COST_TIME_TRACE(__LINE__)

        Monster * pMonster = new Monster;
		__IEntity * pNPC = CAST_TYPE(__IEntity*, pMonster);
		if ( pNPC==0 )
        {
            ErrorLn("NPCService::doCreate id=" << id << " new object failed");
            return;
        }

		ISchemeCenter * pSchemeCenter =  gServerGlobal->getSchemeCenter();
		if(pSchemeCenter == NULL)
		{
            ErrorLn("NPCService::doCreate id=" << id << " pSchemeCenter==0");
            safeRelease(pNPC);
			return;
		}
		ISchemeMonster* pSchemeMonstor = gServerGlobal->getSchemeCenter()->getSchemeMonster();
		if (pSchemeMonstor == NULL)
		{
            ErrorLn("NPCService::doCreate id=" << id << " pSchemeMonstor==0");
			safeRelease(pNPC);
			return;
		}
		SMonsterScheme * pSchemeInfo = pSchemeMonstor->getMonsterShemeInfo(id);
		if (pSchemeInfo == NULL)
		{
            ErrorLn("NPCService::doCreate id=" << id << " pSchemeInfo==0");
			safeRelease(pNPC);
			return;
		}

		int nCamp = pSchemeInfo->byCamp;

		int nCampParm = context.nCamp;
		if ( nCampParm >0 )
		{
			nCamp = nCampParm;
		}			
		context.nLevel = max(1, context.nLevel);

        Vector3 revisedLoc = loc;

        COST_TIME_TRACE(__LINE__)

        gServerGlobal->getHeightFieldMgr()->correctPosFromPhysic(m_nMapID, revisedLoc, 50, 50);

        COST_TIME_TRACE(__LINE__)
// 
//         IPathFindManager * pPathFindMgr = gServerGlobal->getPathFindManager();
//         if ( pPathFindMgr!=NULL )
//         {
// 			float fHeight;
// 			if ( pPathFindMgr->getHeight(m_nMapID, m_nSceneID, &loc, fHeight) )
// 			{
// 				revisedLoc.y = fHeight;
// 			}
//         }
        if ( abs(revisedLoc.y-loc.y)>10.0f )
        {
            ErrorLn( "monster id:"<<id<<"'s location("<<
                      loc.x<<","<<loc.y<<","<<loc.z<<") is revised to("<<
                      revisedLoc.x<<","<<revisedLoc.y<<","<<revisedLoc.z<<")" );
        }		

		MonsterProperty npc_data;
		npc_data.nID			= id;						// 怪物ID
		sstrcpyn( npc_data.szName,pSchemeInfo->szName,sizeof(npc_data.szName) );
		npc_data.nSceneID		= m_nSceneID;				// 所在地图
		npc_data.location		= revisedLoc;		    	// 在地图上的位置
		npc_data.angle			= context.Angle;			// 在地图上的朝向
		npc_data.nMonsType		= pSchemeInfo->nMonsType;	// 怪物类型
        npc_data.nVocation		= pSchemeInfo->nSubType;	// 怪物子类型 MONSTER_SUB_TYPE
		npc_data.nSkin			= pSchemeInfo->nDefaultSkin;// 皮肤（外形）
		npc_data.nLevel			= context.nLevel;			// 等级
		npc_data.nHP			= pSchemeInfo->nMaxHP;		// 血量
		npc_data.nMaxHP			= pSchemeInfo->nMaxHP;		// 最大血量
		npc_data.byCamp			= nCamp;					// 阵营
		npc_data.nMP			= 100;						// 法术值
		npc_data.nMaxMP			= 100;						// 最大法术值
		npc_data.uid			= uid;
        npc_data.uidMaster      = context.uidCreator;
		npc_data.nPASD			= pSchemeInfo->nASD;	    // 基础攻击速度
		npc_data.nMask			= pSchemeInfo->nMonsterMask;// 标志位
        //npc_data.nObstacleFlag  = pSchemeInfo->nObsFlag;    // 可否穿越动态障碍标志位
        npc_data.nObstacleFlag  = 0x01;    // 可否穿越动态障碍标志位
        npc_data.nAISchemeID    = context.nAIID; 

		unsigned short nflag =  TO_PUBLIC;
        unsigned short partid = PART_PROPERTY;

        obuf o;
        // 嵌入创建上下文
        o << context;

        // 压入部件
        o << partid;
        int nLenOffset = o.size();
        o << (int)0;  // 压入部件长度
        int nBegin = o.size();

        COST_TIME_TRACE(__LINE__)

        npc_data.getDataBuffer( o,nflag );

        COST_TIME_TRACE(__LINE__)

        *((int*)(o.data() + nLenOffset)) = o.size() - nBegin;

        partid = PART_MOVE;
        o << partid;
        nLenOffset = o.size(); 
        o << (int)0;  // 压入部件长度
        nBegin = o.size();
        o << npc_data.location << npc_data.angle;  

        *((int*)(o.data() + nLenOffset)) = o.size() - nBegin;
        
        partid = PART_WAR; 
        o << partid;
        nLenOffset = o.size(); 
        o << (int)0;  // 压入部件长度
        nBegin = o.size();
        o << context.nWarID;
        o << (int)-1; 
        o << (int)0;

        *((int*)(o.data() + nLenOffset)) = o.size() - nBegin;

		// 设置地图ID
		pMonster->setMapID(m_nMapID);

        COST_TIME_TRACE(__LINE__)
       
        // 先将MONSTER加入到容器中
		int index  = UID_2_SERIAL(uid) % MAX_NPC_COUNT;
        m_npcMap[index] =  NPC_PTR(pNPC); 

        COST_TIME_TRACE(__LINE__)

		// 创建实体
		if ( !pNPC->create(uid,o.data(),o.size(),nflag ) )
		{
            ErrorLn("NPCService::doCreate id=" << id << ", call create() failed");
            destroyNPC(uid);
			return;
        }

        COST_TIME_TRACE(__LINE__)

		// 设置怪物阵营
		pMonster->setCamp(nCamp);
        // 设置怪物AI配置ID
        __IAIPart* pAIPart = (__IAIPart*)pMonster->getEntityPart(PART_AI);
        if(pAIPart)
        {
            pAIPart->setPathID(context.nPathID);
        }
		// 设置怪物气势加成系数
		pMonster->setVigourRatio(context.VigourRatio);

		// 设置PK类型
		__IPlayerKillPart* pPKPart = (__IPlayerKillPart*)pMonster->getEntityPart(PART_PK);
		if (pPKPart)
		{
			pPKPart->setPKType(context.nPKType);
			pPKPart->setPKState(context.nPKState);
		}

        COST_TIME_TRACE(__LINE__)

        //if (context.bRepication)
        //{
        //    // 分身怪物 发送
        //    __IEntityEvent * pEntityEvent = pMonster->getEntityEvent();
        //    if(pEntityEvent)
        //    {
        //        event_entity_replication_monster eventData;
        //        eventData.bAdd = true;
        //        eventData.uidMaster = context.uidCreator;
        //        eventData.uidMonster = uid;

        //        pEntityEvent->FireExecute(EVENT_ENTITY_REPLICATION_MONSTER, (LPCSTR) &eventData, sizeof(eventData));
        //        pEntityEvent->FireExecute(EVENT_GAME_ENTITY_REPLICATION_MONSTER, (LPCSTR) &eventData, sizeof(eventData));
        //        
        //    }
        //}
	}

	// 手动销毁NPC
	virtual bool destroyNPC( UID uid )
	{
		size_t index = UID_2_SERIAL(uid) % MAX_NPC_COUNT;
		if ( m_npcMap[index]==0 )
			return false;

		m_npcMap[index]->destroy();               // 智能指针只是让多线程调用时不要崩溃，对象的生存周期还是要自己管理

        m_npcMap[index]=NPC_PTR(0);

        // 最后来回收序号
        if(!push_index(index))
        {
            ErrorLn(__FUNCTION__": index greater than "<<MAX_NPC_INDEX<<", uid="<<uid <<", index="<< index);
        }

		return true;
	}

	//////////////////////////////// IEntity ///////////////////////////////////////////////
	// 取得实体名称
	virtual string getName(UID uid)
	{
		NPC_PTR pNPC = findNPC(uid);
		if ( pNPC==0 )
			return "Unknown";

		return pNPC->getName();
	}

	// 取得当前所在场景ID
	virtual int getSceneID()
	{
		return m_nSceneID;
	}

	// 取得实体位置
	virtual Vector3 getLocation(UID uid)
	{
		NPC_PTR pNPC = findNPC(uid);
		if ( pNPC==0 )
			return Vector3();

		return pNPC->getLocation();
	}

	// 设置实体位置
	virtual void setLocation(UID uid, Vector3 vLoc)
	{
		NPC_PTR pNPC = findNPC(uid);
		if ( pNPC==0 )
			return;

		pNPC->setLocation(vLoc);
	}

	// 取得实体角度
	virtual Vector3 getAngle(UID uid)
	{
		NPC_PTR pNPC = findNPC(uid);
		if ( pNPC==0 )
			return Vector3();

		return pNPC->getAngle();
	}

    // 添加效果 返回效果ID记录
    virtual	int mobaGoodsAddEffect(UID uid, int nEffectID, void * pAddContext, size_t len)
    {
        NPC_PTR pNPC = findNPC(uid);
		if ( pNPC==0 )
			return 0;

		return pNPC->mobaGoodsAddEffect(nEffectID, pAddContext, len);
    }

    // 移除效果
    virtual	void mobaGoodsRemoveEffect(UID uid, int nID)
    {
        NPC_PTR pNPC = findNPC(uid);
		if ( pNPC==0 )
			return;
		return pNPC->mobaGoodsRemoveEffect(nID);
    }

    // 实体通知主人
    virtual void entityNotifyMaster(UID uid, EntityBelongContext context)
    {
        
    }

	// 处理消息
	virtual void handleMessage( UID uid,void *pData,int nLen )
	{
        if(nLen >= sizeof(SNetMsgHead) + sizeof(SMsgEntityHead))
        {
            SNetMsgHead* head = (SNetMsgHead *)pData;
            SMsgEntityHead* entityHead = (SMsgEntityHead*)(head + 1);

            ++m_mapHandleMessageStatTable[head->byKeyModule][head->byKeyAction][entityHead->byPartID];

            // 每2分钟打印一下 handleMessage 统计信息
            if(getTickCountEx() - m_dwStartStatHandleMessageTick > 2 * 60 * 1000)
            {
                SceneLogicHelper helper(m_nSceneID);
                if(helper.m_pContainer != 0)
                {
                    EmphasisLn("scenelogic service id=" << helper.m_pContainer->get_id() << " handleMessage stat:----------------");
                }
                for(std::map<int, std::map<int, std::map<int, int>>>::iterator it = m_mapHandleMessageStatTable.begin();
                    it != m_mapHandleMessageStatTable.end(); ++it)
                {
                    for(std::map<int, std::map<int, int>>::iterator it1 = it->second.begin(); it1 != it->second.end(); ++it1)
                    {
                        for(std::map<int, int>::iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2)
                        {
							EmphasisLn("in the past 2 minutes, npcservice handleMessage stat (moudle=" << it->first << ", action=" << it1->first << ",part=" << it2->first << "), count:" << it2->second);
						}
                    }
                }

                m_mapHandleMessageStatTable.clear();
                m_dwStartStatHandleMessageTick = getTickCountEx();
            }
        }

		NPC_PTR pNPC = findNPC(uid);
		if ( pNPC==0 )
			return;

		return pNPC->handleMessage(pData,nLen);
	}

    // 群体怪物收到消息
    virtual void batchHandleMessage(UID* uidArray, DWORD dwArrayCount, BYTE byKeyModule, BYTE byPartID, BYTE byKeyAction, LPCSTR pszData = 0, int nDataLen = 0, int nOneDataLen = 0)
    {
        if (uidArray == nullptr || dwArrayCount == 0)
            return;

        // 消息偏移量
        int nMsgOffset = (nOneDataLen != nDataLen ? nOneDataLen : 0);

        obuf msg;
        UID uid = INVALID_UID;
        for (DWORD i = 0; i < dwArrayCount; ++i)
        {
            uid = *(uidArray + i);
            
            msg.clear();

            SNetMsgHead head;
            head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
            head.byDestEndPoint = MSG_ENDPOINT_SCENE;
            head.byKeyModule    = byKeyModule;
            head.byKeyAction    = byKeyAction;

            // 消息头
            SMsgEntityHead etHead;
            etHead.uidMaster= uid;
            etHead.byPartID = byPartID;

            msg << head << etHead;

            if (pszData != nullptr)
            {
                msg.push_back((pszData + i*nMsgOffset), nOneDataLen);
            }

            NPC_PTR pNPC = findNPC(uid);
            if (pNPC != 0)
            {
                pNPC->handleMessage(msg.data(), msg.size());
            }
        }
    }

    virtual void handleReplaceableMessage(UID uid, unsigned long ulKey,void * pMsg,int nLeng)
    {

    }

	/////////////////////// IEntityProperty ///////////////////////////////////////////////
	// 取得某个属性的类型
	virtual string getPropertyType( UID uid,int prop_id )
	{
		__IEntityProperty* property = NULL;
		INNER_GET_MONSTER_PROPERTY_PART(uid, property);
		if ( property==0 )
			return "";

		return property->getPropertyType(prop_id);
	}

	// 取得某个属性的标志数据(通常是序列化标志)
	virtual int getPropertyFlag( UID uid,int prop_id )
	{
		__IEntityProperty* property = NULL;
		INNER_GET_MONSTER_PROPERTY_PART(uid, property);
		if ( property==0 )
			return 0;

		return property->getPropertyFlag(prop_id);
	}

	// 获取实体属性(布尔类型）
	virtual bool getProperty_Bool( UID uid,int prop_id)
	{
		__IEntityProperty* property = NULL;
		INNER_GET_MONSTER_PROPERTY_PART(uid, property);
		if ( property==0 )
			return false;

		return property->getProperty_Bool(prop_id);
	}

	// 设置实体属性(布尔类型）
	virtual bool setProperty_Bool( UID uid,int prop_id,bool value)
	{
		__IEntityProperty* property = NULL;
		INNER_GET_MONSTER_PROPERTY_PART(uid, property);
		if ( property==0 )
			return false;

		return property->setProperty_Bool(prop_id,value);
	}

	// 获取实体属性(数字,实际内部支持用int,byte,short三种类型存)
	virtual int getProperty_Integer( UID uid,int prop_id)
	{
		__IEntityProperty* property = NULL;
		INNER_GET_MONSTER_PROPERTY_PART(uid, property);
		if ( property==0 )
			return 0;

		return property->getProperty_Integer(prop_id);
	}

	// 设置实体属性(数字,实际内部支持用int,byte,short三种类型存)
	virtual bool setProperty_Integer( UID uid,int prop_id,int value)
	{
		__IEntityProperty* property = NULL;
		INNER_GET_MONSTER_PROPERTY_PART(uid, property);
		if ( property==0 )
			return false;

		return property->setProperty_Integer(prop_id,value);
	}

	// 增加属性数值(注意内部需要控制上下限)
	virtual bool addProperty_Integer( UID uid,int prop_id,int delta)
	{
		__IEntityProperty* property = NULL;
		INNER_GET_MONSTER_PROPERTY_PART(uid, property);
		if ( property==0 )
			return false;

		return property->addProperty_Integer(prop_id,delta);
	}

	// 获取实体属性(字符串)
	virtual string getProperty_String( UID uid,int prop_id )
	{
		__IEntityProperty* property = NULL;
		INNER_GET_MONSTER_PROPERTY_PART(uid, property);
		if ( property==0 )
			return "";

		return property->getProperty_String(prop_id);
	}

	// 设置实体属性(字符串)
	virtual bool setProperty_String( UID uid,int prop_id,string value)
	{
		__IEntityProperty* property = NULL;
		INNER_GET_MONSTER_PROPERTY_PART(uid, property);
		if ( property==0 )
			return false;

		return property->setProperty_String(prop_id,value);
	}

	// 获取实体属性(自定义结构,如果属性存在返回)
	virtual string getProperty_custom( UID uid,int prop_id)
	{
		__IEntityProperty* property = NULL;
		INNER_GET_MONSTER_PROPERTY_PART(uid, property);
		if ( property==0 )
			return "";

		return property->getProperty_String(prop_id);
	}

	// 设置实体属性(自定义结构)
	virtual bool setProperty_custom( UID uid,int prop_id,string value)
	{
		NPC_PTR pNPC = findNPC(uid);
		if ( pNPC==0 )
			return false;

		__IEntityProperty* property = (__IEntityProperty*)pNPC->getEntityPart(PART_PROPERTY);
		if ( property==0 )
			return false;

		return property->setProperty_custom(prop_id,(void*)value.c_str(),value.size());
	}

	/////////////////////// IEntityAOI ///////////////////////////////////////////////
	// 取得视野半径
	virtual float getSight(UID uid)
	{
		__IEntityAOI * pAOIPart = NULL;
		INNER_GET_MONSTER_AOI_PART(uid, pAOIPart);
		if ( pAOIPart==0 )
			return 0.0f;

		return pAOIPart->getSight();
	}

	// 调整视野半径
	virtual void setSight( UID uid,float radii )
	{
		__IEntityAOI * pAOIPart = NULL;
		INNER_GET_MONSTER_AOI_PART(uid, pAOIPart);
		if ( pAOIPart==0 )
			return ;

		return pAOIPart->setSight(radii);
	}

	// 是否隐身
	virtual bool isHide(UID uid)
	{
		__IEntityAOI * pAOIPart = NULL;
		INNER_GET_MONSTER_AOI_PART(uid, pAOIPart);
		if ( pAOIPart==0 )
			return false;

		return pAOIPart->isHide();
	}

	// 设置隐身状态
	virtual void setHide( UID uid,bool state )
	{
		__IEntityAOI * pAOIPart = NULL;
		INNER_GET_MONSTER_AOI_PART(uid, pAOIPart);
		if ( pAOIPart==0 )
			return ;

		return pAOIPart->setHide(state);
	}

	/////////////////////// IEnmityPart ///////////////////////////////////////////////
	// 添加仇恨数据
	virtual void AddEnmityValue(UID uid, UID uidDes, int nVal)
	{
		__IEnmityPart * pPart = NULL;
		INNER_GET_MONSTER_ENMITY_PART(uid, pPart);
		if ( pPart==0 )
			return ;

		pPart->AddEnmityValue(uidDes, nVal);
	}
	virtual void AddBroadEnmityValue(UID uid, UID uidDes, int nVal)
	{
		__IEnmityPart * pPart = NULL;
		INNER_GET_MONSTER_ENMITY_PART(uid, pPart);
		if ( pPart==0 )
			return ;

		pPart->AddBroadEnmityValue(uidDes, nVal);
	}
	virtual void SetEnmityValue(UID uid, UID uidDes, int nVal)
	{
		__IEnmityPart * pPart = NULL;
		INNER_GET_MONSTER_ENMITY_PART(uid, pPart);
		if ( pPart==0 )
			return ;

		pPart->SetEnmityValue(uidDes, nVal);
	}
	virtual void changeState(UID uidMonster, int nState)
	{
		NPC_PTR pNPC= findNPC(uidMonster);
		if (pNPC != 0 && pNPC->changeState(nState))
		{
		}
	}
	virtual int getState(UID uidMonster)
	{
		NPC_PTR pNPC= findNPC(uidMonster);
		if (pNPC != 0)
		{
			return pNPC->getState();
		}

		return 0;
	}

	/////////////////////// IBuffPart ///////////////////////////////////////////////
    // 给群体怪物带检测指定BUFFID和BUFF类型来增加BUFF
    virtual bool BatchCheckAddBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwCheckBuff = 0, DWORD dwCheckLevel = 0, DWORD dwCheckBuffType=0, DWORD dwTime = 0, void *pBuffContext = NULL, int nContextLen = 0, bool bExistAdd =true)
    {
        if (uidArray == nullptr || dwArrayCount == 0)
            return false;

        UID uid = INVALID_UID;
        __IBuffPart * pPart = NULL;

        bool bResult = false;

        for (DWORD i = 0; i < dwArrayCount; ++i)
        {
            uid = *(uidArray + i);

            pPart = NULL;
            INNER_GET_MONSTER_BUFF_PART(uid, pPart);
            if (pPart == 0)
            {
                continue;
            }
            // 检测存在加
            if (bExistAdd)
            {
                // 不存在就不加
                if (dwCheckBuff > 0 && !pPart->IsExist(dwCheckBuff, dwCheckLevel, uidAdd) )
                    continue;
                if (dwCheckBuffType > 0 && !pPart->IsExist(dwCheckBuffType))
                    continue;
            }
            else
            {
                // 存在就不加
                if (dwCheckBuff > 0 && pPart->IsExist(dwCheckBuff, dwCheckLevel, uidAdd))
                    continue;
                if (dwCheckBuffType > 0 && pPart->IsExist(dwCheckBuffType))
                    continue;
            }

            // 增加BUFF
            pPart->Add(dwBuffID, dwBuffLevel, uidAdd, dwTime, pBuffContext, nContextLen);
            bResult = true;
        }
        return bResult;
    }

    // 给群体怪物带检测指定BUFFID和BUFF类型来移除BUFF
    virtual bool BatchCheckRemoveBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, UID uidAdd, UID uidRemove, DWORD dwCheckBuff = 0, DWORD dwCheckLevel = 0, DWORD dwCheckBuffType=0, bool bExistRemove=true)
    {
        if (uidArray == nullptr || dwArrayCount == 0)
            return false;

        UID uid = INVALID_UID;
        __IBuffPart * pPart = NULL;

        for (DWORD i = 0; i < dwArrayCount; ++i)
        {
            uid = *(uidArray + i);

            pPart = NULL;
            INNER_GET_MONSTER_BUFF_PART(uid, pPart);
            if (pPart == 0)
            {
                continue;
            }
            // 检测存在移除
            if (bExistRemove)
            {
                // 不存在就不移除
                if (dwCheckBuff > 0 && !pPart->IsExist(dwCheckBuff, dwCheckLevel, uidAdd))
                    continue;
                if (dwCheckBuffType > 0 && !pPart->IsExist(dwCheckBuffType))
                    continue;
            }
            else
            {
                // 存在就不移除
                if (dwCheckBuff > 0 && pPart->IsExist(dwCheckBuff, dwCheckLevel, uidAdd))
                    continue;
                if (dwCheckBuffType > 0 && pPart->IsExist(dwCheckBuffType))
                    continue;
            }

            // 移除BUFF
            pPart->Remove(dwBuffID, uidAdd, uidRemove);
        }
        return true;
    }


    // 给群体怪物增加BUFF
    virtual bool BatchAddBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime = 0, void *pBuffContext = NULL, int nContextLen = 0)
    {
        if (uidArray == nullptr || dwArrayCount == 0)
            return false;

        UID uid = INVALID_UID;
        for (DWORD i = 0; i < dwArrayCount; ++i)
        {
            uid = *(uidArray + i);
            AddBuff(uid, dwBuffID, dwBuffLevel, uidAdd, dwTime, pBuffContext, nContextLen);
        }

        return true;
    }

    // 给群体怪物移除BUFF
    virtual bool BatchRemoveBuff(UID* uidArray, DWORD dwArrayCount, DWORD dwBuffID, UID uidAdd, UID uidRemove)
    {
        if (uidArray == nullptr || dwArrayCount == 0)
            return false;

        UID uid = INVALID_UID;
        for (DWORD i = 0; i < dwArrayCount; ++i)
        {
            uid = *(uidArray + i);
            RemoveBuff(uid, dwBuffID, uidAdd, uidRemove);
        }

        return true;
    }

	virtual bool AddBuff(UID uid, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0)
	{
		__IBuffPart * pPart = NULL;
		INNER_GET_MONSTER_BUFF_PART(uid, pPart);
		if ( pPart==0 )
			return false;
		return pPart->Add(dwBuffID, dwBuffLevel, uidAdd, dwTime, pBuffContext, nContextLen);
	}

	virtual bool RemoveBuff(UID uid, DWORD dwBuffID, UID uidAdd, UID uidRemove)
	{
		__IBuffPart * pPart = NULL;
		INNER_GET_MONSTER_BUFF_PART(uid, pPart);
		if ( pPart==0 )
			return false;
		return pPart->Remove(dwBuffID, uidAdd, uidRemove);
	}

	virtual bool IsExistBuff(UID uid, DWORD dwBuffID, DWORD dwBuffLevel, UID uidAdd)
	{
		__IBuffPart * pPart = NULL;
		INNER_GET_MONSTER_BUFF_PART(uid, pPart);
		if ( pPart==0 )
			return false;
		return pPart->IsExist(dwBuffID, dwBuffLevel, uidAdd);
	}

    virtual bool IsExistBuffType(UID uid, DWORD dwBuffType)
    {
        __IBuffPart * pPart = NULL;
        INNER_GET_MONSTER_BUFF_PART(uid, pPart);
        if ( pPart==0 )
            return false;
        return pPart->IsExist(dwBuffType);
    }

	virtual DWORD GetBuffLevel(UID uid, DWORD dwBuffID, UID uidAdd)
	{
		__IBuffPart * pPart = NULL;
		INNER_GET_MONSTER_BUFF_PART(uid, pPart);
		if ( pPart==0 )
			return 0;
		return pPart->GetBuffLevel(dwBuffID, uidAdd);
	}

	virtual int GetLeftTime(UID uid, DWORD dwBuffID, UID uidAdd)
	{
		__IBuffPart * pPart = NULL;
		INNER_GET_MONSTER_BUFF_PART(uid, pPart);
		if ( pPart==0 )
			return -1;
		return pPart->GetLeftTime(dwBuffID, uidAdd);
	}

	virtual void SetAccumulateBuff(UID uid, DWORD dwBuffID, UID uidAdd, bool bOverlay, DWORD dwTime=0, void *pBuffContext=NULL, int nContextLen=0)
	{
		__IBuffPart * pPart = NULL;
		INNER_GET_MONSTER_BUFF_PART(uid, pPart);
		if ( pPart==0 )
			return;
		pPart->SetAccumulateBuff(dwBuffID, uidAdd, bOverlay, dwTime, pBuffContext, nContextLen);
	}

	virtual bool RemoveBuffByIndex(UID uid,DWORD dwIndex)
	{
		__IBuffPart * pPart = NULL;
		INNER_GET_MONSTER_BUFF_PART(uid, pPart);
		if ( pPart==0 )
			return false;
		return pPart->RemoveBuffByIndex(dwIndex);
	};
	/////////////////////// IMovePart ///////////////////////////////////////////////
	virtual bool Transport(UID uidMonster, Vector3 newLoc, Vector3 rotation, bool bTransport, int nSceneID, int nReason)
	{
		__IMovePart* pPart = NULL;
		NPC_PTR pNpc = findNPC(uidMonster);
		if(pNpc == NULL)
		{
			return false;
		}

		__IMovePart * pMovePart = (__IMovePart *)pNpc->getEntityPart(PART_MOVE);
		if(pMovePart == NULL)
		{
			return false;
		}

		Move3 move = pMovePart->getMoveState();
		move.vPosition = newLoc;
        if(rotation != Vector3(-1,-1,-1))
        {
            move.vRotation = rotation;
        }

		if (bTransport)
		{
			return pMovePart->stopAt( move,nSceneID,nReason );
		}
		else
		{
			pMovePart->changeMoveState(move);
		}
		return true;
	}

	virtual bool  MoveTo(UID uidMonster, Vector3 &loc)
	{
		__IMovePart* pPart = NULL;
		NPC_PTR pNpc = findNPC(uidMonster);
		if(pNpc == NULL)
		{
			return false;
		}

		__IMovePart* pMovePart =  (__IMovePart*)pNpc->getEntityPart(PART_MOVE);
		if(pMovePart != NULL)
		{
			return pMovePart->moveTo(loc);
		}

		return false;
	}

    virtual bool isInsight(UID uid, int camp)
    {
        NPC_PTR pNpc = findNPC(uid);
        if(pNpc == NULL)
        {
            return false;
        }

        __IEntityAOI* pAOIPart =  (__IEntityAOI*)pNpc->getEntityPart(PART_AOI);
        if(pAOIPart == NULL)
        {
            return false;
        }

        return pAOIPart->isInsight(camp);
    }

    virtual int getWarID(UID uid)
    {
        NPC_PTR pNpc = findNPC(uid);
        if(pNpc == NULL)
        {
            return 0;
        }

        __IWarPart* pWarPart =  (__IWarPart*)pNpc->getEntityPart(PART_WAR);
        if(pWarPart == NULL)
        {
            return 0;
        }

        return pWarPart->getWarID();
    }

	// 通知指定阵营当前野怪点野怪的生死状况,仅用于隐形实体
	virtual void msgToSpecCampWildInfo(UID uidMonster, int nCamp)
	{
		NPC_PTR pInvisibleMonster = findNPC(uidMonster);
		if(pInvisibleMonster == 0)
		{
			return;
		}

		 Monster* pMonster = (Monster*)(__IEntity*)pInvisibleMonster;
		 pMonster->msgToSpecCampWildInfo(uidMonster, nCamp);
	}

    virtual BuffTypeList getBuff(UID uid, DWORD dwBuffType)
    {
        NPC_PTR pNpc = findNPC(uid);
        if(pNpc == NULL)
        {
            return BuffTypeList();
        }

        __IBuffPart* pBuffPart =  (__IBuffPart*)pNpc->getEntityPart(PART_BUFF);
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

    /** 取得怪物数组
    @param nMonsterID   要找的怪物ID,0为所有怪物ID
    @param nMonsterCamp 要找的怪物阵营,0为所有怪物阵营
    @param pArray       找到怪物UID数组
    @param nArraySize   怪物UID数组数量,防止怪物UID数组越界
    @return 返回找到怪物数量
    */
    virtual int getMonsterArray(int nMonsterID, int nMonsterCamp, UID *pArray, int nArraySize)
    {
        if (pArray == NULL)
        {
            return 0;
        }

        int nCount = 0;
        for ( int i=0;i<MAX_NPC_COUNT;++i )
        {
			if ( m_npcMap[i]==0 )
				continue;

            NPC_PTR pNpc = m_npcMap[i];

            if (nMonsterID != 0)
            {
                // 取得怪物ID
                int nID = pNpc->getIntProperty(PROPERTY_ID);
                if (nID != nMonsterID)
                {
                    continue;
                }
            }

            if (nMonsterCamp != 0)
            {
                // 取得怪物阵营
                int nCamp = pNpc->getIntProperty(PROPERTY_CAMP);
                if (nCamp != nMonsterCamp)
                {
                    continue;
                }
            }

            if (nCount < nArraySize)
            {
                pArray[nCount++] = pNpc->getUID();
            }
            else
            {
                break;
            }
        }

        return nCount;
    }

	// 移除所有NPC
	virtual void destroyAll()
	{
        for ( int i=0;i<MAX_NPC_COUNT;++i )
        {
			if ( m_npcMap[i]==0 )
				continue;

            m_npcMap[i]->destroy();
			m_npcMap[i] = NPC_PTR(0);

            // 回收序号
            push_index(i);
        }
	}

    // 取得实体位置
    virtual void sendDynamicObstacleToMovePart(UID uid, void * pObstacleInfo, size_t nLen)
    {
        NPC_PTR pNpc = findNPC(uid);
        if(pNpc == NULL)
        {
            return;
        }

        __IDiePart* pDiePart =  (__IDiePart*)pNpc->getEntityPart(PART_DIE);
        if(pDiePart ==  NULL)
        {
            return ;
        }

        ((DiePart*)pDiePart)->receiveDynamicObstacleInfo(pObstacleInfo, nLen);
    }

    virtual bool chooseTarget(UID uid, int nTargetFilter, UID uidTarget)
    {
        NPC_PTR pNpc = findNPC(uid);
        if(pNpc == NULL)
        {
            return false;
        }

        __IPlayerKillPart* pPlayerKillPart =  (__IPlayerKillPart*)pNpc->getEntityPart(PART_PK);
        if(pPlayerKillPart == NULL)
        {
            return false;
        }

        return pPlayerKillPart->chooseTarget(nTargetFilter, uidTarget);
    }

    virtual int getPKType(UID uid)
    {
        NPC_PTR pNpc = findNPC(uid);
        if(pNpc == NULL)
        {
            return 0;
        }

        __IPlayerKillPart* pPlayerKillPart =  (__IPlayerKillPart*)pNpc->getEntityPart(PART_PK);
        if(pPlayerKillPart == NULL)
        {
            return 0;
        }

        return pPlayerKillPart->getPKType();
    }

    virtual int getPKState(UID uid)
    {
        NPC_PTR pNpc = findNPC(uid);
        if(pNpc == NULL)
        {
            return 0;
        }

        __IPlayerKillPart* pPlayerKillPart =  (__IPlayerKillPart*)pNpc->getEntityPart(PART_PK);
        if(pPlayerKillPart == NULL)
        {
            return 0;
        }

        return pPlayerKillPart->getPKState();
    }

    virtual void forceEndPK(UID uid)
    {
        NPC_PTR pNpc = findNPC(uid);
        if(pNpc == NULL)
        {
            return;
        }

        __IPlayerKillPart* pPlayerKillPart =  (__IPlayerKillPart*)pNpc->getEntityPart(PART_PK);
        if(pPlayerKillPart == NULL)
        {
            return;
        }

        pPlayerKillPart->forceEndPK();
    }
    

    virtual int getMapID(UID uid)
    {
        return m_nMapID;
    }

    // 冲锋
    virtual void shift(UID uidMonster, void *pData,int nDataLen)
    {
        NPC_PTR pNpc = findNPC(uidMonster);
        if(pNpc == NULL)
        {
            return;
        }

        __IMovePart* pMovePart = (__IMovePart*)pNpc->getEntityPart(PART_MOVE);
        if(pMovePart == NULL)
        {
            return;
        }

        pMovePart->shift(pData, nDataLen);
    }

	// 获取PK阵营
	virtual int getPKCamp(UID uid)
	{
		NPC_PTR pNpc = findNPC(uid);
		if (pNpc == NULL)
		{
			return 0;
		}

		__IPlayerKillPart * pPlayerKillPart = (__IPlayerKillPart *)pNpc->getEntityPart(PART_PK);
		if (pPlayerKillPart == NULL)
		{
			return 0;
		}

		return pPlayerKillPart->getPKCamp();
	}

	// 释放
	virtual void release()
	{
		delete this;
	}
};

