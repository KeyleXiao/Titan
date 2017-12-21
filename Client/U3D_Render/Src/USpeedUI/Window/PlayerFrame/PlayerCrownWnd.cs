/*******************************************************************
** 文件名:	PlayerCrownWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-11-29
** 版  本:	1.0
** 描  述:	皇冠界面
		
********************************************************************/
using ASpeedGame.Data.CrownGenstoneConfig;
using ASpeedGame.Data.ShieldWorld;
using Data.ActorPrizeConfig;
using Data.PointShopConfig;
using DataCenter;
using DG.Tweening;
using GameLogic;
using System;
using System.Collections.Generic;
using U3D_Render.Common;
using UIWidgets;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using USpeedUI.Chat;
using USpeedUI.PlayerFrame;
using USpeedUI.TooltipAndDialog;
using War;

namespace USpeedUI.PlayerCrownWndPart
{
    public enum EGEMSTONEDRAGTYPE
    {
        EGDT_BACKPACKITEM,
        EGDT_INLYAITEM,
        EGDT_APPRAISALITEM,
    }

    public enum ECROWNSUBFRAMETYPE
    {
        ECSFT_MAIN,
        ECSFT_APPRAISAL,
        ECSFT_BACKPACK,
        ECSFT_SORT,
    }

    public class CrownTypeNameInfo
    {
        public double typeValue;
        public int nType;
    };

    public class PlayerCrownWnd : UIFillWnd<PlayerCrownView>//UIBaseSubWnd<PlayerCrownView>
    {
        // 获取ID
        //public override int GetWndType()
        //{
        //    return (int)PlayerSubWndType.WND_CROWN;
        //}

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "PlayerCrownView/PlayerCrownView";
        }

        public override WndMutexLevelDef GetMutexLevel()
        {
            return WndMutexLevelDef.WND_Mutex_Normal;
        }
        // 初始化

        public override bool Init()
        {
            m_isKeyClose = true;
            m_bForbid3DSound = true;

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);

