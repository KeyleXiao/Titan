using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using GameLogic;
using USpeedUI;
using UnityEngine;

namespace DataCenter
{
    public class LoudspeakerManager
    {
        // 聊听消息子对象类型
        public enum MsgObjType
        {
            ObjType_Null,       // 无效对象
            ObjType_Text,       // 纯文本对象
        }


        public enum MsgObjSubType
        {
            ObjSubType_Null,                // 无效对象
            ObjSubType_Text_Text,           // 文本对象
            ObjSubType_Text_Prize,          // 奖励对象
        }

        public struct SLoudspeakerObjectInfo
        {
            public MsgObjType type;                         // 对象类型
            public MsgObjSubType subType;                   // 对象子类型
            public String text;                             // 对象正文
            public Dictionary<String, String> param;        // 参数列表
        }

        public struct SLoudspeakerInfo
        {
            public String strNotice;                        // 消息原文
            public List<SLoudspeakerObjectInfo> objList;   // 解析后的对象列表
        }

        private Queue<SLoudspeakerInfo> m_noticeQueue;

        public Queue<SLoudspeakerInfo> MessageList { get { return m_noticeQueue; } }

        // 表示插入物件起点的特殊字符
        public const char ChatItemFlag = (char)21;

        private ResNode m_noticeItemTextRes;           // 文本物件

        public void Init()
        {
            m_noticeQueue = new Queue<SLoudspeakerInfo>();

            m_noticeItemTextRes = AssetBundleManager.GetAssets(AssetType.Asset_Prefab,"UI/Prefabs/Loudspeaker/LoudspeakerItemText");
        }

        public void Clear()
        {
            AssetBundleManager.DeleteAssets(ref m_noticeItemTextRes, true);
        }

		public void ResetData()
		{
			m_noticeQueue.Clear();
		}

        public SLoudspeakerInfo GetLoudspeakerMessage()
        {
            SLoudspeakerInfo info = m_noticeQueue.Dequeue();
            return info;
        }

        public bool HasLoudspeakerMessage()
        {
            return m_noticeQueue.Count > 0;
        }

        public void AddLoudspeakerMessage(ref gamelogic_show_loudspeaker msgInfo)
        {
            // 在战场就不收广播
            if (GameLogicAPI.isInWarScene() > 0)
            {
                return;
            }

            if (msgInfo.message == null)
                return;

            SLoudspeakerInfo info = new SLoudspeakerInfo();
            info.strNotice = msgInfo.message;

            // 分割解析聊天对象
            info.objList = new List<SLoudspeakerObjectInfo>();


            // 加入聊天内容
            int curIndex = 0;
            String normalColor = CreateColorParam(255, 255, 255);

            while (true)
            {
                if (curIndex >= msgInfo.message.Length)
                    break;

                int beginIndex = msgInfo.message.IndexOf(ChatItemFlag, curIndex);
                if (beginIndex < 0)
                {
                    SLoudspeakerObjectInfo txtObj;
                    txtObj.type = MsgObjType.ObjType_Text;
                    txtObj.subType = MsgObjSubType.ObjSubType_Text_Text;
                    txtObj.text = msgInfo.message.Substring(curIndex);
                    txtObj.param = new Dictionary<String, String>();
                    txtObj.param.Add("color", normalColor);
                    info.objList.Add(txtObj);
                    break;
                }
                else
                {
                    if (beginIndex > curIndex)
                    {
                        SLoudspeakerObjectInfo txtObj;
                        txtObj.type = MsgObjType.ObjType_Text;
                        txtObj.subType = MsgObjSubType.ObjSubType_Text_Text;
                        txtObj.text = msgInfo.message.Substring(curIndex, beginIndex - curIndex);
                        txtObj.param = new Dictionary<String, String>();
                        txtObj.param.Add("color", normalColor);
                        info.objList.Add(txtObj);
                    }

                    curIndex = beginIndex + 1;
                    String typeName;
                    Dictionary<String, String> param;
                    String content;
                    if (GetOneChatParam(msgInfo.message, ref curIndex, out typeName, out param, out content) == false)
                        continue;

                    MsgObjType objType;
                    MsgObjSubType subType;
                    GetNoticeObjStringSubType(typeName, true, out objType, out subType);
                    if (objType != MsgObjType.ObjType_Null)
                    {
                        SLoudspeakerObjectInfo msgObj;
                        msgObj.type = objType;
                        msgObj.subType = subType;
                        msgObj.text = content;
                        msgObj.param = param;
                        info.objList.Add(msgObj);
                    }
                }
            }

            // 聊天对象放入列表
            m_noticeQueue.Enqueue(info);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_LOUDSPEAKER_ADD, null);
        }

