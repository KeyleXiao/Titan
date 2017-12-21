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
using ASpeedGame.Data.ClanConfig;
using U3D_Render.Common;

namespace USpeedUI.ClanTip
{
    public class ClanTipWnd : UIBaseSubWnd<ClanTipWndView>
    {
        // 获取ID
        public override int GetWndType()
        {
            return (int)ClanListFrameWndType.WND_CLAN_TIP;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "Social/UViewClanTip";
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

    public class ClanTipWndView : UIBaseSubWndView
    {
        public Text ClanTip;

        public override bool Init(IUISubWnd wnd)
        {
            string sClanTip = ClanConfig.Instance.tClanConfig.sClanTip;
            if (!sClanTip.Equals(string.Empty))
            {
                UBB.toHtml(ref sClanTip, UBB_FORMAT_TYPE.UGUI);
                ClanTip.text = sClanTip;
            }

            return base.Init(wnd);
        }

        public override void SetVisible(bool visible)
        {
            base.SetVisible(visible);
        }
    }
}
