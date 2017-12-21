using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;
using UnityEngine.UI;

namespace RichTextPart
{
    public enum RichType
    {
        Url = 0,                //超链接
        Bold = 1,               //加粗
        Itality = 2,             //斜体
        Texture = 3,            //图片
        Unline = 4,             //下划线
        DynamicExpression = 5   //动态图（如表情）

    }

    public class URichTextUtil
    {

        //图片
        //@"<quad name=(.+?) size=(\d*\.?\d+%?) width=(\d*\.?\d+%?)>",
        //@"</quad>",

        private static readonly string headPlaceholder = "^";
        //private static readonly string tailPlaceholder = "&";   //暂时没用


        public const int PLACE_HOLDER_LENGTH = 1;   //占位符统一长度

        public const int REGEX_TYPE_SELF = 0;       //自定义的（[x],[\x]）    替换成##
        public const int REGEX_TYPE_SYSTEMA = 1;    //系统A类型的（<x>,<\x>） 替换成##
        public const int REGEX_TYPE_SYSTEMB = 2;    //系统B类型（<x/>）       替换成$$

        public static List<URichTextParseInfo> hrefInfos = new List<URichTextParseInfo>();

        private static readonly StringBuilder s_TextBuilder = new StringBuilder();

        private static List<URichTextHandler_Base> handlers = new List<URichTextHandler_Base>();

        private static List<string[]> richRegexs = new List<string[]>();

        private static bool isInit = false;
        public static void Init()
        {
            URichTextHandler_URL.Init();
            URichTextHandler_Bold.Init();
            URichTextHandler_Itality.Init();
            URichTextHandler_Texture.Init();
            URichTextHandler_Unline.Init();
            
            isInit = true;
        }

        public static void ClearHandlersIndex()
        {
            for (int i = 0; i < handlers.Count; i++)
                handlers[i].curIndex = 0;
        }
        public static bool HandleTextMesh(URichTextParseInfo info, VertexHelper toFill,URichText text)
        {
            //Debug.Log("HandleMesh [" + info.type + "] :" + info.vertStartIndex + " " + (info.startIndex + info.realLength-1) + " " + info.realLength);
            handlers[(int)info.type].richText = text;
            handlers[(int)info.type].vHelper = toFill;
            bool success = handlers[(int)info.type].HandleTextMesh(info);
            handlers[(int)info.type].curIndex++;

            return success;
        }

        public static void AddHandler(URichTextHandler_Base handler)
        {
            handlers.Add(handler);
        }

