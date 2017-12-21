using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using U3D_Render;
using UnityEngine.UI;

namespace USpeedUI.WarAutoMove
{
    public class WarAutoMoveWnd : UIBaseWnd<WarAutoMoveWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_AUTOMOVE;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override string GetPrefabPath()
        {
            return UPath.UDynamicMobaState + "/WarAutoMove/WarAutoMoveView";
        }

        public override bool Init()
        {
            //订阅显示自动寻路标识事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_AUTOMOVE_SHOW,this);
            //订阅隐藏自动寻路标识事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_AUTOMOVE_HIDE, this);
            //订阅离开战场事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            //订阅战场结束事件
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            //退订显示自动寻路标识事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_AUTOMOVE_SHOW, this);
            //退订隐藏自动寻路标识事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_AUTOMOVE_HIDE, this);
            //退订离开战场事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            //退订战场结束事件
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_WAR_AUTOMOVE_SHOW:
                    {
                        this.LoadView();
                        this.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_AUTOMOVE_HIDE:
                    {
                        this.SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_WAREND:
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                    {
                        if (m_wndView != null)
                        {
                            this.UnloadView();
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class WarAutoMoveWndView : UIBaseWndView
    {
        public Image AutoMove;

        /// <summary>
        /// 自动寻路图片Y方向偏移基础值
        /// </summary>
        public int AutoMoveEffectYPosOffsetBase = 200;

        /// <summary>
        /// 标准屏幕宽高
        /// </summary>
        public const int StandScreenWidth = 1920;
        public const int StandScreenHeight = 1080;

        private CreatureProperty creatureProperty;

        public override bool Init(IUIWnd wnd)
        {
            float offset = 0;

            creatureProperty = EntityFactory.MainHero.GetComponent<CreatureProperty>();

            if (creatureProperty)
            {
                offset = creatureProperty.CreatureBoundingRad.y * AutoMoveEffectYPosOffsetBase;
            }
            Vector2 tmpAnchoredPos = AutoMove.rectTransform.anchoredPosition;
            tmpAnchoredPos.y += offset;
            AutoMove.rectTransform.anchoredPosition = tmpAnchoredPos;

            return base.Init(wnd);
        }
    }
}
