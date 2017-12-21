using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListLadder : ListViewCustom<UListComponentLadder,UListItemLadder>
    {
        protected override void SetData(UListComponentLadder component, UListItemLadder item)
        {
            component.SetData(item);
        }
    }
}
