using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.UI;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListComponentMyUser : ListViewItem
    {
        public Text accountText;
        public Text totalPayText;
        public Text totalExchangeText;
        public Text createDatetimeText;
        public Sprite SelectBGSprite;
        public Sprite DefalutBGSprite;

        public virtual void SetData(UListItemMyUser item)
        {
            accountText.text = item.data.UserName;
            totalPayText.text = item.data.TotalPay;
            totalExchangeText.text = item.data.TotalExchange;
            createDatetimeText.text = item.data.CreateDatetime;
        }

        public virtual void DefaultColoring()
        {
            // 未选中
            //if (ComponentBG != null)
            //    ComponentBG.sprite = SelectBGSprite;
        }

        public virtual void SelectColoring()
        {
            // 选中
            //if (ComponentBG != null)
            //    ComponentBG.sprite = SelectBGSprite;
        }
    }
}
