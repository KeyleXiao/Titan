/******************************************************************
** 文件名:	GatewayClient.h
** 版  权:	(C)  深圳冰川网络技术有限公司
** 创建人:	CHENTAO
** 日  期:	2009-09-18
** 版  本:	1.0
** 描  述:	网关客户端
** 应  用:  
******************************************************************/

#ifndef __COMMAND_ATTACK_H__
#define __COMMAND_ATTACK_H__

#include "Singleton.h"
#include "TimerHandler.h"
#include "PingReplyer.h"
#include "NetMessageDef.h"
#include "net/net.h"
#include "GameAPI.h"

using namespace Gateway;


///////////////////////////////////////////////////////////////////////////////////////////////
/**
@name    : 负责与网关进行协议通讯
@note    : 为了支持P2P广播,外网客户端需要监听一个UDP端口
*/
///////////////////////////////////////////////////////////////////////////////////////////////


class GatewayClient : public IMessageHandler,public Singleton<GatewayClient>
{
private:
    DWORD g_dwUCode; //add by lgf 10.04.10
	bool m_bGateWayClose;       // 网关服通知已关闭
	bool m_bTPlayerNotconn;

public:
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Start()
	{
        IMessageDispatch * pMessageDispatch  = gClientGlobal->getMessageDispatch();
        if(pMessageDispatch) {
            pMessageDispatch->registerMessageHandler(MSG_MODULEID_GATEWAY, this);
        }
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	void Stop()
	{
        IMessageDispatch * pMessageDispatch  = gClientGlobal->getMessageDispatch();
        if(pMessageDispatch) {
            pMessageDispatch->unregisterMessageHandler(MSG_MODULEID_GATEWAY,this);	
        }
	}

    void ResetUCode()
    {
        g_dwUCode = 0;
    }

    // 判断网关服是否不能连接
    bool IsGatewayClosed()
    {
        return m_bGateWayClose;
    }

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	virtual void onMessage( SNetMsgHead* head, void* data, size_t len)
	{
		switch(head->byKeyAction)
		{
            case MSG_GATEWAY_HANDSHAKE:         // 网关通知客户端发握手消息
                {
                    SendHandshake();
                }
                break;
			case MSG_GATEWAY_UCODE:             // 网关服务器发送唯一编码给客户端
                {
				    g_dwUCode	= ((SMsgGatewayUCode*)data)->dwUCode;
                }
				break;
            case MSG_GATEWAY_CONNECTSUCCESS:    // 续玩成功
                {
                    TraceLn("reconnect gateway successfule.");
                }
                break;
			case MSG_GATEWAY_CLOSE:             // 网关服务器关闭了，通知客户端不要重连
                {
				    m_bGateWayClose = ((SMsgGatewayClose*)data)->byteClose != 0;
                }
				break;
			case MSG_CLIENT_TPLAYERNOTCONN:
				m_bTPlayerNotconn = true;
				break;
            case MSG_COMMON_MESSAGEBOX:
                {
                    // 给显示层发消息 显示消息框
                    SMsgCommonMessageBox_SC* pBox = (SMsgCommonMessageBox_SC*)data;
                    char* szDesc = (char*)(pBox + 1);
                    cmd_show_message_box cmd;
                    cmd._type = 8;
                    //strncpy(cmd.caption, "message box", sizeof(cmd.caption));
                    ssprintf_s(cmd.caption, sizeof(cmd.caption), "%s", a2utf8("提示")); 
                    ssprintf_s(cmd.text, sizeof(cmd.text), "%s", a2utf8(szDesc)); 

                    if(pBox->nMsgBoxType == EMsgBox_Type_LoginKickout)
                    {
                        strncpy(cmd.okCallBack, "back2LoginState", sizeof(cmd.okCallBack));
                    }
                    if(pBox->nMsgBoxType == EMsgBox_Type_AskIfNeedForceLogin)
                    {
                        strncpy(cmd.okCallBack, "forceLogin", sizeof(cmd.okCallBack));
                    }
                    gClientGlobal->getRenderView()->sendControllerCommand( GVIEWCMD_SHOW_MESSAGEBOX, 0, 0, &cmd, sizeof(cmd));
                }
                break;
			//end add
			default:
				PingReplyer::getInstance().onMessage( head,data,len );
				break;
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	void SendHandshake()
	{
        if (gClientGlobal->getNetConnection()->GetNetworkProtocol() == rkt::PROTOCOL_TCP)
        {
            EmphasisLn("tcp connect server successful.");
        }
        else
        {
            EmphasisLn("udp connect server successful.");
        }

		Ini ini("config.ini");
		int isNated = ini.getInt("Network","is_nated",255);

		obuf ob;

		SMsgGatewayHandshake data;
		data.bIsnated     = isNated!=0;
		data.nUDPPort     = 0;
		sstrcpyn(data.szMacAddress,GetMacAddress(),sizeof(data.szMacAddress));	//add by lgf 10.04.09
		data.dwUCode		= g_dwUCode;

		SGameMsgHead msgHead;
		msgHead.SrcEndPoint = MSG_ENDPOINT_CLIENT;
		msgHead.DestEndPoint = MSG_ENDPOINT_GATEWAY;
		msgHead.byKeyModule = MSG_MODULEID_GATEWAY;
		msgHead.byKeyAction = MSG_GATEWAY_HANDSHAKE;

		ob << msgHead << data;

        net_senddata( ob.data(), ob.size() );
        TraceLn("send handshake to gateway server....");
	}

	GatewayClient() : m_bTPlayerNotconn(false),g_dwUCode(0),m_bGateWayClose(false)
	{
	}
};

#endif//__COMMAND_ATTACK_H__