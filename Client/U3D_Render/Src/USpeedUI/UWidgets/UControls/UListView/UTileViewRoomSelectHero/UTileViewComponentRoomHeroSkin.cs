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
using USpeedUI.UExtensions;


namespace USpeedUI.UWidgets
{
    public class UTileViewComponentRoomHeroSkin : ListViewItem
    {
        private UTileViewItemRoomHeroSkin ItemData;
        private UEffectParamBase m_OldEffectParam;
        private UEffectParamBase m_OldEffectParamEX;
        public UTileViewRoomHeroSkin RoomList;
        [SerializeField]
        public Image SkinIconImage;

        [SerializeField]
        public Image SelectFrame;

        [SerializeField]
        public Text SkinProssesText;

        [SerializeField]
        public Text SkinGradeText;

        [SerializeField]
        public GameObject SkinBuyObj;

        [SerializeField]
        public Text SkinNameText;

        [SerializeField]
        public GameObject SkinMaskObj;
        
        public void SetData( UTileViewItemRoomHeroSkin item )
        {
            if (SkinIconImage == null || SelectFrame == null || SkinProssesText == null || SkinGradeText == null  || SkinNameText == null || SkinBuyObj == null)
            {
                return;
            }

            if (item.eRoomState == Stage_ManagerDef.EMatchRoomState.MatchRoomState_Wait)
            {
                SkinMaskObj.SetActive(false);
            }
            else
            {
                if (item.eRoomState == Stage_ManagerDef.EMatchRoomState.MatchRoomState_SelectHero)
                {
                    SkinMaskObj.SetActive(!item.bIsHaveSkin);
                }
            }

            SkinIconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_GAMEWAITINGROOMVIEW, item.nSkinIconId, 6, item.nHeroID);
            SkinIconImage.gameObject.SetActive(SkinIconImage.sprite);
            string[] szWord = new string[] { "经典", "传说", "史诗", "终极" };
            int nSkinGrade = item.nSkinType;
            if (nSkinGrade >= 0 && nSkinGrade < szWord.Length)
                SkinGradeText.text = szWord[nSkinGrade];
            SkinNameText.text = item.szSkinName;

            string szProssesText = "";
            Color prossesColor = Color.white;
            if (item.bIsHaveSkin)
            {
                if (item.dwSkinLimitTime > 0)
                {
                    DateTime calcTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(item.dwSkinLimitTime);
                    calcTime = calcTime.ToLocalTime();

                    TimeSpan ts = calcTime.Subtract(DateTime.Now).Duration();
                    if (ts.Days > 0)
                        szProssesText = ts.Days.ToString() + "天";//+ ts.Hours.ToString() + "小时";
                    else if (ts.Hours > 0)
                        szProssesText = ts.Hours.ToString() + "小时"; //+ ts.Minutes.ToString() + "分钟";
                    else
                        szProssesText = ts.Minutes.ToString() + "分钟";

                    prossesColor = Color.red;

                    SkinBuyObj.SetActive(false);
                }
                else
                {
                    szProssesText = "已拥有";
                    prossesColor = Color.white;
                    SkinBuyObj.SetActive(false);
                }
            }
            else
            {
                szProssesText = "未拥有";
                prossesColor = Color.red;
                SkinBuyObj.SetActive(true);
            }

            SkinProssesText.text = szProssesText;
            SkinProssesText.color = prossesColor;

            string[] colorList = new string[4] { "FFFFFFFF", "F11DF9", "F9BF1D", "20C53B" };
            if (nSkinGrade >= 0 && nSkinGrade < szWord.Length)
            {
                Color32 colorItem = ColorExtension.FromHex(colorList[nSkinGrade]);
                SkinGradeText.color = colorItem;
                SkinNameText.color = colorItem;
            }

            if (item.bSelect)
            {
                SetSelectDownShotEffect();
                RoomList.SelectHeroItem = this;
            }
            else
            {
                DestorySelectDownShotEffect();
            }

            SelectFrame.gameObject.SetActive(item.bSelect);
            ItemData = item;
            //SkinTypeImage.sprite = 
        }

        //public void OnBuyButtonClick()
        //{
        //    if (!ItemData.bIsHaveSkin)
        //    {
        //        int nSellID = PointShopConfig.Instance.GetPointShopSellIDBySkinID(ItemData.nHeroID,ItemData.nSkinId);
        //        if (nSellID > 0)
        //        {
        //            PointShopBuyGoodsInitData uiData = new PointShopBuyGoodsInitData();
        //            uiData.nSellID = nSellID;
        //            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN, uiData);
        //        }
        //    }
        //}

        public override void OnPointerClick(PointerEventData eventData)
        {
            base.OnPointerClick(eventData);
        }

        public override void OnPointerEnter(PointerEventData eventData)
        {
            base.OnPointerEnter(eventData);

            this.gameObject.transform.DOScale(1.2f,0.2f);
            SetSelectOverLightEffect();
        }

        public override void OnPointerExit(PointerEventData eventData)
        {
            base.OnPointerExit(eventData);
            this.gameObject.transform.DOScale(1f, 0.2f);
            DestorySelectOverLightEffect();
        }

        public UTileViewItemRoomHeroSkin GetData()
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
           // param.AttachDelay = 0.1f;

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
