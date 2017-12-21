/*******************************************************************
** 文件名:	e:\Rocket\Server\TeamService\Src\ExternalFacade.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2008/6/5  11:58
** 版  本:	1.0
** 描  述:	外部接口外观类	
** 应  用:	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#include "StdAfx.h"
#include <time.h>
#include "TeamDef.h"
#include "ExternalFacade.h"
#include "ISocialGlobal.h"
#include "ITimerService.h"

CExternalFacade	g_ExternalFacade;

/** 启用定时器
@param   
@param   
@return  
*/ 
bool CExternalFacade::SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo)
{
	return gSocialGlobal->getTimerService()->SetTimer(timerID, interval, handler, INFINITY_CALL, debugInfo);
}

/** 销毁定时器
@param   
@param   
@return  
*/
bool CExternalFacade::KillTimer(DWORD timerID, ITimerHandler * handler)
{
	return gSocialGlobal->getTimerService()->KillTimer(timerID, handler);
}

/** 
@param   
@param   
@return  
*/
void CExternalFacade::SendDataToPerson(DWORD dwPDBID, LPCSTR pData, int nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return;
    }
    if (dwPDBID == INVALID_PDBID)
    {
        return;
    }

    SSharePersonProperty SharePerson = pShareReceiver->GetSharePerson(dwPDBID);
    if (SharePerson.dwPDBID == INVALID_PDBID)
    {
        return;
    }

    IGatewayAcceptorService* pGatewayAcceptorService = gSocialGlobal->getGatewayAcceptorService();
    if (nullptr == pGatewayAcceptorService)
    {
        ErrorLn(__FUNCTION__": failed! IGatewayAcceptorService PDBID=" << dwPDBID);
        return;
    }

	pGatewayAcceptorService->sendData( SharePerson.dwPDBID, PACKAGE_PTR(new std::string((char const *)pData, nLen)) );
}

/** 
@param   
@param   
@return  
*/
void CExternalFacade::SendDataToZoneSvr(LPCSTR pData, int nLen, CSID ServerID)
{
	ICenterConnectorService * pConnector = gSocialGlobal->getCenterConnectorService();
	if(pConnector == NULL)
	{
		return;
	}

	if(ServerID == INVALID_SERVER_ID)
	{
		ServerData * svrInfo[MAX_SERVER_COUNT];
		ulong count = pConnector->getServerList(svrInfo, MAX_SERVER_COUNT, INVALID_SERVER_ID, MSG_ENDPOINT_SCENE);
		if(count <= 0)
		{		
			return;
		}

		for(ulong i  = 0; i < count; i++)
		{
			pConnector->postMessage(svrInfo[i]->dwServerID, (void *)pData, (ushort)nLen);
		}	
	}
	else
	{
		pConnector->postMessage(ServerID, (void *)pData, (ushort)nLen);
	}
}


LPCSTR CExternalFacade::GetCurTimeString(void)
{
	static char buf[16] = {0};

	buf[0] = 0;
	time_t t = time(NULL);
	struct tm* local = localtime(&t);

	strftime(buf, 16, "%H:%M", local);

	return buf;
}

/** 
@param   
@param   
@return  
*/
bool CExternalFacade::SendActorRightTips(DWORD dwPDBID, LPCSTR pszText)
{
	/*IChatService * pChatService = g_ExternalFacade.GetChatService();
	if(pChatService != NULL)
	{
		pChatService->sendSystemTips(TipType_Operate, dwPDBID, InfoPos_ActorRight, pszText);
	}*/

	return true;
}

/** 显示提示文字
*/
void CExternalFacade::showSystemMessage(PDBID PlayerID, EMChatTipID nTipID, LPCSTR szParam)
{

    SERVICE_PTR pContainer = gSocialGlobal->getChatManager()->getChatService();
    if (pContainer == NULL)
    {
        return;
    }

    IChatService* pChatService = (IChatService*)pContainer->query_interface();
    if ( nullptr == pChatService )
    {
        ErrorLn(__FUNCTION__ << "failed! nullptr == pChatService" );
        return;
    }

    pChatService->broadcastSystemMessage(CHAT_BROADCASTTYPE_SINGLE, PlayerID, nTipID, szParam);
}

