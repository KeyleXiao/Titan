using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UIWidgets;
using UnityEngine.UI;
using GameLogic;
using USpeedUI;
using UnityEngine;
using UnityEngine.EventSystems;
using DataCenter;
using USpeedUI.PointShop;

namespace U3D_Render.USpeedUI.UWidgets.UControls.UListView
{
    public class UTileClanCardComponent : ListViewItem
    {
        public Image cardIcon;
        public Text Name;
        public Text Count;
        public Text DonateCount;
        public Image donateProcessImage;
        public Button JuanZengButton;
        public Sprite[] donateProcessImageSprites;

        private UTileItemClanCard itemData;
        private float processMaxSize;

        protected override void Awake()
        {
            base.Awake();

            processMaxSize = donateProcessImage.rectTransform.sizeDelta.x;
            JuanZengButton.onClick.AddListener(onJuanZeng);
        }

        public virtual void SetData(UTileItemClanCard item)
        {
            itemData = item;

            cardIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_CLAN_SELECTREQUESTCARD, 1, 7, item.data.nCardID);
            Name.text = item.data.szName;
            Count.text = item.data.nMyCurCount + "/" + item.data.nMyMaxCount;
            DonateCount.text = item.data.nCurCount + "/" + item.data.nMaxCount;

            Vector2 curProcessSize = donateProcessImage.rectTransform.sizeDelta;
            if (item.data.nMaxCount > 0)
                curProcessSize.x = processMaxSize * (float)item.data.nCurCount / (float)item.data.nMaxCount;
            else
                curProcessSize.x = processMaxSize;
            donateProcessImage.rectTransform.sizeDelta = curProcessSize;

            if (item.data.nCurCount > 0 && item.data.nCurCount == item.data.nMaxCount)
            {
                donateProcessImage.sprite = donateProcessImageSprites[1];
            }
            else
            {
                donateProcessImage.sprite = donateProcessImageSprites[0];
            }

            int nHeroDBID = 0;
            if (EntityFactory.MainHeroView != null)
                nHeroDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            if (item.data.nMyCurCount <= 0 || item.data.nPDBID == nHeroDBID || item.data.nDonateCardCount <= 0 || item.data.nDayDonateCardCount <= 0)
                JuanZengButton.interactable = false;
            else
                JuanZengButton.interactable = true;
        }

        public void onJuanZeng()
        {
            if (itemData == null)
                return;

            cmd_clan_donate_card cmdData;
            cmdData.nDBID = itemData.data.nPDBID;
            cmdData.nCardID = itemData.data.nCardID;
            cmdData.nCardCount = 1;
            // 捐赠卡牌
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_CLAN_DONATE_CARD, ref cmdData);
        }
    }

    public class UTileClanSelfHeroComponent : ListViewItem
    {
        public Image maxBg;
        public Image heroIcon;
        public GameObject highlightIcon;
        public Image cardBase;
        public Image cardValue;
        public Text cardText;
        public RectTransform heroStarFrame;
       
        private HeroStarItem heroStarItem;

        protected override void Awake()
        {
            base.Awake();

            this.onSelect.AddListener(OnSelect);
            this.onDeselect.AddListener(OnDeselect);

            if (heroStarItem == null)
            {
                heroStarItem = LogicDataCenter.pointShopDataManager.CreateHeroStarItem(heroStarFrame);
            }
        }

        public void SetData(UTileItemClanSelfHero item, UTileClanSelfHero parentList)
        {
            cmd_clan_request_selfhero cmdData = item.data;
            heroIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_CLAN_SELECTREQUESTCARD, 1, 7, cmdData.nHeroID);
            heroStarItem.SetData(cmdData.nHeroID, cmdData.nHeroStar, false, false);

            float fValue = cmdData.nNeedCardCount != 0 ? (float)item.data.nCardCount / item.data.nNeedCardCount : 0.0f;
            Vector2 baseVec2 = cardValue.rectTransform.sizeDelta;
            if (fValue >= 1)
            {
                baseVec2.x = cardBase.rectTransform.sizeDelta.x;
                cardValue.sprite = parentList.cardValueSprites[1];
            }
            else
            {
                baseVec2.x = fValue * cardBase.rectTransform.sizeDelta.x;
                cardValue.sprite = parentList.cardValueSprites[0];
            }
            cardValue.rectTransform.sizeDelta = baseVec2;

            int nStarLvLimit = GameLogicAPI.getHeroStarLvLimit(cmdData.nHeroID);
            if (cmdData.nHeroStar >= nStarLvLimit)
            {
                cardText.text = cmdData.nCardCount.ToString();
                maxBg.gameObject.SetActive(true);
            }
            else
            {
                cardText.text = cmdData.nCardCount.ToString() + "/" + cmdData.nNeedCardCount.ToString();
                maxBg.gameObject.SetActive(false);
            }
        }

        private void SetImageActiveIfValid(Image image)
        {
            if (image.sprite != null)
                image.gameObject.SetActive(true);
            else
                image.gameObject.SetActive(false);
        }

        public void OnDeselect(ListViewItem item)
        {
            highlightIcon.SetActive(false);
        }

        public void OnSelect(ListViewItem item)
        {
            highlightIcon.SetActive(true);
        }
    }
}
