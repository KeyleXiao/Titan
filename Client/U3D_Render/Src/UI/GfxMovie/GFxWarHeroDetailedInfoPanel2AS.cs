///*******************************************************************
//** 文件名:	GFxWarHeroDetailedInfoPanel.cs
//** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
//** 创建人:	陈言蒙
//** 日  期:	2015/09/28
//** 版  本:	1.0
//** 描  述:	游戏状态 GFx界面显示 团队信息, 拆分C#代码中与AS通讯的逻辑
//** 应  用:  
// *          对应Flash Layout文件：WarHeroDetailedInfoPanel.swf/gfx
//********************************************************************/
//using UnityEngine;
//using System;
//using System.Collections;
//using Scaleform;
//using ASpeedGame.Data.GameMobaSchemes;
//using System.Collections.Generic;

//partial class GFxWarHeroDetailedInfoPanel 
//{
//    #region CS -> AS
//    private void SetPropertyLabelName(int _nIndex, string _strName)
//    {
//        if(Value.IsValueValid(vInformationFrame))
//        {
//            if(vInformationFrame.Invoke("SetPropertyLabelName",_nIndex,_strName) == null)
//            {
//                Trace.LogError("call SetPropertyLabelName failed");
//                return;
//            }
//        }
//    }

//    private void SetPropertyLabelDes(int _nIndex, string _strDes)
//    {
//        if (Value.IsValueValid(vInformationFrame))
//        {
//            if (vInformationFrame.Invoke("SetPropertyLabelDes", _nIndex, _strDes) == null)
//            {
//                Trace.LogError("call SetPropertyLabelDes failed");
//                return;
//            }
//        }
//    }

//    private void SetPropertyLabelValue(int _nIndex, string _strValue)
//    {
//        if (Value.IsValueValid(vInformationFrame))
//        {
//            if (vInformationFrame.Invoke("SetPropertyLabelValue", _nIndex, _strValue) == null)
//            {
//                Trace.LogError("call SetPropertyLabelValue failed");
//                return;
//            }
//        }
//    }
//    #endregion
//}