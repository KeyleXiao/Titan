using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine.UI;
using USpeedUI;
using USpeedUI.UWidgets;
using UnityEngine;
using USpeedUI.TimeThresholdGoodsPart;
using Data.PointShopConfig;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UListComponentUTimeThresholdItem : ListViewItem
    {
        private UListTimeThresholdData ItemData;
        public Image GoodIconImage;
        public Text GoodTimeThresholdText;
        public Text[] GoodNameText = new Text[2];
        public Text GoodParamDesc;
        public GameObject GoodShadowObj;
        public virtual void SetData(UListTimeThresholdData item)
        {
            GoodIconImage.sprite = GetSprite(item.nGoodType, item.nGoodIconId, item.nParam);
            for (int i = 0; i < 2;++i )
            {
                GoodNameText[i].text = item.sGoodName[i];
            }

            DateTime calcTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(item.nGoodThresholdTime);
            calcTime = calcTime.ToLocalTime();

            TimeSpan ts = calcTime.Subtract(DateTime.Now).Duration();
            GoodTimeThresholdText.text = ts.Days.ToString() + "天" + ts.Hours.ToString() + "小时" + ts.Minutes.ToString() + "分";

            if(item.nParam>0&&!string.IsNullOrEmpty(item.szParam))
            {
                GoodParamDesc.gameObject.SetActive(true);
                GoodParamDesc.text = GetParamText(item.nGoodType,item.nParam);
            }
            else
            {
                GoodParamDesc.gameObject.SetActive(false);
            }

            GoodShadowObj.SetActive(this.Index%2==0);
            Item = item;
        }

        public UListTimeThresholdData Item
        {
            set
            {
                ItemData = value;
            }
            get
            {
                return ItemData;
            }
        }

        public override void OnPointerClick(UnityEngine.EventSystems.PointerEventData eventData)
        {
            base.OnPointerClick(eventData);
        }

        public override void OnPointerEnter(UnityEngine.EventSystems.PointerEventData eventData)
        {
            base.OnPointerEnter(eventData);
        }

        public Sprite GetSprite(int nGoodType,int nGoodIconID,int nParam)
        {
            Sprite goodSprite = null;
            if(nGoodType == (int)TimeThresholdGoodsType.TIMETHRESHOLDGOODSTYPE_SKIN)
            {
                //goodSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_TIMETHRESHOLDVIEW, nGoodIconID, 3, nParam);
                goodSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_TIMETHRESHOLDVIEW, 1, 7, nParam);
            }

            return goodSprite;
        }

        private string GetParamText(int nGoodType, int nParam)
        {
            string szParam = "";
            if (nGoodType == (int)TimeThresholdGoodsType.TIMETHRESHOLDGOODSTYPE_SKIN)
            {
                PersonSkinModelItem modelData = new PersonSkinModelItem();
                if (GameLogicAPI.GetPersonModel((UInt16)nParam, ref modelData))
                {
                    szParam = "英雄 ： " + modelData.szHeroName;
                }

            }

            return szParam;
        }

        public void OnBuyClick()
        {
            if (ItemData.nGoodType == (int)TimeThresholdGoodsType.TIMETHRESHOLDGOODSTYPE_SKIN)
            {
                int nSellID = PointShopConfig.Instance.GetPointShopSellIDBySkinID(ItemData.nParam, ItemData.nGoodId);
                if (nSellID > 0)
                {
                    PointShopBuyGoodsInitData uiData = new PointShopBuyGoodsInitData();
                    uiData.nSellID = nSellID;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN, uiData);
                }
            }
        }
    }
   
}
