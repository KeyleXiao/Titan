using UnityEngine;
using System.Collections;
using System.Text.RegularExpressions;
using System.Collections.Generic;

namespace RichTextPart
{
    public class URichTextParseInfo {
        public int sortIndex;
        public RichType type;
        public GroupCollection values; //包含各种属性以及原来的文本
        public int isHead;             //-1：头尾结合，1：尾，0：头
        public int startIndex;         // 
        public int endIndex;
        public int realLength;
        public int vertStartIndex;          //记录富文本起始的顶点的index，因为在ugui的顶点列表里面，隐藏的富文本还是占用顶点

        public URichTextURLInfo urlHrefInfo = new URichTextURLInfo();

        //记录富文本结尾的顶点的index，因为在ugui的顶点列表里面，隐藏的富文本还是占用顶点
        //public int vertEndIndex;            //不提供了，要拿自己找过滤后的text，Find对应的结尾字符串去拿

    }

    public class URichTextURLInfo
    {
        public readonly List<Rect> m_tagTextRects = new List<Rect>();
        public string strHrefEvent = null;

        public void Clear()
        {
            m_tagTextRects.Clear();
            strHrefEvent = null;
        }
        public bool IsValid()
        {
            return m_tagTextRects.Count != 0 && strHrefEvent != null;
        }
    }

    public class URichTextUnderLineInfo
    {
        public Color32 lineColor = Color.black;
        public float width;
        //TextGenerator是以顶点为单位，不是以索引
        public Vector2[] uvs = new Vector2[4];
    }
};
