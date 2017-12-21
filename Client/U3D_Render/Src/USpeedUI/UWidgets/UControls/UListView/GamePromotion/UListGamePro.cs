using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListMyUser : ListViewCustom<UListComponentMyUser, UListItemMyUser>
    {
        protected override void SetData(UListComponentMyUser component, UListItemMyUser item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UListComponentMyUser component)
        {

        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UListComponentMyUser component)
        {
            component.SelectColoring();
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UListComponentMyUser component)
        {
            component.DefaultColoring();
        }
    }
}
