using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using U3D_Render;
using GameLogic;
using Kin_ManagerDef;
using Data.SideButtonConfig;
using USpeedUI.PlayerFrame;

namespace DataCenter
{
    public enum ESideButtonItemType
    {
        ESBT_FirstWin = 1,      // 首胜奖励提示
        ESBT_HeroStar,          // 英雄升星提示
        ESBT_PKShowPrompt,      // 显示PK提示
        ESBT_JoinClan,          // 加入联盟
        ESBT_JoinKin,           // 加入战队
        ESBT_SetRune,           // 设置符文
        ESBT_AntiAddict,        // 注册防沉迷
        ESBT_Punish,            // 惩罚信息
        ESBT_TaskSearch,        // 任务刷新

        ESBT_Max
    }

    public class SideButtonDataManager
    {
        private List<SideButtonListAddData> m_itemList;
        public List<SideButtonListAddData> ItemList
        {
            get { return m_itemList; }
        }
        private Dictionary<int, List<SideButtonListAddData>> m_bakItemList;
        public Dictionary<int, List<SideButtonListAddData>> BakItemList
        {
            get { return m_bakItemList; }
        }

        private bool b_login2StaticGame = true;     // 第一次登陆到静态场景标志
        public bool Login2StaticGame { get { return b_login2StaticGame; } set { b_login2StaticGame = value; } }
        private string m_strAddictTip = null;       // 退出战场后是否有需要弹出的提示框,使用完立即置为null,依据是否为null来判定是否提醒
        public string StrAddictTip { get { return m_strAddictTip; } set { m_strAddictTip = value; } }

        public void Init()
        {
            InitData();
        }

        public void Clear()
        {
            m_itemList.Clear();
            m_bakItemList.Clear();
        }

        public void AddItem(SideButtonListAddData data)
        {
            InitData();
            for (int i = 0; i < m_itemList.Count; ++i)
            {
                if (m_itemList[i].nItemID == data.nItemID)
                {
                    if (data.bAllowRepeatItem == false)
                    {
                        // 已存在，更新内容
                        m_itemList[i] = data;
                    }
                    else
                    {
                        // 已存在，存入后备
                        List<SideButtonListAddData> curBakList;
                        if (m_bakItemList.TryGetValue(data.nItemID, out curBakList))
                        {
                            curBakList.Add(data);
                        }
                        else
                        {
                            curBakList = new List<SideButtonListAddData>();
                            curBakList.Add(data);
                            m_bakItemList[data.nItemID] = curBakList;
                        }
                    }

                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SIDEBUTTONLIST_REFRESH, null);
                    return;
                }
            }

            m_itemList.Add(data);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SIDEBUTTONLIST_REFRESH, null);
        }

        public void RemoveItem(SideButtonListRemoveData data)
        {
            InitData();
            for (int i = 0; i < m_itemList.Count; ++i)
            {
                if (m_itemList[i].nItemID == data.nItemID)
                {
                    List<SideButtonListAddData> curBakList;
                    if (m_bakItemList.TryGetValue(data.nItemID, out curBakList))
                    {
                        // 替换后备
                        m_itemList[i] = curBakList[0];
                        curBakList.RemoveAt(0);
                        if (curBakList.Count == 0)
                        {
                            m_bakItemList.Remove(data.nItemID);
                        }
                    }
                    else
                    {
                        m_itemList.RemoveAt(i);
                    }

                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SIDEBUTTONLIST_REFRESH, null);
                    return;
                }
            }
        }

        private void InitData()
        {
            if (m_itemList == null)
                m_itemList = new List<SideButtonListAddData>();
            if (m_bakItemList == null)
                m_bakItemList = new Dictionary<int, List<SideButtonListAddData>>();
        }
    }
}
