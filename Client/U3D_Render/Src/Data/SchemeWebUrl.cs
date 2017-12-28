/*******************************************************************
** 文件名:	SchemeOBControl.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2017 - Speed
** 创建人:	昔文博
** 日  期:	2017/12/6
** 版  本:	1.0
** 描  述:	Web配置 WebUrl.csv
** 应  用:  
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ASpeedGame.Data.Scheme
{
    public enum GameWebUrl
    {
        PAY_LINK = 1000,
        PAY_QR_LINK = 1001,
        QR_BUILD_LINK = 1002,
        TAITAN_ACTIVITIES_LINK = 1003, // 泰坦推广活动链接
        QRCODE_LINK = 1004,            // 二维码链接
        WITHDRAW_LINK = 1005,          // 提现链接  
        WEB_INTERFACE_LINK = 1006,     // 推广员WEB接口链接
        SHARE_PERSONAL_LINK = 1007,    // 分享链接和个人专属链接（带用户ID）
    }

    public class SchemeWebUrlNode
    {
        public int nId;
        public string szName;
        public bool bShowNav;
        public int nClientFuncType;
        public string szPrefixUrl;
        public string szSuffixUrl;

        public SchemeWebUrlNode(int _id, string _name, bool _showNav, int _funcType, string _szPrefixUrl, string _szSuffixUrl)
        {
            nId = _id;
            szName = _name;
            bShowNav = _showNav;
            nClientFuncType = _funcType;
            szPrefixUrl = _szPrefixUrl;
            szSuffixUrl = _szSuffixUrl;
        }
    }

    public class SchemeWebUrl
    {
        enum SCHEME_COLUMN
        {
            COL_ID = 0,                         // ID
            COL_NAME,                           // 名称
            COL_IS_SHOWNAV,                     // 客户端导航显示
            COL_CLIENT_FUNC_TYPE,               // 功能类型(客户端函数channel；0上发到逻辑层)
            COL_IS_AUTOLOGIN,                   // 是否自动登录
            COL_PREV_URL,                       // 前置URL
            COL_FOLLOW_URL,                     // 结尾URL
        }

        public static readonly string schemeFileName = "WebUrl.csv";
        public List<SchemeWebUrlNode> SchemeWebUrlNodeList
        {
            get { return m_dicSchemeWebUrlNodeList; }
        }
        private List<SchemeWebUrlNode> m_dicSchemeWebUrlNodeList = null;
        private static SchemeWebUrl m_instance = new SchemeWebUrl();
        public static SchemeWebUrl Instance
        {
            get { return m_instance; }
        }

        SchemeWebUrl()
        {
            m_dicSchemeWebUrlNodeList = new List<SchemeWebUrlNode>();
        }

        public void UnLoad()
        {
            m_dicSchemeWebUrlNodeList.Clear();
        }

        public void Load(string strFilePath)
        {
            ScpReader reader = new ScpReader(strFilePath, true, 2);
            if (reader == null)
            {
                Trace.LogError("read scheme fail! file:" + strFilePath);
                return;
            }

            m_dicSchemeWebUrlNodeList.Clear();
            int nCount = reader.GetRecordCount();
            for (int row = 0; row < nCount; row++)
            {
                int id = reader.GetInt(row, (int)SCHEME_COLUMN.COL_ID, 0);

                if(m_dicSchemeWebUrlNodeList.Exists(x => x.nId  == id))
                {
                    Trace.LogError("id repeat! file:" + strFilePath);
                    continue;
                }

                string name = reader.GetString(row, (int)SCHEME_COLUMN.COL_NAME, "");
                int showNav = reader.GetInt(row, (int)SCHEME_COLUMN.COL_IS_SHOWNAV, 0);

                //导航不显示的就不读了
                if (showNav <= 0)
                    continue;

                int funcType = reader.GetInt(row, (int)SCHEME_COLUMN.COL_CLIENT_FUNC_TYPE, 0);
                string prefixUrl = reader.GetString(row, (int)SCHEME_COLUMN.COL_PREV_URL, "");
                string suffixUrl = reader.GetString(row, (int)SCHEME_COLUMN.COL_FOLLOW_URL, "");
                SchemeWebUrlNode node = new SchemeWebUrlNode(id, name, true, funcType, prefixUrl, suffixUrl);
                m_dicSchemeWebUrlNodeList.Add(node);
            }
        }
    }
}
