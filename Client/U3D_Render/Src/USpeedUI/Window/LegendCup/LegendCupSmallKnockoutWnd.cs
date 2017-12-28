using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using USpeedUI.UWidgets;
using UnityEngine.EventSystems;
using DataCenter;
using UnityEngine;
using GameLogic;
using ASpeedGame.Data.LegendCupConfig;
using LegendCup_ManagedDef;
using USpeedUI.TooltipAndDialog;
using UIWidgets;
using USpeedUI.UExtensions;
using USpeedUI.UEffect;

namespace USpeedUI.LegendCup
{
    public class LegendCupSmallKnockoutWnd : UIPopupWnd<LegendCupSmallKnockoutWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_SMALLKNOCKOUT;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupSmallKnockoutView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_SMALLKNOCKOUT_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_SMALLKNOCKOUT_UPDATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_SMALLKNOCKOUT_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_SMALLKNOCKOUT_UPDATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_SMALLKNOCKOUT_SHOW:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            LegendCupIDData cmdData = (LegendCupIDData)data;
                            m_wndView.OnShowSmallKnockoutWnd(cmdData.nLegendCupID);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_SMALLKNOCKOUT_UPDATE:
                    {
                        if (m_wndView != null)
                        {
                            LegendCupIDData cmdData = (LegendCupIDData)data;
                            m_wndView.OnShowSmallKnockoutWnd(cmdData.nLegendCupID);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnClickCloseBtn();
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public override bool OnCloseKey()
        {
            if (m_wndView != null)
            {
                m_wndView.OnClickCloseBtn();
            }
            return base.OnCloseKey();
        }
    }

    public struct SKinTemplateData
    {
        public long nLegendCupID;
        public int nRoundID;
        public int nSearchID;
        public int nKinID;
        public int nWinnerKinID;
        public string strKinName;
    }

    public enum EDataGetType    // 数据获取类型
    {
        EDGT_Kin1,              // 战队1
        EDGT_Kin2,              // 战队2
        EDGT_KinWin,            // 胜利战队
        EDGT_KinLose,           // 失败战队

        EDGT_Max,
    }

    public enum EViewRoundIndex // 轮次
    {
        EVRI_First,             // 1轮
        EVRI_Second,            // 2轮
        EVRI_Third,             // 3轮
        EVRI_Fourth,            // 4轮
        EVRI_Fifth,             // 5轮

        EVRI_Max,
    }

    public enum EViewGroupCount // 每轮最大组数
    {
        EVGC_Round1 = 16,       // 1轮16节点，32个Item
        EVGC_Round2 = 8,        // 2轮8节点，16个Item
        EVGC_Round3 = 4,        // 3轮4节点，8个Item
        EVGC_Round4 = 2,        // 4轮2节点，4个Item
        EVGC_Round5 = 2,        // 5轮2节点，7个Item = 4个Item（节点） + 3个Item（冠、亚、季军）

        EVGC_Max = 16,
    }

    public enum ESpecialItemIndex
    {
        ESII_WinGroup1 = 0,
        ESII_WinGroup2 = 1,
        ESII_LoseGroup1 = 2,
        ESII_LoseGroup2 = 3,
        ESII_Pos1 = 4,
        ESII_Pos2 = 5,
        ESII_Pos3 = 6,
    }

    public class LegendCupSmallKnockKinItem : UCustomUIBehaviour, IPointerClickHandler
    {
        private Text KinName;
        private Text ScoreDesc;
        private Text JudgeDesc;
        private int m_nViewItemIndex;
        private int m_nViewRoundIndex;

        private LegendCupSmallKnockoutWndView m_wndView;
        private LegendCupSmallKnockNodeFrame m_nodeFrame;
        private long m_legendCupID;
        private int m_roundID;
        private int m_searchID;
        private int m_kinID;
        public int KinID { get { return m_kinID; } }
        public int SearchID { get { return m_searchID; } }
        
        // 产生比分节点，用于查看上一轮晋级信息
        private cmd_legendcup_recv_competition_node m_sCreateNode;
        //private int m_createScoreNodeRoundId;
        //private int m_createScoreNodeSerachId;
        //private int m_nCreateEnterTime;
        //private int m_nCreateBeginTime;
        //private byte m_byCreateCompetitionNodeState;
        private bool m_bCanJoinMatch = false;
        public bool CanJoinMatch { get { return m_bCanJoinMatch; } }

        private UPopupMenuHandle m_popupMenuHandle;
        private UPopupAction m_popupAction;

        public void InitView(LegendCupSmallKnockoutWndView view, LegendCupSmallKnockNodeFrame frame, int nViewRoundIndex, int nViewItemIndex)
        {
            KinName = this.GetRectTransform.FindChild("KinName").GetComponent<Text>();
            ScoreDesc = this.GetRectTransform.FindChild("ScoreDesc").GetComponent<Text>();
            KinName.text = "-";
            ScoreDesc.text = "-:-";
            m_wndView = view;
            m_nodeFrame = frame;
            m_nViewRoundIndex = nViewRoundIndex;
            m_nViewItemIndex = nViewItemIndex;

            GameObject judgeGo = m_nodeFrame.JudgeFrameTemplate.InstanceMainRes();
            if (!judgeGo)
                return;
            RectTransform judgeRect = judgeGo.transform as RectTransform;
            if (!judgeRect)
                return;
            judgeRect.SetParent(this.gameObject.transform, false);
            // 校验数据
            if (nViewRoundIndex < (int)EViewRoundIndex.EVRI_First || nViewRoundIndex > (int)EViewRoundIndex.EVRI_Fifth || nViewItemIndex < 0)
                return;
            // 此处由于选中框朝向不同，需要判定使用哪一个，参照预制体
            // View最后一轮需特殊处理，指定ESII_WinGroup1，ESII_LoseGroup1用左侧朝向的，其余用右侧朝向的
            // View其余轮次，用Index和总节点比较
            if (nViewRoundIndex == (int)EViewRoundIndex.EVRI_Fifth)
            {
                if (nViewItemIndex == (int)ESpecialItemIndex.ESII_WinGroup1 || nViewItemIndex == (int)ESpecialItemIndex.ESII_LoseGroup1)
                {
                    judgeRect.SetLocalPosition(new Vector3(-58f, -37f, 0f));
                }
                else
                {
                    judgeRect.SetLocalPosition(new Vector3(58f, -37f, 0f));
                }
            }
            else
            {
                if (nViewItemIndex < m_nodeFrame.DicRoundMaxViewItem[nViewRoundIndex] / 2)
                {
                    judgeRect.SetLocalPosition(new Vector3(-58f, -37f, 0f));
                }
                else
                {
                    judgeRect.SetLocalPosition(new Vector3(58f, -37f, 0f));
                }
            }
            JudgeDesc = judgeRect.FindChild("JudgeDesc").GetComponent<Text>();
            JudgeDesc.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "WaitJudgeDesc");
            JudgeDesc.gameObject.SetActive(false);

            m_sCreateNode = new cmd_legendcup_recv_competition_node();
        }

        public void SetData(long nCupID,  SKinTemplateData kindata)
        {
            m_legendCupID = nCupID;
            m_roundID = kindata.nRoundID;
            m_searchID = kindata.nSearchID;
            m_kinID = kindata.nKinID;

            bool bInThisPos = LogicDataCenter.legendCupDataManager.CheckIsSelfInCompetitionMember(kindata.nKinID);
            //m_bCanJoinMatch = bInThisPos && (kindata.byCompetitionNodeState == (byte)ECompetitionNodeState.emNodeState_CanEnter);

            // 队名，（自己， 别人输，别人赢）
            Color clr = new Color();
            if (bInThisPos)
            {
                clr = UDefines.CommomColor(ECommonColor.ECC_Gold1);         //金色
            }
            else
            {
                if (kindata.nWinnerKinID != 0 && kindata.nWinnerKinID != kindata.nKinID)
                {
                    clr = UDefines.CommomColor(ECommonColor.ECC_Gray);     // 灰色
                }
                else
                {
                    clr = UDefines.CommomColor(ECommonColor.ECC_Blue1);     // 蓝色
                } 
            }
            KinName.color = clr;
            KinName.text = kindata.strKinName;

            // 添加右键弹窗
            this.gameObject.RemoveComponent<UPopupMenuHandle>();
            m_popupMenuHandle = this.gameObject.AddComponent<UPopupMenuHandle>();
            if (m_popupMenuHandle != null)
            {
                m_popupAction = new UPopupAction();
                m_popupAction.Add(UPopupItemDataKinView.GetDescString(), new UPopupItemDataKinView(m_kinID));
                m_popupAction.Add(UPopupItemDataCompetitionKinMemberView.GetDescString(), new UPopupItemDataCompetitionKinMemberView(m_kinID));

                m_popupMenuHandle.HandlePopupAction = m_popupAction;
            }
        }

        public void SetScore(string strScoreDesc, cmd_legendcup_recv_competition_node createNode)
        {
            m_sCreateNode = createNode;

            bool bInCreateNode = LogicDataCenter.legendCupDataManager.CheckIsSelfInCompetitionMember(createNode.nKin1ID) || LogicDataCenter.legendCupDataManager.CheckIsSelfInCompetitionMember(createNode.nKin2ID);
            m_bCanJoinMatch = bInCreateNode && (createNode.byCompetitionNodeState == (byte)ECompetitionNodeState.emNodeState_CanEnter);

            // 比分
            ScoreDesc.text = strScoreDesc;
            // 待裁决图标
            JudgeDesc.gameObject.SetActive(createNode.byCompetitionNodeState == (byte)ECompetitionNodeState.emNodeState_WaitJudge);

            //m_createScoreNodeRoundId = nCreateScoreNodeRoundId;
            //m_createScoreNodeSerachId = nCreateScoreNodeSearchId;
            //m_nCreateEnterTime = nCreateEnterTime;
            //m_nCreateBeginTime = nCreateBeginTime;
            //m_byCreateCompetitionNodeState = byCompetitionNodeState;
            //JudgeDesc.gameObject.SetActive(byCompetitionNodeState == (byte)ECompetitionNodeState.emNodeState_WaitJudge);
        }

        public void OnPointerClick(PointerEventData eventData)
        {
            if (m_nodeFrame == null || m_wndView == null)
                return;

            // 右击响应
            if (eventData.button == PointerEventData.InputButton.Right)
            {
                if (m_popupMenuHandle == null || m_popupAction.Count == 0)
                {
                    Trace.LogWarning("要使用弹出菜单，你要先初始化元素容器.");
                    return;
                }

                //发消息给PopupMenu View
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_POPUPMENU, new UIPopupMenuData(m_popupAction, GetTransform, UIWidgets.TipPosition.MouseBottomRightCorner));
            }

            // 左击响应
            // 不继续使父层级响应
            (eventData as UPointerEventDataEx).CanBubbleEvent = false;
            // 设置选中图片
            m_nodeFrame.SetSelectIcon(this.GetRectTransform, m_nViewItemIndex, m_nViewRoundIndex);
            // 设置选中节点信息
            m_wndView.SetSelectCompetitionNodeID(m_sCreateNode.nRoundID, m_sCreateNode.nSearchID);
            // 设置查看数据
            UpdateCupStateDesc();

            // 双击响应
            if (eventData.clickCount == 2)
            {
                if (m_sCreateNode.nRoundID > 0)         // 内层节点查看战绩
                {
                    m_wndView.OnClickViewCompetitionBtn();
                }
                else if (m_kinID > 0)                   // 外层节点查看战队
                {
                    LogicDataCenter.kinDataManager.viewCompetitionKinInfo(m_kinID);
                }
            }  

            // GM账号，进入比赛入口状态更新
            int nPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            if (GameLogicAPI.bIsGM(nPDBID))
            {
                if (m_sCreateNode.byCompetitionNodeState == (byte)ECompetitionNodeState.emNodeState_CanEnter)
                {
                    m_nodeFrame.SetJoinMatchBtnState(true);
                    m_nodeFrame.CurRoundSelfNodeSearchID = m_sCreateNode.nSearchID;
                }
                else
                {
                    m_nodeFrame.SetJoinMatchBtnState(false);
                }  
            }
        }

