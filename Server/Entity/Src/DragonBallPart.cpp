/*******************************************************************
** 文件名:	DragonBallPart.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林飞
** 日  期:	3/6/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#include "stdafx.h"
#include "DragonBallPart.h"
#include "EntityHelper.h"
#include "IServiceContainer.h"
#include "IServerGlobal.h"
#include "ISceneManager.h"
#include "EventDef.h"
#include "IEntityEvent.h"
#include "INPCService.h"
#include "EntityDef.h"
#include "ServiceHelper.h"
#include "ITankService.h"
#include "IBuffServer.h"
#include "WarMessageDef.h"
#include "IPlayerRole.h"
#include "ICamp.h"
#include <math.h>
#include "IWarPart.h"
#include "IWarManager.h"
#include "IWarService.h"
#include "DiePart.h"
#include "IFreezeServer.h "
#include "IBankPart.h"
#include "Match_Manageddef.h"
#include "ExternalHelper.h"
#include "PlayerBankPart.h"
#include "OssLogDef.h"
#include "IMigrateService.h"
#include "MailHelper.h"
#include "Mail_ManagedDef.h"
#include "ICenterConnectorService.h"
#include "ChatHelper.h"
#include "MovePart.h"
using namespace  Match_ManagerDef;
using namespace Mail_ManagerDef;

//#include "LuaHelper.h"
#define  NOTLUA_CALL_LINE
////////////////////* DragonBallPart */////////////////////////////////////////////
DragonBallPart::DragonBallPart()
    :m_pMaster(NULL)
{

}

DragonBallPart::~DragonBallPart()
{

}

////////////////////* IEntityPart */////////////////////////////////////////////
short DragonBallPart::getPartID()
{
    return PART_DRAGONBALL;
}

bool DragonBallPart::onLoad( __IEntity* pEntity, int nLoginMode)
{
    if (!pEntity)
    {
        return false;
    }

    m_pMaster = pEntity;

    return true;
}