/** 
@param   
@param   
@return  
*/
bool CExternalFacade::SendActorSystemTips(DWORD dwPDBID, LPCSTR pszText)
{
	/*IChatService * pChatService = g_ExternalFacade.GetChatService();
	if(pChatService != NULL)
	{
		pChatService->sendSystemTips(TipType_Operate, dwPDBID, InfoPos_ChatWindow|InfoPos_ActorRight, pszText);
	}*/

	return true;
}

/** 
@param   
@param   
@return  
*/
bool CExternalFacade::SendDataToClient(DWORD dwPDBID, LPCSTR pData, int nLen)
{
    IShareReceiver* pShareReceiver = gSocialGlobal->getShareReceiver();
    if (pShareReceiver==NULL)
    {
        return false;
    }
    if (dwPDBID == INVALID_PDBID)
    {
        return false;
    }

    SSharePersonProperty SharePerson = pShareReceiver->GetSharePerson(dwPDBID);
    if (SharePerson.dwPDBID == INVALID_PDBID)
    {
        return false;
    }

    IGatewayAcceptorService* pGatewayAcceptorService = gSocialGlobal->getGatewayAcceptorService();
    if (nullptr == pGatewayAcceptorService)
    {
        ErrorLn(__FUNCTION__": failed! IGatewayAcceptorService PDBID=" << dwPDBID);
        return false;
    }

    pGatewayAcceptorService->sendData( SharePerson.clientID, PACKAGE_PTR(new std::string((char const *)pData, nLen)) );

	return true;
}


/** 
@param   
@param   
@return  
*/
SERVICE_PTR CExternalFacade::GetChatService(void)
{
	return gSocialGlobal->getChatManager()->getChatService();
}




/** 发送投票事件   
@param   wEventID ：事件ID
@param   bSrcType ：发送源类型
@param   dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0）
@param   pszContext : 上下文
@param   nLen : 上下文长度	  
@return  
@note     
@warning 
@retval buffer 
*/
bool CExternalFacade::FireVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	IEventEngine * pEventEngine = gSocialGlobal->getEventEngine();
	if(pEventEngine == NULL)
	{
		return false;
	}

	// 发送投票事件
	return pEventEngine->FireVote(wEventID, bSrcType, dwSrcID, pszContext, nLen);
}

/** 发送执行事件
@param   
@param   
@return  
*/
void CExternalFacade::FireExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen)
{
	IEventEngine * pEventEngine = gSocialGlobal->getEventEngine();
	if(pEventEngine == NULL)
	{
		return;
	}

	// 发送执行事件
	pEventEngine->FireExecute(wEventID, bSrcType, dwSrcID, pszContext, nLen);
}

/** 订阅投票事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@param   bSrcType ：发送源类型
@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
@param	 pDesc : 订阅者描述，用来调试程序
@return  成功返回true,失败返回false
@note     
@warning 
@retval buffer 
*/
bool CExternalFacade::Subscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc)
{
	IEventEngine * pEventEngine = gSocialGlobal->getEventEngine();
	if(pEventEngine == NULL)
	{
		return false;
	}
	return pEventEngine->Subscibe(pSink,wEventID,bSrcType,dwSrcID,pszDesc);
}

/** 订阅执行事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@param   bSrcType ：发送源类型
@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
@param	 pDesc : 订阅者描述，用来调试程序
@return  成功返回true,失败返回false
@note     
@warning 
@retval buffer 
*/
bool CExternalFacade::Subscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc)
{
	IEventEngine * pEventEngine = gSocialGlobal->getEventEngine();
	if(pEventEngine == NULL)
	{
		return false;
	}
	return pEventEngine->Subscibe(pSink,wEventID,bSrcType,dwSrcID,pszDesc);
}