            return base.Init();
        }
        public override void Destroy()
        {
            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);

            base.Destroy();
        }

        protected override void PostSetVisible(bool visible)
        {
            base.PostSetVisible(visible);
            if (visible)
            {
                SceneManager.Instance.LoadCrownGemstoneViewScene(SetRoomShowAfterLoadScene);
                setPartsUIVisibleInStaticScene(false);
				//UISystem.Instance.SetWndLayerVisible_Not(WndLayerDef.WND_LAYER_FILL, WndLayerDef.WND_LAYER_FIXED, WndLayerDef.WND_LAYER_POPUP01);
			}
            else
            {
                SceneManager.Instance.UnLoadCrownGemstoneViewScene();
                if (this.IsVisible())
                {
                    OnMsgBoxSaveInfo();
                }
                if (m_wndView != null)
                    m_wndView.ResetSceneHangingPoint();
            }

            UCommonFullWndVisible msgData = new UCommonFullWndVisible();
            msgData.msgID = (int)WndMsgID.WND_MSG_COMMON_POPUPWND_VISIBLE;
            msgData.nWndID = (int)GetID();
            msgData.isVisible = visible;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_FULLWND_VISIBLE, msgData);
        }


        public void SetRoomShowAfterLoadScene(SceneResInfoManager info)
        {
            if (m_wndView == null)
                return;

            UpdateWndView();
            Transform GemstoneModelTrans = info.SceneRoot.transform;
            Transform[] CrownInlayTransList = new Transform[UDefines.nInlaySlotMax];
            Transform[] ApparailTransList = new Transform[UDefines.nApparailSlotMax];
            Transform rootSubTran1 = GemstoneModelTrans.FindChild("Scenes_AlwaysNormal");
            Transform cameraTrans = GemstoneModelTrans.FindChild("Hero_Camera");
            if (rootSubTran1 == null)
            {
                Debug.LogError("宝石界面找不到节点：" + "Scenes_AlwaysNormal");
                return;
            }

            Transform apparailTransform = rootSubTran1.FindChild("JewelRoomObj02");
            if (apparailTransform == null)
            {
                Debug.LogError("宝石界面找不到节点：" + "JewelRoomObj02");
                return;
            }

            Transform rootSubTran3 = GemstoneModelTrans.FindChild("Scenes_baoshi");
            if (rootSubTran3 == null)
            {
                Debug.LogError("宝石界面找不到节点：" + "Scenes_baoshi");
                return;
            }

            for (int i = 0; i < CrownInlayTransList.Length; ++i)
            {
                CrownInlayTransList[i] = rootSubTran3.FindChild("GameObject_" + (i + 1));               
            }

            for (int i = 0; i < ApparailTransList.Length; ++i)
            {
                if (i == 0)
                {
                    ApparailTransList[i] = apparailTransform.FindChild("gudian1/gudian3");
                }
                else
                {
                    ApparailTransList[i] = apparailTransform.FindChild("gudian1/gudian2");
                }
            }

            m_wndView.SetCrownModelHangingPointTrans(GemstoneModelTrans, CrownInlayTransList, apparailTransform.gameObject, cameraTrans, ApparailTransList);
        }

        private void UpdateWndView()
        {
            if (m_wndView == null || m_wndView.OtherHideItemsFrame == null)
                return;

            if (LogicDataCenter.playerSystemDataManager.bRequestSelf)
                m_wndView.OtherHideItemsFrame.StartShow();
            else
                m_wndView.OtherHideItemsFrame.StartHide();
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
                case WndMsgID.WND_MSG_CROWNPAGE_UPDATEVIEW:
                    {
                        if (m_wndView == null)
                            return;

                        m_wndView.UpdateView();
                    }
                    break;
                case WndMsgID.WND_MSG_CROWNPAGE_OTHERPAGESINGLESHOW:
                    {
                        this.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_CROWNPAGE_OTHERUPDATEVIEW:
                    {
                        if (m_wndView == null)
                            return;

                        m_wndView.UpdateView();
                    }
                    break;
                case WndMsgID.WND_MSG_CROWNPAGE_SELFPAGESINGLESHOW:
                    {
                        if (this.IsVisible())
                            return;

                        this.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_CROWNPAGE_APPRAISALRET:
                    {
                        if (m_wndView == null)
                            return;

                        m_wndView.ShowCrownPageAppraisalRet(msgID, msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_CROWNPAGE_ACTIVEGEMMODEL:
                    {
                        if (m_wndView == null || msgData == null)
                            return;

                        m_wndView.ShowCrownModel(msgData as crownActiveSlotModelData);
                    }
                    break;
                default:
                    break;
            }
        }

		public void setPartsUIVisibleInStaticScene(bool bVisible)
        {
            //UISystem.Instance.ShowWnd(WndID.WND_ID_TEAMMATEINFO, bVisible);
            //UISystem.Instance.ShowWnd(WndID.WND_ID_PLAYERBUTTONFRAME, bVisible);
            //UISystem.Instance.ShowWnd(WndID.WND_ID_SIDEBUTTONLIST, bVisible);
            //UISystem.Instance.ShowWnd(WndID.WND_ID_STATICBOTTOMCENTER, bVisible);

            if (bVisible && LogicDataCenter.playerSystemDataManager.bRequestSelf)
            {
                UISystem.Instance.ShowWnd(WndID.WND_ID_PLAYERFRAME, bVisible);
                UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_PLAYER);
                ButtonType.bIsRequestSelf = LogicDataCenter.playerSystemDataManager.bRequestSelf;
                ButtonType.nRequestPDBID = LogicDataCenter.playerSystemDataManager.nRequestPDBID;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);
            }
            else
            {
                UISystem.Instance.ShowWnd(WndID.WND_ID_PLAYERFRAME, false);
            }
        }

        public void OnMsgBoxSaveInfo()
        {
            if (m_isVisible && LogicDataCenter.crownPageDataManager.IsCurCrownPageChanged())
            {
                UIDialogData data = new UIDialogData(title: ULocalizationService.Instance.Get("UIView", "CrownGem", "SaveGemPageTitle"),
                                            message: ULocalizationService.Instance.Get("UIView", "CrownGem", "SaveGemPageDesc"),
                                            buttons: new UIWidgets.DialogActions() { { ULocalizationService.Instance.Get("UIView", "CrownGem", "ConfirmBtnDesc"), OnMsgBoxSaveHandelSave },
                                               { ULocalizationService.Instance.Get("UIView", "CrownGem", "CancelBtnDesc"), OnMsgBoxSaveHandelCancel}
                                            },
                                            closeButtonAction: new UnityEngine.Events.UnityAction(OnMsgBoxSaveHandelCancelEx)

                                           );
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
            }
            else
            {
                EndView();
            }
        }

        private bool OnMsgBoxSaveHandelSave()
        {
            LogicDataCenter.crownPageDataManager.SaveCurCrownPage();
            EndView();
            return true;
        }


        private bool OnMsgBoxSaveHandelCancel()
        {
            EndView();
            return true;
        }

        private void OnMsgBoxSaveHandelCancelEx()
        {
            EndView();
        }

        private void EndView()
        {
            base.SetVisible(false);
            setPartsUIVisibleInStaticScene(true);
            LogicDataCenter.crownPageDataManager.RescoverCurCrownPage();
            LogicDataCenter.crownPageDataManager.CrownPageCurPageItemData = null;
            LogicDataCenter.crownPageDataManager.BackPackSelGemstoneDBID = 0;
        }

        public override WndID GetID()
        {
            return WndID.WND_ID_CROWNPAGE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FILL;
        }
    }

    #region 子窗口基类
    public abstract class PlayerCrownSubFrameBase : MonoBehaviour
    {
        private PlayerCrownView m_parentFrame;
        public virtual void UpdateFrame()
        {
            return;
        }
        public PlayerCrownView ParentCrownView
        {
            set { m_parentFrame = value; }
            get { return m_parentFrame; }
        }
    }

    public abstract class PlayerCrownSecondSubFrameBase : MonoBehaviour
    {
        public virtual void UpdateSecondSubFrame()
        {
            return;
        }
    }
    #endregion 子窗口基类

    #region 宝石物体脚本
    public abstract class PlayerCrownGemstoneItemBase : MonoBehaviour, IBeginDragHandler, IDragHandler, IEndDragHandler, IDropHandler, IPointerClickHandler, IPointerEnterHandler
    {
        public Image GemStoneImage;
        public PlayerCrownGemStoneDraggingItem DraggingItem;
        public RectTransform DraggingTransform;
        public UTooltipTrigger GemTooltip;

        protected int m_nGemStoneDBID = 0;
        protected bool m_draging;
        public int GemStoneDBID
        {
            get { return m_nGemStoneDBID; }
        }
        

        // IBeginDragHandler
        public virtual void OnBeginDrag(PointerEventData eventData)
        {
            if (GemStoneImage.sprite == null)
                return;

            if (m_draging == false)
            {
                m_draging = true;
                if (DraggingItem != null)
                {
                    var rt = DraggingItem.gameObject.GetComponent<RectTransform>();
                    Vector3 globalMousePos;
                    if (RectTransformUtility.ScreenPointToWorldPointInRectangle(DraggingTransform, eventData.position, eventData.pressEventCamera, out globalMousePos))
                    {
                        rt.SetPosition(globalMousePos);
                        rt.rotation = DraggingTransform.rotation;
                    }
                }
            }
        }

        // IDragHandler
        public virtual void OnDrag(PointerEventData eventData)
        {
            if (m_draging)
            {
                if (DraggingItem != null)
                {
                    var rt = DraggingItem.gameObject.GetComponent<RectTransform>();
                    Vector3 globalMousePos;
                    if (RectTransformUtility.ScreenPointToWorldPointInRectangle(DraggingTransform, eventData.position, eventData.pressEventCamera, out globalMousePos))
                    {
                        rt.SetPosition(globalMousePos);
                        rt.rotation = DraggingTransform.rotation;
                    }
                }
            }
        }

        // IEndDragHandler
        public virtual void OnEndDrag(PointerEventData eventData)
        {
            if (m_draging)
            {
                m_draging = false;
                DraggingItem.GemStoneDBID = 0;
                DraggingItem.gameObject.SetActive(false);
            }
        }

        public virtual void OnDrop(PointerEventData eventData)
        {
        }

        public virtual void OnPointerClick(PointerEventData eventData)
        {
        }

        public void UpdateView(int nGemstoneDBID)
        {
            if (m_nGemStoneDBID == nGemstoneDBID)
                return;

            m_nGemStoneDBID = nGemstoneDBID;
            GemStoneImage.sprite = LogicDataCenter.crownPageDataManager.GetCrownGemStoneSpriteByDBID(nGemstoneDBID);
            GemStoneImage.gameObject.SetActive(GemStoneImage.sprite);

            //if (m_tooltip == null)
            //    m_tooltip = this.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_GemstoneItem);

            GemTooltip.enabled = m_nGemStoneDBID > 0 ? true : false;                
        }

        public virtual void ClearView()
        {
            m_nGemStoneDBID = 0;
            m_draging = false;
            GemStoneImage.gameObject.SetActive(false);

            //if (m_tooltip == null)
             //   m_tooltip = this.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_GemstoneItem);

            GemTooltip.enabled = false;
        }

        public virtual void OnPointerEnter(PointerEventData eventData)
        {
            if (m_nGemStoneDBID <= 0)
                return;

            if (GemTooltip == null || GemTooltip.enabled == false)
                return;

            GemTooltip.backgroundTint = Color.grey;

            cmd_Crown_Gemstone_Item_Info data = LogicDataCenter.crownPageDataManager.GetGemStoneItemData(m_nGemStoneDBID);
            SSchemeCrownGemstoneConfig config = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(data.nGemstoneID);
            if (config == null)
                return;

            int m_nGemstoneGrade = data.byGrade;
            if (m_nGemstoneGrade < 0 || m_nGemstoneGrade >= UDefines.sGemstoneGradeDesc.Length)
                return;

            //string sDesc = "[" + UDefines.sGemstoneGradeDesc[m_nGemstoneGrade] + config.sGemstoneName + "]";
            //m_tooltip.SetText(UTooltipParamName.BodyText, sDesc);

            GemTooltip.SetText("GemstoneName", config.sGemstoneName);

            Sprite gemstoneSprite = LogicDataCenter.crownPageDataManager.GetCrownGemStoneSpriteByIconID(config.nGemstoneIconID);
            GemTooltip.SetImage("GemstoneIcon", gemstoneSprite);

            string sGemstonePropertyHeadText = "";
            string sGemstonePropertyDescText = "";
            if (m_nGemstoneGrade >= 0 && m_nGemstoneGrade < config.szPropertyDescArray.Length)
            {
                string[] sCrownPropertyIDArray = ((string)(config.szPropertyArray[m_nGemstoneGrade])).Split(';');
                int nPropertyID = 0;
                string sPropertyDesc = "";
                int nTotalPower = 0;
                foreach (string sPropertyID in sCrownPropertyIDArray)
                {
                    int.TryParse(sPropertyID, out nPropertyID);
                    SSchemeCrownEffectConfig effectConfig = CrownPageConfigPage.Instance.GetCrownPageEffectConfig(nPropertyID);
                    if (effectConfig == null)
                        continue;

                    sPropertyDesc += effectConfig.sGemstoneDesc + "\n";
                    nTotalPower += effectConfig.nPropertyValue;
                }

                sGemstonePropertyHeadText = ULocalizationService.Instance.Get("UIView", "CrownGem", "TotalPowerDesc1") + nTotalPower;
                sGemstonePropertyDescText = sPropertyDesc;
            }

            GemTooltip.SetText("GemstonePropertyDesc", sGemstonePropertyDescText);
            GemTooltip.SetText("GemstonePropertyHead", sGemstonePropertyHeadText);

            
            if ( m_nGemstoneGrade == (int)EGEMSTONE_GRADE_TYPE.EGGT_OBSOLETEGRADE)
            {
                GemTooltip.TurnSectionOn("EnergySection");
                GemTooltip.SetText("EnergyTip", ULocalizationService.Instance.Get("UIView", "CrownGem", "EnergyTipDesc"));
                //energyProcess.fillAmount = ((float)data.nAppraisalCount / (float)config.nAppraisalProcessMax);
            }
            else
            {
                GemTooltip.TurnSectionOff("EnergySection");
            }
        }

        public bool CanDrag()
        {
            return LogicDataCenter.playerSystemDataManager.bRequestSelf;
        }
    }

    public class PlayerCrownGemstoneBackpackItem : PlayerCrownGemstoneItemBase
    {
        public GameObject MaskObj;
        public Text SeilMoneyText;
        public Image SelectImage;
        private int m_nSlotID;
        public int SlotID { get { return m_nSlotID; } }
        public override void OnBeginDrag(PointerEventData eventData)
        {
            if (GemStoneImage.sprite == null || !GemStoneImage.IsActive())
                return;

            if (eventData.button == PointerEventData.InputButton.Right)
                return;

            if (!m_draging && DraggingItem)
                DraggingItem.setData(m_nGemStoneDBID, EGEMSTONEDRAGTYPE.EGDT_BACKPACKITEM, GemStoneImage, m_nSlotID);

            Data.UISoundConfig.SSchemeUSoundData data = new Data.UISoundConfig.SSchemeUSoundData();
            Data.UISoundConfig.UISoundConfig.Instance.GetData("gem", "Button_gemstorage_drag", ref data);
            SoundManager.CreateUISound(data.USoundID);

            base.OnBeginDrag(eventData);
        }

        public override void OnDrop(PointerEventData eventData)
        {
            GameObject DragObj = eventData.pointerDrag;
            if (DragObj == null)
                return;

            if (eventData.button == PointerEventData.InputButton.Right)
                return;

            PlayerCrownGemStoneDraggingItem dragItem = this.DraggingItem;
            if (dragItem == null)
                return;

            if (dragItem.GemStoneDBID <= 0)
                return;

            if (dragItem.DragType == EGEMSTONEDRAGTYPE.EGDT_BACKPACKITEM)
            {
                // 改为自动排序，拖拽暂时不排序
                //LogicDataCenter.crownPageDataManager.UpdateCrownGemstonePosition(m_nGemStoneDBID, m_nSlotID, dragItem.GemStoneDBID, dragItem.SlotID);
            }
            else if (dragItem.DragType == EGEMSTONEDRAGTYPE.EGDT_INLYAITEM)
            {
                LogicDataCenter.crownPageDataManager.UpdateCrownPageGemStoneInlay(dragItem.SlotID, 0);
            }
            else if (dragItem.DragType == EGEMSTONEDRAGTYPE.EGDT_APPRAISALITEM)
            {
                LogicDataCenter.crownPageDataManager.UpdateCrownPageGemStoneAppraisalSlot(dragItem.SlotID, 0);
            }

            Data.UISoundConfig.SSchemeUSoundData data = new Data.UISoundConfig.SSchemeUSoundData();
            Data.UISoundConfig.UISoundConfig.Instance.GetData("gem", "Button_gemstorage_put", ref data);
            SoundManager.CreateUISound(data.USoundID);

            dragItem.clearData();
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            if (!CanDrag())
                return;

            if (eventData.button == PointerEventData.InputButton.Left)
            {
                if (eventData.clickCount == 1)
                {
                    SendGemstoneInfoToChatItem();
                }
                else if (eventData.clickCount == 2)
                {
                    LogicDataCenter.crownPageDataManager.UpdateCrownPageGemStoneInlayOrAppraisalSlot(GemStoneDBID);
                }                    
            }
            else if (eventData.button == PointerEventData.InputButton.Right)
            {
                LogicDataCenter.crownPageDataManager.UpdateCrownPageGemStoneInlayOrAppraisalSlot(GemStoneDBID);
            }

            LogicDataCenter.crownPageDataManager.BackPackSelGemstoneDBID = m_nGemStoneDBID;
        }

        public void UpdateView(int nGemstoneDBID, int nSlotID)
        {
            base.UpdateView(nGemstoneDBID);
            m_nSlotID = nSlotID;
            SetMask();
        }

        private void SetMask()
        {
            bool bMaskVisible = false;
            if (LogicDataCenter.crownPageDataManager.AppraisalVisible)
            {
                cmd_Crown_Gemstone_Item_Info data = LogicDataCenter.crownPageDataManager.GetGemStoneItemData(m_nGemStoneDBID);
                if (data.nGemstoneID <= 0)
                    return;

                if (data.byGrade != (byte)EGEMSTONE_GRADE_TYPE.EGGT_OBSOLETEGRADE)
                {
                    bMaskVisible = true;
                }
             }

            MaskObj.SetActive(bMaskVisible);
        }

        public void SetDefualtSlot(int nSlotID)
        {
            m_nSlotID = nSlotID;
        }

        public void SendGemstoneInfoToChatItem()
        {
            if (!InputManager.GetKey(KeyCode.LeftControl) && !InputManager.GetKey(KeyCode.RightControl))
            {
                return;
            }

            cmd_Crown_Gemstone_Item_Info data = LogicDataCenter.crownPageDataManager.GetGemStoneItemData(m_nGemStoneDBID);
            if (data.nGemstoneID <= 0)
                return;

            SSchemeCrownGemstoneConfig config = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(data.nGemstoneID);
            if (config == null)
                return;


            Vector3[] colorList = new Vector3[7] { new Vector3(255, 0, 0), new Vector3(255, 128, 0), new Vector3(255, 255, 0), new Vector3(0, 255, 0), new Vector3(0, 255, 255), new Vector3(0, 0, 255), new Vector3(128, 0, 255) };

            SChatMessageObjectInfo channelObj;
            channelObj.type = ChatItemType.ObjType_Text;
            channelObj.subType = ChatItemSubType.ObjSubType_Text_Gemstone;
            channelObj.text = "[" + UDefines.sGemstoneGradeDesc[data.byGrade] + config.sGemstoneName + "]";
            channelObj.param = new Dictionary<String, String>();
            if (data.byGrade >= 0 && data.byGrade < colorList.Length)
                channelObj.param.Add("color", LogicDataCenter.chatMessageManager.CreateColorParam(235, 80, 30));
            channelObj.param.Add("gemstoneID", "" + data.nGemstoneID);
            channelObj.param.Add("gemstoneGrade", "" + data.byGrade);
            channelObj.param.Add("gemstoneExp", "" + data.nExp);
            ChatBoxAddInputFieldItemData uiData = new ChatBoxAddInputFieldItemData();
            uiData.msgID = (int)WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM;
            uiData.objInfo = channelObj;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CHATBOX_INPUTFIELDADDITEM, uiData);
        }

        public override void ClearView()
        {
            base.ClearView();
            SelectImage.gameObject.SetActive(false);
            MaskObj.SetActive(false);
        }

        public void DoSeilFloatWord()
        {
            cmd_Crown_Gemstone_Item_Info data = LogicDataCenter.crownPageDataManager.GetGemStoneItemData(LogicDataCenter.crownPageDataManager.BackPackSelGemstoneDBID);
            if (data.nGemstoneID <= 0)
                return;

            SSchemeCrownGemstoneConfig config = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(data.nGemstoneID);
            if (config == null)
                return;

            string[] sArray = config.szSeilMoneyArray.Split(';');
            if (data.byGrade >= 0 && data.byGrade < sArray.Length)
            {
                SeilMoneyText.text = "+" + sArray[data.byGrade];
                SeilMoneyText.DOFade(1, 1).OnComplete(() => SeilMoneyText.DOFade(0, 1)); ;
            }
        }
    }

    public class PlayerCrownGemstoneInlayItem : PlayerCrownGemstoneItemBase
    {
        public Transform PropertyTextTrans;
        public Transform Bordertrs;
        public GameObject TmpTextItemObj;
        public Text SlotText;

        private Text[] PropertyTextList;
        private int m_nSlotCountMax = 5;

        private int m_nSlotID;
        public int SlotID { get { return m_nSlotID; } }

        public void Start()
        {
            Bordertrs.DOLocalRotate(new Vector3(0, 0, 180), 2f).SetLoops(-1, LoopType.Incremental).SetEase(Ease.Linear);
        }

        public override void OnBeginDrag(PointerEventData eventData)
        {
            if (!CanDrag())
                return;

            if (eventData.button == PointerEventData.InputButton.Right)
                return;

            if (GemStoneImage.sprite == null)
                return;

            if (!m_draging && DraggingItem)
                DraggingItem.setData(m_nGemStoneDBID, EGEMSTONEDRAGTYPE.EGDT_INLYAITEM, GemStoneImage, m_nSlotID);

            crownActiveSlotModelData data = new crownActiveSlotModelData();
            data.type = EGEMSTONEDRAGTYPE.EGDT_INLYAITEM;
            data.slotID = m_nSlotID;
            data.bActive = false;

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CROWNPAGE_ACTIVEGEMMODEL, data);

            Data.UISoundConfig.SSchemeUSoundData soundData = new Data.UISoundConfig.SSchemeUSoundData();
            Data.UISoundConfig.UISoundConfig.Instance.GetData("gem", "Button_gemstorage_drag", ref soundData);
            SoundManager.CreateUISound(soundData.USoundID);

            base.OnBeginDrag(eventData);
        }

        public override void OnEndDrag(PointerEventData eventData)
        {
            crownActiveSlotModelData data = new crownActiveSlotModelData();
            data.type = EGEMSTONEDRAGTYPE.EGDT_INLYAITEM;
            data.slotID = m_nSlotID;
            data.bActive = true;

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CROWNPAGE_ACTIVEGEMMODEL, data);

            base.OnEndDrag(eventData);
        }

        public override void OnDrop(PointerEventData eventData)
        {
            if (!CanDrag())
                return;

            if (eventData.button == PointerEventData.InputButton.Right)
                return;

            GameObject DragObj = eventData.pointerDrag;
            if (DragObj == null)
                return;

            PlayerCrownGemStoneDraggingItem dragItem = this.DraggingItem;
            if (dragItem == null)
                return;

            if (dragItem.GemStoneDBID <= 0)
                return;

            if (dragItem.DragType == EGEMSTONEDRAGTYPE.EGDT_BACKPACKITEM)
                LogicDataCenter.crownPageDataManager.UpdateCrownPageGemStoneInlay(m_nSlotID, dragItem.GemStoneDBID);
            else if (dragItem.DragType == EGEMSTONEDRAGTYPE.EGDT_INLYAITEM)
                LogicDataCenter.crownPageDataManager.UpdateCrownPageGemStoneInlay(m_nGemStoneDBID, m_nSlotID, dragItem.GemStoneDBID, dragItem.SlotID);

            dragItem.clearData();
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            if (!CanDrag())
                return;

            if (eventData.button == PointerEventData.InputButton.Right)
            {
                LogicDataCenter.crownPageDataManager.UpdateCrownPageGemStoneInlay(m_nSlotID, 0);
            }
            else if (eventData.button == PointerEventData.InputButton.Left)
            {
                if(eventData.clickCount == 2)
                {
                    LogicDataCenter.crownPageDataManager.UpdateCrownPageGemStoneInlay(m_nSlotID, 0);
                    return;
                }
                if (m_nGemStoneDBID <= 0)
                {
                    int nPageGemCount = LogicDataCenter.crownPageDataManager.GetCurPageGemstoneCount();
                    if (LogicDataCenter.crownPageDataManager.CrownGemtoneDic.Count > nPageGemCount)
                    {
                        UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CROWNS_GEMSTONEINLAY_PUT);
                    }
                    else
                    {
                        UConsumptionMsgData cmdData = new UConsumptionMsgData((int)EConsumptionRequestType.Request_BuyGoods, (int)EConsumptionGuideType.Guide_Gemstone);
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW, cmdData);
                    }
                }
            }
        }

        public void SetSlotText(bool bActive)
        {
            SlotText.text = ULocalizationService.Instance.Get("UIView", "CrownGem", "InlaySlotDesc");
            SlotText.gameObject.SetActive(bActive);
        }

        public void UpdateView(int nGemstoneDBID, int nSlotID)
        {
            base.UpdateView(nGemstoneDBID);
            m_nSlotID = nSlotID;
            GemStoneImage.enabled = false;
            SetSlotText(nGemstoneDBID <= 0);
            UpdatePropView(nGemstoneDBID);
        }

        public void UpdatePropView(int nGemstoneDBID)
        {
            if (PropertyTextList == null)
            {
                TmpTextItemObj.SetActive(false);

                PropertyTextList = new Text[m_nSlotCountMax];
                for (int i = 0; i < m_nSlotCountMax; ++i)
                {
                    GameObject TextItemObject = Instantiate<GameObject>(TmpTextItemObj);
                    if (TextItemObject == null)
                    {
                        return;
                    }

                    Text item = TextItemObject.GetComponent<Text>();
                    if (item == null)
                    {
                        return;
                    }

                    TextItemObject.SetActive(false);
                    TextItemObject.transform.SetParent(PropertyTextTrans, false);
                    PropertyTextList[i] = item;
                }
            }

            foreach (Text item in PropertyTextList)
            {
                if (item == null)
                    continue;

                item.text = "";
                item.gameObject.SetActive(false);
            }

            if (nGemstoneDBID <= 0)
            {
                Bordertrs.DOPause();
                return;
            }
            else
            {
                Bordertrs.DOPlay();
            }

            cmd_Crown_Gemstone_Item_Info data = LogicDataCenter.crownPageDataManager.GetGemStoneItemData(nGemstoneDBID);
            SSchemeCrownGemstoneConfig gemstoneConfig = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(data.nGemstoneID);
            if (gemstoneConfig == null)
                return;

            if (data.byGrade < 0 || data.byGrade >= (int)EGEMSTONE_GRADE_TYPE.EGGT_MAX)
                return;

            int nTextIndex = 0;
            string[] sCrownPropertyIDArray = ((string)(gemstoneConfig.szPropertyArray[data.byGrade])).Split(';');
            int nPropertyID = 0;
            foreach (string sPropertyID in sCrownPropertyIDArray)
            {
                int.TryParse(sPropertyID, out nPropertyID);
                SSchemeCrownEffectConfig effectConfig = CrownPageConfigPage.Instance.GetCrownPageEffectConfig(nPropertyID);
                if (effectConfig == null)
                    continue;

                if (nTextIndex < PropertyTextList.Length)
                {
                    String szPropDesc = effectConfig.sGemstoneDesc;
                    UBB.toHtml(ref szPropDesc, UBB_FORMAT_TYPE.UGUI);
                    PropertyTextList[nTextIndex].text = szPropDesc; 
                    PropertyTextList[nTextIndex].gameObject.SetActive(effectConfig.sGemstoneDesc.Length > 0);
                    ++nTextIndex;
                }
            }          
        }
    }

    public class PlayerCrownGemstoneAppraisalItem : PlayerCrownGemstoneItemBase
    {
        public Text SlotText;

        private int m_nSlotID;
        public int SlotID { get { return m_nSlotID; } }
        public override void OnBeginDrag(PointerEventData eventData)
        {
            if (GemStoneImage.sprite == null)
                return;

            if (eventData.button == PointerEventData.InputButton.Right)
                return;

            if (!m_draging && DraggingItem)
                DraggingItem.setData(m_nGemStoneDBID, EGEMSTONEDRAGTYPE.EGDT_APPRAISALITEM, GemStoneImage, m_nSlotID);

            Data.UISoundConfig.SSchemeUSoundData data = new Data.UISoundConfig.SSchemeUSoundData();
            Data.UISoundConfig.UISoundConfig.Instance.GetData("gem", "Button_gemstorage_drag", ref data);
            SoundManager.CreateUISound(data.USoundID);

            base.OnBeginDrag(eventData);
        }

        public override void OnDrop(PointerEventData eventData)
        {
            GameObject DragObj = eventData.pointerDrag;
            if (DragObj == null)
                return;

            if (eventData.button == PointerEventData.InputButton.Right)
                return;

            PlayerCrownGemStoneDraggingItem dragItem = this.DraggingItem;
            if (dragItem == null)
                return;

            if (dragItem.GemStoneDBID <= 0)
                return;

            if (dragItem.DragType == EGEMSTONEDRAGTYPE.EGDT_BACKPACKITEM)
                LogicDataCenter.crownPageDataManager.UpdateCrownPageGemStoneAppraisalSlot(m_nSlotID, dragItem.GemStoneDBID);
            else if (dragItem.DragType == EGEMSTONEDRAGTYPE.EGDT_APPRAISALITEM)
                LogicDataCenter.crownPageDataManager.UpdateCrownPageGemStoneAppraisalSlot(m_nGemStoneDBID, m_nSlotID, dragItem.GemStoneDBID, dragItem.SlotID);

            dragItem.clearData();
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            if (eventData.button == PointerEventData.InputButton.Right)
            {
                LogicDataCenter.crownPageDataManager.UpdateCrownPageGemStoneAppraisalSlot(m_nSlotID, 0);
            }
        }

        public void SetSlotText(bool bActive)
        {
            if (m_nSlotID == 0 )
            {
                String MainSlotDesc = ULocalizationService.Instance.Get("UIView", "CrownGem", "AppraisalMainSlotDesc"); ;
                UBB.toHtml(ref MainSlotDesc, UBB_FORMAT_TYPE.UGUI);
                SlotText.text = MainSlotDesc;
            }
            else
            {
                String AssSlotDesc = ULocalizationService.Instance.Get("UIView", "CrownGem", "AppraisalAssSlotDesc");
                UBB.toHtml(ref AssSlotDesc, UBB_FORMAT_TYPE.UGUI);
                SlotText.text = AssSlotDesc;
            }
            
            SlotText.gameObject.SetActive(bActive);
        }

        public void UpdateView(int nGemstoneDBID, int nSlotID)
        {
            base.UpdateView(nGemstoneDBID);
            m_nSlotID = nSlotID;
            GemStoneImage.enabled = false;
            SetSlotText(nGemstoneDBID <= 0);
        }
    }

    public class PlayerCrownGemStoneDraggingItem : MonoBehaviour
    {
        public Image GemStoneImage;
        private int m_nGemstoneDBID;
        private EGEMSTONEDRAGTYPE m_eDragItemType;
        private int m_nSlotID;
        public int GemStoneDBID
        {
            set { m_nGemstoneDBID = value; }
            get { return m_nGemstoneDBID; }
        }

        public EGEMSTONEDRAGTYPE DragType
        {
            set { m_eDragItemType = value; }
            get { return m_eDragItemType; }
        }

        public int SlotID
        {
            set { m_nSlotID = value; }
            get { return m_nSlotID; }
        }

        public void setData(int nGemstoneDBID, EGEMSTONEDRAGTYPE eGEMSTONEDRAGTYPE, Image GemStoneImage, int nSlotID)
        {
            this.gameObject.SetActive(true);
            this.m_nGemstoneDBID = nGemstoneDBID;
            m_eDragItemType = eGEMSTONEDRAGTYPE;
            this.GemStoneImage.sprite = GemStoneImage.sprite;
            m_nSlotID = nSlotID;
        }

        public void clearData()
        {
            this.gameObject.SetActive(false);
            this.m_nGemstoneDBID = 0;
            m_eDragItemType = EGEMSTONEDRAGTYPE.EGDT_BACKPACKITEM;
            this.GemStoneImage.sprite = GemStoneImage.sprite;
            m_nSlotID = 0;
        }

        public void OnApplicationFocus(bool focus)
        {
            if (!focus)
                clearData();
        }
    }
    #endregion 宝石物体脚本

    #region 背包子界面
    public class PlayerCrownBackPackFrame : PlayerCrownSubFrameBase
    {
        public PlayerCrownGemstoneBackpackItem[] BackPackItemList;

        private const int m_nBackPackItemMax = 30;
        private const int m_nFilterListCountMax = 5;

        public override void UpdateFrame()
        {
            if (ParentCrownView == null)
                return;

            if (!gameObject.activeSelf)
                return;

            if (BackPackItemList == null)
                return;

            int nDefaultSlot = 0;
            foreach (PlayerCrownGemstoneBackpackItem packItem in BackPackItemList)
            {
                if (packItem)
                {
                    packItem.ClearView();
                    packItem.SetDefualtSlot(nDefaultSlot++);
                }
            }

            int[] filterList = new int[m_nFilterListCountMax];
            if (ParentCrownView.AppraisalFrame.gameObject.activeSelf)
            {
                if (LogicDataCenter.crownPageDataManager.CrownPageAppraisalList != null)
                    Array.Copy(LogicDataCenter.crownPageDataManager.CrownPageAppraisalList, filterList, LogicDataCenter.crownPageDataManager.CrownPageAppraisalList.Length);
            }

            else if (ParentCrownView.MainFrame.gameObject.activeSelf)
            {
                if (LogicDataCenter.crownPageDataManager.CrownPageInlayList != null)
                    Array.Copy(LogicDataCenter.crownPageDataManager.CrownPageInlayList, filterList, LogicDataCenter.crownPageDataManager.CrownPageInlayList.Length);
            }

            Dictionary<int, cmd_Crown_Gemstone_Item_Info> gemStoneInfoDic = LogicDataCenter.crownPageDataManager.CrownPageGemStoneInfo;
            foreach (cmd_Crown_Gemstone_Item_Info item in gemStoneInfoDic.Values)
            {
                int nSlotID = item.nSlotID;
                if (nSlotID < 0 || nSlotID >= m_nBackPackItemMax)
                    continue;

                Image stoneImage = BackPackItemList[nSlotID].GemStoneImage;
                if (FilterBackPackItem(filterList, item.nGemstoneDBID))
                {
                    // 放入圆环的宝石，所在槽位由空改为宝石还在，但是变灰掉
                    stoneImage.sprite = LogicDataCenter.crownPageDataManager.GetCrownGemStoneSpriteByDBID(item.nGemstoneDBID);
                    stoneImage.gameObject.SetActive(true);
                    stoneImage.color = new Color(1.0f, 1.0f, 1.0f, 0x1f / 255.0f);
                }
                else
                {
                    stoneImage.color = new Color(1.0f, 1.0f, 1.0f, 1.0f);
                    BackPackItemList[nSlotID].UpdateView(item.nGemstoneDBID, nSlotID);
                }
            }
        }

        public bool FilterBackPackItem(int[] filterList, int nGemstoneDBID)
        {
            if (filterList == null)
                return false;

            foreach (int nGemStoneDBIDItem in filterList)
            {
                if (nGemStoneDBIDItem == nGemstoneDBID)
                    return true;
            }

            return false;
        }

        public void OnSeilButtonClick()
        {
            cmd_Crown_Gemstone_Item_Info Itemdata = LogicDataCenter.crownPageDataManager.GetGemStoneItemData(LogicDataCenter.crownPageDataManager.BackPackSelGemstoneDBID);
            if (Itemdata.nGemstoneID <= 0)
                return;

            SSchemeCrownGemstoneConfig config = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(Itemdata.nGemstoneID);
            if (config == null)
                return;

            if (Itemdata.byGrade == 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CROWNS_GEMSTONEINLAY_NOT_SELL_OBSOLETE);
                return;
            }
            string[] sArray = config.szSeilMoneyArray.Split(';');
            if (Itemdata.byGrade >= 0 && Itemdata.byGrade < sArray.Length)
            {
                TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                    title: ULocalizationService.Instance.Get("UIView", "CrownGem", "SellGemstoneTitle"),
                                    message: String.Format(ULocalizationService.Instance.Get("UIView", "CrownGem", "SellGemstoneMsg"), config.sGemstoneName, sArray[Itemdata.byGrade]),
                                    buttons: new UIWidgets.DialogActions() { { ULocalizationService.Instance.Get("UIView", "CrownGem", "ConfirmBtnDesc"), OnSeilGemStone }, { ULocalizationService.Instance.Get("UIView", "CrownGem", "CancelBtnDesc"), null } }
                                    );
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
            }            
        }

        private bool OnSeilGemStone()
        {
            LogicDataCenter.crownPageDataManager.SendSeilCrownPageMessage();
            DoSeilFloatWordOnBackpackItem();

            return true;
        }

        private void DoSeilFloatWordOnBackpackItem()
        {
            if (LogicDataCenter.crownPageDataManager.BackPackSelGemstoneDBID <= 0)
                return;

            foreach(PlayerCrownGemstoneBackpackItem item in BackPackItemList)
            {
                if (item.GemStoneDBID != LogicDataCenter.crownPageDataManager.BackPackSelGemstoneDBID)
                    continue;

                item.DoSeilFloatWord();
                break;
            }
        }
    }
    #endregion

    #region 鉴定子界面
    public class PlayerCrownAppraisalSlotFrame : TimeCountDown
    {
        public Button AppraisalButton;
        public Text AppraisalBtnText;
        public Text TicketCostText;
        public Text MoneyCostText;
        public PlayerCrownGemstoneAppraisalItem[] SlotItemList;
        public Transform CostTransform;

        private int m_nSlotCountMax = 2;
        private Transform[] GemstoneModelTrans;
        private GameObject[] m_LastGemstoneModelList;
        private GameObject m_CrownModelObj;
        private Animator m_Animator;
        private Transform m_MidEffectTrans;


        private SceneEffectParamBase[] m_SceneEffectParamArray;
        private SceneEffectParamBase m_SceneEffectParam;

        private int m_MainGmestoneID = 0;
        private int m_AssistGmestoneID = 0;
        private int m_CostTicketNum = 0;
        private int m_CostMoneyNum = 0;
        private bool m_AppraisalFlag = false;
        public bool m_bIsClose = false;

        public void Awake()
        {
            m_LastGemstoneModelList = new GameObject[m_nSlotCountMax];
            m_SceneEffectParamArray = new SceneEffectParamBase[m_nSlotCountMax];
            AppraisalBtnText.text = ULocalizationService.Instance.Get("UIView", "CrownGem", "AppraisalBtnDesc");
        }

        public void OnEnable()
        {
            m_bIsClose = false;
            m_AppraisalFlag = false;
            StartModelSceneEffect();
        }

        public void OnDisable()
        {
            m_bIsClose = true;

            if (m_LastGemstoneModelList != null)
            {
                for (int i = 0; i < m_LastGemstoneModelList.Length; ++i)
                {
                    if (m_LastGemstoneModelList[i] != null)
                    {
                        ResNode.DestroyRes(ref m_LastGemstoneModelList[i]);
                        m_LastGemstoneModelList[i] = null;
                    }
                }
            }

            CostTransform.gameObject.SetActive(false);

            m_AppraisalFlag = false;
            m_MainGmestoneID = 0;
            m_AssistGmestoneID = 0;
            m_CostTicketNum = 0;
            m_CostMoneyNum = 0;

            CloseModelSceneEffect();
        }

        public void Destroy()
        {
            if (m_LastGemstoneModelList != null)
            {
                for (int i = 0; i < m_LastGemstoneModelList.Length; ++i)
                {
                    if (m_LastGemstoneModelList[i] != null)
                    {
                        ResNode.DestroyRes(ref m_LastGemstoneModelList[i]);
                        m_LastGemstoneModelList[i] = null;
                    }
                }
            }

            CostTransform.gameObject.SetActive(false);

            m_AppraisalFlag = false;
            m_MainGmestoneID = 0;
            m_AssistGmestoneID = 0;
            m_CostTicketNum = 0;
            m_CostMoneyNum = 0;

            CloseModelSceneEffect();
        }

        public void SetAppraisalGemstoneModelTrans(Transform[] AppraisalTransList, GameObject CrownModelObj)
        {
            this.GemstoneModelTrans = AppraisalTransList;
            m_CrownModelObj = CrownModelObj;

            if (m_CrownModelObj != null)
            {
                m_Animator = m_CrownModelObj.GetComponent<Animator>();
                m_MidEffectTrans = m_CrownModelObj.transform.FindChild("gudian1");
            }
        }

        public void CloseModelSceneEffect()
        {
            for (int i = 0; i < m_SceneEffectParamArray.Length; ++i)
            {
                if (m_SceneEffectParamArray[i] != null)
                    SceneEffectManager.Instance.DestroySceneEffect(ref m_SceneEffectParamArray[i]);
            }

            if (m_SceneEffectParam != null)
                SceneEffectManager.Instance.DestroySceneEffect(ref m_SceneEffectParam);
        }

        public void StartModelSceneEffect()
        {
            if (m_SceneEffectParamArray == null)
                return;

            for (int i = 0; i < m_SceneEffectParamArray.Length; ++i)
            {
                if (m_SceneEffectParamArray[i] != null)
                    SceneEffectManager.Instance.DestroySceneEffect(ref m_SceneEffectParamArray[i]);

                if (i == 0)
                {
                    if (m_MidEffectTrans != null)
                        m_SceneEffectParamArray[i] = new SceneEffectParamBase((int)ESceneEffectID.SE_CrownGemAppraisal_Main, m_MidEffectTrans, null, true);
                }
                else
                {
                    m_SceneEffectParamArray[i] = new SceneEffectParamBase((int)ESceneEffectID.SE_CrownGemAppraisal_Ass, GemstoneModelTrans[i], null, true);
                }

                SceneEffectManager.Instance.CreateSceneEffect(ref m_SceneEffectParamArray[i]);
            }
        }

        public void StartAppraisalModelSceneEffect(bool bAppraisalRet)
        {
            if (!bAppraisalRet)
            {
                m_AppraisalFlag = false;
                UpdateSecondSubFrame();
                return;
            }

            if (m_SceneEffectParam != null)
                SceneEffectManager.Instance.DestroySceneEffect(ref m_SceneEffectParam);

            if (m_MidEffectTrans != null)
            {
                m_SceneEffectParam = new SceneEffectParamBase((int)ESceneEffectID.SE_CrownGemAppraisal_Special, m_MidEffectTrans, null, true);
                SceneEffectManager.Instance.CreateSceneEffect(ref m_SceneEffectParam);
            }            

            if (m_Animator != null)
            {
                m_Animator.cullingMode = AnimatorCullingMode.AlwaysAnimate;
                m_Animator.Play("Stand");
            }

            this.StartCountDown((float)2300 / 1000, 0.5f, 0.1f);
        }

        protected override void CountDownCompleteCallBack()
        {
            base.CountDownCompleteCallBack();
            StopAppraisalModelSceneEffect();
        }

        public void StopAppraisalModelSceneEffect()
        {
            m_AppraisalFlag = false;

            if (m_SceneEffectParam != null)
                SceneEffectManager.Instance.DestroySceneEffect(ref m_SceneEffectParam);

            if (m_Animator != null)
                m_Animator.Play("Rest");

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CROWNPAGE_UPDATEVIEW, null);
        }

        public void UpdateSecondSubFrame()
        {
            if (SlotItemList == null || m_AppraisalFlag == true)
                return;

            foreach (PlayerCrownGemstoneAppraisalItem item in SlotItemList)
            {
                if (item != null)
                    item.ClearView();
            }

            CostTransform.gameObject.SetActive(false);

            int nSlotID = 0;
            int nHaveGemstoneCount = 0;
            foreach (int nGemsoneDBID in LogicDataCenter.crownPageDataManager.CrownPageAppraisalList)
            {
                if (nSlotID < 0 || nSlotID >= LogicDataCenter.crownPageDataManager.CrownPageAppraisalList.Length)
                    continue;

                if (m_LastGemstoneModelList != null && m_LastGemstoneModelList[nSlotID] != null && nGemsoneDBID <= 0)
                {
                    ResNode.DestroyRes(ref m_LastGemstoneModelList[nSlotID]);
                    m_LastGemstoneModelList[nSlotID] = null;
                }

                if (nSlotID == 0)
                {
                    if (nGemsoneDBID > 0 && m_MainGmestoneID != nGemsoneDBID)
                        UpdateMainAppraisalGenstoneModel(nGemsoneDBID, nSlotID);

                    m_MainGmestoneID = nGemsoneDBID;
                }
                else
                {
                    if (nGemsoneDBID > 0 && m_AssistGmestoneID != nGemsoneDBID)
                        UpdateMainAppraisalGenstoneModel(nGemsoneDBID, nSlotID);

                    m_AssistGmestoneID = nGemsoneDBID;
                }
                    

                SlotItemList[nSlotID].UpdateView(nGemsoneDBID, nSlotID);
                nSlotID++;

                if (nGemsoneDBID > 0)
                    nHaveGemstoneCount++;
            }

            bool btnInteractable = nHaveGemstoneCount >= 2;
            if (m_MainGmestoneID > 0 && m_AssistGmestoneID > 0 && CalcAppraisalCost())
            {
                CostTransform.gameObject.SetActive(true);

                if (m_CostMoneyNum > EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_GOLD))
                {
                    btnInteractable = false;
                    MoneyCostText.text = "<color=red>" + m_CostMoneyNum.ToString() + "</color>";
                }
                else
                {
                    MoneyCostText.text = m_CostMoneyNum.ToString();
                }

                if (m_CostTicketNum > EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TICKET))
                {
                    btnInteractable = false;
                    TicketCostText.text = "<color=red>" + m_CostTicketNum.ToString() + "</color>";
                }
                else
                {
                    TicketCostText.text = m_CostTicketNum.ToString();
                }
            }

            AppraisalButton.interactable = btnInteractable;
        }

        private bool CalcAppraisalCost()
        {
            cmd_Crown_Gemstone_Item_Info mainItem = LogicDataCenter.crownPageDataManager.GetGemStoneItemData(SlotItemList[0].GemStoneDBID);
            if (mainItem.nGemstoneDBID != SlotItemList[0].GemStoneDBID)
                return false;

            SSchemeCrownGemstoneConfig mainGemstoneConfig = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(mainItem.nGemstoneID);
            if (mainGemstoneConfig == null)
                return false;

            cmd_Crown_Gemstone_Item_Info assItem = LogicDataCenter.crownPageDataManager.GetGemStoneItemData(SlotItemList[1].GemStoneDBID);
            if (assItem.nGemstoneDBID != SlotItemList[1].GemStoneDBID)
                return false;

            SSchemeCrownGemstoneConfig assGemstoneConfig = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(assItem.nGemstoneID);
            if (assGemstoneConfig == null)
                return false;

            m_CostMoneyNum = (int)((float)mainGemstoneConfig.nAppraisalProcess / (float)mainGemstoneConfig.nAppraisalProcessMax * (float)mainGemstoneConfig.nAppraisalCostMoney);

            m_CostTicketNum = (int)((float)mainGemstoneConfig.nAppraisalProcess / (float)mainGemstoneConfig.nAppraisalProcessMax * (float)mainGemstoneConfig.nAppraisalCostTicket);

            return true;
        }

        private void UpdateMainAppraisalGenstoneModel(int nGemsoneDBID, int nSlotID)
        {
            if (m_bIsClose)
                return;

            if (m_LastGemstoneModelList[nSlotID])
			{
				ResNode.DestroyRes(ref m_LastGemstoneModelList[nSlotID]);
				m_LastGemstoneModelList[nSlotID] = null;
			}
                
            if (GemstoneModelTrans == null)
                return;

            cmd_Crown_Gemstone_Item_Info data = LogicDataCenter.crownPageDataManager.GetGemStoneItemData(nGemsoneDBID);
            if (data.nGemstoneID <= 0)
                return;

            SSchemeCrownGemstoneConfig gemstoneConfig = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(data.nGemstoneID);
            if (gemstoneConfig == null)
                return;

            if (data.byGrade < 0 || data.byGrade >= gemstoneConfig.nExteriorIDArray.Length)
                return;

            SSchemeCrownGemstoneExteriorConfig exteriorConfig = CrownPageConfigPage.Instance.GetCrownPageExteriorConfig(gemstoneConfig.nExteriorIDArray[data.byGrade]);
            if (exteriorConfig == null)
                return;

            if (string.IsNullOrEmpty(exteriorConfig.szModelPath))
                return;

            ResNode itemNode = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "Effect/Scene/UI/StaticState/Baoshi/" + exteriorConfig.szModelPath);
            if (itemNode == null)
                return;

            Effect.EffectNode itemObject = itemNode.InstanceMainRes<Effect.EffectNode>();
            if (itemObject == null)
            {
                return;
            }

            itemObject.StopEffect();
            itemObject.PlayEffect(false,-1, itemObject.SrcObj, itemObject.DstObj);
            

            float fScale = (nSlotID == 0 ? exteriorConfig.fScale2 : exteriorConfig.fScale);

            m_LastGemstoneModelList[nSlotID] = itemObject.gameObject;
            itemObject.transform.localScale = new UnityEngine.Vector3(fScale, fScale, fScale);
            itemObject.transform.SetParent(GemstoneModelTrans[nSlotID], false);
            itemObject.transform.localPosition = new UnityEngine.Vector3(0f, 0f, 0f);

            Data.UISoundConfig.SSchemeUSoundData soundData = new Data.UISoundConfig.SSchemeUSoundData();
            Data.UISoundConfig.UISoundConfig.Instance.GetData("gem", "Button_gemusingframe_put", ref soundData);
            SoundManager.CreateUISound(soundData.USoundID);
        }

        public void OnAppraisalButtonClick()
        {
            cmd_Crown_Gemstone_Item_Info mainItem = LogicDataCenter.crownPageDataManager.GetGemStoneItemData(SlotItemList[0].GemStoneDBID);
            if (mainItem.nGemstoneDBID != SlotItemList[0].GemStoneDBID)
                return;

            SSchemeCrownGemstoneConfig mainGemstoneConfig = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(mainItem.nGemstoneID);
            if (mainGemstoneConfig == null)
                return;

            cmd_Crown_Gemstone_Item_Info assItem = LogicDataCenter.crownPageDataManager.GetGemStoneItemData(SlotItemList[1].GemStoneDBID);
            if (assItem.nGemstoneDBID != SlotItemList[1].GemStoneDBID)
                return;

            SSchemeCrownGemstoneConfig assGemstoneConfig = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(assItem.nGemstoneID);
            if (assGemstoneConfig == null)
                return;

            int nCostMoney = (int)((float)mainGemstoneConfig.nAppraisalProcess / (float)mainGemstoneConfig.nAppraisalProcessMax * (float)mainGemstoneConfig.nAppraisalCostMoney);

            int nCostTicket = (int)((float)mainGemstoneConfig.nAppraisalProcess / (float)mainGemstoneConfig.nAppraisalProcessMax * (float)mainGemstoneConfig.nAppraisalCostTicket);

            if (nCostMoney > EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_GOLD))
            {
                return;
            }

            if (nCostTicket > EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TICKET))
            {
                return;
            }

            string sGemstoneName = assGemstoneConfig.sGemstoneName;
            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                        modal: true,
                        title: ULocalizationService.Instance.Get("UIView", "CrownGem", "AppraisalTitle"),
                        message: string.Format(ULocalizationService.Instance.Get("UIView", "CrownGem", "AppraisalBoxMsgDesc"), sGemstoneName, nCostMoney, nCostTicket),
                        buttons: new UIWidgets.DialogActions() { { ULocalizationService.Instance.Get("UIView", "CrownGem", "ConfirmBtnDesc"), OnSendAppraisalGenmstoneMsg } }
                        );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        private bool OnSendAppraisalGenmstoneMsg()
        {
            m_AppraisalFlag = true;
            LogicDataCenter.crownPageDataManager.SendAppraisalGemstoneMessage();

            return true;
        }
    }

    public class PlayerCrownAppraisalContrastsItem : MonoBehaviour
    {
        public Text GemstoneNameText;
        public Text GemstoneGradeText;
        public Image GemstoneExpImage;
        //public Text GemstoneExpText;
        public Text GemstonePropertyHeadText;

        public Transform GemstonePropTrans;
        public GameObject TmpPropObj;

        private Text[] PropertyTextList;
        private int m_nPropCountMax = 5;

        private void ResetPropArray()
        {
            if (PropertyTextList == null)
            {
                TmpPropObj.SetActive(false);
                PropertyTextList = new Text[m_nPropCountMax];
                for (int i = 0; i < m_nPropCountMax; ++i)
                {
                    GameObject TextItemObject = Instantiate<GameObject>(TmpPropObj);
                    if (TextItemObject == null)
                    {
                        return;
                    }

                    Text item = TextItemObject.GetComponent<Text>();
                    if (item == null)
                    {
                        return;
                    }

                    TextItemObject.SetActive(false);
                    TextItemObject.transform.SetParent(GemstonePropTrans, false);
                    PropertyTextList[i] = item;
                }
            }

            foreach (Text item in PropertyTextList)
            {
                if (item == null)
                    continue;

                item.text = "";
                item.gameObject.SetActive(false);
            }
        }

        public void UpdateItem(cmd_Crown_Gemstone_Item_Info data)
        {
            ResetPropArray();

            this.gameObject.SetActive(data.nGemstoneDBID > 0);
            if (data.nGemstoneDBID <= 0)
                return;

            SSchemeCrownGemstoneConfig config = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(data.nGemstoneID);
            if (config == null)
                return;

            string gemstoneName = config.sGemstoneName;
            UBB.toHtml(ref gemstoneName);
            GemstoneNameText.text = gemstoneName;

            int nTotalPower = 0;
            int nPropIndex = 0;
            if (data.byGrade >= 0 && data.byGrade < config.szPropertyDescArray.Length)
            {
                string[] sCrownPropertyIDArray = ((string)(config.szPropertyArray[data.byGrade])).Split(';');
                int nPropertyID = 0;
                foreach (string sPropertyID in sCrownPropertyIDArray)
                {
                    int.TryParse(sPropertyID, out nPropertyID);
                    SSchemeCrownEffectConfig effectConfig = CrownPageConfigPage.Instance.GetCrownPageEffectConfig(nPropertyID);
                    if (effectConfig == null)
                        continue;

                    nTotalPower += effectConfig.nPropertyValue;

                    if (nPropIndex < PropertyTextList.Length)
                    {
                        String szPropDesc = effectConfig.sGemstoneDesc;
                        UBB.toHtml(ref szPropDesc, UBB_FORMAT_TYPE.UGUI);
                        PropertyTextList[nPropIndex].text = szPropDesc;
                        PropertyTextList[nPropIndex].gameObject.SetActive(effectConfig.sGemstoneDesc.Length > 0);
                        ++nPropIndex;
                    }
                }

                GemstonePropertyHeadText.text = ULocalizationService.Instance.Get("UIView", "CrownGem", "TotalPowerDesc1") + nTotalPower;
            }

            GemstoneGradeText.text = ULocalizationService.Instance.Get("UIView", "CrownGem", "EnergyValueDesc");
            GemstoneExpImage.fillAmount = 1;
            if (data.byGrade == (int)EGEMSTONE_GRADE_TYPE.EGGT_OBSOLETEGRADE && data.nAppraisalCount < config.nAppraisalProcessMax)
            {
                GemstoneExpImage.fillAmount = ((float)data.nAppraisalCount / (float)config.nAppraisalProcessMax);
                //GemstoneExpText.text = config.nExpUpper + "/" + config.nExpUpper;
            }
        }
    }

    public class PlayerCrownAppraisalContrastsFrame : PlayerCrownSecondSubFrameBase
    {
        public PlayerCrownAppraisalContrastsItem ProperyItemBefore;
        //public PlayerCrownAppraisalContrastsItem ProperyItemAfter;

        public override void UpdateSecondSubFrame()
        {
            GemStoneAppraisalContrastsData aAppraisalContrastsData = LogicDataCenter.crownPageDataManager.CrownPageAppraisalContrastsData;
            ProperyItemBefore.UpdateItem(aAppraisalContrastsData.appraisalBeforeData);
            //ProperyItemAfter.UpdateItem(aAppraisalContrastsData.appraisalAfterData);
        }
    }

    public class PlayerCrownAppraisalFrame : PlayerCrownSubFrameBase
    {
        public PlayerCrownAppraisalSlotFrame SlotFrame;
        public PlayerCrownAppraisalContrastsFrame ContrastsFrame;

        private GameObject m_CrownModelObj;
        private Transform m_CrownCameraTrans;
        public void OnEnable()
        {
            UpdateFrame();
            SetCrownModelVisible(true);
            SetCameraBeCloser();
            LogicDataCenter.crownPageDataManager.AppraisalVisible = true;
        }

        public override void UpdateFrame()
        {
            if (!gameObject.activeSelf)
                return;

            SlotFrame.UpdateSecondSubFrame();
            ContrastsFrame.UpdateSecondSubFrame();
        }

        public void OnDisable()
        {
            if (ParentCrownView)
                ParentCrownView.SetMainFrameSlotTrans();

            if (ParentCrownView && ParentCrownView.MainFrame)
                ParentCrownView.MainFrame.gameObject.SetActive(true);

            LogicDataCenter.crownPageDataManager.ClearCrownAppraisalData();
            SetCrownModelVisible(false);
            SetCameraBeReback();
            LogicDataCenter.crownPageDataManager.AppraisalVisible = false;
        }

        public GameObject AppraisaFrameStageModelObj
        {
            set { m_CrownModelObj = value; }
            get { return m_CrownModelObj; }
        }

        private void SetCrownModelVisible(bool bVisible)
        {
            if (m_CrownModelObj)
                m_CrownModelObj.SetActive(bVisible);
        }

        public Transform AppraisaFrameCameraTrans
        {
            set { m_CrownCameraTrans = value; }
            get { return m_CrownCameraTrans; }
        }

        private void SetCameraBeReback()
        {
            //if (m_CrownCameraTrans)
            //    m_CrownCameraTrans.localPosition = new UnityEngine.Vector3(0f, 0.62f, 2.56f);
        }

        private void SetCameraBeCloser()
        {
            //if (m_CrownCameraTrans)
            //    m_CrownCameraTrans.localPosition = new UnityEngine.Vector3(0.012f, 0.448f, 0.87f);
        }
    }

    #endregion

    #region 主界面
    public class PlayerCrownMainFrame : PlayerCrownSubFrameBase
    {
        public PlayerCrownMainSlotFrame SlotFrame;
        public PlayerCrownMainTopPageFrame TopFrame;
        public PlayerCrownMainPropertyTextFrame ProTextFrame;
        public PlayerCrownMainSDRRACFuntionFrame FuntionFrame;

        private int m_nLastSkinId = 0;
        private Transform m_SelectHeroTransform;
        private SkinInstance m_HeroGO;
        //private SceneEffectParamBase m_LastSkinEffectParam;
        //private int m_LastSkinSoundID = 0;

        public void Awake()
        {
            FuntionFrame.initTextDesc();
        }

        public void OnEnable()
        {
            OnEnableEx();
            UpdateFrame();
            SetCrownModelVisible(true);
        }

        private void OnEnableEx()
        {
            SlotFrame.OnEnable();
        }

        private void SetCrownModelVisible(bool bVisible)
        {
            //if (m_CrownModelObj)
            //    m_CrownModelObj.SetActive(bVisible);

            if (bVisible)
            {
                LoadCrownHeroSkinEffect();
            }
            else
            {
                CloseCrownHeroSkinEffect();
            }
        }

        public void OnDisable()
        {
            SetCrownModelVisible(false);
        }

        public override void UpdateFrame()
        {
            if (!gameObject.activeSelf)
                return;

            TopFrame.UpdateSecondSubFrame();
            SlotFrame.UpdateSecondSubFrame();
            ProTextFrame.UpdateSecondSubFrame();
        }

        public void OnCloseMainFrameButtonClick(bool bIsOn)
        {
            if (!bIsOn)
                return;

            this.gameObject.SetActive(false);
        }

        public void SetCrownHeroSkinEffect(Transform GemstoneModelTrans)
        {
            if (GemstoneModelTrans == null)
                return;

            m_SelectHeroTransform = GemstoneModelTrans;

            LoadCrownHeroSkinEffect();
        }

        public void LoadCrownHeroSkinEffect()
        {
            if (m_SelectHeroTransform == null || m_HeroGO != null)
                return;

            if (LogicDataCenter.playerSystemDataManager.bRequestSelf)
            {
                if (EntityFactory.MainHeroView != null && EntityFactory.MainHeroView.Property != null)
                {
                    int nHeroID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
                    int nSkinID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SKIN);
                    //SkinManager.SetChangeHeroSkinEffect(nSkinID, ref m_LastSkinEffectParam, ref m_nLastSkinId, m_SelectHeroTransform);

                    SkinManager.SetChangeHeroSkin(nSkinID, nHeroID, ref m_HeroGO, ref m_nLastSkinId, m_SelectHeroTransform, HEROSKIN_RENDER_TYPE.SCENE, ASpeedGame.Data.PersonModelTransFormInfo.PERSONMODELVIEWID.PERSONMODELVIEWID_WARSUPPORT);

                    // 音效
                    //SkinInfoScheme skinConfig = new SkinInfoScheme();
                    //if (GameLogicAPI.GetHeroSkinInfo(nSkinID, ref skinConfig))
                    //{
                    //    SoundManager.DeleteSound(m_LastSkinSoundID);
                    //    SoundManager.CreateUISound(skinConfig.nSoundID);
                    //    m_LastSkinSoundID = skinConfig.nSoundID;
                    //}
                }
            }
            else
            {
                ActorPlayerInfo playerInfo = LogicDataCenter.playerSystemDataManager.GetActorPlayerInfo(LogicDataCenter.playerSystemDataManager.nRequestPDBID);
                if (playerInfo != null)
                {
                    int nHeroID = playerInfo.playerInfo.nHeroID;
                    int nSkinID = playerInfo.playerInfo.nSkinID;
                    SkinManager.SetChangeHeroSkin(nSkinID, nHeroID, ref m_HeroGO, ref m_nLastSkinId, m_SelectHeroTransform, HEROSKIN_RENDER_TYPE.SCENE, ASpeedGame.Data.PersonModelTransFormInfo.PERSONMODELVIEWID.PERSONMODELVIEWID_WARSUPPORT);
                }
            }
        }

        public void CloseCrownHeroSkinEffect()
        {
            if (SkinInstance.isNullOrEmpry(m_HeroGO))
                return;

            //SceneEffectManager.Instance.DestroySceneEffect(ref m_LastSkinEffectParam);
            //m_LastSkinEffectParam = null;
            SkinManager.DeleteSkin(ref m_HeroGO);
            //SkinManager.CacheSkin(m_nLastSkinId, m_HeroGO);
            m_nLastSkinId = 0;
            m_HeroGO = null;
            //SoundManager.DeleteSound(m_LastSkinSoundID);
            //m_LastSkinSoundID = 0;
        }
    }

    public class PlayerCrownMainSDRRACFuntionFrame : PlayerCrownSecondSubFrameBase
    {
        public Text AppraisalBtnText;
        public Text ShowBtnText;
        public Text SellBtnText;

        public void initTextDesc()
        {
            AppraisalBtnText.text = ULocalizationService.Instance.Get("UIView", "CrownGem", "AppraisalBtnDesc");
            ShowBtnText.text = ULocalizationService.Instance.Get("UIView", "CrownGem", "ShowBtnDesc");
            SellBtnText.text = ULocalizationService.Instance.Get("UIView", "CrownGem", "SellBtnDesc");
        }

        public void onSaveBtnClick()
        {
            LogicDataCenter.crownPageDataManager.SaveCurCrownPage();
        }

        public void onRescoverBtnClick()
        {
            LogicDataCenter.crownPageDataManager.RescoverCurCrownPage();
        }

        public void onResetBtnClick()
        {
            LogicDataCenter.crownPageDataManager.ResetCurCrownPage();
        }

        public void onDeleteBtnClick()
        {
            LogicDataCenter.crownPageDataManager.DeleteCurCrownPage();

        }

        public void onShowOfBtnClick()
        {
            LogicDataCenter.crownPageDataManager.ShowOfCrownPage();
        }
    }

    public class PlayerCrownMainSlotFrame : PlayerCrownSecondSubFrameBase
    {
        public PlayerCrownGemstoneInlayItem[] SlotItemList;

        private Transform[] CrownInlayTransList;
        private GameObject[] m_LastGemstoneModelList;
        private int[] m_LastGemstoneDBIDList;
        private bool m_bIsClose = false;

        public void Awake()
        {
            m_LastGemstoneModelList = new GameObject[UDefines.nInlaySlotMax];
            m_LastGemstoneDBIDList = new int[UDefines.nInlaySlotMax];
        }

        public void Destroy()
        {
            CrownInlayTransList = null;

            if (m_LastGemstoneModelList != null)
            {
                for (int i = 0; i < m_LastGemstoneModelList.Length; ++i)
                {
                    if (m_LastGemstoneModelList[i] != null)
                    {
                        ResNode.DestroyRes(ref m_LastGemstoneModelList[i]);
                        m_LastGemstoneModelList[i] = null;
                    }     
                }
            }

            if (m_LastGemstoneDBIDList != null)
            {
                for (int i = 0; i < m_LastGemstoneDBIDList.Length; ++i)
                {
                    m_LastGemstoneDBIDList[i] = 0;
                }
            }
        }

        public void OnEnable()
        {
            m_bIsClose = false;
        }

        public void OnDisable()
        {
            m_bIsClose = true;
            CrownInlayTransList = null;

            if (m_LastGemstoneModelList != null)
            {
                for (int i = 0; i < m_LastGemstoneModelList.Length; ++i)
                {
                    if (m_LastGemstoneModelList[i] != null)
                    {
                        ResNode.DestroyRes(ref m_LastGemstoneModelList[i]);
                        m_LastGemstoneModelList[i] = null;
                    }
                }
            }

            if (m_LastGemstoneDBIDList != null)
            {
                for (int i = 0; i < m_LastGemstoneDBIDList.Length; ++i)
                {
                    m_LastGemstoneDBIDList[i] = 0;
                }
            }
        }

        public void SwithcSlotState(int slotID, bool bActive)
        {
            if (slotID < 0 || slotID > m_LastGemstoneModelList.Length - 1)
                return;

            if (m_LastGemstoneModelList[slotID] != null)
            {
                m_LastGemstoneModelList[slotID].SetActive(bActive);
                if (bActive)
                {
                    Effect.EffectNode node = m_LastGemstoneModelList[slotID].GetComponent<Effect.EffectNode>();
                    if (node != null)
                    {
                        node.StopEffect();
                        node.PlayEffect(false, -1, node.SrcObj, node.DstObj);
                    }
                }
            }
        }

        public override void UpdateSecondSubFrame()
        {
            bool bRequestSelf = LogicDataCenter.playerSystemDataManager.bRequestSelf;
            CrownPageItemData data = (bRequestSelf ? LogicDataCenter.crownPageDataManager.CrownPageCurPageItemData : LogicDataCenter.crownPageDataManager.CrownPageOtherCurPageItemData);
            if (data == null)
                return;

            cmd_Crown_Page_Item_Info reviseData = data.reviseData;

            if (SlotItemList == null)
                return;

            foreach (PlayerCrownGemstoneInlayItem item in SlotItemList)
            {
                if (item)
                {
                    item.ClearView();
                }
            }

            for (int i = 0; i < reviseData.crownInlaySlotList.Length; ++i)
            {
                int nGemsoneDBID = reviseData.crownInlaySlotList[i];
                if (!LogicDataCenter.crownPageDataManager.CrownGemtoneDic.ContainsKey(nGemsoneDBID))
                {
                    nGemsoneDBID = 0;
                }

                SlotItemList[i].UpdateView(nGemsoneDBID, i);
                UpdateInlaySlotGemstoneModel(nGemsoneDBID, i);
            }
        }

        public void SetMainFrameModelHangingPointTrans(Transform[] hangingPointTrans)
        {
            this.CrownInlayTransList = hangingPointTrans;
            UpdateSecondSubFrame();
        }

        private void UpdateInlaySlotGemstoneModel(int nGemsoneDBID, int nSlotID)
        {
            if (m_bIsClose)
                return;

            if (CrownInlayTransList == null)
                return;

            if (m_LastGemstoneModelList == null || nSlotID < 0 || nSlotID >= m_LastGemstoneModelList.Length)
                return;

            if (m_LastGemstoneDBIDList == null || nSlotID >= m_LastGemstoneDBIDList.Length || (m_LastGemstoneDBIDList[nSlotID] == nGemsoneDBID && nGemsoneDBID > 0))
                return;

            if (m_LastGemstoneModelList[nSlotID])
            {
                ResNode.DestroyRes(ref m_LastGemstoneModelList[nSlotID]);
                m_LastGemstoneModelList[nSlotID] = null;
            }

            if (m_LastGemstoneDBIDList[nSlotID] > 0)
                m_LastGemstoneDBIDList[nSlotID] = 0;

            if (CrownInlayTransList == null || nSlotID >= CrownInlayTransList.Length)
                return;

            cmd_Crown_Gemstone_Item_Info data = LogicDataCenter.crownPageDataManager.GetGemStoneItemData(nGemsoneDBID);
            if (data.nGemstoneID <= 0)
                return;

            SSchemeCrownGemstoneConfig gemstoneConfig = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(data.nGemstoneID);
            if (gemstoneConfig == null)
                return;

            if (data.byGrade < 0 || data.byGrade >= gemstoneConfig.nExteriorIDArray.Length)
                return;

            SSchemeCrownGemstoneExteriorConfig exteriorConfig = CrownPageConfigPage.Instance.GetCrownPageExteriorConfig(gemstoneConfig.nExteriorIDArray[data.byGrade]);
            if (exteriorConfig == null)
                return;

            if (string.IsNullOrEmpty(exteriorConfig.szModelPath))
                return;

            ResNode itemNode = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"Effect/Scene/UI/StaticState/Baoshi/" + exteriorConfig.szModelPath);
            if (itemNode == null)
                return;

            Effect.EffectNode itemObject = itemNode.InstanceMainRes<Effect.EffectNode>();
            if (itemObject == null)
            {
                return;
            }

            itemObject.StopEffect();
            itemObject.PlayEffect(false,-1, itemObject.SrcObj, itemObject.DstObj);

            m_LastGemstoneModelList[nSlotID] = itemObject.gameObject;
            m_LastGemstoneDBIDList[nSlotID] = nGemsoneDBID;
            itemObject.transform.localScale = new UnityEngine.Vector3(exteriorConfig.fScale, exteriorConfig.fScale, exteriorConfig.fScale);
            itemObject.transform.SetParent(this.CrownInlayTransList[nSlotID], false);
            //itemObject.transform.localRotation = new Quaternion(0, 0, 0, 0);
            itemObject.transform.localPosition = new UnityEngine.Vector3(0f, 0f, 0f);

            Data.UISoundConfig.SSchemeUSoundData soundData = new Data.UISoundConfig.SSchemeUSoundData();
            Data.UISoundConfig.UISoundConfig.Instance.GetData("gem", "Button_gemusingframe_put", ref soundData);
            SoundManager.CreateUISound(soundData.USoundID);
        }
    }

    public class PlayerCrownMainTopButtonItem : MonoBehaviour
    {
        public Text NumText;
        private int m_nDBPageIndex;
        public int DBPageIndex
        {
            set { m_nDBPageIndex = value; }
            get { return m_nDBPageIndex; }
        }
    }

    public class PlayerCrownMainTopPageFrame : PlayerCrownSecondSubFrameBase
    {
        public GameObject TmpTopButtonObj;
        public Transform TopButtonFrameTrans;
        public Button AddPageButton;
        public InputField PageNameInput;
        private List<GameObject> TopButtonObjList = new List<GameObject>();

        private int m_CurPageBtnIndex = 0;
        private int m_MaxPageNum = 10;

        public void Start()
        {
            UpdateSecondSubFrame();
        }

        public override void UpdateSecondSubFrame()
        {
            ClearAllTopButton();

            if (TopButtonObjList.Count > 0)
                return;

            AddPageButton.gameObject.SetActive(false);

            bool bRequestSelf = LogicDataCenter.playerSystemDataManager.bRequestSelf;

            PageNameInput.text = (bRequestSelf ? LogicDataCenter.crownPageDataManager.CrownPageName : LogicDataCenter.crownPageDataManager.OtherCrownPageName);
            PageNameInput.interactable = bRequestSelf;

            TmpTopButtonObj.SetActive(false);
            
            foreach (CrownPageItemData data in LogicDataCenter.crownPageDataManager.CrownPageDic.Values)
            {
                GameObject ItemBtnObject = Instantiate<GameObject>(TmpTopButtonObj);
                ItemBtnObject.transform.FindChild("Text").GetComponent<Text>().text = data.nPageTextIndex.ToString();

                ItemBtnObject.transform.SetParent(TopButtonFrameTrans, false);

                TopButtonObjList.Add(ItemBtnObject);

                ItemBtnObject.SetActive(true);
            }

            if (TopButtonObjList.Count < m_MaxPageNum && bRequestSelf)
            {
                AddPageButton.gameObject.SetActive(true);
                AddPageButton.transform.SetAsLastSibling();
            }

            int nSelectIndex = (bRequestSelf ? LogicDataCenter.crownPageDataManager.CrownPageTextIndex : LogicDataCenter.crownPageDataManager.OtherCrownPageTextIndex);
            if (nSelectIndex > 0 && nSelectIndex <= TopButtonObjList.Count)
            {
                m_CurPageBtnIndex = nSelectIndex;
                TopButtonFrameTrans.GetComponent<UButtonGroup>().SetItemSelectedItem(nSelectIndex - 1);
                //TopButtonObjList[nSelectIndex - 1].GetComponent<UButtonGroupItem>().Selected = true;
            }          
        }

        public void ClearAllTopButton()
        {
            foreach (GameObject TopButtonObj in TopButtonObjList)
            {
                var temp = TopButtonObj;
                ResNode.DestroyRes(ref temp);
            }

            TopButtonObjList.Clear();
            m_CurPageBtnIndex = 0;
            AddPageButton.gameObject.SetActive(false);
        }

        public void onTextEndEdit(string sText)
        {
            if (!LogicDataCenter.playerSystemDataManager.bRequestSelf || !PageNameInput.interactable)
                return;

            if (ShieldWord.Instance.IsHaveShieldWord(sText))
            {
                PageNameInput.text = LogicDataCenter.crownPageDataManager.CrownPageName;
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_PLAYER_CROWNPAGENAME_SHIELD_WORD);
                return;
            }

            LogicDataCenter.crownPageDataManager.CrownPageName = sText;
        }

        public void onNumListBtnClick(UButtonGroupItem btnItem, bool bActive)          // 切换页的时候弹窗询问。目前暂时直接保存
        {
            if (!bActive)
                return;

            int nIndex = int.Parse(btnItem.transform.FindChild("Text").GetComponent<Text>().text);
            if (nIndex == m_CurPageBtnIndex)
                return;

            if (LogicDataCenter.playerSystemDataManager.bRequestSelf)
            {
                if (LogicDataCenter.crownPageDataManager.ChangeCrownPage(nIndex) == true)
                {
                    m_CurPageBtnIndex = nIndex;
                    PageNameInput.text = LogicDataCenter.crownPageDataManager.CrownPageName;
                    PageNameInput.interactable = false;
                }
            }
            else
            {
                if (LogicDataCenter.crownPageDataManager.OtherChangeCrownPage(nIndex) == true)
                {
                    m_CurPageBtnIndex = nIndex;
                    PageNameInput.text = LogicDataCenter.crownPageDataManager.OtherCrownPageName;
                    PageNameInput.interactable = false;
                }
            }            
        }

        public void onAddBtnClick()
        {
            if (!LogicDataCenter.playerSystemDataManager.bRequestSelf)
                return;

            int nSellID = 0;
            List<SSchemePointShopGoodsConfig> configList = PointShopConfig.Instance.GetGoodsListByType(PointShopGoodsType.Goods_GemStone);
            if (configList != null && configList.Count > 0)
            {
                foreach (var item in configList)
                {
                    SSchemeActorPrizeConfig prizeConfig = new SSchemeActorPrizeConfig();
                    prizeConfig = ActorPrizeConfig.Instance.GetActorPrizeConfig(item.nPrizeID);
                    if(prizeConfig != null && prizeConfig.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_GEMSTONEPAGE)
                    {
                        nSellID = item.nSellID;
                        break;
                    }
                }

                if (nSellID > 0)
                {
                    PointShopBuyGoodsInitData uiData = new PointShopBuyGoodsInitData();
                    uiData.nSellID = nSellID;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOPBUYGOODS_OPEN, uiData);
                }
            }

            //if (!bActive)
            //    return;

            //if (!LogicDataCenter.playerSystemDataManager.bRequestSelf)
            //    return;

            //if (LogicDataCenter.crownPageDataManager.AddCurCrownPage() == false || LogicDataCenter.crownPageDataManager.CrownPageCurPageItemData == null)
            //    return;

            //int curIndex = LogicDataCenter.crownPageDataManager.CrownPageCurPageItemData.nPageTextIndex;
            //if (curIndex != TopButtonObjList.Count + 1)
            //    return;

            //GameObject ItemBtnObject = Instantiate<GameObject>(TmpTopButtonObj);
            //ItemBtnObject.transform.FindChild("Text").GetComponent<Text>().text = curIndex.ToString();
            //ItemBtnObject.transform.SetParent(TopButtonFrameTrans, false);
            //TopButtonObjList.Add(ItemBtnObject);
            //ItemBtnObject.SetActive(true);

            //PageNameInput.text = LogicDataCenter.crownPageDataManager.CrownPageName;
            //PageNameInput.interactable = false;

            //m_CurPageBtnIndex = curIndex;
            //ItemBtnObject.GetComponent<UButtonGroupItem>().Selected = true;

            //if (TopButtonObjList.Count < m_MaxPageNum)
            //{
            //    AddPageButton.gameObject.SetActive(true);
            //    AddPageButton.transform.SetAsLastSibling();
            //}
            //else
            //{
            //    AddPageButton.gameObject.SetActive(false);
            //}
        }

        public void OnEditPageNameClick()
        {
            if (LogicDataCenter.playerSystemDataManager.bRequestSelf && PageNameInput.interactable == false)
                PageNameInput.interactable = true;
        }

        public void OnDisable()
        {
            ClearAllTopButton();
        }
    }

    public class PlayerCrownMainPropertyTextFrame : PlayerCrownSecondSubFrameBase
    {
        private Text[] PropertyTextList;
        public Text PropertyHeadText;
        public Transform PropertyTextTrans;
        public GameObject TmpTextItemObj;
        private int m_nSlotCountMax = 8;
 
        public void Awake()
        {
        
        }

        public void DoInitSlotList()
        {
            TmpTextItemObj.SetActive(false);
            PropertyTextList = new Text[m_nSlotCountMax];
            for (int i = 0; i < m_nSlotCountMax; ++i)
            {
                GameObject TextItemObject = ResNode.InstantiateRes(TmpTextItemObj);
                if (TextItemObject == null)
                {
                    return;
                }

                Text item = TextItemObject.GetComponent<Text>();
                if (item == null)
                {
                    return;
                }

                TextItemObject.SetActive(false);
                TextItemObject.transform.SetParent(PropertyTextTrans, false);
                PropertyTextList[i] = item;
            }
        }

        public override void UpdateSecondSubFrame()
        {
            if (PropertyTextList == null)
                DoInitSlotList();

            if (PropertyTextList == null || PropertyHeadText == null)
                return;

            foreach (Text item in PropertyTextList)
            {
                if (item == null)
                    continue;

                item.text = "";
                item.gameObject.SetActive(false);
            }

            if (LogicDataCenter.crownPageDataManager.CrownPageInlayList == null)
                return;

            int nPower = 0;
            int nFaceValue = 0;
            Dictionary<string, CrownTypeNameInfo> nameTypeValueDic = new Dictionary<string, CrownTypeNameInfo>();
            for (int i = 0; i < LogicDataCenter.crownPageDataManager.CrownPageInlayList.Length; ++i)
            {
                int nGemstoneDBID = LogicDataCenter.crownPageDataManager.CrownPageInlayList[i];
                cmd_Crown_Gemstone_Item_Info data = LogicDataCenter.crownPageDataManager.GetGemStoneItemData(nGemstoneDBID);

                SSchemeCrownGemstoneConfig gemstoneConfig = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(data.nGemstoneID);
                if (gemstoneConfig == null)
                    continue;

                if (data.byGrade < 0 || data.byGrade >= (int)EGEMSTONE_GRADE_TYPE.EGGT_MAX)
                    continue;

                string[] sCrownPropertyIDArray = ((string)(gemstoneConfig.szPropertyArray[data.byGrade])).Split(';');
                int nPropertyID = 0;
                foreach (string sPropertyID in sCrownPropertyIDArray)
                {
                    int.TryParse(sPropertyID, out nPropertyID);
                    SSchemeCrownEffectConfig effectConfig = CrownPageConfigPage.Instance.GetCrownPageEffectConfig(nPropertyID);
                    if (effectConfig == null)
                        continue;

                    nPower += effectConfig.nPropertyValue;

                    if (!nameTypeValueDic.ContainsKey(effectConfig.sGemstoneName))
                    {
                        CrownTypeNameInfo info = new CrownTypeNameInfo();
                        info.nType = effectConfig.nPropertyNameType;
                        nameTypeValueDic[effectConfig.sGemstoneName] = info;
                    }

                    nameTypeValueDic[effectConfig.sGemstoneName].typeValue += effectConfig.nPropShowValue;
                }

                if (data.byGrade < 0 || data.byGrade >= gemstoneConfig.nExteriorIDArray.Length)
                    continue;

                int nExteriorID = gemstoneConfig.nExteriorIDArray[data.byGrade];
                SSchemeCrownGemstoneExteriorConfig exteriorConfig = CrownPageConfigPage.Instance.GetCrownPageExteriorConfig(nExteriorID);
                if (exteriorConfig == null)
                    continue;

                nFaceValue += exteriorConfig.nFacescore;
            }

            PropertyHeadText.text = ULocalizationService.Instance.Get("UIView", "CrownGem", "TotalPowerDesc2") + nPower;
            PropertyHeadText.gameObject.SetActive(nPower > 0);

            int nTextIndex = 0;
            foreach (string sName in nameTypeValueDic.Keys)
            {
                if (nTextIndex < 0 || nTextIndex >= PropertyTextList.Length)
                    break;

                string szDesc = sName + GetPropertyName(nameTypeValueDic[sName].nType, nameTypeValueDic[sName].typeValue);

                if (nTextIndex < PropertyTextList.Length)
                {
                    PropertyTextList[nTextIndex].text = szDesc;
                    PropertyTextList[nTextIndex].gameObject.SetActive(sName.Length > 0);
                    nTextIndex++;
                }                
            }

            CrownPageItemData pageData = LogicDataCenter.crownPageDataManager.CrownPageCurPageItemData;
            if (pageData == null)
                return;

            pageData.nFaceValue = nFaceValue;
            pageData.nPowerValue = nPower;
        }

        public string GetPropertyName(int nType, double nValue)
        {
            string sOperater = "";
            string sPersent = "";
            nValue /= 100;

            sOperater = LogicDataCenter.runePageDataManager.IsAddOperater(nType) ? "+" : "-";
            nValue = LogicDataCenter.runePageDataManager.IsRuneGrow(nType) ? nValue * 18 : nValue;

            if (LogicDataCenter.runePageDataManager.IsPersentOperater(nType))
            {
                nValue /= 100;
                sPersent = "%";
            }

            if (LogicDataCenter.runePageDataManager.IsEnlargeHundredOperater(nType))
            {
                nValue /= 100;
            }

            return sOperater + nValue + sPersent;
        }
    }
    #endregion

    #region 隐藏其他玩家部分不显示界面
    public class PlayerCrownOtherHideItemsFrame : PlayerCrownSubFrameBase
    {
        public GameObject[] HideObjList;
        public GameObject[] ShowObjList;
        public void StartHide()
        {
            SetListObjItemVisible(false);
        }

        public void StartShow()
        {
            SetListObjItemVisible(true);
        }

        public void SetListObjItemVisible(bool bVisible)
        {
            if (HideObjList == null||ShowObjList == null)
                return;

            foreach (GameObject item in HideObjList)
            {
                if (item == null)
                    continue;

                item.SetActive(bVisible);
            }

            foreach (GameObject item in ShowObjList)
            {
                if (item == null)
                    continue;

                item.SetActive(!bVisible);
            }
        }
    }
    #endregion 隐藏其他人部分不显示界面

    public class PlayerCrownView : UIBaseWndView//UIBaseSubWndView
    {
        public PlayerCrownAppraisalFrame AppraisalFrame;
        public PlayerCrownBackPackFrame BackPackFrame;
        public PlayerCrownMainFrame MainFrame;
        public PlayerCrownOtherHideItemsFrame OtherHideItemsFrame;

        public Image MainBg;
        public Image AppraisalBg;
        public Button CloseBtn;

        private PlayerCrownWnd m_PlayerCrownWnd;
        private Transform[] m_CrownInlayTransList;

        public override void Update()
        {
            if (InputManager.GetKey(KeyCode.Escape))
                OnCloseButtonClick();
        }

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            m_PlayerCrownWnd = wnd as PlayerCrownWnd;
            SetSubFramesParent();

            return true;
        }

        internal override void SetRendererVisible(bool visible)
        {
            base.SetRendererVisible(visible);
            if (visible)
            {
                BackPackFrame.UpdateFrame();

                if (AppraisalFrame.gameObject.activeSelf)
                {
                    MainBg.enabled = false;
                    AppraisalBg.enabled = true;
                }
                else
                {
                    MainBg.enabled = true;
                    AppraisalBg.enabled = false;
                }
            }
        }

        public void UpdateView()
        {
            SetMainFrameSlotTrans();
            AppraisalFrame.UpdateFrame();
            BackPackFrame.UpdateFrame();
            MainFrame.UpdateFrame();
        }

        public void SetMainFrameSlotTrans()
        {
            if (MainFrame != null && MainFrame.SlotFrame != null)
            {
                MainFrame.SlotFrame.SetMainFrameModelHangingPointTrans(m_CrownInlayTransList);
            }
        }

        public void SetSubFramesParent()
        {
            AppraisalFrame.ParentCrownView = this;
            BackPackFrame.ParentCrownView = this;
            MainFrame.ParentCrownView = this;
        }

        public void OnCloseButtonClick()
        {
            if (m_PlayerCrownWnd != null)
                m_PlayerCrownWnd.SetVisible(false);
        }

        public void OnHidCloseButton()
        {
            if (CloseBtn != null)
                CloseBtn.gameObject.SetActive(false);

            MainBg.enabled = false;
            AppraisalBg.enabled = true;
        }

        public void OnShowCloseButton()
        {
            if (CloseBtn != null)
                CloseBtn.gameObject.SetActive(true);

            MainBg.enabled = true;
            AppraisalBg.enabled = false;
        }

        public void OnMainFrameAppraisalBtnClick()
        {
            bool hasContain = false;
            Dictionary<int, cmd_Crown_Gemstone_Item_Info> stoneMap = LogicDataCenter.crownPageDataManager.CrownPageGemStoneInfo;
            if (stoneMap != null && stoneMap.Values != null)
            {
                foreach (cmd_Crown_Gemstone_Item_Info item in stoneMap.Values)
                {
                    if (item.byGrade == (byte)EGEMSTONE_GRADE_TYPE.EGGT_OBSOLETEGRADE)
                    {
                        hasContain = true;
                        break;
                    }
                }
            }
            
            if (!hasContain)
            {
                UIDialogData data = new UIDialogData(title: ULocalizationService.Instance.Get("UIView", "CrownGem", "MainFrameAppraisalBtnTitle"),
                            message: ULocalizationService.Instance.Get("UIView", "CrownGem", "MainFrameAppraisalBtnDesc"),
                            buttons: new UIWidgets.DialogActions() { { ULocalizationService.Instance.Get("UIView", "CrownGem", "MainFrameAppraisalBtnConfirm"), null } },
                            closeButtonAction: null,
                            modal: true,
                            hideByModalClick:false);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);

                return;
            }

            AppraisalFrame.gameObject.SetActive(true);
            MainFrame.OnCloseMainFrameButtonClick(true);
            BackPackFrame.UpdateFrame();
            OnHidCloseButton();
        }

        public void SetCrownModelHangingPointTrans(Transform GemstoneModelTrans, Transform[] CrownInlayTransList, GameObject AppraisalStageObj, Transform CameraTrans, Transform[] AppraisalTransList)
        {
            m_CrownInlayTransList = CrownInlayTransList;
            if (MainFrame != null && MainFrame.SlotFrame != null)
            {
                MainFrame.SetCrownHeroSkinEffect(GemstoneModelTrans);
                MainFrame.SlotFrame.SetMainFrameModelHangingPointTrans(m_CrownInlayTransList);
            }

            if (AppraisalFrame != null && AppraisalFrame.SlotFrame != null)
            {
                AppraisalFrame.AppraisaFrameStageModelObj = AppraisalStageObj;
                AppraisalFrame.AppraisaFrameCameraTrans = CameraTrans;
                AppraisalFrame.SlotFrame.SetAppraisalGemstoneModelTrans(AppraisalTransList, AppraisalStageObj);
            }

            UpdateView();
        }

        public void ResetSceneHangingPoint()
        {
            m_CrownInlayTransList = null;
        }

        public void ShowCrownPageAppraisalRet(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(cmdData.ptrParam, cmdData.nPtrLen);
            GameLogic.cmd_Crown_Gemstone_Appraisal_Ret appraisalRet = helper.get<GameLogic.cmd_Crown_Gemstone_Appraisal_Ret>();

            AppraisalFrame.SlotFrame.StartAppraisalModelSceneEffect(appraisalRet.nAppreaisalRet == 1);

            if (appraisalRet.nAppreaisalRet == 1)
                LogicDataCenter.crownPageDataManager.UpdateGemStoneAppraisalAfterData(appraisalRet); 
        }

        public void ShowCrownModel(crownActiveSlotModelData data)
        {
            if(data.type == EGEMSTONEDRAGTYPE.EGDT_INLYAITEM)
            {
                if(MainFrame != null && MainFrame.SlotFrame != null)
                {
                    MainFrame.SlotFrame.SwithcSlotState(data.slotID, data.bActive);
                }
            }
        }
    }
}
