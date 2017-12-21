using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using ASpeedGame.Data.MentorshipConfig;
using Mentorship_ManagerDef;
using ASpeedGame.Data.ShieldWorld;

namespace USpeedUI.MsgBoxMasterIssue
{
    public class MsgBoxMasterIssueWnd : UIBaseWnd<MsgBoxMasterIssueWndView>
    {
        private MsgBoxMasterIssueWndView view { get { return m_wndView as MsgBoxMasterIssueWndView; } }

        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_MASTER_MSGBOXISSUE;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewMsgBoxMasterIssue";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_OVERHEAD;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            // 预先载入
            //LoadView();
            SetVisible(false);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                default:
                    break;
            }
        }

    }

    public class MasterIssueTransformFrame : TimeCountDown
    {
        public Button issueBtn;
        public Text issueBtnText;
        public Text cancelBtnText;
        public InputField issueInfo;
        public Text issueLenText;

        private float m_fLastClickTime;

        public bool Init()
        {
            issueBtnText.text = ULocalizationService.Instance.Get("UIView", "MasterIssue", "IssueBtnDesc");
            cancelBtnText.text = ULocalizationService.Instance.Get("UIView", "MasterIssue", "CancelBtnDesc");

            issueInfo.onValueChanged.AddListener(OnIssueInfoChange);

            return true;
        }

        private void OnIssueInfoChange(string strText)
        {
            issueLenText.text = "字数：" + strText.Length.ToString() + "/32";
        }

        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();

            if ((int)fCurTime > 0)
                issueBtnText.text = ((int)fCurTime).ToString();
        }

        protected override void CountDownCompleteCallBack()
        {
            base.CountDownCompleteCallBack();
            SetIssueBtnState(true);
        }

        public void OnIssueBtnClick()
        {
            if (issueInfo.text.Length <= 0)
                return;

            // 每10秒只让点一次
            if (Time.unscaledTime < m_fLastClickTime + 10f)
                return;

            m_fLastClickTime = Time.unscaledTime;

            GameLogic.cmd_mentorship_request_issue_info cmdData = new GameLogic.cmd_mentorship_request_issue_info();

            cmdData.nIssueType = (int)EMMentorshipIssueType.emMentorshipIssueType_Master;
            cmdData.szIssueInfo = issueInfo.text;

            // 处理屏蔽词
            ShieldWord.Instance.filterShieldWord(ref cmdData.szIssueInfo);

            ViewEventHelper.Instance.SendCommand<cmd_mentorship_request_issue_info>(GameLogicDef.GVIEWCMD_MENTORSHIP_REQUESTISSUEINFO, ref cmdData);

            this.StartCountDown((float)30000 / 1000, 1.0f, 1.0f);
            issueBtnText.text = ((int)fCurTime).ToString();
            SetIssueBtnState(false);

            UISystem.Instance.ShowWnd(WndID.WND_ID_MASTER_MSGBOXISSUE, false);
        }

        public void OnCancelBtnClick()
        {
            // 关闭该窗体
            UISystem.Instance.ShowWnd(WndID.WND_ID_MASTER_MSGBOXISSUE, false);
        }

        private void SetIssueBtnState(bool bState)
        {
            if (bState)
                issueBtnText.text = ULocalizationService.Instance.Get("UIView", "MasterIssue", "IssueBtnDesc");

            Color clr = new Color();
            string htmlString = (bState ? "#ffffff" : "#bebebe");
            ColorUtility.TryParseHtmlString(htmlString, out clr);
            issueBtn.interactable = bState;
            issueBtnText.color = clr;  
        }
    }

    public class MsgBoxMasterIssueWndView : UIBaseWndView
    {
        public Text issueTitleText;
        public MasterIssueTransformFrame MasterIssueFrame;

        public override bool Init(IUIWnd wnd)
        {
            issueTitleText.text = ULocalizationService.Instance.Get("UIView", "MasterIssue", "IssueTitleDesc");
            MasterIssueFrame.Init();
            return base.Init(wnd);
        }

        public void OnColseBtnClick()
        {
            // 关闭该窗体
            UISystem.Instance.ShowWnd(WndID.WND_ID_MASTER_MSGBOXISSUE, false);
        }
    }
}
