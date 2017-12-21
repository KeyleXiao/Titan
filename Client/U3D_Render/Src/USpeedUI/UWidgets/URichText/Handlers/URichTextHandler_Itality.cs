using UnityEngine;
using System.Collections;
using System.Text.RegularExpressions;

namespace RichTextPart
{
    public class URichTextHandler_Itality : URichTextHandler_Base
    {
        private static URichTextHandler_Itality _handler = null;
        public static void Init()
        {
            if (_handler == null)
            {
                _handler = new URichTextHandler_Itality();
                URichTextUtil.AddHandler(_handler);
            }
        }

        URichTextHandler_Itality()
        {

        }

        //前期的字符串替换
        public override string HandleText(ref int addTextCount, string text, int startIndex, int endIndex, int length, GroupCollection group)
        {
            text = URichTextUtil.ReplacePlaceHolder("<i>", "</i>", text, startIndex, endIndex);
            return text;
        }

        //字符串网格控制
        public override bool HandleTextMesh(URichTextParseInfo info)
        {
            return true;
        }
        public override bool IsSingleRegex() { return false; }

        public override int GetRegexType() { return URichTextUtil.REGEX_TYPE_SYSTEMA; }

        public override string[] GetRegexInfo() {
            return new string[] { @"<It>", @"</It>" };
        }
    }
}