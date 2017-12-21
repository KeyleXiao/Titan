using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using ASpeedGame.Data.GameMobaSchemes;
using UnityEngine.UI;
using USpeedUI.UExtensions;
using USpeedUI.UEffect;
using UIWidgets;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.Events;

namespace USpeedUI.WarGoodsStore
{
    public class UWarGoodsStoreRecommendComponent : UButtonGroupItem
    {
        /// <summary>
        /// 物品图片
        /// </summary>
        [SerializeField]
        public Image GoodsIcon;
        /// <summary>
        /// 物品价格
        /// </summary>
        [SerializeField]
        public Text GoodsCost;
        /// <summary>
        /// 物品选择边框
        /// </summary>
        [SerializeField]
        public Image GoodsSelectedFrame;
        /// <summary>
        /// 物品tooltip
        /// </summary>
        [SerializeField]
        public UTooltipTrigger GoodsTooltips;

        public String GoodsTooltip_IconParamName = "GoodsIcon";
        public String GoodsTooltip_CostParamName = "GoodsCost";
        public String GoodsTooltip_NameParamName = "GoodsName";
        public String GoodsTooltip_InfoParamName = "GoodsInfo";
        public String GoodsTooltip_LocInstructionsParamName = "GoodsLocationInstructions";

        /// <summary>
        /// 当前组件对应的物品ID，控制一些不需要经常更新的数据
        /// </summary>
        [NonSerialized]
        public int CurrentGoodsID = -1;

        public static UEffectParamBase StaticGoodsIconMatParam = new UEffectMaterialParam(UEffectMaterialType.EMT_Gray);
        public Color GoodsIconUnBuyColor = new Color(0.5f, 0.5f, 0.5f);
        public Color GoodsIconCanBuyColor = Color.gray;

        protected override void Awake()
        {
            base.Awake();
            if (GoodsIcon == null)
                GoodsIcon = this.transform.FindChild("WarGoodsIconBase/GoodsIcon").GetComponent<Image>();
            if (GoodsCost == null)
                GoodsCost = this.transform.FindChild("CostText").GetComponent<Text>();
            if (GoodsSelectedFrame == null)
                GoodsSelectedFrame = this.transform.FindChild("SelectedFrame").GetComponent<Image>();
            if (GoodsTooltips == null)
                GoodsTooltips = this.gameObject.GetComponent<UTooltipTrigger>();

        }

        protected override void OnDisable()
        {

        }

        public void SetData(UWarGoodsStoreItem item)
        {
            AlwaysChangeData(item);

            if (CurrentGoodsID != item.GoodsID)
            {
                CurrentGoodsID = item.GoodsID;
                NonAlwaysChangeData(item);
            }
            GoodsTooltips.enabled = CurrentGoodsID > 0;

            this.gameObject.SetActive(item.GoodsComponentVisible);
        }

        protected virtual void AlwaysChangeData(UWarGoodsStoreItem item)
        {
            if (GoodsCost != null)
                GoodsCost.text = item.GoodsCost.ToString();

            if (item.GoodsCanBuy)
            {
                GoodsIcon.color = Color.white;
                GoodsCost.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
            }
            else
            {
                GoodsIcon.color = Color.gray;
                GoodsCost.color = Color.gray;
            }


            if (IsTooltipInit())
                GoodsTooltips.SetText(GoodsTooltip_CostParamName, item.GoodsCost.ToString());

        }

        protected virtual void NonAlwaysChangeData(UWarGoodsStoreItem item)
        {
            if (item.GoodsID == 0 || item.SchemeGoodsInfo == null)
                return;
            UWarGoodsStoreView.GetWarGoodsIcon(item.SchemeGoodsInfo != null ? item.SchemeGoodsInfo.GoodsIconID : 0, GoodsIcon);


            if (IsTooltipInit())
            {
                GoodsTooltips.SetImage(GoodsTooltip_IconParamName, UWarGoodsStoreView.GetWarGoodsIcon(item.SchemeGoodsInfo.GoodsIconID));
                GoodsTooltips.SetText(GoodsTooltip_NameParamName, item.SchemeGoodsInfo.GoodsName);
                string GoodsDes = item.SchemeGoodsInfo.GoodsDes;
                UBBProperty.GetInstance().getRatio(GoodsDes);
                U3D_Render.Common.UBB.toHtml(ref GoodsDes, UBB_FORMAT_TYPE.UGUI);
                GoodsTooltips.SetText(GoodsTooltip_InfoParamName, GoodsDes);

                string GoodsLocIns = item.SchemeGoodsInfo.GoodsLocationInstructions;
                U3D_Render.Common.UBB.toHtml(ref GoodsLocIns, UBB_FORMAT_TYPE.UGUI);
                GoodsTooltips.SetText(GoodsTooltip_LocInstructionsParamName, GoodsLocIns);
            }
            else
            {
                GoodsTooltips.SetImage(GoodsTooltip_IconParamName, null);
                GoodsTooltips.SetText(GoodsTooltip_NameParamName, "");
                GoodsTooltips.SetText(GoodsTooltip_InfoParamName, "");
                GoodsTooltips.SetText(GoodsTooltip_LocInstructionsParamName, "");
            }

        }

        protected bool IsTooltipInit()
        {
            if (GoodsTooltips == null || GoodsTooltips.tooltipStyle == null)
            {
                Trace.LogWarning("物品的Tooltip未初始化或TooltipStyle为null。");
                return false;
            }
            return true;
        }

    }
}
