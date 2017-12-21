using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListHeroTalentList : ListViewCustom<UListComponentTalentHeroItem, UListHeroTalentData>
    {

        protected override void SetData(UListComponentTalentHeroItem component, UListHeroTalentData item)
        {
            component.SetData(item);
        }

        protected override void HighlightColoring(UListComponentTalentHeroItem component)
        {
            base.HighlightColoring(component);
        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UListComponentTalentHeroItem component)
        {
            base.SelectColoring(component);
        }

    }   
}
