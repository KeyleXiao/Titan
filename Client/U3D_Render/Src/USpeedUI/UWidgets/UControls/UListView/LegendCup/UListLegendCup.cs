using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListLegendCup : ListViewCustom<UListComponentLegendCup, UListItemLegendCup>
    {
        protected override void SetData(UListComponentLegendCup component, UListItemLegendCup item)
        {
            component.SetData(item);
        }

        protected override void SelectColoring(UListComponentLegendCup component)
        {
            component.SelectColoring();
        }

        protected override void DefaultColoring(UListComponentLegendCup component)
        {
            component.DefaultColoring();
        }
    }

    public class UListLegendCupRegist : ListViewCustom<UListComponentLegendCupRegist, UListItemLegendCupRegist>
    {
        protected override void SetData(UListComponentLegendCupRegist component, UListItemLegendCupRegist item)
        {
            component.SetData(item);
        }

        //protected override void SelectColoring(UListComponentLegendCupRegist component)
        //{
        //    component.SelectColoring();
        //}

        //protected override void DefaultColoring(UListComponentLegendCupRegist component)
        //{
        //    component.DefaultColoring();
        //}
    }

    public class UListLegendCupBlacklist : ListViewCustom<UListComponentLegendCupBlacklist, UListItemLegendCupBlacklist>
    {
        protected override void SetData(UListComponentLegendCupBlacklist component, UListItemLegendCupBlacklist item)
        {
            component.SetData(item);
        }

        protected override void SelectColoring(UListComponentLegendCupBlacklist component)
        {
            component.SelectColoring();
        }

        protected override void DefaultColoring(UListComponentLegendCupBlacklist component)
        {
            component.DefaultColoring();
        }
    }

    public class UListLegendCupGroup : ListViewCustom<UlistComponentLegendCupGroup, UListItemLegendCupGroup>
    {
        protected override void SetData(UlistComponentLegendCupGroup component, UListItemLegendCupGroup item)
        {
            component.SetData(item);
        }
    }

    public class UListLegendCupBigKnockout : ListViewCustom<UlistComponentLegendCupBigKnockout, UListItemLegendCupBigKnockout>
    {
        protected override void SetData(UlistComponentLegendCupBigKnockout component, UListItemLegendCupBigKnockout item)
        {
            component.SetData(item);
        }
    }
}
