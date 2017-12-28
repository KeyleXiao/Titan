/*******************************************************************
** 文件名:	UViewHeroDetail.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-11-21
** 版  本:	1.0
** 描  述:	英雄详细信息界面
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using GameLogic;
using DataCenter;
using UnityEngine;
using U3D_Render.Common;
using System.Runtime.InteropServices;
using UnityEngine.UI;
using UIWidgets;
using ASpeedGame.Data.PrizeUnlockConfig;
using Data.ActorPrizeConfig;
using USpeedUI.UExtensions;
using Data.PointShopConfig;
using USpeedUI.PointShop;
using USpeedUI.UEffect;
using ASpeedGame.Data.HeroTalentConfig;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using UnityEngine.EventSystems;
using ASpeedGame.Data.HeroXPskillConfig;
using DG.Tweening;
using USpeedUI.UWidgets;
using USpeedUI.LegendCup;
using ASpeedGame.Data.HeroTypeConfig;
using Data.PersonModel;
using USpeedUI.TooltipAndDialog;
using Data.UISoundConfig;
namespace USpeedUI.HeroDetail
{
    public interface IHeroDetailPartFrameBase
    {
        bool Init();

        void OnDestroy();

        // 暂时没用 预留接口
        bool SetParent(Transform parentsTrans);

        void Reset();

        void OnHeroDetailFrameClick(bool bSelect);
    }

    public abstract class HeroDetailPartFrameBase : MonoBehaviour, IHeroDetailPartFrameBase
    {
        public virtual bool Init()
        {
            return true;
        }

        public virtual void OnDestroy()
        {
            return;
        }

        public virtual bool SetParent(Transform parentsTrans)
        {
            return true;
        }

        public virtual void Reset()
        {
            return;
        }

        public virtual void OnHeroDetailFrameClick(bool bSelect)
        {
            return;
        }

        public virtual bool checkIsBigUpstar()
        {
            bool bIsBig = false;

            cmd_entity_hero_detail_info cmdInfo = HeroDetailViewData.Instance.info.heroDetailInfo;

            if (!PrizeUnlockConfig.Instance.heroStarPrizeUnlockDic.ContainsKey(cmdInfo.nHeroID))
                return false;
            List<HeroStarPrizeUnlockItem> starPrizeUnlockList = PrizeUnlockConfig.Instance.heroStarPrizeUnlockDic[cmdInfo.nHeroID];
            if (starPrizeUnlockList == null)
                return false;
            HeroStarPrizeUnlockItem prizeConfig = null;
            foreach (var item in starPrizeUnlockList)
            {
                if (item.nUnlockLevel > cmdInfo.nStar)
                {
                    prizeConfig = item;
                    break;
                }
            }
            if (prizeConfig == null)
                return false;
            bIsBig = (cmdInfo.nStar + 1 == prizeConfig.nUnlockLevel);

            return bIsBig;
        }
    }

    #region// Left
    public class HeroDetailSkinPhotoItem : MonoBehaviour, IPointerEnterHandler, IPointerExitHandler, IPointerDownHandler, IPointerUpHandler
    {
        public Image SkinMask;
        public Image SkinImage;
        public Image SkinGradeIcon;
        public Text SkinNameText;
        public Text SkinProssesText;
        
        private int m_nSkinID = 0;
        public int SkinID { get {return m_nSkinID;} }
        private bool m_bClick;
        public bool BClick { get { return m_bClick; } set { m_bClick = value; } }
        private bool m_bEnter;
        private bool m_bClicking;

        public void UpdateItem(HeroDetailSkinPhotoFrame parent, UTileViewItemRoomHeroSkin data)
        {
            this.gameObject.SetActive(true);
            m_nSkinID = data.nSkinId;
            int nSkinGrade = data.nSkinType;

            // 基础设置
            SkinMask.gameObject.SetActive(false);
            SkinImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_GAMEWAITINGROOMVIEW, data.nSkinIconId, 8, data.nHeroID);

            if (nSkinGrade >= 0 && nSkinGrade < parent.SkinFrameIcon.Length)
                SkinGradeIcon.sprite = parent.SkinFrameIcon[nSkinGrade];
            SkinGradeIcon.gameObject.SetActive(false);

            string[] szWord = ULocalizationService.Instance.Get("UIView", "Common", "SkinGradeDesc").Split(';');
            if (nSkinGrade >= 0 && nSkinGrade < szWord.Length)
                SkinNameText.text = String.Format("{0}({1})", data.szSkinName, szWord[nSkinGrade]);
            string[] colorList = new string[4] { "FFFFFF", "48b7fe", "97149C", "ffca27" };//白 蓝 紫 金
            if (nSkinGrade >= 0 && nSkinGrade < szWord.Length)
            {
                Color32 colorItem = ColorExtension.FromHex(colorList[nSkinGrade]);
                SkinNameText.color = colorItem;
            }

            string strTip = "";
            SkinInfoScheme skinConfig = new SkinInfoScheme();
            if (GameLogicAPI.GetHeroSkinInfo(m_nSkinID, ref skinConfig))
            {
                strTip = skinConfig.szSkinTip;
            }
            UpdatePossessState(data.dwSkinLimitTime, data.bIsHaveSkin, strTip);
        }

        public void ClearItem()
        {
            this.GetComponent<Toggle>().isOn = false;
            this.gameObject.SetActive(false);
        }

        public void OnPointerEnter(PointerEventData eventData)
        {
            m_bEnter = true;
            UpdateItemState();
        }

        public void OnPointerExit(PointerEventData eventData)
        {
            m_bEnter = false;
            UpdateItemState();
        }

        public void OnPointerDown(PointerEventData eventData)
        {
            m_bClicking = true;
            UpdateItemState();
        }

        public void OnPointerUp(PointerEventData eventData)
        {
            m_bClicking = false;
            UpdateItemState();
        }

        public void UpdatePossessState(uint dwSkinLimitTime, bool bIsHaveSkin, string strSkinTip)
        {
            string days = ULocalizationService.Instance.Get("UIView", "Common", "Days");
            string hours = ULocalizationService.Instance.Get("UIView", "Common", "Hour");
            string minutes = ULocalizationService.Instance.Get("UIView", "Common", "Minute");
            string szProssesText = "";
            Color prossesColor = Color.white;
            if (bIsHaveSkin)
            {
                if (dwSkinLimitTime > 0)
                {
                    DateTime calcTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(dwSkinLimitTime);
                    calcTime = calcTime.ToLocalTime();

                    TimeSpan ts = calcTime.Subtract(DateTime.Now).Duration();
                    if (ts.Days > 0)
                        szProssesText = ts.Days.ToString() + days + ts.Hours.ToString() + hours;
                    else
                        szProssesText = ts.Hours.ToString() + hours + ts.Minutes.ToString() + minutes;
                    prossesColor = ColorExtension.FromHex("ffffff");
                }
                else
                {
                    szProssesText = ULocalizationService.Instance.Get("UIView", "Common", "Possess");
                    prossesColor = ColorExtension.FromHex("ffca27");
                }
            }
            else
            {
                szProssesText = ULocalizationService.Instance.Get("UIView", "Common", "NotPossess");
                // 未拥有，优先读脚本
                if (!string.IsNullOrEmpty(strSkinTip))
                {
                    szProssesText = strSkinTip;
                }

                prossesColor = ColorExtension.FromHex("ffffff");
            }

            SkinProssesText.text = szProssesText;
            SkinProssesText.color = prossesColor;
        }

        public void UpdateItemState()
        {
            float fTime = 0.5f;
            if (m_bEnter || m_bClick)
            {
                SkinImage.transform.DOScale(1.1f, fTime);

                SkinGradeIcon.gameObject.SetActive(true);
                SkinGradeIcon.transform.DOScale(1.1f, fTime);

            }
            else
            {
                SkinImage.transform.DOScale(1f, fTime);

                SkinGradeIcon.gameObject.SetActive(false);
                SkinGradeIcon.transform.DOScale(1f, fTime);
            }

            SkinMask.gameObject.SetActive(m_bClicking);
        }
    }

    public class HeroDetailSkinPhotoFrame : HeroDetailPartFrameBase
    {
        public Transform SkinListTrans;
        public GameObject TmpSlotItemObj;
        public Sprite[] SkinFrameIcon;

        public GameObject PrePageBtn;
        public GameObject NextPageBtn;
        public Text PageNum;

        public URichText ChangeSkinRT;
        public HeroDetailCommomFrame CommonFrame;

        private HeroDetailSkinPhotoItem[] SlotItemList;
        private const int SKIN_MAX_COUNT = 5;
        private int m_nSelectedSkinID = -1;
        public int SelectedSkinID{ get { return m_nSelectedSkinID; } }
        private int m_nCurPage = 1;

        public override bool Init()
        {
            base.Init();

            UpdateSlotList();

            ChangeSkinRT.text = "<a href=ChangeSkin color=ffca27>" + ULocalizationService.Instance.Get("UIView", "HeroDetailView", "ChangeSkin") + "</a>";
            ChangeSkinRT.onHrefClick.AddListener(OnClickChangeSkinRT);
            ChangeSkinRT.gameObject.SetActive(LogicDataCenter.playerSystemDataManager.bRequestSelf);
            
            return true;
        }

        public override void OnDestroy()
        {
            base.OnDestroy();
        }

        public override void Reset()
        {
            base.Reset();

            m_nSelectedSkinID = -1;
        }

        public void DoDetailHandel(BoolDetailHandel boolHandel, VoidDetailHandel voidHandel)
        {
            if (boolHandel != null)
            {
                boolHandel();
            }
            else if (voidHandel != null)
            {
                voidHandel();
            }
        }

        public void UpdateSlotList()
        {
            DoInitSlotList();

            if (SlotItemList == null)
                return;

            foreach (HeroDetailSkinPhotoItem item in SlotItemList)
            {
                if (item != null)
                    item.ClearItem();
            }

            int nHeroID = (UInt16)HeroDetailViewData.Instance.nHeroID;
            cmd_entity_hero_detail_info info = HeroDetailViewData.Instance.info.heroDetailInfo;

            PersonSkinModelItem modelData = new PersonSkinModelItem();
            if (!GameLogicAPI.GetPersonModel((UInt16)HeroDetailViewData.Instance.nHeroID, ref modelData))
                return;

            int nIndex = 0;
            for (int j = (m_nCurPage - 1) * SKIN_MAX_COUNT; j < modelData.nSkinIDList.Length && nIndex < SlotItemList.Length; ++j)
            {
                int nSkinID = modelData.nSkinIDList[j];
                
                SkinInfoScheme skinConfig = new SkinInfoScheme();
                if (!GameLogicAPI.GetHeroSkinInfo(nSkinID, ref skinConfig))
                    continue;

                UTileViewItemRoomHeroSkin skinItemData = new UTileViewItemRoomHeroSkin();
                cmd_Hero_Skin_Info skinCmd = new cmd_Hero_Skin_Info();
                if (LogicDataCenter.heroSkinDataManager.GetHeroSkinInfo(nHeroID, nSkinID, ref skinCmd))
                {
                    skinItemData.bIsHaveSkin = true;
                    skinItemData.dwSkinLimitTime = skinCmd.dwDurationTime;
                }
                else
                {
                    skinItemData.bIsHaveSkin = (info.bIsHaveHero && skinConfig.nSkinLevel == (int)SKIN_LEVEL_TYPE.ESLT_NORMAL) ? true : false;
                    skinItemData.dwSkinLimitTime = 0;
                }

                skinItemData.nHeroID = nHeroID;
                skinItemData.bSelect = false;
                skinItemData.nSkinId = nSkinID;
                skinItemData.nSkinType = skinConfig.nSkinLevel;
                skinItemData.nSkinIconId = skinConfig.nSkinIconID;
                skinItemData.szSkinName = skinConfig.szSkinName;
                SlotItemList[nIndex++].UpdateItem(this, skinItemData);
            }

            // 动态处理ChangeSkinRT锚点位置
            ChangeSkinRT.rectTransform.SetParent(SkinListTrans, true);
            Vector3 startVec = SlotItemList[0].gameObject.GetComponent<RectTransform>().anchoredPosition;
            float fHeight = SlotItemList[0].gameObject.GetComponent<RectTransform>().sizeDelta.y;
            startVec.y -= nIndex * fHeight;
            ChangeSkinRT.rectTransform.anchoredPosition = startVec;
            ChangeSkinRT.rectTransform.SetParent(this.gameObject.GetComponent<RectTransform>(), true);

            // 页处理
            PrePageBtn.SetActive(m_nCurPage > 1);
            NextPageBtn.SetActive(modelData.nSkinIDList.Length > m_nCurPage * SKIN_MAX_COUNT);
            int nMaxPage = (modelData.nSkinIDList.Length % SKIN_MAX_COUNT == 0) ? modelData.nSkinIDList.Length / SKIN_MAX_COUNT : modelData.nSkinIDList.Length / SKIN_MAX_COUNT + 1;
            PageNum.gameObject.SetActive(nMaxPage > 1);
            PageNum.text = String.Format("{0}/{1}", m_nCurPage, nMaxPage);

            // 1.当前使用皮肤2.默认皮肤
            int nDefaultSkinID = modelData.nDefaultSkinID;
            int nCurUseSkinID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SKIN);
            List<int> vecSkinID = new List<int>();
            for (int i = 0; i < modelData.nSkinIDList.Length; i++)
			{
                vecSkinID.Add(modelData.nSkinIDList[i]);
			}
            int nDefaultIndex = vecSkinID.FindIndex(item => item == nDefaultSkinID);
            int nCurUseIndex = vecSkinID.FindIndex(item => item == nCurUseSkinID);
            if (nCurUseIndex >= 0)
            {
                m_nSelectedSkinID = nCurUseSkinID;
                SlotItemList[nCurUseIndex].gameObject.GetComponent<Toggle>().isOn = true;
                OnHeroSkinSelect(SlotItemList[nCurUseIndex]);
            }
            else if (nDefaultIndex >= 0)
            {
                m_nSelectedSkinID = nDefaultSkinID;
                SlotItemList[nDefaultIndex].gameObject.GetComponent<Toggle>().isOn = true;
                OnHeroSkinSelect(SlotItemList[nDefaultIndex]);
            }
        }

        public void UpdateSkinPossessState()
        {
            int nHeroID = HeroDetailViewData.Instance.nHeroID;
            bool bIsHaveHero = HeroDetailViewData.Instance.info.heroDetailInfo.bIsHaveHero;

            bool bIsHaveSkin = false;
            uint dwSkinLimitTime = 0;
            cmd_Hero_Skin_Info skinCmd = new cmd_Hero_Skin_Info();
            SkinInfoScheme skinConfig = new SkinInfoScheme();
            foreach (var item in SlotItemList)
            {
                if (item.SkinID <= 0)
                    continue;
                if (!GameLogicAPI.GetHeroSkinInfo(item.SkinID, ref skinConfig))
                    continue;

                if (LogicDataCenter.heroSkinDataManager.GetHeroSkinInfo(nHeroID, item.SkinID, ref skinCmd))
                {
                    bIsHaveSkin = true;
                    dwSkinLimitTime = skinCmd.dwDurationTime;
                }
                else
                {
                    bIsHaveSkin = (bIsHaveHero && skinConfig.nSkinLevel == (int)SKIN_LEVEL_TYPE.ESLT_NORMAL) ? true : false;
                    dwSkinLimitTime = 0;
                }

                item.UpdatePossessState(dwSkinLimitTime, bIsHaveSkin, skinConfig.szSkinTip);
            }
        }

        public void OnClickPreBtn()
        {
            --m_nCurPage;
            UpdateSlotList();
        }

        public void OnClickNextBtn()
        {
            ++m_nCurPage;
            UpdateSlotList();
        }

        public void OnHeroSkinSelect(HeroDetailSkinPhotoItem item)
        {
            if (item == null)
                return;
            if (item.gameObject.GetComponent<Toggle>().isOn == false)
                return;
            if (CommonFrame == null || CommonFrame.SkinModelFrame == null || CommonFrame.BuyBtnFrame == null)
                return;

            m_nSelectedSkinID = item.SkinID;
            CommonFrame.SkinModelFrame.setHeroSkinModel(HeroDetailViewData.Instance.nHeroID, item.SkinID);
            CommonFrame.BuyBtnFrame.SetBuyHeroButton();
            
            // 设置换肤按钮
            foreach (var skinItem in SlotItemList)
            {
                if (skinItem.SkinID != 0 && skinItem.SkinID == item.SkinID)
                {
                    skinItem.BClick = true;
                    skinItem.UpdateItemState();
                }
                else
                {
                    skinItem.BClick = false;
                    skinItem.UpdateItemState();
                }
            }
        }

        public void OnClickChangeSkinRT(string eventName)
        {
            U3D_Render.EntityView evHero = EntityFactory.MainHeroView;
            if(evHero == null)
            {
                return;
            }

            if (eventName == "ChangeSkin")
            {
                CreatureStateMachine pFSM = EntityFactory.MainHero.gameObject.transform.GetComponentInParent<CreatureStateMachine>();
                int nHeroId = HeroDetailViewData.Instance.nHeroID;

                // 统一通过MASKFLAG标识位来控制切换
                int nMaskFlag = evHero.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MASK);
                // 禁止切换英雄和皮肤
                if ((nMaskFlag & (int)ENTITY_MASKFLAG.MASKFLAG_FORBID_CHANGE) == (int)ENTITY_MASKFLAG.MASKFLAG_FORBID_CHANGE)
                {
                    if (pFSM.GetState() == GameLogic.EntityState.Floating)
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CHANGEHERO_FAILED_REASON_FLY);
                    }
                    else
                    {
                        if (LogicDataCenter.pkDataManager.BIsPK)
                        {
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CHANGEHERO_FAILED_REASON_PK);
                        }
                        else
                        {
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_FORBID_CHANGE);
                        }
                    }
                    return;
                }

                // 未选择皮肤
                if (m_nSelectedSkinID <= 0)
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CHANGEHERO_FAIL_NO_SELECT);
                    return;
                }

                // 无英雄 
                if (!HeroDetailViewData.Instance.info.heroDetailInfo.bIsHaveHero)
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CHANGEHERO_FAIL_NO_HERO);

                    int nSellID = PointShopConfig.Instance.GetPointShopSellIDByHeroID(nHeroId);
                    if (nSellID > 0 && LogicDataCenter.pointShopDataManager.CheckCanBuyGoods(nSellID, 1))
                    {
                        PointShopBuyGoodsInitData uiData = new PointShopBuyGoodsInitData();
                        uiData.nSellID = nSellID;
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN, uiData);
                    }
                    return;
                }
                // 有英雄无皮肤
                else if (!LogicDataCenter.pointShopDataManager.checkIsHaveSkin(nHeroId, m_nSelectedSkinID))  
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CHANGEHERO_FAIL_NO_SKIN);

                    int nSellID = PointShopConfig.Instance.GetPointShopSellIDBySkinID(nHeroId, m_nSelectedSkinID);
                    if (nSellID > 0 && LogicDataCenter.pointShopDataManager.CheckCanBuyGoods(nSellID, 1))
                    {
                        PointShopBuyGoodsInitData uiData = new PointShopBuyGoodsInitData();
                        uiData.nSellID = nSellID;
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN, uiData);
                    }
                    return;
                }

                cmd_entity_change_hero cmd_data;
                if (nHeroId != EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION))
                {
                    cmd_data.nOptType = (int)CHANGE_HERO_DATA_TYPE.CHANGE_HERO_TYPE_HERO;
                    cmd_data.nChangeID = nHeroId;
                    cmd_data.nReserveData = m_nSelectedSkinID;   // 切换英雄时顺便把皮肤带上
                    EntityEventHelper.Instance.SendCommand<cmd_entity_change_hero>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQ_CHANGEHERO, ref cmd_data);
                }
                else if (m_nSelectedSkinID != EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SKIN))
                {
                    cmd_data.nOptType = (int)CHANGE_HERO_DATA_TYPE.CHANGE_HERO_TYPE_SKIN;
                    cmd_data.nChangeID = m_nSelectedSkinID;
                    cmd_data.nReserveData = nHeroId;   // 切换皮肤时顺便把英雄带上
                    EntityEventHelper.Instance.SendCommand<cmd_entity_change_hero>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQ_CHANGEHERO, ref cmd_data);
                }
                else
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CHANGEHERO_FAIL_SAME_SKIN);
                }
            }
        }

        private void DoInitSlotList()
        {
            if (SlotItemList != null)
                return;

            float fAnchorY = TmpSlotItemObj.GetComponent<RectTransform>().anchoredPosition.y;
            float fHeight = TmpSlotItemObj.GetComponent<RectTransform>().sizeDelta.y;

            TmpSlotItemObj.SetActive(false);
            SlotItemList = new HeroDetailSkinPhotoItem[SKIN_MAX_COUNT];
            for (int i = 0; i < SKIN_MAX_COUNT; ++i)
            {
                GameObject SlotItemObject = Instantiate<GameObject>(TmpSlotItemObj);
                if (!SlotItemObject)
                    return;

                HeroDetailSkinPhotoItem item = SlotItemObject.GetComponent<HeroDetailSkinPhotoItem>();
                if (!item)
                    return;

                SlotItemObject.transform.SetParent(SkinListTrans, false);
                Vector3 vec = SlotItemObject.GetComponent<RectTransform>().anchoredPosition;
                vec.y = fAnchorY - fHeight * i;
                SlotItemObject.GetComponent<RectTransform>().anchoredPosition = vec;

                SlotItemObject.SetActive(true);
                SlotItemList[i] = item;
            }
        }
    }
    #endregion

    #region// Middle
    public class HeroDetailCommomFrame : HeroDetailPartFrameBase
    {
        public HeroDetailCommonUpstarBtnFrame UpstarBtnFrame;
        public HeroDetailCommomBuyBtnFrame BuyBtnFrame;
        public HeroDetailSkinModelFrame SkinModelFrame;
        private List<IHeroDetailPartFrameBase> CommomFrameList = new List<IHeroDetailPartFrameBase>();
        public Text HeroStarMaxDesc;
        public Text ViewOtherHeroStarDesc;

        public override bool Init()
        {
            if (UpstarBtnFrame == null || BuyBtnFrame == null || SkinModelFrame == null)
            {
                return false;
            }
            this.gameObject.SetActive(true);

            if (CommomFrameList.Count == 0)
            {
                CommomFrameList.Add(UpstarBtnFrame);
                CommomFrameList.Add(BuyBtnFrame);
                CommomFrameList.Add(SkinModelFrame);
            }

            foreach (IHeroDetailPartFrameBase item in CommomFrameList)
            {
                if (item != null)
                    item.Init();
            }

            // 满星级设置
            string strDesc = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "HeroStarMaxDesc");
            UBB.toHtml(ref strDesc);
            HeroStarMaxDesc.text = strDesc;
            HeroStarMaxDesc.gameObject.SetActive(false);
            // 查看其他星级设置
            ViewOtherHeroStarDesc.gameObject.SetActive(false);
            if (!LogicDataCenter.playerSystemDataManager.bRequestSelf)
            {
                cmd_entity_hero_detail_info info = HeroDetailViewData.Instance.info.heroDetailInfo;
                string str1 = ULocalizationService.Instance.Get("UIView", "Common", "HeroGradeLevel");
                ViewOtherHeroStarDesc.text = String.Format(str1 + ":" + info.chStarDesc + "({0})", info.nStar);
                ViewOtherHeroStarDesc.gameObject.SetActive(true);
            }

            return true;
        }

        public override void OnDestroy()
        {
            foreach (IHeroDetailPartFrameBase item in CommomFrameList)
            {
                if (item != null)
                    item.OnDestroy();
            }

            HeroStarMaxDesc.gameObject.SetActive(false);
        }

        public override void Reset()
        {
            foreach (IHeroDetailPartFrameBase item in CommomFrameList)
            {
                if (item != null)
                    item.Reset();
            }

            HeroStarMaxDesc.gameObject.SetActive(false);
        }

        public void DoDetailHandel(BoolDetailHandel boolHandel, VoidDetailHandel voidHandel)
        {
            if (boolHandel != null)
            {
                boolHandel();
            }
            else if (voidHandel != null)
            {
                voidHandel();
            }
        }

        public void ShowUpStarSucess()
        {
            UpstarBtnFrame.OnUpStarSucess();
        }

        public void UpdateUpStarInfo(cmd_entity_hero_diff_expend cmdInfo)
        {
            UpstarBtnFrame.UpdateUpStarInfo(cmdInfo);
            if (cmdInfo.nStarLv == (int)EHeroStar.HERO_STAR_LEVEL_MAX)
            {
                ViewOtherHeroStarDesc.gameObject.SetActive(false);
                UpstarBtnFrame.gameObject.SetActive(false);
                HeroStarMaxDesc.gameObject.SetActive(true);
            }
        }

        public void UpdateUpStarGoldView(int nHaveGold)
        {
            UpstarBtnFrame.UpdateUpStarGoldView(nHaveGold);
        }

        public void SetHeroSkinModel(int nHeroID)
        {
            SkinModelFrame.setHeroSkinModel(nHeroID);
        }
    }

    public class PointDetailHeroPageDragHeroTool : MonoBehaviour, IBeginDragHandler, IDragHandler, IEndDragHandler
    {
        private HeroDetailSkinModelFrame m_parent;
        private bool m_draging;
        private Vector3 m_lastPos;

        public void SetParent(HeroDetailSkinModelFrame parent)
        {
            m_parent = parent;
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
                float fDeltaX = eventData.position.x - m_lastPos.x;
                if (m_parent)
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

    public class HeroDetailSkinModelFrame : HeroDetailPartFrameBase
    {
        private Transform m_SelectHeroTransform;
        public Transform SelectTransForm
        {
            set { m_SelectHeroTransform = value; }
            get { return m_SelectHeroTransform; }
        }
        private SceneEffectParamBase m_LastSkinEffectParam;
        public SceneEffectParamBase LastSkinEffectParam
        {
            get { return m_LastSkinEffectParam; }
            set { m_LastSkinEffectParam = value; }
        }
        public PointDetailHeroPageDragHeroTool m_DragTool;

        public Text HeroName;
        public Text HeroTitle;

        private int m_nLastSkinId;
        public int LastSkinId
        {
            get { return m_nLastSkinId; }
        }
        private int m_nHeroID;

        private int m_nSoundID;

        public override void OnDestroy()
        {
            base.OnDestroy();
            ClearSelectSkinModel();
            m_nHeroID = 0;
        }

        public override bool Init()
        {
            if (m_nHeroID == HeroDetailViewData.Instance.nHeroID)
                return true;

            m_nHeroID = HeroDetailViewData.Instance.nHeroID;
            m_nSoundID = 0;
            if (m_DragTool)
                m_DragTool.SetParent(this);

            SSchemePersonModelData sPersonModel = null;
            if (PersonModelConfig.Instance.GetData_Out(m_nHeroID, out sPersonModel))
            {
                HeroName.text = sPersonModel.szHeroName;
                HeroTitle.text = sPersonModel.szHeroTitle;
            }
                
            return true;
        }

        public override void Reset()
        {
            base.Reset();

            m_nLastSkinId = 0;
            m_nHeroID = 0;
        }

        private void ClearSelectSkinModel()
        {
            if (m_LastSkinEffectParam == null)
            {
                return;
            }

            SceneEffectManager.Instance.DestroySceneEffect(ref m_LastSkinEffectParam);
            m_LastSkinEffectParam = null;
            m_nLastSkinId = 0;

            SoundManager.DeleteSound(m_nSoundID);
        }

        public void setHeroSkinModel(int nParamHeroID = -1, int nSkinID = -1)
        {
            nParamHeroID = nParamHeroID == -1 ? m_nHeroID : nParamHeroID;
            if (nParamHeroID <= 0)
                return;

            int nParamSkinID = nSkinID;
            if (nSkinID == -1)
            {
                // 默认皮肤
                PersonSkinModelItem modelData = new PersonSkinModelItem();
                GameLogicAPI.GetPersonModel((UInt16)nParamHeroID, ref modelData);
                nParamSkinID = modelData.nDefaultSkinID;
                // 当前使用皮肤
                int nCurSkinID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SKIN);
                foreach (var item in modelData.nSkinIDList)
                {
                    if (item == nCurSkinID)
                    {
                        nParamSkinID = item;
                        break;
                    }
                }
            }

            if (m_nLastSkinId > 0 && m_nLastSkinId == nParamSkinID)
                return;

            if (SelectTransForm == null)
                return;

            // 音效
            SkinInfoScheme skinConfig = new SkinInfoScheme();
            if (GameLogicAPI.GetHeroSkinInfo(nParamSkinID, ref skinConfig))
            {
                SoundManager.DeleteSound(m_nSoundID);
                m_nSoundID = skinConfig.nSoundID;
            }

            // 光效
            SkinManager.SetChangeHeroSkinEffect(nParamSkinID, ref m_LastSkinEffectParam, ref m_nLastSkinId, SelectTransForm, ASpeedGame.Data.PersonModelTransFormInfo.PERSONMODELVIEWID.PERSONMODELVIEWID_GAMEWAITINGROOM, OnSkinLoadFinish, this);
            m_nHeroID = nParamHeroID;
        }

        private void OnSkinLoadFinish(object obj)
        {
            SoundManager.CreateUISound(m_nSoundID, false);
        }

        public void RotateHeroModel(float fDelta)
        {
            if (m_LastSkinEffectParam == null || m_LastSkinEffectParam.ReturnComponent == null)
            {
                return;
            }

            Vector3 rotation = m_LastSkinEffectParam.ReturnComponent.transform.localEulerAngles;
            rotation = new Vector3(rotation.x, rotation.y + fDelta, rotation.z);
            m_LastSkinEffectParam.ReturnComponent.transform.rotation = Quaternion.Euler(rotation);
        }

        public void ScaleHeroModel(float fDelta)
        {
            if (m_LastSkinEffectParam == null || m_LastSkinEffectParam.ReturnComponent == null)
            {
                return;
            }

            Vector3 scale = m_LastSkinEffectParam.ReturnComponent.transform.localScale;
            scale = new Vector3(scale.x + fDelta, scale.y + fDelta, scale.z + fDelta);
            m_LastSkinEffectParam.ReturnComponent.transform.localScale = scale;
        }

    }

    public class HeroDetailCommomBuyBtnFrame : HeroDetailPartFrameBase
    {
        public Text BuyOrSwitchText;
        public Text UnlockHeroDesc;

        public HeroDetailSkinPhotoFrame skinPhotoFrame;
        public HeroDetailCommonUpstarBtnFrame upstarModelFrame;

        public override bool Init()
        {
            SetBuyHeroButton();

            return base.Init();
        }

        public void OnClickBuyHero()
        {
            cmd_entity_hero_detail_info info = HeroDetailViewData.Instance.info.heroDetailInfo;
            PersonSkinModelItem modelData = new PersonSkinModelItem();
            if (!GameLogicAPI.GetPersonModel((UInt16)info.nHeroID, ref modelData))
                return;

            int nSelectSkinID = -1;
            if (skinPhotoFrame != null)
            {
                nSelectSkinID = skinPhotoFrame.SelectedSkinID;
                if (nSelectSkinID <= 0)
                {
                    nSelectSkinID = modelData.nDefaultSkinID;
                }
            }
            bool bIsHaveSkin = LogicDataCenter.pointShopDataManager.checkIsHaveSkin(info.nHeroID, nSelectSkinID);
            bool bIsDefaultSkin = nSelectSkinID == modelData.nDefaultSkinID;

            if (info.bIsHaveHero)
            {
                if (!bIsHaveSkin)  // 有英雄无皮肤
                {
                    int nSellID = PointShopConfig.Instance.GetPointShopSellIDBySkinID(info.nHeroID, nSelectSkinID);
                    if (nSellID > 0 && LogicDataCenter.pointShopDataManager.CheckCanBuyGoods(nSellID, 1))
                    {
                        PointShopBuyGoodsInitData uiData = new PointShopBuyGoodsInitData();
                        uiData.nSellID = nSellID;
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN, uiData);
                    }
                }
            }
            else    // 无英雄无皮肤
            {
                // 无英雄，点购买皮肤，需要增加提示
                if (!bIsDefaultSkin)
                {
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CHANGEHERO_FAIL_NO_HERO);
                }

                int nSellID = PointShopConfig.Instance.GetPointShopSellIDByHeroID(info.nHeroID);
                if (nSellID > 0 && LogicDataCenter.pointShopDataManager.CheckCanBuyGoods(nSellID, 1))
                {
                    PointShopBuyGoodsInitData uiData = new PointShopBuyGoodsInitData();
                    uiData.nSellID = nSellID;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN, uiData);
                }
            }
        }

        public void SetBuyHeroButton()
        {
            if (BuyOrSwitchText == null || skinPhotoFrame == null || upstarModelFrame == null)
            {
                return;
            }

            int nHeroId = HeroDetailViewData.Instance.nHeroID;
            int nSkinId = skinPhotoFrame.SelectedSkinID;
            if (nSkinId <= 0)
            {
                PersonSkinModelItem modelData = new PersonSkinModelItem();
                if (GameLogicAPI.GetPersonModel((UInt16)nHeroId, ref modelData))
                    nSkinId = modelData.nDefaultSkinID;
            }
            bool bIsHaveHero = HeroDetailViewData.Instance.info.heroDetailInfo.bIsHaveHero;
            bool bIsHaveSkin = LogicDataCenter.pointShopDataManager.checkIsHaveSkin(nHeroId, nSkinId);
            // 优先购买英雄
            if (bIsHaveHero)
            {
                UnlockHeroDesc.gameObject.SetActive(false);
                if (bIsHaveSkin)
                {
                    BuyOrSwitchText.text = ULocalizationService.Instance.Get("UIView", "Common", "SwitchHero");
                    this.gameObject.SetActive(false);
                    upstarModelFrame.gameObject.SetActive(LogicDataCenter.playerSystemDataManager.bRequestSelf && HeroDetailViewData.Instance.info.heroDetailInfo.nStar < (int)EHeroStar.HERO_STAR_LEVEL_MAX);
                }
                else
                {
                    bool bShowInShop = true;
                    int nSellID = PointShopConfig.Instance.GetPointShopSellIDBySkinID(nHeroId, nSkinId);
                    if (nSellID <= 0)
                    {
                        bShowInShop = false;
                    }
                        
                    BuyOrSwitchText.text = ULocalizationService.Instance.Get("UIView", "Common", "SkinUnlock");
                    this.gameObject.SetActive(LogicDataCenter.playerSystemDataManager.bRequestSelf && bShowInShop);
                    upstarModelFrame.gameObject.SetActive(false);
                }
            }
            else
            {
                BuyOrSwitchText.text = ULocalizationService.Instance.Get("UIView", "Common", "HeroUnlock");
                int nUnlockLevel = GameLogicAPI.getUnlockLevel(nHeroId);
                int nCurLevel = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
                UnlockHeroDesc.text = String.Format(ULocalizationService.Instance.Get("UIView", "Common", "UnlockHeroDesc"), nCurLevel, nUnlockLevel - nCurLevel);
                UnlockHeroDesc.gameObject.SetActive(nUnlockLevel > nCurLevel);
                this.gameObject.SetActive(LogicDataCenter.playerSystemDataManager.bRequestSelf);
                upstarModelFrame.gameObject.SetActive(false);
            }
        }
    }

    public class HeroDetailCommonUpstarBtnFrame : HeroDetailPartFrameBase
    {
        public Text upstarBtnDesc;
        public Text heroStarGradeLv;
        
        public GameObject lifeHeroGo;
        public Text lifeHeroDesc;
        public Text expendGold1;
        public Text expendGold2;
        public Text expendGold3;
        public Image expendGoldline;
        public Text expendCard1;
        public Text expendCard2;
        public Text expendCard3;
        public Image expendCardline;

        private Transform m_SelectHeroTransform;
        public Transform SelectTransForm
        {
            set { m_SelectHeroTransform = value; SetBigUpstarEffect(); }
            get { return m_SelectHeroTransform; }
        }

        private cmd_entity_hero_diff_expend m_heroDiffExpend;
        private UEffectParamBase param;
        private SceneEffectParamBase bigUpstarParam;
        private Vector3 BigUpstarEffectOffsetPos = new Vector3(0, 0, -0.5f);

        // 策划要求暂时关闭
        //private bool bShowTip;

        public override bool Init()
        {
            cmd_entity_hero_detail_info info = HeroDetailViewData.Instance.info.heroDetailInfo;
            string str1 = ULocalizationService.Instance.Get("UIView", "Common", "HeroGradeLevel");
            heroStarGradeLv.text = String.Format(str1 + ":" + info.chStarDesc + "({0})", info.nStar);
            bool bHaveHero = LogicDataCenter.playerSystemDataManager.CheckIsHaveHero(info.nHeroID);
            this.gameObject.SetActive(LogicDataCenter.playerSystemDataManager.bRequestSelf && bHaveHero && info.nStar < (int)EHeroStar.HERO_STAR_LEVEL_MAX);

            if (param != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
            }
            param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_HeroDetailInfo_UpstarButton, _tfAttachParent: this.gameObject.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

            if (checkIsBigUpstar())
                upstarBtnDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "UpStarButtonBig");
            else
                upstarBtnDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "UpStarButtonNormal");

            //bShowTip = true;

            lifeHeroDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "LifeHeroDiscount");

            return true;
        }

        // 设置大晋升光效
        public void SetBigUpstarEffect()
        {
            // 是否大晋升
            if (bigUpstarParam != null)
            {
                SceneEffectManager.Instance.DestroySceneEffect(ref bigUpstarParam);
            }
            if (checkIsBigUpstar())
            {
                bigUpstarParam = new SceneEffectParamBase((int)ESceneEffectID.SE_HeroDetailInfo_UpstarBase, m_SelectHeroTransform,_vecPositionOffset: BigUpstarEffectOffsetPos);
                SceneEffectManager.Instance.CreateSceneEffect(ref bigUpstarParam);
            }
        }

        // 当点击确定按钮时向逻辑层请求升星
        public void onClickUpStarButton()
        {
            Dictionary<int, cmd_entity_lifehero_info> lifeHeroDic = LogicDataCenter.lifeHeroDataManager.AllLifeHero;
            if (EntityFactory.MainHeroView == null || lifeHeroDic == null)
                return;

            int nPlayerLv = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            int nHeroID = HeroDetailViewData.Instance.nHeroID;
            bool bLifeHero = lifeHeroDic.ContainsKey(nHeroID);
            // 1.玩家小于10级 2.本命英雄不满 3.该英雄不是本命英雄 引导去添加本命英雄
            if (nPlayerLv < 10 && lifeHeroDic.Count < (int)ELifeHeroType.LIFEHERO_MAX_COUNT && !bLifeHero)
            {
                UIDialogData data = new UIDialogData(title: ULocalizationService.Instance.Get("UIView", "HeroDetailView", "UpstarGuideTitle"),
                message: ULocalizationService.Instance.Get("UIView", "HeroDetailView", "UpstarGuideContent"),
                buttons: new UIWidgets.DialogActions() 
                                { 
                                    { ULocalizationService.Instance.Get("UIView", "HeroDetailView", "UpstarGuideConfirmBtn"), OnConfirmBtnClick },
                                });
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);

                return; 
            }

            // 真实价格
            int nRealCard = m_heroDiffExpend.nExpendCard;
            int nRealGold = m_heroDiffExpend.nExpedGold;
            if (bLifeHero)
            {
                nRealCard = m_heroDiffExpend.nDiscountCard > 0 ? m_heroDiffExpend.nDiscountCard : nRealCard;
                nRealGold = m_heroDiffExpend.nDiscountGold > 0 ? m_heroDiffExpend.nDiscountGold : nRealGold;
            }
            if (nRealCard <= 0 || nRealGold <= 0)
                return;

            if (m_heroDiffExpend.nHaveCardCount < nRealCard)
            {
                UConsumptionMsgData cmdData = new UConsumptionMsgData((int)EConsumptionRequestType.Request_Upstar, (int)EConsumptionGuideType.Guide_HeroCard);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW, cmdData);
                return;
            }
            if (m_heroDiffExpend.nHaveGold < nRealGold)
            {
                UConsumptionMsgData cmdData = new UConsumptionMsgData((int)EConsumptionRequestType.Request_Upstar, (int)EConsumptionGuideType.Guide_Gold);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW, cmdData);
                return;
            }

            SSchemeUSoundData soundData = new SSchemeUSoundData();
            if (UISoundConfig.Instance.GetData("upgrade", "Button_upgrade_click", ref soundData))
            {
                SoundManager.CreateUISound(soundData.USoundID);
            }

            IntPtrHelper helper = new IntPtrHelper();
            IntPtr ptr = helper.toPtr<int>(ref nHeroID);
            int nLen = Marshal.SizeOf(nHeroID);
            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_REQ_UPGRADE_STAR, nLen, "", ptr, nLen);
        }

        private bool OnConfirmBtnClick()
        {
            UISystem.Instance.ShowWnd(WndID.WND_ID_LIFE_HERO_CULTURE, true);

            return true;
        }

        public void UpdateUpStarInfo(cmd_entity_hero_diff_expend cmdInfo)
        {
            m_heroDiffExpend = cmdInfo;

            // 真实价格
            int nRealCard = cmdInfo.nExpendCard;
            int nRealGold = cmdInfo.nExpedGold;
            if (LogicDataCenter.lifeHeroDataManager.AllLifeHero.ContainsKey(HeroDetailViewData.Instance.nHeroID))
            {
                nRealCard = cmdInfo.nDiscountCard > 0 ? cmdInfo.nDiscountCard : nRealCard;
                nRealGold = cmdInfo.nDiscountGold > 0 ? cmdInfo.nDiscountGold : nRealGold;
            }
            if (nRealCard <= 0 || nRealGold <= 0)
                return;

            // 赋值
            expendGold1.text = ULocalizationService.Instance.Get("UIView", "Common", "Gold");
            expendGold2.text = cmdInfo.nExpedGold.ToString();
            expendGold3.text = String.Format("{0}({1}{2})", nRealGold, ULocalizationService.Instance.Get("UIView", "Common", "CurrentHave"), cmdInfo.nHaveGold);
            expendCard1.text = ULocalizationService.Instance.Get("UIView", "Common", "Card");
            expendCard2.text = cmdInfo.nExpendCard.ToString();
            expendCard3.text = String.Format("{0}({1}{2})", nRealCard, ULocalizationService.Instance.Get("UIView", "Common", "CurrentHave"), cmdInfo.nHaveCardCount);
            // 颜色
            if (cmdInfo.nHaveGold < nRealGold)
            {
                expendGold1.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
                expendGold2.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
                expendGold3.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
            }
            else
            {
                expendGold1.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
                expendGold2.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
                expendGold3.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
            }

            if (cmdInfo.nHaveCardCount < nRealCard)
            {
                expendCard1.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
                expendCard2.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
                expendCard3.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
            }
            else
            {
                expendCard1.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
                expendCard2.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
                expendCard3.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
            }
            // 显示与否(本命英雄)
            bool bLifeHero = LogicDataCenter.lifeHeroDataManager.AllLifeHero.ContainsKey(cmdInfo.nHeroID);
            lifeHeroGo.SetActive(bLifeHero && (cmdInfo.nDiscountCard > 0 || cmdInfo.nDiscountGold > 0));
            expendGold2.gameObject.SetActive(bLifeHero && cmdInfo.nDiscountGold > 0);
            expendCard2.gameObject.SetActive(bLifeHero && cmdInfo.nDiscountCard > 0);

            //if (bShowTip)
            //{
            //    if (cmdInfo.nHaveGold >= nRealGold && cmdInfo.nHaveCardCount >= nRealCard)
            //    {
            //        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CAN_UPGRADE_TIP);
            //    }
            //    else
            //    {
            //        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CAN_NOT_UPGRADE_TIP);
            //    }

            //    bShowTip = false;
            //}
        }

        public void UpdateUpStarGoldView(int nHaveGold)
        {
            m_heroDiffExpend.nHaveGold = nHaveGold;

            // 真实价格
            int nRealGold = m_heroDiffExpend.nExpedGold;
            if (LogicDataCenter.lifeHeroDataManager.AllLifeHero.ContainsKey(HeroDetailViewData.Instance.nHeroID))
            {
                nRealGold = m_heroDiffExpend.nDiscountGold > 0 ? m_heroDiffExpend.nDiscountGold : nRealGold;
            }
            if (nRealGold <= 0)
                return;

            // 赋值
            expendGold3.text = String.Format("{0}({1}{2})", nRealGold, ULocalizationService.Instance.Get("UIView", "Common", "CurrentHave"), nHaveGold);
            // 颜色
            if (nHaveGold < nRealGold)
            {
                expendGold1.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
                expendGold2.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
                expendGold3.color = UDefines.CommomColor(ECommonColor.ECC_Red1);
            }
            else
            {
                expendGold1.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
                expendGold2.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
                expendGold3.color = UDefines.CommomColor(ECommonColor.ECC_Gold1);
            }
        }

        public void OnUpStarSucess()
        {
            cmd_entity_hero_detail_info info = HeroDetailViewData.Instance.info.heroDetailInfo;
            string str1 = ULocalizationService.Instance.Get("UIView", "Common", "HeroGradeLevel");
            heroStarGradeLv.text = String.Format(str1 + ":" + info.chStarDesc + "({0})", info.nStar);

            // 是否大晋升
            SetBigUpstarEffect();

            if (checkIsBigUpstar())
                upstarBtnDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "UpStarButtonBig");
            else
                upstarBtnDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "UpStarButtonNormal");
        }

        public override void OnDestroy()
        {
            base.OnDestroy();

            if (param != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
            }
            if (bigUpstarParam != null)
            {
                SceneEffectManager.Instance.DestroySceneEffect(ref bigUpstarParam);
            }

            //bShowTip = false;
        }
    }
    #endregion

    #region// Right
    public enum EHeroDetailPartsType
    {
        EHDPT_PANDECT,
        EHDPT_SPELL,
        EHDPT_TALENT,
        EHDPT_XP,
        EHDPT_BACKDRO,
        EHDPT_GOODSDISPOSITION,
    }

    public class HeroDetailPartsFrame : HeroDetailPartFrameBase
    {
        public Image StarRankImage;
        public Toggle[] ComToggleGroup;

        public HeroDetailPandectFame PandectFame;
        public HeroDetailSkillFrame SkillFrame;
        public HeroDetailHeroTalentFrame TalentFrame;
        public HeroDetailXpFrame XpFrame;
        public HeroDetailBackdropFrame BackdropFrame;

        private List<IHeroDetailPartFrameBase> PartFrameList = new List<IHeroDetailPartFrameBase>();

        public override bool Init()
        {
            if (StarRankImage == null || ComToggleGroup == null || PandectFame == null || SkillFrame == null || TalentFrame == null || XpFrame == null || BackdropFrame == null)
            {
                return false;
            }
            this.gameObject.SetActive(true);

            if (PartFrameList.Count == 0)
            {
                PartFrameList.Add(PandectFame);
                PartFrameList.Add(SkillFrame);
                PartFrameList.Add(TalentFrame);
                PartFrameList.Add(XpFrame);
                PartFrameList.Add(BackdropFrame);
            }

            #region 2017.6.2 大boss说目前不开放这功能
            if (ComToggleGroup.Length > (int)EHeroDetailPartsType.EHDPT_GOODSDISPOSITION)
                ComToggleGroup[(int)EHeroDetailPartsType.EHDPT_GOODSDISPOSITION].gameObject.SetActive(false);
            #endregion

            foreach (IHeroDetailPartFrameBase item in PartFrameList)
            {
                if (item != null)
                    item.Init();
            }

            foreach (Toggle item in ComToggleGroup)
            {
                if (item != null)
                    item.isOn = false;
            }
            if ((int)HeroDetailViewData.Instance.eType >= 0 && (int)HeroDetailViewData.Instance.eType < ComToggleGroup.Length)
            {
                if ((int)HeroDetailViewData.Instance.eType == (int)EHeroDetailPartsType.EHDPT_PANDECT)
                    XpFrame.ClearHeroXpGuide();

                ComToggleGroup[(int)HeroDetailViewData.Instance.eType].isOn = true;
            }

            cmd_entity_hero_detail_info info = HeroDetailViewData.Instance.info.heroDetailInfo;
            int nStarIconID = GameLogicAPI.getHeroStarIconID(info.nHeroID, info.nStar);
            if (nStarIconID >= 0)
            {
                StarRankImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeroStarIcon, "HeroStarFrame" + nStarIconID, WndID.WND_ID_HERODETAIL);
                StarRankImage.gameObject.SetActive(true);
            }
            else
            {
                StarRankImage.gameObject.SetActive(false);
            }

            return true;
        }

        public override void OnDestroy()
        {
            foreach (IHeroDetailPartFrameBase item in PartFrameList)
            {
                if (item != null)
                    item.OnDestroy();
            }
        }

        public bool SetParent()
        {
            foreach (IHeroDetailPartFrameBase item in PartFrameList)
            {
                if (item != null)
                    item.SetParent(this.transform);
            }

            return true;
        }

        public override void Reset()
        {
            foreach (IHeroDetailPartFrameBase item in PartFrameList)
            {
                if (item != null)
                    item.Reset();
            }
        }

        public void DoDetailHandel(BoolDetailHandel boolHandel, VoidDetailHandel voidHandel)
        {
            if (boolHandel != null)
            {
                boolHandel();
            }
            else if (voidHandel != null)
            {
                voidHandel();
            }
        }

        public void SetHeroStarData(cmd_entity_hero_diff_expend cmdData)
        {
            if (PandectFame)
                PandectFame.SetHeroStarData(cmdData);
        }

        public void ShowUpStarSucess()
        {
            // 显示奖励界面
            LogicDataCenter.playerSystemDataManager.ShowHeroUpStarUnlockPrize();

            // 更新奖励图标
            if (PandectFame)
                PandectFame.SetHeroStarPrize();

            // 更新星级图标
            cmd_entity_hero_detail_info info = HeroDetailViewData.Instance.info.heroDetailInfo;
            int nStarIconID = GameLogicAPI.getHeroStarIconID(info.nHeroID, info.nStar);
            if (nStarIconID >= 0)
            {
                StarRankImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeroStarIcon, "HeroStarFrame" + nStarIconID, WndID.WND_ID_HERODETAIL);
                StarRankImage.gameObject.SetActive(true);
            }
        }

        public void OnToggleHeroGoodsDispositionView(bool _bIsOn)
        {
            if(_bIsOn)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTHERO_External, new UIMsgCmdData((int)WndMsgID.WND_MSG_GOODSDISPOSITION_SET_CURRENTHERO_External, HeroDetailViewData.Instance.nHeroID, string.Empty, IntPtr.Zero, 0));
            }            
        }

        public void OnUpdateHeroTalent()
        {
            if(TalentFrame.gameObject.activeSelf)
            {
                TalentFrame.InitShowDataInfo();
            }
        }
    }

    // 总览
    public class HeroDetailPandectFame : HeroDetailPartFrameBase
    {
        public Text PlayTipDesc;
        public Text UpStarDesc;
        public Text UpstarPrizeDesc;

        public Text PlayWayDesc;
        public Text UseCountDesc;
        public Text WinRateDesc;
        public Text MilitaryExploitsDesc;

        public Text BaseDataDesc;
        public Text GrowDataDesc;
        public Text HpDesc;
        public Text PhysicAttackDesc;
        public Text PhysicDefDesc;
        public Text MpDesc;
        public Text MagicAttackDesc;
        public Text MagicDefDesc;

        // 介绍
        public Text UseCountText;
        public Text PlayerWaysText;
        public Text WinRateText;
        public Text MilitaryExploitsText;
        public Image[] ScoreLvImage;

        // 属性
        public UTooltipTrigger propertyTip;
        public Text[] Hp;
        public Text[] PhysicAttack;
        public Text[] PhysicDef;
        public Text[] Mp;
        public Text[] MagicAttack;
        public Text[] MagicDef;

        // 奖励
        public GameObject PrizeFrame;
        public Text UpStarPrizeDesc;
        public RectTransform UpStarPrizeFrame;
        public GameObject UpStarPrizeItem;
        private List<LegendCupPrizeItem> m_upstarPrizeList;

        public override bool Init()
        {
            UpStarDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "HeroUpstar");
            UpstarPrizeDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "UpstarPrizeDesc");

            PlayWayDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "PlayWayDesc");
            UseCountDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "UseCountDesc");
            WinRateDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "WinRate");
            MilitaryExploitsDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "WarRecord");

            BaseDataDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "BaseDataDesc");
            GrowDataDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "GrowDataDesc");
            HpDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "HpDesc");
            PhysicAttackDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "PhysicAttackDesc");
            PhysicDefDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "PhysicDefDesc");
            MpDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "MpDesc");
            MagicAttackDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "MagicAttackDesc");
            MagicDefDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "MagicDefDesc");

            propertyTip.SetText(UTooltipParamName.BodyText, ULocalizationService.Instance.Get("UIView", "HeroDetailView", "PropertyTip"));

            m_upstarPrizeList = new List<LegendCupPrizeItem>();
            SSchemeHeroTypeConfig config = HeroTypeConfig.Instance.GetHeroTypeConfig(HeroDetailViewData.Instance.info.heroDetailInfo.nHeroID);
            if (config != null)
                PlayTipDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "HeroPlayTip") + config.strPlayWay;

            SetHeroIntroduce();
            SetHeroStarPrize();

            // 请求数据
            int nHeroID = HeroDetailViewData.Instance.nHeroID;
            IntPtrHelper helper = new IntPtrHelper();
            cmd_get_hero_detail_info data = new cmd_get_hero_detail_info();
            data.nHeroID = nHeroID;
            data.nStarLv = 1;
            data.nStaticLv = 1;
            ActorHeroInfo info = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
            if (info.dicEntityHeroInfo.ContainsKey(nHeroID))
            {
                data.nStarLv = info.dicEntityHeroInfo[nHeroID].byStarLv;
                data.nStaticLv = info.dicEntityHeroInfo[nHeroID].nLv;
            }
            IntPtr ptr = helper.toPtr<cmd_get_hero_detail_info>(ref data);
            int len = Marshal.SizeOf(data);

            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_REQ_UPGRADESTARANDHEROPRO, len, "", ptr, len);

            return true;
        }

        public void SetHeroIntroduce()
        {
            cmd_entity_hero_detail_info info = HeroDetailViewData.Instance.info.heroDetailInfo;
            SSchemeHeroTypeConfig config = HeroTypeConfig.Instance.GetHeroTypeConfig(HeroDetailViewData.Instance.info.heroDetailInfo.nHeroID);

            UseCountText.text = info.nGameCount + ULocalizationService.Instance.Get("UIView", "Common", "Times");
            PlayerWaysText.text = config.strHeroLocation;
            WinRateText.text = String.Format("{0}%", info.usWinPro);
            MilitaryExploitsText.text = info.ulRecordScore.ToString();
            for (int i = 0; i < ScoreLvImage.Length; ++i)
            {
                ScoreLvImage[i].gameObject.SetActive(false);
            }
            int nCount = info.nCount;
            for (int i = 0; i < nCount; ++i)
            {
                if (i >= info.nIconID.Length)
                {
                    Trace.LogError("i >= cmdInfo.nIconID.Length is error");
                    continue;
                }

                int nIconID = info.nIconID[i];
                if (i >= ScoreLvImage.Length)
                {
                    continue; ;
                }

                ScoreLvImage[i].sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ScoreLvImage, WndID.WND_ID_HERODETAIL, 1, nIconID);
                ScoreLvImage[i].gameObject.SetActive(ScoreLvImage[i].sprite);
            }
        }

        public void SetHeroStarData(cmd_entity_hero_diff_expend cmdInfo)
        {
            for (int i = 0; i < UDefines.PROPERTY_COUNT; ++i)
            {
                if (i >= 2)
                {
                    Hp[i].text = (cmdInfo.fHP[i] == 0 ? "" : ("+" + cmdInfo.fHP[i].ToString("F2")));
                    PhysicAttack[i].text = (cmdInfo.fPA[i] == 0 ? "" : ("+" + cmdInfo.fPA[i].ToString("F2")));
                    PhysicDef[i].text = (cmdInfo.fPD[i] == 0 ? "" : ("+" + cmdInfo.fPD[i].ToString("F2")));
                    Mp[i].text = (cmdInfo.fMagic[i] == 0 ? "" : ("+" + cmdInfo.fMagic[i].ToString("F2")));
                    MagicAttack[i].text = (cmdInfo.fMA[i] == 0 ? "" : ("+" + cmdInfo.fMA[i].ToString("F2")));
                    MagicDef[i].text = (cmdInfo.fMD[i] == 0 ? "" : ("+" + cmdInfo.fMD[i].ToString("F2")));
                }
                else
                {
                    string szFormal = i == 0 ? "" : "F2";
                    Hp[i].text = cmdInfo.fHP[i].ToString(szFormal);
                    PhysicAttack[i].text = cmdInfo.fPA[i].ToString(szFormal);
                    PhysicDef[i].text = cmdInfo.fPD[i].ToString(szFormal);
                    Mp[i].text = cmdInfo.fMagic[i].ToString(szFormal);
                    MagicAttack[i].text = cmdInfo.fMA[i].ToString(szFormal);
                    MagicDef[i].text = cmdInfo.fMD[i].ToString(szFormal);
                }
            }
        }

        public void SetHeroStarPrize()
        {
            cmd_entity_hero_detail_info cmdInfo = HeroDetailViewData.Instance.info.heroDetailInfo;

            if (!PrizeUnlockConfig.Instance.heroStarPrizeUnlockDic.ContainsKey(cmdInfo.nHeroID))
                return;
            List<HeroStarPrizeUnlockItem> starPrizeUnlockList = PrizeUnlockConfig.Instance.heroStarPrizeUnlockDic[cmdInfo.nHeroID];
            if (starPrizeUnlockList == null)
                return;
            HeroStarPrizeUnlockItem prizeConfig = null;
            foreach (var item in starPrizeUnlockList)
            {
                if (item.nUnlockLevel > cmdInfo.nStar)
                {
                    prizeConfig = item;
                    break;
                }
            }
            if (prizeConfig == null)
            {
                PrizeFrame.SetActive(false);
                return;
            }
            bool bShowEffect = (cmdInfo.nStar + 1 == prizeConfig.nUnlockLevel);

            PrizeFrame.SetActive(true);
            UpStarPrizeDesc.text = String.Format(ULocalizationService.Instance.Get("UIView", "HeroDetailView", "UpStarPrizeDesc"), prizeConfig.strUnlockStarDesc, prizeConfig.nUnlockLevel);

            int nCount = UpStarPrizeFrame.childCount;
            for (int i = nCount - 1; i >= 0; --i)
            {
                Transform childTrans = UpStarPrizeFrame.GetChild(i);
                if (childTrans)
                {
                    // 销毁obj
                    Destroy(childTrans.gameObject);
                }
            }

            m_upstarPrizeList.Clear();
            // 普通奖励
            for (int j = 0; j < prizeConfig.nPrizeIdList.Length; j++)
            {
                GameObject prizeObj = Instantiate<GameObject>(UpStarPrizeItem);
                if (!prizeObj)
                    return;

                RectTransform prizeRect = prizeObj.transform as RectTransform;
                if (!prizeRect)
                    return;
                prizeRect.SetParent(UpStarPrizeFrame, false);

                LegendCupPrizeItem prizeItem = prizeObj.GetComponent<LegendCupPrizeItem>();
                if (!prizeItem)
                    return;

                prizeItem.SetData(prizeConfig.nPrizeIdList[j], ECupPrizeType.ECPT_Personal, UEffectPrefabType.UEPT_HeroDetailInfo_UpstarPrizeItem);

                m_upstarPrizeList.Add(prizeItem);
            }

            // 天赋奖励
            List<SSchemeHeroTalentConfig> talentConfigList = HeroTalentConfigPage.Instance.getNextPrizeTalent(cmdInfo.nHeroID, cmdInfo.nStar);
            foreach(var item in talentConfigList)
            {
                GameObject prizeObj = Instantiate<GameObject>(UpStarPrizeItem);
                if (!prizeObj)
                    return;

                RectTransform prizeRect = prizeObj.transform as RectTransform;
                if (!prizeRect)
                    return;
                prizeRect.SetParent(UpStarPrizeFrame, false);

                LegendCupPrizeItem prizeItem = prizeObj.GetComponent<LegendCupPrizeItem>();
                if (!prizeItem)
                    return;

                prizeItem.SetData(item, (cmdInfo.nStar+1) == item.nActiveStarLv);
            }


            UpStarPrizeItem.SetActive(false);
        }

        public void ClearPrizeEffect()
        {
            if (m_upstarPrizeList == null || m_upstarPrizeList.Count <= 0)
                return;

            foreach (var item in m_upstarPrizeList)
            {
                item.Clear();
            }
        }
    }

    // 技能
    public class HeroDetailSkillItem : MonoBehaviour
    {
        public Text SkillDescText;
        public Image SkillIconImage;

        public void SetHeroDetailSkillItemInfo(string szDesc, int nIconID)
        {
            if (string.IsNullOrEmpty(szDesc))
            {
                return;
            }

            if (SkillIconImage == null || SkillDescText == null)
            {
                return;
            }

            UBB.toHtml(ref szDesc);
            SkillDescText.text = szDesc;
            SkillIconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_HERODETAIL, 1, nIconID);
        }
    }

    public class HeroDetailSkillFrame : HeroDetailPartFrameBase
    {
        public Text SkillTitleDesc;

        public GameObject TempSkillItem;
        public Transform SkillListTrans;

        private HeroDetailSkillItem[] SkillItemGroup;
        private const int m_nSkillItemCount = 4;

        public override bool Init()
        {
            if (SkillTitleDesc == null || TempSkillItem == null || SkillListTrans == null)
            {
                return false;
            }

            SkillTitleDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "SkillTitleDesc");

            TempSkillItem.SetActive(false);
            if (SkillItemGroup == null)
            {
                SkillItemGroup = new HeroDetailSkillItem[4];
                for (int i = 0; i < m_nSkillItemCount; ++i)
                {
                    GameObject SkillItemObj = Instantiate<GameObject>(TempSkillItem);
                    if (SkillItemObj == null)
                        return false;

                    SkillItemGroup[i] = SkillItemObj.GetComponent<HeroDetailSkillItem>();
                    if (!SkillItemGroup[i])
                        return false;

                    SkillItemObj.SetActive(true);
                    SkillItemObj.transform.SetParent(this.SkillListTrans, false);
                }
            }

            ActorHeroDetailInfo info = HeroDetailViewData.Instance.info;
            for (int i = 0; i < info.cmdCount.nCount; ++i)
            {
                cmd_match_room_hero_skill_desc skillDesc = info.heroSkillDesc[i];

                if (skillDesc.nSpellIconID == 0)
                {
                    continue;
                }

                UBB.toHtml(ref skillDesc.heroSkillDes, UBB_FORMAT_TYPE.UGUI);
                SetHeroDetailSkillItemInfo(i, skillDesc.heroSkillDes, skillDesc.nSpellIconID);
            }

            return true;
        }

        public void SetHeroDetailSkillItemInfo(int nSlotID, string szDesc, int nIconID)
        {
            if (SkillItemGroup == null || nSlotID < 0 || nSlotID >= SkillItemGroup.Length || SkillItemGroup[nSlotID] == null)
            {
                return;
            }

            SkillItemGroup[nSlotID].SetHeroDetailSkillItemInfo(szDesc, nIconID);
        }
    }

    // 天赋
    public class HeroDetailHeroTalentFrame : HeroDetailPartFrameBase
    {
        public Text TalentTitelDesc;

        public HeroTalentViewPart TalentPart;

        public override bool Init()
        {
            TalentTitelDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "TalentTitelDesc");

            return true;
        }

        public override void OnHeroDetailFrameClick(bool bSelect)
        {
            if (!bSelect)
            {
                return;
            }

            InitShowDataInfo();
        }

        public void InitShowDataInfo()
        {
            if (TalentPart == null)
            {
                return;
            }

            int nHeroID = HeroDetailViewData.Instance.nHeroID;
            if (LogicDataCenter.heroTalentDataManager.dicTalentInfoData == null || !LogicDataCenter.heroTalentDataManager.dicTalentInfoData.ContainsKey(nHeroID))
            {
                return;
            }

            LogicDataCenter.heroTalentDataManager.nCurHeroId = nHeroID;
            TalentPart.ClearHeroTalentData();
            HeroTalentConfigPage Instance = HeroTalentConfigPage.Instance;
            List<HeroTalentDataManager.HeroTalentInfo> listInfo = LogicDataCenter.heroTalentDataManager.dicTalentInfoData[nHeroID];
            Dictionary<int, int> dicTalentLevelRecord = new Dictionary<int, int>();
            foreach (HeroTalentDataManager.HeroTalentInfo info in listInfo)
            {
                SSchemeHeroTalentConfig config = Instance.GetHeroTalentConfig(nHeroID, info.nTalentId);
                if (config == null)
                {
                    continue;
                }

                string sTalentDescInStatic = info.bActive ? config.sTalenActiveDescInStatic : config.sTalenDisableDescInStatic;
                int nLevelLimit = config.nLevelLimit;
                dicTalentLevelRecord[nLevelLimit] = 0;

                AddHeroTalentListData(info.nTalentId, info.bActive, config.nShowType, config.nTalentIconId, nLevelLimit, config.sTalentName, sTalentDescInStatic, config.sTalenDescInWar, config.nOnlyActive, 0);
            }

            CalculateWithLevelArray(dicTalentLevelRecord);
            TalentPart.SetHeroTalentListData();
            TalentPart.InitTalentNumButtonInfo(ref LogicDataCenter.heroTalentDataManager.LevelArray);
            if (TalentPart.NumGroup)
                TalentPart.NumGroup.ChangeTogglePageIndex(0);
        }

        private void CalculateWithLevelArray(Dictionary<int, int> dicTalentLevelRecord)
        {
            int nConfigLevelCount = dicTalentLevelRecord.Count;
            LogicDataCenter.heroTalentDataManager.LevelArray = new int[nConfigLevelCount];

            int nIndex = 0;
            foreach (int nLevel in dicTalentLevelRecord.Keys)
            {
                LogicDataCenter.heroTalentDataManager.LevelArray[nIndex] = nLevel;
                nIndex++;
            }

            Array.Sort(LogicDataCenter.heroTalentDataManager.LevelArray);
        }

        private void AddHeroTalentListData(int nTalentId, bool bCanUse, int nShowType, int nIconId, int nTalentLevel, string sTalentName, string sTalentDescInStatic, string sTalenDescInWar, int nOnlyActive, int nTalentTermId)
        {
            if (TalentPart == null)
            {
                return;
            }

            if (!bCanUse && nShowType <= 0)
            {
                return;
            }

            string sTalentDesc = sTalentDescInStatic;
            UListHeroTalentData data = new UListHeroTalentData();
            data.nTalentId = nTalentId;
            //data.bActive = false;
            data.nTalentIconId = nIconId;
            data.nTalentLevel = nTalentLevel;
            data.sTalentName = sTalentName;
            data.sTalentDesc = sTalentDesc;
            data.bOnlyActive = nOnlyActive > 0;
            //data.bCanActive = true;
            data.bLock = !bCanUse;
            data.nTalentTermId = nTalentTermId;
            TalentPart.AddHeroTalentListData(data);
        }
    }

    // 终结技
    public class HeroDetailXPSkillItem : MonoBehaviour
    {
        public Image XpSkillIconImage;
        public GameObject XpIconLockObj;
        public Text XpNameText;
        public Text XpDescText;
        public Text XpFragmentText;
        public Text XpUseText;
        public Image XpUseImage;
        public GameObject XpLightObj;
        public Toggle XpToggle;
        public GameObject XpGuideObj;

        private int m_nInitPositionY = 0;
        private int m_nOneColOffsetY = 0;
        private int m_XPSkillID = 0;
        private UTooltipTrigger m_toolTip;
        private UTooltipTrigger m_toolTip1;
        public int XpSkillID
        {
            get { return m_XPSkillID; }
        }
        public void SetData(SSchemeHeroXPskillConfig config)
        {
            if (config == null)
                return;

            m_XPSkillID = config.nXPSkillID;
            XpNameText.text = config.szXpName;
            XpSkillIconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Skill, WndID.WND_ID_HERODETAIL, 1, config.nIconID);
            XpIconLockObj.SetActive(true);
            XpDescText.text = config.szNoLockDesc;
            int nProsessFragmentNum = LogicDataCenter.heroXPSkillDataManager.GetHeroXPSkillFragmentCount(HeroDetailViewData.Instance.nHeroID, config.nXPSkillID);
            bool bFragmentNumEnough = nProsessFragmentNum >= config.nLockXPFragmentNum;
            if (bFragmentNumEnough)
                XpFragmentText.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "ClickUnlock");
            else
                XpFragmentText.text = String.Format(ULocalizationService.Instance.Get("UIView", "HeroDetailView", "ClickUnlock") + "{0}/{1}", nProsessFragmentNum, config.nLockXPFragmentNum);
            XpFragmentText.gameObject.SetActive(true);
            OnToggleSelect(false);
            XpUseImage.gameObject.SetActive(false);
            XpLightObj.gameObject.SetActive(false);
            XpToggle.gameObject.SetActive(true);
            if (m_toolTip == null)
                m_toolTip = this.XpToggle.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
            if (m_toolTip1 == null)
                m_toolTip1 = XpIconLockObj.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);

            string strXpLockDesc = config.szLockDesc;
            UBB.toHtml(ref strXpLockDesc);
            m_toolTip.SetText(UTooltipParamName.BodyText, strXpLockDesc);
            XpGuideObj.gameObject.SetActive(false);
            m_toolTip1.SetText(UTooltipParamName.BodyText, strXpLockDesc);

            UpdateTextPosition();

        }

        private void UpdateTextPosition()
        {
            return;
            int nRowNum = (int)XpDescText.preferredHeight / 18 - 1;//+ nRemainderNum;
            int nDescPositionY = m_nInitPositionY + nRowNum * m_nOneColOffsetY;
            RectTransform descRectTransform = XpNameText.gameObject.GetComponent<RectTransform>();
            Vector2 anchoredPosition = descRectTransform.anchoredPosition;
            descRectTransform.anchoredPosition = new Vector2(anchoredPosition.x, nDescPositionY);
        }

        public void UpdateData(cmd_Hero_XP_Skill_Info item, int nUseXpID)
        {
            if (m_XPSkillID != item.nXPSkillID)
                return;

            SSchemeHeroXPskillConfig config = HeroXPskillConfig.Instance.GetHeroXPskillConfig(item.nHeroID, item.nXPSkillID);
            if (config == null)
                return;

            XpSkillIconImage.gameObject.SetActive(true);
            XpIconLockObj.SetActive(false);
            XpDescText.text = config.szLockDesc;
            XpFragmentText.gameObject.SetActive(false);
            XpUseImage.gameObject.SetActive(true);
            XpLightObj.gameObject.SetActive(true);
            XpToggle.gameObject.SetActive(true);
            XpGuideObj.SetActive(LogicDataCenter.heroXPSkillDataManager.GetHeroXpGuideVisile(item.nHeroID, item.nXPSkillID));
            if (nUseXpID == m_XPSkillID)
                XpToggle.GetComponent<Toggle>().isOn = true;

            if (m_toolTip && m_toolTip1)
            {
                string strXpLockDesc = config.szNoLockDesc;
                UBB.toHtml(ref strXpLockDesc);
                m_toolTip.SetText(UTooltipParamName.BodyText, strXpLockDesc);
                m_toolTip1.SetText(UTooltipParamName.BodyText, strXpLockDesc);
            } 

            UpdateTextPosition();
        }

        public void ClearItem()
        {
            this.XpToggle.isOn = false;
        }

        public void OnUnlockButtonClick()
        {
            UConsumptionMsgData cmdData = new UConsumptionMsgData((int)EConsumptionRequestType.Request_UnlockXp, (int)EConsumptionGuideType.Guide_Xp);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW, cmdData);
        }

        public void OnToggleSelect(bool bIsOn)
        {
            if (!XpUseImage.gameObject.activeSelf)
                return;

            if (!bIsOn)
                XpUseImage.AddNewMaterial(EUIShaderType.EST_Gray);
            else
                XpUseImage.ResetToDefaultMaterial();

            XpUseText.text = bIsOn ? ULocalizationService.Instance.Get("UIView", "HeroDetailView", "AlreadyUnlock") : ULocalizationService.Instance.Get("UIView", "HeroDetailView", "ClickUnlock");

            cmd_Hero_XP_Skill_Info cmdInfo = new cmd_Hero_XP_Skill_Info();

            if (bIsOn)
            {
                int nHeroID = HeroDetailViewData.Instance.nHeroID;
                int nUseXPSkillID = 0;
                ActorHeroInfo info = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
                if (info.dicEntityHeroInfo.ContainsKey(nHeroID))
                {
                    cmd_entity_hero_info heroInfo = info.dicEntityHeroInfo[nHeroID];
                    nUseXPSkillID = heroInfo.nUseXPSkillID;
                }
                else if (info.dicEntityNoHaveHeroInfo.ContainsKey(nHeroID))
                {
                    nUseXPSkillID = info.dicEntityNoHaveHeroInfo[nHeroID].nUseXPSkillID;
                }

                if (nUseXPSkillID == m_XPSkillID)
                    return;

                cmdInfo.nHeroID = HeroDetailViewData.Instance.nHeroID;
                cmdInfo.nXPSkillID = m_XPSkillID;
                EntityEventHelper.Instance.SendCommand<cmd_Hero_XP_Skill_Info>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_XP_SKILL_OPERATE_SELECT, ref cmdInfo);
            }
        }

        public bool GetXpToggle()
        {
            return this.XpToggle.isOn;
        }
    }

    public class HeroDetailXpFrame : HeroDetailPartFrameBase
    {
        public Text XpTitleDesc;

        public Transform SlotTrans;
        public GameObject TmpSlotItemObj;
        public ToggleGroup XpToogleGroup;
        public Text HeroPlayTip;

        public URichText GetXpRT;

        private HeroDetailXPSkillItem[] SlotItemList;

        public override bool Init()
        {
            XpTitleDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "Xp");
            UpdateSlotList();

            return base.Init();
        }

        public override void OnDestroy()
        {
            base.OnDestroy();

            if (SlotItemList == null)
                return;

            foreach (HeroDetailXPSkillItem item in SlotItemList)
            {
                Destroy(item.gameObject);
            }

            SlotItemList = null;
        }

        public void UpdateSlotList()
        {
            DoInitSlotList();

            GetXpRT.text = "<a href=GetXp color=ffca27>" + ULocalizationService.Instance.Get("UIView", "HeroDetailView", "GetXpWayDesc") + "</a>";
            GetXpRT.onHrefClick.AddListener(OnClickGetXpRT);

            if (SlotItemList == null)
                return;

            foreach (HeroDetailXPSkillItem item in SlotItemList)
            {
                if (item != null)
                    item.ClearItem();
            }

            int nHeroID = HeroDetailViewData.Instance.nHeroID;
            int nUseXPSkillID = 0;
            ActorHeroInfo info = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
            if (info.dicEntityHeroInfo.ContainsKey(nHeroID))
            {
                cmd_entity_hero_info heroInfo = info.dicEntityHeroInfo[nHeroID];
                nUseXPSkillID = heroInfo.nUseXPSkillID;
            }
            else if (info.dicEntityNoHaveHeroInfo.ContainsKey(nHeroID))
            {
                nUseXPSkillID = info.dicEntityNoHaveHeroInfo[nHeroID].nUseXPSkillID;
            }

            List<cmd_Hero_XP_Skill_Info> heroXpSkillList = LogicDataCenter.heroXPSkillDataManager.GetHeroXPSkillList(nHeroID);
            if (heroXpSkillList == null)
                return;

            foreach (cmd_Hero_XP_Skill_Info xpItem in heroXpSkillList)
            {
                foreach (HeroDetailXPSkillItem slotItem in SlotItemList)
                {
                    if (slotItem == null)
                        continue;

                    if (xpItem.nXPSkillID != slotItem.XpSkillID)
                        continue;

                    slotItem.UpdateData(xpItem, nUseXPSkillID);
                }
            }
        }

        public void DoInitSlotList()
        {
            if (SlotItemList != null)
                return;

            TmpSlotItemObj.SetActive(false);

            List<SSchemeHeroXPskillConfig> configList = HeroXPskillConfig.Instance.GetHeroXPskillConfigList(HeroDetailViewData.Instance.nHeroID);
            if (configList == null)
                return;

            int nListCount = configList.Count;
            SlotItemList = new HeroDetailXPSkillItem[nListCount];
            int nSlotID = 0;
            foreach (SSchemeHeroXPskillConfig config in configList)
            {
                GameObject SlotItemObject = Instantiate<GameObject>(TmpSlotItemObj);
                if (SlotItemObject == null)
                {
                    return;
                }

                HeroDetailXPSkillItem item = SlotItemObject.GetComponent<HeroDetailXPSkillItem>();
                if (item == null)
                {
                    return;
                }

                if (nSlotID >= SlotItemList.Length)
                    continue;

                item.SetData(config);
                SlotItemObject.SetActive(true);
                SlotItemList[nSlotID++] = item;
                SlotItemObject.transform.SetParent(SlotTrans, false);
            }
        }

        public void ClearHeroXpGuide()
        {
            if (!LogicDataCenter.heroXPSkillDataManager.GetHeroXpGuideVisile(HeroDetailViewData.Instance.nHeroID))
                return;

            int nHeroID = HeroDetailViewData.Instance.nHeroID;
            int nUseXPSkillID = 0;
            ActorHeroInfo info = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
            if (info.dicEntityHeroInfo.ContainsKey(nHeroID))
            {
                cmd_entity_hero_info heroInfo = info.dicEntityHeroInfo[nHeroID];
                nUseXPSkillID = heroInfo.nUseXPSkillID;
            }
            else if (info.dicEntityNoHaveHeroInfo.ContainsKey(nHeroID))
            {
                nUseXPSkillID = info.dicEntityNoHaveHeroInfo[nHeroID].nUseXPSkillID;
            }

            List<cmd_Hero_XP_Skill_Info> heroXpSkillList = LogicDataCenter.heroXPSkillDataManager.GetHeroXPSkillList(nHeroID);
            if (heroXpSkillList == null)
                return;

            foreach (cmd_Hero_XP_Skill_Info xpItem in heroXpSkillList)
            {
                if (xpItem.nGuide <= 0)
                    continue;

                cmd_Hero_XP_Skill_Info cmdInfo = new cmd_Hero_XP_Skill_Info();
                cmdInfo.nHeroID = nHeroID;
                cmdInfo.nXPSkillID = xpItem.nXPSkillID;
                EntityEventHelper.Instance.SendCommand<cmd_Hero_XP_Skill_Info>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_XP_SKILL_OPERATE_SELECT, ref cmdInfo);
            }
        }

        public void OnClickGetXpRT(string eventName)
        {
            if (eventName == "GetXp")
            {
                UConsumptionMsgData cmdData = new UConsumptionMsgData((int)EConsumptionRequestType.Request_UnlockXp, (int)EConsumptionGuideType.Guide_Xp);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW, cmdData);
            }
        }
    }

    // 背景
    public class HeroDetailBackdropFrame : HeroDetailPartFrameBase
    {
        public Text BackdropTitleDesc;

        public Text BackdropDesc;

        public override bool Init()
        {
            BackdropTitleDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "Backdrop");

            SetHeroDetailBackdropDesc();
            return true;
        }

        public void SetHeroDetailBackdropDesc()
        {
            if (BackdropDesc == null)
            {
                return;
            }

            SSchemeHeroTypeConfig config = HeroTypeConfig.Instance.GetHeroTypeConfig(HeroDetailViewData.Instance.info.heroDetailInfo.nHeroID);
            if (config == null || string.IsNullOrEmpty(config.strHeroDesc))
            {
                return;
            }

            UBB.toHtml(ref config.strHeroDesc);
            BackdropDesc.text = config.strHeroDesc;
        }
    }
    #endregion

    #region// UpgradeEffect
    public class HeroDetailUpGradeEffectFrame : TimeCountDown, IHeroDetailPartFrameBase
    {
        public Text titleDesc;
        public Text starLvDesc;
        public Image curStarIcon;
        private Sequence m_Sequence = null;
        private VoidDetailHandel m_voidDetailHandel = null;
        private Vector3 vTitleOrgPossition;
        private Vector3 vStarLvOrgPossition;
        private SceneEffectParamBase m_SceneEffectParam = null;
        private bool m_bSpecialEffect;
        private UViewHeroDetail m_HeroWndView;
        public UViewHeroDetail HeroWndView
        {
            set { m_HeroWndView = value; }
            get { return m_HeroWndView; }
        }

        private Transform m_SelectHeroTransform;
        public Transform SelectTransForm
        {
            set { m_SelectHeroTransform = value;}
            get { return m_SelectHeroTransform; }
        }

        public bool Init()
        {
            vTitleOrgPossition = titleDesc.transform.localPosition;
            vStarLvOrgPossition = starLvDesc.transform.localPosition;
            titleDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "UpStar") + "：";
            Reset();

            return true;
        }

        public void Reset()
        {
            if (m_Sequence != null)
                m_Sequence.Kill();

            m_voidDetailHandel = null;
            m_Sequence = null;
            //whiteEffect.enabled = false;
            //barBgImage.enabled = false;
            //barImage.fillAmount = (float)0;
            //barImage.enabled = false;
            starLvDesc.text = "";
            curStarIcon.enabled = false;
            m_bSpecialEffect = false;
            titleDesc.gameObject.SetActive(false);
            starLvDesc.gameObject.SetActive(false);
            this.gameObject.SetActive(false);
        }

        public void OnDestroy()
        {
            setPartsUIVisibleInStaticScene(true);

            if (m_Sequence != null)
                m_Sequence.Kill();
        }

        public bool IsVisible()
        {
            return this.gameObject.activeSelf;
        }

        public bool SetParent(Transform parentsTrans)
        {
            return true;
        }

        public void OnHeroDetailFrameClick(bool bSelect)
        {
            return;
        }

        public void DoDetailHandel(BoolDetailHandel boolHandel, VoidDetailHandel voidHandel)
        {
            if (boolHandel != null)
            {
                boolHandel();
            }
            else if (voidHandel != null)
            {
                voidHandel();
            }
        }

        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();

            //barImage.fillAmount = (float)(1 - (this.fCurTime / this.fOriginalTime));
        }

        protected override void CountDownCompleteCallBack()
        {
            base.CountDownCompleteCallBack();

            //UIMsgCmdData uiPlayerProgress = new UIMsgCmdData((int)WndMsgID.WND_MSG_WARBUTTOMCENTER_CLEARPROGRESSBAR, 0, null, IntPtr.Zero, 0);
            //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WARBUTTOMCENTER_CLEARPROGRESSBAR, uiPlayerProgress);
            ShowUpStarWhiteEffect();
        }

        public void ShowUpStarSucess(cmd_Return_HeroDetail_Upgrade_Success cmdData, VoidDetailHandel voidHandel = null)
        {
            if (voidHandel == null)
                return;

            Reset();
            this.gameObject.SetActive(true);
            setPartsUIVisibleInStaticScene(false);

            starLvDesc.text = String.Format(cmdData.chStarDesc + "({0})", cmdData.nCurStarLv);

            m_voidDetailHandel = voidHandel;

            if (m_SceneEffectParam != null)
                SceneEffectManager.Instance.DestroySceneEffect(ref m_SceneEffectParam);

            ESceneEffectID eAttachEffectID = ESceneEffectID.SE_HeroUpStar_Normal;
            m_bSpecialEffect = (cmdData.nSpecialEffect == 1 ? true : false);
            if (m_bSpecialEffect)
            {
                eAttachEffectID = ESceneEffectID.SE_HeroUpStar_Special;
                curStarIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeroStarIcon, "HeroStarFrame" + cmdData.nCurStarIconID, WndID.WND_ID_HERODETAIL);
            }

            if (m_HeroWndView.CommomFrame.SkinModelFrame.LastSkinEffectParam != null && m_HeroWndView.CommomFrame.SkinModelFrame.LastSkinEffectParam.ReturnComponent != null)
            {
                m_SceneEffectParam = new SceneEffectParamBase((int)eAttachEffectID, m_SelectHeroTransform, null, true);
                SceneEffectManager.Instance.CreateSceneEffect(ref m_SceneEffectParam);
                AddUpStarLightingEffect(m_HeroWndView.CommomFrame.SkinModelFrame.LastSkinEffectParam.ReturnComponent.gameObject);

            }

            this.StartCountDown((float)1500 / 1000, 0.5f, 0.1f);
        }

        public void ShowUpStarWhiteEffect()
        {
            titleDesc.transform.localPosition = vTitleOrgPossition;
            starLvDesc.transform.localPosition = vStarLvOrgPossition;
            titleDesc.DOFade(1, 0.1f);
            starLvDesc.DOFade(1, 0.1f);

            titleDesc.gameObject.SetActive(true);
            starLvDesc.gameObject.SetActive(true);

            Vector3 vec = m_HeroWndView.PropertyObj.GetComponent<RectTransform>().anchoredPosition;
            vec.x = 550;
            vec.y = -100;
            m_HeroWndView.PropertyObj.GetComponent<RectTransform>().anchoredPosition = vec;
            m_HeroWndView.PropertyObj.SetActive(true);
            m_Sequence = DOTween.Sequence();

            if (m_bSpecialEffect)
            {
                curStarIcon.DOFade(1, 0.1f);
                curStarIcon.enabled = true;
                m_Sequence.AppendInterval(1.0f)
                        .Append(titleDesc.rectTransform.DOAnchorPosX(-183, 1.0f))
                      .Insert(1.0f, titleDesc.DOFade(0, 1.0f))
                      .Insert(1.0f, starLvDesc.rectTransform.DOAnchorPosX(183, 1.0f))
                      .Insert(1.0f, starLvDesc.DOFade(0, 1.0f))
                      .Insert(1.0f, m_HeroWndView.PropertyObj.GetComponent<CanvasGroup>().DOFade(0, 1.0f))
                      .Insert(1.0f, curStarIcon.DOFade(0, 1.5f))
                      .OnComplete(SequenceComplete);
            }
            else
            {
                m_Sequence.AppendInterval(1.0f)
                        .Append(titleDesc.rectTransform.DOAnchorPosX(-183, 1.0f))
                      .Insert(1.0f, titleDesc.DOFade(0, 1.0f))
                      .Insert(1.0f, starLvDesc.rectTransform.DOAnchorPosX(183, 1.0f))
                      .Insert(1.0f, starLvDesc.DOFade(0, 1.0f))
                      .Insert(1.0f, m_HeroWndView.PropertyObj.GetComponent<CanvasGroup>().DOFade(0, 1.0f))
                      .OnComplete(SequenceComplete);
            }
        }

        private void SequenceComplete()
        {
            if (m_voidDetailHandel != null)
            {
                m_voidDetailHandel();
            }

            setPartsUIVisibleInStaticScene(true);
            Reset();
        }

        private void setPartsUIVisibleInStaticScene(bool bVisible)
        {
            UISystem.Instance.ShowWnd(WndID.WND_ID_VOICESYSTEM, bVisible);
            UISystem.Instance.ShowWnd(WndID.WND_ID_CHATBOX, bVisible);
        }

        private void AddUpStarLightingEffect(GameObject skinObject)
        {
            if (skinObject == null)
                return;

            LightingEffectManager effectManager = EntityFactory.EffectGroup.GetComponent<LightingEffectManager>();
            if (effectManager != null)
            {
                LightEffectParam param = new LightEffectParam();
                param.srcObject = skinObject;
                param.sourcePos = skinObject.transform.position;
                param.targetPos = skinObject.transform.position;
                effectManager.AddLighting(206, param);
            }
        }

    }
    #endregion

    public delegate bool BoolDetailHandel();
    public delegate void VoidDetailHandel();
    public class UViewHeroDetail : UIBaseWndView
    {
        public Text SkinTitleDesc;
        public Text SkinPhotoWtitchDesc;
        public Text UpstarBtnDesc;
        public Text PartsFrameSwitchDesc;
        
        public Text PandectDesc;
        public Text SkillDesc;
        public Text TalentDesc;
        public Text XpDesc;
        public Text BackdropDesc;

        public HeroDetailSkinPhotoFrame SkinPhotoFrame;
        public GameObject SkinPhotoSwitch;
        public HeroDetailCommomFrame CommomFrame;
        public HeroDetailPartsFrame PartsFrame;
        public GameObject PartsFrameSwitch;
        public HeroDetailUpGradeEffectFrame EffectFrame;
        public GameObject PropertyObj;
        public GameObject CloseBtn;

        public override bool Init(IUIWnd wnd)
        {
            SkinTitleDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "SkinTitleDesc");
            SkinPhotoWtitchDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "SkinPhotoWtitchDesc");
            UpstarBtnDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "UpStar");
            PartsFrameSwitchDesc.text = ULocalizationService.Instance.Get("UIView", "HeroDetailView", "PartsFrameSwitchDesc");

            PandectDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "Pandect");
            SkillDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "Skill");
            TalentDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "Talent");
            XpDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "Xp");
            BackdropDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "Backdrop");

            if (EffectFrame)
            {
                EffectFrame.Init();
                EffectFrame.HeroWndView = this;
            }

            return base.Init(wnd);
        }

        public override void Destroy()
        {
            base.Destroy();
            DoDetailHandelInSkinPhotoFrame(null, SkinPhotoFrame.OnDestroy);
            DoDetailHandelInComFrame(null, CommomFrame.OnDestroy);
            DoDetailHandelInPartsFrame(null, PartsFrame.OnDestroy);
            DoDetailHandelInEffectFrame(null, EffectFrame.OnDestroy);
        }

        internal override void SetRendererVisible(bool bVisible)
        {
            if (bVisible && EffectFrame.IsVisible())
                return;

            if (bVisible)
            {
                Init();
            }
            else
            {
                Reset();
            }

            base.SetRendererVisible(bVisible);
            return;
        }

        public bool Init()
        {
            DoDetailHandelInSkinPhotoFrame(SkinPhotoFrame.Init, null);
            DoDetailHandelInComFrame(CommomFrame.Init, null);
            DoDetailHandelInPartsFrame(PartsFrame.Init, null);
            DoDetailHandelInEffectFrame(null, EffectFrame.Reset);

            InitAllView();

            return true;
        }

        public void Reset()
        {
            DoDetailHandelInSkinPhotoFrame(null, SkinPhotoFrame.Reset);
            DoDetailHandelInComFrame(null, CommomFrame.Reset);
            DoDetailHandelInPartsFrame(null, PartsFrame.Reset);
            DoDetailHandelInEffectFrame(null, EffectFrame.Reset);
        }

        public void DoDetailHandelInSkinPhotoFrame(BoolDetailHandel boolHandel = null, VoidDetailHandel voidHandel = null)
        {
            if (SkinPhotoFrame)
                SkinPhotoFrame.DoDetailHandel(boolHandel, voidHandel);
        }

        public void DoDetailHandelInComFrame(BoolDetailHandel boolHandel = null, VoidDetailHandel voidHandel = null)
        {
            if (CommomFrame)
                CommomFrame.DoDetailHandel(boolHandel, voidHandel);
        }

        public void DoDetailHandelInPartsFrame(BoolDetailHandel boolHandel = null, VoidDetailHandel voidHandel = null)
        {
            if (PartsFrame)
                PartsFrame.DoDetailHandel(boolHandel, voidHandel);
        }

        public void DoDetailHandelInEffectFrame(BoolDetailHandel boolHandel = null, VoidDetailHandel voidHandel = null)
        {
            if (EffectFrame)
                EffectFrame.DoDetailHandel(boolHandel, voidHandel);
        }

        public void OnOpenSkinPhotoFrame()
        {
            SkinPhotoFrame.gameObject.SetActive(true);
            SkinPhotoSwitch.SetActive(false);
        }

        public void OnCloseSkinPhotoFrame()
        {
            SkinPhotoFrame.gameObject.SetActive(false);
            SkinPhotoSwitch.SetActive(true);
        }

        public void OnOpenPartsFrame()
        {
            PartsFrame.gameObject.SetActive(true);
            PartsFrameSwitch.SetActive(false);
            Vector3 vec = CloseBtn.GetComponent<RectTransform>().anchoredPosition;
            vec.x = -480;
            CloseBtn.GetComponent<RectTransform>().anchoredPosition = vec;
        }

        public void OnClosePartsFrame()
        {
            PartsFrame.gameObject.SetActive(false);
            PartsFrameSwitch.SetActive(true);
            Vector3 vec = CloseBtn.GetComponent<RectTransform>().anchoredPosition;
            vec.x = -30;
            CloseBtn.GetComponent<RectTransform>().anchoredPosition = vec;
        }

        public void OnCloseBtnClick()
        {
            if (m_wnd != null)
            {
                PartsFrame.PandectFame.ClearPrizeEffect();
                m_wnd.UnloadView();
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PRIZEPOPUP_CLOSE, null);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_HERODETAIL_CLOSE);
            }
        }

        public void SetHeroSkinModel(int nHeroID)
        {
            CommomFrame.SetHeroSkinModel(nHeroID);
        }


        public void ShowUpStarSucess(cmd_Return_HeroDetail_Upgrade_Success cmdData)
        {
            bool bSucess = (cmdData.nSucess == 1 ? true : false);
            if (IsVisible() && bSucess)
            {
                CloseAllView();
                PropertyObj.transform.SetParent(EffectFrame.gameObject.transform, false);
                PropertyObj.SetActive(false);

                EffectFrame.ShowUpStarSucess(cmdData, ShowUpStarSucessOver);
            }
        }

        public void ShowUpStarSucessOver()
        {
            if (this.IsVisible())
            {
                EffectFrame.gameObject.SetActive(false);

                InitAllView();
                Vector3 vec = PropertyObj.GetComponent<RectTransform>().anchoredPosition;
                vec.x = -33;
                vec.y = 125;
                PropertyObj.GetComponent<RectTransform>().anchoredPosition = vec;
                PropertyObj.transform.SetParent(PartsFrame.PandectFame.gameObject.transform, false);
                PropertyObj.GetComponent<CanvasGroup>().alpha = 1.0f;
                PropertyObj.SetActive(true);

                if (PartsFrame)
                    PartsFrame.ShowUpStarSucess();
                if (CommomFrame)
                    CommomFrame.ShowUpStarSucess();
            }
        }

        public void UpdateXPSkillList()
        {
            if (PartsFrame == null || PartsFrame.XpFrame == null)
            {
                return;
            }

            PartsFrame.XpFrame.UpdateSlotList();
        }

        public void OnRecvHeroDiffInfo(cmd_entity_hero_diff_expend cmdData)
        {
            if (PartsFrame)
                PartsFrame.SetHeroStarData(cmdData);
            if (CommomFrame)
                CommomFrame.UpdateUpStarInfo(cmdData);
        }

        public void UpdateUpStarGoldView(int nHaveGold)
        {
            if (CommomFrame)
                CommomFrame.UpdateUpStarGoldView(nHaveGold);
        }

        public void OnBuyOrChangeHeroSuccess()
        {
            CommomFrame.BuyBtnFrame.SetBuyHeroButton();
            SkinPhotoFrame.UpdateSkinPossessState();
            //SkinPhotoFrame.UpdateSkinUseState();

            // 拥有英雄，重新请求一次数据
            int nHeroID = HeroDetailViewData.Instance.nHeroID;
            IntPtrHelper helper = new IntPtrHelper();
            cmd_get_hero_detail_info data = new cmd_get_hero_detail_info();
            data.nHeroID = nHeroID;
            data.nStarLv = 1;
            data.nStaticLv = 1;
            ActorHeroInfo info = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
            if (info.dicEntityHeroInfo.ContainsKey(nHeroID))
            {
                data.nStarLv = info.dicEntityHeroInfo[nHeroID].byStarLv;
                data.nStaticLv = info.dicEntityHeroInfo[nHeroID].nLv;
            }
            IntPtr ptr = helper.toPtr<cmd_get_hero_detail_info>(ref data);
            int len = Marshal.SizeOf(data);

            GameLogicAPI.onEntityEvent(EntityFactory.MainHeroID, (int)EntityLogicDef.ENTITY_CMD_REQ_UPGRADESTARANDHEROPRO, len, "", ptr, len);
        }

        public void OnUpdateHeroTalent()
        {
            PartsFrame.OnUpdateHeroTalent();
        }

        private void InitAllView()
        {
            SkinPhotoFrame.gameObject.SetActive(true);
            SkinPhotoSwitch.SetActive(false);
            PartsFrame.gameObject.SetActive(true);
            PartsFrameSwitch.SetActive(false);
            CommomFrame.gameObject.SetActive(true);
            CloseBtn.SetActive(true);
            Vector3 vec = CloseBtn.GetComponent<RectTransform>().anchoredPosition;
            vec.x = -480;
            CloseBtn.GetComponent<RectTransform>().anchoredPosition = vec;
        }

        private void CloseAllView()
        {
            SkinPhotoFrame.gameObject.SetActive(false);
            SkinPhotoSwitch.gameObject.SetActive(false);
            CommomFrame.gameObject.SetActive(false);
            PartsFrame.gameObject.SetActive(false);
            PartsFrameSwitch.gameObject.SetActive(false);
            CloseBtn.SetActive(false);
        }


        #region 新手引导相关

        public void GuideActionBegin(UGuideWidgetMsgData _msgData)
        {
            if(CommomFrame == null || CommomFrame.UpstarBtnFrame == null)
            {
                Trace.LogError("是不是 CommomFrame.UpstarBtnFrame 改了？这里需要新手引导响应，处理下。");
                return;
            }
            Guide.UGuideWidget widget = CommomFrame.UpstarBtnFrame.gameObject.AddComponent<Guide.UGuideWidget>();
            widget.SetFromMsgData(_msgData);
        }

        public void GuideActionEnd(UGuideWidgetMsgData _msgData)
        {
            if (CommomFrame == null || CommomFrame.UpstarBtnFrame == null)
            {
                Trace.LogError("是不是 CommomFrame.UpstarBtnFrame 改了？这里需要新手引导响应，处理下。");
                return;
            }
            CommomFrame.UpstarBtnFrame.gameObject.RemoveComponent<Guide.UGuideWidget>();

        }
        #endregion
    }
}
