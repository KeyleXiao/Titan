using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.UWidgets;

namespace USpeedUI.WarGoodsStore
{
    public class UWarTreeNodeHorizonalLineComponnet : Image
    {
        protected RectTransform RectTransform { get { return rectTransform; } }
        public void SetVisible(bool _bVisible)
        {
            this.gameObject.SetActive(_bVisible);
        }

        public float x
        {
            get
            {
                return RectTransform.anchoredPosition.x;
            }
            set
            {
                Vector2 vPos = RectTransform.anchoredPosition;
                vPos.x = value;
                RectTransform.anchoredPosition = vPos;
            }
        }

        public float y
        {
            get
            {
                return RectTransform.anchoredPosition.y;
            }
            set
            {
                Vector2 vPos = RectTransform.anchoredPosition;
                vPos.y = value;
                RectTransform.anchoredPosition = vPos;
            }
        }

        public float width
        {
            get
            {
                return RectTransform.rect.width;
            }
            set
            {
                Vector2 vPos = RectTransform.sizeDelta;
                vPos.x = value;
                RectTransform.sizeDelta = vPos;
            }
        }
    }
    public  class UWarGoodsStoreTreeNodeComponent : UWarGoodsStoreItemComponent
    {
        /// <summary>
        /// 连接子节点的垂直线
        /// </summary>
        public Image ChildVerticalLine;
        /// <summary>
        /// 连接父节点的垂直线
        /// </summary>
        public Image ParentVerticalLine;
        /// <summary>已购买标识</summary>
        public Image PurchasedIcon;

        /// <summary>
        /// 节点子节点数
        /// </summary>
        public int ChildCount;

        public float x
        {
            get
            {
                return RectTransform.anchoredPosition.x;
            }
            set
            {
                Vector2 vPos = RectTransform.anchoredPosition;
                vPos.x = value;
                RectTransform.anchoredPosition = vPos;
            }
        }

        public float y
        {
            get
            {
                return RectTransform.anchoredPosition.y;
            }
            set
            {
                Vector2 vPos = RectTransform.anchoredPosition;
                vPos.y = value;
                RectTransform.anchoredPosition = vPos;
            }
        }

        public float width
        {
            get
            {
                return RectTransform.rect.width;
            }
            set
            {
                Vector2 vPos = RectTransform.sizeDelta;
                vPos.x = value;
                RectTransform.sizeDelta = vPos;
            }

        }
        public float height
        {
            get
            {
                return RectTransform.rect.height + ChildVerticalLineHeight ;//+ ParentVerticalLineHeight;
            }
            set
            {
                Vector2 vPos = RectTransform.sizeDelta;
                vPos.y = value;
                RectTransform.sizeDelta = vPos;
            }

        }

        public float ChildVerticalLineHeight
        {
            get
            {
                if (!ChildVerticalLine.gameObject.activeSelf)
                    return 0;
                return CaleHeight(ChildVerticalLine.rectTransform.rect);
            }
        }

        public float ChildVerticalLineLeft
        {
            get
            {
                return ChildVerticalLine.rectTransform.anchoredPosition.x;
            }             
        }
        public float ChildVerticalLineTop
        {
            get
            {
                return ChildVerticalLine.rectTransform.anchoredPosition.y;
            }
        }

        public float ChildVerticalLineRight
        {
            get
            {
                return ChildVerticalLine.rectTransform.rect.xMax + ChildVerticalLine.rectTransform.anchoredPosition.x;
            }
        }

        public float ParentVerticalLineHeight
        {
            get
            {
                if (!ParentVerticalLine.gameObject.activeSelf)
                    return 0;
                return CaleHeight(ParentVerticalLine.rectTransform.rect);
            }
        }

        private float CaleHeight(Rect rect)
        {
            return Math.Abs(rect.y) + rect.height;
        }

        public void SetChildVerticalLineVisible(bool _bChildLine)
        {
            ChildVerticalLine.gameObject.SetActive(_bChildLine);
        }

        public void SetParentVerticalLineVisible(bool _bParentLine)
        {
            ParentVerticalLine.gameObject.SetActive(_bParentLine);
        }

        public void SetPurchasedIconVisible(bool _bPurchased)
        {
            PurchasedIcon.enabled = _bPurchased;
        }

        public void SetVerticalLineVisible(bool _bChildLine,bool _bParentLine)
        {

        }
    }
}
