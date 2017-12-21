using UnityEngine;
using System.Collections.Generic;
using System;
using UIWidgets;
using War;

namespace USpeedUI.UWidgets
{
    public class UNPCGoodsTileView : TileView<UNPCGoodsComponent, UNPCGoodsItem>
    {
        /// <summary>
        /// Sets component data with specified item.
        /// </summary>
        /// <param name="component">Component.</param>
        /// <param name="item">Item.</param>
        protected override void SetData(UNPCGoodsComponent component, UNPCGoodsItem item)
        {
            component.SetData(item);
        }

        /// <summary>
        /// Set highlights colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void HighlightColoring(UNPCGoodsComponent component)
        {
            base.HighlightColoring(component);
        }

        /// <summary>
        /// Set select colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void SelectColoring(UNPCGoodsComponent component)
        {
            base.SelectColoring(component);
        }

        /// <summary>
        /// Set default colors of specified component.
        /// </summary>
        /// <param name="component">Component.</param>
        protected override void DefaultColoring(UNPCGoodsComponent component)
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

        protected override void DeselectItem(int index)
        {
            if (index >= 0 && index < components.Count)
            {
                components[index].OnDeselect(components[index]);
            }
        }

        public void SetAllDeselect()
        {
            foreach (UNPCGoodsComponent component in components)
            {
                component.OnDeselect(component);
            }
        }

        public void SetGoodsIconMask()
        {
            uint tick = GameLogicAPI.getSyncTickCount();
            foreach (UNPCGoodsComponent component in components)
            {
                if (component.Index >= 0 && component.Index < DataSource.Count)
                {
                    float fillCount = 0;
                    uint lastTime = (uint)DataSource[component.Index].nLastBuyTime;
                    if (lastTime > 0)
                    {
                        int coolTime = DataSource[component.Index].nCoolTime;
                        int eclapsedTime = (int)(tick - lastTime) / 1000;
                        if (eclapsedTime < coolTime)
                        {
                            fillCount = (float)(coolTime - eclapsedTime) / (float)coolTime;
                        }
                    }

                    component.SetIconMask(fillCount);
                }
            }
        }

        public void UpdateGoodsMask(SNPCGoodsBuySuccess data)
        {
            foreach (UNPCGoodsItem goodsItem in DataSource)
            {
                if (goodsItem == null)
                    continue;

                if (goodsItem.uidNPC == data.uidNPC && goodsItem.schemeGoods.GoodsID == data.goodsID)
                {
                    goodsItem.nLastBuyTime = data.buyTime;
                    break;
                }
            }
        }
    }
}
