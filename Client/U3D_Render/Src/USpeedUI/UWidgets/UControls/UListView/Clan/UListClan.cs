using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListClanMember : ListViewCustom<UListComponentClanMember, UListItemClanMember>
    {
        protected override void SetData(UListComponentClanMember component, UListItemClanMember item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UListComponentClanMember component)
        {

        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UListComponentClanMember component)
        {
            component.SelectColoring();
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UListComponentClanMember component)
        {
            component.DefaultColoring();
        }
    }

    public class UListClanAcceptMember : ListViewCustom<UListComponentClanAcceptMember, UListItemClanAcceptMember>
    {
        protected override void SetData(UListComponentClanAcceptMember component, UListItemClanAcceptMember item)
        {
            component.SetData(item);
        }

        protected override void SelectColoring(UListComponentClanAcceptMember component)
        {
            component.SelectColoring();
        }

        protected override void DefaultColoring(UListComponentClanAcceptMember component)
        {
            component.DefaultColoring();
        }
    }

    public class UListClanEvent : ListViewCustom<UListComponentClanEvent, UListItemClanEvent>
    {
        protected override void SetData(UListComponentClanEvent component, UListItemClanEvent item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UListComponentClanEvent component)
        {

        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UListComponentClanEvent component)
        {
            component.SelectColoring();
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UListComponentClanEvent component)
        {
            component.DefaultColoring();
        }
    }
}
