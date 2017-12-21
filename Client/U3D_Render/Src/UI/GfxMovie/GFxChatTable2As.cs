using UnityEngine;
using System;
using System.Collections.Generic;
using Scaleform;
using System.Collections;
using GameLogic;
using U3D_Render;
using System.Runtime.InteropServices;
using U3D_Render.Common;

partial class GFxChatTable
{
    #region C#->AS
    // 0为默认类型 显示系统数据
    public void ShowChatInfo(String stChatInfo, int nType = 0, bool bIsSelf = true)
    {
        if (!Value.IsValueValid(_vMainChatTable))
        {
            return;
        }
        
        if( stChatInfo == null )
        {
            return;
        }
        string stInfo = stChatInfo;
        UBB.toHtml(ref stInfo);
        if (_vMainChatTable.Invoke("ShowChatInfo", stInfo, nType, bIsSelf) == null)
        {
            Trace.LogError("call ShowChatInfo failed");
            return;
        }
    }

    #endregion
}