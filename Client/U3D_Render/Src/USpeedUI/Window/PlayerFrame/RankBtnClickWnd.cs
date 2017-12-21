/*******************************************************************
** 文件名:	RankBtnClickWnd.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	2016-7-13
** 版  本:	1.0
** 描  述:	rank按钮点击窗体
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using DataCenter;
using U3D_Render.Common;
using USpeedUI.UExtensions;
namespace USpeedUI.RankBtnClickInfo
{

    public class URankBtnClickInfoWnd : UIPopupWnd<URankBtnClickInfoWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_RANK_CLICK;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "PlayerInfo/RankBtnClickView";
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

            // 订阅消息

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RANK_CLICK_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RANK_CLICK_CLOSE, this);

            
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RANK_CLICK_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RANK_CLICK_CLOSE, this);

            
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_RANK_CLICK_SHOW:
                    {
                        if (m_wndView == null)
                        {
                            LoadView();
                        }
                        m_wndView.SetVisible(true);

                    }
                    break;

                case WndMsgID.WND_MSG_RANK_CLICK_CLOSE:
                    {
                        if (m_wndView == null)
                            return;
                        m_wndView.SetVisible(false);
                    }
                    break;

                case WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER:
                    {
                        if (m_wndView == null)
                            return;
                        m_wndView.SetVisible(false);
                    }
                    break;

                default:
                    break;
            }
        }
    }

    public class URankBtnClickInfoWndView : UIBaseWndView
    {
        public Text Title;
        public Text DetailDes;
        public Text StartDes;
        public Text CancleDes;
        public URichText InviteDes;

        public override bool Init(IUIWnd wnd)
        {

            Title.text = ULocalizationService.Instance.Get("UIView", "RankBtnClickInfo", "Title");

            string strTip = ULocalizationService.Instance.Get("UIView", "RankBtnClickInfo", "DetailDes");
            UBB.toHtml(ref strTip);
            DetailDes.text = strTip;

            StartDes.text = ULocalizationService.Instance.Get("UIView", "RankBtnClickInfo", "StartBtnDes");
            CancleDes.text = ULocalizationService.Instance.Get("UIView", "RankBtnClickInfo", "CancleBtnDes");

            InviteDes.text = "<a href=InviteDesClick color=" + UDefines.CommomColor_Hex (ECommonColor.ECC_Gold1)+ ">" + ULocalizationService.Instance.Get("UIView", "RankBtnClickInfo", "InviteDes") + "</a>";
            InviteDes.onHrefClick.AddListener(OnClickInviteDesRT);
            InviteDes.UnderLineColor = UDefines.CommomColor(ECommonColor.ECC_Gold1);

            return base.Init(wnd);
        }

        public void OnClickInviteDesRT(string eventName)
        {
            if (eventName == "InviteDesClick")
            {
                if (LogicDataCenter.taskDataManager.CTaskGuideManager != null)
                {
                    LogicDataCenter.taskDataManager.CTaskGuideManager.OnClanFirstWinGuideHandler();
                }
                SetVisible(false);
            }
        }

        public void OnStartButtonClick()
        {
            // 开始排位比赛
            if (LogicDataCenter.viewPersonStateDataManager.bIsMatching == true)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_MATCH_PLEASE_QUIT_MATCH);
                return;
            }
            LogicDataCenter.matchDataManager.OnRankModeBtnClick();

            SetVisible(false);
        }

        public void OnCancleButtonClick()
        {
            SetVisible(false);
        }
    }
}
