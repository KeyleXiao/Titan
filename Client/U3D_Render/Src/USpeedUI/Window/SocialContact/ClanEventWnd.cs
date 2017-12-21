using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using DataCenter;
using UIWidgets;
using DG.Tweening;
using USpeedUI.Social;

namespace USpeedUI.ClanEvent
{
    public class ClanEventWnd : UIBaseSubWnd<ClanEventWndView>
    {
        // 获取ID
        public override int GetWndType()
        {
            return (int)ClanListFrameWndType.WND_CLAN_EVENT;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewClanEvent";
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            return true;
        }

        // 设置是否可见
        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_CLAN_RECV_EVENTLIST:
                    {
                        if (m_wndView != null)
                            m_wndView.OnUpdateClanEventList();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class ClanEventWndView : UIBaseSubWndView
    {
        public UListClanEvent ListClanEvent;

        public override bool Init(IUISubWnd wnd)
        {
            return base.Init(wnd);
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);

            if (visible)
            {
                OnUpdateClanEventList();
            }
        }

        public void OnUpdateClanEventList()
        {
            ListClanEvent.DataSource.BeginUpdate();
            ListClanEvent.DataSource.Clear();
            List<cmd_clan_eventlist> EventList = LogicDataCenter.clanDataManager.ClanEventList;
            for (int i = 0; i < EventList.Count; ++i)
            {
                cmd_clan_eventlist cmdNodeData = EventList[i];
                DateTime tTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(cmdNodeData.nTime);
                tTime = tTime.ToLocalTime();
                UListItemClanEvent item = new UListItemClanEvent();
                item.ClanEventDes = tTime.Year + "/" + tTime.Month + "/" + tTime.Day + "/" + tTime.Hour.ToString().PadLeft(2, '0') + ":" + tTime.Minute.ToString().PadLeft(2, '0') + "  "
                                    + cmdNodeData.szCoutext;
                ListClanEvent.DataSource.Add(item);
            }

            ListClanEvent.DataSource.EndUpdate();
        }
    }
}
