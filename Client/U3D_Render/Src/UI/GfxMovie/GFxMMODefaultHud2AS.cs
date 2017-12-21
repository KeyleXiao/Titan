/*******************************************************************
** 文件名:	GFxMMODefaultHud.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	陈言蒙
** 日  期:	2016/1/13
** 版  本:	1.0
** 描  述:	非战场界面的
** 应  用:  
 *          对应Flash Layout文件：MMODefaultHud.swf/gfx
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述:
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using Scaleform;
using GameLogic;

partial class GFxMMODefaultHud
{
    #region C# -> AS
    // 设置自己段位,青铜五--王者
    public void SetStrengthLine(IntPtr ptrParam, int nPtrLen)
    {
        if (!Value.IsValueValid(MyRoot))
        {
            return;
        }
        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
        cmd_count nCountdata = helper.get<cmd_count>();
        for (int i = 0; i < nCountdata.nCount; ++i)
        {
            cmd_entity_match_type_rank data = helper.get<cmd_entity_match_type_rank>();
            if (MyRoot.Invoke("SetStrengthLine", data.szGradeName) == null)
            {
                Trace.LogError("call SetStrengthLine failed");
                return;
            }
        }
    }
    #endregion
}