        public String CreateColorParam(int r, int g, int b)
        {
            return String.Format("{0}+{1}+{2}", r, g, b);
        }

        private bool GetOneChatParam(String message, ref int curIndex, out String typeName, out Dictionary<String, String> param, out String content)
        {
            typeName = "";
            param = null;
            content = "";
            if (message[curIndex] != '[')
                return false;

            int endIndex = message.IndexOf(']', curIndex);
            if (endIndex < 0)
                return false;

            String subStr = message.Substring(curIndex + 1, endIndex - curIndex - 1);
            int paramIndex = subStr.IndexOf(' ');
            if (paramIndex < 0)
            {
                typeName = subStr;
            }
            else if (paramIndex == 0)
            {
                return false;
            }
            else
            {
                typeName = subStr.Substring(0, paramIndex);
                param = new Dictionary<String, String>();
                for (int i = 0; i < 100; ++i)
                {
                    int startIndex = paramIndex + 1;
                    paramIndex = subStr.IndexOf(' ', startIndex);
                    String strSingleParam;
                    if (paramIndex < 0)
                    {
                        strSingleParam = subStr.Substring(startIndex);
                    }
                    else
                    {
                        strSingleParam = subStr.Substring(startIndex, paramIndex - startIndex);
                    }

                    int equalIndex = strSingleParam.IndexOf('=');
                    if (equalIndex < 0)
                        return false;

                    String key = strSingleParam.Substring(0, equalIndex);
                    String val = strSingleParam.Substring(equalIndex + 1);
                    if (!param.ContainsKey(key))
                        param.Add(key, val);

                    if (paramIndex < 0)
                        break;
                }
            }

            int endContentIndex = message.IndexOf("[/" + typeName + "]", endIndex);
            if (endContentIndex < 0)
                return false;

            if (endContentIndex - endIndex - 1 > 0)
                content = message.Substring(endIndex + 1, endContentIndex - endIndex - 1);
            curIndex = endContentIndex + typeName.Length + 3;

            return true;
        }

        public ResNode GetChatItemPrefabByObjType(MsgObjType type)
        {
            switch (type)
            {
                case MsgObjType.ObjType_Text:
                    return m_noticeItemTextRes;
                default:
                    break;
            }

            Debug.LogWarning("LoudspeakerManager GetChatItemPrefabByObjType unknown infotype" + type);
            return null;
        }

        public String GetNoticeObjSubTypeString(MsgObjSubType subType, bool bLimit)
        {
            switch (subType)
            {
                case MsgObjSubType.ObjSubType_Text_Text:
                    return "text";
                case MsgObjSubType.ObjSubType_Text_Prize:
                    return "prize";
                default:
                    break;
            }

            return "";
        }

        public void GetNoticeObjStringSubType(String type, bool bLimit, out MsgObjType objType, out MsgObjSubType subType)
        {
            objType = MsgObjType.ObjType_Null;
            subType = MsgObjSubType.ObjSubType_Null;
            switch (type)
            {
                case "text":
                    {
                        objType = MsgObjType.ObjType_Text;
                        subType = MsgObjSubType.ObjSubType_Text_Text;
                    }
                    break;
                case "prize":
                    {
                        objType = MsgObjType.ObjType_Text;
                        subType = MsgObjSubType.ObjSubType_Text_Prize;
                    }
                    break;
                default:
                    break;
            }
        }
    }
}
