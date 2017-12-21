using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using USpeedUI.UWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListItemMail
    {
        public int nIndex;
        public long nMailID;
        public int nType;
        public int nSourceType;
        public string szTitle;
	    public string szSenderName;
        public int nSenderPDBID;
	    public int nIsPlusData;
        public int nRecvTime;
        public int nLifeTime;
        public int nReadFlag;
    }

    public class UListItemMailGoods
    {
        public int nIndex;
        public int nGoodsID;
        public int nCount;
    }

    public class UListItemMailPlusData
    {
        public int nIndex;
        public string szName;
        public int nCount;
    }
}

    
