using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DataCenter;

namespace Data.ChatEmoticonConfig
{
    public enum EMChatEmoticonType
    {
        Emoticon_Default = 1,
        Emoticon_Large,
        Emoticon_Skill,
    }

    public enum EChatEmoticonConfig : int 
    {
        EEmoticon_ID = 0,
        EEmoticon_Serial,
        EEmoticon_Type,
        EEmoticon_Key,
        EEmoticon_CharInput,
        EEmoticon_TipDesc,
        EEmoticon_SellID,
        EEmoticon_skillID,
    }

    public class SSchemeChatEmoticonData
    {
        public int nEmoticonID;
        public int nEmoticonSerialID;
        public int nEmoticonSellID;
        public int nEmoticonSkillID;
        public EMChatEmoticonType eEmoticonType;
        public string szEmoticonKey;
        public string szCharInput;
        public string szTipDesc;
    }
    public class ChatEmoticonConfig : Singleton<ChatEmoticonConfig>, IModelConfig
    {
        public Dictionary<int, List<SSchemeChatEmoticonData>> ChatEmoticonSerialList
        {
            get { return m_chatEmoticonSerialList; }
        }

        private Dictionary<int, SSchemeChatEmoticonData> m_dicChatEmoticonConfig;
        private Dictionary<int, List<SSchemeChatEmoticonData>> m_chatEmoticonSerialList;

        public void Load(string strPath)
        {
            m_dicChatEmoticonConfig = new Dictionary<int, SSchemeChatEmoticonData>();
            m_chatEmoticonSerialList = new Dictionary<int, List<SSchemeChatEmoticonData>>();
            InitChatEmoticonConfigCsv(strPath);
        }

        private void InitChatEmoticonConfigCsv(string strPath)
        {
            ScpReader reader = new ScpReader(strPath, true, 2);
            for (int i = 0; i < reader.GetRecordCount(); i++)
            {
                SSchemeChatEmoticonData sConfig = new SSchemeChatEmoticonData();
                sConfig.nEmoticonID = reader.GetInt(i,(int)EChatEmoticonConfig.EEmoticon_ID, 0);
                sConfig.nEmoticonSerialID = reader.GetInt(i, (int)EChatEmoticonConfig.EEmoticon_Serial, 0);
                sConfig.eEmoticonType = (EMChatEmoticonType)Enum.Parse(typeof(EMChatEmoticonType), reader.GetString(i, (int)EChatEmoticonConfig.EEmoticon_Type, ""));
                sConfig.szEmoticonKey = reader.GetString(i, (int)EChatEmoticonConfig.EEmoticon_Key, string.Empty);
                sConfig.szCharInput = reader.GetString(i, (int)EChatEmoticonConfig.EEmoticon_CharInput, string.Empty);
                sConfig.szTipDesc = reader.GetString(i, (int)EChatEmoticonConfig.EEmoticon_TipDesc, string.Empty);
                sConfig.nEmoticonSellID = reader.GetInt(i, (int)EChatEmoticonConfig.EEmoticon_SellID, 0);
                sConfig.nEmoticonSkillID = reader.GetInt(i, (int)EChatEmoticonConfig.EEmoticon_skillID, 0);

                if (m_dicChatEmoticonConfig.ContainsKey(sConfig.nEmoticonID))
                {
                    Trace.LogWarning("有相同的聊天表情ID" + sConfig.nEmoticonID + "表第" + i + "行");
                }
                else
                    m_dicChatEmoticonConfig.Add(sConfig.nEmoticonID, sConfig);

                if (!m_chatEmoticonSerialList.ContainsKey(sConfig.nEmoticonSerialID))
                {
                    m_chatEmoticonSerialList.Add(sConfig.nEmoticonSerialID, new List<SSchemeChatEmoticonData>());
                }
                m_chatEmoticonSerialList[sConfig.nEmoticonSerialID].Add(sConfig);
            }
            reader.Dispose();
            reader = null;
        }

        public void UnLoad()
        {
            if (m_dicChatEmoticonConfig != null)
            {
                m_dicChatEmoticonConfig.Clear();
                m_dicChatEmoticonConfig = null;
            }
        }

        public string GetEmoticonTipDesc(int emoticonID)
        {
            string desc = "";
            if (m_dicChatEmoticonConfig.ContainsKey(emoticonID))
                desc = m_dicChatEmoticonConfig[emoticonID].szTipDesc;
            return desc;
        }

        public void ReplaceChatEmoticonChar(ref string content)
        {
            if (string.IsNullOrEmpty(content))
                return;

            foreach(KeyValuePair<int, SSchemeChatEmoticonData> data in m_dicChatEmoticonConfig)
            {
                SSchemeChatEmoticonData emotionData = data.Value;
                if (emotionData.szCharInput != "")
                {
                    // 大表情不需要替换（只能通过点击大表情发送，不能通过输入快捷缩写发送）
                    if(emotionData.eEmoticonType == EMChatEmoticonType.Emoticon_Large)
                    {
                        continue;
                    }

                    while(content.Contains(emotionData.szCharInput))
                    {
                        //string newContent = (char)21 + "[emo id=" + data.Key + " " + "animKey=" + emotionData.szEmoticonKey.ToString() + "]" + "[/emo]";
                        string newContent = String.Format("{0}[emo id={1} serialid={2} animkey={3} type={4}] [/emo]", (char)21, data.Key, emotionData.nEmoticonSerialID, emotionData.szEmoticonKey, emotionData.eEmoticonType);
                        content = content.Replace(emotionData.szCharInput, newContent);
                    }
            }
            }
        }
    }
}
