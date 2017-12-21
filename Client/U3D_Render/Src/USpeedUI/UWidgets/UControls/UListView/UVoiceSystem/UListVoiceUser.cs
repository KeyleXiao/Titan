using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListVoiceUser : ListViewCustom<UListComponentVoiceUser, UListItemVoiceUser>
    {
        protected override void SetData(UListComponentVoiceUser component, UListItemVoiceUser item)
        {
            component.SetData(item);
        }
    }
}
