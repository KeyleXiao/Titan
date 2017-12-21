using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using Kin_ManagerDef;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using DataCenter;
using USpeedUI.UWidgets;
using ASpeedGame.Data.KinConfig;
using U3D_Render.Common;
using UIWidgets;
using ASpeedGame.Data.ShieldWorld;
using ASpeedGame.Data.LegendCupConfig;

namespace USpeedUI.Social
{

    public class KinMemberWnd : UIBaseSubWnd<KinMemberWndView>
    {

        // 获取ID
        public override int GetWndType()
        {
            return (int)SocialSubWndType.WND_KINMEMBER;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewKinMember";
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            SetVisible(false);
            return true;
        }

        // 设置是否可见
        public override void SetVisible(bool visible)
        {
            if (visible)
                OnOpenKinMemberWnd();

            base.SetVisible(visible);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData data = msgData as UIMsgCmdData;
            switch (msgID)
            {

                case WndMsgID.WND_MSG_KIN_RECV_EVENTLIST:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateKinEventList();
                    }
                    break;

                case WndMsgID.WND_MSG_KIN_UPDATEMEMBER:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateKinMember();
                    }
                    break;

                case WndMsgID.WND_MSG_KIN_RECEV_MEMBERLIST:
                    {
                        if (m_wndView != null)
                            m_wndView.OnRecvKinMemberList();
                    }
                    break;

                case WndMsgID.WND_MSG_KIN_RECEV_KINBASEDATA:
                case WndMsgID.WND_MSG_KIN_NUM_PROP_UPDATE:
                case WndMsgID.WND_MSG_KIN_STRING_PROP_UPDATE:
                    {
                        // 战队基础数据
                        if (m_wndView != null)
                            m_wndView.OnRecvKinBaseData();
                    }
                    break;

                case WndMsgID.WND_MSG_KIN_UPDATEAPPLY_TIPS:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateApllyTip();
                    }
                    break;

                case WndMsgID.WND_MSG_KIN_RECV_GLORYDATA:
                    {
                        if (m_wndView != null)
                            m_wndView.OnRecevKinGloryData();
                    }
                    break;

