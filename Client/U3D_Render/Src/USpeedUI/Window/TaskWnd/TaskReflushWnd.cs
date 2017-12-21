using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using DataCenter;
using GameLogic;
using Data.PointShopConfig;
using U3D_Render.Common;

namespace USpeedUI.TaskWnd
{
    // 任务刷新界面Wnd
    class TaskReflushWnd : UIBaseWnd<TaskReflushWndView>
    {
        // 获取WndID
        public override WndID GetID()
        {
            return WndID.WND_ID_TASK_REFLUSH;
        }

        // 获取预制体路径
        public override string GetPrefabPath()
        {
            return "Task/TaskReflushView";
        }

        // 获取UI所在层
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();
            m_isKeyClose = true;

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_REFLUSH_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_REFLUSH_UPDATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_REFLUSH_CLOSE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_CLOSE_OTHER, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_REFLUSH_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_REFLUSH_UPDATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_REFLUSH_CLOSE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_CLOSE_OTHER, this);

        }

        // 接收消息
        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_TASK_REFLUSH_SHOW:
                    {
                        if (IsVisible())
                            return;

                        SetVisible(true);
                        if (m_wndView != null)
                        {
                            m_wndView.OnShowReflushWnd();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_REFLUSH_UPDATE:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnShowReflushWnd();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_REFLUSH_CLOSE:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnClickCloseBtn();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_MAIN_CLOSE_OTHER:
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

        // ESC退出
        public override bool OnCloseKey()
        {
            base.OnCloseKey();

            if (m_wndView != null)
            {
                m_wndView.OnClickCloseBtn();
            }

            return true;
        }
    }

    // 任务刷新界面View
    public class TaskReflushWndView : UIBaseWndView, ITimerHanlder
    {
        public Text TitleDesc;
        public Text TipDesc;
        public Text BtnDesc;
        public Text LeftTimeDesc;
        public Text MoneyTitleDesc;
        public Text MoneyDesc;
        public Text CountDesc;

        private int m_nLeftTime;

        // 初始化
        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            TitleDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "TipText");
            string szTip = ULocalizationService.Instance.Get("UIView", "Task", "ReflushTip");
            UBB.toHtml(ref szTip);
            TipDesc.text = szTip;
            BtnDesc.text = ULocalizationService.Instance.Get("UIView", "Task", "ReflushNow");
            MoneyTitleDesc.text = ULocalizationService.Instance.Get("UIView", "Task", "ReflushMoney");

            return true;
        }

        // 显示刷新界面
        public void OnShowReflushWnd()
        {
            float fElapseTime = Time.unscaledTime - LogicDataCenter.taskDataManager.RecvTime;
            m_nLeftTime = LogicDataCenter.taskDataManager.ReflushLeftTime - (int)fElapseTime;
            SetReflushTime();
            TimerManager.KillTimer(this, 1);
            TimerManager.SetTimer(this, 1, 1.0f);

            CountDesc.text = String.Format(ULocalizationService.Instance.Get("UIView", "Task", "TodayRemainCount"), LogicDataCenter.taskDataManager.RemainReflushCount);

            SSchemePointShopGoodsConfig config = null;
            List<SSchemePointShopGoodsConfig> configList = PointShopConfig.Instance.GetGoodsListByType(PointShopGoodsType.Goods_Special);
            if (configList == null || configList.Count <= 0)
                return;
            foreach (var item in configList)
            {
                if (item.nConsumpApproach == (int)PointShopConsumpApproach.Approach_TaskOpt_Reflush)
                {
                    config = item;
                    break;
                }
            }
            if (config == null)
                return;
            MoneyDesc.text = config.nPointPrice.ToString();
        }

        // 定时响应
        public virtual void OnTimer(int nTimerID)
        {
            m_nLeftTime--;
            SetReflushTime();
        }

        // 设置时间
        private void SetReflushTime()
        {
            m_nLeftTime = m_nLeftTime <= 0 ? 0 : m_nLeftTime;
            int nHour = m_nLeftTime / 3600;
            int nMin = m_nLeftTime % 3600 / 60;
            int nSec = m_nLeftTime % 60;
            LeftTimeDesc.text = String.Format("{0:D2}: {1:D2}: {2:D2}", nHour, nMin, nSec);
        }

        // 刷新响应
        public void OnClickReflushBtn()
        {
            // 点券不足
            SSchemePointShopGoodsConfig config = null;
            List<SSchemePointShopGoodsConfig> configList = PointShopConfig.Instance.GetGoodsListByType(PointShopGoodsType.Goods_Special);
            if (configList == null || configList.Count <= 0)
                return;
            foreach (var item in configList)
            {
                if (item.nConsumpApproach == (int)PointShopConsumpApproach.Approach_TaskOpt_Reflush)
                {
                    config = item;
                    break;
                }
            }
            if (config == null)
                return;

            int nCurPoint = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_TICKET);
            if (config.nPointPrice > 0 && config.nPointPrice > nCurPoint)
            {
                UConsumptionMsgData cmdData = new UConsumptionMsgData((int)EConsumptionRequestType.Request_BuyGoods, (int)EConsumptionGuideType.Guide_Point);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_CONSUMPTIONGUIDE_SHOW, cmdData);
                return;
            }

            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_REFLUSH_OPT_TASK, 0, null);
        }

        // 关闭响应
        public void OnClickCloseBtn()
        {
            TimerManager.KillTimer(this, 1);
            SetVisible(false);
        }
    }
}
