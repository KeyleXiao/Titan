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
using UnityEngine.EventSystems;
using DG.Tweening;
using USpeedUI.UEffect;
using Data.PointShopConfig;


namespace USpeedUI.UWidgets
{
    public class UTileViewComponentRoomHero : ListViewItem
    {
        private UTileViewItemRoomHero ItemData;
        private UEffectParamBase m_OldEffectParam;
        private UEffectParamBase m_OldEffectParamEX;
        [SerializeField]
        public Image HeadIcon;

        [SerializeField]
        public Image Mask;

        [SerializeField]
        public Image SelectFrame;

        [SerializeField]
        public Text PleaseBuyText;

        [SerializeField]
        public Text LockDesc;

        [SerializeField]
        public Image RandomImage;

        [SerializeField]
        public Text PractisedLevelText;

        [SerializeField]
        public Text HeroNameText;

        [SerializeField]
        public Image FreeHero;

        [SerializeField]
        public GameObject BuyObj;

        [SerializeField]
        public Image FpsHero;

        [SerializeField]
        public Image BanIcon;

        [SerializeField]
        public Image BanFrame;

        [SerializeField]
        public GameObject NewHeroGo;

        public void SetData( UTileViewItemRoomHero item )
        {
            HeadIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_GAMEWAITINGROOMVIEW, 1, 6, item.nHeroIconId);
            HeadIcon.gameObject.SetActive(HeadIcon.sprite);
            if (item.eRoomState == Stage_ManagerDef.EMatchRoomState.MatchRoomState_Wait)
            {
                Mask.gameObject.SetActive(false);
                BanFrame.gameObject.SetActive(false);
            }
            else if (item.eRoomState == Stage_ManagerDef.EMatchRoomState.MatchRoomState_SelectHero || item.eRoomState == Stage_ManagerDef.EMatchRoomState.MatchRoomState_ReadyGo)
            {
                Mask.gameObject.SetActive(item.bIsLock || item.bSelect || item.bIsBan);
                BanFrame.gameObject.SetActive(item.bIsBan);
            }
            BanIcon.gameObject.SetActive(item.bIsBan);
            SelectFrame.gameObject.SetActive(item.bSelect);

            int nActorLv = 0;
            if (EntityFactory.MainHeroView != null)
            {
                nActorLv = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            }
            int nUnlockLevel = item.nUnlockLevel;
            if (nUnlockLevel <= 0)
            {
                LockDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "ChargeUnlock");
            }
            else if (nActorLv >= nUnlockLevel)
            {
                LockDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "ChargeUnlock");
            }
            else
            {
                LockDesc.text = String.Format(ULocalizationService.Instance.Get("UIView", "Common", "LevelUnlock"), nUnlockLevel);
            }
            LockDesc.gameObject.transform.parent.gameObject.SetActive(!item.bIsHaveHero && !item.bWeekFree);

            ItemData = item;
            if (ItemData.nSlotId == 0)
            {
                HeadIcon.sprite = RandomImage.sprite;
                HeadIcon.gameObject.SetActive(HeadIcon.sprite ? true : false);
            }
            PractisedLevelText.text = item.nPractisedLevel.ToString();
            PractisedLevelText.gameObject.SetActive(item.nPractisedLevel > 0);
            HeroNameText.gameObject.SetActive(false);
            HeroNameText.text = item.sHeroName;

            if (item.bWeekFree)
            {
                FreeHero.gameObject.SetActive(true);
            }
            else
            {
                FreeHero.gameObject.SetActive(false);
            }

            FpsHero.gameObject.SetActive(item.bFpsHero);

            NewHeroGo.SetActive(item.nIsNew > 0);
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            base.OnPointerClick(eventData);           
        }

        public override void OnPointerEnter(PointerEventData eventData)
        {
            base.OnPointerEnter(eventData);

            this.gameObject.transform.DOScale(1.2f,0.2f);
            SetSelectOverLightEffect();
            HeroNameText.gameObject.SetActive(true);
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            base.OnPointerExit(eventData);
            this.gameObject.transform.DOScale(1f, 0.2f);
            DestorySelectOverLightEffect();

            HeroNameText.gameObject.SetActive(SelectFrame.gameObject.activeSelf);
        }

        public UTileViewItemRoomHero GetData()
        {
            return ItemData;
        }

        public void SetSelectOverLightEffect()
        {
            if (m_OldEffectParam != null)
            {
                return;
            }

            UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_UWaitingRoom_SelectOverLight, _tfAttachParent: this.transform);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

            m_OldEffectParam = param;
            UEffectPrefabParam paramPrefab = (UEffectPrefabParam)param;
            paramPrefab.EffectGameObject.transform.SetAsFirstSibling();
             
        }

        public void DestorySelectOverLightEffect()
        {
            if (m_OldEffectParam == null)
            {
                return;
            }

            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_OldEffectParam);
            m_OldEffectParam = null;
        }


        public void SetSelectDownShotEffect()
        {
            if (m_OldEffectParamEX != null)
            {
                return;
            }

            UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_UWaitingRoom_SelectDownLight, _tfAttachParent: this.transform);

            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

            m_OldEffectParamEX = param;
            UEffectPrefabParam paramPrefab = (UEffectPrefabParam)param;
            paramPrefab.EffectGameObject.transform.localPosition = new Vector3(0, 0, -100);
        }

        public void DestorySelectDownShotEffect()
        {
            if (m_OldEffectParamEX == null) 
            {
                return;
            }

            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_OldEffectParamEX);
            m_OldEffectParamEX = null;
        }
    }
}
