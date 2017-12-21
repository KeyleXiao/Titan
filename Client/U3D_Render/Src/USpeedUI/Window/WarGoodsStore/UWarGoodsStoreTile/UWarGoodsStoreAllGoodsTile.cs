using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;

namespace USpeedUI.WarGoodsStore
{
    /// <summary>
    /// 商店物品列表
    /// </summary>
    public class UWarGoodsStoreAllGoodsTile : TileView<UWarGoodsStoreItemComponent,UWarGoodsStoreItem>
    {

        protected override void SetData(UWarGoodsStoreItemComponent component, UWarGoodsStoreItem item)
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
    }

    public class UWarGoodsStorePurchasedGoodsTile : TileView<UWarGoodsStorePurchasedItemComponent, UWarGoodsStoreItem>
    {
        protected override void SetData(UWarGoodsStorePurchasedItemComponent component, UWarGoodsStoreItem item)
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
    }
}
