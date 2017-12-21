using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using USpeedUI.UWidgets;

namespace U3D_Render.USpeedUI.UWidgets
{
    public class UListWarMainMode : ListViewCustom<UListWarMainModeComponent, UListWarMainModeItem>
    {
        protected override void SetData(UListWarMainModeComponent component, UListWarMainModeItem item)
        {
            component.SetData(item);
        }
    }

    
}
