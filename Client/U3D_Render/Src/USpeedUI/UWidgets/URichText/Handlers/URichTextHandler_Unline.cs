using UnityEngine;
using System.Collections;
using System.Text.RegularExpressions;
using UnityEngine.UI;
using System.Collections.Generic;

namespace RichTextPart
{
    public class URichTextHandler_Unline : URichTextHandler_Base
    {
        private static URichTextHandler_Unline _handler = null;

        private URichTextSprite _sprite;
        public URichTextSprite TextSprite
        {
            get
            {
                if (!_sprite)
                    _sprite = this.richText.GetTextSprite();
                return _sprite;
            }
        }

        public static void Init()
        {
            if (_handler == null)
            {
                _handler = new URichTextHandler_Unline();
                URichTextUtil.AddHandler(_handler);
            }
        }

        URichTextHandler_Unline()
        {

        }

        string preMsg = "<quad width=0/>";
        string endMsg = "<quad name width=0/>";

        //前期的字符串替换
        public override string HandleText(ref int addTextCount, string text, int startIndex, int endIndex, int length, GroupCollection group)
        {
            text = URichTextUtil.ReplacePlaceHolder(preMsg, endMsg, text, startIndex, endIndex);
            return text;
        }

        float space = 4;
        int lineHeight = 3;


        //字符串网格控制
        public override bool HandleTextMesh(URichTextParseInfo info)
        {
            if (info.vertStartIndex < 0)
                return false;

            int vertEndIndex = GetVertEndIndex(info.vertStartIndex,endMsg);
            int vertStartIndex = info.vertStartIndex + preMsg.Length;

            //可以考虑用内存池，减少内存分配
            var output = new List<UIVertex>();
            vHelper.GetUIVertexStream(output);
            
            //绘制下划线
            vHelper.Clear();
            List<Rect> lineRects = richText.GetTextTiles(vertStartIndex, vertEndIndex,output);
            for(int i=0;i<lineRects.Count;i++)
            {
                //Debug.Log("tile[" + i + "]: " + lineRects[i]);
                output.AddRange(URichTextUtil.GetUnderLineVerts(lineRects[i],vertEndIndex,richText.GetUnderLineInfo()));
            }
            //添加的顶点数组
            vHelper.AddUIVertexTriangleStream(output);
            return true;
        }

        public override bool IsSingleRegex() { return false; }
        public override int GetRegexType() { return URichTextUtil.REGEX_TYPE_SYSTEMA; }

        public override string[] GetRegexInfo()
        {
            return new string[] { @"<u>", @"</u>" };
        }
    }
}