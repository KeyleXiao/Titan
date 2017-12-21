using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using USpeedUI.UWidgets;

namespace USpeedUI
{
    public class UPlayerFrameTitleAnim : UCustomUIBehaviour
    {
        /// <summary>
        /// 下划线
        /// </summary>
        public Image TitleItemUnderLine = null;
        /// <summary>
        /// 按钮按下图片
        /// </summary>
        public Image TitleItemSelectedMark = null;

        private UButtonGroup m_ButtonGroup;

        public Color TitleItemText_Up = Color.white;
        public Color TitleItemText_Down = Color.white;

        public EMoveDirection TitleItemMoveDirection;

        public enum EMoveDirection
        {
            EMD_Horizontal,
            EMD_Vertical,
            
        }

        protected override void Awake()
        {
            base.Awake();
            m_ButtonGroup = GetComponent<UButtonGroup>();
            if (m_ButtonGroup == null)
            {
                Trace.LogWarning("该Gameobj 上没有UButtonGroup 功能不能完全使用");
            }
            else
            {
                m_ButtonGroup.onButtonGroupSelectedChange.AddListener(OnButtonGroupSelectedChange);
            }
        }
        protected override void Start()
        {
            base.Start();
            

        }

        public void OnPointerEnter(BaseEventData _data)
        {
            PointerEventData PointerData = (_data as PointerEventData);
            if(PointerData != null)
            {
                if(TitleItemUnderLine != null && PointerData.pointerEnter != null && PointerData.pointerEnter.transform is RectTransform)
                {
                    SetNewTransformPosition(TitleItemUnderLine.rectTransform, (PointerData.pointerEnter.transform as RectTransform));
                }
            }            
        }

        public void OnPointerExit(BaseEventData _data)
        {
            PointerEventData PointerData = (_data as PointerEventData);
            if (PointerData != null)
            {
                if (TitleItemUnderLine != null && m_ButtonGroup != null && m_ButtonGroup.SelectedItem != null)
                {
                    SetNewTransformPosition(TitleItemUnderLine.rectTransform, (m_ButtonGroup.SelectedItem.transform as RectTransform));
                }
            }
        }

        public void OnPointerClick(BaseEventData _data)
        { }

        public void OnButtonGroupSelectedChange(UButtonGroupItem _oldItem,UButtonGroupItem _newItem)
        {
            
            if(TitleItemUnderLine != null)
            {
                TitleItemUnderLine.gameObject.SetActive(true);
            }
            if(TitleItemSelectedMark != null && _newItem != null)
            {
                TitleItemSelectedMark.gameObject.SetActive(true);

                SetNewTransformPosition(TitleItemSelectedMark.rectTransform, (_newItem.transform as RectTransform));
                SetNewTransformPosition(TitleItemUnderLine.rectTransform, (_newItem.transform as RectTransform));


                Text[] enterTexts = _newItem.gameObject.GetComponentsInChildren<Text>();
                foreach (var t in enterTexts)
                {
                    t.color = TitleItemText_Down;
                }
            }
            if(_oldItem != null)
            {
                Text[] enterTexts = _oldItem.gameObject.GetComponentsInChildren<Text>();
                foreach (var t in enterTexts)
                {
                    t.color = TitleItemText_Up;
                }
            }
        }

        private void SetNewTransformPosition(RectTransform _renderTransform,RectTransform _targetTransform)
        {
            Vector2 newPostion = _renderTransform.anchoredPosition;
            if (TitleItemMoveDirection == EMoveDirection.EMD_Horizontal)
                newPostion.x = _targetTransform.anchoredPosition.x;
            else
                newPostion.y = _targetTransform.anchoredPosition.y;

            _renderTransform.anchoredPosition = newPostion;
        }
    }
}
