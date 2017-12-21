using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using UnityEngine;
using UnityEngine.UI;
using GameLogic;
using LegendCup_ManagedDef;

namespace USpeedUI.LegendCup
{
    public class LegendCupJudgeWnd : UIPopupWnd<LegendCupJudgeWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LEGENDCUP_JUDGE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "LegendCup/LegendCupJudgeView";
        }

        public override bool Init()
        {
            m_isKeyClose = true;
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_JUDGE_SHOW, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LEGENDCUP_JUDGE_SHOW, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_LEGENDCUP_JUDGE_SHOW:
                    {
                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            LegendCupJudgeWarData cmdData = (LegendCupJudgeWarData)data;
                            m_wndView.OnShowJudgeWnd(cmdData);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

    }

    public class LegendCupJudgeWndView : UIBaseWndView
    {
        public Text TitleDes;
        public Text Kin1WinDes;
        public Text Kin2WinDes;
        public Text AllKnockoutDes;
        public Text ConfirmBtnDes;
        public Text CancelBtnDes;
        public UButtonGroup NodeResult;
        public GameObject Kin1WinGo;
        public GameObject Kin2WinGo;
        public GameObject AllKnockoutGo;

        private LegendCupJudgeWarData m_judegeData;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);
            TitleDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupJudge", "Title");
            AllKnockoutDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupJudge", "AllKnockout");
            ConfirmBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupJudge", "ConfirmBtn");
            CancelBtnDes.text = ULocalizationService.Instance.Get("UIView", "LegendCupJudge", "CancelBtn");

            NodeResult.SetAllItemDeSelected();
            //NodeResult.SetItemSelectedItem(0);
            
            return true;
        }

        public void OnShowJudgeWnd(LegendCupJudgeWarData judgeData)
        {
            m_judegeData = judgeData;
            Kin1WinDes.text = String.Format("{0}<color=#a5dbef>{1}</color>", ULocalizationService.Instance.Get("UIView", "LegendCupJudge", "WinKin"), judgeData.szKin1Name);
            Kin2WinDes.text = String.Format("{0}<color=#a5dbef>{1}</color>", ULocalizationService.Instance.Get("UIView", "LegendCupJudge", "WinKin"), judgeData.szKin2Name);
            Kin1WinGo.SetActive(false);
            Kin2WinGo.SetActive(false);
            AllKnockoutGo.SetActive(false);
        }

        public void OnClickConfirmBtn()
        {
            // 未选择裁决结果
            if (NodeResult.SelectedIndex < 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LEGENDCUP_NOT_SELECT_CANT_JUDGE_WAR);
                return;
            }

            EJudgeNodeWar byJudgeFlag = EJudgeNodeWar.JUDGE_NONE;
            int nWinID = 0;
            string strResult = null;
            switch (NodeResult.SelectedIndex)
            {
                case 0:
                    {
                        byJudgeFlag = EJudgeNodeWar.JUDGE_SelectWinner;
                        nWinID = m_judegeData.nKin1ID;
                        strResult = m_judegeData.szKin1Name + ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Win");
                    }
                    break;
                case 1:
                    {
                        byJudgeFlag = EJudgeNodeWar.JUDGE_SelectWinner;
                        nWinID = m_judegeData.nKin2ID;
                        strResult = m_judegeData.szKin2Name + ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Win");
                    }
                    break;
                case 2:
                    {
                        byJudgeFlag = EJudgeNodeWar.JUDGE_AllKnockOut;
                        nWinID = 0;
                        strResult = ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Abstain");
                    }
                    break;
                default:
                    break;
            }

            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData(
                        title: ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "ConfirmTitle"),
                        message: string.Format(ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "ConfirmContent"), m_judegeData.szKin1Name, m_judegeData.szKin2Name, strResult),
                        buttons: new UIWidgets.DialogActions() { { ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Confirm"), () => { DoJudgeCup(byJudgeFlag, nWinID); return true; } }, { ULocalizationService.Instance.Get("UIView", "LegendCupWarRecord", "Cancel"), null } }
                        );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        private bool DoJudgeCup(EJudgeNodeWar judgeResult, int nWinID)
        {
            cmd_legendcup_judge_competition_node node = new cmd_legendcup_judge_competition_node();
            node.nLegendCupID = m_judegeData.nLegendCupID;	// 杯赛ID
            node.nRoundID = m_judegeData.nRoundID;			// 轮次ID
            node.nSearchID = m_judegeData.nSearchID;		// 节点ID
            node.nBoCount = m_judegeData.nBoCount;          // 第几场
            node.llWarID = m_judegeData.nWarID;				// 比赛ID
            node.byJudgeFlag = (byte)judgeResult;			// EJudgeNodeWar 
            node.dwWinnerKinID = nWinID;
            ViewEventHelper.Instance.SendCommand<cmd_legendcup_judge_competition_node>(GameLogicDef.GVIEWCMD_LEGENDCUP_JUDGE_COMPETITION_NODE, ref node);

            m_wnd.UnloadView();

            return true;
        }


        public void OnGroupKin1WinClick(bool bActive)
        {
            Kin1WinGo.SetActive(bActive);
            Kin2WinGo.SetActive(!bActive);
            AllKnockoutGo.SetActive(!bActive);
        }

        public void OnGroupKin2WinClick(bool bActive)
        {
            Kin1WinGo.SetActive(!bActive);
            Kin2WinGo.SetActive(bActive);
            AllKnockoutGo.SetActive(!bActive);
        }

        public void OnGroupAllKnockClick(bool bActive)
        {
            Kin1WinGo.SetActive(!bActive);
            Kin2WinGo.SetActive(!bActive);
            AllKnockoutGo.SetActive(bActive);
        }

        public void OnClickCancelBtn()
        {
            m_wnd.UnloadView();
        }

        public void OnClickCloseBtn()
        {
            m_wnd.UnloadView();
        }
    }
}