        public static void AnalyzeText(ref string text)
        {
            hrefInfos.Clear();
            if (text == "" || text.Length <= 3) return;
            if (!isInit)
                Init();
            //***********第一步：对整个字符串进行正则表达式替换********//
            //三种正则：
            //自定义的（[x],[\x]）    替换成##
            //系统A类型的（<x>,<\x>） 替换成##
            //系统B类型（<x/>）       替换成$$
            string beginFlagText = text;        //记录富文本的实际开始作用index
            string originalText = text;
            for (int n = 0; n < handlers.Count; n++)
            {
                URichTextHandler_Base handler = handlers[n];
                int regexCount = 2;
                if (handler.IsSingleRegex())
                    regexCount = 1;
                for (int i = 0; i < regexCount; i++)
                {
                    s_TextBuilder.Length = 0;
                    Regex s_Regex = new Regex(handler.GetRegexInfo()[i], RegexOptions.Singleline);
                    int isHead = regexCount == 1 ? -1 : i;
                    foreach (Match match in s_Regex.Matches(originalText))
                    {
                        var group = match.Groups[1];
                        string[] richValues = new string[match.Groups.Count];
                        var hrefInfo = new URichTextParseInfo
                        {
                            sortIndex = match.Index,
                            type = (RichType)(n),
                            values = match.Groups,
                            isHead = isHead
                        };
                        ////替换字符串，进替换匹配到的第一个
                        int length = match.Value.Length;
                        int index = text.IndexOf(match.Value);
                        
                        text = ReplaceTheFirst(text,match.Value, headPlaceholder);
                        beginFlagText = ReplaceTheFirst(beginFlagText, match.Value,isHead == 1 ? "" : headPlaceholder);       
                        //text = text.Replace(match.Value, hrefInfo.isHead==1 ? tailPlaceholder:headPlaceholder);  //重复replace，消耗性能
                        hrefInfos.Add(hrefInfo);
                    }
                    //Debug.Log(beginFlagText);
                    //Debug.Log(text);
                }
            }
            //***********第二步：对href进行排序********//
            hrefInfos.Sort(CompareByIndex);

            //***********第三步：对替换后的字符串进行分析********//
            int curTextIndex = 0;
            int curHrefIndex = 0;
            //遍历字符串
            while (true)
            {
                int addTextCount = 1;   //处理完后，会对字符串进行添加的字符数

                if (curTextIndex>=0 && curTextIndex< text.Length && text[curTextIndex].ToString() == headPlaceholder && curHrefIndex < hrefInfos.Count)
                {
                    URichTextParseInfo hrefInfo = hrefInfos[curHrefIndex];
                    //Debug.Log("currrrr: " + hrefInfo.type + "  " + hrefInfo.sortIndex +"  "+ hrefInfo.isHead);
                    int hrefStartIndex = curTextIndex;  //富文本需要处理字符串的首字母index
                    int hrefEndIndex = curTextIndex + 1;  //富文本需要处理字符串的尾字母index
                    int hrefLength = 0;                 //富文本需要处理字符串的长度
                    int hrefInsideCout = 0;             //富文本需要处理字符串的中内嵌的占位符数量
                    //遍历起始占位符到结尾占位符之间的字符串
                    bool isFindEnd = false;
                    if (hrefInfo.isHead != -1)
                    {
                        while (true)
                        {

                            if (hrefEndIndex >= 0 && hrefEndIndex < text.Length && text[hrefEndIndex].ToString() == headPlaceholder)
                            {
                                hrefInsideCout++;
                                URichTextParseInfo nextHrefInfo = hrefInfos[curHrefIndex + hrefInsideCout];

                                //Debug.Log("nenenenenL: " + nextHrefInfo.type + "  " + nextHrefInfo.sortIndex + " " + nextHrefInfo.isHead);
                                //找到的末尾
                                if (nextHrefInfo.isHead == 1 && nextHrefInfo.type == hrefInfo.type)
                                {
                                    hrefInfos.Remove(nextHrefInfo);
                                    isFindEnd = true;
                                }
                            }

                            if (!isFindEnd)
                                hrefEndIndex++;
                            else
                            {
                                hrefInsideCout--;   //减去末尾的
                                                    //检查是否合法：
                                                    //...
                                break;
                            }
                            if (hrefEndIndex >= text.Length)
                                break;
                        }
                        if (isFindEnd)
                        {

                            //计算总长度
                            hrefLength = hrefEndIndex - hrefStartIndex - hrefInsideCout + 1 - 2;
                            hrefInfo.vertStartIndex = curTextIndex;
                            hrefInfo.realLength = hrefLength;   //保存作用长度
                            text = handlers[(int)hrefInfo.type].HandleText(ref addTextCount, text, hrefStartIndex, hrefEndIndex, hrefLength, hrefInfo.values);
                            curHrefIndex++;
                        }
                        else
                        {
                            curTextIndex = text.Length + 1;     //如果没找到那就是该字符串有问题，直接返回
                            Debug.LogWarning("regex Type[" + hrefInfo.type + "] has no end!!!");
                        }
                    }
                    else
                    {
                        hrefInfo.vertStartIndex = curTextIndex;
                        hrefInfo.realLength = 0;   //保存作用长度
                        text = handlers[(int)hrefInfo.type].HandleText(ref addTextCount, text, hrefStartIndex, hrefStartIndex, hrefLength, hrefInfo.values);
                        curHrefIndex++;
                    }
                    //Debug.Log("regex Type[" + hrefInfo.type + "]: " + text);
                }

                curTextIndex += addTextCount;
                if (curTextIndex >= text.Length)
                    break;
            }
            //***********第四步：得到HrefInfo的开始作用Index********//
            curTextIndex = 0;
            curHrefIndex = 0;
            //遍历beginFlagText，找到开始的index
            while (true)
            {
                if (beginFlagText[curTextIndex].ToString() == headPlaceholder)
                {
                    //Debug.Log(beginFlagText);
                    if (curHrefIndex >= hrefInfos.Count)
                        break;
                    URichTextParseInfo hrefInfo = hrefInfos[curHrefIndex];
                    hrefInfo.startIndex = curTextIndex;
                    beginFlagText = beginFlagText.Remove(curTextIndex, PLACE_HOLDER_LENGTH);
                    curHrefIndex++;
                }
                else
                    curTextIndex++;
                if (curTextIndex >= beginFlagText.Length)
                    break;
            }
            //Debug.Log(beginFlagText);
        }

        public static int CompareByIndex(URichTextParseInfo x, URichTextParseInfo y)//从大到小排序器  
        {
            if (x == null)
            {
                Debug.LogError("the x URichTextParseInfo isNUll");
                return 0;
            }
            if (y == null)
            {
                Debug.LogError("the y URichTextParseInfo isNUll");
                return 0;
            }
            int retval = x.sortIndex.CompareTo(y.sortIndex);
            return retval;
        }

        public static string ReplacePlaceHolder(string head, string tail, string textSource, int start, int end)
        {
            StringBuilder builder = new StringBuilder();
            string pre = textSource.Substring(0, start);
            string middle = textSource.Substring(start + 1, end - (start + 1));
            string final = textSource.Substring(end + PLACE_HOLDER_LENGTH);

            builder.Append(textSource.Substring(0, start));
            builder.Append(head);
            builder.Append(textSource.Substring(start + 1, end - (start + 1)));
            builder.Append(tail);
            builder.Append(textSource.Substring(end + PLACE_HOLDER_LENGTH));


            //Debug.Log("LLL: pre  " + pre);
            //Debug.Log("LLL: middle  " + middle);
            //Debug.Log("LLL: final  " + final);

            //builder.Append(pre);
            //builder.Append(head);
            //builder.Append(middle);
            //builder.Append(tail);
            //builder.Append(final);
            
            //textSource = builder.ToString();
            //textSource = pre + head + middle + tail + final;
            return builder.ToString();
        }