bool DragonBallPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
    if(m_pMaster == NULL)
    {
        ErrorLn(__FUNCTION__": m_pMaster == NULL");
        return false;
    }

    switch(msgId)
    {
    case CS_MSG_ENTITY_TEST:
    case CS_MSG_ENTITY_TEST_RELIVE:
        {
            if (len < sizeof(msg_entity_smart_test))
            {
                return false;
            }
            msg_entity_smart_test * pData = (msg_entity_smart_test*)data;
            if (len < (sizeof(msg_entity_smart_test)+pData->nStrLen))
            {
                return false;
            }

            char* pStr = (char*)pData+sizeof(msg_entity_smart_test);
            //	ENTITY_EMPHASISLN("DragonBallPart Receive Data: str="<< pStr);

            //解析一下字符串 暂时先只创建怪物所以把参数给解析出来
            //SupperStone_CallCustomMonster(1,2,1,2,1)
            string parseStri = pStr;
            size_t posBegin = parseStri.find_first_of("(");
            //到时可以增加这个函数合法性的检测
            string fun_name = parseStri.substr(0,posBegin);
            size_t posEnd = parseStri.find_first_of(")");
            int parmList[7]={0};
            string strparm = parseStri.substr(posBegin+1,posEnd-posBegin-1);
            strparm+=",";
            posBegin = 0;
            int i=0;
            while(true){
                posEnd = strparm.find_first_of(",");
                if (string::npos ==posEnd )
                {
                    break;
                }
                string parm1 = strparm.substr(0,posEnd-posBegin);
                parmList[i]=atoi(parm1.data());
                i++;
                strparm= strparm.substr(posEnd+1);
            }
            UID uid = m_pMaster->getUID();
            SERVICE_PTR pContainer = gServerGlobal->getSceneManager()->getScene(UID_2_SCENE(uid));
#ifdef NOTLUA_CALL_LINE

            {
                //之所以这样用本来是打算用Lua解决的。也就相当于用一个消息码
                if(fun_name=="SupperStone_CallCustomMonster")
                {
                    int mlist, mrow , mRm, mposZ;
                    ActorHelper actorHelper(m_pMaster->getUID());
                    IActorService *pActorService = actorHelper.m_ptr;
                    if ( pActorService == NULL)
                    {
                        ErrorLn(__FUNCTION__":pActorService == NULL");
                        return false;
                    }

                    if ( pContainer != 0 )
                    {
                        MonsterHelper monsterHelper(m_pMaster->getUID());
                        INPCService * pNpcService = monsterHelper.m_ptr;
                        if (pNpcService != NULL)
                        {
                            int npcID = parmList[0];
                            if ( parmList[0]<=0  )
                            {
                                npcID =1;
                            }
                            Vector3 posIm = m_pMaster->getLocation();
                            if ( parmList[1] >0  )
                            {
                                posIm.x =parmList[1];
                            }
                            if ( parmList[2] >0  )
                            {
                                posIm.y =parmList[2];
                            }
                            if ( parmList[3] >0  )
                            {
                                posIm.z =parmList[3];
                            }
                            int nCreateCount = parmList[4];
                            if(nCreateCount<=0)
                            {
                                nCreateCount =1;	
                            }

                            int nCamp = parmList[5];
                            if(nCamp<=0)
                            {
                                nCamp =0;	
                            }

                            int nLevel = parmList[6];

                            mlist =  sqrt((float)nCreateCount);
                            mrow = nCreateCount / mlist;
                            mRm = nCreateCount % mlist;
                            mposZ = posIm.z;

                            for ( int i=0;i< mlist;i++ )
                            {
                                for (int j =0; j< mrow; j++)
                                {
                                    posIm.z +=1;

                                    CreateNpcContext npcContext;
                                    npcContext.nCamp = nCamp;
                                    npcContext.nLevel = nLevel;
                                    npcContext.nWarID = pActorService->getWarID();

                                    UID uid = pNpcService->createNPC(npcID, posIm, npcContext);
                                    //	ENTITY_EMPHASISLN("DragonBallPart create Data: uid="<< uid<<" npcID:"<<npcID<< " x:"<<posIm.x<<" z:"<<posIm.z<<" y:"<<posIm.y);

                                }
                                posIm.z = mposZ;
                                posIm.x +=1;											
                            }
                            if( mRm != 0 )
                            {
                                for (int i =0; i< mRm; i++)
                                {
                                    posIm.z +=1;
                                    CreateNpcContext npcContext;
                                    npcContext.nCamp = nCamp;
                                    npcContext.nLevel = nLevel;
                                    UID uid = pNpcService->createNPC(npcID, posIm, npcContext);
                                    //ENTITY_EMPHASISLN("DragonBallPart create Data: uid="<< uid<<" npcID:"<<npcID<< " x:"<<posIm.x<<" z:"<<posIm.z<<" y:"<<posIm.y);
                                }
                            }
                        }
                    }
                }
                else if(fun_name=="SupperStone_AddExperience")
                {
                    int nExp = parmList[0];
                    addExp(uid, nExp, 0);
                    //	ENTITY_EMPHASISLN("DragonBallPart create Data: uid="<< uid<<" SetExp:"<<nExp);
                }
                else if(fun_name=="SupperStone_SetLevel")
                {
                    int nLevel = parmList[0];
                    setProperty_Integer(uid,PROPERTY_LEVEL,nLevel);
                    //	ENTITY_EMPHASISLN("DragonBallPart create Data: uid="<< uid<<" Exp:"<<getProperty_Integer(uid,PROPERTY_EXP));
                }
                else if(fun_name=="SupperStone_Retrieve")
                {
                    int InputUid = parmList[0];
                    if ( InputUid >0 )
                    {
                        uid = InputUid;
                    }
                    setProperty_Integer(uid,PROPERTY_HP,getProperty_Integer(uid,PROPERTY_MAX_HP));
                    setProperty_Integer(uid,PROPERTY_MP,getProperty_Integer(uid,PROPERTY_MAX_MP));
                    //	ENTITY_EMPHASISLN("DragonBallPart create Data: uid="<< uid<<" HpAndMp:");
                }
                else if (fun_name == "SupperStone_ChangeHeroSkin")
                {
                    int nHeroID = parmList[0];
                    int nSkinID = parmList[1];

                    if (0 == nHeroID)
                    {
                        return false;
                    }
                    if (0 == nSkinID)
                    {
                        const PersonSkinModelItem * pPersonSkinModelItem = gServerGlobal->getSchemeCenter()->getSchemePersonModel()->getPersonSkinModelItem(nHeroID, nSkinID);
                        // 不是当前英雄的皮肤时，将用默认皮肤
                        if (!pPersonSkinModelItem)
                        {
                            return false;
                        }
                        nSkinID = pPersonSkinModelItem->nDefaultSkinID;
                    }

                    __IBankPart * pBankPart = CAST_TYPE(__IBankPart*, m_pMaster->getEntityPart(PART_BANK));
                    if (pBankPart)
                    {
                        msg_entity_change_hero msgInfo;
                        msgInfo.nOptType = CHANGE_HERO_TYPE_HERO;       // 当前操作类型:0.切英雄（带皮肤ID），1.切皮肤(带英雄ID)
                        msgInfo.nChangeID = nHeroID;                    // 英雄/皮肤ID
                        msgInfo.nReserveData = nSkinID;                 // 备用数据(切英雄时，此值为皮肤ID)

                        pBankPart->onMessage(0, CS_MSG_ENTITY_CHANGE_HERO, &msgInfo, sizeof(msgInfo));
                    }
                }
                else if(fun_name=="SupperStone_CallTank")
                {
                    if ( pContainer != 0 )
                    {
                        TankHelper helper(m_pMaster->getUID());
                        ITankService * ptankService = helper.m_ptr;			
                        if (ptankService != NULL)
                        {
                            int tankID = parmList[0];
                            if ( parmList[0]<=0  )
                            {
                                tankID =1;
                            }
                            Vector3 loc = m_pMaster->getLocation();
                            if ( parmList[1] >0  )
                            {
                                loc.x =parmList[1];
                            }
                            if ( parmList[2] >0  )
                            {
                                loc.y =parmList[2];
                            }
                            if ( parmList[3] >0  )
                            {
                                loc.z =parmList[3];
                            }
                            int nCreateCount = parmList[5];
                            if(nCreateCount<=0)
                            {
                                nCreateCount =1;	
                            }

                            for ( int i=1;i<=nCreateCount;i++ )
                            {
                                if (i%2==0)
                                {
                                    loc.z +=1;
                                }
                                else
                                {
                                    loc.x +=1;
                                }
                                UID uid = ptankService->createTank(tankID, loc, NULL, 0);
                                //		ENTITY_EMPHASISLN("DragonBallPart create Data: uid="<< uid<<" tankID:"<<tankID<< " x:"<<loc.x<<" z:"<<loc.z<<" y:"<<loc.y);
                            }
                        }
                    }
                }
                else if (fun_name=="SupperStone_SetBuff")
                {
                    int targetUID   = parmList[0];
                    int buffId      = parmList[1];
                    int buffLevl    = parmList[2];
                    
                    //__IBuffPart *pBuffPart = (__IBuffPart *)m_pMaster->getEntityPart(PART_BUFF);
                    //if (pBuffPart == NULL)
                    //{
                    //    break;
                    //}
                    if(targetUID==0) targetUID = uid;
                    // 给实体添加buff
                    if ( buffLevl == 0)
                    {
                        //pBuffPart->CleanBuff(buffId,0,uid );
                        RemoveBuff(targetUID, buffId, targetUID, targetUID);
                    }
                    else
                    {
                        //pBuffPart->Add(buffId, buffLevl, uid);
                        AddBuff(targetUID, buffId, buffLevl, targetUID);
                    }

                    //	ENTITY_EMPHASISLN("DragonBallPart create Data: uid="<< uid<<" buffId:"<<buffId<< " buffLevl:"<<buffLevl);
                }
                else if(fun_name=="SupperStone_EndtheWar")
                {
                    int scenceId = parmList[0];

                    if(scenceId == 0)
                        scenceId = m_pMaster->getSceneID() ;

                    __IWarPart* pWarPart = (__IWarPart*)m_pMaster->getEntityPart(PART_WAR);
                    if(pWarPart == NULL)
                    {
                        return false;
                    }

                    int nWarID = pWarPart->getWarID();

                    // 静态地图
                    if (nWarID == -1)
                    {
                        return false;
                    }

                    SERVICE_PTR pWarContainer = gServerGlobal->getWarManager()->GetWar(nWarID);
                    // 不是role或者不在战场(在静态地图)
                    if(pWarContainer == 0)
                    {
                        return false;
                    }
                    IWarService* pWarService = (IWarService*)pWarContainer->query_interface();
                    if(pWarService == NULL)
                    {
                        return false;
                    }

                    Vector3 v = m_pMaster->getLocation();
                    int nCamp = getProperty_Integer(m_pMaster->getUID(),PROPERTY_CAMP);
                    pWarService->DragonBallEndWar(nCamp,true);
                }
                else if(fun_name=="SupperStone_Suicide")
                {
                    __IBuffPart *pBuffPart = (__IBuffPart *)m_pMaster->getEntityPart(PART_BUFF);
                    if (pBuffPart == NULL)
                    {
                        break;
                    }
                    // 给实体添加buff

                    int buffId = 1024;
                    int buffLevl = 1;

                    pBuffPart->Add(buffId, buffLevl, uid);	
                }
                else if(fun_name=="SupperStone_ShowPosition")
                {
                    msg_dragon_show_position showPosition;
                    showPosition.uUID = parmList[0];

                    if(showPosition.uUID == 0)
                    {
                        showPosition.Position = m_pMaster->getLocation();
                    }
                    else
                    {
                        showPosition.Position = getLocation( showPosition.uUID );
                    }

                    if (showPosition.uUID == 0)
                    {
                        showPosition.uUID = m_pMaster->getUID();
                    }

                    obuf service_position;
                    service_position<< showPosition;

                    obuf o;
                    SNetMsgHead head;
                    head.byDestEndPoint = MSG_ENDPOINT_CLIENT;
                    head.bySrcEndPoint  = MSG_ENDPOINT_SCENE;
                    head.byKeyModule    = MSG_MODULEID_DRAGON;  //龙珠模块
                    head.byKeyAction    = CS_MSG_ENTITY_TEST;

                    o << head;
                    o.push_back(service_position.data(), service_position.size());

                    ((__IPlayerRole*)m_pMaster)->send2Client(o.data(),o.size());

                    //IGameSceneService * pScene = (IGameSceneService*)pContainer->query_interface();

                    //if (pScene != NULL)
                    //{
                    //	pScene->broadcast_all(BROADCAST_SCENE, std::string(o.data(),o.size()));
                    //}	
                }
                else if(fun_name == "SupperStone_AddTalentPoint")
                {
                    int nAddPoint = parmList[0];
                    //addProperty_Integer(uid,PROPERTY_TALENT_POINT,nAddPoint);
                    updateTalentPoint(uid, nAddPoint, ENUM_TALENT_DRAGON);
                }
                else if(fun_name == "SupperStone_Relive")
                {

                    DiePart *pDiePart=(DiePart *)m_pMaster->getEntityPart(PART_DIE);
                    if(pDiePart==NULL)
                    {
                        return false;
                    }
                    pDiePart->OnTimer(ETIMER_RELIVE);
                }
                else if(fun_name == "SupperStone_ClearCD")
                {
                    __IFreezePart *pFreezePart=(__IFreezePart *)m_pMaster->getEntityPart(PART_FREEZE);
                    if (pFreezePart == NULL)
                    {
                        return false;
                    }
                    if(!pFreezePart->CleanAllFreeze())
                    {
                        // 全清失败
                        return false;
                    }
                }
                else if(fun_name == "SupperStone_AddEnmity")
                {

                    UID uidAddTarget = (UID)parmList[0];
                    int nAddEnmityValue = parmList[1];
                    UID uidSelf=m_pMaster->getUID();
                    if(isInvalidUID(uidSelf)||isInvalidUID(uidAddTarget)||nAddEnmityValue==0)
                    {
                        return false;
                    }
                    AddEnmityValue(uidAddTarget,uidSelf,nAddEnmityValue);
                }
                else if ( fun_name == "SupperStone_AddGold")
                {
                    __IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
                    if ( pBankPart == NULL )
                    {
                        ErrorLn(__FUNCTION__": pBankPart == NULL ");
                        return false;
                    }

                    int nType = parmList[0];
                    int nGlod = parmList[1];
                    int nTicket = 0;
                    if(nType == 8)
                    {
                        nTicket  = parmList[2]; 
                    }
                    // TODO先不支持内存增加金币和点券
					int nReason = 0;
					if (nGlod > 0)
						nReason = OSSRES_ADD_DRAGONBALL;
					else
						nReason = OSSRES_DEL_DRAGONBALL;
					((__IPlayerRole*)m_pMaster)->addMoney(nTicket, nGlod, nReason, 0, NULL);
                }
                else if( fun_name == "SupperStone_AddCard")
                {
                    __IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
                    if ( pBankPart == NULL )
                    {
                        ErrorLn(__FUNCTION__": pBankPart == NULL ");
                        return false;
                    }

                    DWORD dwCardID = parmList[0];
                    int nCardCount = parmList[1];
                    bool bResult = pBankPart->addHeroCardInfo(&dwCardID , &nCardCount);

                }else if ( fun_name == "SupperStone_AddPlayerExp")
                {
                    __IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
                    if ( pBankPart == NULL)
                    {
                        ErrorLn(__FUNCTION__": pBankPart == NULL ");
                        return false;
                    }
                    DWORD dwExp = parmList[0];

                    pBankPart->setActorBaseInfo(dwExp, ENPLAYERINFOTYPE_EXP, ENCHANGEDATAMODE_ADD);
                }else if ( fun_name == "SupperStone_AddStaticHeroExp")
                {
                    __IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
                    if ( pBankPart == NULL)
                    {
                        ErrorLn(__FUNCTION__": pBankPart == NULL ");
                        return false;
                    }
                    int nHeroID = parmList[0];
                    DWORD dwExp = parmList[1];

                    pBankPart->setHeroOneInfo(nHeroID, dwExp, ENHEROINFOTYPE_EXP, ENCHANGEDATAMODE_ADD);
                }else if (fun_name == "SupperStone_SetStarLv")
                {
                    __IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
                    if ( pBankPart == NULL)
                    {
                        ErrorLn(__FUNCTION__": pBankPart == NULL ");
                        return false;
                    }
                    int nHeroID = parmList[0];
                    int nStarLv = parmList[1];

                    pBankPart->setHeroOneInfo( nHeroID, nStarLv, ENHEROINFOTYPE_STAR, ENCHANGEDATAMODE_SET);
                }else if ( fun_name == "SupperStone_SetHeroOrPlayerLv")
                {
                    __IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
                    if ( pBankPart == NULL)
                    {
                        ErrorLn(__FUNCTION__": pBankPart == NULL ");
                        return false;
                    }
                    int nHeroID = parmList[0];
                    int nHeroLv = parmList[1];
                    int nPlayerLv = parmList[2];

                    if ( nHeroID > 0 && nHeroLv > 0 )
                    {
						nHeroLv = (nHeroLv >= 30) ? 30 : nHeroLv;	// 英雄、角色等级设置加上限
                        pBankPart->setHeroOneInfo(nHeroID, nHeroLv, ENHEROINFOTYPE_LV, ENCHANGEDATAMODE_SET);
                    }

                    if ( nPlayerLv > 0 )
                    {
						nPlayerLv = (nPlayerLv >= 120) ? 120 : nPlayerLv;
                        pBankPart->setActorBaseInfo(nPlayerLv, ENPLAYERINFOTYPE_LEVEL, ENCHANGEDATAMODE_SET);
                    }
                }
                else if ( fun_name == "SupperStone_SetHeroMatchTypeRank")
                {

                    int uPDBID = parmList[0];
                    EMMatchType eMatchType = (EMMatchType)parmList[1];
                    int RankScore = parmList[2];
                    int HideRankScore = parmList[3];
                    if(uPDBID == 0)
                    {
                        // 如果是0 就添加自己的
                        uPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
                    }

                    if(eMatchType <= MatchType_None ||  eMatchType >= MatchType_MaxID)
                    {
                        ErrorLn("PDBID ISVALID Please input right MatchType "<<MatchType_None + 1<<" - " <<MatchType_MaxID-1);
                        return false;
                    }

                    ActorHelper actorHelper(uPDBID,ACTOR_DBID);

                    IActorService *pActorService = actorHelper.m_ptr;
                    if (pActorService != NULL)
                    {
                        pActorService->addMatchTypeRank(eMatchType,RankScore, HideRankScore);
                    }
                }
                else if ( fun_name == "SupperStone_ResetMatchTypeRank")
                {
                    ISchemeCenter * pSchemeCenter = gServerGlobal->getSchemeCenter();
                    if (!pSchemeCenter)
                    {
                        return false;
                    }
                    ISchemeEntity *pSchemeEntity = pSchemeCenter->getSchemeEntity();
                    if (pSchemeEntity == NULL)
                    {
                        return false;
                    }
                    int nMatchTypeBaseScore= pSchemeEntity->getEntityXml()->nMatchTypeBaseScore;
                    int uPDBID = parmList[0];
                    EMMatchType eMatchType = (EMMatchType)parmList[1];
                    if(uPDBID == 0)
                    {
                        // 如果是0 就添加自己的
                        uPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
                    }

                    if(eMatchType <= MatchType_None ||  eMatchType >= MatchType_MaxID)
                    {
                        ErrorLn("PDBID ISVALID Please input right MatchType "<<MatchType_None + 1<<" - " <<MatchType_MaxID-1);
                        return false;
                    }

                    ActorHelper actorHelper(uPDBID,ACTOR_DBID);

                    IActorService *pActorService = actorHelper.m_ptr;
                    if (pActorService != NULL)
                    {
                        pActorService->setMatchTypeRank(eMatchType,nMatchTypeBaseScore, nMatchTypeBaseScore);
                    }
                }
                else if ( fun_name == "SupperStone_SetHeroTalentActive")
                {
                    int nHeroId = parmList[0];
                    int nTalentId = parmList[1];
                    __IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
                    if ( pBankPart == NULL)
                    {
                        ErrorLn(__FUNCTION__": pBankPart == NULL ");
                        return false;
                    }

                    ((CPlayerBankPart*)pBankPart)->addHeroTalentInfo(nHeroId,nTalentId);
                }
				else if ( fun_name == "SupperStone_SetHeroXPSkill")
				{
					int nHeroId = parmList[0];
					int nXpSkillId = parmList[1];
					__IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
					if ( pBankPart == NULL)
					{
						ErrorLn(__FUNCTION__": pBankPart == NULL ");
						return false;
					}

					if(!((CPlayerBankPart*)pBankPart)->canAddHeroXPSkill(nHeroId,nXpSkillId))
						return false;

					((CPlayerBankPart*)pBankPart)->addHeroXPSkill(nHeroId,nXpSkillId);
				}
				else if ( fun_name == "SupperStone_SetCrownGemstone")
				{
					int nGemstoneID = parmList[0];
					int nCount = parmList[1];
					int nGradeLevel = parmList[2];
					__IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
					if ( pBankPart == NULL)
						return false;
					if(!((CPlayerBankPart*)pBankPart)->canAddCrownGemstone(nGemstoneID, nGradeLevel,nCount))
						return false;

					((CPlayerBankPart*)pBankPart)->addCrownGemstone(nGemstoneID,nCount,nGradeLevel);
				}
     //           else if ( fun_name == "SupperStone_AddRuneStoreItem")
     //           {
     //               int nRuneId = parmList[0];
     //               int nRuneCount = parmList[1];
     //               __IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
     //               if ( pBankPart == NULL)
     //               {
     //                   ErrorLn(__FUNCTION__": pBankPart == NULL ");
     //                   return false;
     //               }

					//CPlayerBankPart*pPlayerBankPart = (CPlayerBankPart*)pBankPart;
					//RUNE_BUY_ERROR_TYPE eErrorTyp = RBYEY_INVALID;
					//if(!pPlayerBankPart->canAddRuneStoreInfo(nRuneId,eErrorTyp,nRuneCount))
					//{
					//	return false;
					//}

     //               pPlayerBankPart->addRuneStoreInfo(nRuneId,nRuneCount);                    
     //           }
                else if (fun_name == "SupperStone_AddChest")
                {
                    int nChestId = parmList[0];
                    int nNum = parmList[1];
                    __IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
                    if ( pBankPart == NULL)
                    {
                        ErrorLn(__FUNCTION__": pBankPart == NULL ");
                        return false;
                    }

                    pBankPart->addPrizeChest(nChestId, nNum, OSSRES_ADD_DRAGONBALL, 0, "七龙珠");
                }
                else if (fun_name == "SupperStone_AddHeroSkin")
                {
                    int nHeroId = parmList[0];
                    int nSkinId = parmList[1];
                    int nDay = parmList[2];
                    __IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
                    if ( pBankPart == NULL)
                    {
                        ErrorLn(__FUNCTION__": pBankPart == NULL ");
                        return false;
                    }

                    int nDurationTime = nDay*24*3600;
                    ((CPlayerBankPart*)pBankPart)->addHeroSkinInfo(nHeroId,nSkinId,nDurationTime);  
                }
                else if(fun_name == "SupperStone_SwitchScene")
                {
					PDBID pdbid = m_pMaster->getIntProperty(PROPERTY_ID);
					if (((__IPlayerRole*)m_pMaster)->getGameState() != GAME_STATE_IDLE)
					{
						ChatHelper helper;
						IChatSceneService* pChat = helper.m_ptr;
						if (pChat != NULL)
						{
							pChat->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, m_pMaster->getIntProperty(PROPERTY_ID), CHAT_TIP_MIGRATE_CANT_SWITCHSCENE);
						}
						return false;
					}

                    gServerGlobal->getMigrateService()->enterScene(pdbid, parmList[0], getThisGameWorldID());
                }
                else if(fun_name == "SupperStone_AcceptTask")
                {
                    ILuaService *pLuaService = NULL;
                    SceneLogicHelper helper(m_pMaster->getSceneID());
                    if (helper.m_ptr)
                    {
                        pLuaService = helper.m_ptr->getLuaService();
                    }
                    if (pLuaService == NULL)
                    {
                        return false;
                    }

                    int nTaskID = parmList[0];
                    CLuaParam params[2];
                    params[0] = m_pMaster->getIntProperty(PROPERTY_ID);
                    params[1] = nTaskID;
                    pLuaService->RunLuaFunction(fun_name.c_str(), params, _countof(params));
                }
                else if(fun_name == "SupperStone_FinishTask")
                {
                    ILuaService *pLuaService = NULL;
                    SceneLogicHelper helper(m_pMaster->getSceneID());
                    if (helper.m_ptr)
                    {
                        pLuaService = helper.m_ptr->getLuaService();
                    }
                    if (pLuaService == NULL)
                    {
                        return false;
                    }

                    int nTaskID = parmList[0];
                    CLuaParam params[2];
                    params[0] = m_pMaster->getIntProperty(PROPERTY_ID);
                    params[1] = nTaskID;
                    pLuaService->RunLuaFunction(fun_name.c_str(), params, _countof(params));
                }
                else if(fun_name == "SupperStone_DeleteTask")
                {
                    ILuaService *pLuaService = NULL;
                    SceneLogicHelper helper(m_pMaster->getSceneID());
                    if (helper.m_ptr)
                    {
                        pLuaService = helper.m_ptr->getLuaService();
                    }
                    if (pLuaService == NULL)
                    {
                        return false;
                    }

                    int nTaskID = parmList[0];
                    CLuaParam params[2];
                    params[0] = m_pMaster->getIntProperty(PROPERTY_ID);
                    params[1] = nTaskID;
                    pLuaService->RunLuaFunction(fun_name.c_str(), params, _countof(params));
                }
                else if(fun_name == "SupperStone_RemoveTaskDayAndWeekCount")
                {
                    ILuaService *pLuaService = NULL;
                    SceneLogicHelper helper(m_pMaster->getSceneID());
                    if (helper.m_ptr)
                    {
                        pLuaService = helper.m_ptr->getLuaService();
                    }
                    if (pLuaService == NULL)
                    {
                        return false;
                    }

                    int nTaskID = parmList[0];
                    CLuaParam params[2];
                    params[0] = m_pMaster->getIntProperty(PROPERTY_ID);
                    params[1] = nTaskID;
                    pLuaService->RunLuaFunction(fun_name.c_str(), params, _countof(params));
                }
                else if(fun_name == "SupperStone_SetTaskData")
                {
                    ILuaService *pLuaService = NULL;
                    SceneLogicHelper helper(m_pMaster->getSceneID());
                    if (helper.m_ptr)
                    {
                        pLuaService = helper.m_ptr->getLuaService();
                    }
                    if (pLuaService == NULL)
                    {
                        return false;
                    }

                    int nTaskDataID = parmList[0];
                    int nTaskDataValue = parmList[1];

                    CLuaParam params[3];
                    params[0] = m_pMaster->getIntProperty(PROPERTY_ID);
                    params[1] = nTaskDataID;
                    params[2] = nTaskDataValue;
                    pLuaService->RunLuaFunction(fun_name.c_str(), params, _countof(params));
                }
				else if(fun_name == "SupperStone_SendMail")
				{
					MailHelper mailHelper;
					IMailSceneService *pMailSceneService = mailHelper.m_ptr;
					if (pMailSceneService == NULL)
						return false;

					SMailSendData sendData;
					sendData.nType = emMailType_System;
					sendData.nSourceType = emMailSourceType_System;
					sendData.dwSenderPDBID = 0;
					sstrcpyn(sendData.szSenderName, a2utf8("系统"), sizeof(sendData.szSenderName));
					sstrcpyn(sendData.szTitle, a2utf8("测试"), sizeof(sendData.szTitle));
					sstrcpyn(sendData.szContext, a2utf8("系统测试"), sizeof(sendData.szContext));
					sendData.nPlusMoney = (parmList[0] > 0 ? parmList[0] : 0);
					sendData.nPlusExp = (parmList[1] > 0 ? parmList[1] : 0);
					sendData.nPlusGoodsNum = 0;
					sendData.dwMasterPDBID = m_pMaster->getIntProperty(PROPERTY_ID);

					for (int i = 0; i < 5; ++i)
					{
						if (parmList[i+2] > 0)
						{
							sendData.sGoodsList[i].dwGoodsID = parmList[i+2];
							sendData.sGoodsList[i].nCount = 1;
							++sendData.nPlusGoodsNum;
						}
					}

					pMailSceneService->sendMail(sendData, emMailFill_DragonBall, m_pMaster->getName());
				}
				else if (fun_name == "SupperStone_AddNewHero")
				{
					__IBankPart* pBankPart = (__IBankPart*)m_pMaster->getEntityPart(PART_BANK);
					if ( pBankPart == NULL)
					{
						ErrorLn(__FUNCTION__": pBankPart == NULL ");
						return false;
					}
					int nHeroID = parmList[0];

					if (nHeroID > 0)
					{
						// 增加新英雄
						if (pBankPart->canAddHero(nHeroID))
						{
							pBankPart->addNewHero(nHeroID);
						}
					}
					else
					{
						// 删除英雄（需要重新登录生效）
						nHeroID = abs(nHeroID);
						if (pBankPart->delHero(nHeroID))
						{
							g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_CUSTOMER_TIP, a2utf8("删除英雄成功，请重新登录生效"));
						}
						else
						{
							g_EHelper.showSystemMessage(m_pMaster, CHAT_TIP_CUSTOMER_TIP, a2utf8("删除英雄失败"));
						}
					}
				}

                else if (fun_name == "SupperStone_PunishActor")
                {
                    DWORD dwPDBID = m_pMaster->getIntProperty(PROPERTY_ID);
                    if ( dwPDBID == INVALID_PDBID)
                    {
                        ErrorLn(__FUNCTION__": pBankPart == NULL ");
                        return false;
                    }
                    bool bAdd = parmList[0] == 0? false: true;
                    BYTE nPunishType = parmList[1];
                    DWORD dwActorID = m_pMaster->getIntProperty(PROPERTY_ID);

                    SMsgPunishActor_SO Msgdata;
                    Msgdata.bAdd = bAdd;
                    Msgdata.byPunishType = nPunishType;
                    Msgdata.dwActorID = dwActorID;
                    Msgdata.dwFromWorldID = m_pMaster->getFromGameWorldID();
                    ICenterConnectorService* pCenterConnectorService =  gServerGlobal->getCenterConnectorService();
                    if(pCenterConnectorService)
                    {
                        // 子消息相关
                        SMsgMatchDataSubMsg subMsg;
                        subMsg.dwMsgCode = SO_MSG_MATCH_PUNISH_ACTOR;

                        // 数据打包
                        obuf256 obMsg;
                        obMsg<<subMsg<<Msgdata;

                        pCenterConnectorService->sendToSocial(MSG_MODULEID_MATCH, MSG_MATCH_SERVICE_MODULEMSG, (const char*)obMsg.data(), obMsg.size());
                    }
                    
                }
				else if (fun_name == "SupperStone_ResetLifeHero")
				{
					CPlayerBankPart* pBankPart = (CPlayerBankPart*)m_pMaster->getEntityPart(PART_BANK);
					if (pBankPart == NULL)
					{
						return false;
					}
					pBankPart->resetLifeHero();
				}
				else if (fun_name == "SupperStone_AddLifeHero")
				{
					CPlayerBankPart* pBankPart = (CPlayerBankPart*)m_pMaster->getEntityPart(PART_BANK);
					if (pBankPart == NULL)
					{
						return false;
					}
					pBankPart->addLifeHero(parmList[0], parmList[1]);
				}
				else if (fun_name == "SupperStone_ReflushTaskOptCount")
				{
					ILuaService *pLuaService = NULL;
					SceneLogicHelper helper(m_pMaster->getSceneID());
					if (helper.m_ptr)
					{
						pLuaService = helper.m_ptr->getLuaService();
					}
					if (pLuaService == NULL)
					{
						return false;
					}

					int nTaskCatchCount = parmList[0];
					int nTaskPublishCount = parmList[1];

					CLuaParam params[3];
					params[0] = m_pMaster->getIntProperty(PROPERTY_ID);
					params[1] = nTaskCatchCount;
					params[2] = nTaskPublishCount;
					pLuaService->RunLuaFunction(fun_name.c_str(), params, _countof(params));
				}
				else if	(fun_name == "SupperStone_SetGuideStep")
				{
					CPlayerBankPart* pBankPart = (CPlayerBankPart*)m_pMaster->getEntityPart(PART_BANK);
					if (pBankPart == NULL)
					{
						return false;
					}
					msg_req_set_noviceguide msg;
					msg.dwNoviceGuideType = parmList[0];

					pBankPart->onReqSetNoviceGuideData(&msg, sizeof(msg));
				}
				else if (fun_name == "SupperStone_Transport")
				{
					CMovePart* pMovePart = (CMovePart*)m_pMaster->getEntityPart(PART_MOVE);
					if (pMovePart == NULL)
					{
						return false;
					}
					Vector3 newLoc(parmList[0], parmList[1], parmList[2]);

					Move3 move = pMovePart->getMoveState();
					move.vPosition = newLoc;

					pMovePart->stopAt(move, 0, TRANSPORT_REASON_REBORN);
				}
				else if (fun_name == "SupperStone_CollectHeroGenic")
				{
					__IWarPart* pWarPart = (__IWarPart*)m_pMaster->getEntityPart(PART_WAR);
					if (pWarPart == NULL)
					{
						return false;
					}

					int nWarID = pWarPart->getWarID();

					// 静态地图
					if (nWarID == -1)
					{
						return false;
					}

					SERVICE_PTR pWarContainer = gServerGlobal->getWarManager()->GetWar(nWarID);
					// 不是role或者不在战场(在静态地图)
					if (pWarContainer == 0)
					{
						return false;
					}
					IWarService* pWarService = (IWarService*)pWarContainer->query_interface();
					if (pWarService == NULL)
					{
						return false;
					}

					int nHeroID = parmList[0];
					int nSkinID = parmList[1];
					
					if (nSkinID <= 0)
					{
						// 取默认皮肤
						const PersonSkinModelItem * pPersonSkinModelItem = gServerGlobal->getSchemeCenter()->getSchemePersonModel()->getPersonSkinModelItem(nHeroID);
						if (pPersonSkinModelItem)
						{
							nSkinID = pPersonSkinModelItem->nDefaultSkinID;
						}
					}

					if (nSkinID <= 0 || nHeroID <= 0)
						return false;

					pWarService->collectHeroGenic(m_pMaster->getUID(), nHeroID, nSkinID);
				}
            }
