using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListMail : ListViewCustom<UListComponentMail, UListItemMail>
    {
        private float m_fLastReadMailTime = 0f;

        public float LastReadMailTime { get { return m_fLastReadMailTime; } }

        protected override void SetData(UListComponentMail component, UListItemMail item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UListComponentMail component)
        {

        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UListComponentMail component)
        {
            component.SelectColoring();
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UListComponentMail component)
        {
            component.DefaultColoring();
        }

        public override void Select(int index)
        {
            if (UnityEngine.Time.unscaledTime < m_fLastReadMailTime + 0.5f)
                return;

            m_fLastReadMailTime = UnityEngine.Time.unscaledTime;

            base.Select(index);
        }
    }

    public class UListMailGoods : ListViewCustom<UListComponentMailGoods, UListItemMailGoods>
    {
        protected override void SetData(UListComponentMailGoods component, UListItemMailGoods item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UListComponentMailGoods component)
        {

        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UListComponentMailGoods component)
        {
            component.SelectColoring();
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UListComponentMailGoods component)
        {
            component.DefaultColoring();
        }
    }

    public class UListMailPlusData : ListViewCustom<UListComponentMailPlusData, UListItemMailPlusData>
    {
        protected override void SetData(UListComponentMailPlusData component, UListItemMailPlusData item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UListComponentMailPlusData component)
        {

        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UListComponentMailPlusData component)
        {
            component.SelectColoring();
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UListComponentMailPlusData component)
        {
            component.DefaultColoring();
        }
    }
}
