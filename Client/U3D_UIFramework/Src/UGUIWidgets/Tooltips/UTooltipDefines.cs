using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

namespace UIWidgets
{
    public class TooltipStyle : MonoBehaviour
    {
        public Sprite topLeftCorner;
        public Sprite topRightCorner;
        public Sprite bottomLeftCorner;
        public Sprite bottomRightCorner;
        public Sprite topMiddle;
        public Sprite bottomMiddle;
        public Sprite leftMiddle;
        public Sprite rightMiddle;
        public int tipOffset;
        public LayoutElement mainTextContainer;
        [SerializeField]
        [HideInInspector]
        protected int m_nTooltipID;
        public int TooltipID { get { return m_nTooltipID; } }

    }

    public enum TipPosition
    {
        TopRightCorner,
        BottomRightCorner,
        TopLeftCorner,
        BottomLeftCorner,
        MouseTopRightCorner,
        MouseBottomRightCorner,
        MouseTopLeftCorner,
        MouseBottomLeftCorner,
        TopMiddle,
        BottomMiddle,
        RightMiddle,
        LeftMiddle,
        MouseTopMiddle,
        MouseBottomMiddle,
        MouseRightMiddle,
        MouseLeftMiddle
    }

    /// <summary>
    /// Tooltip Style 里面声明的参数名（"%BodyText%"）一对%包括的部分
    /// <para>根据这个参数名将数据填到对应的文本里面</para>
    /// </summary>
    public class UTooltipParamName
    {
        public const string BodyText = "BodyText";
        public const string ItemName = "ItemName";
        public const string ItemCost = "ItemCost";
        public const string ItemDesc = "ItemDesc";
    }
}
