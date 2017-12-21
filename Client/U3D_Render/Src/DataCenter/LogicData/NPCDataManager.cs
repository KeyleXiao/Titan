/*******************************************************************
** 文件名:	NPCDataManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李有红
** 日  期:	2017/2/6
** 版  本:	1.0
** 描  述:	NPC数据管理
		
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using System.Runtime.InteropServices;
using System.Xml;
using USpeedUI;
using UnityEngine;
using USpeedUI.NPC;
using GameLogic;

namespace DataCenter
{
    public enum NPC_VIEW_TYPE
    {
        NPC_VIEW_WAR_WING = 1,              // 翅膀界面
        NPC_VIEW_WAR_GOODS_SHOP = 2,        // 野外NPC商店界面
    }

    public class NPCDataManager
    {
        /// <summary>
        /// NPC菜单功能项
        /// </summary>
        public class NPCTopic
        {
            public string Text { get; private set; }
            public string FuncName { get; private set; }
            public string[] ParamList { get; private set; }
            public string Link { get; private set; }

            public NPCTopic(string text, string func, string strParams)
            {
                Text = text;
                FuncName = func;
                ParamList = strParams.Split(';');

                BuildLink();
            }

            /// <summary>
            /// 生成一个HTTP链接地址
            /// 格式如下：HTMLRequestHandler?1=RequestBuyWings&2=456&3=**abc
            /// </summary>
            private void BuildLink()
            {
                string url = FuncName;

                int i = 1;
                foreach (string param in ParamList)
                {
                    if (i == 1)
                    {
                        url = String.Format("{0}?{1}={2}", url, i, param);
                    }
                    else
                    {
                        url = String.Format("{0}&{1}={2}", url, i, param);
                    }

                    i++;
                }

                Link = url;
            }
        }

		/// <summary>
		/// 点击NPC后，NPC返回的对话框内容
		/// </summary>
        public class NPCResponse
        {
            public int TopicCount { 
                get { 
                    return NPCTopicList.Count; 
                } 
            }

            public uint NpcUID { get; private set; }
            public int NpcViewType { get; private set; }
            public string NpcName { get; private set; }
            public string NpcDialog { get; private set; }
            public List<NPCTopic> NPCTopicList { get; private set; }

            //private List<NPCTopic> m_NPCTopicList;

            public NPCResponse(string strRes)
            {
                NPCTopicList = new List<NPCTopic>();

                XmlDocument xmlDoc = new XmlDocument();
                xmlDoc.LoadXml(strRes);
                XmlElement npcEle = xmlDoc.SelectSingleNode("npc") as XmlElement;

                string uid = npcEle.GetAttribute("uid");
                uint npcUid = 0;
                uint.TryParse(uid, out npcUid);
                NpcUID = npcUid;

                string szNpcViewType = npcEle.GetAttribute("npc_view_type");
                int npcViewType;
                int.TryParse(szNpcViewType, out npcViewType);
                NpcViewType = npcViewType;

                NpcName = npcEle.GetAttribute("name");
                NpcDialog = npcEle.GetAttribute("dialog");

                foreach (XmlNode node in npcEle.SelectNodes("topic"))
                {
                    XmlElement ele = node as XmlElement;
                    string text = ele.InnerText;
                    string func = ele.GetAttribute("function");
                    string strParams = ele.GetAttribute("params");

                    NPCTopicList.Add(new NPCTopic(text, func, strParams));
                }
            }
        }

        public void Init()
        {
          
        }

        public void Clear()
        {

        }

#region 接收服务器数据
        // 打开战场商店
        public void recvNPCOpenWarShop()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_GOODSSTORE_TOGGLEVISIBLE, null);
        }

        // 关闭NPC对话框
        public void closeNPCDialogBox()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_NPC_CLOSE_DIALOGBOX, null);
        }

        // 触发NPC后得到回应
        public void recvNPCResponse(string strRes)
        {
            if (strRes == null)
            {
                return;
            }

            // 处理NPC返回的数据
            NPCResponse res = new NPCResponse(strRes);

            UNPCResponseData data = new UNPCResponseData();
            data.msgID = (int)WndMsgID.WND_MSG_NPC_RESPONSE;
            data.npcResp = res;

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_NPC_RESPONSE, data);
        }
#endregion

#region 向服务器发出请求
        public void reqNPCFunc(uint uidNPC, string url)
        {
            cmd_entity_npc_func cmd = new cmd_entity_npc_func();
            cmd.uidNPC = uidNPC;
            cmd.szHTMLRequest = url;
            EntityEventHelper.Instance.SendCommand<cmd_entity_npc_func>(EntityFactory.getMainPlayer().ID, EntityLogicDef.ENTITY_CMD_NPC_FUNC, ref cmd);
        }
#endregion

    }
}
