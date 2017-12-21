using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

namespace USpeedUI.ChatBox
{
    public class ChatBoxResizer : MonoBehaviour, IBeginDragHandler, IDragHandler, IEndDragHandler
    {
        // 继承Graphic以接收UI消息。如果没有底图时可以把下面两行加上
        //public override void SetMaterialDirty() { return; }
        //public override void SetVerticesDirty() { return; }

        [SerializeField]
        public RectTransform containerTrans;

        [SerializeField]
        public float resizeMinWidth;
        [SerializeField]
        public float resizeMinHeight;
        [SerializeField]
        public float resizeMaxWidth;
        [SerializeField]
        public float resizeMaxHeight;

        private ChatBoxWndView m_wndView;
        private bool m_draging;
		public bool IsDraging { get { return m_draging; } }
        private Vector2 m_dragStartPos;
        private Vector2 m_dragStartSize;

        public void SetWndView(ChatBoxWndView wndView)
        {
            m_wndView = wndView;
        }

        public void OnBeginDrag(PointerEventData eventData)
        {
            if (m_draging == false && eventData.button == PointerEventData.InputButton.Left)
            {
                m_draging = true;
                m_dragStartPos = eventData.position;
                m_dragStartSize = containerTrans.sizeDelta;

                if (m_wndView != null)
                    m_wndView.SetFrameFade(false);
            }
        }

        public void OnDrag(PointerEventData eventData)
        {
            if (m_draging)
            {
                Canvas canvas = UISystem.Instance.GetCanvas();
                float scaleFactor = (canvas.scaleFactor > 0f) ? 1f / canvas.scaleFactor : 1f;
                Vector2 sizeDelta = eventData.position - m_dragStartPos;
                Vector2 targetSize = m_dragStartSize + sizeDelta * scaleFactor;

                // 大小限制
                if (targetSize.x < resizeMinWidth)
                    targetSize.x = resizeMinWidth;
                if (targetSize.y < resizeMinHeight)
                    targetSize.y = resizeMinHeight;
                if (targetSize.x > resizeMaxWidth)
                    targetSize.x = resizeMaxWidth;
                if (targetSize.y > resizeMaxHeight)
                    targetSize.y = resizeMaxHeight;

                containerTrans.sizeDelta = targetSize;
            }
        }

        public void OnEndDrag(PointerEventData eventData)
        {
            if (m_draging)
            {
                m_draging = false;

                if (m_wndView != null)
                    m_wndView.SetFrameFade(true);
            }
        }
    }
}
