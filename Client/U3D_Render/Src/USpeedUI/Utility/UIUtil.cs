/*******************************************************************
** 文件名:	UExtensions.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-3-24
** 版  本:	1.0
** 描  述:	扩展类的方法，目前主要用于UI
		
********************************************************************/
using System;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;
using GameLogic;

namespace USpeedUI
{
    public class UIUtil
    {
        // 发送聊天消息
        // @param isDetectTextRepeat: 是否做相似度检查
        public static void SendChatMessage(EMChatChannelType channel, string msg, bool isDetectTextRepeat = false)
        {
            if (channel == EMChatChannelType.CHAT_CHANNEL_WORLD || channel == EMChatChannelType.CHAT_CHANNEL_ZONE)
            {
                isDetectTextRepeat = true;
            }

            gameview_send_chat_message msgInfo = new gameview_send_chat_message();
            msgInfo.channel = (int)channel;
            msgInfo.message = msg;
            msgInfo.isDetectTextRepeat = isDetectTextRepeat;

            ViewEventHelper.Instance.SendCommand<gameview_send_chat_message>(GameLogicDef.GVIEWCMD_SEND_CHAT_MESSAGE, ref msgInfo);
        }

        // 发送聊听消息
        // @param isDetectTextRepeat: 是否做相似度检查
        public static void SendChatMessage(EMChatChannelType channel, string msg, string target, uint pdbid, bool isDetectTextRepeat = false)
        {
            if (channel == EMChatChannelType.CHAT_CHANNEL_WORLD || channel == EMChatChannelType.CHAT_CHANNEL_ZONE)
            {
                isDetectTextRepeat = true;
            }

            gameview_send_chat_message msgInfo = new gameview_send_chat_message();
            msgInfo.channel = (int)channel;
            msgInfo.message = msg;
            msgInfo.targetName = target;
            msgInfo.targetPdbid = pdbid;
            msgInfo.isDetectTextRepeat = isDetectTextRepeat;

            ViewEventHelper.Instance.SendCommand<gameview_send_chat_message>(GameLogicDef.GVIEWCMD_SEND_CHAT_MESSAGE, ref msgInfo);
        }

        // 显示系统消息
        public static void ShowSystemMessage(EMChatTipID tipID, string param = "")
        {
            gameview_show_system_message msgInfo = new gameview_show_system_message();
            msgInfo.nIndexId = (int)tipID;
            msgInfo.param = param;

            ViewEventHelper.Instance.SendCommand<gameview_show_system_message>(GameLogicDef.GVIEWCMD_SHOW_SYSTEM_MESSAGE, ref msgInfo);
        }

        // 显示喇叭
        public static void ShowLoudspeaker(EMChatTipID tipID, string param = "")
        {
            gameview_show_loudspeaker msgInfo = new gameview_show_loudspeaker();
            msgInfo.nIndexId = (int)tipID;
            msgInfo.param = param;

            ViewEventHelper.Instance.SendCommand<gameview_show_loudspeaker>(GameLogicDef.GVIEWCMD_SHOW_LOUDSPEAKER, ref msgInfo);
        }

    }
}