#else
            {
                // 执行Lua
                /////////////////////////
                SceneLogicHelper helper(m_pMaster->getSceneID());

                ILuaService * pLuaService = NULL;
                if (helper.m_ptr)
                {
                    pLuaService = helper.m_ptr->getLuaService();
                }

                if (pLuaService == NULL)
                {
                    return false;
                }

                CLuaParam params[5];
                params[0] = m_pMaster->getSceneID();
                params[1] = 1016;
                params[2] = m_pMaster->getLocation().x;
                params[3] = m_pMaster->getLocation().y;
                params[4] = m_pMaster->getLocation().z;
                pLuaService->RunLuaFunction("LuaCreateNPC", params, sizeof(params)/sizeof(CLuaParam));
                ///////////////////////////////////////////////////
                //UID uid = m_pMaster->getUID();
                //SERVICE_PTR pContainer = ActorHelper(uid).m_pContainer;
                //if (pContainer == 0)
                //{
                //	return false;
                //}
                //// 取得LUA引擎
                //ILuaEngine* pLuaEngine = pContainer->getLuaEngine();
                //if(pLuaEngine == NULL)
                //{
                //	ErrorLn("LuaEngine==NULL");
                //	return false;
                //}
                //// 执行Lua
                //CLuaParam params[5];
                //params[0] = 1;
                //params[1] = 1;
                //params[2] = 50;
                //params[3] = 18;
                //params[4] = 50;

                //pLuaEngine->RunLuaFunction("LuaCreateNPC", params, sizeof(params)/sizeof(CLuaParam));

                ///////////////////////////////////////////////////

                //CLuaParam params[2];
                //params[0] = uid;
                //params[1] = pStr;
                //pLuaEngine->RunLuaFunction("CheckZoneServerRegister",params,sizeof(params)/sizeof(CLuaParam)/*,&outParm,1*/);
            }
#endif
        }
        break;
    }
    return true;
}

__IEntity * DragonBallPart::getHost()
{
    return m_pMaster;
}

void DragonBallPart::release()
{
    m_pMaster = 0;

    delete this;
}

bool DragonBallPart::onSerialize( rkt::ibuffer & in,int flag )
{
    return true;
}

bool DragonBallPart::deSerialize( rkt::obuf & out,int flag )
{
    return true;
}

bool DragonBallPart::Parse(string linkContent)
{

    linkContent.find("(");

    return true;
}

void DragonBallPart::restore()
{

}


//////////////////////////////////////////////////////////////////////////
extern "C" __declspec(dllexport) __IDragonBallPart * CreateDragonBallPart()
{
    if(gServerGlobal && gServerGlobal->isOpenSupperStone())
    {
        return new DragonBallPart;
    }
    else
    {
        return NULL;
    }
}