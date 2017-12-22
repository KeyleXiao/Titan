using ASpeedGame.Data.Scheme;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Xml;
using UnityEngine;
using USpeedUI;

namespace DataCenter
{
    public class GamePromotionDataManager
    {
        private const string KEY = "9895FD16-FD7B-1B1E-8BC7-DC53144A39C4";
        private const string HOST = "http://tg.q1.com";
        private const string GET_INFO_AND_USER_LIST = "/ws/RecommendApi.asmx/GetInfoAndUserList?userid={0}&username={1}&gameid={2}&time={3}&sign={4}";

        private Dictionary<GameWebUrl, string> m_dicWebUrl = new Dictionary<GameWebUrl, string>();
        public Dictionary<GameWebUrl, string> WebUrl
        {
            get
            {
                return m_dicWebUrl;
            }
        }

        public string ShareURL
        {
            get
            {
                string shareURL;
                if (m_dicWebUrl.TryGetValue(GameWebUrl.TAITAN_MAINHOME, out shareURL))
                {
                    return shareURL;
                }

                return null;
            }
        }

        private int m_totalNumberGames;
        public int TotalNumGames
        {
            get
            {
                return m_totalNumberGames;
            }
        }

        public class MyUser
        {
            // 通行证名
            public string UserName
            {
                get; set;
            }
            // 总消费额
            public string TotalPay
            {
                get; set;
            }
            // 总返现额
            public string TotalExchange
            {
                get; set;
            }
            // 注册时间
            public string CreateDatetime
            {
                get; set;
            }

            public void DeserializeFromXML(XmlElement item)
            {
                XmlElement element = (XmlElement)item.SelectSingleNode("UserName");
                if (element != null)
                    UserName = element.InnerText;
                element = (XmlElement)item.SelectSingleNode("TotalPay");
                if (element != null)
                    TotalPay = element.InnerText;
                element = (XmlElement)item.SelectSingleNode("TotalExchange");
                if (element != null)
                    TotalExchange = element.InnerText;
                element = (XmlElement)item.SelectSingleNode("CreateDatetime");
                if (element != null)
                    CreateDatetime = element.InnerText;
            }
        }

        public class PromotionPrizeData
        {
            // 下家数量
            public string MyUserCount
            {
                get; set;
            }
            // 总余额（分）
            public string TotalBalance
            {
                get; set;
            }
            // 可用余额（分）
            public string UsableBalance
            {
                get; set;
            }
            // 冻结余额（分）
            public string FreezeBalance
            {
                get; set;
            }
            // 下家列表
            public List<MyUser> UserList
            {
                get; set;
            }

            public void DeserializeFromXML(XmlElement root)
            {
                if (root == null)
                    return;

                XmlElement element = (XmlElement)root.SelectSingleNode("MyUserCount");
                if (element != null)
                    MyUserCount = element.InnerText;
                element = (XmlElement)root.SelectSingleNode("TotalBalance");
                if (element != null)
                    TotalBalance = element.InnerText;
                element = (XmlElement)root.SelectSingleNode("UsableBalance");
                if (element != null)
                    UsableBalance = element.InnerText;
                element = (XmlElement)root.SelectSingleNode("FreezeBalance");
                if (element != null)
                    FreezeBalance = element.InnerText;

                XmlNodeList nodeList = root.SelectNodes("data");
                if (nodeList != null)
                {
                    UserList = new List<MyUser>();
                    foreach (XmlElement item in nodeList)
                    {
                        MyUser user = new MyUser();
                        user.DeserializeFromXML(item);
                        UserList.Add(user);
                    }
                }
                else
                {
                    UserList = null;
                }
            }
        }

        private PromotionPrizeData m_promotionPrizeData;
        public PromotionPrizeData PromotionPrize {get {return m_promotionPrizeData;} }

        public void Init()
        {
            GetTotalGameNumber();
        }

        // 获取总局数
        private void GetTotalGameNumber()
        {

        }

        private string GetURL()
        {
            if (EntityFactory.MainHeroView == null)
                return null;

            uint nUserID = GameLogicAPI.getPlayerUserID(EntityFactory.MainHeroView.ID);
            if (nUserID == 0)
                return null;

            IntPtr account = GameLogicAPI.getActorClientAccount();
            if (account == null || account == IntPtr.Zero)
            {
                return null;
            }

            string username = Marshal.PtrToStringAnsi(account);
            if (string.IsNullOrEmpty(username))
            {
                return null;
            }

            int gameid = GameLogicAPI.getMasterFromGameWorldID();
            if (gameid == 0)
                return null;

            string time = DateTime.Now.ToString("yyyyMMddHHmm");
            string sign = Md5Sum(string.Format("{0}{1}{2}{3}", nUserID, username, time, KEY));

            string host;
            if (!WebUrl.TryGetValue(GameWebUrl.PROMOTION_LINK, out host))
            {
                Trace.Log("not have host in WebUrl.");
                return null;
            }

            if (string.IsNullOrEmpty(host))
            {
                host = HOST;
            }
            else
            {
                Trace.Log(string.Format("落地页:{0}", host));
            }

            string get_info_and_user_list = host + GET_INFO_AND_USER_LIST;
            string url = string.Format(get_info_and_user_list, nUserID, username, gameid, time, sign);

            return url;
        }

        public System.Collections.IEnumerator GetInfoAndUserList()
        {
            string url = GetURL();
            if (url == null)
                yield break;

            WWW www = new WWW(url);

            yield return www;

            if (www.error != null)
            {
                Debug.LogWarning(www.error);
                yield break;
            }

            ParseDataFromXML(www.text);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_GAME_PROMOTION_UPDATE_MY_USERS_INFO);
        }

        private bool ParseDataFromXML(string httpResult)
        {
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.LoadXml(httpResult);
            XmlElement root = xmlDoc.DocumentElement;
            XmlElement element = (XmlElement)root.SelectSingleNode("retCode");
            if (element == null)
            {
                Debug.LogWarning("not find xmlNoe(retCode)");
                return false;
            }
            else if (element.InnerText != "1")
            {
                element = (XmlElement)root.SelectSingleNode("retMsg");
                if (element != null)
                    Debug.LogWarning(element.Value);

                return false;
            }
            else
            {
                m_promotionPrizeData = new PromotionPrizeData();
                m_promotionPrizeData.DeserializeFromXML(root);
            }

            return true;
        }

        private string Md5Sum(string origin)
        {
            if (string.IsNullOrEmpty(origin))
                return null;

            MD5 md5 = MD5.Create();
            byte[] bs = UTF8Encoding.UTF8.GetBytes(origin);
            byte[] hashBytes = md5.ComputeHash(bs);

            return System.BitConverter.ToString(hashBytes).Replace("-", "").ToLower();
        }
    }
}