                default:
                    break;
            }
        }

        public void OnOpenKinMemberWnd()
        {
            // 打开我的战队
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_KIN_OPEN);
        }
    }

    public class KinLabelCupIconGroup : MonoBehaviour
    {
        public Image[] CupIcon;
        public Text NumIcon;

        public void SetData(int nNum)
        {
            int nLen = CupIcon.Length;
            if (nNum <= nLen)
            {
                for (int i = 0; i < nLen; i++)
                {
                    if (i < nNum)
                        CupIcon[i].gameObject.SetActive(true);
                    else
                        CupIcon[i].gameObject.SetActive(false);
                }

                NumIcon.gameObject.SetActive(false);
            }
            else
            {
                CupIcon[0].gameObject.SetActive(true);
                for (int i = 1; i < nLen; i++)
                    CupIcon[i].gameObject.SetActive(false);

                NumIcon.text = nNum.ToString();
                NumIcon.gameObject.SetActive(true);
             }
        }
    }

    // 战队按钮标签类型
    public enum KinMemberFrameButtonType
    {
        BTN_KINMEMBER,
        BTN_KINEVENT,
        BTN_KINDES,
        BTN_KINGLORY,
    }

    public class KinMemberWndView : UIBaseSubWndView
    {
        private Dictionary<int, GameObject> m_dicSubFreamWnd = new Dictionary<int, GameObject>();
        private KinMemberFrameButtonType m_curShowPageType;
        public UButtonGroup buttonGroup;
        public GameObject FrameBaseData;
        public URichText ViewKinListRT;

        // KinMemberWnd
        private KinMemberWnd MemberWnd { get { return m_wnd as KinMemberWnd; } }
        public UListKinMember ListKinMember;
        private UListItemKinMember CurSelectItem;
        public URedPointTip ApplyTip;

        public Text KinDissmissBtnDes;
        public Text KinQuitBtnDes;
        public Text KinSettingBtnDes;
        public Text AcceptMemberBtnDes;
        public Text KickMemberBtnDes;

        public Text IdentityIcon;
        public Text HeadLevel;
        public Text HeadPersonName;
        public Text HeadPower;
        public Text HeadPowerRank;
        public Text HeadGradIcon;
        public Text HeadState;

        private bool isIdentityIconDescending = false;
        private bool isLevelDescending = false;
        private bool isPersonNameDescending = false;
        private bool isPowerDescending = false;
        private bool isPowerRankDescending = false;
        private bool isGradIconDescending = false;
        private bool isStateDescending = false;

        public Text KinInfoSubTitle;
        public Text KinNameDes;
        public Text KinShaikhNameDes;
        public Text KinLevelDes;
        public Text TotalMemberInfoDes;
        public Text TotalFightDes;
        public Text TotalOrderDes;
        public Text KinName;
        public Text KinShaikhName;
        public Text KinLevel;
        public Text TotalMemberInfo;
        public Text TotalFight;
        public Text TotalOrder;
        public Text WeekActivityDes;
        public Text WeekActivityValue;

        public Text KinLabelSubTitle;
        public Text KinLabelDes;
        public Text MatchResultDes;
        public Text GoldAwardDes;
        public Text SilverAwardDes;
        public Text KinLabel;
        public Text MatchResult;
        public KinLabelCupIconGroup GoldAward;
        public KinLabelCupIconGroup SilverAward;

        public Text KinNoticeSubTitle;
        public Text KinNoticeDes;
        public InputField NoticeTI;
        
        public Transform OffLineTog;
        public Text OffLineTogDes;

        private UPopupAction PopupAction;

        // KinEventWnd
        public UListKinEvent ListKinEvent;

        // KinDesWnd
        public Text KinDetailDes;

        // KinGloryWnd
        public Text GloryMatchRecordBtnDes;
        public Text GloryCupSubTitleDes;

        public Text GloryKinName;
        public Text GloryTotalDes;
        public Text GloryTotalNum;
        public Text GloryWinDes;
        public Text GloryWinNum;
        public Text GloryWinPro;
        public Slider GloryWinSlider;

        public Text GloryGodDes;
        public Text Glory5KillDes;
        public Text Glory4KillDes;
        public Text Glory3KillDes;
        public Text GloryDeathDes;
        public Text GloryKillDes;
        public Text GloryAssistDes;
        public Text GloryGodNum;
        public Text Glory5KillNum;
        public Text Glory4KillNum;
        public Text Glory3KillNum;
        public Text GloryDeathNum;
        public Text GloryKillNum;
        public Text GloryAssistNum;

        public UListKinGloryCup ListKinGloryCup;

        // 子窗体
        public GameObject FreamKinMember;
        public GameObject FreamKinEvent;
        public GameObject FreamKinDes;
        public GameObject FreamKinGlory;

        // Button
        public Button AcceptMemberBtn;
        public Button KickMemberBtn;
        public Button KinSettingBtn;
        public Button KinDissmissBtn;
        public Button NoticeEditBtn;
        public Button NoticeSaveBtn;
        public Button MatchRecordBtn;
        public Button KinQuitBtn;

        public override bool Init(IUISubWnd wnd)
        {
            KinDissmissBtnDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinDissmissBtnDes");
            KinQuitBtnDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinQuitBtnDes");
            KinSettingBtnDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinSettingBtnDes");
            AcceptMemberBtnDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "AcceptMemberBtnDes");
            KinInfoSubTitle.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinInfoDes");
            KinShaikhNameDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinShaikhNameDes");
            KinLevelDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinLevelDes");
            KinNameDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinNameDes");
            TotalMemberInfoDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "TotalMemberInfoDes");
            //SysDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "SysDes");
            KinNoticeDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinNoticeDes");
            KickMemberBtnDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KickMemberBtnDes");
            IdentityIcon.text = ULocalizationService.Instance.Get("UIView", "KinMember", "IdentityIcon");
            HeadLevel.text = ULocalizationService.Instance.Get("UIView", "KinMember", "HeadLevel"); ;
            HeadPersonName.text = ULocalizationService.Instance.Get("UIView", "KinMember", "HeadPersonName");
            HeadPower.text = ULocalizationService.Instance.Get("UIView", "KinMember", "HeadPower");
            HeadPowerRank.text = ULocalizationService.Instance.Get("UIView", "KinMember", "HeadPowerRank");
            HeadGradIcon.text = ULocalizationService.Instance.Get("UIView", "KinMember", "HeadGradIcon");
            HeadState.text = ULocalizationService.Instance.Get("UIView", "KinMember", "HeadState");
            OffLineTogDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "OffLineTogDes");
            TotalFightDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "TotalFightDes");
            TotalOrderDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "TotalOrderDes");
            WeekActivityDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "WeekActivityDes");
            WeekActivityDes.GetComponentInParent<UTooltipTrigger>().SetText(UTooltipParamName.BodyText, ULocalizationService.Instance.Get("UIView", "KinMember", "WeekActivityDesTooltip"));
            KinLabelSubTitle.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinLabelSubTitle");
            KinLabelDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinLabelDes");
            MatchResultDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "MatchResultDes");
            GoldAwardDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "GoldAwardDes");
            SilverAwardDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "SilverAwardDes");
            KinNoticeSubTitle.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinNoticeSubTitle");

            GloryMatchRecordBtnDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "GloryMatchRecordBtnDes");
            GloryCupSubTitleDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "GloryCupSubTitleDes");
            GloryWinDes.text = String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "KinMember", "GloryWinDes"));
            GloryTotalDes.text = String.Format("{0}:", ULocalizationService.Instance.Get("UIView", "KinMember", "GloryTotalDes"));
            GloryGodDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "GloryGodDes");
            Glory5KillDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "Glory5KillDes");
            Glory4KillDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "Glory4KillDes");
            Glory3KillDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "Glory3KillDes");
            GloryDeathDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "GloryDeathDes");
            GloryKillDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "GloryKillDes");
            GloryAssistDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "GloryAssistDes");

            string sKinDetailDes = KinConfig.Instance.tKinConfig.sKinDetailDes;
            if (!sKinDetailDes.Equals(string.Empty))
            {
                UBB.toHtml(ref sKinDetailDes, UBB_FORMAT_TYPE.UGUI);
                KinDetailDes.text = sKinDetailDes;
            }

            Toggle tOffLineTog = OffLineTog.GetComponent<Toggle>();
            tOffLineTog.isOn = true;

            m_dicSubFreamWnd.Add((int)KinMemberFrameButtonType.BTN_KINMEMBER, FreamKinMember);
            m_dicSubFreamWnd.Add((int)KinMemberFrameButtonType.BTN_KINEVENT, FreamKinEvent);
            m_dicSubFreamWnd.Add((int)KinMemberFrameButtonType.BTN_KINDES, FreamKinDes);
            m_dicSubFreamWnd.Add((int)KinMemberFrameButtonType.BTN_KINGLORY, FreamKinGlory);
            m_curShowPageType = KinMemberFrameButtonType.BTN_KINMEMBER;

            ViewKinListRT.text = "<a href=ViewKinListRT color=ffca27>" + ULocalizationService.Instance.Get("UIView", "KinMember", "KinListDesc") + "</a>";
            ViewKinListRT.onHrefClick.AddListener(OnClickViewKinList);

            return base.Init(wnd);
        }

        public void OnClickViewKinList(string eventName)
        {
            if (eventName == "ViewKinListRT")
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_KIN_SHOWKINLIST, null);
            }
        }

        public void AppointShaikhCallBack()
        {
            OnKinAppointShaikhBtnClick();
        }

        public void KickoutKinCallBack()
        {
            OnKinKickBtnClick();
        }

        public void LeaveKinCallBack()
        {
            OnKinQuitBtnClick();
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            //更新界面显示
            if (visible)
            {
                m_curShowPageType = KinMemberFrameButtonType.BTN_KINMEMBER;
                SetButtonSelect((int)m_curShowPageType);
                OnUpdateWndView();
            }
            
        }

        public void OnUpdateApllyTip()
        {
            int nTipCount = LogicDataCenter.kinDataManager.KinBaseData.nApplyCount;
            ApplyTip.SetTipCount(nTipCount);
        }

        public void OnRecvKinMemberList()
        {
            CurSelectItem = null;
            ListKinMember.DataSource.Clear();
            ListKinMember.OnSelect.RemoveListener(onSelectKin);
            ListKinMember.onRightClick.RemoveListener(onSelectKin);

            cmd_kin_basedata cmdHead = LogicDataCenter.kinDataManager.KinBaseData;
            List<cmd_kin_membernode> listData = LogicDataCenter.kinDataManager.KinMemberList;
            UIWidgets.ObservableList<UListItemKinMember> dataSource = new UIWidgets.ObservableList<UListItemKinMember>();
            ListKinMember.DataSource.Clear();
            for (int i = 0; i < cmdHead.nMemberCount; ++i)
            {
                GameLogic.cmd_kin_membernode cmdNodeData = listData[i];
                UListItemKinMember item = new UListItemKinMember();
                Toggle tOffLineTog = OffLineTog.GetComponent<Toggle>();
                if (!tOffLineTog.isOn && cmdNodeData.bIsOnline == 0)
                {
                    // 隐藏不在线玩家
                    continue;
                }
                item.IdentityID     = cmdNodeData.nIdentity;
                item.PlayerID       = cmdNodeData.dwPDBID;
                item.GradIcon      += cmdNodeData.szGradeName;
                item.PersonIcon     = cmdNodeData.nFaceID;
                item.PersonName    += cmdNodeData.szName;
                item.Power         += cmdNodeData.dwBattleVal;
                item.PowerRank     += cmdNodeData.dwBVOrder;
                item.GameState = cmdNodeData.byGameState;
                item.Level         += cmdNodeData.nLevel;
                item.isOnline       = cmdNodeData.bIsOnline;
                item.nSex           = cmdNodeData.nSex;
                item.nLastOrder     = cmdNodeData.dwLastOrder;
                item.nBVOrder       = cmdNodeData.dwBVOrder;
                if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == item.PlayerID)
                {
                    PopupAction = new UPopupAction();
                    PopupAction.Add(ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinLeave"), new UListComponentItemDataPopup(_actionSelected: LeaveKinCallBack));
                }
                else if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == cmdHead.dwShaikhID)
                {
                    PopupAction = new UPopupAction();
                    PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamInvite.GetDescString(), new UPopupItemDataTeamInvite(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamRequest.GetDescString(), new UPopupItemDataTeamRequest(item.PlayerID));
                    PopupAction.Add(ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinAppointShaikh"), new UListComponentItemDataPopup(_actionSelected: AppointShaikhCallBack));
                    PopupAction.Add(ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinKickout"), new UListComponentItemDataPopup(_actionSelected: KickoutKinCallBack));
                    if(item.isOnline > 0 && GameLogicAPI.isMainMap((int)cmdNodeData.dwSceneID))
                        PopupAction.Add(UPopupItemDataTransmitTo.GetDescString(), new UPopupItemDataTransmitTo((int)cmdNodeData.dwWorldID, (int)cmdNodeData.serverID, (int)cmdNodeData.dwSceneID));
                }
                else
                {
                    PopupAction = new UPopupAction();
                    PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamInvite.GetDescString(), new UPopupItemDataTeamInvite(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamRequest.GetDescString(), new UPopupItemDataTeamRequest(item.PlayerID));
                    if(item.isOnline > 0 && GameLogicAPI.isMainMap((int)cmdNodeData.dwSceneID))
                        PopupAction.Add(UPopupItemDataTransmitTo.GetDescString(), new UPopupItemDataTransmitTo((int)cmdNodeData.dwWorldID, (int)cmdNodeData.serverID, (int)cmdNodeData.dwSceneID));
                }
                item.PopupAction = PopupAction;
                dataSource.Add(item);
            }

            // 排序
            IEnumerable<UListItemKinMember> ListQuery = from item in dataSource
                                                        orderby item.isOnline descending, item.IdentityID descending, item.Power descending
                                                        select item;

            UIWidgets.ObservableList<UListItemKinMember> tDataSource = new UIWidgets.ObservableList<UListItemKinMember>();
            foreach (UListItemKinMember Item in ListQuery)
            {
                tDataSource.Add(Item);
            }

            ListKinMember.DataSource = tDataSource;

            ListKinMember.OnSelect.AddListener(onSelectKin);
            ListKinMember.onRightClick.AddListener(onSelectKin);
        }
        

        public void OnUpdateWndView()
        {
            // 按钮控制显示
            //TKinPopedom.emKinPopedom_AppointShaikh;   // 任命族长
            //TKinPopedom.emKinPopedom_AppointElder;   // 任命副族长

            //TKinPopedom.emKinPopedom_AcceptMember;  // 招收成员
            //TKinPopedom.emKinPopedom_KickMember; // 开除成员(不包括族长和副族长)
            //TKinPopedom.emKinPopedom_Fight; // 发起战队站
            //TKinPopedom.emKinPopedom_ModifyNotice;// 修改公告
            //TKinPopedom.emKinPopedom_ModifyManifesto;// 修改宣言
            //TKinPopedom.emKinPopedom_ResearchSkill;// 研究技能
            //TKinPopedom.emKinPopedom_ModifyRemark;// 修改成员备注

            //TKinPopedom.emKinPopedom_ShaikhPopedom;// 族长权限
            //TKinPopedom.emKinPopedom_ElderPopedom;// 副族长权限

            KinMemberWnd Wnd = m_wnd as KinMemberWnd;
            int dwPopedomFlag = LogicDataCenter.kinDataManager.SelfMemberInfo.dwPopedomFlag;
            if ((dwPopedomFlag & (int)TKinPopedom.emKinPopedom_AcceptMember) == (int)TKinPopedom.emKinPopedom_AcceptMember)
            {
                AcceptMemberBtn.gameObject.SetActive(true);
            }
            else
            {
                AcceptMemberBtn.gameObject.SetActive(false);
            }

            if ((dwPopedomFlag & (int)TKinPopedom.emKinPopedom_KickMember) == (int)TKinPopedom.emKinPopedom_KickMember)
            {
                KickMemberBtn.gameObject.SetActive(true);
            }
            else
            {
                KickMemberBtn.gameObject.SetActive(false);
            }

            if ((dwPopedomFlag & (int)TKinPopedom.emKinPopedom_AppointElder) == (int)TKinPopedom.emKinPopedom_AppointElder)
            {
                KinSettingBtn.gameObject.SetActive(false);
            }
            else
            {
                KinSettingBtn.gameObject.SetActive(false);
            }

            if ((dwPopedomFlag & (int)TKinPopedom.emKinPopedom_AppointShaikh) == (int)TKinPopedom.emKinPopedom_AppointShaikh)
            {
                KinDissmissBtn.gameObject.SetActive(true);
            }
            else
            {
                KinDissmissBtn.gameObject.SetActive(false);
            }

            if ((dwPopedomFlag & (int)TKinPopedom.emKinPopedom_ModifyNotice) == (int)TKinPopedom.emKinPopedom_ModifyNotice)
            {
                //关闭编辑状态
                NoticeTI.enabled = false;

                // 显示编辑公告按钮
                NoticeEditBtn.gameObject.SetActive(true);
                
                // 隐藏编辑按钮
                NoticeSaveBtn.gameObject.SetActive(false);
            }
            else
            {
                //关闭编辑状态
                NoticeTI.enabled = false;
                // 隐藏编辑公告按钮
                NoticeEditBtn.gameObject.SetActive(false);

                // 隐藏编辑按钮
                NoticeSaveBtn.gameObject.SetActive(false);
            }
        }

        public void onSelectKin(int index, UIWidgets.ListViewItem item)
        {
            CurSelectItem = ListKinMember.DataSource[index];
        }

        public void OnKinDissmissBtnClick()
        {
            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                    title: ULocalizationService.Instance.Get("UIView", "KinMember", "DissmissTitle"),
                                    message: ULocalizationService.Instance.Get("UIView", "KinMember", "DissmissContent"),
                                    buttons: new UIWidgets.DialogActions() { 
                                    { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), ConfigBtnDismissClick },
                                    { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), null }}
                                    );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
            
        }
        private bool ConfigBtnDismissClick()
        {
            // 解散战队
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_KIN_DISMISS);
            return true;
        }

        public void OnKinQuitBtnClick()
        {
            
            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                    title: ULocalizationService.Instance.Get("UIView", "KinMember", "QuitTitle"),
                                    message: ULocalizationService.Instance.Get("UIView", "KinMember", "QuitContent"),
                                    buttons: new UIWidgets.DialogActions() { 
                                    { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), ConfigBtnQuitClick },
                                    { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), null }}
                                    );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);

           
        }

        private bool ConfigBtnQuitClick()
        {
            // 退出战队
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_KIN_QUIT);
            return true;
        }
         
        public void OnAcceptMemberBtnClick()
        {
            // 招收成员界面显示
            UISystem.Instance.ShowWnd(WndID.WND_ID_KIN_ACCEPTMEMBER, true);
            //请求战队申请人员列表
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_KIN_APPLY_LIST);
            //请求战队自动收人设置
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_KIN_AUTOACCEPT_REQUEST);
        }

        public void OnKinSettingBtnClick()
        {
            // 战队设置
        }

        public void OnUpdateKinMember()
        {
            if (EntityFactory.MainHeroView == null)
            {
                return;
            }

            CurSelectItem = null;

            ListKinMember.DataSource.BeginUpdate();
            ListKinMember.DataSource.Clear();
            List<cmd_kin_membernode> KinMemberList = LogicDataCenter.kinDataManager.KinMemberList;
            for (int i = 0; i < KinMemberList.Count; ++i)
            {
                cmd_kin_membernode cmdNodeData = KinMemberList[i];
                UListItemKinMember item = new UListItemKinMember();
                Toggle tOffLineTog = OffLineTog.GetComponent<Toggle>();
                if (!tOffLineTog.isOn && cmdNodeData.bIsOnline == 0)
                {
                    // 隐藏不在线玩家
                    continue;
                }
                item.IdentityID     = cmdNodeData.nIdentity;
                item.PlayerID       = cmdNodeData.dwPDBID;
                item.GradIcon      += cmdNodeData.szGradeName;
                item.PersonIcon     = cmdNodeData.nFaceID;
                item.PersonName    += cmdNodeData.szName;
                item.Power         = cmdNodeData.dwBattleVal;
                item.PowerRank     += cmdNodeData.dwBVOrder;
                item.GameState = cmdNodeData.byGameState;
                item.Level         += cmdNodeData.nLevel;
                item.isOnline       = cmdNodeData.bIsOnline;
                item.nSex           = cmdNodeData.nSex;
                item.nLastOrder     = cmdNodeData.dwLastOrder;
                item.nBVOrder       = cmdNodeData.dwBVOrder;
                item.bIsSelf = (LogicDataCenter.kinDataManager.SelfMemberInfo.dwPDBID == cmdNodeData.dwPDBID) && LogicDataCenter.kinDataManager.SelfMemberInfo.dwPDBID  > 0 ? true : false;
                if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == item.PlayerID)
                {
                    PopupAction = new UPopupAction();
                    PopupAction.Add(ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinLeave"), new UListComponentItemDataPopup(_actionSelected: LeaveKinCallBack));
                }
                else if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == LogicDataCenter.kinDataManager.KinBaseData.dwShaikhID)
                {
                    PopupAction = new UPopupAction();
                    PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamInvite.GetDescString(), new UPopupItemDataTeamInvite(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamRequest.GetDescString(), new UPopupItemDataTeamRequest(item.PlayerID));
                    PopupAction.Add(ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinAppointShaikh"), new UListComponentItemDataPopup(_actionSelected: AppointShaikhCallBack));
                    PopupAction.Add(ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinKickout"), new UListComponentItemDataPopup(_actionSelected: KickoutKinCallBack));
                }
                else
                {
                    PopupAction = new UPopupAction();
                    PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamInvite.GetDescString(), new UPopupItemDataTeamInvite(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamRequest.GetDescString(), new UPopupItemDataTeamRequest(item.PlayerID));
                }
                item.PopupAction = PopupAction;

                ListKinMember.DataSource.Add(item);
            }

            // 排序
            IEnumerable<UListItemKinMember> ListQuery = from item in ListKinMember.DataSource
                                                        orderby item.isOnline descending, item.IdentityID descending, item.Power descending
                                                        select item;

            UIWidgets.ObservableList<UListItemKinMember> dataSource = new UIWidgets.ObservableList<UListItemKinMember>();
            foreach (UListItemKinMember Item in ListQuery)
            {
                dataSource.Add(Item);
            }

            ListKinMember.DataSource = dataSource;
            ListKinMember.DataSource.EndUpdate();

            OnRecvKinBaseData();

        }

        public void OnUpdateKinEventList()
        {
            ListKinEvent.DataSource.BeginUpdate();
            ListKinEvent.DataSource.Clear();
            List<cmd_kin_eventlist> EventList = LogicDataCenter.kinDataManager.KinEventList;
            for (int i = 0; i < EventList.Count; ++i)
            {
                cmd_kin_eventlist cmdNodeData = EventList[i];
                UListItemKinEvent item = new UListItemKinEvent();
                Trace.Log("cmdNodeData.nYear =" + cmdNodeData.nYear + "cmdNodeData.nMonth =" + cmdNodeData.nMonth);
                item.KinEventDes = cmdNodeData.nYear + "/" + cmdNodeData.nMonth + "/" + cmdNodeData.nDay + "/" + cmdNodeData.nHour.ToString().PadLeft(2, '0') +":" + cmdNodeData.nMinute.ToString().PadLeft(2, '0')
                                    + cmdNodeData.szCoutext;
                ListKinEvent.DataSource.Add(item);
            }

            ListKinEvent.DataSource.EndUpdate();
        }

        public void SetButtonSelect(int nIndex)
        {
            buttonGroup.SetItemSelectedItem(nIndex, true);
        }

        public void OnGroupKinMemberBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            SwitchWndPage(KinMemberFrameButtonType.BTN_KINMEMBER);
        }

        public void OnGroupKinEventBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            SwitchWndPage(KinMemberFrameButtonType.BTN_KINEVENT);

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_REQUEST_KIN_EVENTLIST);
        }

        public void OnGroupKinDesBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            SwitchWndPage(KinMemberFrameButtonType.BTN_KINDES);
        }

        public void OnGroupKinGloryBtnClick(bool bActive)
        {
            if (!bActive)
                return;

            SwitchWndPage(KinMemberFrameButtonType.BTN_KINGLORY);

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_KIN_REQUESTKINGLORY);
        }

        public void SwitchWndPage(KinMemberFrameButtonType pageType)
        {
            if (EntityFactory.MainHeroView == null)
                return;

            FrameBaseData.SetActive(true);
            MatchRecordBtn.gameObject.SetActive(false);
            AcceptMemberBtn.gameObject.SetActive(false);
            KickMemberBtn.gameObject.SetActive(false);
            KinSettingBtn.gameObject.SetActive(false);
            KinDissmissBtn.gameObject.SetActive(false);
            KinQuitBtn.gameObject.SetActive(false);
            switch (pageType)
            {
                case KinMemberFrameButtonType.BTN_KINMEMBER:
                    {
                        ShowSubWnd(KinMemberFrameButtonType.BTN_KINMEMBER);
                        RefreshKinMemberBtn();
                    }
                    break;
                case KinMemberFrameButtonType.BTN_KINEVENT:
                    {
                        ShowSubWnd(KinMemberFrameButtonType.BTN_KINEVENT);
                    }
                    break;
                case KinMemberFrameButtonType.BTN_KINDES:
                    {
                        ShowSubWnd(KinMemberFrameButtonType.BTN_KINDES);
                    }
                    break;
                case KinMemberFrameButtonType.BTN_KINGLORY:
                    {
                        ShowSubWnd(KinMemberFrameButtonType.BTN_KINGLORY);
                        FrameBaseData.SetActive(false);
                        MatchRecordBtn.gameObject.SetActive(true);
                    }
                    break;
                default:
                    return;
            }

            m_curShowPageType = pageType;
        }

        private void ShowSubWnd(KinMemberFrameButtonType wndType)
        {
            foreach (KeyValuePair<int, GameObject> kv in m_dicSubFreamWnd)
            {
                if (kv.Key == (int)wndType)
                {
                    // 显示
                    kv.Value.gameObject.SetActive(true);
                }
                else
                {
                    // 隐藏
                    kv.Value.gameObject.SetActive(false);
                }
                
            }
        }

        public void OnKinKickBtnClick()
        {
            if (CurSelectItem == null)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_KIN_KICK_NOT_SELECT);
                return;
            }

            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                    title: ULocalizationService.Instance.Get("UIView", "KinMember", "KickTitle"),
                                    message: String.Format(ULocalizationService.Instance.Get("UIView", "KinMember", "KickContent"), CurSelectItem.PersonName),
                                    buttons: new UIWidgets.DialogActions() { 
                                    { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), ConfigBtnKickClick },
                                    { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), null }}
                                    );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        private bool ConfigBtnKickClick()
        {
            // 踢出战队
            if (CurSelectItem == null)
            {
                return false;
            }

            GameLogic.cmd_kin_kick cmdData = new GameLogic.cmd_kin_kick();
            cmdData.nMemberPDBID = CurSelectItem.PlayerID;
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_KIN_KICK, ref cmdData);
            CurSelectItem = null;
            return true;
        }

        public void OnKinAppointShaikhBtnClick()
        {

            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                    title: ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinAppointShaikh"),
                                    message: ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinAppointContent"),
                                    buttons: new UIWidgets.DialogActions() { 
                                    { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), AppointShaikhClick },
                                    { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), null }}
                                    );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);

            
        }

        private bool AppointShaikhClick()
        {
            // 踢出战队
            if (CurSelectItem == null)
            {
                return false;
            }

            cmd_kin_appoint_identity data = new cmd_kin_appoint_identity();
            data.dwPDBID = CurSelectItem.PlayerID;
            data.nIdentity = (int)TKinIdentity.emKinIdentity_Shaikh;

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_KIN_APPOINT_IDENTITY, ref data);

            return true;
        }
   
        public void OnRecvKinUpdateHeroMember(IntPtr ptr, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            GameLogic.cmd_kin_self_hero_member cmdHead = helper.get<GameLogic.cmd_kin_self_hero_member>();
            
        }

        public void OnRecvKinBaseData()
        {
            GameLogic.cmd_kin_basedata cmdHead = LogicDataCenter.kinDataManager.KinBaseData;
            // 设置战队基本数据
            KinName.text = cmdHead.szKinName;
            KinShaikhName.text = cmdHead.szShaikhName;
            KinLevel.text = cmdHead.nKinLevel.ToString();
            TotalMemberInfo.text = cmdHead.nOnlineCount + " / " + cmdHead.nMemberCount + "( " + ULocalizationService.Instance.Get("UIView", "KinMember", "MaxCountDes") + cmdHead.nKinMaxMember + ")";
            TotalFight.text = cmdHead.nTotalFight.ToString();
            TotalOrder.text = cmdHead.nFightOrder.ToString();
            WeekActivityValue.text = cmdHead.nWeekActivity.ToString();
            KinLabel.text = cmdHead.szKinLabel;
            MatchResult.text = cmdHead.nMatchWinNum + " / " + cmdHead.nMatchTotalNum;
            GoldAward.SetData(cmdHead.nGoldNum);
            SilverAward.SetData(cmdHead.nSilverNum);

            NoticeTI.placeholder.GetComponent<Text>().text = cmdHead.szNotice;

            // 刷新申请人数按钮
            OnUpdateApllyTip();

            KinShaikhName.text = cmdHead.szShaikhName;
            if (m_curShowPageType == KinMemberFrameButtonType.BTN_KINMEMBER)
            {
                RefreshKinMemberBtn();
            }
        }

        private void RefreshKinMemberBtn()
        {
            int dwPopedomFlag = LogicDataCenter.kinDataManager.SelfMemberInfo.dwPopedomFlag;
            bool bAcceptMember = (dwPopedomFlag & (int)TKinPopedom.emKinPopedom_AcceptMember) == (int)TKinPopedom.emKinPopedom_AcceptMember;
            bool bKickMember = (dwPopedomFlag & (int)TKinPopedom.emKinPopedom_KickMember) == (int)TKinPopedom.emKinPopedom_KickMember;
            int nSelfActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            bool bIsShaikh = LogicDataCenter.kinDataManager.KinBaseData.dwShaikhID == nSelfActorID;

            AcceptMemberBtn.gameObject.SetActive(bAcceptMember);
            KickMemberBtn.gameObject.SetActive(bKickMember);
            KinSettingBtn.gameObject.SetActive(false);
            KinDissmissBtn.gameObject.SetActive(bIsShaikh);
            KinQuitBtn.gameObject.SetActive(true);

        }

        public void OnNoticeEditBtnClick()
        {
            // 编辑公告点击
            NoticeTI.enabled = true;
            
            // 显示编辑按钮
            NoticeSaveBtn.gameObject.SetActive(true);
            NoticeEditBtn.gameObject.SetActive(false);
        }

        public void OnNoticeSaveBtnClick()
        {
            // 请求保存
            GameLogic.cmd_kin_motifynotice cmdData = new GameLogic.cmd_kin_motifynotice();

            if (NoticeTI.textComponent.text == string.Empty)
                cmdData.szText = NoticeTI.placeholder.GetComponent<Text>().text;
            else
                cmdData.szText = NoticeTI.textComponent.text;

            if (ShieldWord.Instance.IsHaveShieldWord(cmdData.szText))
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_KIN_NOTICE_HAS_SHIELD_WORLD);
                NoticeTI.text = "";
                return;
            }

            // 编辑公告点击
            NoticeTI.enabled = false;

            // 显示编辑按钮
            NoticeEditBtn.gameObject.SetActive(true);
            NoticeSaveBtn.gameObject.SetActive(false);

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_KIN_MOTIFYNOTICE, ref cmdData);
        }

        public void OnOffLineTogChanged(bool bSelect)
        {
            CurSelectItem = null;

            ListKinMember.DataSource.BeginUpdate();
            ListKinMember.DataSource.Clear();
            List<cmd_kin_membernode> KinMemberList = LogicDataCenter.kinDataManager.KinMemberList;
            for (int i = 0; i < KinMemberList.Count; ++i)
            {
                cmd_kin_membernode cmdNodeData = KinMemberList[i];
                UListItemKinMember item = new UListItemKinMember();
                Toggle tOffLineTog = OffLineTog.GetComponent<Toggle>();
                if (!tOffLineTog.isOn && cmdNodeData.bIsOnline == 0)
                {
                    // 隐藏不在线玩家
                    continue;
                }
                item.IdentityID     = cmdNodeData.nIdentity;
                item.PlayerID       = cmdNodeData.dwPDBID;
                item.GradIcon      += cmdNodeData.szGradeName;
                item.PersonIcon     = cmdNodeData.nFaceID;
                item.PersonName    += cmdNodeData.szName;
                item.Power         = cmdNodeData.dwBattleVal;
                item.PowerRank     += cmdNodeData.dwBVOrder;
                item.GameState = cmdNodeData.byGameState;
                item.Level         += cmdNodeData.nLevel;
                item.isOnline       = cmdNodeData.bIsOnline;
                item.nSex           = cmdNodeData.nSex;
                item.nLastOrder     = cmdNodeData.dwLastOrder;
                item.nBVOrder       = cmdNodeData.dwBVOrder;
                if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == item.PlayerID)
                {
                    PopupAction = new UPopupAction();
                    PopupAction.Add(ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinLeave"), new UListComponentItemDataPopup(_actionSelected: LeaveKinCallBack));
                }
                else if (EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID) == LogicDataCenter.kinDataManager.KinBaseData.dwShaikhID)
                {
                    PopupAction = new UPopupAction();
                    PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamInvite.GetDescString(), new UPopupItemDataTeamInvite(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamRequest.GetDescString(), new UPopupItemDataTeamRequest(item.PlayerID));
                    PopupAction.Add(ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinAppointShaikh"), new UListComponentItemDataPopup(_actionSelected: AppointShaikhCallBack));
                    PopupAction.Add(ULocalizationService.Instance.Get("UIView", "PopupMenu", "KinKickout"), new UListComponentItemDataPopup(_actionSelected: KickoutKinCallBack));
                }
                else
                {
                    PopupAction = new UPopupAction();
                    PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamInvite.GetDescString(), new UPopupItemDataTeamInvite(item.PlayerID));
                    PopupAction.Add(UPopupItemDataTeamRequest.GetDescString(), new UPopupItemDataTeamRequest(item.PlayerID));
                }
                item.PopupAction = PopupAction;

                ListKinMember.DataSource.Add(item);
            }

            // 排序
            IEnumerable<UListItemKinMember> ListQuery = from item in ListKinMember.DataSource
                                                        orderby item.isOnline descending, item.IdentityID descending, item.Power descending
                                                        select item;

            UIWidgets.ObservableList<UListItemKinMember> dataSource = new UIWidgets.ObservableList<UListItemKinMember>();
            foreach (UListItemKinMember Item in ListQuery)
            {
                dataSource.Add(Item);
            }

            ListKinMember.DataSource = dataSource;
            ListKinMember.DataSource.EndUpdate();
        }

        // GloryWnd
        public void OnRecevKinGloryData()
        {
            cmd_kin_glory_head gloryHead = LogicDataCenter.kinDataManager.GloryHead;
            GloryKinName.text = gloryHead.szKinName;
            int nTotalNum = gloryHead.nWinNum + gloryHead.nLoseNum;
            GloryTotalNum.text = nTotalNum.ToString();
            GloryWinNum.text = gloryHead.nWinNum.ToString();
            int nWinPro = nTotalNum > 0 ? (gloryHead.nWinNum * 100 / nTotalNum) : 0;
            GloryWinPro.text = String.Format("{0}%{1}", nWinPro, ULocalizationService.Instance.Get("UIView", "Common", "WinRate"));
            GloryWinSlider.value = (float)gloryHead.nWinNum / nTotalNum;
            GloryWinSlider.maxValue = 1;
            GloryGodNum.text = gloryHead.nGodNum.ToString();
            Glory5KillNum.text = gloryHead.n5KillNum.ToString();
            Glory4KillNum.text = gloryHead.n4KillNum.ToString();
            Glory3KillNum.text = gloryHead.n3KillNum.ToString();
            GloryDeathNum.text = gloryHead.nDeathNum.ToString();
            GloryKillNum.text = gloryHead.nKillNum.ToString();
            GloryAssistNum.text = gloryHead.nAssistNum.ToString();

            ListKinGloryCup.DataSource.Clear();
            ObservableList<UListItemKinGloryCup> dataSource = new ObservableList<UListItemKinGloryCup>();
            List<cmd_kin_glory_node> gloryNodeList = LogicDataCenter.kinDataManager.GloryNodeList;
            for (int i = 0; i < gloryNodeList.Count; i++)
            {
                cmd_kin_glory_node nodeData = gloryNodeList[i];

                SSchemeLegendCupConfig cupConfig = LegendCupConfig.Instance.GetCupConfig(nodeData.nCupConfigID);
                if (cupConfig == null)
                    continue;
                // 8强赛保留12名，16强赛保留123名，32强赛保留前8名
                if (cupConfig.nMaxKinNum > 0 && cupConfig.nMaxKinNum <= 8 && nodeData.nCupRank > 2)
                    continue;
                else if (cupConfig.nMaxKinNum > 8 && cupConfig.nMaxKinNum <= 16 && nodeData.nCupRank > 3)
                    continue;
                else if (cupConfig.nMaxKinNum > 16 && cupConfig.nMaxKinNum <= 32 && nodeData.nCupRank > 8)
                    continue;

                UListItemKinGloryCup item = new UListItemKinGloryCup();
                item.nCupRank = nodeData.nCupRank;
                item.MatchName = nodeData.szCupName;
                item.llLegendCupID = nodeData.llLegendCupId;
                item.nCupConfigID = nodeData.nCupConfigID;
                item.nEndTime = nodeData.nEndTime;
                item.ConfigName = cupConfig.sCupConfigName;

                dataSource.Add(item);
            }
            ListKinGloryCup.DataSource = dataSource;
        }

        public void OnMatchRecordBtnClick()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_SHOW, null);
        }

