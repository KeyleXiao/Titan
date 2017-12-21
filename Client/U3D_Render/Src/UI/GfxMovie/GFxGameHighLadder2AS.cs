/*******************************************************************
** 文件名:	GFxGameHighLadder2AS.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	陈言蒙
** 日  期:	2016/01/05
** 版  本:	1.0
** 描  述:	GFx 游戏大厅界面, 拆分C#代码中与AS通讯的逻辑
				
** 应  用:  	对应Flash Layout文件：GFxGameHighLadder.swf/gfx

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Runtime.InteropServices;
using Scaleform;
using GameLogic;

public partial class GFxGameHighLadder
{
    #region C# -> AS 

    // 添加200个天梯排名数据，一起发过来解析,刷新也用这个
    public void AddHighLadderItem(ref IntPtr ptrParam, ref  int nPtrLen)
    {
        if (!Value.IsValueValid(vRoomItemStruct) || !Value.IsValueValid(vRoomListFrame))
        {
            return;
        }
        // 更新数据的时候先移除旧数据，再加载新数据
        if (vRoomListFrame.Invoke("RemoveHallRoomAllItem") == null)
        {
            Trace.LogError("call RemoveHallRoomAllItem error!!");
        }
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        cmd_Entity_Rank_Info_head nCountdata = helper.get<cmd_Entity_Rank_Info_head>();
        for (uint i= 1; i <= nCountdata.nCount; ++i)
        {
            // 暂时用这个结构体
            cmd_Entity_HighLadder_Rank_Info data = helper.get<cmd_Entity_HighLadder_Rank_Info>();
            vRoomItemStruct.ClearElements();
            vRoomItemStruct.SetElement(0, data.nOrderID);
            vRoomItemStruct.SetElement(1, data.szPlayerName);
            vRoomItemStruct.SetElement(2, data.nRankGrade);
            vRoomItemStruct.SetElement(3, data.nRankScore);
            vRoomItemStruct.SetElement(4, data.szGradeName);
           // vRoomItemStruct.SetElement(i, data);
            if (vRoomListFrame.Invoke("AddHallRoomItem") == null)
            {
                Trace.LogError("call AddHallRoomItem failed!!");
            }
        }
    }
    #endregion

}
