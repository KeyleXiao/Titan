using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UTileClanCard : TileView<UTileClanCardComponent, UTileItemClanCard>
    {
        protected override void SetData(UTileClanCardComponent component, UTileItemClanCard item)
        {
            component.SetData(item);
        }
    }

    public class UTileClanSelfHero : TileView<UTileClanSelfHeroComponent, UTileItemClanSelfHero>
    {
        public Sprite[] cardValueSprites;

        protected override void SetData(UTileClanSelfHeroComponent component, UTileItemClanSelfHero item)
        {
            component.SetData(item, this);
        }
    }
}
