using System;
using UIWidgets;
using UnityEngine.UI;
using USpeedUI.GoodsDisposition;
using USpeedUI.WarGoodsStore;

namespace USpeedUI.UWidgets
{
    public class UNPCGoodsComponent : ListViewItem
    {
        /// <summary> 物品图片 </summary>
        public Image GoodsIcon;
        /// <summary> 物品价格 </summary>
        public Text GoodsCost;
        /// <summary> 物品名称 </summary>
        public Image GoodsSelectedFrame;
        /// <summary> 物品图标掩码 </summary>
        public Image GoodsIconMask;

        /// <summary>
        /// 物品tooltip
        /// </summary>
        public UTooltipTrigger GoodsTooltips;
        public String GoodsTooltip_IconParamName = "GoodsIcon";
        public String GoodsTooltip_CostParamName = "GoodsCost";
        public String GoodsTooltip_NameParamName = "GoodsName";
        public String GoodsTooltip_InfoParamName = "GoodsInfo";
        public String GoodsTooltip_LocInstructionsParamName = "GoodsLocationInstructions";

        public void SetData(UNPCGoodsItem item)
        {
            GoodsIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MobaGoods, WndID.WND_ID_NPC, UGoodsDispositionView.GoodsIconSize, item.schemeGoods.GoodsIconID);

            if (item.schemeGoods.GoodsCost > 0)
                GoodsCost.text = string.Format("{0}\n{1}", item.schemeGoods.GoodsCost.ToString(), item.schemeGoods.GoodsName);
            else
                GoodsCost.text = string.Format("{0}", item.schemeGoods.GoodsName);

            GoodsTooltips.SetImage(GoodsTooltip_IconParamName, UWarGoodsStoreView.GetWarGoodsIcon(item.schemeGoods.GoodsIconID));
            GoodsTooltips.SetText(GoodsTooltip_CostParamName, item.schemeGoods.GoodsCost.ToString());
            GoodsTooltips.SetText(GoodsTooltip_NameParamName, item.schemeGoods.GoodsName);
            string GoodsDes = item.schemeGoods.GoodsDes;
            UBBProperty.GetInstance().getRatio(GoodsDes);
            U3D_Render.Common.UBB.toHtml(ref GoodsDes, UBB_FORMAT_TYPE.UGUI);
            GoodsTooltips.SetText(GoodsTooltip_InfoParamName, GoodsDes);

            string GoodsLocIns = item.schemeGoods.GoodsLocationInstructions;
            U3D_Render.Common.UBB.toHtml(ref GoodsLocIns, UBB_FORMAT_TYPE.UGUI);
            if (string.IsNullOrEmpty(GoodsLocIns))
                GoodsLocIns = " ";
            GoodsTooltips.SetText(GoodsTooltip_LocInstructionsParamName, GoodsLocIns);
        }

        public void OnDeselect(ListViewItem item)
        {
            GoodsSelectedFrame.gameObject.SetActive(false);
        }

        public void OnSelect(ListViewItem item)
        {
            GoodsSelectedFrame.gameObject.SetActive(true);
        }

        public void SetIconMask(float fillCount)
        {
            GoodsIconMask.fillAmount = fillCount;
        }
    }
}
