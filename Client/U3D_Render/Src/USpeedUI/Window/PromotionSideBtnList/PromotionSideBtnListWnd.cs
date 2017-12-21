using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace USpeedUI.PromotionSideBtnList
{
    /// <summary>
    /// 活动侧边栏按钮组
    /// </summary>
    public class PromotionSideBtnListWnd : UIBaseWnd<PromotionSideBtnListWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_MSG_PROMOTION_SIDE_BTN_LIST;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override string GetPrefabPath()
        {
            return UPath.UStaticState + "/PromotionSideBtnList/PromotionSideBtnList";
        }

        public override bool Init()
        {
            base.Init();
            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        SetVisible(false);
                    }
                    break;
            }
        }
    }

    public class PromotionSideBtnListWndView : UIBaseWndView
    {
        public void onPromotionClick()
        {
            UISystem.Instance.ShowWnd(WndID.WND_ID_GAME_PROMOTION, true);
        }
    }

}
