/*******************************************************************
** 文件名:	UOBKeyCodeCtrlTreeItemData.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	昔文博
** 日  期:	2017/11/27
** 版  本:	1.0
** 描  述:	OB键位设置数据
** 应  用:  
********************************************************************/
using System;
using System.Collections.Generic;
using UIWidgets;

namespace USpeedUI.UWidgets.UOBKeyCodeCtrl
{
    public class UOBKeyCodeCtrlTreeItemData : TreeViewItem
    {
        public int nGroupID;
        public int nItemIndex;
        public string szGroupName;
        public string szLabel;
        public string szKeyCode;

        public UOBKeyCodeCtrlTreeItemData(string szGroupName, UnityEngine.Sprite spriteIcon) : base(szGroupName, spriteIcon)
        {

        }
    }
}
