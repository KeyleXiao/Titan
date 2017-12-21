using UnityEngine;
using System.Collections;
using System.Text.RegularExpressions;
using UnityEngine.UI;

namespace RichTextPart
{
    public class URichTextHandler_Bold : URichTextHandler_Base
    {
        private static URichTextHandler_Bold _handler = null;
        public static void Init()
        {
            if (_handler == null)
            {
                _handler = new URichTextHandler_Bold();
                URichTextUtil.AddHandler(_handler);
            }
        }

        URichTextHandler_Bold()
        {

        }

        //前期的字符串替换
        public override string HandleText(ref int addTextCount, string text, int startIndex, int endIndex, int length, GroupCollection group)
        {
            text = URichTextUtil.ReplacePlaceHolder("<b>", "</b>", text, startIndex, endIndex);
            return text;
        }

        //字符串网格控制
        public override bool HandleTextMesh(URichTextParseInfo info)
        {
            return true;
        }

        public override bool IsSingleRegex() { return false; }
        public override int GetRegexType() { return URichTextUtil.REGEX_TYPE_SYSTEMA; }

        public override string[] GetRegexInfo()
        {
            return new string[] { @"<Bo>", @"</Bo>" };
        }
    }
}