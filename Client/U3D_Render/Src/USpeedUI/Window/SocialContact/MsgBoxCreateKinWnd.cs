using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using ASpeedGame.Data.KinConfig;
using ASpeedGame.Data.ShieldWorld;

namespace USpeedUI.MsgBoxCreateKin
{
    public class MsgBoxCreateKinWnd : UIBaseWnd<MsgBoxCreateKinWndView>
    {
        private MsgBoxCreateKinWndView view { get { return m_wndView as MsgBoxCreateKinWndView; } }

        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_KIN_MSGBOXCREATEKIN;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewMsgBoxCreateKin";
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

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_KIN_CREATESUCCESS, this);
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_KIN_CREATESUCCESS, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_KIN_CREATESUCCESS:
                    {
                        // 隐藏窗体
                        view.SetVisible(false);
                    }
                    break;
                default:
                    break;
            }
        }

    }

    public class MsgBoxCreateKinWndView : UIBaseWndView
    {
        public Transform KinNameText;
        public Text CreateBtnDes;
        public Text QuitBtnDes;
        public Text SysDes;
        public Text CreateWarningInfoDes;
        public Text NameDes;
        public Text LabelDes;
        public Transform KinLabelText;

        public override bool Init(IUIWnd wnd)
        {
            CreateBtnDes.text = ULocalizationService.Instance.Get("UIView", "CreateKin", "CreateBtnDes");
            QuitBtnDes.text = ULocalizationService.Instance.Get("UIView", "CreateKin", "QuitBtnDes");
            SysDes.text = ULocalizationService.Instance.Get("UIView", "CreateKin", "SysDes");
            CreateWarningInfoDes.text = ULocalizationService.Instance.Get("UIView", "CreateKin", "CreateWarningInfoDes");
            NameDes.text = ULocalizationService.Instance.Get("UIView", "CreateKin", "NameDes");
            LabelDes.text = ULocalizationService.Instance.Get("UIView", "KinMember", "KinLabelSubTitle");
            return base.Init(wnd);
        }

        public override void Start()
        {
            // 设置提示信息
            if (CreateWarningInfoDes == null)
            {
                return;
            }
            string test = CreateWarningInfoDes.text;
            test = test.Replace("{1}", KinConfig.Instance.tKinConfig.nCreateNeedLevel.ToString());
            test = test.Replace("{2}", KinConfig.Instance.tKinConfig.nCreateNeedMoney.ToString());
            CreateWarningInfoDes.text = test;
        }

        public void OnCreateBtnClick()
        {
            InputField KinName = KinNameText.GetComponent<InputField>();
            InputField KinLabel = KinLabelText.GetComponent<InputField>();
            // 判空
            if (KinName.textComponent.text == string.Empty)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_KIN_CREATE_NAME_ISEMPTY);
                return;
            }
            if (KinLabel.textComponent.text == string.Empty)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_KIN_CREATE_LABEL_ISEMPTY);
                return;
            }

            // 判敏感字符
            List<string> shieldStrList = new List<string>();
            if (ShieldWord.Instance.IsHaveShieldWordEx(KinName.textComponent.text, out shieldStrList))
            {
                if (shieldStrList != null && shieldStrList.Count > 0)
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_KIN_NAME_HAS_SHIELD_WORLD, shieldStrList[0]);
                return;
            }
            shieldStrList.Clear();
            if (ShieldWord.Instance.IsHaveShieldWordEx(KinLabel.textComponent.text, out shieldStrList))
            {
                if (shieldStrList != null && shieldStrList.Count > 0)
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_KIN_LABEL_HAS_SHIELD_WORLD, shieldStrList[0]);
                return;
            }

            GameLogic.cmd_kin_create cmdData = new GameLogic.cmd_kin_create();
            cmdData.szKinName = KinName.textComponent.text;
            cmdData.szKinLabel = KinLabel.textComponent.text;
            ViewEventHelper.Instance.SendCommand<cmd_kin_create>(GameLogicDef.GVIEWCMD_KIN_CREATE, ref cmdData);
        }

        public void OnColseBtnClick()
        {
            // 关闭该窗体
            UISystem.Instance.ShowWnd(WndID.WND_ID_KIN_MSGBOXCREATEKIN, false);
        }
    }
}
