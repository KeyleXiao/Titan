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

namespace USpeedUI.ClanTask
{
    public class ClanTaskWnd : UIBaseSubWnd<ClanTaskWndView>
    {
        // 获取ID
        public override int GetWndType()
        {
            return (int)ClanListFrameWndType.WND_CLAN_TASK;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewClanTask";
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
        }
    }

    public class ClanTaskWndView : UIBaseSubWndView
    {
        public override bool Init(IUISubWnd wnd)
        {
            return base.Init(wnd);
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
        }
    }
}
