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
using DataCenter;

namespace USpeedUI.Guide
{
    public class PrizeTipGuideWnd : UIPopupWnd<PrizeTipGuideWndView>
    {
        private PrizeTipGuideWndView view { get { return m_wndView as PrizeTipGuideWndView; } }

        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_PRIZETIPGUIDE;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewPrizeTipGuide";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 预先载入
            //LoadView();
            SetVisible(false);

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this); 
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_PRIZETIP_UPDATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_MAILBOX_OPEN, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_PRIZETIP_UPDATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_MAILBOX_OPEN, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        LogicDataCenter.prizeTipDataManager.RemoveAllItem();
                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        LogicDataCenter.prizeTipDataManager.RemoveAllItem();
                        this.UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_MAIN_SHOW:
                    {
                        if(m_wndView != null)
                        {
                            if (m_wndView.PrizeTipType == EMPrizeTipType.EMPrizeTipType_Task)
                                SetVisible(false);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_MAILBOX_OPEN:
                    {
                        if (m_wndView != null)
                        {
                            if (m_wndView.PrizeTipType == EMPrizeTipType.EMPrizeTipType_Mail)
                                SetVisible(false);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_PRIZETIP_UPDATE:
                    {
                        // 战场中不显示任务奖励
                        if (GameLogicAPI.isInWarScene() > 0)
                            return;

                        PrizeTipData data = msgData as PrizeTipData;
                        switch (data.eTipType)
                        {
                            case EMPrizeTipType.EMPrizeTipType_Task:
                                {
                                    if (UISystem.Instance.IsWndVisible(WndID.WND_ID_TASK_MAIN))
                                        return;
                                }
                                break;
                            case EMPrizeTipType.EMPrizeTipType_Mail:
                                {
                                    if (UISystem.Instance.IsWndVisible(WndID.WND_ID_MAILBOX))
                                        return;
                                }
                                break;
                            default:
                                break;
                        }

                        SetVisible(true);

                        if (m_wndView != null)
                        {
                            m_wndView.OnUpdatePrizeTip(msgData as PrizeTipData);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

    }

    public class PrizeTipGuideWndView : UIBaseWndView
    {
        //public Text prizeTitleText;
        public Image prizeTipImage;
        public Text goBtnText;

        public EMPrizeTipType PrizeTipType
        {
            get
            {
                if (m_PrizeTipData == null)
                    return EMPrizeTipType.EMPrizeTipType_Invalid;
                return m_PrizeTipData.eTipType;
            }
        }
        private PrizeTipData m_PrizeTipData;

        public override bool Init(IUIWnd wnd)
        {
            m_PrizeTipData = null;

            goBtnText.text = ULocalizationService.Instance.Get("UIView", "PrizeTip", "GoBtnTextDesc");

            return base.Init(wnd);
        }

        public void OnUpdatePrizeTip(PrizeTipData tipData)
        {
            m_PrizeTipData = tipData;
            int nTipType = (int)tipData.eTipType;
            //prizeTitleText.text = ULocalizationService.Instance.Get("UIView", "PrizeTip", "PrizeTipTitle_" + nTipType.ToString());
        }

        public void OnGoBtnClick()
        {
            if (m_PrizeTipData != null)
            {
                LogicDataCenter.prizeTipDataManager.OnPrizeTipGuide(m_PrizeTipData.eTipType);
                LogicDataCenter.prizeTipDataManager.RemoveItem(m_PrizeTipData);
            }

            m_PrizeTipData = null;

            if (!LogicDataCenter.prizeTipDataManager.ShowNextPrizeTipView())
                UISystem.Instance.ShowWnd(WndID.WND_ID_PRIZETIPGUIDE, false);            
        }

        public void OnColseBtnClick()
        {
            if (m_PrizeTipData != null)
                LogicDataCenter.prizeTipDataManager.RemoveItem(m_PrizeTipData);

            m_PrizeTipData = null;

            if (!LogicDataCenter.prizeTipDataManager.ShowNextPrizeTipView())
                UISystem.Instance.ShowWnd(WndID.WND_ID_PRIZETIPGUIDE, false); 
        }
    }
}
