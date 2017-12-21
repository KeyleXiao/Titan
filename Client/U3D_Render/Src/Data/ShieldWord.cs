using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Collections;
using System.Text.RegularExpressions;
using UnityEngine;

namespace ASpeedGame.Data.ShieldWorld
{
    class ShieldWord
    {
        private static ShieldWord singleton = new ShieldWord();
		// 屏蔽词替换字符
		public static char REPLACE_CHAR = '*';

        public static ShieldWord Instance
        {
            get
            {
                if (singleton != null)
                    return singleton;
                else
                    return new ShieldWord();
            }
        }

		public void Load()
		{

		}

		public void UnLoad()
        {
			singleton = null;
		}

		// 判断字符串是否是在屏蔽字集内
		public bool IsConformString(string stInfo)
        {
            if (!IsStringLenRight(stInfo))
            {
                return false;
            }

            if (IsSensitiveAndSpecialString(stInfo))
            {
                return false;
            }

            return true;
        }

        public bool IsSensitiveAndSpecialString(string stInfo)
        {
			return GameLogicAPI.ExistKeyWordOfName(stInfo);
        }

		/// <summary>
		/// 是否含有屏蔽词
		/// </summary>
		/// <param name="stInfo"></param>
		/// <returns></returns>
		/// 适用于名称信息,比如角色名,房间名,联盟名称等等
		public bool IsHaveShieldWord(string stInfo)
        {
			return GameLogicAPI.ExistKeyWordOfName(stInfo);
        }

        /// <summary>
        /// 判断句子中是否含有中文
        /// </summary>
        /// <param >字符串</param>
        public bool strHaveCN(string words)
        {
            string s = words.ToString().Trim();
            string c;

            for (int i = 0; i < s.Length; i++)
            {
                c = s.Substring(i, 1);
                Regex rx = new Regex("^[\u4e00-\u9fa5]$"); // 中文 正则  
                if (rx.IsMatch(c))
                {
                    return true;
                }
            }
            return false;
        }

        public bool IsStringLenRight(string stInfo)
        {
            int nMinLength = 4;
            int nMaxLength = 14;
            Regex r = new Regex(@"[\u4e00-\u9fa5]+");
            Match mc = r.Match(stInfo);
            int nChineseCharacters = mc.Length;

            int nLeng = stInfo.Length + nChineseCharacters;

            if (nLeng >= nMinLength && nLeng <= nMaxLength)
            {
                return true;
            }

            return false;
        }

        public bool IsStringMaxLenRight(string stInfo)
        {
            Regex r = new Regex(@"[\u4e00-\u9fa5]+");
            Match mc = r.Match(stInfo);
            int nChineseCharacters = mc.Length;

            int nMaxLength = 14;
            int nLeng = stInfo.Length + nChineseCharacters;
            if ( nLeng <= nMaxLength)
            {
                return true;
            }

            return false;
        }

        public bool IsStringMinLenRight(string stInfo)
        {
            Regex r = new Regex(@"[\u4e00-\u9fa5]+");
            Match mc = r.Match(stInfo);
            int nChineseCharacters = mc.Length;

            int nMinLength = 4;
            int nLeng = stInfo.Length + nChineseCharacters;
            if (nLeng >= nMinLength)
            {
                return true;
            }

            return false;
        }

        public bool IsStringHeadCharIsNum(string stInfo)
        {
            if(string.IsNullOrEmpty(stInfo))
            {
                return false;
            }

            return Regex.IsMatch(stInfo[0].ToString(), @"^[+-]?\d*$");
        }

		/// <summary>
		/// 过滤屏蔽词，用星号代替
		/// </summary>
		/// <param name="content"></param>
		/// <returns>是否含有屏蔽词</returns>
		/// 适用于聊天内容,公告内容等
		public bool filterShieldWord(ref string  content)
        {
			byte[] outByteArray = null;

			if (!GameLogicAPI.FilterKeyWord(content, out outByteArray))
			{
				return false;
			}
			else
			{
				content = Encoding.UTF8.GetString(outByteArray);
				return true;
			}
		}

		/// <summary>
		/// 过滤屏蔽词，用星号代替
		/// </summary>
		/// <param name="content"></param>
		/// <returns>是否含有屏蔽词</returns>
		/// 适用于名称的过滤,比如角色名,房间名,联盟名称等等
		public bool filterShieldWordOfName(ref string content)
		{
			byte[] outByteArray = null;

			if (!GameLogicAPI.FilterKeyWordOfName(content, out outByteArray))
			{
				return false;
			}
			else
			{
				content = Encoding.UTF8.GetString(outByteArray);
				return true;
			}
		}

		/// <summary>
		/// 判断是否含有屏蔽词,且返回屏蔽词
		/// </summary>
		/// <param name="content"></param>
		/// <param name="shieldWordArray">屏蔽词列表</param>
		/// <returns>是否含有屏蔽词</returns>
		/// 适用于名称的过滤,比如角色名,房间名,联盟名称等等
		public bool IsHaveShieldWordEx(string content, out List<string> shieldWordList)
		{
			string tmpStr = content;
			shieldWordList = new List<string>();

			if (!filterShieldWordOfName(ref tmpStr))
			{
				return false;
			}

			// 根据*号来提取屏蔽词
			int curIndex = 0;
			while(true)
			{
				// 查找第一个*号
				int start = tmpStr.IndexOf(REPLACE_CHAR, curIndex);

				if (start < 0)
					break;

				// 相连的*号视为一个屏蔽词
				int end = start;
				for(end = start; end < tmpStr.Length; ++end)
				{
					if (tmpStr[end] != REPLACE_CHAR)
						break;
				}

				shieldWordList.Add(content.Substring(start, end - start));

				curIndex = end + 1;

				// 已经查找完成
				if (curIndex >= tmpStr.Length)
					break;
			}

			return true;
		}

	}
}
