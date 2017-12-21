using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using ASpeedGame.Data.ShieldWorld;

namespace USpeedUI.MsgBoxCreateClan
{
    public class MsgBoxCreateClanWnd : UIBaseWnd<MsgBoxCreateClanWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_CLAN_MSGBOXCREATECLAN;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewMsgBoxCreateClan";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FILL;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            SetVisible(false);

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_CLAN_CREATESUCCEED, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_CLAN_CREATESUCCEED, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_CLAN_CREATESUCCEED:
                    {
                        // 隐藏窗体
                        SetVisible(false);
                    }
                    break;
                default:
                    break;
            }
        }

    }

    // 创建帮会旗帜按钮标签类型
    public enum CreateClanQiZhiButtonType
    {
        BTN_QIZHI1,             // 旗帜1
        BTN_QIZHI2,             // 旗帜2
        BTN_QIZHI3,             // 旗帜3
        BTN_QIZHI4,             // 旗帜4
        BTN_QIZHI5,             // 旗帜5
        BTN_QIZHI6,             // 旗帜6
        BTN_QIZHI7,             // 旗帜7
        BTN_QIZHI8,             // 旗帜8
    }

    public class MsgBoxCreateClanWndView : UIBaseWndView
    {
        public Text createClanTitleText;
        public Text clanNameDescText;
        public InputField clanName;
        public Text clanManifestoDescText;
        public InputField clanManifesto;
        public Text clanFlagDescText;
        public Text clanTotalDescText;
        public Text clanButtonDescText;

        private float m_fLastClickCreateTime;

        private CreateClanQiZhiButtonType m_curButtonIndex;
        public UButtonGroup buttonGroup;

        // 旗帜索引
        private int m_nFlagIndex = 0;

        public override bool Init(IUIWnd wnd)
        {
            createClanTitleText.text = ULocalizationService.Instance.Get("UIView", "ClanCreate", "ClanCreateTitle");
            clanNameDescText.text = ULocalizationService.Instance.Get("UIView", "ClanCreate", "ClanNameDesc");
            clanManifestoDescText.text = ULocalizationService.Instance.Get("UIView", "ClanCreate", "ClanManifestoDesc");
            clanFlagDescText.text = ULocalizationService.Instance.Get("UIView", "ClanCreate", "ClanFlagDesc");
            clanTotalDescText.text = ULocalizationService.Instance.Get("UIView", "ClanCreate", "ClanTotalDesc");
            clanButtonDescText.text = ULocalizationService.Instance.Get("UIView", "ClanCreate", "ClanCreateBtn");

            m_curButtonIndex = CreateClanQiZhiButtonType.BTN_QIZHI1;

            return base.Init(wnd);
        }

        public void OnCreateBtnClick()
        {
            if (clanName.text.Length <= 0)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CLAN_NAME_IS_EMPTY);
                return;
            }

            // 判敏感字符
            List<string> shieldStrList = new List<string>();
            if (ShieldWord.Instance.IsHaveShieldWordEx(clanName.text, out shieldStrList))
            {
                if (shieldStrList != null && shieldStrList.Count > 0)
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CLAN_CREATE_NAME_SHIELD_WORD, shieldStrList[0]);
                return;
            }
            shieldStrList.Clear();
            if (ShieldWord.Instance.IsHaveShieldWordEx(clanManifesto.text, out shieldStrList))
            {
                if (shieldStrList != null && shieldStrList.Count > 0)
                    UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_CLAN_MANIFESTO_SHIELD_WORD, shieldStrList[0]);
                return;
            }

            // 每秒只让点一次
            if (Time.unscaledTime < m_fLastClickCreateTime + 1f)
                return;
            m_fLastClickCreateTime = Time.unscaledTime;

            GameLogic.cmd_clan_create cmdData = new GameLogic.cmd_clan_create();
            cmdData.szClanName = clanName.text;
            cmdData.szManifesto = clanManifesto.text;
            cmdData.nClanFlag = m_nFlagIndex;
            ViewEventHelper.Instance.SendCommand<cmd_clan_create>(GameLogicDef.GVIEWCMD_CLAN_CREATE, ref cmdData);
        }

        public void OnColseBtnClick()
        {
            // 关闭该窗体
            UISystem.Instance.ShowWnd(WndID.WND_ID_CLAN_MSGBOXCREATECLAN, false);
        }

        // 设置是否可见
        internal override void SetRendererVisible(bool visible)
        {
            base.SetRendererVisible(visible);

            if (visible)
            {
                SetButtonSelect((int)m_curButtonIndex);
            }
        }

        public void SetButtonSelect(int nIndex)
        {
            buttonGroup.SetItemSelectedItem(nIndex, true);
        }

        public void chooseButtonSelect(int nIndex)
        {
            // 按钮点击事件
            m_nFlagIndex = nIndex;
        }
    }
}
