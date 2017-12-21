using UnityEngine;
using System.Collections.Generic;
using System;
using UIWidgets;

namespace USpeedUI.UWidgets
{
    public class ULifeHeroTileView : TileView<ULifeHeroComponent, ULifeHeroItem>
    {
        bool bIsStartedTileViewHero = false;

        public void Awake()
        {
            Start();
        }

        /// <summary>
        /// Start this instance.
        /// </summary>
        public override void Start()
        {
            if (bIsStartedTileViewHero)
            {
                return;
            }
            bIsStartedTileViewHero = true;

            base.Start();
        }


        /// <summary>
        /// Sets component data with specified item.
        /// </summary>
        /// <param name="component">Component.</param>
        /// <param name="item">Item.</param>
        protected override void SetData(ULifeHeroComponent component, ULifeHeroItem item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(ULifeHeroComponent component)
        {
            base.HighlightColoring(component);
        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(ULifeHeroComponent component)
        {
            base.SelectColoring(component);
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(ULifeHeroComponent component)
        {
            base.DefaultColoring(component);
        }

        protected override void SelectItem(int index)
        {
            if (index >= 0 && index < components.Count)
            {
                components[index].OnSelect(components[index]);
            }
        }

        public void SetAllDeselect()
        {
            foreach (ULifeHeroComponent component in components)
            {
                component.OnDeselect(component);
            }

            selectedIndex = -1;
        }
    }
}
