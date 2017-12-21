using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListRankRecord : ListViewCustom<UListComponentRankRecord, UListItemRankRecord>
    {
        protected override void SetData(UListComponentRankRecord component, UListItemRankRecord item)
        {
            component.SetData(item);
        }

        protected override void SelectColoring(UListComponentRankRecord component)
        {
            component.SelectColoring();
        }

        protected override void DefaultColoring(UListComponentRankRecord component)
        {
            component.DefaultColoring();
        }
    }
}
