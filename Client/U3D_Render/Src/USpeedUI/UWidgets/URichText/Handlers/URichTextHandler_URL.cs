/*******************************************************************
** 文件名:	URichTextHandler_URL.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	何嘉文
** 日  期:	2016/11/24
** 版  本:	1.0
** 描  述:	富文本解析超链接部分
** 应  用:  
**************************** 修改记录 ******************************
** 修改人: 林建聪
** 日  期: 2017.1.3
** 描  述: 1、增加 用户可以修改超链接内容显示颜色，默认是m_defaultURLColor(蓝色)
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Text.RegularExpressions;
using UnityEngine.UI;
using System.Collections.Generic;

namespace RichTextPart {
    public class URichTextHandler_URL : URichTextHandler_Base
    {
        private static URichTextHandler_URL _handler = null;
        string preMsg = "<quad width=0/><{0}>";
        string endMsg = "</color><quad name width=0/>";
        //下划线默认颜色
        string m_defaultURLColor = "color=0000FF";

        public static void Init()
        {
            if (_handler == null)
            {
                _handler = new URichTextHandler_URL();
                URichTextUtil.AddHandler(_handler);
            }
        }

        URichTextHandler_URL()
        {
            
        }

        public override string HandleText(ref int addTextCount, string text, int startIndex, int endIndex, int length, GroupCollection group)
        {
            string strColor = m_defaultURLColor;
            //如果有匹配到 color=的话取匹配到的颜色
            if (group.Count >= 2 && group[2].Length > 0)
                strColor = group[2].Value.Remove(0, 1).ToString();

            strColor = strColor.Insert(strColor.IndexOf("=") + 1, "#");

            string tmpPreMsg = string.Format(preMsg, strColor);

            text = URichTextUtil.ReplacePlaceHolder(tmpPreMsg, endMsg, text, startIndex, endIndex);

            return text;
        }

        //字符串网格控制
        public override bool HandleTextMesh(URichTextParseInfo info)
        {
            if (info.vertStartIndex < 0)
                return false;

            int vertEndIndex = GetVertEndIndex(info.vertStartIndex, endMsg);
            int vertStartIndex = info.vertStartIndex + preMsg.Length;


            //可以考虑用内存池，减少内存分配
            var output = new List<UIVertex>();
            vHelper.GetUIVertexStream(output);

            //绘制下划线
            vHelper.Clear();
            List<Rect> lineRects = richText.GetTextTiles(vertStartIndex, vertEndIndex, output);

            for (int i = 0; i < lineRects.Count; i++)
            {
                output.AddRange(URichTextUtil.GetUnderLineVerts(lineRects[i], vertEndIndex, richText.GetUnderLineInfo()));
            }
            //添加的顶点数组
            vHelper.AddUIVertexTriangleStream(output);

            info.urlHrefInfo.m_tagTextRects.AddRange(lineRects);

            info.urlHrefInfo.strHrefEvent = info.values[1].ToString();
            return true;
        }

        public override bool IsSingleRegex() { return false; }
        public override int GetRegexType() { return URichTextUtil.REGEX_TYPE_SELF; }

        public override string[] GetRegexInfo()
        {
                                //@"<a href=([^>\n\s]+)>(.*?)(</a>)"
            return new string[] { @"<a href=([^>\n\s]+)(?( color=)(.+?))>", @"</a>" };
        }
    }
}