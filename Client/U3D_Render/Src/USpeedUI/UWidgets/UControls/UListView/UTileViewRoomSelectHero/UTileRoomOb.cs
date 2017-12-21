using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace USpeedUI.UWidgets
{
    public class UTileRoomOb : TileView<UTileComponentRoomOb, UTileItemRoomOb>
    {
        protected override void SetData(UTileComponentRoomOb component, UTileItemRoomOb item)
        {
            component.SetData(item);
        }
    }
}
