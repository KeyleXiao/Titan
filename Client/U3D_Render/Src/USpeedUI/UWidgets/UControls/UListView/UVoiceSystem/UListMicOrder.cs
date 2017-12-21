using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListMicOrder : ListViewCustom<UListComponentMicOrder, UListItemMicOrder>
    {
        protected override void SetData(UListComponentMicOrder component, UListItemMicOrder item)
        {
            component.SetData(item);
        }
    }
}
