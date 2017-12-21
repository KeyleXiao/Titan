using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace USpeedUI.UWidgets
{
    public class URuneTypeTreeItemData : TreeViewItem
    {
        public int nRuneId;
        public int nRuneType;
        public string sRuneDesc ;

        public URuneTypeTreeItemData(string strItemName, UnityEngine.Sprite sprIcon) :base(strItemName, sprIcon)
        {

        }
    }
}
