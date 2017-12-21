using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI;
using UIWidgets;
using USpeedUI.UWidgets;


namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListGloryMoment : TileView<UListComponentGloryMoment, UListItemGloryMoment>
    {
        protected override void SetData(UListComponentGloryMoment component, UListItemGloryMoment item)
        {
            component.SetData(item);
        }
    }
}
