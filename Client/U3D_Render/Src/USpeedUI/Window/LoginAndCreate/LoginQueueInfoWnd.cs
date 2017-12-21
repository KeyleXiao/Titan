/*******************************************************************
** 文件名:	LoginQueueInfoWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	2016-7-13
** 版  本:	1.0
** 描  述:	登陆排队	
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using DataCenter;
using GameLogic;
namespace USpeedUI.LoginQueueInfo
{

    public class ULoginQueueInfoWnd : UIPopupWnd<ULoginQueueInfoWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_LOGINQUEUEINFO;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Login/LoginQueueOrderInfoView";
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
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOADSCENE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_OPEN_LOGINQUEUE, this);
            
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_OPEN_LOGINQUEUE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOADSCENE, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_OPEN_LOGINQUEUE:
                    {
                        if (m_wndView == null || !m_wndView.IsVisible())
                        {
                            SetVisible(true);
                        }

                        UIMsgCmdData uData = msgData as UIMsgCmdData;
                        cmd_open_queue_wnd cmdData = IntPtrHelper.toData<cmd_open_queue_wnd>(uData.ptrParam);
                        if (m_wndView != null)
                            m_wndView.updateLoginQueueInfo(cmdData.order, cmdData.RemainTime);
                    }
                    break;

                case WndMsgID.WND_MSG_COMMON_LOADSCENE:
                    {
                        SetVisible(false);
                    }
                    break;
                
                default:
                    break;
            }
        }
    }

    public class ULoginQueueInfoWndView : UIBaseWndView
    {
        public Text Title;
        public Text RemainTimeTitle;
        public Text RemainTime;
        public Text OrderTitle;
        public Text Order;
        

        public override bool Init(IUIWnd wnd)
        {
            Title.text = ULocalizationService.Instance.Get("UIView", "LoginQueueInfo", "Title");
            RemainTimeTitle.text = ULocalizationService.Instance.Get("UIView", "LoginQueueInfo", "RemainTimeTitle");
            OrderTitle.text = ULocalizationService.Instance.Get("UIView", "LoginQueueInfo", "OrderTitle");
            
            return base.Init(wnd);
        }

        public void updateLoginQueueInfo(int nOrder, int nRemainTime)
        {
           //更新界面信息
            RemainTime.text = nRemainTime == 0 ? ULocalizationService.Instance.Get("UIView", "LoginQueueInfo", "RemainTime0") : nRemainTime.ToString();
            Order.text = nOrder.ToString();
        }
       
    }
}
