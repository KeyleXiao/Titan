using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using UnityEngine.UI;
using GameLogic;
using DataCenter;
using LegendCup_ManagedDef;

namespace USpeedUI.LegendCup
{
    public class LegendCupOnlineResetWnd : UIPopupWnd<LegendCupOnlineResetWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_ONLINE_RESET;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupOnlineResetView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_ONLINE_RESET_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_ONLINE_RESET_CLOSE, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_ONLINE_RESET_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_ONLINE_RESET_CLOSE, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_ONLINE_RESET_SHOW:
                    {
                        SetVisible(true);

                        LegendCupOnlineResetData cmdData = (LegendCupOnlineResetData)data;
                        if (m_wndView != null)
                            m_wndView.OnAddLegendCup(cmdData);
                    }
                    break;
                case WndMsgID.WND_MSG_LEGENDCUP_ONLINE_RESET_CLOSE:
                    {
                        if (m_wndView != null)
                            UnloadView();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class LegendCupOnlineResetWndView : UIBaseWndView
    {
        public Text TitleDes;
        public Text ResetScoreDes;
        public Text ResetTimeDes;
        public Text Kin1Des;
        public Text Kin2Des;
        public Text ScoreDes;
        public Text MonthDes;
        public Text DayDes;
        public Text HourDes;
        public Text MinuteDes;
        public Text ConfirmBtnDes;
        public Text CancelBtnDes;
        public Text ResetEndTimeDes;

        public InputField Kin1ScoreInputField;
        public InputField Kin2ScoreInputField;
        public InputField MonthInputField;
        public InputField DayInputField;
        public InputField HourInputField;
        public InputField MinuteInputField;

        private LegendCupOnlineResetData m_dataInfo;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);
            TitleDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupOnlineReset", "TitleDes");
            ResetScoreDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupOnlineReset", "ResetScoreDes");
            ResetTimeDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupOnlineReset", "ResetTimeDes");
            ScoreDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupOnlineReset", "ScoreDes");
            MonthDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupOnlineReset", "MonthDes");
            HourDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupOnlineReset", "HourDes");
            MinuteDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupOnlineReset", "MinuteDes");
            ConfirmBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupOnlineReset", "ConfirmBtnDes");
            CancelBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupOnlineReset", "CancelBtnDes");

            return true;
        }

        public void OnAddLegendCup(LegendCupOnlineResetData dataInfo)
        {
            m_dataInfo = dataInfo;
            // 初始化其他数据
            Kin1Des.text = dataInfo.szKin1Name;
            Kin2Des.text = dataInfo.szKin2Name;

            // 设置为最后可重置比赛的时间点
            DateTime resetEndTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(dataInfo.nResetEndTime);
            resetEndTime = resetEndTime.ToLocalTime();
            ResetEndTimeDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupOnlineReset", "CancelBtnDes")+ resetEndTime.ToString(" MM-dd HH:mm");
            MonthInputField.placeholder.GetComponent<Text>().text = resetEndTime.Month.ToString();
            DayInputField.placeholder.GetComponent<Text>().text = resetEndTime.Day.ToString();
            HourInputField.placeholder.GetComponent<Text>().text = resetEndTime.Hour.ToString();
            MinuteInputField.placeholder.GetComponent<Text>().text = resetEndTime.Minute.ToString();
            Kin1ScoreInputField.placeholder.GetComponent<Text>().text = "0";
            Kin2ScoreInputField.placeholder.GetComponent<Text>().text = "0";
        }

