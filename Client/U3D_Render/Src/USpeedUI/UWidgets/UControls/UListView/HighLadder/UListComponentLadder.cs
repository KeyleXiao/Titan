using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine.UI;
using USpeedUI.UWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListComponentLadder : ListViewItem
    {
        public Text OrderID;
        public Text PlayerName;
        public Text RankGrade;
        public Text RankScore;
        public Text RankGradeDes;
        public UPopupMenuHandle PopupMenuHandle;

        public virtual void SetData(UListItemLadder item)
        {
            OrderID.text = item.OrderID.ToString();
            PlayerName.text = item.PlayerName;
            RankGrade.text = item.RankGrade.ToString();
            RankScore.text = item.RankScore.ToString();
            RankGradeDes.text = item.RankGradeDes.ToString();

            if (PopupMenuHandle == null)
                PopupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();
            PopupMenuHandle.HandlePopupAction = item.PopupAction;
        }
    }
}