/** 取消订阅投票事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@param   bSrcType ：发送源类型
@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
@param	 pDesc : 订阅者描述，用来调试程序
@return  成功返回true,失败返回false
@note     
@warning 
@retval buffer 
*/
bool CExternalFacade::UnSubscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID)
{
	IEventEngine * pEventEngine = gSocialGlobal->getEventEngine();
	if(pEventEngine == NULL)
	{
		return false;
	}
	return pEventEngine->UnSubscibe(pSink,wEventID,bSrcType,dwSrcID);
}

/** 取消订阅执行事件
@param   pSink : 回调sink
@param   wEventID : 事件ID
@param   bSrcType ：发送源类型
@param	 dwSrcID : 发送源标识（实体为UID中"序列号"部份，非实体就为0），如果为0，则表示订阅所有
@param	 pDesc : 订阅者描述，用来调试程序
@return  成功返回true,失败返回false
@note     
@warning 
@retval buffer 
*/
bool CExternalFacade::UnSubscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID)
{
	IEventEngine * pEventEngine = gSocialGlobal->getEventEngine();
	if(pEventEngine == NULL)
	{
		return false;
	}
	return pEventEngine->UnSubscibe(pSink,wEventID,bSrcType,dwSrcID);
}




/** 取得剩余时间字串
@param  char * szBuf:字串buffer
@param  int nBufSize:字串buffer长度
@param  DWORD nRemainSeconds:剩余时间(秒)
@return  
*/	

void CExternalFacade::GetRemainTimeString(char * szBuf,int nBufSize,DWORD nRemainSeconds)
{
	// 剩余时间计数
	int bNums=0,sNums=0;

	// 小于等于60秒
	if (nRemainSeconds<60)
	{
		sprintf_s(szBuf, nBufSize, "%d秒",nRemainSeconds);
	}
	else if(nRemainSeconds<3600)
	{// 小于1小时
		bNums=(int)(nRemainSeconds / 60) ;
		sNums =(int)(nRemainSeconds % 60) ;
		if (sNums==0)
		{
			sprintf_s(szBuf, nBufSize, "%d分钟",bNums );
		} 
		else
		{
			sprintf_s(szBuf, nBufSize, "%d分钟%d秒",bNums,sNums);
		}

	}
	else if (nRemainSeconds<86400)
	{ // 小于1天
		bNums=(int)(nRemainSeconds / 3600) ;
		sNums =(int)((nRemainSeconds % 3600)/60) ;	
		if (sNums==0)
		{
			sprintf_s(szBuf, nBufSize, "%d小时",bNums );
		} 
		else
		{
			sprintf_s(szBuf, nBufSize, "%d小时%d分钟",bNums,sNums);
		}
	} 
	else
	{//大于等于1天
		bNums=(int)(nRemainSeconds / 86400) ;
		sNums =(int)((nRemainSeconds % 86400)/3600) ;
		if (sNums==0)
		{
			sprintf_s(szBuf, nBufSize, "%d天",bNums );
		} 
		else
		{
			sprintf_s(szBuf, nBufSize, "%d天%d小时",bNums,sNums);
		}
	}
}



/** 取得时间字串
@param   DWORD dwTime:时间
@param   
@return  LPCSTR
*/
LPCSTR CExternalFacade::GetTimeString(DWORD dwTime)
{
	static char szTimebuf[10][32] = {0};
	static BYTE nBufIndex = 0;

	nBufIndex++;
	if (nBufIndex>=10)
	{
		nBufIndex =0;
	}

	memset(szTimebuf[nBufIndex],0,sizeof(szTimebuf[nBufIndex]));
	if (dwTime==0)
	{
		return szTimebuf[nBufIndex];
	}
	time_t t = (time_t) dwTime;
	tm local;
	memset(&local,0,sizeof(local));
	localtime_s(&local,&t);

	sprintf_s(szTimebuf[nBufIndex], sizeof(szTimebuf[nBufIndex]), "%d-%.2d-%.2d %.2d:%.2d:%.2d",
		1900+local.tm_year,local.tm_mon+1,local.tm_mday,local.tm_hour,local.tm_min,local.tm_sec);

	return szTimebuf[nBufIndex];
}

