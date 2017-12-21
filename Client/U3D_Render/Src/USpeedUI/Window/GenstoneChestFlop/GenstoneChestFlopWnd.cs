/*******************************************************************
** 文件名:	GenstoneChestFlopWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-12-8
** 版  本:	1.0
** 描  述:	宝石宝箱打开界面
		
********************************************************************/
using System.Collections.Generic;
using GameLogic;
using System;
using System.Runtime.InteropServices;
using DataCenter;
using USpeedUI.TooltipAndDialog;
using UnityEngine;
using System.Collections;
using U3D_Render;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using ASpeedGame.Data.CrownGenstoneConfig;
using UIWidgets;
using USpeedUI.UExtensions;
using Data.ActorPrizeConfig;
namespace USpeedUI.GenstoneChestFlop
{
    public class GenstoneChestFlopWnd : UIPopupWnd<GenstoneChestFlopView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_GEMSTONECHESTFLOP;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "GenstoneChestFloptView/GenstoneChestFloptView";
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

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_GEMSTONECHESTFLOP_UPDATEVIEW, this);
       
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_GEMSTONECHESTFLOP_UPDATEVIEW, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_GEMSTONECHESTFLOP_UPDATEVIEW:
                    {
                        this.SetVisible(true);
                        if (m_wndView)
                            m_wndView.UpdateView();
                    }
                    break;
                default:
                    break;
            }
        }

        public override bool OnCloseKey()
        {
            return true;
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
            //UISystem.Instance.ShowWnd(WndID.WND_ID_PRIZECHEST, !visible);
        }
    }

    public enum EGEMSTONESELECTMODEL
    {
        EGSM_COB,
        EGSM_WIN,
        EGSM_END,
    }

    public class GenstoneChestFlopItem:MonoBehaviour
    {
        public Image CobGemstoneImage;
        public Image WinGensoneImage;
        public Image SeletedGemsotneImage;
        public GenstoneChestFlopView ParentView;
        private EGEMSTONESELECTMODEL m_eSelectModel;
        private int m_nGemstoneID = 0;
        private int m_nCobstoneID = 0;
        private int m_nSlotID = 0;
        private UTooltipTrigger m_tooltip;
        public Toggle FlopToggle;
        public GameObject EndFrameObj;
        public void UpdateItem(int nCobGemstoneID, bool bWin, bool bWinEnd, int cobStoneID, SSchemeCrownGemstoneConfig config)
        {
            SSchemeCrownCobstoneExteriorConfig cobConfig = CrownPageConfigPage.Instance.GetCrownPageCobConfig(cobStoneID);
            if (cobConfig == null)
                return;

            this.FlopToggle.interactable = true;
            EndFrameObj.SetActive(false);
            m_nGemstoneID = nCobGemstoneID;
            m_nCobstoneID = cobStoneID;
            CobGemstoneImage.sprite = LogicDataCenter.crownPageDataManager.GetCrownGemStoneSpriteByIconID(cobConfig.nCobIconID, 2);
            WinGensoneImage.sprite = LogicDataCenter.crownPageDataManager.GetCrownGemStoneSpriteByIconID(config.nGemstoneIconID, 1);
            SeletedGemsotneImage.sprite = LogicDataCenter.crownPageDataManager.GetCrownGemStoneSpriteByIconID(config.nGemstoneIconID,1);

            if(bWin)
            {
                WinGensoneImage.gameObject.SetActive(true);
            }

            if(bWinEnd)
            {
                EnterCobModel();
            }
            else
            {
                EnterWinModel();
            }

            SetCobstoneToolTip();
        }

        public void SetCobstoneToolTip()
        {
            if (m_tooltip)
            {
                this.gameObject.RemoveComponent<UTooltipTrigger>();
                m_tooltip = null;
            }

            if (m_tooltip == null)
            {
                m_tooltip = this.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_CobstoneItem);
            }

            if (m_tooltip == null)
                return;

            m_tooltip.backgroundTint = Color.grey;

            SSchemeCrownCobstoneExteriorConfig config = CrownPageConfigPage.Instance.GetCrownPageCobConfig(m_nCobstoneID);
            if (config == null)
                return;

            m_tooltip.SetText("GemstoneName", config.szName);
            m_tooltip.SetText("GemstoneDesc", config.szDesc);
            Sprite gemstoneSprite = LogicDataCenter.crownPageDataManager.GetCrownGemStoneSpriteByIconID(config.nCobIconID,2);
            m_tooltip.SetImage("GemstoneIcon", gemstoneSprite);
        }

        public void SetGemstoneToolTip()
        {
            if (m_tooltip)
            {
                this.gameObject.RemoveComponent<UTooltipTrigger>();
                m_tooltip = null;
            }
               if (m_tooltip == null)
            {
                m_tooltip = this.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_GemstoneItem);
            }

            if (m_tooltip == null)
                return;

            m_tooltip.backgroundTint = Color.grey;

            SSchemeCrownGemstoneConfig config = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(m_nGemstoneID);
            if (config == null)
                return;

            int m_nGemstoneGrade = 1;
            if (m_nGemstoneGrade < 0 || m_nGemstoneGrade >= UDefines.sGemstoneGradeDesc.Length)
                return;

            m_tooltip.SetText("GemstoneName", config.sGemstoneName);
            m_tooltip.SetText("GemstoneGrade", UDefines.sGemstoneGradeDesc[m_nGemstoneGrade]);

            Sprite gemstoneSprite = LogicDataCenter.crownPageDataManager.GetCrownGemStoneSpriteByIconID(config.nGemstoneIconID);
            m_tooltip.SetImage("GemstoneIcon", gemstoneSprite);

            string sGemstonePropertyHeadText = "";
            string sGemstonePropertyDescText = "";
            if (m_nGemstoneGrade > 0 && m_nGemstoneGrade < config.szPropertyDescArray.Length)
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

                sGemstonePropertyHeadText = "威力 :" + nTotalPower;
                sGemstonePropertyDescText = sPropertyDesc;
            }

            m_tooltip.SetText("GemstonePropertyDesc", sGemstonePropertyDescText);
            m_tooltip.SetText("GemstonePropertyHead", sGemstonePropertyHeadText);

            if ( m_nGemstoneGrade == (int)EGEMSTONE_GRADE_TYPE.EGGT_OBSOLETEGRADE)
            {
                //GemstoneExpImage.fillAmount = 1;
                m_tooltip.SetText("GemstoneExp", config.nExpUpper + "/" + config.nExpUpper);
            }
            else
            {
                string[] expArray = config.szExpArray.Split(';');
                int nNextExp = 0;
                int nGrade = m_nGemstoneGrade;
                if (nGrade - 1 >= 0 && nGrade - 1 < expArray.Length)
                {
                    int.TryParse(expArray[nGrade - 1], out nNextExp);
                    nNextExp = nNextExp <= 0 ? 1 : nNextExp;
                    // GemstoneExpImage.fillAmount = ((float)data.nExp / nNextExp);
                }

                m_tooltip.SetText("GemstoneExp", 0 + "/" + nNextExp);
            }

            int nExteriorID = 0;
            if (m_nGemstoneGrade >= 0 && m_nGemstoneGrade < config.nExteriorIDArray.Length)
                nExteriorID = config.nExteriorIDArray[m_nGemstoneGrade];

            string sGemstoneExteriorHeadText = "";
            string sGemstoneExteriorDescText = "";
            SSchemeCrownGemstoneExteriorConfig exteriorConfig = CrownPageConfigPage.Instance.GetCrownPageExteriorConfig(nExteriorID);
            if (exteriorConfig != null)
            {
                string sSymbols = "\n";
                sGemstoneExteriorHeadText = "颜值 :" + exteriorConfig.nFacescore;
                string sExteriorDesc = "色泽 :" + exteriorConfig.nColours + sSymbols
                + "透光 :" + exteriorConfig.nLight + sSymbols + "重量 :" + exteriorConfig.nWeight;
                sGemstoneExteriorDescText = sExteriorDesc;
            }

            m_tooltip.SetText("GemstoneExteriorDesc", sGemstoneExteriorDescText);
            m_tooltip.SetText("GemstoneExteriorHead", sGemstoneExteriorHeadText);
        }

        public void Reset()
        {
            CobGemstoneImage.gameObject.SetActive(true);
            WinGensoneImage.gameObject.SetActive(false);
            SeletedGemsotneImage.gameObject.SetActive(false);
            EnterCobModel();
        }

        public void OnToogleClick(bool bIsOn)
        {
            if (!bIsOn || m_eSelectModel == EGEMSTONESELECTMODEL.EGSM_END)
                return;

            if (m_eSelectModel == EGEMSTONESELECTMODEL.EGSM_COB)
            {
                CobGemstoneImage.gameObject.SetActive(false);
                WinGensoneImage.gameObject.SetActive(false);
                SeletedGemsotneImage.gameObject.SetActive(true);
                SetGemstoneToolTip();
                SendCobSelectMessage();
                ParentView.EntereEndModel();
            }
            else
            {
                CobGemstoneImage.gameObject.SetActive(true);
                WinGensoneImage.gameObject.SetActive(true);
                SeletedGemsotneImage.gameObject.SetActive(false);
                SetCobstoneToolTip();
                SendWinSelectMessage();
                ParentView.EnterCobModel();
            }
        }

        private void SendCobSelectMessage()
        {
            cmd_Crown_Cobbleston_Selected msgInfo = new cmd_Crown_Cobbleston_Selected();
            msgInfo.nGemstoneID = m_nGemstoneID;
            EntityEventHelper.Instance.SendCommand<cmd_Crown_Cobbleston_Selected>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_CROWN_DCOBBLESTON_SELECTED, ref msgInfo);
        }

        private void SendWinSelectMessage()
        {
            cmd_Crown_WindowStone_Selected msgInfo = new cmd_Crown_WindowStone_Selected();
            msgInfo.nGemstoneID = m_nGemstoneID;
            msgInfo.nSlotID = m_nSlotID;
            EntityEventHelper.Instance.SendCommand<cmd_Crown_WindowStone_Selected>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_CROWN_DWINDOWSTONE_SELECTED, ref msgInfo);
        }

        public void EnterWinModel()
        {
            m_eSelectModel = EGEMSTONESELECTMODEL.EGSM_WIN;
        }

        public void EnterCobModel()
        {
            m_eSelectModel = EGEMSTONESELECTMODEL.EGSM_COB;
        }

        public void EntereEndModel()
        {
            m_eSelectModel = EGEMSTONESELECTMODEL.EGSM_END;

            if (SeletedGemsotneImage.gameObject.activeSelf)
                EndFrameObj.SetActive(true);
            this.FlopToggle.interactable = false;
        }

        public int Slot
        {
            set { m_nSlotID = value; }
        }
    }

    public class GenstoneChestFlopView : UIBaseWndView,ITimerHanlder
    {
        private GenstoneChestFlopWnd m_FlopWnd;
        private GenstoneChestFlopItem[] m_GenstoneChestFlopList;
        private int m_nFlopItemMax = 9;
        public GameObject TmpFlopItem;
        public Transform FlopListTrans;
        public Text OperationStateText;
        public Text BestGemstoneTipText;
        public Text ChestDescText;
        public Button OpenWinButton;
        public Button EndButton;
        public override void Start()
        {
            UTooltipTrigger trigger = OpenWinButton.gameObject.AddComponent<UTooltipTrigger>(ETooltipStyle.ETS_Default);
            trigger.SetText(UTooltipParamName.BodyText, "矿石里面到底是什么宝石，好想提前知道");
            return;
        }

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            m_FlopWnd = wnd as GenstoneChestFlopWnd;
            return true;
        }

        internal override void SetRendererVisible(bool visible)
        {
            base.SetRendererVisible(visible);

            if (!visible&&this.IsVisible())
                m_FlopWnd.UnloadView();
        }

        private void ResetView()
        {
            if(m_GenstoneChestFlopList == null)
                return;

            foreach (GenstoneChestFlopItem item in m_GenstoneChestFlopList)
            {
                item.Reset();
            }

            OpenWinButton.gameObject.SetActive(true) ;
            EndButton.gameObject.SetActive(false);
            OperationStateText.text = "请选择原石";
        }

        public void UpdateView()
        {
            GemStoneProductData productData = LogicDataCenter.crownPageDataManager.CrownGemstoneProductInfo;
            DoInitList();
            ResetView();
            DoLoadListData(productData.data);
        }

        private void DoLoadListData(cmd_Crown_Gemstone_Product_Info data)
        {
            if (m_GenstoneChestFlopList == null)
                return;

            if(data.cobblestonGemstoneIDList.Length!=data.windowGemstoneIDList.Length)
                return;

            int nGradeMax = 0;
            for (int i = 0; i < data.cobblestonGemstoneIDList.Length;++i )
            {
                bool bWin = false;
                int nCobGemstoneID = data.cobblestonGemstoneIDList[i];
                if(nCobGemstoneID <=0 )
                    continue;

                SSchemeCrownGemstoneConfig config = CrownPageConfigPage.Instance.GetCrownPageGemstoneConfig(nCobGemstoneID);
                if (config == null)
                    continue;

                nGradeMax = config.nGradeUpper > nGradeMax ? config.nGradeUpper : nGradeMax;
                for (int j = 0; j < data.windowGemstoneIDList.Length;++j )
                {
                    if(data.windowGemstoneIDList[i] ==nCobGemstoneID)
                    {
                        bWin = true;
                        break;
                    }
                }

                m_GenstoneChestFlopList[i].UpdateItem(nCobGemstoneID, bWin, data.bOpenWinEnd > 0, data.cobStoneIDList[i], config);
            }

            OperationStateText.text = data.bOpenWinEnd > 0 ? "点击抽取其中一颗宝石" : "请选择开窗宝石";

            if (nGradeMax >= 0 && nGradeMax < UDefines.sGemstoneGradeDesc.Length)
                BestGemstoneTipText.text = "最高品级 : " + UDefines.sGemstoneGradeDesc[nGradeMax];

            int nOpenChestID = LogicDataCenter.prizeChestDataManager.OpenChestID;
            SSchemePrizeChestConfig chestConfig = ActorPrizeConfig.Instance.GetPrizeChestConfig(nOpenChestID);
            if (chestConfig != null)
            {
                ChestDescText.text = chestConfig.strChestName;
            }
        }

        public void DoInitList()
        {
            if (m_GenstoneChestFlopList != null)
                return;

            m_GenstoneChestFlopList = new GenstoneChestFlopItem[m_nFlopItemMax];
            TmpFlopItem.SetActive(false);
            for (int i = 0; i < m_nFlopItemMax; ++i)
            {
                GameObject GenstoneChestObject = ResNode.InstantiateRes(TmpFlopItem);
                if (GenstoneChestObject == null)
                {
                    return;
                }

                GenstoneChestFlopItem item = GenstoneChestObject.GetComponent<GenstoneChestFlopItem>();
                if (item == null)
                {
                    return;
                }

                item.Slot = i;
                GenstoneChestObject.SetActive(true);
                m_GenstoneChestFlopList[i] = item;
                GenstoneChestObject.transform.SetParent(FlopListTrans, false);
            }
        }

        public void OnOpenWinButtonClick()
        {
            OnMsgBoxOpenWinInfo();
        }

        public void EndView()
        {
            if (m_GenstoneChestFlopList == null)
                return;

            foreach (GenstoneChestFlopItem item in m_GenstoneChestFlopList)
            {
                item.EntereEndModel();
            }

            OperationStateText.text = "宝石选取结束";
        }

        public void EnterCobModel()
        {
            if (m_GenstoneChestFlopList == null)
                return;

            foreach (GenstoneChestFlopItem item in m_GenstoneChestFlopList)
            {
                item.EnterCobModel();
            }

            OperationStateText.text = "请选择原石";
        }

        public void EntereEndModel()
        {
            if (m_GenstoneChestFlopList == null)
                return;

            TimerManager.SetTimer(this, 0, 2.0f, 1);
            foreach (GenstoneChestFlopItem item in m_GenstoneChestFlopList)
            {
                item.EntereEndModel();
            }

            OpenWinButton.gameObject.SetActive(false);
            EndButton.gameObject.SetActive(true);
            OperationStateText.text = "宝石选取结束";
        }

        public void OnTimer(int nTimerID)
        {
            TimerManager.KillTimer(this, 0);
            foreach (GenstoneChestFlopItem item in m_GenstoneChestFlopList)
            {
                if(item&&item.SeletedGemsotneImage)
                {
                    item.SeletedGemsotneImage.gameObject.SetActive(true);
                    item.SetGemstoneToolTip();
                }
            }
        }

        public void OnViewClose()
        {
            if (m_FlopWnd == null)
                return;

            m_FlopWnd.SetVisible(false);
        }

        public void OnMsgBoxOpenWinInfo()
        {
            {

                int nOpenChestID = LogicDataCenter.prizeChestDataManager.OpenChestID;
                int nCostMoney = 0;
                SSchemePrizeChestConfig chestConfig = ActorPrizeConfig.Instance.GetPrizeChestConfig(nOpenChestID);
                if (chestConfig != null)
                {
                    GemStoneProductData productData = LogicDataCenter.crownPageDataManager.CrownGemstoneProductInfo;
                    SSchemeCrownGemstoneRandomConfig randConfig = CrownPageConfigPage.Instance.GetCrownPageGemsRandomConfig(chestConfig.nPrizeGroupID);
                    if (randConfig != null && productData != null)
                    {
                        int nGemstoneGroupID = chestConfig.nPrizeGroupID;
                        int nBaseTicket = randConfig.nOpenWinCost; ;
                        int nAdditionTicket = randConfig.nAdditionWinCost;
                        nCostMoney = nBaseTicket + GetListEffectiveCount(productData.data.windowGemstoneIDList) * nAdditionTicket;
                    }
                }

                UIDialogData data = new UIDialogData(title: "透视宝石",
                                            message: "消耗"+nCostMoney+"点券，获得一次透视矿石机会",
                                            buttons: new UIWidgets.DialogActions() { { "确认", OnMsgBoxOpenWinHandelSave },
                                               { "取消", OnMsgBoxOpenWinHandelCancel}
                                            },
                                            closeButtonAction: new UnityEngine.Events.UnityAction(OnMsgBoxOpenWinHandelCancelEx)

                                           );
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
            }
        }

        private int GetListEffectiveCount(int[] list)
        {
            int nCount = 0;
            for (int i = 0; i < list.Length;++i )
            {
                if (list[i] > 0)
                    nCount++;
            }

            return nCount;
        }

        private bool OnMsgBoxOpenWinHandelSave()
        {
            IntPtr tmpPtr = IntPtr.Zero;
            EntityEventHelper.Instance.SendCommand<IntPtr>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_CROWN_DWINDOWSTON, ref tmpPtr);
            return true;
        }


        private bool OnMsgBoxOpenWinHandelCancel()
        {
            return true;
        }

        private void OnMsgBoxOpenWinHandelCancelEx()
        {
            return;
        }
    }
}
