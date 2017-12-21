using UnityEngine;
using System.Collections;
using System.Text.RegularExpressions;
using UnityEngine.UI;
using System.Collections.Generic;

namespace RichTextPart { 

    public class URichTextHandler_Base : URichTextHandlerInterface
    {
        public VertexHelper vHelper;
        public URichText richText;
        public int curIndex = 0;    //记录当前info是同类型的第几个
        public virtual string HandleText(ref int addTextCount, string text, int startIndex, int endIndex, int length, GroupCollection group) { return ""; }

        public virtual bool HandleTextMesh(URichTextParseInfo info) { return true; }

        public virtual bool IsSingleRegex() { return false; }
        public virtual int GetRegexType() { return -1; }
        public virtual string[] GetRegexInfo() { return null; }

        protected int GetVertEndIndex(int nStart,string endRegex)
        {
            if(endRegex=="")
            {

            }
            int L = richText.m_OutputText.Length;
            int vEndIndex = L;
            string newOutputText = richText.m_OutputText.Substring(nStart);
            vEndIndex = newOutputText.IndexOf(endRegex);
            vEndIndex += nStart;
            //string[] msgs = newOutputText.Split(new string[] { endRegex },System.StringSplitOptions.None);
            //int msgCount = msgs.Length;
            //int index = 0;
            //for (int i=0;i<=curIndex;i++)
            //{
            //    index = msgCount - 1 - i;
            //    if(index >= 0 && index < msgs.Length)
            //        vEndIndex -= (endRegex.Length + msgs[msgCount - 1 - i].Length);
            //}
            return vEndIndex;
        }
    }
}
