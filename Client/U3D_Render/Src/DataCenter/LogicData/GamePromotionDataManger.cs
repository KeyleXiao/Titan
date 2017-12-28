using ASpeedGame.Data.Scheme;
using GameLogic;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;
using System.Xml;
using UnityEngine;
using USpeedUI;
using USpeedUI.GamePromotion;

namespace DataCenter
{
    public class RecommRecord
    {
        public uint dwMatchNum;
        public int nPrizeTaskData;
        public Dictionary<uint, RecommendRecordItem> dicRecommRecord;

        public RecommRecord()
        {
            dwMatchNum = 0;
            nPrizeTaskData = 0;
            dicRecommRecord = new Dictionary<uint, RecommendRecordItem>();
        }
    }
    public class GamePromotionDataManager
    {
        private const int GAMEID = 7;
        private const string KEY = "9895FD16-FD7B-1B1E-8BC7-DC53144A39C4";
        //private const string HOST = "http://tg.szg1a.net/";
        private const string GET_INFO_AND_USER_LIST = "/ws/RecommendApi.asmx/GetInfoAndUserList?userid={0}&username={1}&gameid={2}&time={3}&sign={4}";
        private RecommRecord m_recommRecord;
        private Texture2D m_RQCode;

        public Texture2D RecommendQRCode
        {
            get
            {
                return m_RQCode;
            }
        }
        public RecommRecord recommRecord
        {
            get
            {
                return m_recommRecord;
            }
        }
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
                if (m_dicWebUrl.TryGetValue(GameWebUrl.SHARE_PERSONAL_LINK, out shareURL))
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
            // 用户ID
            public string UserID
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

            // 总局数
            public string MatchNum
            {
                get;set;
            }

            public void DeserializeFromXML(XmlElement item)
            {
                XmlElement element = (XmlElement)item.SelectSingleNode("UserName");
                if (element != null)
                    UserName = element.InnerText;

                element = (XmlElement)item.SelectSingleNode("UserID");
                if (element != null)
                    UserID = element.InnerText;

                element = (XmlElement)item.SelectSingleNode("TotalExchange");
                if (element != null)
                {
                    TotalExchange = element.InnerText;
                    string szTotalExchange = "";
                    float value;
                    if (float.TryParse(TotalExchange, out value))
                    {
                        value = value / 100;
                        szTotalExchange = value.ToString("0.00");
                    }
                    else
                    {
                        Trace.Log("gamepromotion : parse float failed " + TotalExchange);
                    }
                    TotalExchange = szTotalExchange;
                }

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
                {
                    TotalBalance = element.InnerText;
                    string szTotalBalance = "";
                    float totalBalance;
                    if (float.TryParse(TotalBalance, out totalBalance))
                    {
                        totalBalance = totalBalance / 100;
                        szTotalBalance = totalBalance.ToString("0.00");
                    }
                    else
                    {
                        Trace.Log("gamepromotion : parse float failed " + TotalBalance);
                    }
                    TotalBalance = szTotalBalance;
                }
                
                element = (XmlElement)root.SelectSingleNode("UsableBalance");
                if (element != null)
                {
                    UsableBalance = element.InnerText;
                    string szBalance = "";
                    float balance;
                    if (float.TryParse(UsableBalance, out balance))
                    {
                        balance = balance / 100;
                        szBalance = balance.ToString("0.00");
                    }
                    else
                    {
                        Trace.Log("gamepromotion : parse float failed " + UsableBalance);
                    }
                    UsableBalance = szBalance;
                }

                element = (XmlElement)root.SelectSingleNode("FreezeBalance");
                if (element != null)
                {
                    FreezeBalance = element.InnerText;
                    string szBalance = "";
                    float balance;
                    if (float.TryParse(FreezeBalance, out balance))
                    {
                        balance = balance / 100;
                        szBalance = balance.ToString("0.00");
                    }
                    else
                    {
                        Trace.Log("gamepromotion : parse float failed " + FreezeBalance);
                    }
                    FreezeBalance = szBalance;
                }
                    

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
        }

        public void RequestWebUrl(int nUrlID)
        {
            PromotionWebUrl url = new PromotionWebUrl();
            url.webUrlID = nUrlID;
            EntityEventHelper.Instance.SendCommand<PromotionWebUrl>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQUEST_WEBURL, ref url);
        }

