using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListHall : ListViewCustom<UListComponentHall, UListItemHallData>
    {
        protected override void SetData(UListComponentHall component, UListItemHallData item)
        {
            component.SetData(item);
        }

        public void UpdateVisible()
        {
            var indicies = Enumerable.Range(topHiddenItems, visibleItems).ToArray();
            components.ForEach((x, i) =>
            {
                x.Index = indicies[i];
                SetData(x, DataSource[indicies[i]]);
                Coloring(x as ListViewItem);
            });
        }

        protected override void SelectColoring(UListComponentHall component)
        {
            base.SelectColoring(component);
            component.SelectColoring();
        }

        protected override void DefaultColoring(UListComponentHall component)
        {
            base.DefaultColoring(component);
            component.DefaultColoring();
        }
    }  
}
