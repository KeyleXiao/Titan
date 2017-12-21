using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using USpeedUI.UExtensions;
using USpeedUI.UEffect;
using DG.Tweening;

namespace USpeedUI.WarGoodsStore
{
    /// <summary>
    /// 商店物品显示对象
    /// </summary>
    public class UWarGoodsStoreItemComponent : ListViewItem
    {
        /// <summary> 物品图片 </summary>
        public Image GoodsIcon;
        /// <summary> 物品价格 </summary>
        public Text GoodsCost;
        /// <summary> 物品选择边框 </summary>
        public Image GoodsSelectedFrame;
        /// <summary> 物品可使用次数 </summary>
        public Text GoodsCanUseTimes;
        /// <summary> 已购买物品图标 </summary>
        public Image GoodsPurchasedIcon;

        /// <summary>
        /// 变色效果
        /// </summary>
        public static UEffect.UEffectParamBase StaticGoodsIconMatParam = new UEffect.UEffectMaterialParam(UEffect.UEffectMaterialType.EMT_Gray);
        /// <summary>
        /// 物品tooltip
        /// </summary>
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


        public bool CanUseProperty_GoodsCanBuy = true;
        /// <summary>
        /// 是否要显示物品名字和物品价格
        /// true:名字、价格都显示
        /// false： 只显示价格
        /// </summary>
        public bool CanShowProperty_GoodsName = false;
        /// <summary>
        /// 是否需要根据已购买物品数据控制显示
        /// true:已购买物品隐藏物品名字和价格，显示已购买图标
        /// false:显示物品名字和价格
        /// </summary>
        public bool CanUseProperty_IsPurchased = false;

        public void SetData(UWarGoodsStoreItem item)
        {
            //if (item.SchemeGoodsInfo != null && gameObject != null)
            //    Debug.Log(item.SchemeGoodsInfo.GoodsName + ",Enable：" + this.enabled + ",go.active:" + this.gameObject.activeInHierarchy);
            //else
            //    Debug.Log("null 不能设置颜色");
           if (CurrentGoodsID != item.GoodsID)
            {
                CurrentGoodsID = item.GoodsID;
                NonAlwaysChangeData(item);
            }

            AlwaysChangeData(item);

            GoodsTooltips.enabled = CurrentGoodsID > 0;

        }

        protected virtual void AlwaysChangeData(UWarGoodsStoreItem item)
        {
            if (GoodsCost != null )
            {
                if (item.SchemeGoodsInfo != null && CanShowProperty_GoodsName)
                    GoodsCost.text = item.SchemeGoodsInfo.GoodsName + "\n" + item.GoodsCost.ToString();
                else
                    GoodsCost.text = item.GoodsCost.ToString();
            }


            if (CanUseProperty_GoodsCanBuy)
            {
                if (item.GoodsCanBuy)
                {
                    GoodsIcon.color = Color.white;//.DOColor(Color.white, 0.1f);
                    GoodsCost.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
                }
                else
                {
                    GoodsIcon.color = Color.gray;//.DOColor(Color.gray, 0.1f);
                    GoodsCost.color = Color.gray;
                }
                //GoodsIcon.Rebuild(CanvasUpdate.PreRender);
            }
			//物品购买时是否需要显示已购买图标
            if(CanUseProperty_IsPurchased && GoodsPurchasedIcon != null)
            {
                GoodsPurchasedIcon.gameObject.SetActive(item.IsPurchased);
                GoodsCost.gameObject.SetActive(!item.IsPurchased);
            }

            if (GoodsCanUseTimes != null)
            {
                GoodsCanUseTimes.text = item.GoodsCanUseTimes > 1 ? item.GoodsCanUseTimes.ToString() : " ";
            }

            if(IsTooltipInit())
                GoodsTooltips.SetText(GoodsTooltip_CostParamName, item.GoodsCost.ToString());
        }

        protected virtual void NonAlwaysChangeData(UWarGoodsStoreItem item)
        {
            
            UWarGoodsStoreView.GetWarGoodsIcon(item.SchemeGoodsInfo != null ? item.SchemeGoodsInfo.GoodsIconID:0,GoodsIcon);



            if (IsTooltipInit() && item.SchemeGoodsInfo != null)
            {
                GoodsTooltips.SetImage(GoodsTooltip_IconParamName, UWarGoodsStoreView.GetWarGoodsIcon(item.SchemeGoodsInfo.GoodsIconID));
                GoodsTooltips.SetText(GoodsTooltip_NameParamName, item.SchemeGoodsInfo.GoodsName);
                string GoodsDes = item.SchemeGoodsInfo.GoodsDes;
                UBBProperty.GetInstance().getRatio(GoodsDes);
                U3D_Render.Common.UBB.toHtml(ref GoodsDes,UBB_FORMAT_TYPE.UGUI);
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

        public void SetVisible(bool _bVisible)
        {
            this.gameObject.SetActive(_bVisible);
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

    public class UWarGoodsStorePurchasedItemComponent : UWarGoodsStoreItemComponent
    {
        protected override void Awake()
        {
            base.Awake();
            CanUseProperty_GoodsCanBuy = false;
        }

        protected override void AlwaysChangeData(UWarGoodsStoreItem item)
        {
            base.AlwaysChangeData(item);

            if (IsTooltipInit())
                GoodsTooltips.SetText(GoodsTooltip_CostParamName, item.GoodsSoldOutCost.ToString());
        }
    }
}
