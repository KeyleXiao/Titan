/*******************************************************************
** 文件名:	ULanguageInfo.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-3-30
** 版  本:	1.0
** 描  述:	本地化系统 语言配置
		
********************************************************************/

using System;

namespace USpeedUI
{
    public class ULanguageInfo : IEquatable<ULanguageInfo>
    {
        /// <summary>
        /// 语言名字
        /// </summary>
        public string Name;
        /// <summary>
        /// 语言缩写
        /// </summary>
        public string Abbreviation;


        public ULanguageInfo()
        {

        }
        public ULanguageInfo(string _strName)
        {
            Name = _strName;
        }

        public ULanguageInfo(string _strName, string _strAbb)
        {
            Name = _strName;
            Abbreviation = _strAbb;
        }

        public static readonly ULanguageInfo zhCn = new ULanguageInfo("zh_cn");

        public bool Equals(ULanguageInfo other)
        {
            return other.Name == this.Name;
        }
    }
}
