using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine.UI;
using UnityEngine;
using UnityEngine.EventSystems;
using Stage_ManagerDef;

namespace USpeedUI.WarMapSignal
{
    public class WarMapSignalWnd : UIBaseWnd<WarMapSignalWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_MAPSIGNAL;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "DynamicState_Moba/WarMinimapView/MapCommandSignal";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_OVERHEAD;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MAPSIGNAL_SETSELECTSIGNAL, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WAR_MAPSIGNAL_MINIMAPDRAGEND, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MAPSIGNAL_SETSELECTSIGNAL, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WAR_MAPSIGNAL_MINIMAPDRAGEND, this);
        }

        // 设置是否可见
        protected override void PreSetVisible(bool _bVisible)
        {
            base.PreSetVisible(_bVisible);
            if (_bVisible)
            {
                if (StageManager.Instance.getCurrentState() != EStage_State.ESS_BATTLE)
                {
                    return;
                }
            }
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_WAR_MAPSIGNAL_SETSELECTSIGNAL:
                    {
                        if (m_wndView != null)
                            m_wndView.SetSelectSignal(msgData as MapSignalSetSelectSignal);
                    }
                    break;
                case WndMsgID.WND_MSG_WAR_MAPSIGNAL_MINIMAPDRAGEND:
                    {
                        if (m_wndView != null)
                            m_wndView.OnMinimapDragEnd();
                    }
                    break;
                default:
                    break;
            }
        }
    }

    public class WarMapSignalWndView : UIBaseWndView
    {
        public Image highlightImage;
        public Image mouseLineImage;

        private float m_fMouseLineOriginLen;
        private Vector3 m_startPos;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            m_fMouseLineOriginLen = mouseLineImage.rectTransform.sizeDelta.x * UISystem.Instance.GetCanvas().scaleFactor;
            return true;
        }

        internal override void SetRendererVisible(bool visible)
        {
            if (visible)
            {
                RectTransform parentTrans = transform.parent as RectTransform;
                m_startPos = MouseCtrl.Instance.CommandStartScreenPos;
                Vector2 localPos;
                RectTransformUtility.ScreenPointToLocalPointInRectangle(parentTrans, m_startPos, UISystem.Instance.GetCamera(), out localPos);
                RectTransform selfTrans = transform as RectTransform;
                selfTrans.anchoredPosition = localPos;
            }

            base.SetRendererVisible(visible);
        }

        public override void Update()
        {
            base.Update();

            Vector2 vec = InputManager.MousePosition - m_startPos;
            float rot = (float)Math.Atan2(vec.y, vec.x);
            float fLen = vec.magnitude;
            if (fLen > m_fMouseLineOriginLen * 2f)
                fLen = m_fMouseLineOriginLen * 2f;

            mouseLineImage.rectTransform.localScale = new Vector3(fLen / m_fMouseLineOriginLen, 1f, 1f);
            mouseLineImage.rectTransform.eulerAngles = new Vector3(0f, 0f, rot * Mathf.Rad2Deg);
        }

        public void SetSelectSignal(MapSignalSetSelectSignal data)
        {
            if (data == null)
                return;

            if (data.nType == (int)MouseAttachFuncType.DangerSign)
            {
                highlightImage.rectTransform.eulerAngles = new Vector3(0f, 0f, 90f);
            }
            else if (data.nType == (int)MouseAttachFuncType.OnWaySign)
            {
                highlightImage.rectTransform.eulerAngles = new Vector3(0f, 0f, 0f);
            }
            else if (data.nType == (int)MouseAttachFuncType.HelpMeSign)
            {
                highlightImage.rectTransform.eulerAngles = new Vector3(0f, 0f, 270f);
            }
            else if (data.nType == (int)MouseAttachFuncType.LostEyeSign)
            {
                highlightImage.rectTransform.eulerAngles = new Vector3(0f, 0f, 180f);
            }
            else
            {
                highlightImage.rectTransform.eulerAngles = new Vector3(0f, 0f, 0f);
            }
        }

        public void OnMinimapDragEnd()
        {
            if (IsVisible())
            {
                MouseCtrl.Instance.OnTeamCommand(false);
            }
        }
    }
}
