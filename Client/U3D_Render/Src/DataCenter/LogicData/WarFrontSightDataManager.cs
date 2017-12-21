using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using USpeedUI;
using USpeedUI.WarFrontSight;

namespace DataCenter
{
    public class WarFrontSightDataManager : Singleton<WarFrontSightDataManager>, IModelData
    {
        public void Init()
        {
            UISystem.OnResolutionChanged += onResolutionChanged;
        }

        public void Clear()
        {
            UISystem.OnResolutionChanged -= onResolutionChanged;
        }

        protected Vector2? m_FrontSightDefaultPos = null;
        /// <summary>
        /// UI坐标系下 原始坐标
        /// </summary>
        public Vector2 FrontSightDefaultPos
        {
            get
            {
                return m_FrontSightDefaultPos ?? Vector2.zero ;
            }
        }


        protected Vector2? m_FrontSightPos = null;
        /// <summary>
        /// UI坐标系下 原始坐标+偏移值
        /// </summary>
        public Vector2 FrontSightPos
        {
            get
            {
                return m_FrontSightPos ?? FrontSightDefaultPos;
            }
            set
            {
                if(m_FrontSightPos == null || m_FrontSightPos != value)
                {
                    m_FrontSightPos = (Vector2)value;
                    UFrontSightPosData data = new UFrontSightPosData();
                    data.Pos = (Vector2)m_FrontSightPos;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_POS, data);

                    CalculateFrontSightScreenPos();
                }                
            }
        }

        protected Vector2? m_FrontSightScreenPos = null;
        /// <summary>
        /// 屏幕坐标系下 原始坐标+偏移值
        /// </summary>
        public Vector2 FrontSightScreenPos
        {
            get
            {
                return m_FrontSightScreenPos ?? new Vector2(Screen.width / 2, Screen.height / 2);
            }
        }
        /// <summary>
        /// 计算屏幕坐标系下的准星坐标
        /// </summary>
        public void CalculateFrontSightScreenPos()
        {
            //将坐标转换为屏幕坐标
            IUIWnd tmpWnd = UISystem.Instance.GetWnd(WndID.WND_ID_WAR_FRONTSIGHT);
            if (tmpWnd != null)
            {
                IUIWndView tmpView = tmpWnd.GetWndView();
                if (tmpView != null && tmpView.GetTransform() != null)
                {
                    Vector2 ScreenPos = RectTransformUtility.WorldToScreenPoint(UISystem.Instance.GetCamera(), tmpView.GetTransform().position);
                    m_FrontSightScreenPos = ScreenPos;
                }
            }
        }


        protected Vector2 m_FrontSightPosOffset;
        /// <summary>
        /// UI坐标系下 偏移值
        /// </summary>
        public Vector2 FrontSightPosOffset
        {
            get { return m_FrontSightPosOffset; }
            set
            {
                m_FrontSightPosOffset = value;
                FrontSightPos = FrontSightDefaultPos + m_FrontSightPosOffset;
            }

        }

        protected Color m_FrontSightColor;
        public Color FrontSightColor
        {
            get { return m_FrontSightColor; }
            set
            {
                if(m_FrontSightColor != value)
                {
                    m_FrontSightColor = value;
                    UFrontSightColorData data = new UFrontSightColorData();
                    data.nColor = m_FrontSightColor;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_COLOR, data);
                }
            }
        }

        protected bool m_FrontSightVisible;
        public bool FrontSightVisible
        {
            get { return m_FrontSightVisible; }
            set
            {
                if(m_FrontSightVisible != value)
                {
                    m_FrontSightVisible = value;
                    UFrontSightVisibleData data = new UFrontSightVisibleData();
                    data.bShow = m_FrontSightVisible;
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_FRONTSIGHT_VISIBLE, data);

                }
            }
        }


        protected void onResolutionChanged()
        {
            CalculateFrontSightScreenPos();
        }
    }
}
