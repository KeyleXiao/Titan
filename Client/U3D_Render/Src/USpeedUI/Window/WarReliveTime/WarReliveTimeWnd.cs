/*******************************************************************
** 文件名:	WarReliveTimeWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016-6-22
** 版  本:	1.0
** 描  述:	战场复活时间界面逻辑模块(接发消息)+视图模块(显示数据)	
********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI.WarReliveTime
{
    public class WarReliveTimeCountDown : TimeCountDown
    {
        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();

            this.gameObject.transform.FindChild("NumText").gameObject.GetComponent<Text>().text = ((int)fCurTime).ToString();
        }

        protected override void CountDownCompleteCallBack()
        {
            base.CountDownCompleteCallBack();

            this.gameObject.SetActive(false);
        }

        public void RunCountDownCompleteCallBack()
        {
            CountDownCompleteCallBack();
        }
    }

    public class UWarReliveTimeWnd : UIPopupWnd<UWarReliveTimeWndView>  
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_RELIVETIME;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "DynamicState_Moba/WarReliveTimeView/WarReliveTimeView";
        }

        public override bool Init()
        {
            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARRELIVETIME_REMOVERELIVETIME, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);

			return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARRELIVETIME_REMOVERELIVETIME, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
		}

		public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        this.LoadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        this.UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_WARRELIVETIME_SHOWRELIVETIME:
                    {
                        UIWarShowReliveTimeData data = msgData as UIWarShowReliveTimeData;
                        if (m_wndView == null || !data.isHero)
                            return;

                        m_wndView.ShowReliveTime(data.nReliveTime);
                    }
                    break;
                case WndMsgID.WND_MSG_WARRELIVETIME_REMOVERELIVETIME:
                    {
                        if (m_wndView == null)
                            return;

                        m_wndView.RemoveReliveTime();
                    }
                    break;
                default:
                    break;
            }
        }

    }

    public class UWarReliveTimeWndView : UIBaseWndView
    {
        public Text ReliveTime;
        public Text TitleText;
        public Text UnitText;

        public override bool Init(IUIWnd wnd)
        {
            TitleText.text = ULocalizationService.Instance.Get("UIView", "WarReliveTime", "Title");
            UnitText.text = ULocalizationService.Instance.Get("UIView", "WarReliveTime", "Unit");

            return base.Init(wnd);
        }

        public void ShowReliveTime(int nReliveTime)
        {
            ReliveTime.text = nReliveTime.ToString();
            SetVisible(true);

            this.gameObject.GetComponent<WarReliveTimeCountDown>().StartCountDown(nReliveTime, 1.0f, 1.0f);
        }

        public void RemoveReliveTime()
        {
            this.gameObject.GetComponent<WarReliveTimeCountDown>().RunCountDownCompleteCallBack();
        }
    }
}
