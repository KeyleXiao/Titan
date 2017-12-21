using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using USpeedUI.UWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListItemKin
    {
        public int nIndex;
        public int KinID;
        public string Name;
        public string Count;
        public string MasterName;
        public string KinDes;
        public string ApplyDes;
    }

    public class UListItemKinMember
    {
        public int nSex;
        public int isOnline;
        public int PlayerID;
        public string GradIcon;
        public int IdentityID;
        public int PersonIcon;
        public string PersonName;
        public int Power;
        public string PowerRank;
        public byte GameState;
        public string Level;
        public bool bIsSelf;
        public int nLastOrder;
        public int nBVOrder;
        public UPopupAction PopupAction;

    }

    public class UListItemKinAcceptMember
    {
        public int PlayerID;
        public string PersonName;
        public string Level;
        public string Power;

    }

    public class UListItemKinEvent
    {
        
        public string KinEventDes;
    }

    public class UListItemKinGloryCup
    {
        public long llLegendCupID;
        public int nCupRank;
        public int nCupConfigID;
        public int nEndTime;
        public string MatchName;
        public string ConfigName;
    }

    
}

    
