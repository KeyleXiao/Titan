using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using UnityEngine;
using USpeedUI.TooltipAndDialog;

namespace DataCenter
{
    public enum EMPrizeTipType
    {
        EMPrizeTipType_Invalid = 0,      // 无效标志
        EMPrizeTipType_Task,
        EMPrizeTipType_Mail,
        EMPrizeTipType_Chest,

        EMPrizeTipType_Max
    }

    public class PrizeTipDataManager
    {
        // 数据
        private Dictionary<EMPrizeTipType, PrizeTipData> m_itemList;
        public Dictionary<EMPrizeTipType, PrizeTipData> ItemList
        {
            get { return m_itemList; }
        }

        public void Init()
        {
            m_itemList = new Dictionary<EMPrizeTipType, PrizeTipData>();
        }

        public void Clear()
        {
            m_itemList.Clear();
        }

        public void AddItem(PrizeTipData data)
        {
            if (m_itemList.ContainsKey(data.eTipType))
                return;

            m_itemList.Add(data.eTipType, data);

            if (m_itemList.Count >= 1)
            {
                RefreshViewData(data);
            }
        }

        public void RemoveItem(PrizeTipData data)
        {
            if (!m_itemList.ContainsKey(data.eTipType))
                return;

            m_itemList.Remove(data.eTipType);
        }

        public void RemoveAllItem()
        {
            m_itemList.Clear();
        }

        public void RefreshViewData(PrizeTipData data)
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PRIZETIP_UPDATE, data);
        }

        public bool ShowNextPrizeTipView()
        {
            if (m_itemList.Count <= 0)
                return false;

            foreach (var item in m_itemList)
            {
                RefreshViewData(item.Value);
                return true;
            }

            return false;
        }

        public void OnPrizeTipGuide(EMPrizeTipType eTipType)
        {
            if (eTipType == EMPrizeTipType.EMPrizeTipType_Task)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_SHOW, null);
            }
            else if (eTipType == EMPrizeTipType.EMPrizeTipType_Mail)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_MAILBOX_OPEN, null);
            }
            else if (eTipType == EMPrizeTipType.EMPrizeTipType_Chest)
            {
                UISystem.Instance.ShowWnd(WndID.WND_ID_PRIZECHEST, true);
            }
        }
    }
}
