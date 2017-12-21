using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI;
using UnityEngine;
using USpeedUI.TooltipAndDialog;

namespace DataCenter
{
    public enum EDIDAType
    {
        EDIDA_Invalid = 0,      // 无效标志
        EDIDA_Countdown,        // 倒计时
        EDIDA_Friend,           // 好友
        EDIDA_Kin,              // 战队
        EDIDA_Clan,             // 联盟
        EDIDA_Chat,             // 聊天
        EDIDA_PK,               // PK
        EDIDA_Team,             // 组队
        EDIDA_Prompt,           // 提醒
        EDIDA_LegendCup,        // 杯赛
        EDIDA_Mentorship = 10,  // 师徒
        EDIDA_InvitePlayGame,   // 邀请一起游戏
        EDIDA_RANK,             // 排位

        EDIDA_Max
    }

    public enum EDIDATimer
    {
        EDT_Left_Timer = 0,       // DIDA定时器
        EDT_Middle_Timer,
        EDT_Right_Timer,
    }

    public enum EDIDALayout : int
    {
        EDT_Left = 0,                    //DIDA布局
        EDT_Middle,
        EDT_Right,
        MAX,
    }

    public class DIDADataManager : ITimerHanlder
    {
        // 所有DIDA数据
        private Dictionary<EDIDALayout, List<DIDAAddData> > m_itemList;
        public Dictionary<EDIDALayout, List<DIDAAddData> > ItemList {get {return m_itemList;}}

        private Dictionary<EDIDALayout, List<float>> m_nTimeList;
        public Dictionary<EDIDALayout, List<float>> TimeList { get { return m_nTimeList; } }
        private const float DIDATIME_INTERVAL = 12 * 60 * 60f;
        private List<float> m_fLastUpdateTimeList;

        // DIDA显示数据
        private const int MAXVIEWNUM = 3;
        public int MaxDidaItemNum { get { return MAXVIEWNUM; } }
        private List<DIDAAddData> m_viewList;
        public List<DIDAAddData> ViewList { get { return m_viewList; } }

        // 标识当前类型是否要显示，不重复显示图标
        private List<bool> m_bIsChooseList;

        public void Init()
        {
            m_itemList = new Dictionary<EDIDALayout,List<DIDAAddData> >();
            m_nTimeList = new Dictionary<EDIDALayout, List<float> >();
            m_fLastUpdateTimeList = new List<float>();

            for(int i = 0; i < (int)EDIDALayout.MAX; i++)
            {
                m_itemList.Add((EDIDALayout)i, new List<DIDAAddData>());
                m_nTimeList.Add((EDIDALayout)i, new List<float>());
                TimerManager.SetTimer(this, i, 1.0f);
                m_fLastUpdateTimeList.Add(Time.unscaledTime);
            }

            m_viewList = new List<DIDAAddData>();
            m_bIsChooseList = new List<bool>();
            m_bIsChooseList.AddRange(Enumerable.Repeat(false, (int)EDIDAType.EDIDA_Max));
        }

        public void Clear()
        {
            m_itemList.Clear();
            m_nTimeList.Clear();
            m_viewList.Clear();
            m_bIsChooseList.Clear();
            m_fLastUpdateTimeList.Clear();

            for (int i = 0; i < (int)EDIDALayout.MAX; i++)
            {
                TimerManager.KillTimer(this, i);
            }
        }

        public void AddItem(DIDAAddData data)
        {
            List<DIDAAddData> dataList = m_itemList[data.eLayout];
            List<float> timeList = m_nTimeList[data.eLayout];

            // 不允许重复，更新条目
            if (data.bAllowRepeatItem == false)
	        {
                for (int i = dataList.Count - 1; i >= 0; --i)
                {
                    if (dataList[i].nItemID == data.nItemID)
                    {
                        dataList.RemoveAt(i);
                        timeList.RemoveAt(i);
                        break;
                    }
                }
	        }
            
            // 重复申请，更新条目
            for (int i = dataList.Count - 1; i >= 0; --i)
            {
                if (dataList[i].nItemID == data.nItemID && dataList[i].sCustomData.Equals(data.sCustomData))
                {
                    dataList.RemoveAt(i);
                    timeList.RemoveAt(i);
                    break;
                }
            }

            dataList.Add(data);
            m_itemList[data.eLayout] = dataList;
            // 设置DIDA提醒时间
            if (data.nDIDATime != 0)
            {
                timeList.Add(data.nDIDATime);
            }
            else
            {
                timeList.Add(DIDATIME_INTERVAL);
            }
            m_nTimeList[data.eLayout] = timeList;
            RefreshViewData(data.eLayout);
        }

        public void RemoveItem(DIDARemoveData data)
        {
            for(int i = 0; i < (int)EDIDALayout.MAX; i++)
            {
                List<DIDAAddData> dataList = m_itemList[(EDIDALayout)i];
                List<float> timeList = m_nTimeList[(EDIDALayout)i];
                for (int j = dataList.Count - 1; j >= 0; --j)
                {
                    if (dataList[j].nItemID == data.nItemID && dataList[j].sCustomData.Equals(data.sCustomData))
                    {
                        dataList.RemoveAt(j);
                        timeList.RemoveAt(j);
                    }
                }
                m_itemList[(EDIDALayout)i] = dataList;
                m_nTimeList[(EDIDALayout)i] = timeList;
                RefreshViewData((EDIDALayout)i);
            }
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)EDIDATimer.EDT_Left_Timer:
                case (int)EDIDATimer.EDT_Middle_Timer:
                case (int)EDIDATimer.EDT_Right_Timer:
                    {
                        float fElapseTime = Time.unscaledTime - m_fLastUpdateTimeList[nTimerID];
                        m_fLastUpdateTimeList[nTimerID] = Time.unscaledTime;

                        bool bChange = false;
                        List<float> timeList = m_nTimeList[(EDIDALayout)nTimerID];
                        List<DIDAAddData> dataList = m_itemList[(EDIDALayout)nTimerID];
                        for (int i = timeList.Count - 1; i >= 0; --i)
                        {
                            timeList[i] -= fElapseTime;

                            if (timeList[i] <= 0)
                            {
                                timeList.RemoveAt(i);
                                dataList.RemoveAt(i);
                                bChange = true;
                            }
                        }
                        m_nTimeList[(EDIDALayout)nTimerID] = timeList;
                        m_itemList[(EDIDALayout)nTimerID] = dataList;

                        if (bChange)
                        {
                            RefreshViewData((EDIDALayout)nTimerID);
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public void RefreshViewData(EDIDALayout layout)
        {
            m_viewList.Clear();

            for (int i = 0; i < m_bIsChooseList.Count; i++)
            {
                m_bIsChooseList[i] = false;
            }

            int nIndex = 0;
            List<DIDAAddData> dataList = m_itemList[layout];
            for (int j = dataList.Count - 1; j >= 0; j--)
            {
                if (nIndex >= MAXVIEWNUM)
                    break;

                if (m_bIsChooseList[dataList[j].nItemID])
                    continue;

                m_viewList.Add(dataList[j]);
                nIndex++;
                m_bIsChooseList[dataList[j].nItemID] = true;
            }
            DIDARefreshData refreshData = new DIDARefreshData();
            refreshData.eLayout = layout;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_DIDA_UPDATE, refreshData);
        }
    }
}
