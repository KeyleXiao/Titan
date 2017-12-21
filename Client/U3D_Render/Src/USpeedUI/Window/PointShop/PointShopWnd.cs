using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using Data.PointShopConfig;
using UnityEngine.EventSystems;
using Data.ActorPrizeConfig;
using War;
using Effect;
using USpeedUI.UEffect;
using ASpeedGame.Data.PersonModelTransFormInfo;
using ASpeedGame.Data.RunePage;
using DataCenter;
using GameLogic;
using U3D_Render.Common;
using UIWidgets;
using System.Globalization;
using System.Web;
using System.Runtime.InteropServices;

namespace USpeedUI.PointShop
{
    public class PointShopWnd : UIPopupWnd<PointShopWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_POINTSHOP;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "PointShop/PointShop";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;
            m_bForbid3DSound = true;

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_POINTSHOP_OPEN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_POINTSHOP_UPDATECARDLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            //UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_POINTSHOP_OPEN_PAY, this);
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_POINTSHOP_OPEN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_POINTSHOP_UPDATECARDLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            //UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_POINTSHOP_OPEN_PAY, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_POINTSHOP_OPEN:
                    {
                        PointShopOpenData uiData = msgData as PointShopOpenData;
                        SetVisible(true);
                        if (m_wndView != null)
                            m_wndView.SetPageBtnSelect(uiData.nPageType);
                    }
                    break;
                case WndMsgID.WND_MSG_POINTSHOP_UPDATECARDLIST:
                    {
                        if (m_wndView != null)
                            m_wndView.UpdateCardList();
                    }
                    break;
                case WndMsgID.WND_MSG_HEROPREVIEWFRAME_GETHEROINFO:
                    {
                        if (m_wndView != null)
                            m_wndView.OnHeroInfoUpdate();
                    }
                    break;
                //case WndMsgID.WND_MSG_POINTSHOP_OPEN_PAY:
                //    {
                //        SetVisible(true);
                //        if (m_wndView != null)
                //            m_wndView.SetPageBtnSelect((int)PointShopPageBtnType.PageBtn_Pay);
                //    }
                //    break;
                default:
                    break;
            }
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);

            //if (m_wndView == null)
            //    return;
            //if (_bVisible)
            //    m_wndView.PlayOpenAnim();
            //else
            //    m_wndView.PlayCloseAnim();

            UCommonPopupWndVisible msgData = new UCommonPopupWndVisible();
            msgData.msgID = (int)WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE;
            msgData.nWndID = (int)GetID();
            msgData.isVisible = _bVisible;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE, msgData);
        }
    }

    #region 通用组件
    enum FrameSpriteIndex
    {
        HeroCard,
        HeroXp,

        Max,
    }
    public class PrizeIconItem : MonoBehaviour
    {
        public Image baseImage;
        public Image frameImage;
        public Sprite[] frameSprites;

        const int RAND_CARD_FIRST_INDEX = 901;

        public void SetContainer(RectTransform container)
        {
            if (container == null)
                return;
            transform.SetParent(container, false);
        }

        public void SetPrizeID(int nPrizeID)
        {
            if (nPrizeID <= 0)
            {
                SetVisible(false);
                return;
            }

            //SSchemeActorPrizeConfig prizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(nPrizeID);
            SSchemeActorPrizeConfig prizeConfig = null;
            if (nPrizeID >= RAND_CARD_FIRST_INDEX && nPrizeID < LogicDataCenter.prizeChestDataManager.RandCardIndex)
            {
                prizeConfig = LogicDataCenter.prizeChestDataManager.RandCardResult[nPrizeID];
            }
            else
            {
                prizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(nPrizeID);
            }
            if (prizeConfig == null)
            {
                SetVisible(false);
                return;
            }

            frameImage.gameObject.SetActive(false);
            baseImage.sprite = null;
            if (prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HERO)
            {
                baseImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_POINTSHOP, 1, 3, prizeConfig.nPrizeParam[0]);
            }
            else if (prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROCARD)
            {
                baseImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_POINTSHOP, 1, 7, prizeConfig.nPrizeParam[0]);
                frameImage.sprite = frameSprites[(int)FrameSpriteIndex.HeroCard];
                frameImage.gameObject.SetActive(true);

            }
            else if (prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_RUNE)
            {
                SSchemeRuneConfig runeConfig = RunePage.Instance.allRunePageTable[prizeConfig.nPrizeParam[0]] as SSchemeRuneConfig;
                if (runeConfig != null)
                {
                    baseImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Rune, WndID.WND_ID_POINTSHOP, runeConfig.nRuneInlayType, runeConfig.nRuneIconId);
                }
            }
            else if (prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROSKIN)
            {
                SkinInfoScheme skinConfig = new SkinInfoScheme();
                if (GameLogicAPI.GetHeroSkinInfo(prizeConfig.nPrizeParam[1], ref skinConfig))
                {
                    baseImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_POINTSHOP, skinConfig.nSkinIconID, 3, prizeConfig.nPrizeParam[0]);
                }
            }
            else if (prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROXPSKILL)
            {
                baseImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_POINTSHOP, prizeConfig.nPrizeType, prizeConfig.nPrizeIcon);
                frameImage.sprite = frameSprites[(int)FrameSpriteIndex.HeroXp];
                frameImage.gameObject.SetActive(true);
            }
            else
            {
                baseImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_POINTSHOP, prizeConfig.nPrizeType, prizeConfig.nPrizeIcon);
            }
            //baseImage.SetNativeSize();
            //frameImage.SetNativeSize();
        }

        public void SetVisible(bool bVisible)
        {
            gameObject.SetActive(bVisible);
        }
    }

    public class ShopMoneyItem : MonoBehaviour
    {
        public HorizontalLayoutGroup layout;
        public GameObject pointContainer;
        public Text pointText;
        public GameObject splitContainer;
        public GameObject moneyContainer;
        public Text moneyText;

        public void Init(RectTransform parent)
        {
            transform.SetParent(parent, false);
            RectTransform selfTrans = transform as RectTransform;
            selfTrans.sizeDelta = parent.sizeDelta;
        }

        public void SetData(int nPointPrice, int nMoneyPrice)
        {
            splitContainer.SetActive(true);

            if (nPointPrice > 0)
            {
                pointContainer.SetActive(true);
                pointText.text = nPointPrice.ToString();
            }
            else
            {
                pointContainer.SetActive(false);
                splitContainer.SetActive(false);
            }

            if (nMoneyPrice > 0)
            {
                moneyContainer.SetActive(true);
                moneyText.text = nMoneyPrice.ToString();
            }
            else
            {
                moneyContainer.SetActive(false);
                splitContainer.SetActive(false);
            }
        }

        public void SetSpace(int nSpace)
        {
            layout.spacing = nSpace;
        }

        public void SetAlign(TextAnchor alignment)
        {
            layout.childAlignment = alignment;
        }
    }

    public class SquareHighlightItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler
    {
        public GameObject highlightEffect;

        public void Init(RectTransform parent)
        {
            transform.SetParent(parent, false);
            RectTransform selfTrans = transform as RectTransform;
            selfTrans.sizeDelta = parent.sizeDelta;
        }

        public virtual void OnPointerEnter(PointerEventData eventData)
        {
            highlightEffect.SetActive(true);
        }

        public virtual void OnPointerExit(PointerEventData eventData)
        {
            highlightEffect.SetActive(false);
        }

        public void OnEnable()
        {
            highlightEffect.SetActive(false);
        }
    }

    public class HeroStarItem : MonoBehaviour
    {
        public Image StarBase;
        public Image StarIcon;
        public Text StarLevel;

        public void Init(RectTransform parent)
        {
            transform.SetParent(parent, false);
            RectTransform selfTrans = transform as RectTransform;
            selfTrans.sizeDelta = parent.sizeDelta;
        }

        public void SetData(int nHeroID, int nStarLv, bool bShowBase, bool bShowLv)
        {
            // 关闭该功能
            StarBase.gameObject.SetActive(false);
            StarIcon.gameObject.SetActive(false);
            StarLevel.gameObject.SetActive(false);
            return;

            //if (nHeroID <= 0 || nStarLv <= 0)
            //{
            //    StarBase.gameObject.SetActive(false);
            //    StarIcon.gameObject.SetActive(false);
            //    StarLevel.gameObject.SetActive(false);
            //    return;
            //}

            //StarBase.gameObject.SetActive(bShowBase);

            //int nStarIconID = GameLogicAPI.getHeroStarIconID(nHeroID, nStarLv);
            //if (nStarIconID >= 0)
            //{
            //    StarIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeroStarIcon, "HeroStarIcon" + nStarIconID, WndID.WND_ID_POINTSHOP);
            //    StarIcon.gameObject.SetActive(true);
            //}
            //else
            //{
            //    StarIcon.gameObject.SetActive(false);
            //}

            //StarLevel.text = "英雄评级: " + nStarLv.ToString();
            //StarLevel.gameObject.SetActive(bShowLv);
        }

    }

    // 排序物件类型
    public enum EMSortType
    {
        SORT_TYPE_NONE,         // 不排序
        SORT_TYPE_INCREASE,     // 递增
        SORT_TYPE_DECREASE,     // 递减

        SORT_TYPE_MAX
    }

    public class PointShopSortItem : MonoBehaviour, IPointerDownHandler
    {
        public Image iconImage;
        public Text descText;

        private int m_nSortType;
        public int SortType { get { return m_nSortType; } }

        public delegate void ClickDelegate(PointShopSortItem item);
        public ClickDelegate OnClickItem;

        private Comparison<SSchemePointShopGoodsConfig> m_comparison;
        public Comparison<SSchemePointShopGoodsConfig> Comparison { get { return m_comparison; } }

        public void Init(RectTransform container, String strDesc, Comparison<SSchemePointShopGoodsConfig> comparison)
        {
            transform.SetParent(container, false);
            descText.text = strDesc;
            m_comparison = comparison;
            ReflushIcon();
        }

        public virtual void OnPointerDown(PointerEventData eventData)
        {
            if (++m_nSortType >= (int)EMSortType.SORT_TYPE_MAX)
            {
                m_nSortType = 0;
            }
            ReflushIcon();

            if (OnClickItem != null)
                OnClickItem(this);
        }

        public void Clear()
        {
            m_nSortType = 0;
            ReflushIcon();
        }

        void ReflushIcon()
        {
            if (m_nSortType == 0)
            {
                iconImage.enabled = false;
            }
            else
            {
                iconImage.enabled = true;
                iconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PointShop, "SortType" + m_nSortType, WndID.WND_ID_POINTSHOP);
                iconImage.SetNativeSize();
            }
        }
    }

    #endregion

    // 商城按钮分页类型
    enum PointShopPageBtnType
    {
        PageBtn_AD,
        PageBtn_Hero,
        PageBtn_Wing,
        PageBtn_Card,
        PageBtn_Rune,
        PageBtn_Chest,
        PageBtn_Xp,
        PageBtn_GemStone,
        PageBtn_Other,
        PageBtn_Pay,

        PageBtn_Max
    }

    public abstract class PointShopPageItem : MonoBehaviour
    {
        protected PointShopWndView m_wndView;
        public PointShopWndView WndView { get { return m_wndView; } }
        protected int m_nCurPageType;

        public virtual void Init(PointShopWndView wndView)
        {
            m_wndView = wndView;
        }

        public virtual void SetVisible(bool bVisible)
        {
            gameObject.SetActive(bVisible);

            if (bVisible)
            {
                OnReflush();
            }
        }

        public virtual bool IsVisible()
        {
            return gameObject.activeSelf;
        }

        public virtual void SetCurPageType(int nType)
        {
            m_nCurPageType = nType;
            // 暂时只打开5个，勋章、终结技、水晶、其他、充值
            if (m_nCurPageType == (int)PointShopPageBtnType.PageBtn_AD)
                m_nCurPageType = (int)PointShopPageBtnType.PageBtn_Card;
            else if (m_nCurPageType == (int)PointShopPageBtnType.PageBtn_Hero)
                m_nCurPageType = (int)PointShopPageBtnType.PageBtn_Xp;
            else if (m_nCurPageType == (int)PointShopPageBtnType.PageBtn_Wing)
                m_nCurPageType = (int)PointShopPageBtnType.PageBtn_GemStone;
            else if (m_nCurPageType == (int)PointShopPageBtnType.PageBtn_Card)
                m_nCurPageType = (int)PointShopPageBtnType.PageBtn_Other;
            else if (m_nCurPageType == (int)PointShopPageBtnType.PageBtn_Rune)
                m_nCurPageType = (int)PointShopPageBtnType.PageBtn_Pay;

            if (IsVisible())
            {
                OnReflush();
            }
        }

        public abstract void OnReflush();
    }

    #region 广告页面
    enum PointShopADLocType
    {
        MainPic = 1,
        SubPic,
        BestSell,
        Recommend,
    }

    public abstract class PointShopADItem : MonoBehaviour
    {
        public Text titleText;

        protected PointShopADPageItem m_parent;

        public abstract void SetData(PointShopADPageItem parent, SSchemePointShopADConfig config);
    }

    public class PointShopADMainPicItem : PointShopADItem
    {
        public Image picImage;
        public UButtonGroup pageBtnGroup;
        public GameObject pageBtnTemplate;

        private List<UButtonGroupItem> m_btnList = new List<UButtonGroupItem>();
        private List<SSchemePointShopADConfig> m_configList = new List<SSchemePointShopADConfig>();

        public override void SetData(PointShopADPageItem parent, SSchemePointShopADConfig config)
        {
            m_parent = parent;

            GameObject newObj = ResNode.InstantiateRes(pageBtnTemplate);
            if (newObj)
            {
                newObj.transform.SetParent(pageBtnGroup.transform, false);
                newObj.SetActive(true);
                UButtonGroupItem item = newObj.GetComponent<UButtonGroupItem>();
                Text[] pageText = newObj.GetComponentsInChildren<Text>(true);
                if (pageText != null)
                {
                    pageText[0].text = (m_btnList.Count + 1).ToString();
                }
                item.onSelectedChanged.AddListener(OnADPageClick);
                item.Group = pageBtnGroup;
                pageBtnGroup.RegisterButton(item);

                m_btnList.Add(item);
                m_configList.Add(config);
            }

            if (m_btnList.Count == 1)
            {
                m_btnList[0].Selected = true;
                OnADPageClick(true);
            }
        }

        void OnADPageClick(bool bActive)
        {
            if (!bActive)
                return;

            int nIndex = pageBtnGroup.SelectedIndex;
            if (nIndex < 0 || nIndex >= m_configList.Count)
                return;

            SSchemePointShopADConfig config = m_configList[nIndex];
            titleText.text = config.strTitle;
            picImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PointShop, config.strPicName, WndID.WND_ID_POINTSHOP);
        }

        public void OnPointerDown()
        {
            int nIndex = pageBtnGroup.SelectedIndex;
            if (nIndex < 0 || nIndex >= m_configList.Count)
                return;

            SSchemePointShopADConfig config = m_configList[nIndex];
            m_parent.WndView.SetSearchList(config.nSellIDList);
            m_parent.WndView.ChangeADSearchPage();
        }
    }

    public class PointShopADSubPicItem : PointShopADItem, IPointerDownHandler
    {
        public Text descText;
        public Image picImage;

        private SSchemePointShopADConfig m_config;

        public override void SetData(PointShopADPageItem parent, SSchemePointShopADConfig config)
        {
            m_parent = parent;
            m_config = config;

            titleText.text = config.strTitle;
            descText.text = config.strDesc;
            picImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PointShop, config.strPicName, WndID.WND_ID_POINTSHOP);
        }

        public virtual void OnPointerDown(PointerEventData eventData)
        {
            m_parent.WndView.SetSearchList(m_config.nSellIDList);
            m_parent.WndView.ChangeADSearchPage();
        }
    }

    public class PointShopADSingleGoodsItem : PointShopADItem, IPointerDownHandler
    {
        public RectTransform goodsIconFrame;
        public RectTransform priceFrame;

        private PrizeIconItem goodsIcon;
        private ShopMoneyItem moneyItem;

        private SSchemePointShopADConfig m_config;

        public override void SetData(PointShopADPageItem parent, SSchemePointShopADConfig config)
        {
            m_parent = parent;
            m_config = config;

            titleText.text = config.strTitle;

            if (config.nSellIDList != null && config.nSellIDList.Count > 0)
            {
                int nSellID = config.nSellIDList[0];
                SSchemePointShopGoodsConfig goodsConfig = PointShopConfig.Instance.GetPointShopGoodsConfig(nSellID);
                if (goodsConfig != null)
                {
                    goodsIcon = LogicDataCenter.pointShopDataManager.CreatePrizeIconItem(goodsIconFrame);
                    goodsIcon.SetPrizeID(goodsConfig.nPrizeID);
                    moneyItem = LogicDataCenter.pointShopDataManager.CreateShopMoneyItem(priceFrame);
                    moneyItem.SetAlign(TextAnchor.MiddleLeft);
                    moneyItem.SetData(goodsConfig.nPointPrice, goodsConfig.nMoneyPrice);
                }
            }
        }

        public virtual void OnPointerDown(PointerEventData eventData)
        {
            m_parent.WndView.SetSearchList(m_config.nSellIDList);
            m_parent.WndView.ChangeADSearchPage();
        }
    }

    public class PointShopADPageItem : PointShopPageItem
    {
        public PointShopADMainPicItem mainPicItem;
        public GameObject subPicItemTemplate;
        public Text topSellDesc;
        public GameObject topSellItemTemplate;
        public Text recommendDesc;
        public GameObject recommendItemTemplate;

        private PointShopADSubPicItem[] subPicItem;
        private PointShopADSingleGoodsItem[] topSellItem;
        private PointShopADSingleGoodsItem[] recommendItem;

        public override void Init(PointShopWndView wndView)
        {
            base.Init(wndView);

            subPicItem = new PointShopADSubPicItem[3];
            subPicItem[0] = subPicItemTemplate.GetComponent<PointShopADSubPicItem>();
            LogicDataCenter.pointShopDataManager.CreateSquareHighlightItem(subPicItem[0].transform);
            Vector3 subPicItemBasePos = subPicItemTemplate.transform.localPosition;
            for (int i = 1; i < subPicItem.Length; ++i)
            {
                GameObject newObj = ResNode.InstantiateRes(subPicItemTemplate);
                if (newObj)
                {
                    newObj.transform.SetParent(subPicItemTemplate.transform.parent, false);
                    newObj.transform.localPosition = new Vector3(subPicItemBasePos.x + i * 346, subPicItemBasePos.y, subPicItemBasePos.z);
                    subPicItem[i] = newObj.GetComponent<PointShopADSubPicItem>();
                }
            }

            topSellDesc.text = ULocalizationService.Instance.Get("UIView", "PointShop", "ADTopSellDesc");
            topSellItem = new PointShopADSingleGoodsItem[3];
            topSellItem[0] = topSellItemTemplate.GetComponent<PointShopADSingleGoodsItem>();
            LogicDataCenter.pointShopDataManager.CreateSquareHighlightItem(topSellItem[0].transform);
            Vector3 topSellItemBasePos = topSellItemTemplate.transform.localPosition;
            for (int i = 1; i < topSellItem.Length; ++i)
            {
                GameObject newObj = ResNode.InstantiateRes(topSellItemTemplate);
                if (newObj)
                {
                    newObj.transform.SetParent(topSellItemTemplate.transform.parent, false);
                    newObj.transform.localPosition = new Vector3(topSellItemBasePos.x, topSellItemBasePos.y - i * 78, topSellItemBasePos.z);
                    topSellItem[i] = newObj.GetComponent<PointShopADSingleGoodsItem>();
                    LogicDataCenter.pointShopDataManager.CreateSquareHighlightItem(newObj.transform);
                }
            }

            recommendDesc.text = ULocalizationService.Instance.Get("UIView", "PointShop", "ADRecommendDesc");
            recommendItem = new PointShopADSingleGoodsItem[4];
            recommendItem[0] = recommendItemTemplate.GetComponent<PointShopADSingleGoodsItem>();
            LogicDataCenter.pointShopDataManager.CreateSquareHighlightItem(recommendItem[0].transform);
            Vector3 recommendItemBasePos = recommendItemTemplate.transform.localPosition;
            for (int i = 1; i < recommendItem.Length; ++i)
            {
                GameObject newObj = ResNode.InstantiateRes(recommendItemTemplate);
                if (newObj)
                {
                    newObj.transform.SetParent(recommendItemTemplate.transform.parent, false);
                    newObj.transform.localPosition = new Vector3(recommendItemBasePos.x, recommendItemBasePos.y - i * 78, recommendItemBasePos.z);
                    recommendItem[i] = newObj.GetComponent<PointShopADSingleGoodsItem>();
                }
            }

            // 配合buttongroup的bug，不active就设不了东西=_=
            gameObject.SetActive(true);

            int nSubPicIndex = 0;
            int nBestSellIndex = 0;
            int nRecommendIndex = 0;
            List<SSchemePointShopADConfig> adList = PointShopConfig.Instance.GetADConfig();
            foreach (var config in adList)
            {
                switch (config.nLocType)
                {
                    case (int)PointShopADLocType.MainPic:
                        {
                            mainPicItem.SetData(this, config);
                        }
                        break;
                    case (int)PointShopADLocType.SubPic:
                        {
                            if (nSubPicIndex < subPicItem.Length)
                            {
                                subPicItem[nSubPicIndex++].SetData(this, config);
                            }
                        }
                        break;
                    case (int)PointShopADLocType.BestSell:
                        {
                            if (nBestSellIndex < topSellItem.Length)
                            {
                                topSellItem[nBestSellIndex++].SetData(this, config);
                            }
                        }
                        break;
                    case (int)PointShopADLocType.Recommend:
                        {
                            if (nRecommendIndex < recommendItem.Length)
                            {
                                recommendItem[nRecommendIndex++].SetData(this, config);
                            }
                        }
                        break;
                }
            }

            gameObject.SetActive(false);
        }

        public override void OnReflush()
        {
        }
    }
    #endregion

    #region 英雄页面
    public class PointShopHeroPageHeroItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler
    {
        public Image heroIcon;
        public GameObject highlightEffect;
        public Text heroName;
        public RectTransform priceFrame;
        public RectTransform plusIconFrame1;
        public RectTransform plusIconFrame2;
        public Text cannotBuyHeroDesc;

        private PointShopHeroPageItem m_parent;
        private ShopMoneyItem moneyItem;
        private int m_nSellID;
        public int SellID { get { return m_nSellID; } }
        private int m_nHeroID;
        public int HeroID { get { return m_nHeroID; } }
        private bool m_bSelected;

        public void SetParent(PointShopHeroPageItem parent)
        {
            m_parent = parent;

            moneyItem = LogicDataCenter.pointShopDataManager.CreateShopMoneyItem(priceFrame);
            moneyItem.SetSpace(0);
        }

        public void SetHeroInfo(SSchemePointShopGoodsConfig heroInfo)
        {
            if (heroInfo == null)
                return;

            SSchemeActorPrizeConfig prizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(heroInfo.nPrizeID);
            if (prizeConfig == null)
                return;
            if (prizeConfig.nPrizeType != (int)EMActorPrizeType.ACTOR_PRIZE_HERO)
            {
                Debug.LogError("商城卖的英雄分类物品对应的奖励ID不是英雄 PointShopGoods.csv sellID=" + heroInfo.nSellID);
                return;
            }

            m_nSellID = heroInfo.nSellID;
            m_nHeroID = prizeConfig.nPrizeParam[0];
            heroIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_POINTSHOP, 1, 7, m_nHeroID);

            PersonSkinModelItem modelData = new PersonSkinModelItem();
            if (GameLogicAPI.GetPersonModel((UInt16)m_nHeroID, ref modelData))
            {
                heroName.text = modelData.szHeroName;
            }

            ActorHeroInfo actorHeroInfo = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
            cmd_entity_hero_info heroData;
            bool bCanBuyHero = true;
            if (actorHeroInfo.dicEntityHeroInfo.TryGetValue(m_nHeroID, out heroData))
            {
                if (heroData.bIsBuy > 0)
                    bCanBuyHero = false;
            }

            if (bCanBuyHero)
            {
                moneyItem.SetData(heroInfo.nPointPrice, heroInfo.nMoneyPrice);
                cannotBuyHeroDesc.text = "";
                LogicDataCenter.pointShopDataManager.FillGoodsPlusIcon(heroInfo, 0.69f, plusIconFrame1, plusIconFrame2);
            }
            else
            {
                moneyItem.SetData(0, 0);
                cannotBuyHeroDesc.text = ULocalizationService.Instance.Get("UIView", "PointShop", "HeroAlreadyHave");
                LogicDataCenter.pointShopDataManager.FillGoodsPlusIcon(null, 0.69f, plusIconFrame1, plusIconFrame2);
            }
        }

        public void OnClick()
        {
            if (m_parent)
            {
                m_parent.OnSelectHero(m_nSellID, m_nHeroID);
            }
        }

        public void SetSelected(bool bSelect)
        {
            m_bSelected = bSelect;

            if (bSelect)
                highlightEffect.SetActive(true);
            else
                highlightEffect.SetActive(false);
        }

        public virtual void OnPointerEnter(PointerEventData eventData)
        {
            if (!m_bSelected)
                highlightEffect.SetActive(true);
        }

        public virtual void OnPointerExit(PointerEventData eventData)
        {
            if (!m_bSelected)
                highlightEffect.SetActive(false);
        }
    }

    public class PointShopHeroTypeFilterItem : MonoBehaviour
    {
        public Image baseImage;
        public UButtonGroupItem button;
        public Text btnName;

        public void Init(String strName, int nIndex, UButtonGroup btnGroup)
        {
            btnName.text = strName;
            baseImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PointShop, "HeroType_" + nIndex + "_1", WndID.WND_ID_POINTSHOP);
            SpriteState state = button.spriteState;
            state.highlightedSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PointShop, "HeroType_" + nIndex + "_2", WndID.WND_ID_POINTSHOP);
            state.pressedSprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PointShop, "HeroType_" + nIndex + "_3", WndID.WND_ID_POINTSHOP);
            button.spriteState = state;
            button.Group = btnGroup;

            if (nIndex == 0)
            {
                button.Selected = true;
            }
        }
    }

    public class PointShopHeroPageDragHeroTool : MonoBehaviour, IBeginDragHandler, IDragHandler, IEndDragHandler
    {
        private PointShopHeroPageItem m_parent;
        private bool m_draging;
        private Vector3 m_lastPos;

        public void SetParent(PointShopHeroPageItem parent)
        {
            m_parent = parent;
        }

        public void Update()
        {
            float fMouseScrollValue = InputManager.GetAxis(InputAxisSet.Axis_MouseScrollWheel);
            if (fMouseScrollValue != 0f)
            {
                m_parent.ScaleHeroModel(fMouseScrollValue * 0.2f);
            }
        }

        // IBeginDragHandler
        public virtual void OnBeginDrag(PointerEventData eventData)
        {
            if (m_draging == false && eventData.button == PointerEventData.InputButton.Left)
            {
                m_lastPos = eventData.position;
                m_draging = true;
            }
        }

        // IDragHandler
        public virtual void OnDrag(PointerEventData eventData)
        {
            if (m_draging && m_parent != null)
            {
                float fDeltaX = eventData.position.x -  m_lastPos.x;
                m_parent.RotateHeroModel(-fDeltaX * 0.4f);
                m_lastPos = eventData.position;
            }
        }

        // IEndDragHandler
        public virtual void OnEndDrag(PointerEventData eventData)
        {
            if (m_draging)
            {
                m_draging = false;
            }
        }
    }

    public class PointShopHeroPageSkinItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler
    {
        public Image skinIcon;
        public GameObject highlightEffect;
        public Text skinName;
        public Text possessDesc;
        public RectTransform priceFrame;
        public RectTransform plusIconFrame1;
        public RectTransform plusIconFrame2;
        
        private PointShopHeroPageItem m_parent;
        private ShopMoneyItem moneyItem;
        private int m_nSellID;
        public int SellID { get { return m_nSellID; } }
        private int m_nSkinID;
        public int SkinID { get { return m_nSkinID; } }
        private bool m_bSelected;

        public void SetParent(PointShopHeroPageItem parent)
        {
            m_parent = parent;

            moneyItem = LogicDataCenter.pointShopDataManager.CreateShopMoneyItem(priceFrame);
            moneyItem.SetSpace(0);
        }

        public void SetSkinInfo(SSchemePointShopGoodsConfig skinInfo)
        {
            if (skinInfo == null)
                return;

            SSchemeActorPrizeConfig prizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(skinInfo.nPrizeID);
            if (prizeConfig == null)
                return;
            if (prizeConfig.nPrizeType != (int)EMActorPrizeType.ACTOR_PRIZE_HEROSKIN)
            {
                Debug.LogError("商城卖的皮肤分类物品对应的奖励ID不是皮肤 错误脚本PointShopGoods.csv sellID = " + skinInfo.nSellID);
                return;
            }

            // 参数0,1分别为 英雄ID和皮肤ID
            m_nSellID = skinInfo.nSellID;
            m_nSkinID = prizeConfig.nPrizeParam[1];
            int nHeroID = prizeConfig.nPrizeParam[0];
            skinIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_POINTSHOP, 1, 7, nHeroID);

            SkinInfoScheme skinConfig = new SkinInfoScheme();
            if (!GameLogicAPI.GetHeroSkinInfo(m_nSkinID, ref skinConfig))
                return;

            skinName.text = skinConfig.szSkinName;

            bool bHaveSkin = false;
            cmd_Hero_Skin_Info cmdSkinInfo = new cmd_Hero_Skin_Info();
            if (LogicDataCenter.heroSkinDataManager.GetHeroSkinInfo(nHeroID, m_nSkinID, ref cmdSkinInfo))
                bHaveSkin = true;
            else
                bHaveSkin = skinConfig.nSkinLevel == (int)SKIN_LEVEL_TYPE.ESLT_NORMAL ? true : false;

            if (!bHaveSkin)
            {
                moneyItem.SetData(skinInfo.nPointPrice, skinInfo.nMoneyPrice);
                moneyItem.gameObject.SetActive(true);
                possessDesc.gameObject.SetActive(false);
                LogicDataCenter.pointShopDataManager.FillGoodsPlusIcon(skinInfo, 0.69f, plusIconFrame1, plusIconFrame2);
            }
            else
            {
                possessDesc.text = ULocalizationService.Instance.Get("UIView", "PointShop", "SkinAlreadyHave");
                moneyItem.gameObject.SetActive(false);
                possessDesc.gameObject.SetActive(true);
                LogicDataCenter.pointShopDataManager.FillGoodsPlusIcon(null, 0.69f, plusIconFrame1, plusIconFrame2);
            }
        }

        public void OnClick()
        {
            if (m_parent)
            {
                m_parent.OnSelectSkin(m_nSellID, m_nSkinID);
            }
        }

        public void SetSelected(bool bSelect)
        {
            m_bSelected = bSelect;

            if (bSelect)
                highlightEffect.SetActive(true);
            else
                highlightEffect.SetActive(false);
        }

        public virtual void OnPointerEnter(PointerEventData eventData)
        {
            if (!m_bSelected)
                highlightEffect.SetActive(true);
        }

        public virtual void OnPointerExit(PointerEventData eventData)
        {
            if (!m_bSelected)
                highlightEffect.SetActive(false);
        }
    }

    public class PointShopHeroPageItem : PointShopPageItem
    {
        public Text buyPriceText;
        public Text buyButtonText;
        public UButtonGroup heroTypeFilterGroup;
        public GameObject heroTypeFilterBtnTemplate;
        public GameObject singleHeroItemTemplate;
        public RectTransform heroItemFrame;
        public Text heroPageText;
        public Button heroPagePrevBtn;
        public Button heroPageNextBtn;
        public Dropdown heroFilterDropdown;
        public InputField searchHeroInputField;
        public Text searchHeroPlaceHolder;
        public RectTransform heroSortFrame;
        public Dropdown skinFilterDropdown;
        public Button buyBtn;
        public Transform meshRenderNode;
        public PointShopHeroPageDragHeroTool heroRotate;

        public GameObject singleSkinItemTemplate;
        public RectTransform skinItemFrame;
        public Text skinPageText;
        public Button skinPagePrevBtn;
        public Button skinPageNextBtn;

        private PointShopHeroTypeFilterItem[] m_heroTypeFilterItemList;
        private PointShopSortItem[] m_heroSortItemList;
        private PointShopHeroPageHeroItem[] m_heroItem;
        private PointShopHeroPageSkinItem[] m_skinItem;
        private const int m_nSingleHeroPageCount = 8;
        private const int m_nSingleSkinPageCount = 8;
        private int m_nCurSkinPage = 0;
        private int m_nCurHeroPage = 0;
        private int m_nCurSelHero = -1;
        private int m_nCurSelSkin = -1;
        private int m_nCurHeroSellID = -1;
        private int m_nCurSkinSellID = -1;
        private SSchemePointShopGoodsConfig m_curHeroSellConfig;
        private SSchemePointShopGoodsConfig m_curSkinSellConfig;

        private int m_nLastSkinId = -1;
        private SkinInstance m_LastSkinObject;

        public override void Init(PointShopWndView wndView)
        {
            base.Init(wndView);

            meshRenderNode.gameObject.SetActive(true);
            // 配合buttongroup的bug，不active就设不了东西=_=
            gameObject.SetActive(true);

            buyButtonText.text = ULocalizationService.Instance.Get("UIView", "PointShop", "BuyBtn");
            searchHeroPlaceHolder.text = ULocalizationService.Instance.Get("UIView", "PointShop", "HeroSearchField");

            String[] strHeroTypeFilter = ULocalizationService.Instance.Get("UIView", "PointShop", "HeroTypeFilter").Split(';');
            m_heroTypeFilterItemList = new PointShopHeroTypeFilterItem[strHeroTypeFilter.Length];
            for (int i = 0; i < strHeroTypeFilter.Length; ++i)
            {
                GameObject heroTypeBtnObj = ResNode.InstantiateRes(heroTypeFilterBtnTemplate);
                if (heroTypeBtnObj)
                {
                    RectTransform itemTrans = heroTypeBtnObj.transform as RectTransform;
                    itemTrans.SetParent(heroTypeFilterGroup.transform, false);
                    heroTypeBtnObj.SetActive(true);

                    m_heroTypeFilterItemList[i] = heroTypeBtnObj.GetComponent<PointShopHeroTypeFilterItem>();
                    if (m_heroTypeFilterItemList[i])
                    {
                        m_heroTypeFilterItemList[i].Init(strHeroTypeFilter[i], i, heroTypeFilterGroup);
                        m_heroTypeFilterItemList[i].button.onSelectedChanged.AddListener(OnFilterChanged);
                    }
                }
            }

            String[] strHeroSortBtn = ULocalizationService.Instance.Get("UIView", "PointShop", "HeroSortBtn").Split(';');
            m_heroSortItemList = new PointShopSortItem[strHeroSortBtn.Length];
            for (int i = 0; i < strHeroSortBtn.Length; ++i)
            {
                if (i == 0)
                {
                    // 名称排序
                    // 直接取系统语言总是返回en-US。这里先写死取中文区域了，以后谁有空看下吧
                    // CultureInfo cult = System.Globalization.CultureInfo.InstalledUICulture;
                    m_heroSortItemList[i] = LogicDataCenter.pointShopDataManager.CreateSingleSortBtnItem(heroSortFrame, strHeroSortBtn[i], (x, y) => { CultureInfo cult = new CultureInfo("zh-CN"); return cult.CompareInfo.Compare(x.strGoodsName, y.strGoodsName); });
                }
                else if (i == 1)
                {
                    // 价格排序
                    m_heroSortItemList[i] = LogicDataCenter.pointShopDataManager.CreateSingleSortBtnItem(heroSortFrame, strHeroSortBtn[i], (x, y) => { if (x.nMoneyPrice == y.nMoneyPrice) return 0; else return (x.nMoneyPrice > y.nMoneyPrice) ? 1 : -1; });
                }
                else
                {
                    continue;
                }

                m_heroSortItemList[i].OnClickItem += OnHeroSortChange;
            }

            m_heroItem = new PointShopHeroPageHeroItem[m_nSingleHeroPageCount];
            float fXOffset = -90f;
            float fYOffset = 258f;
            for (int i = 0; i < m_nSingleHeroPageCount; ++i)
            {
                GameObject heroItemObj = ResNode.InstantiateRes(singleHeroItemTemplate);
                if (heroItemObj)
                {
                    m_heroItem[i] = heroItemObj.GetComponent<PointShopHeroPageHeroItem>();
                    m_heroItem[i].SetParent(this);
                    RectTransform itemTrans = heroItemObj.transform as RectTransform;
                    itemTrans.SetParent(heroItemFrame, false);
                    itemTrans.anchoredPosition = new Vector2(fXOffset, fYOffset);
                    heroItemObj.SetActive(false);

                    fXOffset = -fXOffset;
                    if (fXOffset < 0f)
                        fYOffset -= 165f;
                }
            }

            m_skinItem = new PointShopHeroPageSkinItem[m_nSingleSkinPageCount];
            for (int i = 0; i < m_nSingleSkinPageCount; ++i)
            {
                GameObject skinItemObj = ResNode.InstantiateRes(singleSkinItemTemplate);
                if (skinItemObj)
                {
                    m_skinItem[i] = skinItemObj.GetComponent<PointShopHeroPageSkinItem>();
                    m_skinItem[i].SetParent(this);
                    RectTransform itemTrans = skinItemObj.transform as RectTransform;
                    itemTrans.SetParent(skinItemFrame, false);
                    skinItemObj.SetActive(false);
                }
            }

            String[] strHeroFilter = ULocalizationService.Instance.Get("UIView", "PointShop", "HeroOwnFilter").Split(';');
            for (int i = 0; i < strHeroFilter.Length; ++i)
            {
                Dropdown.OptionData opData = new Dropdown.OptionData();
                opData.text = strHeroFilter[i];
                heroFilterDropdown.options.Add(opData);
            }
            // heroFilterDropdown.RefreshShownValue(); // 假如unity版本升级了就可以用这个代替
            // 策划要求默认选择改为非拥有
            //heroFilterDropdown.value = 0;
            //heroFilterDropdown.captionText.text = strHeroFilter[0];
            heroFilterDropdown.value = 1;
            heroFilterDropdown.captionText.text = strHeroFilter[1];
            heroFilterDropdown.onValueChanged.AddListener(OnFilterChanged);

            String[] strSkinFilter = ULocalizationService.Instance.Get("UIView", "PointShop", "SkinOwnFilter").Split(';');
            for (int i = 0; i < strSkinFilter.Length; ++i)
            {
                Dropdown.OptionData opData = new Dropdown.OptionData();
                opData.text = strSkinFilter[i];
                skinFilterDropdown.options.Add(opData);
            }
            // skinFilterDropdown.RefreshShownValue(); // 假如unity版本升级了就可以用这个代替
            //skinFilterDropdown.value = 0;
            //skinFilterDropdown.captionText.text = strSkinFilter[0];
            // 策划要求默认选择改为非拥有
            skinFilterDropdown.value = 1;
            skinFilterDropdown.captionText.text = strSkinFilter[1];
            skinFilterDropdown.onValueChanged.AddListener(OnFilterChanged);

            heroRotate.SetParent(this);

            gameObject.SetActive(false);
        }

        public override void SetVisible(bool bVisible)
        {
            base.SetVisible(bVisible);

            if (!bVisible)
            {
                // 清空状态（英雄模型、选中状态）
                Clear();
            }
        }

        public override void OnReflush()
        {
            // 英雄显示处理
            List<SSchemePointShopGoodsConfig> heroConfigInfo = PointShopConfig.Instance.GetGoodsListByType(PointShopGoodsType.Goods_Hero);
            if (heroConfigInfo == null)
                return;

            bool bHaveSelHero = false;
            ActorHeroInfo actorHeroInfo = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
            List<SSchemePointShopGoodsConfig> filteredHeroConfigInfo = new List<SSchemePointShopGoodsConfig>();
            foreach (var config in heroConfigInfo)
            {
                SSchemeActorPrizeConfig prizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(config.nPrizeID);
                if (prizeConfig != null && prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HERO)
                {
                    int nHeroID = prizeConfig.nPrizeParam[0];
                    switch (heroFilterDropdown.value)
                    {
                        case 1:// 未拥有
                            {
                                if (actorHeroInfo.dicEntityHeroInfo.ContainsKey(nHeroID) && actorHeroInfo.dicEntityHeroInfo[nHeroID].bIsBuy > 0)
                                    continue;
                            }
                            break;
                        case 2:// 已拥有
                            {
                                if (!(actorHeroInfo.dicEntityHeroInfo.ContainsKey(nHeroID) && actorHeroInfo.dicEntityHeroInfo[nHeroID].bIsBuy > 0))
                                    continue;
                            }
                            break;
                        default:
                            break;
                    }

                    int nHeroTypeFilter = heroTypeFilterGroup.SelectedIndex;
                    if (nHeroTypeFilter > 0)
                    {
                        PersonSkinModelItem tempModelData = new PersonSkinModelItem();
                        if (GameLogicAPI.GetPersonModel((UInt16)nHeroID, ref tempModelData))
                        {
                            if (tempModelData.nType != (nHeroTypeFilter - 1))
                                continue;
                        }
                    }

                    String strSearch = searchHeroInputField.text;
                    if (strSearch.Length > 0)
                    {
                        if (!config.strGoodsName.Contains(strSearch))
                            continue;
                    }

                    filteredHeroConfigInfo.Add(config);
                    if (m_nCurSelHero == nHeroID)
                    {
                        bHaveSelHero = true;
                        m_nCurHeroSellID = config.nSellID;
                        m_curHeroSellConfig = config;
                    }
                }
            }
            if (!bHaveSelHero)
            {
                m_nCurSelHero = -1;
                m_nCurHeroSellID = -1;
                m_curHeroSellConfig = null;
            }

            for (int i = 0; i < m_heroSortItemList.Length; ++i)
            {
                int nSortType = m_heroSortItemList[i].SortType;
                if (nSortType > 0)
                {
                    PointShopConfig.InsertionSort(filteredHeroConfigInfo, m_heroSortItemList[i].Comparison, nSortType == (int)EMSortType.SORT_TYPE_DECREASE);
                    break;
                }
            }

            int nStartIndex = m_nCurHeroPage * m_nSingleHeroPageCount;  // 当前页显示、或者跳回第一页
            if (m_nCurHeroPage > 0 && filteredHeroConfigInfo.Count <= nStartIndex)
            {
                m_nCurHeroPage = 0;
                nStartIndex = 0;
            }

            int nHeroIndex = 0;
            for (int i = nStartIndex; (i < nStartIndex + m_nSingleHeroPageCount) && (i < filteredHeroConfigInfo.Count); ++i)
            {
                SSchemePointShopGoodsConfig heroInfo = filteredHeroConfigInfo[i];
                m_heroItem[nHeroIndex].SetHeroInfo(heroInfo);
                m_heroItem[nHeroIndex].gameObject.SetActive(true);
                ++nHeroIndex;
            }
            for (int i = nHeroIndex; i < m_nSingleHeroPageCount; ++i)
            {
                m_heroItem[i].gameObject.SetActive(false);
            }
            for (int i = 0; i < m_nSingleHeroPageCount; i++)
            {
                if (m_heroItem[i].HeroID == m_nCurSelHero)
                    m_heroItem[i].SetSelected(true);
                else
                    m_heroItem[i].SetSelected(false);
            }

            if (m_nCurHeroPage <= 0)
                heroPagePrevBtn.interactable = false;
            else
                heroPagePrevBtn.interactable = true;
            if (nStartIndex + m_nSingleHeroPageCount >= filteredHeroConfigInfo.Count)
                heroPageNextBtn.interactable = false;
            else
                heroPageNextBtn.interactable = true;
            heroPageText.text = String.Format("{0}/{1}", m_nCurHeroPage + 1, (filteredHeroConfigInfo.Count - 1) / m_nSingleHeroPageCount + 1);

            // 皮肤显示处理
            List<SSchemePointShopGoodsConfig> skinConfigInfo = PointShopConfig.Instance.GetGoodsListByType(PointShopGoodsType.Goods_Skin);
            if (skinConfigInfo == null)
                return;

            List<SSchemePointShopGoodsConfig> filteredSkinConfigInfo = new List<SSchemePointShopGoodsConfig>();
            bool bHaveSelSkin = false;
            int nTempHeroId = 0;
            foreach (var config in skinConfigInfo)
            {
                SSchemeActorPrizeConfig prizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(config.nPrizeID);
                if (prizeConfig != null && prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROSKIN)
                {
                    int nHeroID = prizeConfig.nPrizeParam[0];
                    int nSkinID = prizeConfig.nPrizeParam[1];

                    // 选择的英雄
                    PersonSkinModelItem modelData = new PersonSkinModelItem();
                    if (GameLogicAPI.GetPersonModel((UInt16)m_nCurSelHero, ref modelData))
                    {
                        List<int> nSkinIdList = new List<int>();
                        nSkinIdList.AddRange(modelData.nSkinIDList);
                        if (!nSkinIdList.Contains(nSkinID))
                            continue;
                    }

                    // 选择的皮肤
                    switch (skinFilterDropdown.value)
                    {
                        case 1:// 未拥有
                            {
                                if (LogicDataCenter.pointShopDataManager.checkIsHaveSkin(nHeroID, nSkinID))
                                    continue;
                            }
                            break;
                        case 2:// 已拥有
                            {
                                if (!LogicDataCenter.pointShopDataManager.checkIsHaveSkin(nHeroID, nSkinID))
                                    continue;
                            }
                            break;
                        default:
                            break;
                    }

                    // TODO 皮肤别的筛选方式

                    filteredSkinConfigInfo.Add(config);
                    if (m_nCurSelSkin == nSkinID)
                    {
                        bHaveSelSkin = true;
                        m_nCurSkinSellID = config.nSellID;
                        m_curSkinSellConfig = config;
                        nTempHeroId = nHeroID;
                    }
                }
            }
            if (!bHaveSelSkin)
            {
                m_nCurSelSkin = -1;
                m_nCurSkinSellID = -1;
                m_curSkinSellConfig = null;
            }

            nStartIndex = m_nCurSkinPage * m_nSingleSkinPageCount;//当前页显示、或者跳回第一页
            if (m_nCurSkinPage > 0 && nStartIndex >= filteredSkinConfigInfo.Count)
            {
                m_nCurSkinPage = 0;
                nStartIndex = 0;
            }

            int nSkinIndex = 0;
            for (int i = nStartIndex; i < filteredSkinConfigInfo.Count && nSkinIndex < m_nSingleSkinPageCount; ++i)
            {
                m_skinItem[nSkinIndex].SetSkinInfo(filteredSkinConfigInfo[nStartIndex]);
                m_skinItem[nSkinIndex].gameObject.SetActive(true);
                ++nSkinIndex;
            }
            for (int i = nSkinIndex; i < m_nSingleSkinPageCount; ++i)
            {
                m_skinItem[i].gameObject.SetActive(false);
            }
            for (int i = 0; i < m_nSingleSkinPageCount; i++)
            {
                if (m_skinItem[i].SkinID == m_nCurSelSkin)
                    m_skinItem[i].SetSelected(true);
                else
                    m_skinItem[i].SetSelected(false);
            }

            skinPagePrevBtn.interactable = (m_nCurSkinPage > 0);
            skinPageNextBtn.interactable = (filteredSkinConfigInfo.Count > (m_nCurSkinPage + 1) * m_nSingleSkinPageCount);
            skinPageText.text = String.Format("{0}/{1}", m_nCurSkinPage + 1, filteredSkinConfigInfo.Count / m_nSingleSkinPageCount + 1);

            // 购买显示处理,优先购买英雄
            if (LogicDataCenter.pointShopDataManager.CheckCanBuyGoods(m_nCurHeroSellID, 1))
            {
                if (m_curHeroSellConfig.nMoneyPrice > 0 && m_curHeroSellConfig.nPointPrice > 0)
                    buyPriceText.text = String.Format(ULocalizationService.Instance.Get("UIView", "PointShop", "HeroBuyPriceDesc"), m_curHeroSellConfig.nMoneyPrice, m_curHeroSellConfig.nPointPrice);
                else
                {
                    if (m_curHeroSellConfig.nMoneyPrice > 0)
                        buyPriceText.text = String.Format(ULocalizationService.Instance.Get("UIView", "PointShop", "GoldPriceDesc"), m_curHeroSellConfig.nMoneyPrice);
                    else if (m_curHeroSellConfig.nPointPrice > 0)
                        buyPriceText.text = String.Format(ULocalizationService.Instance.Get("UIView", "PointShop", "PointPriceDesc"), m_curHeroSellConfig.nPointPrice);
                }
                buyButtonText.text = ULocalizationService.Instance.Get("UIView", "PointShop", "HeroBuyBtn");
                buyBtn.interactable = true;
            }
            else if (LogicDataCenter.pointShopDataManager.CheckCanBuyGoods(m_nCurSkinSellID, 1))
            {
                if (m_curSkinSellConfig.nMoneyPrice > 0 && m_curSkinSellConfig.nPointPrice > 0)
                    buyPriceText.text = String.Format(ULocalizationService.Instance.Get("UIView", "PointShop", "HeroBuyPriceDesc"), m_curSkinSellConfig.nMoneyPrice, m_curSkinSellConfig.nPointPrice);
                else
                {
                    if (m_curSkinSellConfig.nMoneyPrice > 0)
                        buyPriceText.text = String.Format(ULocalizationService.Instance.Get("UIView", "PointShop", "GoldPriceDesc"), m_curSkinSellConfig.nMoneyPrice);
                    else if (m_curSkinSellConfig.nPointPrice > 0)
                        buyPriceText.text = String.Format(ULocalizationService.Instance.Get("UIView", "PointShop", "PointPriceDesc"), m_curSkinSellConfig.nPointPrice);
                }
                buyButtonText.text = ULocalizationService.Instance.Get("UIView", "PointShop", "SkinBuyBtn");
                buyBtn.interactable = true;
            }
            else
            {
                if (m_nCurSelHero < 0)
                    buyPriceText.text = "";
                else
                    buyPriceText.text = ULocalizationService.Instance.Get("UIView", "PointShop", "HeroSkinAlreadyHave");
                buyButtonText.text = ULocalizationService.Instance.Get("UIView", "PointShop", "BuyBtn");
                buyBtn.interactable = false;
            }

            // 模型显示处理
            int nModelHeroId = -1;
            int nModelSkinId = -1;
            if (bHaveSelHero && bHaveSelSkin)
            {
                nModelHeroId = m_nCurSelHero;
                nModelSkinId = m_nCurSelSkin;
            }
            else if (bHaveSelHero)
            {
                nModelHeroId = m_nCurSelHero;
                PersonSkinModelItem modelData = new PersonSkinModelItem();
                if (GameLogicAPI.GetPersonModel((UInt16)m_nCurSelHero, ref modelData))
                    nModelSkinId = modelData.nDefaultSkinID;
            }
            else if (bHaveSelSkin)
            {
                nModelHeroId = nTempHeroId;
                nModelSkinId = m_nCurSelSkin;
            }
            if (m_nLastSkinId != nModelSkinId)
                SkinManager.SetChangeHeroSkin(nModelSkinId, nModelHeroId, ref m_LastSkinObject, ref m_nLastSkinId, meshRenderNode, HEROSKIN_RENDER_TYPE.CAMERA, PERSONMODELVIEWID.PERSONMODELVIEWID_SHOP);
        }

        public void OnSelectHero(int nSellID, int nHeroID)
        {
            if (nSellID <= 0)
            {
                m_nCurSelHero = -1;
                m_nCurHeroSellID = -1;
            }
            else
            {
                m_nCurSelHero = nHeroID;
                m_nCurHeroSellID = nSellID;
            }

            for (int i = 0; i < m_nSingleHeroPageCount; ++i)
            {
                if (m_heroItem[i].HeroID == nHeroID)
                    m_heroItem[i].SetSelected(true);
                else
                    m_heroItem[i].SetSelected(false);
            }

            OnReflush();
        }

        public void OnHeroPrevPageBtn()
        {
            --m_nCurHeroPage;
            OnReflush();
        }

        public void OnHeroNextPageBtn()
        {
            ++m_nCurHeroPage;
            OnReflush();
        }

        public void OnSearchHeroBtn()
        {
            OnReflush();
        }

        public void OnFilterChanged(int nValue)
        {
            OnReflush();
        }

        public void OnFilterChanged(bool bActive)
        {
            if (!bActive)
                return;
            OnReflush();
        }

        public void OnHeroSortChange(PointShopSortItem item)
        {
            for (int i = 0; i < m_heroSortItemList.Length; ++i)
            {
                if (m_heroSortItemList[i] != item)
                    m_heroSortItemList[i].Clear();
            }

            OnReflush();
        }


        public void OnSelectSkin(int nSellID, int nSkinID)
        {
            if (nSellID <= 0)
            {
                m_nCurSelSkin = -1;
                m_nCurSkinSellID = -1;
            }
            else
            {
                m_nCurSelSkin = nSkinID;
                m_nCurSkinSellID = nSellID;
            }

            for (int i = 0; i < m_nSingleSkinPageCount; ++i)
            {
                if (m_skinItem[i].SkinID == nSkinID)
                    m_skinItem[i].SetSelected(true);
                else
                    m_skinItem[i].SetSelected(false);
            }

            OnReflush();
        }

        public void OnSkinPrePageBtn()
        {
            --m_nCurSkinPage;
            OnReflush();
        }

        public void OnSkinNextPageBtn()
        {
            ++m_nCurHeroPage;
            OnReflush();
        }

        public void RotateHeroModel(float fDelta)
        {
            if (SkinInstance.isNullOrEmpry(m_LastSkinObject))
            {
                return;
            }
        
            Vector3 rotation = m_LastSkinObject.SkinObj.transform.localEulerAngles;
            rotation = new Vector3(rotation.x, rotation.y + fDelta, rotation.z);
            m_LastSkinObject.SkinObj.transform.rotation = Quaternion.Euler(rotation);
        }

        public void ScaleHeroModel(float fDelta)
        {
            if (SkinInstance.isNullOrEmpry(m_LastSkinObject))
            {
                return;
            }

            Vector3 scale = m_LastSkinObject.SkinObj.transform.localScale;
            scale = new Vector3(scale.x + fDelta, scale.y + fDelta, scale.z + fDelta);
            m_LastSkinObject.SkinObj.transform.localScale = scale;
        }

        public void OnBuyBtn()
        {
            // 优先购买英雄
            if (LogicDataCenter.pointShopDataManager.CheckCanBuyGoods(m_nCurHeroSellID, 1))
            {
                m_wndView.OpenBuyWnd(m_nCurHeroSellID);
            }
            else if (LogicDataCenter.pointShopDataManager.CheckCanBuyGoods(m_nCurSkinSellID, 1))
            {
                m_wndView.OpenBuyWnd(m_nCurSkinSellID);
            }
        }

        private void Clear()
        {
            if (m_nLastSkinId > 0)
                SkinManager.SetChangeHeroSkin(-1, 0, ref m_LastSkinObject, ref m_nLastSkinId, meshRenderNode, HEROSKIN_RENDER_TYPE.CAMERA, PERSONMODELVIEWID.PERSONMODELVIEWID_SHOP);
            
            m_nCurSkinPage = 0;
            m_nCurHeroPage = 0;
            m_nCurSelHero = -1;
            m_nCurSelSkin = -1;
            m_nCurHeroSellID = -1;
            m_nCurSkinSellID = -1;
        }
    }
    #endregion

    #region 卡牌页面
    public class PointShopSingleCardItem : MonoBehaviour, IPointerDownHandler
    {
        public RectTransform iconFrame;
        public Text cardName;
        public RectTransform heroStarFrame;
        public Text cardProcessText;
        public RectTransform priceFrame;
        public Slider cardNum;
        public Sprite[] ExpValueType;
        public Image expValue;
        public Text cardDesc;

        public delegate void ClickDelegate(SSchemePointShopGoodsConfig config);
        public ClickDelegate OnClickItem;

        private PrizeIconItem iconItem;
        private ShopMoneyItem moneyItem;
        private HeroStarItem heroStarItem;
        private SSchemePointShopGoodsConfig m_goodsConfig;

        public void Init(RectTransform parent)
        {
            transform.SetParent(parent, false);

            iconItem = LogicDataCenter.pointShopDataManager.CreatePrizeIconItem(iconFrame);
            moneyItem = LogicDataCenter.pointShopDataManager.CreateShopMoneyItem(priceFrame);
            heroStarItem = LogicDataCenter.pointShopDataManager.CreateHeroStarItem(heroStarFrame); 
        }

        public void SetData(int nSellID)
        {
            m_goodsConfig = PointShopConfig.Instance.GetPointShopGoodsConfig(nSellID);
            if (m_goodsConfig == null)
                return;

            SSchemeActorPrizeConfig prizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(m_goodsConfig.nPrizeID);
            if (prizeConfig == null)
                return;

            cardName.text = m_goodsConfig.strGoodsName;
            iconItem.SetPrizeID(m_goodsConfig.nPrizeID);

            if (m_goodsConfig.strPlusDesc == "")
            {
                cardDesc.gameObject.SetActive(false);
            }
            else
            {
                String strTemp = m_goodsConfig.strPlusDesc;
                UBB.toHtml(ref strTemp);
                cardDesc.text = strTemp;
                cardDesc.gameObject.SetActive(true);
            }

            int nPointPrice = m_goodsConfig.nPointPrice;
            int nMoneyPrice = m_goodsConfig.nMoneyPrice;
            if (m_goodsConfig.nNumLimitID > 0)
            {
                nPointPrice = LogicDataCenter.pointShopDataManager.CalcNumLimitPrice(m_goodsConfig.nNumLimitID, m_goodsConfig.nSellID, 0, 1);
                nMoneyPrice = LogicDataCenter.pointShopDataManager.CalcNumLimitPrice(m_goodsConfig.nNumLimitID, m_goodsConfig.nSellID, 1, 1);
            }
            moneyItem.SetData(nPointPrice, nMoneyPrice);

            ActorHeroInfo actorHeroInfo = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
            if (actorHeroInfo == null)
                return;
            
            int nCardID = prizeConfig.nPrizeParam[0];
            if (actorHeroInfo.dicEntityHeroInfo.ContainsKey(nCardID))
            {
                cmd_entity_hero_info heroInfo = actorHeroInfo.dicEntityHeroInfo[nCardID];

                heroStarItem.gameObject.SetActive(true);
                heroStarItem.SetData(heroInfo.nHeroID, heroInfo.byStarLv, false, false);

                int nRealCard = heroInfo.nNeedCardCount;
                if (LogicDataCenter.lifeHeroDataManager.AllLifeHero.ContainsKey(heroInfo.nHeroID))
                {
                    nRealCard = heroInfo.nDiscountCardCount > 0 ? heroInfo.nDiscountCardCount : nRealCard;
                }
                int nStarLvLimit = GameLogicAPI.getHeroStarLvLimit(heroInfo.nHeroID);
                if (heroInfo.byStarLv >= nStarLvLimit)
                {
                    cardProcessText.text = "" + heroInfo.nCardCount;
                }
                else
                {
                    cardProcessText.text = "" + heroInfo.nCardCount + "/" + nRealCard;
                }

                cardNum.maxValue = 1;
                if (nRealCard == 0)
                    return;
                if (heroInfo.nCardCount >= nRealCard)
                {
                    cardNum.value = 1;
                    expValue.sprite = ExpValueType[1];
                } 
                else
                {
                    cardNum.value = (float)heroInfo.nCardCount / nRealCard;
                    expValue.sprite = ExpValueType[0];
                }
            }
            else if (actorHeroInfo.dicEntityNoHaveHeroInfo.ContainsKey(nCardID))
            {
                heroStarItem.gameObject.SetActive(false);

                cmd_entity_nohanve_hero nohaveHeroInfo = actorHeroInfo.dicEntityNoHaveHeroInfo[nCardID];

                int nRealCard = nohaveHeroInfo.nNeedCardCount;
                if (LogicDataCenter.lifeHeroDataManager.AllLifeHero.ContainsKey(nohaveHeroInfo.nHeroID))
                {
                    nRealCard = nohaveHeroInfo.nDiscountCardCount > 0 ? nohaveHeroInfo.nDiscountCardCount : nRealCard;
                }

                cardProcessText.text = "" + nohaveHeroInfo.nHaveCardCount + "/" + nRealCard;

                cardNum.maxValue = 1;
                if (nRealCard == 0)
                    return;
                if (nohaveHeroInfo.nHaveCardCount >= nRealCard)
                {
                    cardNum.value = 1;
                    expValue.sprite = ExpValueType[1];
                }
                else
                {
                    cardNum.value = (float)nohaveHeroInfo.nHaveCardCount / nRealCard;
                    expValue.sprite = ExpValueType[0];
                }
            }
        }

        public void SetVisible(bool bVisible)
        {
            gameObject.SetActive(bVisible);
        }

        public virtual void OnPointerDown(PointerEventData eventData)
        {
            if (OnClickItem != null)
            {
                OnClickItem(m_goodsConfig);
            }
        }
    }

    public class PointShopCardPageItem : PointShopPageItem
    {
        public Text countdownDesc;
        public Text countdownTime;
        public Button reflushBtn;
        public Text reflushBtnText;
        public Text reflushCostText;
        public URichText reflushRT;
        public RectTransform cardItemFrame;
        public GameObject singleCardItemTemplate;

        private float m_fNextUpdateTime;
        private const int nMaxCardItemNum = 6;
        private PointShopSingleCardItem[] m_cardItemList;

        public override void Init(PointShopWndView wndView)
        {
            base.Init(wndView);

            countdownDesc.text = ULocalizationService.Instance.Get("UIView", "PointShop", "CardFlushTime");
            m_cardItemList = new PointShopSingleCardItem[nMaxCardItemNum];
            for (int i = 0; i < nMaxCardItemNum; ++i)
            {
                GameObject goodsItem = ResNode.InstantiateRes(singleCardItemTemplate);
                if (goodsItem)
                {
                    m_cardItemList[i] = goodsItem.GetComponent<PointShopSingleCardItem>();
                    if (m_cardItemList[i])
                    {
                        m_cardItemList[i].Init(cardItemFrame);
                        LogicDataCenter.pointShopDataManager.CreateSquareHighlightItem(m_cardItemList[i].transform);
                        m_cardItemList[i].OnClickItem += OnClickGoods;
                    }
                }
            }

            reflushRT.text = "<a href=reflushRT color=48b7fe>" + ULocalizationService.Instance.Get("UIView", "PointShop", "reflushNow") + "</a>";
            reflushRT.onHrefClick.AddListener(OnClickReflushRT);
        }

        public void Update()
        {
            if (Time.time > m_fNextUpdateTime)
            {
                m_fNextUpdateTime = Time.time + 1f;

                int nLeftReflushTime = LogicDataCenter.pointShopDataManager.LeftReflushTime;
                int nHour = nLeftReflushTime / (60 * 60);
                int nMinus = nLeftReflushTime % (60 * 60) / 60;
                int nSec = nLeftReflushTime % 60;
                countdownTime.text = String.Format("{0:D2}:{1:D2}:{2:D2}", nHour, nMinus, nSec);

                //// 先这么写下，不过取本地时间显然误差很大
                //DateTime nextDay = DateTime.Now;
                //nextDay = nextDay.AddDays(1);
                //DateTime zeroClock = new DateTime(nextDay.Year, nextDay.Month, nextDay.Day);
                //TimeSpan deltaTime = zeroClock - DateTime.Now;
                //deltaTime += new TimeSpan(0, 1, 0);
                //countdownTime.text = String.Format("{0:D2}:{1:D2}:{2:D2}", nHour, nMinus, nSec);
            }
        }

        void OnClickGoods(SSchemePointShopGoodsConfig config)
        {
            if (config == null)
                return;

            m_wndView.OpenBuyWnd(config.nSellID);
        }

        public override void OnReflush()
        {
            List<int> cardListInfo = LogicDataCenter.pointShopDataManager.CardListInfo;
            if (cardListInfo == null)
                return;

            for (int i = 0; i < nMaxCardItemNum && i < cardListInfo.Count; ++i)
            {
                if (cardListInfo[i] > 0)
                {
                    m_cardItemList[i].SetData(cardListInfo[i]);
                    m_cardItemList[i].SetVisible(true);
                }
                else
                {
                    m_cardItemList[i].SetVisible(false);
                }
            }
        }

        public void OnClickReflushRT(string eventName)
        {
            if (eventName == "reflushRT")
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPREFLUSHCARD_SHOW, null);
            }
        }
    }
    #endregion

    #region 物品页面
    public class PointShopSingleGoodsItem : MonoBehaviour, IPointerDownHandler, IPointerEnterHandler, IPointerExitHandler
    {
        public RectTransform goodsIconFrame;
        public Text goodsNameText;
        public Text goodsDescText;
        public RectTransform priceFrame;
        public GameObject highlightEffect;
        public RectTransform plusIconFrame1;
        public RectTransform plusIconFrame2;
        public UTooltipTrigger tooltip;
        public Text goodsSubDescText;
        public Slider cardNum;
        public Text cardProcessText;
        public Image expValue;
        public Sprite[] ExpValueType;

        public delegate void ClickDelegate(SSchemePointShopGoodsConfig config);
        public ClickDelegate OnClickItem;

        private PrizeIconItem goodsIcon;
        private ShopMoneyItem moneyItem;
        private SSchemePointShopGoodsConfig m_goodsConfig;

        private bool m_useHighlight;

        public void Init(RectTransform parent)
        {
            transform.SetParent(parent, false);

            goodsIcon = LogicDataCenter.pointShopDataManager.CreatePrizeIconItem(goodsIconFrame);
            moneyItem = LogicDataCenter.pointShopDataManager.CreateShopMoneyItem(priceFrame);
        }

        public void SetUseHighlight(bool bUse)
        {
            m_useHighlight = bUse;
        }

        public void SetGoodsInfo(SSchemePointShopGoodsConfig config)
        {
            if (config == null)
            {
                SetVisible(false);
                return;
            }
            SetVisible(true);

            m_goodsConfig = config;

            goodsIcon.SetPrizeID(config.nPrizeID);
            goodsNameText.text = config.strGoodsName;
            if (config.strPlusDesc == "")
            {
                goodsDescText.gameObject.SetActive(false);
            }
            else
            {
                String strTemp = config.strPlusDesc;
                UBB.toHtml(ref strTemp);
                goodsDescText.text = strTemp;
                goodsDescText.gameObject.SetActive(true);
            }
            int nPointPrice = config.nPointPrice;
            int nMoneyPrice = config.nMoneyPrice;
            if (config.nNumLimitID > 0)
            {
                nPointPrice = LogicDataCenter.pointShopDataManager.CalcNumLimitPrice(config.nNumLimitID, config.nSellID, 0, 1);
                nMoneyPrice = LogicDataCenter.pointShopDataManager.CalcNumLimitPrice(config.nNumLimitID, config.nSellID, 1, 1);
            }
            moneyItem.SetData(nPointPrice, nMoneyPrice);
            if (config.strTooltip.Length > 0)
            {
                tooltip.enabled = true;
                String strTooltip = config.strTooltip;
                UBB.toHtml(ref strTooltip, UBB_FORMAT_TYPE.UGUI);
                tooltip.SetText(UTooltipParamName.BodyText, strTooltip);
            }
            else
            {
                tooltip.enabled = false;
            }

            LogicDataCenter.pointShopDataManager.FillGoodsPlusIcon(config, 0.77f, plusIconFrame1, plusIconFrame2);

            goodsSubDescText.gameObject.SetActive(false);
            cardNum.gameObject.SetActive(false);
            SSchemeActorPrizeConfig curPrizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(config.nPrizeID);
            if (curPrizeConfig == null)
                return;
            if (curPrizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HERO)
            {
                int nHeroID = curPrizeConfig.nPrizeParam[0];
                ActorHeroInfo actorHeroInfo = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
                if (actorHeroInfo.dicEntityHeroInfo.ContainsKey(nHeroID) && actorHeroInfo.dicEntityHeroInfo[nHeroID].bIsBuy > 0)
                {
                    goodsSubDescText.text = ULocalizationService.Instance.Get("UIView", "PointShop", "HeroAlreadyHave");
                    goodsSubDescText.gameObject.SetActive(true);
                }
            }
            else if (curPrizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROCARD)
            {
                cardNum.gameObject.SetActive(true);

                ActorHeroInfo actorHeroInfo = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
                if (actorHeroInfo == null)
                    return;

                int nCardID = curPrizeConfig.nPrizeParam[0];
                if (actorHeroInfo.dicEntityHeroInfo.ContainsKey(nCardID))
                {
                    cmd_entity_hero_info heroInfo = actorHeroInfo.dicEntityHeroInfo[nCardID];

                    int nRealCard = heroInfo.nNeedCardCount;
                    if (LogicDataCenter.lifeHeroDataManager.AllLifeHero.ContainsKey(heroInfo.nHeroID))
                    {
                        nRealCard = heroInfo.nDiscountCardCount > 0 ? heroInfo.nDiscountCardCount : nRealCard;
                    }

                    int nStarLvLimit = GameLogicAPI.getHeroStarLvLimit(heroInfo.nHeroID);
                    if (heroInfo.byStarLv >= nStarLvLimit)
                    {
                        cardProcessText.text = "" + heroInfo.nCardCount;
                    }
                    else
                    {
                        cardProcessText.text = "" + heroInfo.nCardCount + "/" + nRealCard;
                    }

                    cardNum.maxValue = 1;
                    if (nRealCard == 0)
                        return;
                    if (heroInfo.nCardCount >= nRealCard)
                    {
                        cardNum.value = 1;
                        expValue.sprite = ExpValueType[1];
                    }
                    else
                    {
                        cardNum.value = (float)heroInfo.nCardCount / nRealCard;
                        expValue.sprite = ExpValueType[0];
                    }
                }
                else if (actorHeroInfo.dicEntityNoHaveHeroInfo.ContainsKey(nCardID))
                {
                    cmd_entity_nohanve_hero nohaveHeroInfo = actorHeroInfo.dicEntityNoHaveHeroInfo[nCardID];

                    int nRealCard = nohaveHeroInfo.nNeedCardCount;
                    if (LogicDataCenter.lifeHeroDataManager.AllLifeHero.ContainsKey(nohaveHeroInfo.nHeroID))
                    {
                        nRealCard = nohaveHeroInfo.nDiscountCardCount > 0 ? nohaveHeroInfo.nDiscountCardCount : nRealCard;
                    }

                    cardProcessText.text = "" + nohaveHeroInfo.nHaveCardCount + "/" + nRealCard;

                    cardNum.maxValue = 1;
                    if (nRealCard == 0)
                        return;
                    if (nohaveHeroInfo.nHaveCardCount >= nRealCard)
                    {
                        cardNum.value = 1;
                        expValue.sprite = ExpValueType[1];
                    }
                    else
                    {
                        cardNum.value = (float)nohaveHeroInfo.nHaveCardCount / nRealCard;
                        expValue.sprite = ExpValueType[0];
                    }
                }
            }
        }

        public void SetVisible(bool bVisible)
        {
            gameObject.SetActive(bVisible);
        }

        public virtual void OnPointerDown(PointerEventData eventData)
        {
            if (OnClickItem != null)
            {
                OnClickItem(m_goodsConfig);
            }
        }

        public virtual void OnPointerEnter(PointerEventData eventData)
        {
            if (m_useHighlight)
                highlightEffect.SetActive(true);
        }

        public virtual void OnPointerExit(PointerEventData eventData)
        {
            if (m_useHighlight)
                highlightEffect.SetActive(false);
        }
    }

    public abstract class PointShopGoodsFilterItem : MonoBehaviour
    {
        public Text titleText;

        public delegate void SelectChangedDelegate(int nFilterIndex);
        public SelectChangedDelegate onSelectChanged;

        private int m_nFilterIndex;

        public virtual void Init(int nFilterIndex, String title, String[] content)
        {
            m_nFilterIndex = nFilterIndex;
            titleText.text = title;
        }

        // 获得当前选项(单选用)
        public abstract int GetCurSelect();
        // 获得选项是否被选择(多选用)
        public abstract bool IsSelected(int nIndex);

        protected void OnChanged()
        {
            if (onSelectChanged != null)
                onSelectChanged(m_nFilterIndex);
        }
    }

    public class PointShopGoodsDropdownFilterItem : PointShopGoodsFilterItem
    {
        public Dropdown dropdown;

        public override void Init(int nFilterIndex, string title, string[] content)
        {
            base.Init(nFilterIndex, title, content);

            if (content == null)
                return;

            for (int i = 0; i < content.Length; ++i)
            {
                Dropdown.OptionData data = new Dropdown.OptionData();
                data.text = content[i];
                dropdown.options.Add(data);
            }

            // heroFilterDropdown.RefreshShownValue(); // 假如unity版本升级了就可以用这个代替
            dropdown.value = 0;
            dropdown.captionText.text = content[0];

            dropdown.onValueChanged.AddListener(OnValueChanged);
        }

        public override int GetCurSelect()
        {
            return dropdown.value;
        }

        public override bool IsSelected(int nIndex)
        {
            return (nIndex == dropdown.value);
        }

        void OnValueChanged(int nValue)
        {
            OnChanged();
        }
    }

    public class PointShopGoodsToggleFilterItem : PointShopGoodsFilterItem
    {
        public ToggleGroup toggleGroup;
        public GameObject toggleTemplate;

        private Toggle[] toggleList;

        public override void Init(int nFilterIndex, string title, string[] content)
        {
            base.Init(nFilterIndex, title, content);

            toggleList = new Toggle[content.Length];
            for (int i = 0; i < content.Length; ++i)
            {
                GameObject newToggle = ResNode.InstantiateRes(toggleTemplate);
                if (newToggle)
                {
                    toggleList[i] = newToggle.GetComponent<Toggle>();
                    if (i == 0)
                    {
                        toggleList[i].isOn = true;
                    }
                    toggleList[i].onValueChanged.AddListener(OnValueChanged);

                    Text[] toggleText = newToggle.GetComponentsInChildren<Text>(true);
                    if (toggleText != null)
                    {
                        toggleText[0].text = content[i];
                    }

                    newToggle.transform.SetParent(transform, false);
                    newToggle.SetActive(true);
                }
            }
        }

        public override int GetCurSelect()
        {
            for (int i = 0; i < toggleList.Length; ++i)
            {
                if (toggleList[i].isOn)
                    return i;
            }
            return -1;
        }

        public override bool IsSelected(int nIndex)
        {
            if (nIndex < 0 || nIndex >= toggleList.Length)
                return false;

            return toggleList[nIndex].isOn;
        }

        void OnValueChanged(bool bValue)
        {
            OnChanged();
        }
    }

    public class PointShopGoodsCheckBoxFilterItem : PointShopGoodsFilterItem
    {
        private Toggle[] toggleList;
        public GameObject checkboxTemplate;

        public override void Init(int nFilterIndex, string title, string[] content)
        {
            base.Init(nFilterIndex, title, content);

            toggleList = new Toggle[content.Length];
            for (int i = 0; i < content.Length; ++i)
            {
                GameObject newToggle = ResNode.InstantiateRes(checkboxTemplate);
                if (newToggle)
                {
                    toggleList[i] = newToggle.GetComponent<Toggle>();
                    toggleList[i].onValueChanged.AddListener(OnValueChanged);

                    Text[] toggleText = newToggle.GetComponentsInChildren<Text>(true);
                    if (toggleText != null)
                    {
                        toggleText[0].text = content[i];
                    }

                    newToggle.transform.SetParent(transform, false);
                    newToggle.SetActive(true);
                }
            }
        }

        public override int GetCurSelect()
        {
            for (int i = 0; i < toggleList.Length; ++i)
            {
                if (toggleList[i].isOn)
                    return i;
            }
            return -1;
        }

        public override bool IsSelected(int nIndex)
        {
            if (nIndex < 0 || nIndex >= toggleList.Length)
                return false;

            return toggleList[nIndex].isOn;
        }

        void OnValueChanged(bool bValue)
        {
            OnChanged();
        }
    }

    public class PointShopNormalPageItem : PointShopPageItem
    {
        public RectTransform filterFrame;
        public RectTransform goodsFrame;

        enum GoodsFilterType
        {
            DropdownFilter,
            ToggleFilter,
            CheckboxFilter,
        }
        public GameObject dropdownFilter;
        public GameObject toggleFilter;
        public GameObject checkboxFilter;
        public GameObject sorterFrame;

        public Text pageText;
        public Button pagePrevBtn;
        public Button pageNextBtn;

        private const int nSinglePageGoodsCount = 8;
        private PointShopSingleGoodsItem[] m_goodsItem;
        private List<PointShopGoodsFilterItem> m_filterList;
        private int m_nCurGoodsPage = 0;
        private int m_nLastFilterType = -1;
        private PointShopSortItem[] m_sortItemList;

        public override void Init(PointShopWndView wndView)
        {
            base.Init(wndView);

            m_goodsItem = new PointShopSingleGoodsItem[nSinglePageGoodsCount];
            for (int i = 0; i < nSinglePageGoodsCount; ++i)
            {
                m_goodsItem[i] = LogicDataCenter.pointShopDataManager.CreateShopSingleGoodsItem(goodsFrame);
                if (m_goodsItem[i])
                {
                    m_goodsItem[i].SetUseHighlight(true);
                    m_goodsItem[i].OnClickItem += OnClickGoods;
                }
            }

            m_filterList = new List<PointShopGoodsFilterItem>();
        }

        public override void OnReflush()
        {
            List<SSchemePointShopGoodsConfig> configInfo = null;
            if (m_nCurPageType == (int)PointShopPageBtnType.PageBtn_Rune)
                configInfo = PointShopConfig.Instance.GetGoodsListByType(PointShopGoodsType.Goods_Rune);
            else if (m_nCurPageType == (int)PointShopPageBtnType.PageBtn_Chest)
                configInfo = PointShopConfig.Instance.GetGoodsListByType(PointShopGoodsType.Goods_Chest);
            else if (m_nCurPageType == (int)PointShopPageBtnType.PageBtn_Xp)
                configInfo = PointShopConfig.Instance.GetGoodsListByType(PointShopGoodsType.Goods_Xp);
            else if (m_nCurPageType == (int)PointShopPageBtnType.PageBtn_GemStone)
                configInfo = PointShopConfig.Instance.GetGoodsListByType(PointShopGoodsType.Goods_GemStone);
            else if (m_nCurPageType == (int)PointShopPageBtnType.PageBtn_Other)
                configInfo = PointShopConfig.Instance.GetGoodsListByType(PointShopGoodsType.Goods_BenefitCard);
            else if (m_nCurPageType == (int)PointShopPageBtnType.PageBtn_AD)
                configInfo = m_wndView.SearchList;
            if (configInfo == null)
                return;

            if (m_nLastFilterType != m_nCurPageType)
            {
                ClearFilterContent();
                switch ((PointShopPageBtnType)m_nCurPageType)
                {
                    case PointShopPageBtnType.PageBtn_Rune:
                        {
                            String[] strFilter1 = ULocalizationService.Instance.Get("UIView", "PointShop", "RuneFilter1").Split(';');
                            AddFilter(GoodsFilterType.DropdownFilter, 0, ULocalizationService.Instance.Get("UIView", "PointShop", "RuneFilter1Title"), strFilter1);
                            String[] strFilter2 = ULocalizationService.Instance.Get("UIView", "PointShop", "RuneFilter2").Split(';');
                            AddFilter(GoodsFilterType.ToggleFilter, 1, ULocalizationService.Instance.Get("UIView", "PointShop", "RuneFilter2Title"), strFilter2);
                            String[] strFilter3 = ULocalizationService.Instance.Get("UIView", "PointShop", "RuneFilter3").Split(';');
                            AddFilter(GoodsFilterType.ToggleFilter, 2, ULocalizationService.Instance.Get("UIView", "PointShop", "RuneFilter3Title"), strFilter3);

                            String[] strSort = ULocalizationService.Instance.Get("UIView", "PointShop", "RuneSort").Split(';');
                            int[] nSortType = new int[2]{0, 1};
                            AddSorter(ULocalizationService.Instance.Get("UIView", "PointShop", "GoodsSortTitle"), strSort, nSortType);
                        }
                        break;
                    case PointShopPageBtnType.PageBtn_Chest:
                        {
                            String[] strFilter1 = ULocalizationService.Instance.Get("UIView", "PointShop", "ChestFilter1").Split(';');
                            AddFilter(GoodsFilterType.ToggleFilter, 0, ULocalizationService.Instance.Get("UIView", "PointShop", "ChestFilter1Title"), strFilter1);
                            String[] strFilter2 = ULocalizationService.Instance.Get("UIView", "PointShop", "ChestFilter2").Split(';');
                            AddFilter(GoodsFilterType.ToggleFilter, 1, ULocalizationService.Instance.Get("UIView", "PointShop", "ChestFilter2Title"), strFilter2);

                            String[] strSort = ULocalizationService.Instance.Get("UIView", "PointShop", "ChestSort").Split(';');
                            int[] nSortType = new int[2] { 0, 1 };
                            AddSorter(ULocalizationService.Instance.Get("UIView", "PointShop", "GoodsSortTitle"), strSort, nSortType);
                        }
                        break;
                    case PointShopPageBtnType.PageBtn_Other:
                        {
                            String[] strFilter1 = ULocalizationService.Instance.Get("UIView", "PointShop", "OtherFilter1").Split(';');
                            AddFilter(GoodsFilterType.ToggleFilter, 0, ULocalizationService.Instance.Get("UIView", "PointShop", "OtherFilter1Title"), strFilter1);
                            String[] strFilter2 = ULocalizationService.Instance.Get("UIView", "PointShop", "OtherFilter2").Split(';');
                            AddFilter(GoodsFilterType.ToggleFilter, 1, ULocalizationService.Instance.Get("UIView", "PointShop", "OtherFilter2Title"), strFilter2);

                            String[] strSort = ULocalizationService.Instance.Get("UIView", "PointShop", "OtherSort").Split(';');
                            int[] nSortType = new int[2] { 0, 1 };
                            AddSorter(ULocalizationService.Instance.Get("UIView", "PointShop", "GoodsSortTitle"), strSort, nSortType);
                        }
                        break;
                    default:
                        break;
                }
                m_nLastFilterType = m_nCurPageType;
                m_nCurGoodsPage = 0;
            }

            List<SSchemePointShopGoodsConfig> filterConfigInfo = new List<SSchemePointShopGoodsConfig>();
            foreach (var config in configInfo)
            {
                SSchemeActorPrizeConfig prizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(config.nPrizeID);
                switch ((PointShopPageBtnType)m_nCurPageType)
                {
                    case PointShopPageBtnType.PageBtn_Rune:
                        {
                            int nRuneID = prizeConfig.nPrizeParam[0];
                            SSchemeRuneConfig runeConfig = null;
                            RunePage.Instance.GetRuneInfo(nRuneID, ref runeConfig);
                            if (runeConfig == null)
                                continue;

                            if (m_filterList.Count >= 3)
                            {
                                int nTypeFilter = m_filterList[1].GetCurSelect();
                                if (nTypeFilter > 0 && nTypeFilter != runeConfig.nRuneInlayType)
                                    continue;

                                int nFuncFilter = m_filterList[2].GetCurSelect();
                                if (nFuncFilter > 0 && nFuncFilter != runeConfig.nRunePropertyType)
                                    continue;
                            }
                        }
                        break;
                    default:
                        {
                        }
                        break;
                }

                filterConfigInfo.Add(config);
            }

            if (m_sortItemList == null)
            {
                m_sortItemList = new PointShopSortItem[0];
            }

            for (int i = 0; i < m_sortItemList.Length; ++i)
            {
                int nSortType = m_sortItemList[i].SortType;
                if (nSortType > 0)
                {
                    PointShopConfig.InsertionSort(filterConfigInfo, m_sortItemList[i].Comparison, nSortType == (int)EMSortType.SORT_TYPE_DECREASE);
                    break;
                }
            }

            int nStartIndex = m_nCurGoodsPage * nSinglePageGoodsCount;
            int nCurIndex = 0;
            for (int i = nStartIndex; (i < nStartIndex + nSinglePageGoodsCount) && (i < filterConfigInfo.Count); ++i)
            {
                SSchemePointShopGoodsConfig goodsInfo = filterConfigInfo[i];
                m_goodsItem[nCurIndex].SetGoodsInfo(goodsInfo);
                m_goodsItem[nCurIndex].SetVisible(true);
                ++nCurIndex;
            }

            if (nCurIndex == 0 && m_nCurGoodsPage > 0)
            {
                // 跳回第一页
                m_nCurGoodsPage = 0;
                OnReflush();
                return;
            }

            for (int i = nCurIndex; i < nSinglePageGoodsCount; ++i)
            {
                m_goodsItem[i].SetVisible(false);
            }

            if (m_nCurGoodsPage <= 0)
                pagePrevBtn.interactable = false;
            else
                pagePrevBtn.interactable = true;

            if (nStartIndex + nSinglePageGoodsCount >= filterConfigInfo.Count)
                pageNextBtn.interactable = false;
            else
                pageNextBtn.interactable = true;

            pageText.text = String.Format("{0}/{1}", m_nCurGoodsPage + 1, (filterConfigInfo.Count - 1) / nSinglePageGoodsCount + 1);
        }

        public void OnPrevPageBtn()
        {
            --m_nCurGoodsPage;
            OnReflush();
        }

        public void OnNextPageBtn()
        {
            ++m_nCurGoodsPage;
            OnReflush();
        }

        void ClearFilterContent()
        {
            int nCount = filterFrame.childCount;
            for (int i = nCount - 1; i >= 0; --i)
            {
                Transform childTrans = filterFrame.GetChild(i);
                if (childTrans)
                {
                    var temp = childTrans.gameObject;
                    ResNode.DestroyRes(ref temp);
                }
            }

            m_filterList.Clear();
        }

        void AddFilter(GoodsFilterType nFilterType, int nFilterIndex, String title, String[] content)
        {
            GameObject filterObj = null;
            if (nFilterType == GoodsFilterType.DropdownFilter)
            {
                filterObj = ResNode.InstantiateRes(dropdownFilter);
            }
            else if (nFilterType == GoodsFilterType.ToggleFilter)
            {
                filterObj = ResNode.InstantiateRes(toggleFilter);
            }
            else if (nFilterType == GoodsFilterType.CheckboxFilter)
            {
                filterObj = ResNode.InstantiateRes(checkboxFilter);
            }

            if (filterObj == null)
                return;

            filterObj.transform.SetParent(filterFrame, false);
            PointShopGoodsFilterItem filterItem = filterObj.GetComponent<PointShopGoodsFilterItem>();
            filterItem.Init(nFilterIndex, title, content);
            filterItem.onSelectChanged += OnFilterChanged;
            filterObj.SetActive(true);

            m_filterList.Add(filterItem);
        }

        void AddSorter(String title, String[] content, int[] nSortType)
        {
            GameObject frameObj = ResNode.InstantiateRes(sorterFrame);
            frameObj.transform.SetParent(filterFrame, false);
            frameObj.SetActive(true);

            m_sortItemList = new PointShopSortItem[content.Length];
            for (int i = 0; i < content.Length && i < nSortType.Length; ++i)
            {
                if (nSortType[i] == 0)
                {
                    // 名称排序
                    m_sortItemList[i] = LogicDataCenter.pointShopDataManager.CreateSingleSortBtnItem(filterFrame, content[i], (x, y) => { CultureInfo cult = new CultureInfo("zh-CN"); return cult.CompareInfo.Compare(x.strGoodsName, y.strGoodsName); });
                }
                else if (nSortType[i] == 1)
                {
                    // 价格排序
                    m_sortItemList[i] = LogicDataCenter.pointShopDataManager.CreateSingleSortBtnItem(filterFrame, content[i], (x, y) => { if (x.nMoneyPrice == y.nMoneyPrice) return 0; else return (x.nMoneyPrice > y.nMoneyPrice) ? 1 : -1; });
                }
                else
                {
                    continue;
                }

                m_sortItemList[i].OnClickItem += OnSortChange;
                m_sortItemList[i].transform.SetParent(frameObj.transform, false);
            }
        }

        void OnFilterChanged(int nFilterIndex)
        {
            OnReflush();
        }

        void OnSortChange(PointShopSortItem item)
        {
            for (int i = 0; i < m_sortItemList.Length; ++i)
            {
                if (m_sortItemList[i] != item)
                    m_sortItemList[i].Clear();
            }

            OnReflush();
        }

        void OnClickGoods(SSchemePointShopGoodsConfig config)
        {
            if (config == null)
                return;

            m_wndView.OpenBuyWnd(config.nSellID);
        }
    }
    #endregion

    #region 充值页面
    public class PointShopPayItem : PointShopPageItem
    {
        public RawImage QRPayImg;
        public Button unionPayBtn;

        private string szQRCodeUrl;
        private int qrImgWidth;

        public override void Init(PointShopWndView wndView)
        {
            base.Init(wndView);

            qrImgWidth = (int)QRPayImg.rectTransform.sizeDelta.x;

            unionPayBtn.onClick.AddListener(PosRequestOpenLanPage);
        }

        public void OnDestroy()
        {
            QRPayImg.texture = null;
            StopAllCoroutines();
        }

        public override void OnReflush()
        {
            PostRequestHttpQRCodeInfo();
        }

        private void PosRequestOpenLanPage()
        {
            string szUrl = Config.PayUrl;
            szUrl += GetUserName(true);
            szUrl += GetWorldID();
            Application.OpenURL(szUrl);
        }

        private void PostRequestHttpQRCodeInfo()
        {
            string payUrl = GetPayUrl();

            szQRCodeUrl = BuildQRUrl(payUrl, qrImgWidth, 4, 4);

            if(QRPayImg.texture == null)
            {
                StartCoroutine(LoadQRUrlImage());
            }
        }

        IEnumerator LoadQRUrlImage()
        {
            if (IsVisible())
            {
                WWW weChatWww = new WWW(szQRCodeUrl);
                yield return weChatWww;
                if (string.IsNullOrEmpty(weChatWww.error))
                {
                    string contentType = weChatWww.responseHeaders["CONTENT-TYPE"];
                    var contents = contentType.Split('/');
                    if (contents[0] == "image")
                    {
                        if (contents[1].ToLower() == "png" || contents[1].ToLower() == "jpeg")
                        {
                            Texture2D t = weChatWww.texture;
                            if (t != null && QRPayImg != null)
                            {
                                QRPayImg.texture = t;
                            }
                        }
                    }
                }
                yield return new WaitForSeconds(2.0f);
            }
        }

        private string GetPayUrl()
        {
            string payUrl = Config.PayQRUrl;
            payUrl += "?gameid=7";
            payUrl += GetUserName(false);
            payUrl += GetWorldID();

            return payUrl;
        }

        private string BuildQRUrl(string payUrl, int w, int bw, int bs)
        {
            string url = Config.QRBuildUrl;
	
	        //--二维码打开的链接
            url += Uri.EscapeDataString(payUrl);

            //--二维码图片大小
            url += "&w=" + w;
	
	        //--二维码边框大小
            url += "&bw=" + bw;
	
            //--二维码边框间距
            url += "&bs=" + bs;

            //url += "&t=" + 2;
            
            //--二维码微信图标
            url += "&l=" + 0;

            return url;
        }

        private string GetUserName(bool bWebOpen)
        {
            string szUserID = bWebOpen ? "?u=" : "&username=";
            IntPtr account = GameLogicAPI.getActorClientAccount();
            if (account != null && account != IntPtr.Zero)
            {
                string szAccount = Marshal.PtrToStringAnsi(account);
                if (!string.IsNullOrEmpty(szAccount))
                {
                    szUserID += szAccount;
                }
            }
            return szUserID;
        }

        private string GetWorldID()
        {
            string szServerId = "&serverid=";
            int nID = GameLogicAPI.getMasterFromGameWorldID();
            if(nID > 0)
            {
                szServerId += nID;
            }
            return szServerId;
        }
    }
    #endregion

    public class PointShopWndView : UIBaseWndView
    {
        // 商城分页类型
        enum PointShopPageType
        {
            Page_AD,
            Page_Hero,
            Page_Card,
            Page_Normal,
            Page_Pay,

            Page_Max
        }

        //public CanvasGroup frameCanvasGroup;

        //public Text actorPointText;
        //public Text actorMoneyText;
        public Image BgImg;
        public PointShopPageItem[] shopPageItem;

        public RectTransform shopPageBtnFrame;
        public UButtonGroup shopPageButtonGroup;
        public GameObject shopPageBtnTemplate;
        public GameObject payPageBtnTemplate;

        private UButtonGroupItem[] shopPageBtn;
        private Text[] shopPageBtnText;
        private PointShopPageType[] shopPageBtnType;

        private List<SSchemePointShopGoodsConfig> m_searchList;
        public List<SSchemePointShopGoodsConfig> SearchList { get { return m_searchList; } }

        private bool bFirstOpen = false;
        //private UEffectParamBase m_cloudEffectParam;
        //private UEffectParamBase m_pageBtnEffectParam;

        //private float m_nextUpdateTime;
        //private int m_lastActorPoint = -1;
        //private int m_lastActorGold = -1;
        //private bool m_bClosing;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            m_UseImageEffect = true;

            for (int i = 0; i < shopPageItem.Length; ++i)
            {
                shopPageItem[i].Init(this);
            }

            shopPageBtn = new UButtonGroupItem[(int)PointShopPageBtnType.PageBtn_Max];
            shopPageBtnText = new Text[(int)PointShopPageBtnType.PageBtn_Max];
            for (int i = 0; i < (int)PointShopPageBtnType.PageBtn_Max; ++i)
            {
                GameObject pageBtnObj;

                if (i == (int)PointShopPageBtnType.PageBtn_Rune)
                    pageBtnObj = ResNode.InstantiateRes(payPageBtnTemplate);
                else
                    pageBtnObj = ResNode.InstantiateRes(shopPageBtnTemplate);

                if (pageBtnObj)
                {
                    pageBtnObj.transform.SetParent(shopPageBtnFrame, false);
                    pageBtnObj.SetActive(true);
                    // 暂时只打开5个，勋章、终结技、水晶、其他、充值
                    if (i > 4)
                    {
                        pageBtnObj.SetActive(false);
                    }

                    shopPageBtn[i] = pageBtnObj.GetComponent<UButtonGroupItem>();
                    shopPageBtn[i].Group = shopPageButtonGroup;
                    shopPageBtn[i].onSelectedChangedEx.AddListener(OnPageBtnClick);
                    Transform textTrans = pageBtnObj.transform.GetChild(0);
                    if (textTrans)
                    {
                        shopPageBtnText[i] = textTrans.gameObject.GetComponent<Text>();
                    }

                    if(i == (int)PointShopPageBtnType.PageBtn_Rune)
                    {
                        UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_PointShop_PayBtn, _tfAttachParent: pageBtnObj.transform);
                        UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);
                    }
                }
            }

            String[] strPageBtnText = ULocalizationService.Instance.Get("UIView", "PointShop", "PageBtnName").Split(';');

            // 暂时只打开5个，勋章、终结技、水晶、其他、充值
            shopPageBtnType = new PointShopPageType[5];
            int nBtnIndex = (int)PointShopPageBtnType.PageBtn_AD;
            shopPageBtnType[nBtnIndex] = PointShopPageType.Page_Card;
            shopPageBtnText[nBtnIndex].text = strPageBtnText[(int)PointShopPageBtnType.PageBtn_Card];

            nBtnIndex = (int)PointShopPageBtnType.PageBtn_Hero;
            shopPageBtnType[nBtnIndex] = PointShopPageType.Page_Normal;
            shopPageBtnText[nBtnIndex].text = strPageBtnText[(int)PointShopPageBtnType.PageBtn_Xp];

            nBtnIndex = (int)PointShopPageBtnType.PageBtn_Wing;
            shopPageBtnType[nBtnIndex] = PointShopPageType.Page_Normal;
            shopPageBtnText[nBtnIndex].text = strPageBtnText[(int)PointShopPageBtnType.PageBtn_GemStone];

            nBtnIndex = (int)PointShopPageBtnType.PageBtn_Card;
            shopPageBtnType[nBtnIndex] = PointShopPageType.Page_Normal;
            shopPageBtnText[nBtnIndex].text = strPageBtnText[(int)PointShopPageBtnType.PageBtn_Other];

            nBtnIndex = (int)PointShopPageBtnType.PageBtn_Rune;
            shopPageBtnType[nBtnIndex] = PointShopPageType.Page_Pay;
            shopPageBtnText[nBtnIndex].text = strPageBtnText[(int)PointShopPageBtnType.PageBtn_Pay];

            // 以后改成从脚本取
            //shopPageBtnType = new PointShopPageType[(int)PointShopPageBtnType.PageBtn_Max];
            //int nBtnIndex = (int)PointShopPageBtnType.PageBtn_AD;
            //shopPageBtnType[nBtnIndex] = PointShopPageType.Page_AD;
            //shopPageBtnText[nBtnIndex].text = strPageBtnText[nBtnIndex];
            //nBtnIndex = (int)PointShopPageBtnType.PageBtn_Hero;
            //shopPageBtnType[nBtnIndex] = PointShopPageType.Page_Hero;
            //shopPageBtnText[nBtnIndex].text = strPageBtnText[nBtnIndex];
            //nBtnIndex = (int)PointShopPageBtnType.PageBtn_Wing;
            //shopPageBtnType[nBtnIndex] = PointShopPageType.Page_Hero;
            //shopPageBtnText[nBtnIndex].text = strPageBtnText[nBtnIndex];
            //nBtnIndex = (int)PointShopPageBtnType.PageBtn_Card;
            //shopPageBtnType[nBtnIndex] = PointShopPageType.Page_Card;
            //shopPageBtnText[nBtnIndex].text = strPageBtnText[nBtnIndex];
            //nBtnIndex = (int)PointShopPageBtnType.PageBtn_Rune;
            //shopPageBtnType[nBtnIndex] = PointShopPageType.Page_Normal;
            //shopPageBtnText[nBtnIndex].text = strPageBtnText[nBtnIndex];
            //nBtnIndex = (int)PointShopPageBtnType.PageBtn_Chest;
            //shopPageBtnType[nBtnIndex] = PointShopPageType.Page_Normal;
            //shopPageBtnText[nBtnIndex].text = strPageBtnText[nBtnIndex];
            //nBtnIndex = (int)PointShopPageBtnType.PageBtn_Other;
            //shopPageBtnType[nBtnIndex] = PointShopPageType.Page_Normal;
            //shopPageBtnText[nBtnIndex].text = strPageBtnText[nBtnIndex];

            m_searchList = new List<SSchemePointShopGoodsConfig>();

            //m_cloudEffectParam = new UEffectPrefabParam(UEffectPrefabType.UEPT_FullScreen_Cloud, transform, null, null, true);
            //m_pageBtnEffectParam = new UEffectPrefabParam(UEffectPrefabType.UEPT_Shop_Hexagon_Blue, transform, null, null, true);

            // 默认选中第一个
            shopPageBtn[0].Selected = true;
            OnPageBtnClick(shopPageBtn[0], true);

            base.SetRendererVisible(false);

            return true;
        }

        internal override void SetRendererVisible(bool _bVisible)
        {
            base.SetRendererVisible(_bVisible);

            if (!bFirstOpen && _bVisible)
            {
                UEffectParamBase param = new UEffectMaterialParam(UEffectMaterialType.EMT_Blur, BgImg, true);

                if (UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectMaterial, ref param))
                {
                    (param as UEffectMaterialParam).ReturnEffectMaterial.SetColor("_TintColor", new Color(0, 0, 0, 0.9f));
                }
                bFirstOpen = true;
            }
        }

        //public override void Update()
        //{
        //    base.Update();

        //    if (EntityFactory.MainHeroView != null)
        //    {
        //        if (Time.unscaledTime > m_nextUpdateTime)
        //        {
        //            if (m_lastActorPoint != EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TICKET))
        //            {
        //                m_lastActorPoint = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TICKET);
        //                actorPointText.text = m_lastActorPoint.ToString();
        //            }

        //            if (m_lastActorGold != EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_GOLD))
        //            {
        //                m_lastActorGold = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_GOLD);
        //                actorMoneyText.text = m_lastActorGold.ToString();
        //            }

        //            // 每秒更新一次
        //            m_nextUpdateTime = Time.unscaledTime + 1f;
        //        }
        //    }

        //    if (m_bClosing)
        //    {
        //        if (frameCanvasGroup.alpha > 0f)
        //        {
        //            const float fFirstStepTime = 0.8f;
        //            const float fSecondStepTime = 1.2f;
        //            if (frameCanvasGroup.alpha < 0.1f)
        //            {
        //                frameCanvasGroup.alpha -= (0.1f * Time.deltaTime / fFirstStepTime);
        //            }
        //            else
        //            {
        //                frameCanvasGroup.alpha -= (0.9f * Time.deltaTime / fSecondStepTime);
        //            }
        //        }
        //        else
        //        {
        //            m_bClosing = false;

        //            base.SetRendererVisible(false);
        //        }
        //    }
        //    else
        //    {
        //        if (frameCanvasGroup.alpha < 1f)
        //        {
        //            const float fFirstStepTime = 0.8f;
        //            const float fSecondStepTime = 1.2f;
        //            if (frameCanvasGroup.alpha < 0.1f)
        //            {
        //                frameCanvasGroup.alpha += (0.1f * Time.deltaTime / fFirstStepTime);
        //            }
        //            else
        //            {
        //                frameCanvasGroup.alpha += (0.9f * Time.deltaTime / fSecondStepTime);
        //            }

        //            if (frameCanvasGroup.alpha >= 1f)
        //            {
        //                // 恢复选中光效
        //                UButtonGroupItem item = shopPageButtonGroup.SelectedItem;
        //                if (item)
        //                {
        //                    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_pageBtnEffectParam);
        //                    m_pageBtnEffectParam.AttachParent = item.transform;
        //                    UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_pageBtnEffectParam);
        //                }
        //            }
        //        }
        //    }
        //}

        public void SetPageBtnSelect(int nPageType)
        {
            if (nPageType >= 0 && nPageType < shopPageBtn.Length)
            {
                shopPageBtn[nPageType].Selected = true;
            }
        }

        public void OnPageBtnClick(UButtonGroupItem item, bool bActive)
        {
            if (!bActive)
                return;

            //if (payPageItem != null && payPageItem.gameObject.activeInHierarchy)
            //    payPageItem.ShowPayPage(false);

            //if (frameCanvasGroup.alpha >= 1f)
            //{
            //    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_pageBtnEffectParam);
            //    m_pageBtnEffectParam.AttachParent = item.transform;
            //    UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_pageBtnEffectParam);
            //}

            int nCurSelIndex = shopPageButtonGroup.SelectedIndex;
            if (nCurSelIndex < 0)
                return;

            int nTargetPageType = (int)shopPageBtnType[nCurSelIndex];
            for (int i = 0; i < shopPageItem.Length; ++i)
            {
                if (i == nTargetPageType)
                {
                    shopPageItem[i].SetCurPageType(nCurSelIndex);
                    shopPageItem[i].SetVisible(true);
                }
                else
                {
                    shopPageItem[i].SetVisible(false);
                }
            }
        }

        public void ChangeADSearchPage()
        {
            for (int i = 0; i < shopPageItem.Length; ++i)
            {
                if (i == (int)PointShopPageType.Page_Normal)
                {
                    shopPageItem[i].SetCurPageType((int)PointShopPageBtnType.PageBtn_AD);
                    shopPageItem[i].SetVisible(true);
                }
                else
                {
                    shopPageItem[i].SetVisible(false);
                }
            }

            shopPageButtonGroup.GroupMustHadSelected = false;
            for (int i = 0; i < shopPageBtn.Length; ++i)
            {
                shopPageBtn[i].Selected = false;
            }
            shopPageButtonGroup.GroupMustHadSelected = true;
        }

        public void OnCloseBtnClick()
        {
            m_wnd.SetVisible(false);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPREFLUSHCARD_CLOSE, null);
        }

        public void OpenBuyWnd(int nSellID)
        {
            if (nSellID <= 0)
                return;

            PointShopBuyGoodsInitData uiData = new PointShopBuyGoodsInitData();
            uiData.nSellID = nSellID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN, uiData);
        }

        public void UpdateCardList()
        {
            if (shopPageItem[(int)PointShopPageType.Page_Card].IsVisible())
            {
                shopPageItem[(int)PointShopPageType.Page_Card].OnReflush();
            }
        }

        public void OnHeroInfoUpdate()
        {
            if (shopPageItem[(int)PointShopPageType.Page_Hero].IsVisible())
            {
                shopPageItem[(int)PointShopPageType.Page_Hero].OnReflush();
            }
            UpdateCardList();
        }

        public void SetSearchList(List<int> nSellIDList)
        {
            m_searchList.Clear();
            foreach (var nSellID in nSellIDList)
            {
                SSchemePointShopGoodsConfig goodsConfig = PointShopConfig.Instance.GetPointShopGoodsConfig(nSellID);
                if (goodsConfig != null)
                    m_searchList.Add(goodsConfig);
            }
        }

        //internal void PlayOpenAnim()
        //{
        //    if (m_bClosing)
        //        return;

        //    //bool curVisible = IsVisible();


        //    //if (IsVisible() && !curVisible)
        //    {
        //        base.SetRendererVisible(true);

        //        UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_pageBtnEffectParam);

        //        frameCanvasGroup.alpha = 0f;
        //        UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_cloudEffectParam);
        //        UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_cloudEffectParam);
        //    }
        //}
        //internal void PlayCloseAnim()
        //{
        //    if (m_bClosing)
        //        return;

        //    //if (_bVisible && !curVisible)

        //    //else
        //    {
        //        m_bClosing = true;
        //        base.SetRendererVisible(true);

        //        UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_pageBtnEffectParam);

        //        UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_cloudEffectParam);
        //        UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_cloudEffectParam);

        //        // 不立刻执行隐藏
        //        // base.SetVisible(visible);
        //    }
        //}
    }
}
