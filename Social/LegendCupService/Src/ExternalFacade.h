/*******************************************************************
** 文件名:	e:\Rocket\Server\TeamService\Src\ExternalFacade.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2016/6/5  11:58
** 版  本:	1.0
** 描  述:	外部接口外观类	
** 应  用:	

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "ICenterServerProtocol.h"
#include "IShareServer.h"
#include "IGatewayAcceptorService.h"
#include "ICenterConnectorService.h"
#include "IChatManager.h"
#include "IChatService.h"

using namespace CenterServer;

// 申明
class CTeam;

// 外部接口外观类
class CExternalFacade
{
public:
	/** 启用定时器
	@param   
	@param   
	@return  
	*/ 
	virtual bool			SetTimer(DWORD timerID, DWORD interval, ITimerHandler * handler, LPCSTR debugInfo);

	/** 销毁定时器
	@param   
	@param   
	@return  
	*/
	virtual bool			KillTimer(DWORD timerID, ITimerHandler * handler);



	/** 
	@param   
	@param   
	@return  
	*/
	virtual void			SendDataToZoneSvr(LPCSTR pData, int nLen, CSID ServerID = INVALID_SERVER_ID);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual LPCSTR			GetCurTimeString(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual bool			SendActorRightTips(DWORD dwPDBID, LPCSTR pszText);


    /** 
	@param   
	@param   
	@return  
	*/
    virtual void            showSystemMessage(PDBID PlayerID, EMChatTipID nTipID, LPCSTR szParam);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual bool			SendActorSystemTips(DWORD dwPDBID, LPCSTR pszText);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual bool			SendDataToClient(DWORD dwPDBID, LPCSTR pData, int nLen);

    /** 
	@param   
	@param   
	@return  
	*/
    SERVICE_PTR             GetChatService(void);


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
	virtual bool		FireVote(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);
	/** 发送执行事件
	@param   
	@param   
	@return  
	*/
	virtual void		FireExecute(WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszContext, int nLen);

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
	virtual bool		Subscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc);

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
	virtual bool		Subscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID, LPCSTR pszDesc);

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
	virtual bool		UnSubscibe(IEventVoteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID);

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
	virtual bool		UnSubscibe(IEventExecuteSink * pSink, WORD wEventID, BYTE bSrcType, DWORD dwSrcID);
	/** 取得剩余时间字串
	@param  char * szBuf:字串buffer
	@param  int nBufSize:字串buffer长度
	@param  DWORD nRemainSeconds:剩余时间(秒)
	@return  
	*/	

	virtual	void		GetRemainTimeString(char * szBuf,int nBufSize,DWORD nRemainSeconds);

	/** 取得时间字串
	@param   DWORD dwTime:时间
	@param   
	@return  LPCSTR
	*/
	virtual LPCSTR		GetTimeString(DWORD dwTime);

	/** 取得剩余时间字串
	@param  DWORD nRemainSeconds:剩余时间(秒)
	@return  LPCSTR
	*/	
	virtual	LPCSTR		GetRemainTimeString(DWORD nRemainSeconds);

	/** 检查数据有效
	@param  DWORD dwSrcNum	:要处理的数字
	@param  int nAddNum		:要加减的数字
	@param  int nMin		:最小数
	@param  int nMax		:最大数
	@return  bool true:有效,false:无效
	*/	
	virtual	bool			CheckValidNumber(DWORD dwSrcNum,int nAddNum,int nMin,int nMax);

	/** DWORD 加减 int 数字
	@param  DWORD dwSrcNum	:要处理的数字
	@param  int nAddNum		:要加减的数字
	@param  int nMin		:最小数
	@param  int nMax		:最大数
	@return  DWORD 处理后的数字
	*/	
	virtual	DWORD			GetDWORDAddNumber(DWORD dwSrcNum,int nAddNum,int nMin,int nMax);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual IEventEngine * getEventEngine(void);


	/** 
	@param   
	@param   
	@return  
	*/
	virtual ISchemeEngine * getSchemeEngine(void);


	/** 
	@param   
	@param   
	@return  
	*/
	virtual DWORD GetZoneServerID();

	/** 判断两个时间是否同月
	@return
	*/
	virtual bool IsSameMonth(DWORD dwTime1, DWORD dwTime2);

private:
	/** 
	@param   
	@param   
	@return  
	*/
	virtual void			SendDataToPerson(DWORD dwPDBID, LPCSTR pData, int nLen);

};
extern CExternalFacade		g_ExternalFacade;