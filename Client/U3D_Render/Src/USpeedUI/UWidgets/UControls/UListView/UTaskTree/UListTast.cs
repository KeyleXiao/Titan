using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListTaskReply : ListViewCustom<UListComponentTaskReply, UListItemTaskReply>
    {
        protected override void SetData(UListComponentTaskReply component, UListItemTaskReply item)
        {
            component.SetData(item);
        }
    }
}
