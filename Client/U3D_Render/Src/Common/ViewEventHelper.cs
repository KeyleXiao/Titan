/*******************************************************************
** 文件名:	ViewEventHelper.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2016/3/31
** 版  本:	1.0
** 描  述:	

		
********************************************************************/
using System;
using System.Runtime;
using UnityEngine;


public class ViewEventHelper : Singleton<ViewEventHelper>
{
    /// <summary>
    /// 显示层发送View命令到逻辑层
    /// </summary>
    /// <param name="event_id"></param>
    /// <param name="nParam">附加参数（整数）</param>
    /// <param name="strParam">附加参数（字符串）</param>
    public void SendCommand(GameLogic.GameLogicDef event_id, int nParam = 0, string strParam = "")
    {
        GameLogicAPI.onGameViewEvent((int)event_id, nParam, strParam, IntPtr.Zero, 0);
    }

    /// <summary>
    /// 显示层发送View命令到逻辑层
    /// </summary>
    /// <param name="event_id"></param>
    /// <param name="nParam">附加参数（整数）</param>
    /// <param name="strParam">附加参数（字符串）</param>
    /// <param name="ptrParam">附加参数（结构类型）</param>
    /// <param name="len">附加参数（结构类型字节数）</param>
    public void SendCommand(GameLogic.GameLogicDef event_id, int nParam, string strParam, IntPtr ptrParam, int len)
    {
        GameLogicAPI.onGameViewEvent((int)event_id, nParam, strParam, ptrParam, len);
    }

    /// <summary>
    /// 显示层发送View命令到逻辑层
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="event_id"></param>
    /// <param name="data"></param>
    /// <example>
    /// gameview_create_player data = new gameview_create_player();
    /// ViewEventHelper.Instance.SendCommand<gameview_create_player>(EntityLogicDef.GVIEWCMD_USER_CREATE_PLAYER, ref data);
    /// </example>
    public void SendCommand<T>(GameLogic.GameLogicDef event_id, ref T data) where T : struct
    {
        IntPtrHelper helper = new IntPtrHelper();
        int data_len = System.Runtime.InteropServices.Marshal.SizeOf(data);

        IntPtr ptr = helper.toPtr<T>(ref data);

        GameLogicAPI.onGameViewEvent((int)event_id, data_len, "", ptr, data_len);
    }  
       
}