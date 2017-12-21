/*******************************************************************
** 文件名:	TaskClient.cpp
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	彭政林
** 日  期:	7/12/2016
** 版  本:	1.0
** 描  述:	任务客户端
********************************************************************/

#include "stdafx.h"
#include "TaskClient.h"
#include "GameViewDef.h"
#include "TaskMessageDef.h"
#include "ExternalHelper.h"

CTaskClient *g_pTaskClient = NULL;

CTaskClient::CTaskClient(void)
{

}

CTaskClient::~CTaskClient(void)
{

}

bool CTaskClient::Create()
{
	// 注册消息分发器
	gClientGlobal->getMessageDispatch()->registerMessageHandler(MSG_MODULEID_TASK, (IMessageHandler *)this);

	// 注册显示层需要处理命令
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_DELETE_TASK_LIST, this);
    gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_FINISH_TASK, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_FRESH_TASK, this);
	
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_VIEW_TASK_BASE, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_VIEW_TASK_REPLY, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_PUBLISH_TASK, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_PRE_CATCH_TASK, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_CATCH_TASK, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_REPLY_TASK, this);
	gClientGlobal->getEntityClient()->registerGameViewEventHandler(GVIEWCMD_REFLUSH_OPT_TASK, this);
	
	
	
	return true;
}

bool CTaskClient::Release()
{
	// 反注册消息分发器
	gClientGlobal->getMessageDispatch()->unregisterMessageHandler(MSG_MODULEID_TASK, this);

	// 反注册显示层需要处理命令
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_DELETE_TASK_LIST);
    gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_FINISH_TASK);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_FRESH_TASK);
	
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_VIEW_TASK_BASE);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_VIEW_TASK_REPLY);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_PUBLISH_TASK);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_PRE_CATCH_TASK);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_CATCH_TASK);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_REPLY_TASK);
	gClientGlobal->getEntityClient()->unregisterGameViewEventHandler(GVIEWCMD_REFLUSH_OPT_TASK);


	g_pTaskClient = NULL;
	delete this;

	return true;
}