#region // 点击标签排序
        public void OnIdentityIconBtnClicked()
        {
            IEnumerable<UListItemKinMember> ListQuery =
                isIdentityIconDescending ?
                from item in ListKinMember.DataSource orderby item.IdentityID descending select item :
                from item in ListKinMember.DataSource orderby item.IdentityID select item;
            isIdentityIconDescending = !isIdentityIconDescending;
            ResetKinMember(ListQuery);
        }

        public void OnPersonNameBtnClicked()
        {
            IEnumerable<UListItemKinMember> ListQuery =
                isPersonNameDescending ? 
                from item in ListKinMember.DataSource orderby item.PersonName descending select item :
                from item in ListKinMember.DataSource orderby item.PersonName select item;
            isPersonNameDescending = !isPersonNameDescending;
            ResetKinMember(ListQuery);
        }

        public void OnLevelBtnClicked()
        {
            ListKinMember.DataSource.Sort((item1, item2) => {
                int x = 0;
                int y = 0;
                int.TryParse(item1.Level, out x);
                int.TryParse(item2.Level, out y);
                return isLevelDescending ? y - x : x - y;
            });
            isLevelDescending = !isLevelDescending;
        }

        public void OnGradIconBtnClicked()
        {
            IEnumerable<UListItemKinMember> ListQuery =
                isGradIconDescending ?
                from item in ListKinMember.DataSource orderby item.GradIcon descending select item :
                from item in ListKinMember.DataSource orderby item.GradIcon select item;
            isGradIconDescending = !isGradIconDescending;
            ResetKinMember(ListQuery);
        }

        public void OnPowerBtnClicked()
        {
            IEnumerable<UListItemKinMember> ListQuery =
                isPowerDescending ?
                from item in ListKinMember.DataSource orderby item.Power descending select item :
                from item in ListKinMember.DataSource orderby item.Power select item;
            isPowerDescending = !isPowerDescending;
            ResetKinMember(ListQuery);
        }

        public void OnPowerRankBtnClicked()
        {
            ListKinMember.DataSource.Sort((item1, item2) => {
                int x = 0;
                int y = 0;
                int.TryParse(item1.PowerRank, out x);
                int.TryParse(item2.PowerRank, out y);
                return isLevelDescending ? y - x : x - y;
            });

            isPowerRankDescending = !isPowerRankDescending;
        }

        public void OnStateBtnClicked()
        {
            IEnumerable<UListItemKinMember> ListQuery =
                isStateDescending ?
                from item in ListKinMember.DataSource orderby item.GameState descending select item :
                from item in ListKinMember.DataSource orderby item.GameState select item;
            isStateDescending = !isStateDescending;
            ResetKinMember(ListQuery);
        }

        private void ResetKinMember(IEnumerable<UListItemKinMember> ListQuery)
        {
            if (ListQuery == null)
            {
                return;
            }

            UIWidgets.ObservableList<UListItemKinMember> tDataSource = new UIWidgets.ObservableList<UListItemKinMember>();
            foreach (UListItemKinMember Item in ListQuery)
            {
                tDataSource.Add(Item);
            }

            ListKinMember.DataSource.Clear();
            ListKinMember.DataSource = tDataSource;
        }
#endregion

    }
}