        public void OnClickConfirmBtn()
        {

            DateTime newDateTime;
            if (CheckInputTime(out newDateTime) == false)
            {
                return;
            }

            // todo 可以选择时间 和 得分
            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                                    title: ULocalizationService.Instance.Get("UIView", "LegendCupOnlineReset", "OnlineResetTitle"),
                                    message: ULocalizationService.Instance.Get("UIView", "LegendCupOnlineReset", "OnlineResetContent"),
                                    buttons: new UIWidgets.DialogActions() { { ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Confirm"), DoOnlienResetCup }, { ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Cancel"), null } }
                                    );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);

        }

        private bool DoOnlienResetCup()
        {
            DateTime newDateTime;
            if (CheckInputTime(out newDateTime) == false)
            {
                return false;
            }

            cmd_legendcup_recv_cuplist_node cupBaseData = LogicDataCenter.legendCupDataManager.GetSingleLegendCupNode(m_dataInfo.nLegendCupID);
            if (cupBaseData.nLegendCupID == 0)
                return false;
            cmd_legendcup_judge_competition_node judgedInfo = new cmd_legendcup_judge_competition_node();
            judgedInfo.nLegendCupID = m_dataInfo.nLegendCupID;									// 杯赛ID
            judgedInfo.nRoundID = m_dataInfo.nRoundID;										    // 轮次ID
            judgedInfo.nSearchID = m_dataInfo.nSearchID;										// 节点ID
            judgedInfo.byJudgeFlag = (byte)EJudgeNodeWar.JUDGE_Reset;							// EJudgeNodeWar 
            // 获取重置比赛的时间

            System.DateTime startTime = TimeZone.CurrentTimeZone.ToLocalTime(new System.DateTime(1970, 1, 1));
            judgedInfo.lResetNewTime = (long)(newDateTime - startTime).TotalSeconds;
            judgedInfo.nKin1 = m_dataInfo.nKin1ID;                                         // 队伍1
            judgedInfo.nKin1Score = Convert.ToInt32(Kin1ScoreInputField.text == string.Empty ? Kin1ScoreInputField.placeholder.GetComponent<Text>().text : Kin1ScoreInputField.text);
            
            judgedInfo.nKin2 = m_dataInfo.nKin2ID;                                         // 队伍2
            judgedInfo.nKin2Score = Convert.ToInt32(Kin2ScoreInputField.text == string.Empty ? Kin2ScoreInputField.placeholder.GetComponent<Text>().text : Kin2ScoreInputField.text); 						                        // 队伍2得分

            ViewEventHelper.Instance.SendCommand<cmd_legendcup_judge_competition_node>(GameLogicDef.GVIEWCMD_LEGENDCUP_JUDGE_COMPETITION_NODE, ref judgedInfo);

            return true;
        }

        public bool CheckInputTime(out DateTime newDateTime)
        {
            //检查时间输入是否正确
            DateTime resetEndTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(m_dataInfo.nResetEndTime);
            resetEndTime = resetEndTime.ToLocalTime();

            int nYear = resetEndTime.Year;
            int nInputMonth = Convert.ToInt32(MonthInputField.text == string.Empty ? MonthInputField.placeholder.GetComponent<Text>().text : MonthInputField.text);
            int nInputDay = Convert.ToInt32(DayInputField.text == string.Empty ? DayInputField.placeholder.GetComponent<Text>().text : DayInputField.text);
            int nInputHour = Convert.ToInt32(HourInputField.text == string.Empty ? HourInputField.placeholder.GetComponent<Text>().text : HourInputField.text);
            int nInputMinute = Convert.ToInt32(MinuteInputField.text == string.Empty ? MinuteInputField.placeholder.GetComponent<Text>().text : MinuteInputField.text);
            string inputDate = nYear.ToString() + "/" + nInputMonth.ToString() + "/" + nInputDay.ToString() + " " + nInputHour.ToString().PadLeft(2, '0') + ":" + nInputMinute.ToString().PadLeft(2, '0') + ":00";

            if (!DateTime.TryParse(inputDate, out newDateTime))
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_PLEASE_INPUT_RIGHT_TIME);
                return false;
            }

            if (DateTime.Compare(resetEndTime, newDateTime) < 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_RESET_TIME_LARGE_THAN_END_RESETTIME);
                return false;
            }

            return true;
        }

        public void OnClickCancelBtn()
        {
            //SetVisible(false);
            m_wnd.UnloadView();
        }

        public void OnClickCloseBtn()
        {
            //SetVisible(false);
            m_wnd.UnloadView();
        }
    }
}