void CTaskClient::onMessage(SNetMsgHead *head, void *pData, size_t nLen)
{
	switch (head->byKeyAction)
	{
    case TASK_MSG_SHOW_TASK_LIST:
        {
            if (pData == NULL || nLen == 0)
            {
                gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_SHOW_TASK_LIST, 0, 0, 0, 0);
                break;
            }
 
            string str;
            str.append((const char *)pData, nLen);
            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_SHOW_TASK_LIST, 0, str.data(), 0, 0);
        }
        break;

    case TASK_MSG_UPDATE_TASK_LIST:
        {
            if (pData == NULL || nLen == 0)
                break;

            string str;
            str.append((const char *)pData, nLen);
            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_UPDATE_TASK_LIST, 0, str.data(), 0, 0);
        }
        break;
    case TASK_MSG_DELETE_TASK_LIST:
        {
            if (pData == NULL || nLen == 0)
                break;

            string str;
            str.append((const char *)pData, nLen);
            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_DELETE_TASK_LIST, 0, str.data(), 0, 0);
        }
        break;
    case TASK_MSG_SEND_TASK_PRIZE:
        {
            if (pData == NULL || nLen == 0)
                break;

            string str;
            str.append((const char *)pData, nLen);
            gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_SEND_TASK_PRIZE, 0, str.data(), 0, 0);
        }
        break;
	case TASK_MSG_VIEW_CONTACT_BASE_OC:
		{
			if (pData == NULL || nLen < sizeof(SContactTaskNode_OC))
				break;

			SContactTaskNode_OC * pMsg = (SContactTaskNode_OC *)pData;

			obuf ob;
			cmd_task_node_c2v cmdNode;
			cmdNode.llContactID = pMsg->llContactID;
			cmdNode.nPublishPDBID = pMsg->dwPublishPDBID;
			cmdNode.nPublishUserID = pMsg->dwPublishUserID;
			cmdNode.nPublishHeadID = pMsg->nPublishHeadID;
			cmdNode.nPublishSex = pMsg->nPublishSex;
			cmdNode.llTagID = pMsg->llTagID;
			cmdNode.nServerTime = pMsg->dwServerTime;
			cmdNode.nPublishTime = pMsg->dwPublishTime;
			sstrcpyn(cmdNode.szPublishName, pMsg->szPublishName, sizeof(cmdNode.szPublishName));
			sstrcpyn(cmdNode.szEmotionWords, pMsg->szEmotionWords, sizeof(cmdNode.szEmotionWords));
			ob.push_back(&cmdNode, sizeof(cmdNode));

			gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_VIEW_TASK_BASE, 0, NULL, ob.data(), ob.size());
		}
		break;
	case TASK_MSG_VIEW_CONTACT_REPLY_OC:
		{
			if (pData == NULL || nLen < sizeof(SContactMsgHead_OC))
				break;

			SContactMsgHead_OC * pMsg = (SContactMsgHead_OC *)pData;

			// 解析
			char * pData = (char *)(pMsg + 1);
			m_contactReplyInfo.push_back(pData, pMsg->nBufLen);
			// 所有数据下发
			if (!pMsg->bIsEnd)
			{
				return;
			}

			int *pReplyCount = (int *)m_contactReplyInfo.data();
			int nReplyCount = *pReplyCount;

			obuf obData;
			obuf ob;
			SContactTaskReplyNode_OC * pReplyNode = (SContactTaskReplyNode_OC *)(pReplyCount + 1);
			for (int i = 0;	i < nReplyCount; ++i)
			{
				cmd_task_reply_node_c2v cmdNode;
				cmdNode.llContactID = pReplyNode->llContactID;
				cmdNode.nReplyPDBID = pReplyNode->dwReplyPDBID;
				cmdNode.nReplyUserID = pReplyNode->dwReplyUserID;
				cmdNode.nRelpyHeadID = pReplyNode->nRelpyHeadID;
				cmdNode.byReplySex = pReplyNode->byReplySex;
				cmdNode.nReplyTime = pReplyNode->dwReplyTime;
				sstrcpyn(cmdNode.szReplyName, pReplyNode->szReplyName, sizeof(cmdNode.szReplyName));
				sstrcpyn(cmdNode.szReplyContent, pReplyNode->szReplyContent, sizeof(cmdNode.szReplyContent));
				ob.push_back(&cmdNode, sizeof(cmdNode));

				pReplyNode++;
			}
			obData.push_back(&nReplyCount, sizeof(int));
			obData.push_back(ob.data(), ob.size());

			// 清空buf缓存数据
			m_contactReplyInfo.clear();

			gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_VIEW_TASK_REPLY, 0, NULL, obData.data(), obData.size());
		}
		break;
	case TASK_MSG_ADD_CONTACT_TASK_SUCCESS_SC:
		{
			if (pData == NULL || nLen == 0)
				break;

			string str;
			str.append((const char *)pData, nLen);
			gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_PUBLISH_TASK, 0, str.data(), 0, 0);
		}
		break;
	case TASK_MSG_PRE_CATCH_TASK_SC:
		{
			if (pData == NULL || nLen == 0)
				break;

			string str;
			str.append((const char *)pData, nLen);
			gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_PRE_CATCH_TASK, 0, str.data(), 0, 0);
		}
		break;

	case TASK_MSG_CATCH_TASK_SC:
		{
			if (pData == NULL || nLen == 0)
				break;

			string str;
			str.append((const char *)pData, nLen);
			gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_CATCH_TASK, 0, str.data(), 0, 0);
		}
		break;
	case TASK_MSG_REFLUSH_OPT_SC:
		{
			if (pData == NULL || nLen == 0)
				break;

			string str;
			str.append((const char *)pData, nLen);
			gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_REFLUSH_OPT_TASK, 0, str.data(), 0, 0);
		}
		break;
	case TASK_MSG_ACTOR_PUBLISH_CONTACT_OC:
		{
			if (pData == NULL || nLen < sizeof(SActorPublishContact_OC))
				break;

			SActorPublishContact_OC * pMsg = (SActorPublishContact_OC *)pData;

			cmd_task_actor_publish_contact_c2v cmdNode;
			cmdNode.nActorID = pMsg->dwActorID;
			cmdNode.nCount = pMsg->nCount;
			
			obuf obContact;
			SContactTaskNode_OC * pNode = (SContactTaskNode_OC *)(pMsg + 1);
			for (int i = 0; i < cmdNode.nCount; ++i)
			{
				cmd_task_node_c2v cmdNode;
				cmdNode.llContactID = pNode->llContactID;
				cmdNode.nPublishPDBID = pNode->dwPublishPDBID;
				cmdNode.nPublishUserID = pNode->dwPublishUserID;
				cmdNode.nPublishHeadID = pNode->nPublishHeadID;
				cmdNode.nPublishSex = pNode->nPublishSex;
				cmdNode.llTagID = pNode->llTagID;
				cmdNode.nServerTime = pNode->dwServerTime;
				cmdNode.nPublishTime = pNode->dwPublishTime;
				sstrcpyn(cmdNode.szPublishName, pNode->szPublishName, sizeof(cmdNode.szPublishName));
				sstrcpyn(cmdNode.szEmotionWords, pNode->szEmotionWords, sizeof(cmdNode.szEmotionWords));
				obContact.push_back(&cmdNode, sizeof(cmdNode));
				pNode++;
			}

			obuf obData;
			obData.push_back(&cmdNode, sizeof(cmdNode));
			obData.push_back(obContact.data(), obContact.size());
			gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_ACTOR_PUBLISH_CONTACT, 0, NULL, obData.data(), obData.size());
		}
		break;
	case TASK_MSG_VIEW_CONTACT_DELETE_OC:
		{
			if (pData == NULL || nLen < sizeof(SContactBaseDelete_OC))
				break;

			SContactBaseDelete_OC * pMsg = (SContactBaseDelete_OC *)pData;
			gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_VIEW_TASK_DELETE, 0, NULL, &(pMsg->llContactID), sizeof(pMsg->llContactID));
		}
		break;
	default:
		break;
	}
}

