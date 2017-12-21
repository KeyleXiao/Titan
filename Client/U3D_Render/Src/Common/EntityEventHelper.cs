/*******************************************************************
** 文件名:	EntityEventHelper.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015/11/25
** 版  本:	1.0
** 描  述:	

		
********************************************************************/
using System;
using System.Runtime;
using UnityEngine;


public class EntityEventHelper : Singleton<EntityEventHelper>
{
    /// <summary>
    /// 发送EntityEvent命令到逻辑层
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="entity_id"></param>
    /// <param name="event_id"></param>
    /// <param name="nParam">附加参数（整数）</param>
    /// <param name="strParam">附加参数（字符串）</param>
    public void SendCommand(int entity_id, GameLogic.EntityLogicDef event_id, int nParam = 0, string strParam = "")
    {
        if (entity_id == 0)
            return;

        GameLogicAPI.onEntityEvent(entity_id, (int)event_id, nParam, strParam, IntPtr.Zero, 0);
    }


    /// <summary>
    /// 发送EntityEvent命令到逻辑层
    /// </summary>
    /// <typeparam name="T"></typeparam>
    /// <param name="entity_id"></param>
    /// <param name="event_id"></param>
    /// <param name="data"></param>
    public void SendCommand<T>(int entity_id,GameLogic.EntityLogicDef event_id, ref T data) where T : struct
    {
        if (entity_id == 0)
            return;

        IntPtrHelper helper = new IntPtrHelper();
        int data_len = System.Runtime.InteropServices.Marshal.SizeOf(data);

        IntPtr ptr = helper.toPtr<T>(ref data);
        GameLogicAPI.onEntityEvent(entity_id, (int)event_id, data_len, "", ptr, data_len);
    }       
       
}