        public static string ReplacePlaceHolder(string head, string textSource, int start)
        {
            StringBuilder builder = new StringBuilder();
            string pre = textSource.Substring(0, start);
            string final = textSource.Substring(start + PLACE_HOLDER_LENGTH);

            builder.Append(textSource.Substring(0, start));
            builder.Append(head);
            builder.Append(textSource.Substring(start + 1));


            //Debug.Log("LLL: pre  " + pre);
            //Debug.Log("LLL: middle  " + middle);
            //Debug.Log("LLL: final  " + final);

            //builder.Append(pre);
            //builder.Append(head);
            //builder.Append(middle);
            //builder.Append(tail);
            //builder.Append(final);

            //textSource = builder.ToString();
            //textSource = pre + head + middle + tail + final;
            return builder.ToString();
        }

        public static string ReplaceTheFirst(string source,string oldS,string newS)
        {
            int length = oldS.Length;
            int index = source.IndexOf(oldS);
            source = source.Remove(index, length).Insert(index, newS);
            return source;
        }

        private const float UNDER_LINE_SPACE  = -0.25f;
        private const int   UNDER_LINE_HEIGHT = 3;


        public static UIVertex[] GetUnderLineVerts(Rect textBounds, int vertEndIndex, URichTextUnderLineInfo info, int z = 0)
        {
            //由3个片：12个顶点组成：18个index：顺序 前中后
            UIVertex[] newVerts = new UIVertex[18];
            Vector3 textStartpos = new Vector3(textBounds.x, textBounds.y, z);
            Vector3 textEndpos = new Vector3(textBounds.x + textBounds.width, textBounds.y, z);
            
            //text generator的4,5都不放东西的
            //而且uv位置分布0.0：              正常的：
            //          2------1                      0------1
            //          |      |        =>            |      |
            //          3------0                      3------2
            //所以取y，偏移up方向
            float uvoffset = (info.uvs[1].y - info.uvs[0].y) * 0.5f;

            float textWidth = info.width;
            float width = 5;
            float[] offset = { 0f, 0, 0f };   //前中后各偏移的长度
            Vector3[] linePoses =
            {
                textStartpos +offset[0]*Vector3.right,textStartpos +(width + offset[0])*Vector3.right,  //前面
                textStartpos +(width + offset[0])*Vector3.right,textEndpos-(width- offset[1])*Vector3.right,     //中间
                textEndpos +(offset[2]-width)*Vector3.right,textEndpos +(offset[2])*Vector3.right //后面
            };

            float[] uvChangeFlags = { 0, 1, 1, 1, 1, 0 }; //前-head，前-tail，中-head，中-tail，后-head，后-tail
            //设置uv和pos             
            for (int i = 0; i < 3; i++)
            {

                newVerts[i * 6 + 0].position = linePoses[i * 2 + 0] - Vector3.up * (UNDER_LINE_SPACE);
                newVerts[i * 6 + 1].position = linePoses[i * 2 + 1] - Vector3.up * (UNDER_LINE_SPACE);
                newVerts[i * 6 + 2].position = linePoses[i * 2 + 1] - Vector3.up * (UNDER_LINE_SPACE + UNDER_LINE_HEIGHT);
                newVerts[i * 6 + 3].position = linePoses[i * 2 + 1] - Vector3.up * (UNDER_LINE_SPACE + UNDER_LINE_HEIGHT);
                newVerts[i * 6 + 4].position = linePoses[i * 2 + 0] - Vector3.up * (UNDER_LINE_SPACE + UNDER_LINE_HEIGHT);
                newVerts[i * 6 + 5].position = linePoses[i * 2 + 0] - Vector3.up * (UNDER_LINE_SPACE);

                newVerts[i * 6 + 0].uv0 = info.uvs[0] + Vector2.up * uvoffset * uvChangeFlags[i * 2 + 0];
                newVerts[i * 6 + 1].uv0 = info.uvs[1] - Vector2.up * uvoffset * uvChangeFlags[i * 2 + 1];
                newVerts[i * 6 + 2].uv0 = info.uvs[2] - Vector2.up * uvoffset * uvChangeFlags[i * 2 + 1];
                newVerts[i * 6 + 3].uv0 = info.uvs[2] - Vector2.up * uvoffset * uvChangeFlags[i * 2 + 1];
                newVerts[i * 6 + 4].uv0 = info.uvs[3] + Vector2.up * uvoffset * uvChangeFlags[i * 2 + 0];
                newVerts[i * 6 + 5].uv0 = info.uvs[0] + Vector2.up * uvoffset * uvChangeFlags[i * 2 + 0];
            }

            //设置顶点颜色
            for (int i = 0; i < 18; i++)
            {
                newVerts[i].color = info.lineColor;//Color.black;
            }
            return newVerts;
        }
    }
}
