/*******************************************************************
** 文件名:	UTileViewComponentHero
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	
** 日  期:	
** 版  本:	1.0
** 描  述:	
		
********************************************************************/
using UnityEngine;
using UnityEngine.UI;
using System;
using UIWidgets;
using System.Runtime.InteropServices;
using GameLogic;
using USpeedUI.PointShop;
using DataCenter;
using USpeedUI.UEffect;

namespace USpeedUI.UWidgets
{
    public class UTileViewComponentHero:ListViewItem
    {
        [SerializeField]
        public GameObject StarEffectGo; 

        [SerializeField]
        //public Image MaxBg;
        public Image StarGradeBg;

        [SerializeField]
        public Image HeadIcon;

        [SerializeField]
        public Image HaveMask;

        [SerializeField]
        public Image UpGradeMask;

        [SerializeField]
        public Image UpGradeIcon;

        [SerializeField]
        public RectTransform HeroStarFrame;

        [SerializeField]
        public Image weekFree;

        [SerializeField]
        public Text weekFreeDesc;

        [SerializeField]
        public GameObject LockBase;

        [SerializeField]
        public Text LockDesc;

        [SerializeField]
        public Image ExpBase;

        [SerializeField]
        public Image ExpValue;

        [SerializeField]
        public Text ExpText;

        [SerializeField]
        public Text Name;

        [SerializeField]
        public Text Title;

        [SerializeField]
        public Sprite[] expValueSprites;

        [SerializeField]
        public Image FpsIcon;

        private HeroStarItem heroStarItem;

        private UEffectParamBase param;

        protected override void Awake()
        {
            base.Awake();

            if (heroStarItem == null)
            {
                heroStarItem = LogicDataCenter.pointShopDataManager.CreateHeroStarItem(HeroStarFrame);
            }
        }

        public void SetData( UTileViewItemHero item )
        {
            // 基本设置
            HeadIcon.sprite = item.headIcon;
            Name.text = item.stName;
            Title.text = item.stTitle;
            ExpText.text = item.stStarExp;
            heroStarItem.SetData(item.nHeroID, item.nStarLv, false, false);
            weekFreeDesc.text = ULocalizationService.Instance.Get("UIview", "Common", "WeekFree");
            weekFree.gameObject.SetActive(item.bWeekFree);
            UpGradeIcon.gameObject.SetActive(item.bUpGrade);
            UpGradeMask.gameObject.SetActive(item.bUpGrade);
            FpsIcon.gameObject.SetActive(item.bFpsHero);
            // 卡牌设置
            Vector2 baseVec2 = ExpValue.rectTransform.sizeDelta;
            if (item.fExpValue >= 1)
            {
                baseVec2.x = ExpBase.rectTransform.sizeDelta.x;
                ExpValue.sprite = expValueSprites[1];
            }
            else
            {
                baseVec2.x = item.fExpValue * ExpBase.rectTransform.sizeDelta.x;
                ExpValue.sprite = expValueSprites[0];
            }
            ExpValue.rectTransform.sizeDelta = baseVec2;

            // 拥有设置
            if ( item.bHaveHero )
            {
                HaveMask.gameObject.SetActive(false);
                LockBase.SetActive(false);
                heroStarItem.gameObject.SetActive(true);
            }
            else
            {
                HaveMask.gameObject.SetActive(true);
                int nUnlockLevel = GameLogicAPI.getUnlockLevel(item.nHeroID);
                if (nUnlockLevel <= 0)
                {
                    LockDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "ChargeUnlock");
                }
                else if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL) >= nUnlockLevel)
                {
                    LockDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "ChargeUnlock");
                }
                else
                {
                    LockDesc.text = String.Format(ULocalizationService.Instance.Get("UIView", "Common", "LevelUnlock"), nUnlockLevel);
                }
                LockBase.SetActive(true);
                heroStarItem.gameObject.SetActive(false);
            }

            // 星级外框设置
            int nStarFrameID = GameLogicAPI.getHeroStarFrameID(item.nHeroID, item.nStarLv);
            if (nStarFrameID > 0)
            {
                StarGradeBg.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_StaticState, WndID.WND_ID_PLAYERFRAME, nStarFrameID + 10);
                StarGradeBg.gameObject.SetActive(true);
            }
            else
            {
                StarGradeBg.gameObject.SetActive(false);
            }
            // 星级光效设置
            int nStarEffectID = GameLogicAPI.getHeroStarEffectID(item.nHeroID, item.nStarLv);

            if (param != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
                gameObject.name = item.stName;
                param = null;
            }
            if (nStarEffectID > 0)
            {
                if (param == null)
                param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_HeroPreviewFrame_GradeFrame1 + nStarEffectID - 1, _tfAttachParent: StarEffectGo.transform, _bAutoDestroy: false);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);
                gameObject.name = item.stName;

            }
            // 星级其他设置
            if (item.nStarLv >= item.nStarLvLimit)
            {
                Color clr;
                ColorUtility.TryParseHtmlString("#f9bf1d", out clr);
                Name.color = clr;
            }
            else
            {
                Color clr;
                ColorUtility.TryParseHtmlString("#ffffff", out clr);
                Name.color = clr;
            }
        }

        protected override void OnDestroy()
        {
            base.OnDestroy();

            if (param != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
            }
        }
    }
}
