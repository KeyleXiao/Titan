/*******************************************************************
** 文件名:	UTileViewHero
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	
		
********************************************************************/
using UnityEngine;
using System.Collections.Generic;
using System;
using UIWidgets;

namespace USpeedUI.UWidgets
{
    public class UTileViewHero:TileView<UTileViewComponentHero, UTileViewItemHero>
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
        protected override void SetData(UTileViewComponentHero component, UTileViewItemHero item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UTileViewComponentHero component)
        {
            base.HighlightColoring(component);
        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UTileViewComponentHero component)
        {
            base.SelectColoring(component);
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UTileViewComponentHero component)
        {
            base.DefaultColoring(component);
        }
    }
}
