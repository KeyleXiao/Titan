/*******************************************************************
** 文件名:	DragonBallPart.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林飞
** 日  期:	3/10/2015
** 版  本:	1.0
** 描  述:	

********************************************************************/

#include "stdafx.h"
#include "EventDef.h"
#include "IClientGlobal.h"
#include "buffer.h"
#include "NetMessageDef.h"
#include "IClientEntity.h"
#include "DragonBallPart.h"
#include "IClientGlobal.h"
#include "IMessageDispatch.h"
#include "WarDef.h"
#include "WarMessageDef.h"
#include "IClanClient.h"
#include "IKinClient.h"
#include "ISpell.h"
#include "IMentorshipClient.h"

////////////////////DragonBallPart/////////////////////////////////////////////
DragonBallPart::DragonBallPart()
{
	// 实体对象
	m_pMaster = 0;
}

DragonBallPart::~DragonBallPart()
{

}

////////////////////DragonBallPart/////////////////////////////////////////////

bool DragonBallPart::onLoad( IEntity* pEntity)
{
	m_pMaster = pEntity;
   
	gClientGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_DRAGON, this);
	
	return true;
}

void DragonBallPart::release()
{
	gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_DRAGON,this);
	delete this;
}

short DragonBallPart::getPartID()
{
	return PART_DRAGONBALL;
}

IEntity * DragonBallPart::getHost()
{
	return m_pMaster;
}

bool DragonBallPart::onMessage(void * pEntityHead, int msgId, void * data, size_t len)
{
	return true;
}

bool DragonBallPart::onWarMessageID( int msgId,void * data,int32 len )
{
	switch( msgId )
	{
	case CS_MSG_ENTITY_TEST:
		{
			OnMessageShowPoistion(data, len);
		}
		break;
	default:
		{
			ErrorLn("state have a message not handle, message="<< msgId);
			return false;
		}
		
	}

	return true;
}

void DragonBallPart::onMessage( SNetMsgHead* head, void* data, size_t len )
{
	if (head->byKeyModule == MSG_MODULEID_DRAGON)
	{// 大厅服消息
		onWarMessageID(head->byKeyAction, data, len);
		return;
	}
}

bool DragonBallPart::onCommand( int cmdid,void * data,size_t len )
{
	switch( cmdid )
	{
	case ENTITY_CMD_DRAGONBALL:						
		{
			char * pStr = (char*)data;
			int nLen = strlen(pStr);
			++nLen;
			obuf ob;
			msg_entity_smart_test sendData;
			sendData.nStrLen = nLen;
			ob.push_back(&sendData, sizeof(sendData));
			ob.push_back(pStr,nLen);

            //客户端直接给社会服发消息解析
            //解析一下字符串 暂时先只创建怪物所以把参数给解析出来
            //SupperStone_CallCustomMonster(1,2,1,2,1)
            string parseStri = pStr;
            size_t posBegin = parseStri.find_first_of("(");
            //到时可以增加这个函数合法性的检测
            string fun_name = parseStri.substr(0,posBegin);
            size_t posEnd = parseStri.find_first_of(")");
            string parmList[7]={""};
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
                parmList[i]=parm1.data();
                i++;
                strparm= strparm.substr(posEnd+1);
            }
            //社会服处理
            if (strcmp(fun_name.data(),"SupperStone_CreateKin") == 0)
            {
                IKinClient *pKinClient = gClientGlobal->getKinClient();
                pKinClient->dragonBallCreateKin(parmList[0].data());
                return true;
            }
            else if (strcmp(fun_name.data(),"SupperStone_JoinKin") == 0)
            {
                IKinClient *pKinClient = gClientGlobal->getKinClient();
                pKinClient->dragonBallJoinKin(atoi(parmList[0].data()),"");

                return true;
            }
            else if (strcmp(fun_name.data(),"SupperStone_QuitKin") == 0)
            {
                IKinClient *pKinClient = gClientGlobal->getKinClient();
                pKinClient->dragonBallQuitKin();
                return true;
            }
            else if (strcmp(fun_name.data(),"SupperStone_DismissKin") == 0)
            {
                IKinClient *pKinClient = gClientGlobal->getKinClient();
                pKinClient->dragonBallDissmissKin();
                return true;
            }
            // 场景服处理相关
			if(strcmp(pStr,"SupperStone_Relive()")==0)
			{
				SendMessageToEntityScene(m_pMaster->getUID(), PART_DRAGONBALL, CS_MSG_ENTITY_TEST_RELIVE, ob.data(), ob.size());
			}
			else
			{
				SendMessageToEntityScene(m_pMaster->getUID(), PART_DRAGONBALL, CS_MSG_ENTITY_TEST, ob.data(), ob.size());
			}
			
			return true;
		}
	default:
		return false;
	}
}

void DragonBallPart::OnMessageShowPoistion(void *pData, size_t nLen)
{


	//校验数据大小
	if (pData == NULL  || nLen != sizeof( msg_dragon_show_position ) )
	{
		return;
	}
	msg_dragon_show_position *mposition = (msg_dragon_show_position*) pData;
	cmd_pos service_position;
	int mdata;
	service_position.uUID = mposition->uUID;
	mdata = (int) ((mposition->Position.x)*100);
	service_position.fPosition_x = ((float) mdata)/100;;
	mdata = (int) ((mposition->Position.y)*100);
	service_position.fPosition_y = ((float) mdata)/100;;
	mdata = (int) ((mposition->Position.z)*100);
	service_position.fPosition_z = ((float) mdata)/100;;


	if (m_pMaster == NULL)
	{
		return;
	}
	// 通告显示层更新
	m_pMaster->sendCommandToEntityView(ENTITY_TOVIEW_DRAGON_SHOW_POSITION,sizeof(service_position),0,&service_position, sizeof(cmd_pos));
}
////////////////////ISerializable/////////////////////////////////////////////
bool DragonBallPart::onSerialize( UID uid, rkt::ibuffer & in,int flag )
{
	return true;
}

bool DragonBallPart::deSerialize( rkt::obuf & out,int flag )
{
	return true;
}

void DragonBallPart::restore()
{

}
