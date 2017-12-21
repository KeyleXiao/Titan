using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListItemMicOrder
    {
        public bool isSpeaking;
        public string index;
        public int vest;
        public string name;
        public UListItemMicOrder() { }

        public UListItemMicOrder(bool state,string index,int vest, string name)
        {
            this.isSpeaking = state;
            this.index = index;
            this.vest = vest;
            this.name = name;
        }
    }
}
