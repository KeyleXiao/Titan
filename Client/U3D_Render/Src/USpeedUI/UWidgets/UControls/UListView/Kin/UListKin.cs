using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListKin : ListViewCustom<UListComponentKin,UListItemKin>
    {
        protected override void SetData(UListComponentKin component, UListItemKin item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UListComponentKin component)
        {

        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UListComponentKin component)
        {
            component.SelectColoring();
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UListComponentKin component)
        {
            component.DefaultColoring();
        }
    }


    public class UListKinMember : ListViewCustom<UListComponentKinMember, UListItemKinMember>
    {
        protected override void SetData(UListComponentKinMember component, UListItemKinMember item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UListComponentKinMember component)
        {
            
        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UListComponentKinMember component)
        {
            component.SelectColoring();
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UListComponentKinMember component)
        {
            component.DefaultColoring();
        }
    }

    public class UListKinAcceptMember : ListViewCustom<UListComponentKinAcceptMember, UListItemKinAcceptMember>
    {
        protected override void SetData(UListComponentKinAcceptMember component, UListItemKinAcceptMember item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UListComponentKinAcceptMember component)
        {

        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UListComponentKinAcceptMember component)
        {
            component.SelectColoring();
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UListComponentKinAcceptMember component)
        {
            component.DefaultColoring();
        }
    }

    public class UListKinEvent : ListViewCustom<UListComponentKinEvent, UListItemKinEvent>
    {
        protected override void SetData(UListComponentKinEvent component, UListItemKinEvent item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UListComponentKinEvent component)
        {

        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UListComponentKinEvent component)
        {
            component.SelectColoring();
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UListComponentKinEvent component)
        {
            component.DefaultColoring();
        }
    }

    public class UListKinGloryCup  : TileView<UListComponentKinGloryCup, UListItemKinGloryCup>
    {
        protected override void SetData(UListComponentKinGloryCup component, UListItemKinGloryCup item)
        {
            component.SetData(item);
        }
    }
    
}