/** 取得剩余时间字串
@param  DWORD nRemainSeconds:剩余时间(秒)
@return  LPCSTR
*/	
LPCSTR CExternalFacade::GetRemainTimeString(DWORD nRemainSeconds)
{
	static char szTimebuf[10][32] = {0};
	static BYTE nBufIndex = 0;

	nBufIndex++;
	if (nBufIndex>=10)
	{
		nBufIndex =0;
	}

	memset(szTimebuf[nBufIndex],0,sizeof(szTimebuf[nBufIndex]));
	GetRemainTimeString(szTimebuf[nBufIndex],sizeof(szTimebuf[nBufIndex]),nRemainSeconds);
	return szTimebuf[nBufIndex];
}



/** 检查数据有效
@param  DWORD dwSrcNum	:要处理的数字
@param  int nAddNum		:要加减的数字
@param  int nMin		:最小数
@param  int nMax		:最大数
@return  bool true:有效,false:无效
*/	
bool CExternalFacade::CheckValidNumber(DWORD dwSrcNum,int nAddNum,int nMin,int nMax)
{
	if (dwSrcNum>0x7FFFFFFF)
	{
		return false;
	}

	int nSrcNum =(int) dwSrcNum;

	INT64 nNewNumber = (INT64)(nSrcNum + nAddNum);

	//#define  COUNTRY_MAX_DWORD		0xFFFFFFFF				//DWORD 的最大数,0xFFFFFFFF	
	//#define  COUNTRY_MAX_INT			0x7FFFFFFF				//int 的最大数,0x7FFFFFFF	

	// 查检
	if ( nNewNumber<nMin || nNewNumber > nMax )
	{
		return false;
	} 

	return true;
}

/** DWORD 加减 int 数字
@param  DWORD dwSrcNum	:要处理的数字
@param  int nAddNum		:要加减的数字
@param  int nMin		:最小数
@param  int nMax		:最大数
@return  DWORD 处理后的数字
*/	
DWORD CExternalFacade::GetDWORDAddNumber(DWORD dwSrcNum,int nAddNum,int nMin,int nMax)
{
	// 无效数
	if (dwSrcNum>0x7FFFFFFF)
	{
		return dwSrcNum;
	}

	int nSrcNum =(int) dwSrcNum;

	INT64 nNewNumber = (INT64)(nSrcNum + nAddNum);

	//#define  COUNTRY_MAX_DWORD		0xFFFFFFFF				//DWORD 的最大数,0xFFFFFFFF	
	//#define  COUNTRY_MAX_INT			0x7FFFFFFF				//int 的最大数,0x7FFFFFFF	

	// 查检
	if ( nNewNumber<nMin || nNewNumber > nMax )
	{
		return dwSrcNum;
	} 

	DWORD dwRet = (DWORD)(nSrcNum + nAddNum);

	return dwRet;

}


/** 
@param   
@param   
@return  
*/
IEventEngine * CExternalFacade::getEventEngine(void)
{
	return gSocialGlobal->getEventEngine();
}


/** 
@param   
@param   
@return  
*/
ISchemeEngine * CExternalFacade::getSchemeEngine(void)
{
	return gSocialGlobal->getSchemeEngine();
}



/** 
@param   
@param   
@return  
*/
DWORD CExternalFacade::GetZoneServerID()
{
	return 39;
}

/** 判断两个时间是否同月
	@return
	*/
bool CExternalFacade::IsSameMonth(DWORD dwTime1, DWORD dwTime2)
{
	time_t tTime1 = (time_t)dwTime1;
	time_t tTime2 = (time_t)dwTime2;

	tm tmTime1;
	memcpy(&tmTime1, localtime(&tTime1), sizeof(tmTime1));

	tm tmTime2;
	memcpy(&tmTime2, localtime(&tTime2), sizeof(tmTime2));

	if (tmTime1.tm_year == tmTime2.tm_year && tmTime1.tm_mon == tmTime2.tm_mon)
	{
		return true;
	}

	return false;
}