bool CTaskClient::onViewEvent(int eventID, int nParam, const char *strParam, void *ptrParam)
{
	switch ( eventID )
	{
		// 删除任务
    case GVIEWCMD_DELETE_TASK_LIST:
        {
            if (ptrParam == NULL)
                break;

            gamelogic_delete_task_list *pData = (gamelogic_delete_task_list *)ptrParam;

            STaskDeleteList_CS taskDeleteList;
            taskDeleteList.nTaskID = pData->nTaskID;
            g_EHelper.sendMessageToScene(MSG_MODULEID_TASK, TASK_MSG_DELETE_TASK_LIST, &taskDeleteList, sizeof(taskDeleteList));
        }
        break;
		// 请求完成任务
    case GVIEWCMD_FINISH_TASK:
        {
            if (ptrParam == NULL)
                break;

            gamelogic_finish_task *pData = (gamelogic_finish_task *)ptrParam;

            STaskFinishList_CS taskFinish;
            taskFinish.nTaskID = pData->nTaskID;
            g_EHelper.sendMessageToScene(MSG_MODULEID_TASK, TASK_MSG_FINISH_TASK, &taskFinish, sizeof(taskFinish));
        }
        break;
		// 刷新首胜
	case GVIEWCMD_FRESH_TASK:
		{
			if (ptrParam == NULL)
				break;
			
			gamelogic_fresh_task *pData = (gamelogic_fresh_task *)ptrParam;

			STaskFresh_CS freshTask;
			freshTask.nTaskType = pData->nTaskType;
			g_EHelper.sendMessageToScene(MSG_MODULEID_TASK, TASK_MSG_FRESH_TASK, &freshTask, sizeof(freshTask));
		}
		break;
		// 查看交互任务基础
	case GVIEWCMD_VIEW_TASK_BASE:
		{
			if (ptrParam == NULL)
				break;

			cmd_task_view_base_v2c *pData = (cmd_task_view_base_v2c *)ptrParam;

			SContactViewBase_CO viewBase;
			viewBase.llContactID = pData->llContactID;
			g_EHelper.sendMessageToSocial(MSG_MODULEID_TASK, TASK_MSG_VIEW_CONTACT_BASE_C0, &viewBase, sizeof(viewBase));
		}
		break;
		// 查看交互任务回复
	case GVIEWCMD_VIEW_TASK_REPLY:
		{
			if (ptrParam == NULL)
				break;

			cmd_task_view_reply_v2c *pData = (cmd_task_view_reply_v2c *)ptrParam;

			SContactViewReply_CO viewTask;
			viewTask.nTaskCount = pData->nTaskCount;
			memcpy(viewTask.arrContactId, pData->arrContactId, sizeof(viewTask.arrContactId));
			memcpy(viewTask.arrReplyCount, pData->arrRelpyCount, sizeof(viewTask.arrReplyCount));
			g_EHelper.sendMessageToSocial(MSG_MODULEID_TASK, TASK_MSG_VIEW_CONTACT_REPLY_C0, &viewTask, sizeof(viewTask));
		}
		break;
		// 发布任务
	case GVIEWCMD_PUBLISH_TASK:
		{
			if (ptrParam == NULL)
				break;

			cmd_task_publish_v2c *pData = (cmd_task_publish_v2c *)ptrParam;

			SContactTaskAdd_CS addTask;
			addTask.bySubType = (BYTE)pData->nSubType;
			addTask.bySpecificType = (BYTE)pData->nSpecificType;
			addTask.llTagID = pData->llTagID;
			sstrcpyn(addTask.szEmotionWords, pData->szEmotionWords, sizeof(addTask.szEmotionWords));
			g_EHelper.sendMessageToScene(MSG_MODULEID_TASK, TASK_MSG_ADD_CONTACT_TASK_CS, &addTask, sizeof(addTask));
		}
		break;
		// 预获取任务
	case GVIEWCMD_PRE_CATCH_TASK:
		{
			if (ptrParam == NULL)
				break;

			cmd_task_pre_catch_v2c *pData = (cmd_task_pre_catch_v2c *)ptrParam;

			STaskPreCatch_CS preCatch;
			preCatch.nCount = pData->nCount;
			memcpy(preCatch.arrContactId, pData->arrContactId, sizeof(preCatch.arrContactId));
			g_EHelper.sendMessageToScene(MSG_MODULEID_TASK, TASK_MSG_PRE_CATCH_TASK_CS, &preCatch, sizeof(preCatch));
		}
		break;
		// 获取任务
	case GVIEWCMD_CATCH_TASK:
		{
			if (ptrParam == NULL)
				break;

			cmd_task_catch_v2c *pData = (cmd_task_catch_v2c *)ptrParam;

			STaskCatch_CS catchTask;
			catchTask.nTaskID = pData->nTaskID;
			catchTask.llContactID = pData->llContactID;
			catchTask.nTaskType = pData->nTaskType;
			catchTask.nTaskSubType = pData->nTaskSubType;
			catchTask.nTaskSpecificType = pData->nTaskSpecificType;
			catchTask.nPublisherDBID = pData->nPublisherDBID;
			g_EHelper.sendMessageToScene(MSG_MODULEID_TASK, TASK_MSG_CATCH_TASK_CS, &catchTask, sizeof(catchTask));
		}
		break;
		// 发表回复
	case GVIEWCMD_REPLY_TASK:
		{
			if (ptrParam == NULL)
				break;

			cmd_task_add_reply_v2c *pData = (cmd_task_add_reply_v2c *)ptrParam;

			SContactReplyAdd_CO replyData;
			replyData.llContactID = pData->llContactID;
			replyData.nCurReplyCount = pData->nCurReplyCount;
			sstrcpyn(replyData.szContent, pData->szReplyContent, sizeof(replyData.szContent));
			g_EHelper.sendMessageToSocial(MSG_MODULEID_TASK, TASK_MSG_ADD_CONTACT_REPLY_CO, &replyData, sizeof(replyData));
		}
		break;
	case GVIEWCMD_REFLUSH_OPT_TASK:
		{
			g_EHelper.sendMessageToScene(MSG_MODULEID_TASK, TASK_MSG_REFLUSH_OPT_CS, NULL, 0);
		}
		break;
	default:
		break;
	}
	return true;
}


API_EXPORT ITaskClient *CreateTaskClient(void)
{
	// 如果创建了，直接返回
	if (g_pTaskClient != NULL)
	{
		return g_pTaskClient;
	}

	// 实例化任务客户端
	CTaskClient *pTaskClient = new CTaskClient();
	if (pTaskClient == NULL || !pTaskClient->Create())
	{
		SAFE_RELEASE(pTaskClient);
		return NULL;
	}

	g_pTaskClient = pTaskClient;
	return g_pTaskClient;
}