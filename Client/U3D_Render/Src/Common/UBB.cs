/*******************************************************************
** 文件名:	UBB.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-03-31
** 版  本:	1.0
** 描  述:	

		UBB格式化辅助类
********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using UnityEngine;
using USpeedUI;

public enum UBB_FORMAT_TYPE
{
    SCANFORM,       // sf
    UGUI,           // ugui
}

namespace U3D_Render.Common
{

    public delegate string UBBHanlder(RegexInfo reg,string strText);

    public struct RegexInfo
    {
        public string replace;
        public UBBHanlder del;
        public Regex r;
        public bool isMatch;
    }

    /// <summary>
    /// UBB格式化辅助类
    /// </summary>
    public class UBB
    {

        private static Dictionary<string, RegexInfo> ris = Init();
        private static Dictionary<string, RegexInfo> ris_ugui = init_ugui();
        private static UBBPropertyEx ubbProp = new UBBPropertyEx();

        public static void AddReplacement(ref Dictionary<string, RegexInfo> rs, string pattern, string replace)
        {
            RegexInfo ri;
            ri.r = new Regex(pattern/*, RegexOptions.IgnoreCase*/);
            ri.replace = replace;
            ri.del = null;
            ri.isMatch = false;
            rs[pattern] = ri;
        }
        public static void AddReplacementEx(string pattern, UBBHanlder del, bool isMatch=false)
        {
            RegexInfo ri;
            ri.r = new Regex(pattern);
            ri.replace = "";
            ri.del = del;
            ri.isMatch = isMatch;
            ris[pattern] = ri;
            ris_ugui[pattern] = ri;
        }

       

        public static Dictionary<string, RegexInfo> Init()
        {
            Dictionary<string, RegexInfo> rs = new Dictionary<string, RegexInfo>();

            #region 字体加粗 [b][/b]
            // 字体加粗
            AddReplacement(ref rs,@"\[b\](.+?)\[/b\]", "<b>$1</b>");
            #endregion
            
            #region 字体斜体 [i][/i]
            // 字体斜体
            AddReplacement(ref rs, @"\[i\](.+?)\[/i\]", "<i>$1</i>");
            #endregion

            #region 字体下划线 [u][/u]
            // 字体下划线
            AddReplacement(ref rs, @"\[u\](.+?)\[/u\]", "<u>$1</u>");
            #endregion

            #region 字体变红 [red][/red]#BC0605
            // 字体变红
            AddReplacement(ref rs, @"\[red\](.+?)\[/red\]", "<font color='#BC0605'>$1</font>");
            #endregion

            #region 字体变蓝 [blue][/blue]#50E9EF
            // 字体变蓝
            AddReplacement(ref rs, @"\[blue\](.+?)\[/blue\]", "<font color='#50E9EF'>$1</font>");
            #endregion

            #region 字体变黄 [yellow][/yellow]#FEF488
            // 字体变黄
            AddReplacement(ref rs, @"\[yellow\](.+?)\[/yellow\]", "<font color='#FEF488'>$1</font>");
            #endregion

            #region 字体变绿 [green][/green]#21B929
            // 字体变绿
            AddReplacement(ref rs, @"\[green\](.+?)\[/green\]", "<font color='#21B929'>$1</font>");
            #endregion

            #region 字体紫色[purple][/purple]#ACAEDE
            // 字体变紫
            AddReplacement(ref rs, @"\[purple\](.+?)\[/purple\]", "<font color='#7D3AA5'>$1</font>");
            #endregion

            #region 字体乳白色[milky][/milky]#D4D4D4
            AddReplacement(ref rs, @"\[milky\](.+?)\[/milky\]", "<font color='#D4D4D4'>$1</font>");
            #endregion

            #region 字体灰色[gray][/gray]#939189
            AddReplacement(ref rs, @"\[gray\](.+?)\[/gray\]", "<font color='#939189'>$1</font>");
            #endregion

            #region 字体橙色[orange][/orange]#eba12c
            AddReplacement(ref rs, @"\[orange\](.+?)\[/orange\]", "<font color='#eba12c'>$1</font>");
            #endregion

            #region 字体颜色 [color=x][/color]
            // 字体颜色
            AddReplacement(ref rs, @"\[color=(?<color>.+?)](?<name>.+?)\[/color]", "<font color='${color}'>${name}</font>");
            #endregion

            #region 字体大小 [size=x][/size]
            // 字体大小
            AddReplacement(ref rs, @"\[size=(?<size>.+?)](?<name>.+?)\[/size]", "<font size='${size}'>${name}</font>");
            #endregion
            //这个先不管
            #region 图片 [img][/img]
            // 图片
            AddReplacement(ref rs, @"\[img](?<img>.+?)\[/img]", "<img src='${img}' alt=''/>");
            #endregion

            #region 链接 [url=link]name[/url]
            // 链接
            AddReplacement(ref rs, @"\[url=(?<url>.+?)](?<name>.+?)\[/url]", "<a href='${url}' target=_blank>${name}</a>");
            #endregion

            #region 字体 [font=x][/font]
            AddReplacement(ref rs, @"\[font=(?<font>.+?)](?<name>.+?)\[/font]", "<font face='${font}'>${name}</font>");
            #endregion

            return rs;
        }

        // for ugui
        public static Dictionary<string, RegexInfo> init_ugui()
        {
            Dictionary<string, RegexInfo> rs = new Dictionary<string, RegexInfo>();

            #region 字体加粗 [b][/b]
            // 字体加粗
            AddReplacement(ref rs, @"\[b\](.+?)\[/b\]", "<b>$1</b>");
            #endregion

            #region 字体斜体 [i][/i]
            // 字体斜体
            AddReplacement(ref rs, @"\[i\](.+?)\[/i\]", "<i>$1</i>");
            #endregion

            #region 字体下划线 [u][/u]
            // 字体下划线
            //AddReplacement(ref rs, @"\[u\](.+?)\[/u\]", "<u>$1</u>");
            #endregion

            #region 字体变红 [red][/red]#BC0605
            // 字体变红
            AddReplacement(ref rs, @"\[red\](.+?)\[/red\]", "<color='#"+ UDefines.CommomColor_Hex(ECommonColor.ECC_Red1)+"'>$1</color>");
            #endregion

            #region 字体变蓝 [blue][/blue]#50E9EF
            // 字体变蓝
            AddReplacement(ref rs, @"\[blue\](.+?)\[/blue\]", "<color='#"+UDefines.CommomColor_Hex(ECommonColor.ECC_Blue2) +"'>$1</color>");
            #endregion

            #region 字体变黄 [yellow][/yellow]#FEF488
            // 字体变黄
            AddReplacement(ref rs, @"\[yellow\](.+?)\[/yellow\]", "<color='#FEF488'>$1</color>");
            #endregion

            #region 字体变绿 [green][/green]#21B929
            // 字体变绿
            AddReplacement(ref rs, @"\[green\](.+?)\[/green\]", "<color='#"+UDefines.CommomColor_Hex(ECommonColor.ECC_Green1) +"'>$1</color>");
            #endregion

            #region 字体紫色[purple][/purple]#ACAEDE
            // 字体变紫
            AddReplacement(ref rs, @"\[purple\](.+?)\[/purple\]", "<color='#"+ UDefines.CommomColor_Hex(ECommonColor.ECC_Purple1)+ "'>$1</color>");
            #endregion

            #region 字体乳白色[milky][/milky]#D4D4D4
            AddReplacement(ref rs, @"\[milky\](.+?)\[/milky\]", "<color='#D4D4D4'>$1</color>");
            #endregion

            #region 字体灰色[gray][/gray]#939189
            AddReplacement(ref rs, @"\[gray\](.+?)\[/gray\]", "<color='#"+ UDefines.CommomColor_Hex(ECommonColor.ECC_Gray)+"'>$1</color>");
            #endregion

            #region 字体橙色[orange][/orange]#eba12c
            AddReplacement(ref rs, @"\[orange\](.+?)\[/orange\]", "<color='#eba12c'>$1</color>");
            #endregion

            #region 字体颜色 [color=x][/color]
            // 字体颜色
            AddReplacement(ref rs, @"\[color=(?<color>.+?)](?<name>.+?)\[/color]", "<color='${color}'>${name}</color>");
            #endregion

            #region 字体大小 [size=x][/size]
            // 字体大小
            AddReplacement(ref rs, @"\[size=(?<size>.+?)](?<name>.+?)\[/size]", "<size='${size}'>${name}</size>");
            #endregion

            #region 换行 <br>
            // 字体变红
            AddReplacement(ref rs, @"\<br>", "\n");
            #endregion
            #region 去 $
            // 字体变红
            AddReplacement(ref rs, @"\$", "\n");
            #endregion
            //这个先不管
            #region 图片 [img][/img]
            // 图片
            AddReplacement(ref rs, @"\[img](?<img>.+?)\[/img]", "<img src='${img}' alt=''/>");
            #endregion

            #region 链接 [url=link]name[/url]
            // 链接
            //AddReplacement(ref rs, @"\[url=(?<url>.+?)](?<name>.+?)\[/url]", "<a href='${url}' target=_blank>${name}</a>");
            #endregion

            #region 字体 [font=x][/font]
            //AddReplacement(ref rs, @"\[font=(?<font>.+?)](?<name>.+?)\[/font]", "<font face='${font}'>${name}</font>");
            #endregion

            return rs;
        }


        /// <summary>
        /// 格式化字符串为HTML
        /// </summary>
        /// <param name="strText">源字符串</param>
        /// <returns>被格式化后的字符串</returns>
        public static void toHtml(ref string strText, UBB_FORMAT_TYPE type = UBB_FORMAT_TYPE.UGUI)
        {
            if (string.IsNullOrEmpty(strText))
            {
                return;
            }

            // 格式化最前面这个字符，有些脚本中带了这个
            strText = strText.TrimStart(new char[] { '$' });

            //<br>和$这两个比较特殊，不能全用[来判断
            bool bStrTextHasUBB = strText.Contains("[") || strText.Contains("<br>");
            if (bStrTextHasUBB == false)
            {
                return;
            }

            Dictionary<string, RegexInfo> dict = (type == UBB_FORMAT_TYPE.SCANFORM ? ris : ris_ugui);
            foreach (KeyValuePair<string, RegexInfo> keypair in dict)
            {
                int nLastPatternIndex = 0;
                string pattern = keypair.Key;
                RegexInfo ri = keypair.Value;
                if (ri.replace.Length > 0)
                {
                    strText = ri.r.Replace(strText, ri.replace);
                }
                else if (ri.del != null)
                {
                    if (ri.isMatch)
                    {
                        strText = ri.del(ri, strText);
                    }
                    else
                    {
                        // TODO 这个还是耗性能
                        int nLoopTime = 0;
                        while (true)
                        {
                            if (nLoopTime > 10)     // 以防万一还是判断下
                            {
                                Debug.Log("the loop is forever");
                                break;
                            }

                            string sPropertyName = pattern.Substring(1, 3); // 暂时用这么捞逼的方法
                            nLastPatternIndex = strText.IndexOf(sPropertyName, nLastPatternIndex);
                            if (nLastPatternIndex == -1)
                            {
                                nLastPatternIndex = 0;
                                break;
                            }

                            string ret = ri.del(ri, strText);
                            if (ret != null)
                            {
                                strText = ri.r.Replace(strText, ret, 1, nLastPatternIndex);
                            }
                            nLastPatternIndex++;
                            nLoopTime++;
                        }
                    }
                }

            }

        }
       
    }
}