        public System.Collections.IEnumerator BuildRecommendQRCode()
        {
            uint nUserID = GameLogicAPI.getPlayerUserID(EntityFactory.MainHeroView.ID);
            if (nUserID == 0)
            {
                Trace.Log("gamepromotion : get player userid failed.");
            }

            string host = null;
            if (!WebUrl.TryGetValue(GameWebUrl.QRCODE_LINK, out host))
            {
                Trace.Log("gamepromotion : not find QR LINK.");
                yield break;
            }
            if (string.IsNullOrEmpty(host))
            {
                Trace.Log("gamepromotion : QR LINK is empty.");
                yield break;
            }

            string url = string.Format("{0}{1}?u={2}&w=256&c=black&b=white&bw=4&bs=4&bc=orange&t=0&l=", Config.QRBuildUrl, host, nUserID);
            WWW www = new WWW(url);

            yield return www;

            if (www.error != null)
            {
                Debug.LogWarning(www.error);
                yield break;
            }

            m_RQCode = www.texture;

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_GAME_PROMOTION_SET_QRCODE);
        }

        public void SetRecommRecord(IntPtr ptr, int nLen)
        {
            m_recommRecord = new RecommRecord();            
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptr, nLen);
            RecommendRecord record = helper.get<RecommendRecord>();
            m_recommRecord.nPrizeTaskData = record.nPrizeTaskData;
            for (int i = 0; i < record.nCount; ++i)
            {
                RecommendRecordItem item = helper.get<RecommendRecordItem>();
                m_recommRecord.dicRecommRecord[item.dwUserID] = item;
                m_recommRecord.dwMatchNum += item.dwMatchNum;
                Trace.Log("gamepromotion : matchNum=" + item.dwMatchNum + ", userID=" + item.dwUserID);
            }

            MergeData();

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_GAME_PROMOTION_RET_RECOMMEND_DATA);
        }

        public void SetObtainPrizeSuccess(int nParam, string strParam, IntPtr ptrParam, int nPtrLen)
        {
            UIMsgCmdData cmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_GAME_PROMOTION_OBTAIN_PRIZE_SUCCESS, nParam, strParam, ptrParam, nPtrLen);
            int targetID = IntPtrHelper.toData<int>(cmdData.ptrParam);
            if (targetID < 1)
            {
                Trace.Log("gamepromotion : server return targetID failed(" + targetID + ").");
                return;
            }

            if (m_recommRecord != null)
            {
                m_recommRecord.nPrizeTaskData = (1 << (targetID - 1)) | m_recommRecord.nPrizeTaskData;
            }
            Trace.Log("gamepromotion : obtain prize success nPrizeTaskID=" + m_recommRecord.nPrizeTaskData);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_GAME_PROMOTION_OBTAIN_PRIZE_SUCCESS, cmdData);
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

            string time = DateTime.Now.ToString("yyyyMMddHHmm");
            string sign = Md5Sum(string.Format("{0}{1}{2}{3}", nUserID, username, time, KEY));

            string host = null;
            if (!WebUrl.TryGetValue(GameWebUrl.WEB_INTERFACE_LINK, out host))
            {
                Trace.Log("gamepromotion not have host in WebUrl.");
                return null;
            }

            if (string.IsNullOrEmpty(host))
            {
                Trace.Log(string.Format("gamepromotion host is empty"));
                return null;
            }

            string get_info_and_user_list = host + GET_INFO_AND_USER_LIST;
            string url = string.Format(get_info_and_user_list, nUserID, username, GAMEID, time, sign);

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

            MergeData();

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_GAME_PROMOTION_UPDATE_MY_USERS_INFO);
        }

        private void MergeData()
        {
            if (m_recommRecord != null && m_promotionPrizeData != null && m_promotionPrizeData.UserList != null)
            {
                foreach (MyUser user in m_promotionPrizeData.UserList)
                {
                    user.MatchNum = "0";
                    uint userID;
                    if (uint.TryParse(user.UserID, out userID))
                    {
                        RecommendRecordItem value;
                        if (m_recommRecord.dicRecommRecord.TryGetValue(userID, out value))
                        {
                            string szMatchNum = value.dwMatchNum.ToString();
                            user.MatchNum = string.IsNullOrEmpty(szMatchNum) ? "0" : szMatchNum;
                        }
                    }
                }
            }
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