        // 点击时，更新杯赛节点状态显示
        private void UpdateCupStateDesc()
        {
            if (m_sCreateNode.nRoundID < 0)// 最外层节点不设置
            {
                m_nodeFrame.SetEntryTime(m_nodeFrame.CompetitionInfo);
            }
            else
            {
                // 下一轮时间
                int nEnterTime = m_sCreateNode.nEnterTime;
                int nBeginTime = m_sCreateNode.nBeginTime;
                if (nEnterTime <= 0 || nBeginTime <= 0)
                {
                    int nLogicRoundIndex = m_nViewRoundIndex - m_nodeFrame.ViewCurRound + m_nodeFrame.LogicCurRound;
                    int nCreateRound = nLogicRoundIndex - 1;
                    if (m_nViewRoundIndex == (int)EViewRoundIndex.EVRI_Fifth && (m_nViewItemIndex == (int)ESpecialItemIndex.ESII_Pos1 || m_nViewItemIndex == (int)ESpecialItemIndex.ESII_Pos2 || m_nViewItemIndex == (int)ESpecialItemIndex.ESII_Pos3))
                    {
                        nCreateRound = nLogicRoundIndex;
                    }
                    if (m_nodeFrame.CompetitionInfo.roundInfo.Count > nCreateRound - 1)
                    {
                        nEnterTime = m_nodeFrame.CompetitionInfo.roundInfo[nCreateRound - 1].nRoundCreateRoomTme;
                        nBeginTime = m_nodeFrame.CompetitionInfo.roundInfo[nCreateRound - 1].nRoundCreateRoomTme + m_nodeFrame.CompetitionInfo.roundInfo[nCreateRound - 1].nBeginInterval;
                    }
                }
                DateTime enterTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(nEnterTime);
                enterTime = enterTime.ToLocalTime();
                DateTime beginTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(nBeginTime);
                beginTime = beginTime.ToLocalTime();

                if (!m_nodeFrame.BIsInCup)
                {
                    if (m_nodeFrame.BIsCreater)
                    {
                        if (m_nodeFrame.BIsOnline)
                        {
                            switch (m_sCreateNode.byCompetitionNodeState)
                            {
                                case (int)ECompetitionNodeState.emNodeState_WaitStart:
                                case (int)ECompetitionNodeState.emNodeState_CanEnter:
                                    {
                                        //m_nodeFrame.EntryTime.text = String.Format("当前比赛开始时间为：{0}月{1}日{2:D2}:{3:D2}", enterTime.Month, enterTime.Day, enterTime.Hour, enterTime.Minute);
                                        m_nodeFrame.EntryTime.text = String.Format(ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeTime"), enterTime.Month, enterTime.Day, enterTime.Hour, enterTime.Minute);
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_Competition:
                                    {
                                        //m_nodeFrame.EntryTime.text = "当前比赛正在进行中";
                                        m_nodeFrame.EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeCompetition");
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_WaitJudge:
                                    {
                                        //m_nodeFrame.EntryTime.text = "当前比赛已结束，可双击查看比赛详情或重置比赛";
                                        m_nodeFrame.EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeEndViewOrJudge");
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_End:
                                    {
                                        //m_nodeFrame.EntryTime.text = "当前比赛已结束，可双击查看比赛详情";
                                        m_nodeFrame.EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeEndView");
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        else
                        {
                            switch (m_sCreateNode.byCompetitionNodeState)
                            {
                                case (int)ECompetitionNodeState.emNodeState_WaitStart:
                                case (int)ECompetitionNodeState.emNodeState_CanEnter:
                                    {
                                        //m_nodeFrame.EntryTime.text = "比赛还未进行，请等待比赛结束后确认比赛或直接裁定";
                                        m_nodeFrame.EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeWaitStart");
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_Competition:
                                    {
                                        //m_nodeFrame.EntryTime.text = "当前比赛正在进行中";
                                        m_nodeFrame.EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeCompetition");
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_WaitJudge:
                                    {
                                        //m_nodeFrame.EntryTime.text = "该场比赛已结束，请确认比赛或裁定该场比赛的结果";
                                        m_nodeFrame.EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeEndWaitJudge");
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_End:
                                    {
                                        //m_nodeFrame.EntryTime.text = "该比赛已裁定完成，结果不能再更改";
                                        m_nodeFrame.EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeIsJudged");
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                    else
                    {
                        if (m_nodeFrame.BIsOnline)
                        {
                            switch (m_sCreateNode.byCompetitionNodeState)
                            {
                                case (int)ECompetitionNodeState.emNodeState_WaitStart:
                                case (int)ECompetitionNodeState.emNodeState_CanEnter:
                                    {
                                        //m_nodeFrame.EntryTime.text = String.Format("当前比赛开始时间为：{0}月{1}日{2:D2}:{3:D2}", enterTime.Month, enterTime.Day, enterTime.Hour, enterTime.Minute);
                                        m_nodeFrame.EntryTime.text = String.Format(ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeTime"), enterTime.Month, enterTime.Day, enterTime.Hour, enterTime.Minute);
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_Competition:
                                    {
                                        //m_nodeFrame.EntryTime.text = "当前比赛正在进行中";
                                        m_nodeFrame.EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeCompetition");
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_WaitJudge:
                                case (int)ECompetitionNodeState.emNodeState_End:
                                    {
                                        //m_nodeFrame.EntryTime.text = "当前比赛已结束，可双击查看比赛详情";
                                        m_nodeFrame.EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeEndView");
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        else
                        {
                            switch (m_sCreateNode.byCompetitionNodeState)
                            {
                                case (int)ECompetitionNodeState.emNodeState_WaitStart:
                                case (int)ECompetitionNodeState.emNodeState_CanEnter:
                                    {
                                        //m_nodeFrame.EntryTime.text = "当前比赛还未开始";
                                        m_nodeFrame.EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeNotStart");
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_Competition:
                                    {
                                        //m_nodeFrame.EntryTime.text = "当前比赛将正在进行中";
                                        m_nodeFrame.EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeCompetition");
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_WaitJudge:
                                    {
                                        //m_nodeFrame.EntryTime.text = "当前比赛还在等待举办人确认比赛有效";
                                        m_nodeFrame.EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeWaitJudge");
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_End:
                                    {
                                        //m_nodeFrame.EntryTime.text = "当前比赛已结束，可双击查看比赛详情";
                                        m_nodeFrame.EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewNodeEndView");
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                }

            }
        }
    }

    public class LegendCupSmallKnockNodeFrame : UCustomUIBehaviour, IPointerClickHandler
    {
        public RectTransform In32Frame;
        public RectTransform In16Frame;
        public RectTransform In8Frame;
        public RectTransform In4Frame;
        public RectTransform In2Frame;
        public Text EntryTime;
        public Image SelectIcon1;
        public Image SelectIcon2;
        public Button JoinMatchBtn;
        public Text JoinMatchBtnDesc;

        public ResNode JudgeFrameTemplate;

        private LegendCupSmallKnockoutWndView m_wndView;
        private LegendCupSmallKnockKinItem[,] m_kinNodeMatrix;
        private int m_nLogicCurRound;
        public int LogicCurRound { get { return m_nLogicCurRound; } }
        private int m_nViewCurRound;
        public int ViewCurRound { get { return m_nViewCurRound; } }
        private long m_nLegendCupID;
        private int m_nCurRoundSelfNodeSearchID;
        public int CurRoundSelfNodeSearchID { get { return m_nCurRoundSelfNodeSearchID; } set { m_nCurRoundSelfNodeSearchID = value; } }
        private Dictionary<int, int> m_DicRoundMaxViewItem;
        public Dictionary<int, int> DicRoundMaxViewItem { get { return m_DicRoundMaxViewItem; } }
        private bool m_bIsInCup = false;     // 自身是否参赛
        public bool BIsInCup { get { return m_bIsInCup; } }
        private bool m_bIsCreater = false;
        public bool BIsCreater { get { return m_bIsCreater; } }
        private bool m_bIsOnline = false;
        public bool BIsOnline { get { return m_bIsOnline; } }
        private SingleCompetitionInfo m_competitionInfo;
        public SingleCompetitionInfo CompetitionInfo { get { return m_competitionInfo; } }

        public Transform AttachPoint;
        private UEffectParamBase m_effectParam;

        public void InitView(LegendCupSmallKnockoutWndView view)
        {
            if (view == null)
                return;

            m_wndView = view;
            JoinMatchBtnDesc.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "JoinMatch"); 

            m_DicRoundMaxViewItem = new Dictionary<int, int>();
            m_DicRoundMaxViewItem.Add((int)EViewRoundIndex.EVRI_First, (int)EViewGroupCount.EVGC_Round1 * 2);
            m_DicRoundMaxViewItem.Add((int)EViewRoundIndex.EVRI_Second, (int)EViewGroupCount.EVGC_Round2 * 2);
            m_DicRoundMaxViewItem.Add((int)EViewRoundIndex.EVRI_Third, (int)EViewGroupCount.EVGC_Round3 * 2);
            m_DicRoundMaxViewItem.Add((int)EViewRoundIndex.EVRI_Fourth, (int)EViewGroupCount.EVGC_Round4 * 2);
            m_DicRoundMaxViewItem.Add((int)EViewRoundIndex.EVRI_Fifth, (int)EViewGroupCount.EVGC_Round5 * 2 + 3);

            JudgeFrameTemplate = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "UI/Prefabs/LegendCup/JudgeItem");

            // 初始化m_kinNodeMatrix 
            // 预制体内的命名必须为KinTemplate1-32,KinTemplate1-16,KinTemplate1-8,KinTemplate1-4,KinTemplate1-7
            m_kinNodeMatrix = new LegendCupSmallKnockKinItem[(int)EViewRoundIndex.EVRI_Max, (int)EViewGroupCount.EVGC_Max * 2];
            for (int h = 0; h < (int)EViewGroupCount.EVGC_Round1 * 2; ++h)
            {
                m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_First, h] = In32Frame.FindChild("KinTemplate" + (h + 1)).gameObject.GetComponent<LegendCupSmallKnockKinItem>();
            }
            for (int i = 0; i < (int)EViewGroupCount.EVGC_Round2 * 2; ++i)
			{
                m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Second, i] = In16Frame.FindChild("KinTemplate" + (i + 1)).gameObject.GetComponent<LegendCupSmallKnockKinItem>();
			}
            for (int j = 0; j < (int)EViewGroupCount.EVGC_Round3 * 2; ++j)
            {
                m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Third, j] = In8Frame.FindChild("KinTemplate" + (j + 1)).gameObject.GetComponent<LegendCupSmallKnockKinItem>();
            }
            for (int p = 0; p < (int)EViewGroupCount.EVGC_Round4 * 2; ++p)
            {
                m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Fourth, p] = In4Frame.FindChild("KinTemplate" + (p + 1)).gameObject.GetComponent<LegendCupSmallKnockKinItem>();
            }
            for (int q = 0; q < (int)EViewGroupCount.EVGC_Round5 * 2; ++q)
            {
                m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Fifth, q] = In2Frame.FindChild("KinTemplate" + (q + 1)).gameObject.GetComponent<LegendCupSmallKnockKinItem>();
            }
            m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Fifth, (int)ESpecialItemIndex.ESII_Pos1] = In2Frame.FindChild("KinTemplate" + ((int)ESpecialItemIndex.ESII_Pos1 + 1)).gameObject.GetComponent<LegendCupSmallKnockKinItem>();
            m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Fifth, (int)ESpecialItemIndex.ESII_Pos2] = In2Frame.FindChild("KinTemplate" + ((int)ESpecialItemIndex.ESII_Pos2 + 1)).gameObject.GetComponent<LegendCupSmallKnockKinItem>();
            m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Fifth, (int)ESpecialItemIndex.ESII_Pos3] = In2Frame.FindChild("KinTemplate" + ((int)ESpecialItemIndex.ESII_Pos3 + 1)).gameObject.GetComponent<LegendCupSmallKnockKinItem>();

            // 初始化itemView
            for (int m = 0; m < (int)EViewRoundIndex.EVRI_Max; ++m)
            {
                for (int n = 0; n < (int)EViewGroupCount.EVGC_Max * 2; ++n)
                {
                    if (m_kinNodeMatrix[m, n] != null)
                        m_kinNodeMatrix[m, n].InitView(m_wndView, this, m, n);
                }
            }
        }

        public void SetData(long nCupID, SingleCompetitionInfo competitionInfo)
        {
            m_competitionInfo = competitionInfo;
            m_nLegendCupID = nCupID;
            m_nLogicCurRound = competitionInfo.headInfo.nRoundID;

            // 计算当前显示层应该放置位置 m_nViewCurRound
            if (competitionInfo.headInfo.nGroupCount > (int)EViewGroupCount.EVGC_Round2 && competitionInfo.headInfo.nGroupCount <= (int)EViewGroupCount.EVGC_Round1)
                m_nViewCurRound = (int)EViewRoundIndex.EVRI_First;
            else if (competitionInfo.headInfo.nGroupCount > (int)EViewGroupCount.EVGC_Round3 && competitionInfo.headInfo.nGroupCount <= (int)EViewGroupCount.EVGC_Round2)
                m_nViewCurRound = (int)EViewRoundIndex.EVRI_Second;
            else if (competitionInfo.headInfo.nGroupCount > (int)EViewGroupCount.EVGC_Round4 && competitionInfo.headInfo.nGroupCount <= (int)EViewGroupCount.EVGC_Round3)
                m_nViewCurRound = (int)EViewRoundIndex.EVRI_Third;
            else if (competitionInfo.headInfo.nGroupCount <= (int)EViewGroupCount.EVGC_Round4 && competitionInfo.headInfo.nIsFinalRound <= 0)
                m_nViewCurRound = (int)EViewRoundIndex.EVRI_Fourth;
            else
                m_nViewCurRound = (int)EViewRoundIndex.EVRI_Fifth;

            // 设置显示节点的基本信息
            SetItemBaseData(competitionInfo);

            // 显示节点的比分信息
            SetItemCreateData(competitionInfo);

            // 处理布局显示层数
            SetFrameVisible();

            // 进场时间提示
            SetEntryTime(competitionInfo);

            // 设置参赛按钮状态
            UpdateJoinMatchBtnState();
        }

        public void Clear()
        {
            if (m_effectParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_effectParam);
                m_effectParam = null;
            }

            AssetBundleManager.DeleteAssets(ref JudgeFrameTemplate, true);
        }

        public void OnPointerClick(PointerEventData eventData)
        {
            // 取消所有选中图片
            SelectIcon1.gameObject.SetActive(false);
            SelectIcon2.gameObject.SetActive(false);

            // 清空选中节点信息，区分0，-1，是为了给客户端不同的提示
            //m_wndView.SetSelectCompetitionNodeID(0, 0, 0, 0, 0);
            m_wndView.SetSelectCompetitionNodeID(-1, -1);
            SetEntryTime(m_competitionInfo);

            // GM账号，进入比赛入口状态更新
            int nPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            if (GameLogicAPI.bIsGM(nPDBID))
            {
                SetJoinMatchBtnState(false);
            }
        }

        public void SetSelectIcon(RectTransform rect, int nViewIndex, int nViewRound)
        {
            // 校验数据
            if (nViewRound < (int)EViewRoundIndex.EVRI_First || nViewRound > (int)EViewRoundIndex.EVRI_Fifth || nViewIndex < 0)
                return;

            // 此处由于选中框朝向不同，需要判定使用哪一个，参照预制体
            // View最后一轮需特殊处理，指定ESII_WinGroup1，ESII_LoseGroup1用左侧朝向的，其余用右侧朝向的
            // View其余轮次，用Index和总节点比较
            if (nViewRound == (int)EViewRoundIndex.EVRI_Fifth)
            {
                if (nViewIndex == (int)ESpecialItemIndex.ESII_WinGroup1 || nViewIndex == (int)ESpecialItemIndex.ESII_LoseGroup1)
                {
                    SelectIcon1.rectTransform.SetParent(rect, false);
                    SelectIcon1.gameObject.SetActive(true);
                    SelectIcon2.gameObject.SetActive(false);
                }
                else
                {
                    SelectIcon2.rectTransform.SetParent(rect, false);
                    SelectIcon2.gameObject.SetActive(true);
                    SelectIcon1.gameObject.SetActive(false);
                }
            }
            else
            {
                if (nViewIndex < m_DicRoundMaxViewItem[nViewRound] / 2)
                {
                    SelectIcon1.rectTransform.SetParent(rect, false);
                    SelectIcon1.gameObject.SetActive(true);
                    SelectIcon2.gameObject.SetActive(false);
                }
                else
                {
                    SelectIcon2.rectTransform.SetParent(rect, false);
                    SelectIcon2.gameObject.SetActive(true);
                    SelectIcon1.gameObject.SetActive(false);
                }
            }
        }

        public void OnClickJoinMatchBtn()
        {
            if (m_nLegendCupID == 0 || m_nLogicCurRound == 0 || m_nCurRoundSelfNodeSearchID == 0)
            {
                return;
            }

            cmd_legendcup_enter_competition_node nodeData = new cmd_legendcup_enter_competition_node();
            nodeData.nLegendCupID = m_nLegendCupID;
            nodeData.nRoundID = m_nLogicCurRound;
            nodeData.nSearchID = m_nCurRoundSelfNodeSearchID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_enter_competition_node>(GameLogicDef.GVIEWCMD_LEGENDCUP_ENTER_COMPETITION_NODE, ref nodeData);

            // 删除DIDA提醒
            LogicDataCenter.legendCupDataManager.SetLegendCupDIDA(false, m_nLegendCupID, m_nLogicCurRound, m_nCurRoundSelfNodeSearchID);
        }

        // 设置节点的基本信息
        private void SetItemBaseData(SingleCompetitionInfo competitionInfo)
        {
            // 计算每个node应该放置位置
            foreach (cmd_legendcup_recv_competition_node node in competitionInfo.nodeInfo)
            {
                // 小组赛数据不体现在淘汰赛部分
                if (node.byCompetitionType == (int)ERoundCompetitionType.emType_GroupCompetition)
                    continue;

                int nTempViewRound = m_nViewCurRound + node.nRoundID - m_nLogicCurRound;
                // 排除非法节点
                if (nTempViewRound < (int)EViewRoundIndex.EVRI_First || nTempViewRound > (int)EViewRoundIndex.EVRI_Fifth)
                    continue;

                SKinTemplateData kin1Info = GetKinItemData(m_nLegendCupID, node, EDataGetType.EDGT_Kin1);
                SKinTemplateData kin2Info = GetKinItemData(m_nLegendCupID, node, EDataGetType.EDGT_Kin2);
                m_kinNodeMatrix[nTempViewRound, (node.nGroupID - 1) * 2].SetData(m_nLegendCupID, kin1Info);
                m_kinNodeMatrix[nTempViewRound, (node.nGroupID - 1) * 2 + 1].SetData(m_nLegendCupID, kin2Info);
            }
            // 特殊处理当前轮节点：最后一轮（冠、亚、季军ItemItem）；其他（下一轮胜者显示），倒数第二轮（下一轮胜者、败者显示）,
            if (competitionInfo.headInfo.nIsFinalRound > 0)
            {
                cmd_legendcup_recv_competition_node winGroupNode = new cmd_legendcup_recv_competition_node();
                cmd_legendcup_recv_competition_node loseGroupNode = new cmd_legendcup_recv_competition_node();
                foreach (cmd_legendcup_recv_competition_node item in competitionInfo.nodeInfo)
                {
                    if (item.nRoundID != competitionInfo.headInfo.nRoundID)
                        continue;
                    if (item.nGroupID == 1)
                        winGroupNode = item;
                    if (item.nGroupID == 2)
                        loseGroupNode = item;
                }
                if (winGroupNode.nWinnerKinID != 0)
                {
                    // 冠军、亚军
                    SKinTemplateData pos1KinInfo = GetKinItemData(m_nLegendCupID, winGroupNode, EDataGetType.EDGT_KinWin);
                    m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Fifth, (int)ESpecialItemIndex.ESII_Pos1].SetData(m_nLegendCupID, pos1KinInfo);
                    SKinTemplateData pos2KinInfo = GetKinItemData(m_nLegendCupID, winGroupNode, EDataGetType.EDGT_KinLose);
                    m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Fifth, (int)ESpecialItemIndex.ESII_Pos2].SetData(m_nLegendCupID, pos2KinInfo);
                }
                if (loseGroupNode.nWinnerKinID != 0)
                {
                    SKinTemplateData pos3KinInfo = GetKinItemData(m_nLegendCupID, loseGroupNode, EDataGetType.EDGT_KinWin);
                    m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Fifth, (int)ESpecialItemIndex.ESII_Pos3].SetData(m_nLegendCupID, pos3KinInfo);
                }
            }
            else
            {
                foreach (cmd_legendcup_recv_competition_node item in competitionInfo.nodeInfo)
                {
                    if (item.nRoundID == competitionInfo.headInfo.nRoundID && item.nWinnerKinID != 0)
                    {
                        // 胜
                        int nTempViewRound = m_nViewCurRound + item.nRoundID - m_nLogicCurRound;
                        SKinTemplateData kinWinInfo = new SKinTemplateData();
                        kinWinInfo.nKinID = item.nWinnerKinID;
                        kinWinInfo.strKinName = (item.nWinnerKinID == item.nKin1ID) ? item.szKin1Name : item.szKin2Name;
                        m_kinNodeMatrix[nTempViewRound + 1, item.nGroupID - 1].SetData(m_nLegendCupID, kinWinInfo);//nTempViewRound + 1轮，item.nGroupID - 1序
                        // 败
                        if (nTempViewRound == (int)EViewRoundIndex.EVRI_Fourth)
                        {
                            SKinTemplateData kinLoseInfo = new SKinTemplateData();
                            kinLoseInfo.nKinID = (item.nWinnerKinID == item.nKin1ID) ? item.nKin2ID : item.nKin1ID;
                            kinLoseInfo.strKinName = (item.nWinnerKinID == item.nKin1ID) ? item.szKin2Name : item.szKin1Name;
                            m_kinNodeMatrix[nTempViewRound + 1, item.nGroupID + 1].SetData(m_nLegendCupID, kinLoseInfo);//nTempViewRound + 1轮，item.nGroupID + 1序
                        }
                    }
                }
            }
        }

        // 设置产生节点的基本信息
        private void SetItemCreateData(SingleCompetitionInfo competitionInfo)
        {
            foreach (cmd_legendcup_recv_competition_node node in competitionInfo.nodeInfo)
            {
                // 小组赛数据不体现在淘汰赛部分
                if (node.byCompetitionType == (int)ERoundCompetitionType.emType_GroupCompetition)
                    continue;

                int nTempViewRound = m_nViewCurRound + node.nRoundID - m_nLogicCurRound;

                string strCreateScore1 = "-:-";
                string strCreateScore2 = "-:-";
                //int nCreateRoundID = node.nRoundID;
                //int nCreateSearchID = node.nSearchID;
                //int nCreateEnterTime = node.nEnterTime;
                //int nCreateBeginTime = node.nBeginTime;
                //byte byCreateCompetitionState = node.byCompetitionNodeState;
                // 有结果节点赋值
                if (node.nKin1Score != 0 || node.nKin2Score != 0)
                {
                    // 比分设置规则：胜者大比分在前，败者小比分在前
                    if (node.nKin1Score > node.nKin2Score)
                    {
                        strCreateScore1 = String.Format("{0}:{1}", node.nKin1Score, node.nKin2Score);
                        strCreateScore2 = String.Format("{0}:{1}", node.nKin1Score, node.nKin2Score);
                    }
                    else
                    {
                        strCreateScore1 = String.Format("{0}:{1}", node.nKin2Score, node.nKin1Score);
                        strCreateScore2 = String.Format("{0}:{1}", node.nKin2Score, node.nKin1Score);
                    }
                }
 
                if (nTempViewRound < (int)EViewRoundIndex.EVRI_Fourth)        // 1轮，2轮，3轮节点直接赋值下一轮胜者,映射位置（+1，-1）
                {
                    if (m_kinNodeMatrix[nTempViewRound + 1, node.nGroupID - 1] != null)
                        m_kinNodeMatrix[nTempViewRound + 1, node.nGroupID - 1].SetScore(strCreateScore1, node);
                }
                else if (nTempViewRound == (int)EViewRoundIndex.EVRI_Fourth)  // 倒数第二轮,赋值胜者、败者
                {
                    if (m_kinNodeMatrix[nTempViewRound + 1, node.nGroupID - 1] != null)
                        m_kinNodeMatrix[nTempViewRound + 1, node.nGroupID - 1].SetScore(strCreateScore1, node);
                    if (m_kinNodeMatrix[nTempViewRound + 1, node.nGroupID + 1] != null)
                        m_kinNodeMatrix[nTempViewRound + 1, node.nGroupID + 1].SetScore(strCreateScore2, node);
                }
                else if (nTempViewRound == (int)EViewRoundIndex.EVRI_Fifth) // 最后一轮，直接赋值冠、亚、季军
                {
                    if (node.nGroupID == 1 && m_kinNodeMatrix[nTempViewRound, (int)ESpecialItemIndex.ESII_Pos1] != null && m_kinNodeMatrix[nTempViewRound, (int)ESpecialItemIndex.ESII_Pos2] != null)
                    {
                        m_kinNodeMatrix[nTempViewRound, (int)ESpecialItemIndex.ESII_Pos1].SetScore(strCreateScore1, node);
                        m_kinNodeMatrix[nTempViewRound, (int)ESpecialItemIndex.ESII_Pos2].SetScore(strCreateScore2, node);
                    }
                    if (node.nGroupID == 2 && m_kinNodeMatrix[nTempViewRound, (int)ESpecialItemIndex.ESII_Pos3] != null)
                        m_kinNodeMatrix[nTempViewRound, (int)ESpecialItemIndex.ESII_Pos3].SetScore(strCreateScore1, node);
                }
            }
        }

        // 设置轮次是否显示
        private void SetFrameVisible()
        {
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(m_nLegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;
            List<SSchemeLegendCupDetailConfig> cupTypeConfigList = LegendCupConfig.Instance.GetCupDetailConfigList(cupBaseData.nCupConfigID);
            if (cupTypeConfigList == null)
                return;

            // 节点是否拥有（去掉未进行轮次）
            bool[] bHaveNode = new bool[(int)EViewRoundIndex.EVRI_Max];
            for (int i = 0; i < (int)EViewRoundIndex.EVRI_Max; i++)
            {
                for (int j = 0; j < (int)EViewGroupCount.EVGC_Max * 2; j++)
                {
                    if (m_kinNodeMatrix[i, j] != null && m_kinNodeMatrix[i, j].KinID != 0)
                    {
                        bHaveNode[i] = true;
                        break;
                    }
                }
            }

            // 配置是否拥有（去掉非淘汰赛信息）
            bool[] bHaveConfig = new bool[(int)EViewRoundIndex.EVRI_Max];
            foreach (var item in cupTypeConfigList)
            {
                int nTempViewRound = m_nViewCurRound + item.nRoundIndex - m_nLogicCurRound;
                // 排除非法节点
                if (nTempViewRound < (int)EViewRoundIndex.EVRI_First || nTempViewRound > (int)EViewRoundIndex.EVRI_Fifth)
                    continue;

                bHaveConfig[nTempViewRound] = true;
            }
            //bool[] bHaveConfig = new bool[(int)EViewRoundIndex.EVRI_Max];
            //foreach (SSchemeLegendCupDetailConfig item in cupTypeConfigList)
            //{
            //    if (item.nCompetitionType != (int)ERoundCompetitionType.emType_KnockoutCompetition)
            //        continue;

            //    switch (item.nGroupCount)
            //    {
            //        case (int)EViewGroupCount.EVGC_Round1:
            //            {
            //                bHaveConfig[(int)EViewRoundIndex.EVRI_First] = true;
            //            }
            //            break;
            //        case (int)EViewGroupCount.EVGC_Round2:
            //            {
            //                bHaveConfig[(int)EViewRoundIndex.EVRI_Second] = true;
            //            }
            //            break;
            //        case (int)EViewGroupCount.EVGC_Round3:
            //            {
            //                bHaveConfig[(int)EViewRoundIndex.EVRI_Third] = true;
            //            }
            //            break;
            //        case (int)EViewGroupCount.EVGC_Round4:
            //            {
            //                bHaveConfig[(int)EViewRoundIndex.EVRI_Fourth] = true;
            //            }
            //            break;
            //        case (int)EViewGroupCount.EVGC_Round5 - 1:
            //            {
            //                bHaveConfig[(int)EViewRoundIndex.EVRI_Fifth] = true;
            //            }
            //            break;
            //        default:
            //            break;
            //    }
            //}

            In32Frame.gameObject.SetActive(true);
            In16Frame.gameObject.SetActive(true);
            In8Frame.gameObject.SetActive(true);
            In4Frame.gameObject.SetActive(true);
            In2Frame.gameObject.SetActive(true);
            SetFirstRoundScoreState((int)EViewRoundIndex.EVRI_First);
            if (!bHaveNode[(int)EViewRoundIndex.EVRI_First] || !bHaveConfig[(int)EViewRoundIndex.EVRI_First])
            {
                In32Frame.gameObject.SetActive(false);
                SetFirstRoundScoreState((int)EViewRoundIndex.EVRI_Second);
                if (!bHaveNode[(int)EViewRoundIndex.EVRI_Second] || !bHaveConfig[(int)EViewRoundIndex.EVRI_Second])
                {
                    In16Frame.gameObject.SetActive(false);
                    SetFirstRoundScoreState((int)EViewRoundIndex.EVRI_Third);
                    if (!bHaveNode[(int)EViewRoundIndex.EVRI_Third] || !bHaveConfig[(int)EViewRoundIndex.EVRI_Third])
                    {
                        In8Frame.gameObject.SetActive(false);
                        SetFirstRoundScoreState((int)EViewRoundIndex.EVRI_Fourth);
                        if (!bHaveNode[(int)EViewRoundIndex.EVRI_Fourth] || !bHaveConfig[(int)EViewRoundIndex.EVRI_Fourth])
                        {
                            In4Frame.gameObject.SetActive(false);
                            SetFirstRoundScoreState((int)EViewRoundIndex.EVRI_Fifth);
                            if (!bHaveNode[(int)EViewRoundIndex.EVRI_Fifth] || !bHaveConfig[(int)EViewRoundIndex.EVRI_Fifth])
                            {
                                In2Frame.gameObject.SetActive(false);
                            }
                        }
                    }
                }
            }
        }

        // 设置进场提示
        public void SetEntryTime(SingleCompetitionInfo competitionInfo)
        {
            // 最后一轮结算时间
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(m_nLegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;
            int nCupEndTime = 0;
            if (competitionInfo.roundInfo.Count > cupBaseData.nNeedRoundCount - 1)
            {
                nCupEndTime = competitionInfo.roundInfo[cupBaseData.nNeedRoundCount - 1].nRoundEndTime;
            }
            DateTime roundEndTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(nCupEndTime);
            roundEndTime = roundEndTime.ToLocalTime();

            // 线上线下、参赛者、举办者、本轮参赛者、最后一轮节点都结束、线下等待裁决节点
            m_bIsOnline = cupBaseData.nIsOnline > 0;                                                                // 线上线下
            m_bIsInCup = LogicDataCenter.legendCupDataManager.LegendCupSelfInNodeDic.ContainsKey(m_nLegendCupID);// 参赛者
            m_bIsCreater = LogicDataCenter.legendCupDataManager.LegendCupSelfCreateNodeDic.ContainsKey(m_nLegendCupID);// 举办者
            cmd_legendcup_recv_competition_node selfCurRoundNode = new cmd_legendcup_recv_competition_node();// 本轮参赛者
            bool bAllFinalRoundNodeEnd = competitionInfo.headInfo.nIsFinalRound > 0;// 最后一轮节点都结束
            cmd_legendcup_recv_competition_node waitJudgeNode = new cmd_legendcup_recv_competition_node();// 线下等待裁决节点
            foreach (cmd_legendcup_recv_competition_node item in competitionInfo.nodeInfo)
            {
                if (item.nRoundID != competitionInfo.headInfo.nRoundID)
                    continue;

                if (LogicDataCenter.legendCupDataManager.CheckIsSelfInCompetitionMember(item.nKin1ID) || LogicDataCenter.legendCupDataManager.CheckIsSelfInCompetitionMember(item.nKin2ID))
                {
                    selfCurRoundNode = item;
                    m_nCurRoundSelfNodeSearchID = selfCurRoundNode.nSearchID;
                }

                if (competitionInfo.headInfo.nIsFinalRound > 0 && item.byCompetitionNodeState != (int)ECompetitionNodeState.emNodeState_End)
                    bAllFinalRoundNodeEnd = false;

                if (!m_bIsOnline && item.byCompetitionNodeState == (int)ECompetitionNodeState.emNodeState_WaitJudge && waitJudgeNode.nSearchID <= 0)
                    waitJudgeNode = item;
            }

            ///////////////////////////////// 设置提示 //////////////////////////////////////
            if (cupBaseData.nIsEnd > 0)
            {
                //EntryTime.text = "该杯赛已经结束";
                EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "CupIsEnd");
            }
            else
            {
                if (m_bIsInCup)
                {
                    if (bAllFinalRoundNodeEnd)
                    {
                        if (cupBaseData.nIsOnline > 0)
                        {
                            //EntryTime.text = String.Format("杯赛将在{0}月{1}日{2:D2}:{3:D2}最终确认排名并发放奖励", roundEndTime.Month, roundEndTime.Day, roundEndTime.Hour, roundEndTime.Minute);
                            EntryTime.text = String.Format(ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "CupWaitPrize"), roundEndTime.Month, roundEndTime.Day, roundEndTime.Hour, roundEndTime.Minute);
                        }
                        else
                        {
                            //EntryTime.text = "在全部比赛完成后，将最终确认排名并发放奖品";
                            EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "CupWaitEnd");
                        }
                    }
                    else if (selfCurRoundNode.nSearchID > 0)
                    {
                        // 本轮下一场时间
                        DateTime enterTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(selfCurRoundNode.nEnterTime);
                        enterTime = enterTime.ToLocalTime();
                        DateTime beginTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(selfCurRoundNode.nBeginTime);
                        beginTime = beginTime.ToLocalTime();
                        // 下一轮时间
                        DateTime nextRoundEnterTime = new DateTime();
                        DateTime nextRoundBeginTime = new DateTime();
                        if (competitionInfo.roundInfo.Count > competitionInfo.headInfo.nRoundID)
                        {
                            nextRoundEnterTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(competitionInfo.roundInfo[competitionInfo.headInfo.nRoundID].nRoundCreateRoomTme);
                            nextRoundEnterTime = nextRoundEnterTime.ToLocalTime();
                            nextRoundBeginTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(competitionInfo.roundInfo[competitionInfo.headInfo.nRoundID].nRoundCreateRoomTme + competitionInfo.roundInfo[competitionInfo.headInfo.nRoundID].nBeginInterval);
                            nextRoundBeginTime = nextRoundBeginTime.ToLocalTime();
                        }

                        if (m_bIsOnline)
                        {
                            switch (selfCurRoundNode.byCompetitionNodeState)
                            {
                                case (int)ECompetitionNodeState.emNodeState_WaitStart:
                                    {
                                        //EntryTime.text = String.Format("您战队的下一场比赛入场时间为：{0}月{1}日{2:D2}:{3:D2} - {4:D2}:{5:D2}", enterTime.Month, enterTime.Day, enterTime.Hour, enterTime.Minute, beginTime.Hour, beginTime.Minute);
                                        EntryTime.text = String.Format(ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "NodeNextTime"), enterTime.Month, enterTime.Day, enterTime.Hour, enterTime.Minute, beginTime.Hour, beginTime.Minute);
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_CanEnter:
                                    {
                                        //EntryTime.text = String.Format("杯赛已经开启，将在{0:D2}:{1:D2}关闭参赛入口，请尽快参战", beginTime.Hour, beginTime.Minute);
                                        EntryTime.text = String.Format(ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "NodeCanEnter"), beginTime.Hour, beginTime.Minute);
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_Competition:
                                    {
                                        //EntryTime.text = "您的战队正在比赛中";
                                        EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "NodeIsCompetition");
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_WaitJudge:
                                case (int)ECompetitionNodeState.emNodeState_End:
                                    {
                                        if (competitionInfo.headInfo.nIsFinalRound > 0)
                                        {
                                            //EntryTime.text = "你的杯赛已经结束，请等全部比赛结束后发放奖励";
                                            EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "YourCupEnd");
                                        }
                                        else
                                        {
                                            //EntryTime.text = String.Format("您战队的下一场比赛入场时间为：{0}月{1}日{2:D2}:{3:D2} - {4:D2}:{5:D2}", nextRoundEnterTime.Month, nextRoundEnterTime.Day, nextRoundEnterTime.Hour, nextRoundEnterTime.Minute, nextRoundBeginTime.Hour, nextRoundBeginTime.Minute);
                                            EntryTime.text = String.Format(ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "NodeNextTime"), nextRoundEnterTime.Month, nextRoundEnterTime.Day, nextRoundEnterTime.Hour, nextRoundEnterTime.Minute, nextRoundBeginTime.Hour, nextRoundBeginTime.Minute);
                                        }
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        else
                        {
                            switch (selfCurRoundNode.byCompetitionNodeState)
                            {
                                case (int)ECompetitionNodeState.emNodeState_WaitStart:
                                case (int)ECompetitionNodeState.emNodeState_CanEnter:
                                    {
                                        //EntryTime.text = "您可以和对手约好时间，点击参加按钮进入比赛";
                                        EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "YourDealCupTime");
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_Competition:
                                    {
                                        //EntryTime.text = "您的战队正在比赛中";
                                        EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "NodeIsCompetition");
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_WaitJudge:
                                    {
                                        //EntryTime.text = "已经打完该场比赛，请找举办人确认比赛有效";
                                        EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "YourWaitCreaterJudge");
                                    }
                                    break;
                                case (int)ECompetitionNodeState.emNodeState_End:
                                    {
                                        //EntryTime.text = "需要等待本轮全部比赛完成，进入下一轮后才能继续";
                                        EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "YourWaitRoundEnd");
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                    else
                    {
                        //EntryTime.text = "你的杯赛已经结束，请等全部比赛结束后发放奖励";
                        EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "YourCupEnd");
                    }
                }
                else if (m_bIsCreater)
                {
                    if (m_bIsOnline)
                    {
                        if (bAllFinalRoundNodeEnd)
                        {
                            //EntryTime.text = String.Format("杯赛将在{0}月{1}日{2:D2}:{3:D2}最终确认排名并发放奖励", roundEndTime.Month, roundEndTime.Day, roundEndTime.Hour, roundEndTime.Minute);
                            EntryTime.text = String.Format(ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "CupWaitPrize"), roundEndTime.Month, roundEndTime.Day, roundEndTime.Hour, roundEndTime.Minute);
                        }
                        else
                        {
                            //EntryTime.text = "双击比赛节点可以查看比赛";
                            EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "NodeDoubleClickView");
                        }
                    }
                    else
                    {
                        if (waitJudgeNode.nSearchID > 0)
                        {
                            //EntryTime.text = String.Format("{0}VS{1}的比赛需要裁定", waitJudgeNode.nKin1ID, waitJudgeNode.nKin2ID);
                            EntryTime.text = String.Format(ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "NodeWaitJudge"), waitJudgeNode.nKin1ID, waitJudgeNode.nKin2ID);
                        }
                        else
                        {
                            //EntryTime.text = "双击比赛节点可以查看或直接裁定比赛结果";
                            EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "NodeViewOrJudge");
                        }
                    }
                }
                else
                {
                    //EntryTime.text = "当前杯赛无您的比赛，双击比分可查看比赛信息";
                    EntryTime.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "YourNoCup");
                }
            }

        }

        // 更新参赛按钮状态
        private void UpdateJoinMatchBtnState()
        {
            bool bState = false;

            int nCheckNodeRound = m_nViewCurRound == (int)EViewRoundIndex.EVRI_Fifth ? m_nViewCurRound : m_nViewCurRound + 1;
            for (int j = 0; j < (int)EViewGroupCount.EVGC_Max * 2; ++j)
            {
                if (m_kinNodeMatrix[nCheckNodeRound, j] != null && m_kinNodeMatrix[nCheckNodeRound, j].CanJoinMatch)
                {
                    bState = true;
                    break;
                }
            }

            SetJoinMatchBtnState(bState);
        }

        // 设置按钮状态
        public void SetJoinMatchBtnState(bool bState)
        {
            if (m_effectParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_effectParam);
                m_effectParam = null;
            }

            if (bState)
            {
                JoinMatchBtn.interactable = true;

                // 参赛光效
                m_effectParam = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_LegendCupSmallKnockoutView_JoinMatchBtn, _tfAttachParent: AttachPoint, _bAutoDestroy: false);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_effectParam);
            }
            else
            {
                JoinMatchBtn.interactable = false;
            }
        }

        private SKinTemplateData GetKinItemData(long nCupID, cmd_legendcup_recv_competition_node nodeInfo, EDataGetType eGetType)
        {
            SKinTemplateData data = new SKinTemplateData();
            data.nLegendCupID = nCupID;
            data.nRoundID = nodeInfo.nRoundID;
            data.nSearchID = nodeInfo.nSearchID;
            data.nWinnerKinID = nodeInfo.nWinnerKinID;

            switch (eGetType)
            {
                case EDataGetType.EDGT_Kin1:
                    {
                        data.nKinID = nodeInfo.nKin1ID;
                        data.strKinName = nodeInfo.szKin1Name;
                    }
                    break;
                case EDataGetType.EDGT_Kin2:
                    {
                        data.nKinID = nodeInfo.nKin2ID;
                        data.strKinName = nodeInfo.szKin2Name;
                    }
                    break;
                case EDataGetType.EDGT_KinWin:
                    {
                        if (nodeInfo.nWinnerKinID == nodeInfo.nKin1ID)
                        {
                            data.nKinID = nodeInfo.nKin1ID;
                            data.strKinName = nodeInfo.szKin1Name;
                        }
                        else if (nodeInfo.nWinnerKinID == nodeInfo.nKin2ID)
                        {
                            data.nKinID = nodeInfo.nKin2ID;
                            data.strKinName = nodeInfo.szKin2Name;
                        }
                    }
                    break;
                case EDataGetType.EDGT_KinLose:
                    {
                        if (nodeInfo.nWinnerKinID == nodeInfo.nKin1ID)
                        {
                            data.nKinID = nodeInfo.nKin2ID;
                            data.strKinName = nodeInfo.szKin2Name;
                        }
                        else if (nodeInfo.nWinnerKinID == nodeInfo.nKin2ID)
                        {
                            data.nKinID = nodeInfo.nKin1ID;
                            data.strKinName = nodeInfo.szKin1Name;
                        }
                    }
                    break;
                default:
                    break;
            }
            return data;
        }

        // 设置最外一轮节点状态为"战队"，此处只能为EViewGroupCount.EVGC_Round4之前的
        private void SetFirstRoundScoreState(int nFirstViewRound)
        {
            cmd_legendcup_recv_competition_node node = new cmd_legendcup_recv_competition_node();
            node.nRoundID = -1;
            node.nSearchID = -1;

            switch (nFirstViewRound)
            {
                case (int)EViewRoundIndex.EVRI_First:
                    {
                        for (int i = 0; i < (int)EViewGroupCount.EVGC_Round1 * 2; i++)
                        {
                            if (m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_First, i])
                                m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_First, i].SetScore(ULocalizationService.Instance.Get("UIView", "Common", "Team"), node);
                        }
                    }
                    break;
                case (int)EViewRoundIndex.EVRI_Second:
                    {
                        for (int i = 0; i < (int)EViewGroupCount.EVGC_Round2 * 2; i++)
                        {
                            if (m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Second, i])
                                m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Second, i].SetScore(ULocalizationService.Instance.Get("UIView", "Common", "Team"), node);
                        }
                    }
                    break;
                case (int)EViewRoundIndex.EVRI_Third:
                    {
                        for (int i = 0; i < (int)EViewGroupCount.EVGC_Round3 * 2; i++)
                        {
                            if (m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Third, i])
                                m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Third, i].SetScore(ULocalizationService.Instance.Get("UIView", "Common", "Team"), node);
                        }
                    }
                    break;
                case (int)EViewRoundIndex.EVRI_Fourth:
                    {
                        for (int i = 0; i < (int)EViewGroupCount.EVGC_Round4 * 2; i++)
                        {
                            if (m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Fourth, i])
                                m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Fourth, i].SetScore(ULocalizationService.Instance.Get("UIView", "Common", "Team"), node);
                        }
                    }
                    break;
                case (int)EViewRoundIndex.EVRI_Fifth:
                    {
                        m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Fifth, (int)ESpecialItemIndex.ESII_WinGroup1].SetScore(ULocalizationService.Instance.Get("UIView", "Common", "Team"), node);
                        m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Fifth, (int)ESpecialItemIndex.ESII_WinGroup2].SetScore(ULocalizationService.Instance.Get("UIView", "Common", "Team"), node);
                        m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Fifth, (int)ESpecialItemIndex.ESII_LoseGroup1].SetScore(ULocalizationService.Instance.Get("UIView", "Common", "Team"), node);
                        m_kinNodeMatrix[(int)EViewRoundIndex.EVRI_Fifth, (int)ESpecialItemIndex.ESII_LoseGroup2].SetScore(ULocalizationService.Instance.Get("UIView", "Common", "Team"), node);
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class LegendCupSmallKnockDetailFrame : UCustomUIBehaviour
    {
        public Text PrizeTitleDes;
        public URichText ViewOtherPrizeRT;
        public RectTransform PersonalPrizeFrame;
        public GameObject PrizeItemTemplate;

        private long m_LegendCupID;
       
        protected override void Awake()
        {
            base.Awake();

            PrizeTitleDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "Pos1Prize");
            ViewOtherPrizeRT.text = "<a href=CompetitionAward color=ffca27>" + ULocalizationService.Instance.Get("UIView", "LegendCupGroup", "CompetitionAward") + "</a>";
            ViewOtherPrizeRT.onHrefClick.AddListener(OnClickCompetitionAwardRT);
        }

        public void SetCupPrizeData(long lLegendCupID)
        {
            m_LegendCupID = lLegendCupID;
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(lLegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;

            // 冠军奖励配置
            SSchemeLegendCupConfig cupConfig = LegendCupConfig.Instance.GetCupConfig(cupBaseData.nCupConfigID);
            if (cupConfig == null)
                return;
            List<SSchemeLegendCupPrizeConfig> prizeConfigList = LegendCupConfig.Instance.GetCupPrizeConfig(cupConfig.nPrizeConfigID);
            if (prizeConfigList == null)
                return;
            SSchemeLegendCupPrizeConfig Pos1PrizeConfig = null;
            foreach (var item in prizeConfigList)
            {
                if (item.nStartPos == 1)
                {
                    Pos1PrizeConfig = item;
                    break;
                }
            }
            if (Pos1PrizeConfig == null)
                return;

            int nPersonalCount = PersonalPrizeFrame.childCount;
            for (int i = nPersonalCount - 1; i >= 0; i--)
            {
                Transform personalRect = PersonalPrizeFrame.GetChild(i);
                if (personalRect)
                {
                    var temp = personalRect.gameObject;
                    ResNode.DestroyRes(ref temp);
                }
            }

            // 最多只显示3个
            for (int i = 0; i < Pos1PrizeConfig.nPrizeIDList.Length && i < 3; i++)
            {
                GameObject PersonalPrizeObj = ResNode.InstantiateRes(PrizeItemTemplate);
                if (!PersonalPrizeObj)
                    return;
                RectTransform rectPersonal = PersonalPrizeObj.transform as RectTransform;
                rectPersonal.SetParent(PersonalPrizeFrame, false);
                LegendCupPrizeItem personalPrizeItems = PersonalPrizeObj.GetComponent<LegendCupPrizeItem>();
                if (!personalPrizeItems)
                    return;
                personalPrizeItems.SetData(Pos1PrizeConfig.nPrizeIDList[i], ECupPrizeType.ECPT_Personal, UEffectPrefabType.UEPT_None, false);
            }
            PrizeItemTemplate.SetActive(false);
        }

        public void OnClickCompetitionAwardRT(string eventName)
        {
            if (eventName == "CompetitionAward")
            {
                LegendCupIDData uiData = new LegendCupIDData();
                uiData.nLegendCupID = m_LegendCupID;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_PRIZE_SHOW, uiData);
            }
        }
    }

    public class LegendCupSmallKnockRoundStartDataItem : UCustomUIBehaviour
    {
        public Text RoundDes;
        public Text RoundStartDate;
        public Text RoundStartTime;
        public Text StartDes;
        public Text OfflineDes;
        public Image CurRoundIcon;
        public Image NextIcon;

        public void SetData(cmd_legendcup_recv_competition_roundinfo roundInfo , int nCurRound, bool bShowNextIcon)
        {
            RoundDes.text = roundInfo.szRoundName;
            int nRoundBeginTime = roundInfo.nRoundCreateRoomTme + roundInfo.nBeginInterval;
            if (nRoundBeginTime > 0)
            {
                DateTime roundTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(nRoundBeginTime);
                roundTime = roundTime.ToLocalTime();
                RoundStartDate.text = roundTime.Month.ToString("D2") + ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "Month") + roundTime.Day.ToString("D2") + ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "Day");
                RoundStartTime.text = String.Format("{0:D2}:{1:D2}", roundTime.Hour, roundTime.Minute);
                StartDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "StartCup");
                RoundStartDate.gameObject.SetActive(true);
                RoundStartTime.gameObject.SetActive(true);
                StartDes.gameObject.SetActive(true);
                OfflineDes.gameObject.SetActive(false);
            }
            else
            {
                OfflineDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "OfflineDes");
                RoundStartDate.gameObject.SetActive(false);
                RoundStartTime.gameObject.SetActive(false);
                StartDes.gameObject.SetActive(false);
                OfflineDes.gameObject.SetActive(true);
            }
            CurRoundIcon.gameObject.SetActive(roundInfo.nRoundID == nCurRound);
            NextIcon.gameObject.SetActive(bShowNextIcon);
            
            this.gameObject.SetActive(true);
        }
    }

    public class LegendCupSmallKnockRoundStartDataFrame : UCustomUIBehaviour
    {
        public RectTransform RoundStartDataFrame;
        public GameObject RoundStartDataTemplate;

        public void SetData(SingleCompetitionInfo competitionInfo)
        {
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(competitionInfo.headInfo.nLegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;

            // 销毁RoundStartDataFrame内容
            int nCount = RoundStartDataFrame.childCount;
            for (int i = nCount - 1; i >= 0; --i)
            {
                Transform childTrans = RoundStartDataFrame.GetChild(i);
                if (childTrans)
                {
                    var temp1 = childTrans.gameObject;
                    ResNode.DestroyRes(ref temp1);
                }
            }

            // 线上赛根据下发轮次信息显示，线下赛自己根据基础数据计算
            if (cupBaseData.nIsOnline > 0)
            {
                // 设置Item
                foreach (cmd_legendcup_recv_competition_roundinfo item in competitionInfo.roundInfo)
                {
                    // 与Frame一样，去掉(非淘汰赛)信息
                    SCupTypeKey sOnlineKey = new SCupTypeKey(cupBaseData.nCupConfigID, item.nRoundID);
                    SSchemeLegendCupDetailConfig cupTypeOnlineConfig = LegendCupConfig.Instance.GetCupDetailConfig(sOnlineKey);
                    if (cupTypeOnlineConfig == null)
                        continue;
                    if (cupTypeOnlineConfig.nCompetitionType != (int)ERoundCompetitionType.emType_KnockoutCompetition)
                        continue;

                    if (item.nRoundCreateRoomTme + item.nBeginInterval > 0)
                    {
                        cmd_legendcup_recv_competition_roundinfo temp = item;
                        temp.szRoundName = cupTypeOnlineConfig.sRoundDes;
                        AddRoundTemplate(temp, competitionInfo.headInfo.nRoundID, !(item.nRoundID == competitionInfo.roundInfo.Count));
                    }
                }
            }
            else
            {
                // 设置Item
                for (int j = cupBaseData.nBeginRound; j < cupBaseData.nBeginRound + cupBaseData.nNeedRoundCount; j++)
                {
                    SCupTypeKey sOfflineKey = new SCupTypeKey(cupBaseData.nCupConfigID, j);
                    SSchemeLegendCupDetailConfig cupTypeOfflineConfig = LegendCupConfig.Instance.GetCupDetailConfig(sOfflineKey);
                    if (cupTypeOfflineConfig == null)
                        continue;
                    if (cupTypeOfflineConfig.nCompetitionType != (int)ERoundCompetitionType.emType_KnockoutCompetition)
                        continue;

                    cmd_legendcup_recv_competition_roundinfo offLineInfo = new cmd_legendcup_recv_competition_roundinfo();
                    offLineInfo.nRoundCreateRoomTme = 0;
                    offLineInfo.nBeginInterval = 0;
                    offLineInfo.nRoundID = j;
                    offLineInfo.szRoundName = cupTypeOfflineConfig.sRoundDes;
                    AddRoundTemplate(offLineInfo, cupBaseData.nCurrentRound, !(j == cupBaseData.nBeginRound + cupBaseData.nNeedRoundCount - 1));
                }
            }

            RoundStartDataTemplate.gameObject.SetActive(false);
        }

        private void AddRoundTemplate(cmd_legendcup_recv_competition_roundinfo roundInfo, int nCurRound, bool bShowNextIcon)
        {
            GameObject roundStartItemObj = ResNode.InstantiateRes(RoundStartDataTemplate);
            if (!roundStartItemObj)
                return;
            RectTransform rectTrans = roundStartItemObj.transform as RectTransform;
            rectTrans.SetParent(RoundStartDataFrame, false);

            LegendCupSmallKnockRoundStartDataItem roundStartItem = roundStartItemObj.GetComponent<LegendCupSmallKnockRoundStartDataItem>();
            if (!roundStartItem)
                return;
            roundStartItem.SetData(roundInfo, nCurRound, bShowNextIcon);
        }
    }

    public class LegendCupSmallKnockoutWndView : UIBaseWndView
    {
        public Text TitleDes;
        public Text CreaterDes;
        public URichText CupSmallKnockoutTip;
        public Text ViewCompetitionBtnDes;
        public Text ReturnBtnDes;
        public LegendCupSmallKnockNodeFrame smallKnockNodeFrame;
        public LegendCupSmallKnockDetailFrame smallKnockDetailFrame;
        public LegendCupSmallKnockRoundStartDataFrame smallKnockRoundDataFrame;

        private long m_LegendCupID;
        private int m_createScoreNodeRoundId;
        private int m_createScoreNodeSearchId;

        // 标题光效
        public GameObject AttachPoint1;
        public GameObject AttachPoint2;
        private UEffectParamBase param1;
        private UEffectParamBase param2;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            ReturnBtnDes.text = ULocalizationService.Instance.Get("UIView", "Common", "Return");
            ViewCompetitionBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "ViewCompetitionBtn");
            CupSmallKnockoutTip.text = "<a href=CupSmallKnockoutTip color=ffca27>" + ULocalizationService.Instance.Get("UIView", "LegendCupSmallKnockout", "SmallKnockoutTip") + "</a>";
            CupSmallKnockoutTip.onHrefClick.AddListener(OnClickCupSmallKnockoutRT);

            smallKnockNodeFrame.InitView(this);

            if (param1 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param1);
                param1 = null;
            }
            param1 = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_LegendCupListView_ConfineFrame_beisai01, _tfAttachParent: AttachPoint1.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param1);
            if (param2 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param2);
                param2 = null;
            }
            param2 = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_LegendCupListView_ConfineFrame_beisai02, _tfAttachParent: AttachPoint2.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param2);

            return true;
        }

        public void OnShowSmallKnockoutWnd(long nCupID)
        {
            m_LegendCupID = nCupID;
            SingleCompetitionInfo competitionInfo = LogicDataCenter.legendCupDataManager.GetSingleCompetitionInfo(m_LegendCupID);
            if (competitionInfo == null)
                return;
            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(nCupID);
            if (cupBaseData.nLegendCupID == 0)
                return;

            // 数据赋值部分
            TitleDes.text = cupBaseData.szCupName;
            CreaterDes.gameObject.SetActive(!String.IsNullOrEmpty(cupBaseData.szCreateActorName));
            CreaterDes.text = string.Format("举办人：{0}", cupBaseData.szCreateActorName);
            smallKnockNodeFrame.SetData(nCupID, competitionInfo);
            smallKnockDetailFrame.SetCupPrizeData(m_LegendCupID);
            smallKnockRoundDataFrame.SetData(competitionInfo);
        }

        public void OnClickCupSmallKnockoutRT(string eventName)
        {
            if (eventName == "CupSmallKnockoutTip")
            {
                cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(m_LegendCupID);
                if (cupBaseData.nLegendCupID == 0)
                    return;

                UIMsgCmdData uiCupTipData = null;
                if (cupBaseData.nIsOnline > 0)
                    uiCupTipData = new UIMsgCmdData((int)WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW, (int)LegendCupTipType.LCTT_OnLine_Cup16In, "", IntPtr.Zero, 0);
                else
                    uiCupTipData = new UIMsgCmdData((int)WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW, (int)LegendCupTipType.LCTT_OffLine_Cup16In, "", IntPtr.Zero, 0);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_MSGBOXTIP_SHOW, uiCupTipData);
            }
        }

        public void SetSelectCompetitionNodeID(int nCreateScoreNodeRoundId, int nCreateScoreNodeSearchId)
        {
            m_createScoreNodeRoundId = nCreateScoreNodeRoundId;
            m_createScoreNodeSearchId = nCreateScoreNodeSearchId;
        }

        public void OnClickViewCompetitionBtn()
        {
            // 未选中
            if (m_createScoreNodeRoundId < 0 || m_createScoreNodeSearchId < 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_SELECT_ANY_COMPETITION_NODE);
                return;
            }
            // 选中，无数据
            if (m_createScoreNodeRoundId == 0 || m_createScoreNodeSearchId == 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_MATCH_CANT_VIEW_WAR);
                return;
            }

            LegendCupNodeWarData nodeData = new LegendCupNodeWarData();
            nodeData.nLegendCupID = m_LegendCupID;
            nodeData.nRoundID = m_createScoreNodeRoundId;
            nodeData.nSearchID = m_createScoreNodeSearchId;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LEGENDCUP_WARRECORD_CUPNODE_SHOW, nodeData);
        }

        public void OnClickCloseBtn()
        {
            cmd_legendcup_quit_observe observeData = new cmd_legendcup_quit_observe();
            observeData.nLegendCupID = m_LegendCupID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_quit_observe>(GameLogicDef.GVIEWCMD_LEGENDCUP_QUITOBSERVE, ref observeData);

            if (param1 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param1);
                param1 = null;
            }
            if (param2 != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param2);
                param2 = null;
            }

            m_wnd.UnloadView();
        }
    }
}
