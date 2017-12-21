/*******************************************************************
** 文件名:	GFxTooltipHud.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	陈言蒙
** 日  期:	2015/09/28
** 版  本:	1.0
** 描  述:	游戏状态 GFx界面显示 团队信息, 拆分C#代码中与AS通讯的逻辑
** 应  用:  
 *          对应Flash Layout文件：TooltipHud.swf/gfx
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using Scaleform;
using U3D_Render.Common;

partial class GFxTooltipHud 
{

    #region C# -> AS

    public void ShowTooltipEx(Value _tooltipIns, double _x, double _y)
    {
        //Trace.LogError("ShowTooltipEx");
        /*
         * 传的位置要调整
        */
        if (MyRoot.Invoke("ShowTooltip", _tooltipIns, _x, _y) == null)
            Trace.LogError("call ShowTooltip error");
        //  else
        //      Trace.LogError("call ShowTooltip success");

    }

    public void HideTooltipEx(string _tooltipType)
    {
        if (_tooltipType != _curTooltipType)
            return;

        if (MyRoot.Invoke("HideTooltip") == null)
            Trace.LogError("call HideTooltip error");

        this._curTooltipType = "";
    }

    /// <summary>
    /// 判断要显示tooltip的目标所在位置属于舞台九宫格的哪个区域内
    /// </summary>
    /// <param name="_x">目标相对于舞台的x坐标</param>
    /// <param name="_y">目标相对于舞台的y坐标</param>
    /// <returns>0~8为3x3的九宫格格子序号</returns>
    public int GetTargetIn9Grid(double _x, double _y)
    {
        int res = -1;
        Value resValue = null;

        if (MyRoot.Invoke("ContainsPointInGrid", _x, _y) == null)
        {
            Trace.LogError("call ContainsPointInGrid failed");
            return res;
        }

        if (!GetMemberWithCheck(MyRoot, "PointContainsInGridIndex", out resValue))
        {
            return res;
        }
        res = (int)resValue;

        return res;
    }

    #endregion
}