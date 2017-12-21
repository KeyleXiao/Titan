/*******************************************************************
** 文件名:	UMediatorTeamRight.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-8-8
** 版  本:	1.0
** 描  述:	限时物品管理限时界面
		
********************************************************************/
using System.Collections.Generic;
using GameLogic;
using System;
using System.Runtime.InteropServices;
using DataCenter;
using USpeedUI.TooltipAndDialog;
using UnityEngine;
using System.Collections;
using U3D_Render;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
namespace USpeedUI.TimeThresholdGoodsPart
{
    public enum TimeThresholdGoodsType 
    {
        TIMETHRESHOLDGOODSTYPE_SKIN,
    }

    public enum TimeThresholdGoodsTimerID
    {
        TimerID_Update_TimeThresholdView,
    }

    public class TimeThresholdGoodsWnd : UIPopupWnd<TimeThresholdGoodsView>, ITimerHanlder
    {
        private Dictionary<TimeThresholdGoodsType, int> dicGoodTypeTimeThreshold = new Dictionary<TimeThresholdGoodsType, int>();
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_TIMETHRESHOLDVIEW;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "TimeThresholdGood/TimeThresholdGoodView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01; ;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HEROSKINVIEW_UPDATESKINDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HEROSKINVIEW_INITSKINDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            
            SetGoodTypeTimeThreshold(TimeThresholdGoodsType.TIMETHRESHOLDGOODSTYPE_SKIN);
            TimerManager.SetTimer(this, (int)TimeThresholdGoodsTimerID.TimerID_Update_TimeThresholdView, 3600f);// 一个小时更新一次
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HEROSKINVIEW_UPDATESKINDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HEROSKINVIEW_INITSKINDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            
            TimerManager.KillTimer(this, (int)TimeThresholdGoodsTimerID.TimerID_Update_TimeThresholdView);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        if (m_wndView != null)
                        {
                            RefurbishListView(TimeThresholdGoodsType.TIMETHRESHOLDGOODSTYPE_SKIN);
                            return;
                        }

                        this.LoadView();
                        RefurbishListView(TimeThresholdGoodsType.TIMETHRESHOLDGOODSTYPE_SKIN);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        this.SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        TimerManager.KillTimer(this, (int)TimeThresholdGoodsTimerID.TimerID_Update_TimeThresholdView);
                        this.UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_HEROSKINVIEW_INITSKINDATA:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        RefurbishListView(TimeThresholdGoodsType.TIMETHRESHOLDGOODSTYPE_SKIN);
                    }
                    break;
                case WndMsgID.WND_MSG_HEROSKINVIEW_UPDATESKINDATA:
                    {
                        if (m_wndView == null)
                        {
                            return;
                        }

                        RefurbishListView(TimeThresholdGoodsType.TIMETHRESHOLDGOODSTYPE_SKIN);
                    }
                    break;       
                default:
                    break;
            }
        }

        private void RefurbishListView(TimeThresholdGoodsType type)
        {
            this.SetVisible(true);
            m_wndView.ClearTypeThresholdGoodList(type);
            switch (type)
            {
                case TimeThresholdGoodsType.TIMETHRESHOLDGOODSTYPE_SKIN:
                    {
                        Dictionary<int, List<cmd_Hero_Skin_Info>> SkinData = LogicDataCenter.heroSkinDataManager.HeroSkinData;
                        if (SkinData == null)
                        {
                            this.SetVisible(false);
                            return;
                        }

                        foreach(int nHeroId in SkinData.Keys)
                        {
                            foreach(cmd_Hero_Skin_Info info in SkinData[nHeroId])
                            {
                                if (!CheckGoodIsInTimeThreshold(type, info.dwDurationTime))
                                {
                                    continue;
                                }

                                SkinInfoScheme skinConfig = new SkinInfoScheme();
                                if (!GameLogicAPI.GetHeroSkinInfo(info.nSkinId, ref skinConfig))
                                {
                                    continue;
                                }

                                UListTimeThresholdData data = new UListTimeThresholdData();
                                data.nGoodId = info.nSkinId;
                                data.nGoodIconId = nHeroId;//skinConfig.nSkinIconID;
                                data.sGoodName[0] = "皮肤  :";
                                data.sGoodName[1] = skinConfig.szSkinName;
             
                                data.szParam = "英雄";
                                data.nParam = nHeroId;
                                data.nGoodThresholdTime = info.dwDurationTime;
                                m_wndView.AddThresholdGoodListItem(type,data);
                            }
                        }                   
                    }
                    break;
                default:
                    break;
            }

            bool bShow = m_wndView.SetThresholdGoodListDataSource();
            this.SetVisible(bShow);
        }

        private void AddThresholdGoodListItem()
        {
            throw new NotImplementedException();
        }

        private bool CheckGoodIsInTimeThreshold(TimeThresholdGoodsType type, uint dwDurationTime)
        {
            if (!dicGoodTypeTimeThreshold.ContainsKey(type))
            {
                return false;
            }

            DateTime calcTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(dwDurationTime);
            calcTime = calcTime.ToLocalTime();

            TimeSpan ts = calcTime.Subtract(DateTime.Now).Duration();
            int nDay = ts.Days;
            if (dwDurationTime > 0 && nDay < dicGoodTypeTimeThreshold[type])
            {
                return true;
            }
            else
            {
                if (dwDurationTime < 0)
                {
                    Debug.LogError("this skin is overdue");
                }

                return false;
            }
        }

        private void SetGoodTypeTimeThreshold(TimeThresholdGoodsType timeThresholdGoodsType)
        {
            dicGoodTypeTimeThreshold[timeThresholdGoodsType] = 7;
        }

        public virtual void OnTimer(int nTimerID)
        {
            RefurbishListView(TimeThresholdGoodsType.TIMETHRESHOLDGOODSTYPE_SKIN);
        }
    }
}
