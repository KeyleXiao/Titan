using System;
using System.Linq;
using System.Collections.Generic;
using System.Text;
using USpeedUI;
using UnityEngine;
using GameLogic;
using USpeedUI.PlayerFrame;
using USpeedUI.PointShop;
using ASpeedGame.Data.TaskViewTimeConfig;
using USpeedUI.TooltipAndDialog;
using Data.SideButtonConfig;
using ASpeedGame.Data.TaskConfig;

namespace DataCenter
{
    // 任务状态
    public enum Task_State_Type
    {
        TaskStateType_Begin = 0,                // 开始
        TaskStateType_Accept,                   // 接受
        TaskStateType_Finish,                   // 完成
        TaskStateType_End,                      // 结束
    };

    // 任务引导类型
    public enum Task_Guide_Type
    {
        TaskGuideType_FirstWin = 1,             // 首胜引导
        TaskGuideType_ClanTeam = 2,             // 联盟组队
        TaskGuideType_JoinMatch = 3,            // 参与比赛
        TaskGuideType_OpenChest = 4,            // 开宝箱
        TaskGuideType_CrownGem = 5,             // 宝石
        TaskGuideType_ClanDonate = 6,           // 联盟捐赠
        TaskGuideType_Mentorship = 7,           // 师徒
        TaskGuideType_HeroDetail = 8,           // 英雄晋升

        TaskGuideType_ShopCard = 9,             // 商城购买勋章
        TaskGuideType_ShopGoods = 10,           // 商城购买物品
        TaskGuideType_ShopPay = 11,             // 商城充值
        TaskGuideType_TaskBox = 12,             // 任务界面
        TaskGuideType_ShopXp = 13,              // 商城购买终结技
        TaskGuideType_ShopGemStone = 14,        // 商城购买水晶
    };

    public enum Task_Level
    {
        Level_1 = 1,
        Level_2,
        Level_3,
    }

    // 任务奖励信息
    public class Task_Prize_Info
    {
        public int nPrizeID;
        public int nCount;
    };

    // 任务节点
    public class Task_List_Node
    {
        public int nTaskID;						// 任务ID
        public long llContactID;                // 交互ID（非交互任务该值为0）
        public int nTaskType;                   // 任务类型
        public int nTaskSubType;                // 任务子类型
        public int nTaskSpecificType;           // 任务具体类型
        public string szName;                   // 任务名
        public int nTaskState;                  // 任务状态
        public string szDescription;            // 任务描述
        public string szHit;                    // 任务点击  
        public int nClientLink;                 // 前往链接 
        public int nPrizeCount;                 // 任务奖励数量
        public Task_Prize_Info[] arrayPrize;    // 任务奖励ID

        // 以下可无
        public int nPublishPDBID;               // 发布者PDBID
        public int nPublishUserID;              // 发布者UserID
        public int nPublishHeadID;              // 发布者头像ID
        public int nPublishSex;                 // 发布者性别
        public long llTagID;                    // 战绩链接
        public int nServerTime;                 // 服务器当前时间
        public int nPublishTime;                // 发布时间
        public string strPublishName;           // 发布者名字
        public string strEmotionWords;          // 发布者心情
        public List<cmd_task_reply_node_c2v> listReply; // 回复信息

        // 辅助变量
        public bool bReplyChange;
        public int nIsTask;
        public int nLastReplyTime;

        public Task_List_Node()
        {
            listReply = new List<cmd_task_reply_node_c2v>();
        }
    };

    // 任务奖励列表
    public class Task_Prize_Node
    {
        public int nTaskID;						// 任务ID
        public string szTaskName;               // 任务名字
        public string szTaskDescription;        // 任务描述
        public int nBindingGoldCount;           // 绑金数量
        public int nExpCount;                   // 经验数量
        public int[] nPrizeID;                  // 奖励ID
        public int nCount;                      // 奖励数组数量   

        public void Clear()
        {
            nTaskID = 0;
            szTaskName = "";
            szTaskDescription = "";
            nBindingGoldCount = 0;
            nExpCount = 0;
            nPrizeID = null;
            nCount = 0;
        }
    };

    // 任务数据中心
    public class TaskDataManager : ITimerHanlder
    {
        // 定时器
        private enum TimerID
        {
            TimerID_RequestFreshFirstWin,   // 首胜刷新定时器
        }
        // 首胜刷新剩余时间
        private int m_nFirstWinRemainTime;
        public int FirstWinRemainTime { get { return m_nFirstWinRemainTime; } }

        // 发布次数
        private int m_nRemainPublishCount;
        public int RemainPublishCount { get { return m_nRemainPublishCount; } }
        // 捕获次数
        private int m_nRemainCatchCount;
        public int RemainCatchCount { get { return m_nRemainCatchCount; } }
        // 刷新次数
        private int m_nRemainReflushCount;
        public int RemainReflushCount { get { return m_nRemainReflushCount; } }
        // 刷新时间
        private int m_nReflushLeftTime;
        public int ReflushLeftTime { get { return m_nReflushLeftTime; } }
        // 接收时间
        private float m_nRecvTime;
        public float RecvTime { get { return m_nRecvTime; } }

        // 拥有任务列表+发布交互列表
        private List<Task_List_Node> m_listTaskData;
        public List<Task_List_Node> ListTaskData { get { return m_listTaskData; } }
        // 预请求任务
        private List<cmd_task_pre_catch_c2v> m_listPreRequestTask;
        public List<cmd_task_pre_catch_c2v> ListPreRequestTask { get { return m_listPreRequestTask; } }
        // 预请求错误消息码
        private int m_nPreCatchMsgId;
        public int PreCatchMsgId { get { return m_nPreCatchMsgId; } set { m_nPreCatchMsgId = value; } }

        // 奖励缓存
        private Task_Prize_Node m_prizeNode;
        public Task_Prize_Node PrizeNode { get { return m_prizeNode; } }

        // 任务引导类
        private TaskGuideManager m_taskGuideManager;
        public TaskGuideManager CTaskGuideManager { get { return m_taskGuideManager; } }
        
        // 初始化
        public void Init()
        {
            m_nRemainCatchCount = 0;
            m_nRemainPublishCount = 0;
            m_nRemainReflushCount = 0;
            m_nReflushLeftTime = 0;

            m_listTaskData = new List<Task_List_Node>();
            m_listPreRequestTask = new List<cmd_task_pre_catch_c2v>();

            m_taskGuideManager = new TaskGuideManager();
            m_taskGuideManager.Init();

            m_prizeNode = new Task_Prize_Node();
        }

        // 接收任务列表
        public void OnRecvTaskListData(string strParam)
        {
            if (strParam == null)
            {
                // 清除任务列表
                m_listTaskData.Clear();
                return;
            }

            // 清除任务列表
            m_listTaskData.Clear();
            int nIndex = 0;
            int nBeginIndex = 0;
            int nEndIndex = 0;
            string strBeginFind = "";
            string strEndFind = "";
            int nStartIndex = 0;

            // 解析出剩余发布次数
            strBeginFind = "<publishcount>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex != -1)
            {
                nIndex = nBeginIndex;
                strEndFind = "</publishcount>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex != -1)
                {
                    nStartIndex = nBeginIndex + strBeginFind.Length;
                    string strRemainPublishCount = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                    m_nRemainPublishCount = Int32.Parse(strRemainPublishCount);
                    nIndex = nEndIndex + strEndFind.Length;
                }
            }

            // 解析出剩余捕获次数
            if (nIndex < strParam.Length)
            {
                strBeginFind = "<catchcount>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex != -1)
                {
                    nIndex = nBeginIndex;
                    strEndFind = "</catchcount>";
                    nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                    if (nEndIndex != -1)
                    {
                        nStartIndex = nBeginIndex + strBeginFind.Length;
                        string strRemainCatchCount = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                        m_nRemainCatchCount = Int32.Parse(strRemainCatchCount);
                        nIndex = nEndIndex + strEndFind.Length;
                    }
                }
            }
            UpdateTaskSearchSideButton();

            // 解析出剩余发布次数
            if (nIndex < strParam.Length)
            {
                strBeginFind = "<reflushcount>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex != -1)
                {
                    nIndex = nBeginIndex;
                    strEndFind = "</reflushcount>";
                    nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                    if (nEndIndex != -1)
                    {
                        nStartIndex = nBeginIndex + strBeginFind.Length;
                        string strRemainReflushCount = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                        m_nRemainReflushCount = Int32.Parse(strRemainReflushCount);
                        nIndex = nEndIndex + strEndFind.Length;
                    }
                }
            }

            // 解析出刷新时间
            if (nIndex < strParam.Length)
            {
                strBeginFind = "<reflushtime>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex != -1)
                {
                    nIndex = nBeginIndex;
                    strEndFind = "</reflushtime>";
                    nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                    if (nEndIndex != -1)
                    {
                        nStartIndex = nBeginIndex + strBeginFind.Length;
                        string strReflushLeftTime = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                        m_nReflushLeftTime = Int32.Parse(strReflushLeftTime);
                        nIndex = nEndIndex + strEndFind.Length;
                    }
                }
            }

            // 记录接收本地时间
            m_nRecvTime = Time.unscaledTime;

            // 解析任务数据
            while (true)
            {
                if (nIndex >= strParam.Length)
                {
                    break;
                }

                // 解析出id
                strBeginFind = "<id>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</id>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出type
                strBeginFind = "<type>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</type>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strType = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出任务名字
                strBeginFind = "<name>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</name>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strName = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出State
                strBeginFind = "<state>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</state>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strSate = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出任务描述
                strBeginFind = "<description>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</description>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strDescription = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出任务奖励
                strBeginFind = "<prize>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</prize>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strPrize = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                Task_Prize_Info[] arrayPrize = null;
                int nPrizeCount = 0;
                if (!string.IsNullOrEmpty(strPrize))
                {
                    string[] split = strPrize.Split(';');
                    if (split.Length >= 2)
                    {
                        int arrayLen = (int)((float)(split.Length) / 2f);
                        arrayPrize = new Task_Prize_Info[arrayLen];
                        for (int i = 0; i < split.Length; i += 2)
                        {
                            if (string.IsNullOrEmpty(split[i]) || string.IsNullOrEmpty(split[i + 1]))
                            {
                                continue;
                            }

                            Task_Prize_Info prizeInfoNode = new Task_Prize_Info();
                            prizeInfoNode.nPrizeID = Int32.Parse(split[i]);
                            prizeInfoNode.nCount = Int32.Parse(split[i + 1]);
                            arrayPrize[nPrizeCount] = prizeInfoNode;
                            ++nPrizeCount;
                        }
                    }
                }
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出任务点击
                strBeginFind = "<hit>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</hit>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strHit = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出任务前往链接
                strBeginFind = "<clientlink>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</clientlink>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strClientLink = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出子类型
                strBeginFind = "<subtype>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</subtype>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strSubType = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出具体类型
                strBeginFind = "<specifictype>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</specifictype>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strSpecificType = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出交互ID
                strBeginFind = "<contacthigh32id>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</contacthigh32id>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strHigh32ContactID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                Int32 nContactHigh32ID = Int32.Parse(strHigh32ContactID);
                nIndex = nEndIndex + strEndFind.Length;

                strBeginFind = "<contactlow32id>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</contactlow32id>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strLow32ContactID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                Int32 nContactLow32ID = Int32.Parse(strLow32ContactID);
                nIndex = nEndIndex + strEndFind.Length;

                long lHigh32 = (((long)nContactHigh32ID) & 0xffffffff) << 32;
                long lLow32 = ((long)nContactLow32ID) & 0xffffffff;
                long llContactID = lHigh32 | lLow32;

                Task_List_Node node = new Task_List_Node();
                node.nTaskID = Int32.Parse(strID);
                node.nTaskType = Int32.Parse(strType);
                node.szName = strName;
                node.nTaskState = Int32.Parse(strSate);
                node.szDescription = strDescription;
                node.arrayPrize = (arrayPrize == null ? node.arrayPrize : arrayPrize);
                node.nPrizeCount = (arrayPrize == null ? 0 : nPrizeCount);
                node.szHit = strHit;
                node.nClientLink = Int32.Parse(strClientLink);
                node.llContactID = llContactID;
                node.nTaskSubType = Int32.Parse(strSubType);
                node.nTaskSpecificType = Int32.Parse(strSpecificType);
                m_listTaskData.Add(node);

                CheckFirstWinTimer(node.nTaskType, node.nTaskState, strHit);
            }
            // 更新奖励提示
            UpdatePrizeTip();
            // 更新红点
            UpdateTaskRedTip();
        }

        // 更新任务列表
        public void OnUpdateTaskListData(string strParam)
        {
            if (strParam == null)
            {
                return;
            }

            int nIndex = 0;
            int nBeginIndex = 0;
            int nEndIndex = 0;
            string strBeginFind = "";
            string strEndFind = "";
            int nStartIndex = 0;
                
            // 解析任务数据
            while (true)
            {
                if (nIndex >= strParam.Length)
                {
                    break;
                }

                // 解析出id
                strBeginFind = "<id>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</id>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出type
                strBeginFind = "<type>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</type>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strType = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出任务名字
                strBeginFind = "<name>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                // 递进
                nIndex = nBeginIndex;
                strEndFind = "</name>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strName = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出State
                strBeginFind = "<state>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</state>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strSate = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出任务描述
                strBeginFind = "<description>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</description>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strDescription = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出任务奖励
                strBeginFind = "<prize>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</prize>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strPrize = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                Task_Prize_Info[] arrayPrize = null;
                int nPrizeCount = 0;
                if (!string.IsNullOrEmpty(strPrize))
                {
                    string[] split = strPrize.Split(';');
                    if (split.Length >= 2)
                    {
                        int arrayLen = (int)((float)(split.Length) / 2f);
                        arrayPrize = new Task_Prize_Info[arrayLen];
                        for (int i = 0; i < split.Length; i += 2)
                        {
                            if (string.IsNullOrEmpty(split[i]) || string.IsNullOrEmpty(split[i + 1]))
                            {
                                continue;
                            }

                            Task_Prize_Info prizeInfoNode = new Task_Prize_Info();
                            prizeInfoNode.nPrizeID = Int32.Parse(split[i]);
                            prizeInfoNode.nCount = Int32.Parse(split[i + 1]);
                            arrayPrize[nPrizeCount] = prizeInfoNode;
                            ++nPrizeCount;
                        }
                    }
                }
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出任务点击
                strBeginFind = "<hit>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</hit>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strHit = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出任务前往链接
                strBeginFind = "<clientlink>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</clientlink>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strClientLink = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出子类型
                strBeginFind = "<subtype>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</subtype>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strSubType = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析出具体类型
                strBeginFind = "<specifictype>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</specifictype>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strSpecificType = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                nIndex = nEndIndex + strEndFind.Length;

                // 解析交互ID
                strBeginFind = "<contacthigh32id>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</contacthigh32id>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strHigh32ContactID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                Int32 nContactHigh32ID = Int32.Parse(strHigh32ContactID);
                nIndex = nEndIndex + strEndFind.Length;

                strBeginFind = "<contactlow32id>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex == -1)
                {
                    break;
                }
                nIndex = nBeginIndex;
                strEndFind = "</contactlow32id>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex == -1)
                {
                    break;
                }
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strLow32ContactID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                Int32 nContactLow32ID = Int32.Parse(strLow32ContactID);
                nIndex = nEndIndex + strEndFind.Length;

                long lHigh32 = (((long)nContactHigh32ID) & 0xffffffff) << 32;
                long lLow32 = ((long)nContactLow32ID) & 0xffffffff;
                long llContactID = lHigh32 | lLow32;

                // 更新任务列表
                Task_List_Node node = new Task_List_Node();
                int nTaskID = Int32.Parse(strID);
                int nHit = m_listTaskData.FindIndex(item => item.nTaskID == nTaskID);
                if (nHit < 0)
                    m_listTaskData.Add(node);
                else
                    node = m_listTaskData[nHit];

                node.nTaskID = Int32.Parse(strID);
                node.nTaskType = Int32.Parse(strType);
                node.szName = strName;
                node.nTaskState = Int32.Parse(strSate);
                node.szDescription = strDescription;
                node.arrayPrize = (arrayPrize == null ? node.arrayPrize : arrayPrize);
                node.nPrizeCount = (arrayPrize == null ? 0 : nPrizeCount);
                node.szHit = strHit;
                node.nClientLink = Int32.Parse(strClientLink);
                node.llContactID = llContactID;
                node.nTaskSubType = Int32.Parse(strSubType);
                node.nTaskSpecificType = Int32.Parse(strSpecificType);

                CheckFirstWinTimer(node.nTaskType, node.nTaskState, strHit);
            }
            // 更新任务显示
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_HOLD, null);
            // 更新展开回复
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_UPDATE, null);
            // 更新奖励提示
            UpdatePrizeTip();
            // 关闭展开界面,首胜任务走更新、不走删除
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_FINISH, null);
            // 更新红点
            UpdateTaskRedTip();
        }

        // 删除任务列表
        public void OnDeleteTaskListData(string strParam)
        {
            if (strParam == null)
                return;

            List<Task_List_Node> tempFinishList = new List<Task_List_Node>();
            List<Task_List_Node> tempDeleteList = new List<Task_List_Node>();

            int nIndex = 0;
            int nBeginIndex = 0;
            int nEndIndex = 0;
            string strBeginFind = "";
            string strEndFind = "";
            int nStartIndex = 0;

            // 解析出任务完成
            strBeginFind = "<taskfinish>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</taskfinish>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string[] strFinish = strParam.Substring(nStartIndex, nEndIndex - nStartIndex).Split(';');
            nIndex = nEndIndex + strEndFind.Length;
            if (strFinish != null)
            {
                for (int i = 0; i < strFinish.Length; i++)
                {
                    if (string.IsNullOrEmpty(strFinish[i]))
                        continue;
                    int nTaskID = Int32.Parse(strFinish[i]);
                    if (nTaskID <= 0)
                        continue;

                    foreach (var item in m_listTaskData)
                    {
                        if (item.nTaskID == nTaskID)
                        {
                            tempFinishList.Add(item);
                        }
                    }
                }
            }

            // 解析出任务删除
            strBeginFind = "<taskdelete>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</taskdelete>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string[] strDelete = strParam.Substring(nStartIndex, nEndIndex - nStartIndex).Split(';');
            nIndex = nEndIndex + strEndFind.Length;
            if (strDelete != null)
            {
                for (int i = 0; i < strDelete.Length; i++)
                {
                    if (string.IsNullOrEmpty(strDelete[i]))
                        continue;
                    int nTaskID = Int32.Parse(strDelete[i]);
                    if (nTaskID <= 0)
                        continue;

                    foreach (var item in m_listTaskData)
                    {
                        if (item.nTaskID == nTaskID)
                        {
                            tempDeleteList.Add(item);
                        }
                    }
                }
            }

            foreach (var item in tempFinishList)
            {
                m_listTaskData.Remove(item);
            }
            foreach (var item in tempDeleteList)
            {
                m_listTaskData.Remove(item);
            }

            // 更新拥有任务
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_HOLD, null);

            // 关闭展开界面
            if (tempFinishList.Count > 0)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_FINISH, null);
            }
            else if (tempDeleteList.Count > 0)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_DELETE, null);
            }
            // 更新红点
            UpdateTaskRedTip();
        }

        // 首胜定时器回调
        public virtual void OnTimer(int nTimerID)
        {
            if (nTimerID == (int)TimerID.TimerID_RequestFreshFirstWin)
            {
                m_nFirstWinRemainTime--;
                if (m_nFirstWinRemainTime <= 0)
                {
                    TimerManager.KillTimer(this, nTimerID);

                    // 刷新首胜任务
                    gamelogic_fresh_task freshTask = new gamelogic_fresh_task();
                    freshTask.nTaskType = (int)ENTASK_TYPE.ENTASK_TYPE_FIRSTWIN;
                    ViewEventHelper.Instance.SendCommand<gamelogic_fresh_task>(GameLogicDef.GVIEWCMD_FRESH_TASK, ref freshTask);
                }
            }
        }

        // 处理首胜任务
        private void CheckFirstWinTimer(int nTaskType, int nTaskState, string szHit)
        {
            if (nTaskType == (int)ENTASK_TYPE.ENTASK_TYPE_FIRSTWIN)
            {
                // 设置首胜DIDA
                bool bUpdateFlag = (nTaskState == (int)Task_State_Type.TaskStateType_Accept);
                UpdateFirstWinSideButtonItem(bUpdateFlag);
                
                // 设置首胜刷新定时器
                if (nTaskState == (int)Task_State_Type.TaskStateType_End)
                {
                    if (!String.IsNullOrEmpty(szHit))
                    {
                        m_nFirstWinRemainTime = Int32.Parse(szHit);
                        if (m_nFirstWinRemainTime > 0)
                        {
                            m_nFirstWinRemainTime += 60;// 延迟一分钟
                            TimerManager.KillTimer(this, (int)TimerID.TimerID_RequestFreshFirstWin);
                            TimerManager.SetTimer(this, (int)TimerID.TimerID_RequestFreshFirstWin, 1.0f);
                            return;
                        }
                    }

                    // 刷新首胜任务
                    gamelogic_fresh_task freshTask = new gamelogic_fresh_task();
                    freshTask.nTaskType = (int)ENTASK_TYPE.ENTASK_TYPE_FIRSTWIN;
                    ViewEventHelper.Instance.SendCommand<gamelogic_fresh_task>(GameLogicDef.GVIEWCMD_FRESH_TASK, ref freshTask);
                }
            }
        }

        // 刷新首胜DIDA
        private void UpdateFirstWinSideButtonItem(bool bShow)
        {
            int playerLv = 0;
            if (EntityFactory.MainHeroView != null)
                playerLv = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            SSchemeSideButtonInfo config = SideButtonConfig.Instance.GetSideButtonConfig((int)ESideButtonItemType.ESBT_FirstWin);

            if (bShow && playerLv >= config.nLowLv && playerLv <= config.nHighLv)
            {
                SideButtonListAddData msg = new SideButtonListAddData();
                msg.nItemID = (int)ESideButtonItemType.ESBT_FirstWin;
                msg.nButtonIconID = config.nIconID;
                msg.strButtonText = config.strName;
                msg.strTooltip = config.strHint;
                msg.callBack = (customData) =>
                {
                    OnTaskGuide((int)Task_Guide_Type.TaskGuideType_FirstWin);
                };
                LogicDataCenter.sideButtonDataManager.AddItem(msg);
            }
            else
            {
                SideButtonListRemoveData msg = new SideButtonListRemoveData();
                msg.nItemID = (int)ESideButtonItemType.ESBT_FirstWin;
                LogicDataCenter.sideButtonDataManager.RemoveItem(msg);
            }
        }

        // 执行任务引导
        public void OnTaskGuide(int nGuideType)
        {
            if (m_taskGuideManager == null)
                return;

            m_taskGuideManager.OnProcessTaskGuide(nGuideType);
        }

        // 刷新奖励提示
        private void UpdatePrizeTip()
        {
            // 战场中不显示任务奖励
            if (GameLogicAPI.isInWarScene() > 0)
                return;

            int nIndex = m_listTaskData.FindIndex(item => item.nTaskState == (int)Task_State_Type.TaskStateType_Finish);
            if (nIndex >= 0)
            {
                PrizeTipData tipData = new PrizeTipData();
                tipData.eTipType = EMPrizeTipType.EMPrizeTipType_Task;
                tipData.nPrizeID = 0;
                LogicDataCenter.prizeTipDataManager.AddItem(tipData);
            }
        }

        // 查看交互基础
        public void OnRecvTaskBase(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_task_node_c2v data = helper.get<cmd_task_node_c2v>();
            if (data.llContactID <= 0)
                return;

            // 无则新建
            Task_List_Node updateNode = new Task_List_Node(); 
            int nIndex = m_listTaskData.FindIndex(item => item.llContactID == data.llContactID);
            if (nIndex < 0)
            {
                m_listTaskData.Add(updateNode);
            }
            else
            {
                updateNode = m_listTaskData[nIndex];
            }
            // 更新
            updateNode.llContactID = data.llContactID;
            updateNode.nPublishPDBID = data.nPublishPDBID;
            updateNode.nPublishUserID = data.nPublishUserID;
            updateNode.nPublishHeadID = data.nPublishHeadID;
            updateNode.nPublishSex = data.nPublishSex;
            updateNode.llTagID = data.llTagID;
            updateNode.nServerTime = data.nServerTime;
            updateNode.nPublishTime = data.nPublishTime;
            updateNode.strPublishName = data.szPublishName;
            updateNode.strEmotionWords = data.szEmotionWords;

            // 查看时间
            TaskViewTimeConfig.Instance.SetTaskViewTime(updateNode.llContactID, updateNode.nServerTime);

            // 更新拥有显示
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_HOLD, null);

            // 更新红点提示
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_REDTIP, null);

            // 显示展开界面 1.查看时 2.捕获时
            UTaskUnfoldData cmdData = new UTaskUnfoldData();
            cmdData.msgID = (int)WndMsgID.WND_MSG_TASK_UNFOLD_PUBLISH_SHOW;
            cmdData.nTaskID = updateNode.nTaskID;
            cmdData.llContactID = updateNode.llContactID;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_PUBLISH_SHOW, cmdData);
        }

        // 查看交互回复
        public void OnRecvTaskReply(IntPtr ptrParam, int nPtrLen)
        {
            // 解析
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            int nCount = helper.get<int>();
            for (int i = 0; i < nCount; i++)
            {
                cmd_task_reply_node_c2v data = helper.get<cmd_task_reply_node_c2v>();
                int nIndex = m_listTaskData.FindIndex(item => item.llContactID == data.llContactID);
                if (nIndex < 0)
                    continue;

                m_listTaskData[nIndex].listReply.Add(data);
                m_listTaskData[nIndex].bReplyChange = true;
            }
            // 更新
            for (int i = 0; i < m_listTaskData.Count; i++)
			{
                if (m_listTaskData[i].bReplyChange)
	            {
                    // 排序
		            m_listTaskData[i].listReply = m_listTaskData[i].listReply.OrderByDescending(item => item.nReplyTime).ToList();
                    m_listTaskData[i].bReplyChange = false;
	            }
			}
            
            // 更新拥有显示
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_HOLD, null);
            // 更新红点提示
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_REDTIP, null);
            // 更新展开回复
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_UPDATE, null);
        }

        // 接收发布任务
        public void OnRecvPublishTask(string strParam)
        {
            if (strParam == null)
            {
                return;
            }

            int nIndex = 0;
            int nBeginIndex = 0;
            int nEndIndex = 0;
            string strBeginFind = "";
            string strEndFind = "";
            int nStartIndex = 0;

            // 解析出剩余发布次数
            strBeginFind = "<publishcount>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</publishcount>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strRemainPublishCount = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            m_nRemainPublishCount = Int32.Parse(strRemainPublishCount);
            nIndex = nEndIndex + strEndFind.Length;

            // 解析交互ID
            strBeginFind = "<contacthigh32id>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</contacthigh32id>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strHigh32ContactID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            Int32 nContactHigh32ID = Int32.Parse(strHigh32ContactID);
            nIndex = nEndIndex + strEndFind.Length;

            strBeginFind = "<contactlow32id>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</contactlow32id>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strLow32ContactID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            Int32 nContactLow32ID = Int32.Parse(strLow32ContactID);
            nIndex = nEndIndex + strEndFind.Length;

            long lHigh32 = (((long)nContactHigh32ID) & 0xffffffff) << 32;
            long lLow32 = ((long)nContactLow32ID) & 0xffffffff;
            long llContactID = lHigh32 | lLow32;
            Task_List_Node node = new Task_List_Node();
            node.llContactID = llContactID;
            m_listTaskData.Add(node);

            // 更新发布次数
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_SEARCH, null);
            // 更新发布次数
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_HOLD, null);

            // 关闭发布界面
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_PUBLISH_CLOSE, null);
        }

        // 接收预捕获任务
        public void OnRecvPreCatchTask(string strParam)
        {
            if (strParam == null)
            {
                m_listPreRequestTask.Clear();
                return;
            }

            // 清空预捕获结果列表
            m_listPreRequestTask.Clear();
            int nIndex = 0;
            int nBeginIndex = 0;
            int nEndIndex = 0;
            string strBeginFind = "";
            string strEndFind = "";
            int nStartIndex = 0;

            // 解析成功与否
            int nSuccess = 0;
            if (nIndex < strParam.Length)
            {
                strBeginFind = "<success>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex != -1)
                {
                    nIndex = nBeginIndex;
                    strEndFind = "</success>";
                    nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                    if (nEndIndex != -1)
                    {
                        nStartIndex = nBeginIndex + strBeginFind.Length;
                        string strSuccess = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                        nSuccess = Int32.Parse(strSuccess);
                        nIndex = nEndIndex + strEndFind.Length;
                    }
                }
            }

            if (nSuccess > 0)
            {
                // 解析出剩余捕获次数
                if (nIndex < strParam.Length)
                {
                    strBeginFind = "<catchcount>";
                    nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                    if (nBeginIndex != -1)
                    {
                        nIndex = nBeginIndex;
                        strEndFind = "</catchcount>";
                        nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                        if (nEndIndex != -1)
                        {
                            nStartIndex = nBeginIndex + strBeginFind.Length;
                            string strRemainCatchCount = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                            m_nRemainCatchCount = Int32.Parse(strRemainCatchCount);
                            nIndex = nEndIndex + strEndFind.Length;
                        }
                    }
                }
                UpdateTaskSearchSideButton();

                for (int i = 0; i < 3; i++)
                {
                    // 解析出任务类型
                    int nTaskType = 0;
                    strBeginFind = "<tasktype>";
                    nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                    if (nBeginIndex != -1)
                    {
                        nIndex = nBeginIndex;
                        strEndFind = "</tasktype>";
                        nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                        if (nEndIndex != -1)
                        {
                            nStartIndex = nBeginIndex + strBeginFind.Length;
                            string strTaskType = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                            nTaskType = Int32.Parse(strTaskType);
                            nIndex = nEndIndex + strEndFind.Length;
                        }
                    }

                    cmd_task_pre_catch_c2v data = new cmd_task_pre_catch_c2v();
                    data.nTaskType = nTaskType;

                    if (nTaskType == (int)ENTASK_TYPE.ENTASK_TYPE_CONTACT)
                    {
                        // 解析交互ID
                        int nContactHigh32ID = 0;
                        strBeginFind = "<contacthigh32id>";
                        nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                        if (nBeginIndex != -1)
                        {
                            nIndex = nBeginIndex;
                            strEndFind = "</contacthigh32id>";
                            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                            if (nEndIndex != -1)
                            {
                                nStartIndex = nBeginIndex + strBeginFind.Length;
                                string strContactHigh32ID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                                nContactHigh32ID = Int32.Parse(strContactHigh32ID);
                                nIndex = nEndIndex + strEndFind.Length;
                            }
                        }
                        int nContactLow32ID = 0;
                        strBeginFind = "<contactlow32id>";
                        nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                        if (nBeginIndex != -1)
                        {
                            nIndex = nBeginIndex;
                            strEndFind = "</contactlow32id>";
                            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                            if (nEndIndex != -1)
                            {
                                nStartIndex = nBeginIndex + strBeginFind.Length;
                                string strContactLow32ID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                                nContactLow32ID = Int32.Parse(strContactLow32ID);
                                nIndex = nEndIndex + strEndFind.Length;
                            }
                        }
                        long lHigh32 = (((long)nContactHigh32ID) & 0xffffffff) << 32;
                        long lLow32 = ((long)nContactLow32ID) & 0xffffffff;
                        data.llContactID = lHigh32 | lLow32;

                        // 解析子类型
                        strBeginFind = "<subtype>";
                        nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                        if (nBeginIndex != -1)
                        {
                            nIndex = nBeginIndex;
                            strEndFind = "</subtype>";
                            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                            if (nEndIndex != -1)
                            {
                                nStartIndex = nBeginIndex + strBeginFind.Length;
                                string strSubType = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                                data.nTaskSubType = Int32.Parse(strSubType);
                                nIndex = nEndIndex + strEndFind.Length;
                            }
                        }

                        // 解析具体类型
                        strBeginFind = "<specifictype>";
                        nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                        if (nBeginIndex != -1)
                        {
                            nIndex = nBeginIndex;
                            strEndFind = "</specifictype>";
                            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                            if (nEndIndex != -1)
                            {
                                nStartIndex = nBeginIndex + strBeginFind.Length;
                                string strSpecificType = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                                data.nTaskSpecificType = Int32.Parse(strSpecificType);
                                nIndex = nEndIndex + strEndFind.Length;
                            }
                        }

                        // 解析发布者ID
                        strBeginFind = "<publisherid>";
                        nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                        if (nBeginIndex != -1)
                        {
                            nIndex = nBeginIndex;
                            strEndFind = "</publisherid>";
                            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                            if (nEndIndex != -1)
                            {
                                nStartIndex = nBeginIndex + strBeginFind.Length;
                                string strPublisherID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                                data.nPublisherDBID = Int32.Parse(strPublisherID);
                                nIndex = nEndIndex + strEndFind.Length;
                            }
                        }
                    }
                    else
                    {
                        // 解析任务ID
                        strBeginFind = "<taskid>";
                        nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                        if (nBeginIndex != -1)
                        {
                            nIndex = nBeginIndex;
                            strEndFind = "</taskid>";
                            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                            if (nEndIndex != -1)
                            {
                                nStartIndex = nBeginIndex + strBeginFind.Length;
                                string strTaskID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                                data.nTaskID = Int32.Parse(strTaskID);
                                nIndex = nEndIndex + strEndFind.Length;
                            }
                        }
                    }
                    m_listPreRequestTask.Add(data);
                }
            }
            else
            {
                // 解析出msgid
                if (nIndex < strParam.Length)
                {
                    strBeginFind = "<msgid>";
                    nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                    if (nBeginIndex != -1)
                    {
                        nIndex = nBeginIndex;
                        strEndFind = "</msgid>";
                        nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                        if (nEndIndex != -1)
                        {
                            nStartIndex = nBeginIndex + strBeginFind.Length;
                            string strMsgId = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                            m_nPreCatchMsgId = Int32.Parse(strMsgId);
                            nIndex = nEndIndex + strEndFind.Length;
                        }
                    }
                }
            }

            // 主界面是否关闭
            if (UISystem.Instance.GetWnd(WndID.WND_ID_TASK_MAIN) != null && UISystem.Instance.GetWnd(WndID.WND_ID_TASK_MAIN).IsVisible())
            {
                // 更新发布次数
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_SEARCH, null);
            }
            else
            {
                // 自动获取一个
                int nCount = m_listPreRequestTask.Count;
                if (nCount <= 0)
                    return;
                    
                int nSeed = (int)Time.unscaledTime;
                System.Random ran = new System.Random(nSeed);
                int nRand = ran.Next();
                nRand = nRand % nCount;

                cmd_task_catch_v2c msg = new cmd_task_catch_v2c();
                msg.nTaskID = m_listPreRequestTask[nRand].nTaskID;
                msg.llContactID = m_listPreRequestTask[nRand].llContactID;
                msg.nTaskType = m_listPreRequestTask[nRand].nTaskType;
                msg.nTaskSubType = m_listPreRequestTask[nRand].nTaskSubType;
                msg.nTaskSpecificType = m_listPreRequestTask[nRand].nTaskSpecificType;
                msg.nPublisherDBID = m_listPreRequestTask[nRand].nPublisherDBID;
                ViewEventHelper.Instance.SendCommand<cmd_task_catch_v2c>(GameLogicDef.GVIEWCMD_CATCH_TASK, ref msg);

                // 清理数据，下次使用无误
                m_listPreRequestTask.Clear();
            }

        }

        // 接收捕获任务
        public void OnRecvCatchTask(string strParam)
        {
            if (strParam == null)
            {
                return;
            }

            int nIndex = 0;
            int nBeginIndex = 0;
            int nEndIndex = 0;
            string strBeginFind = "";
            string strEndFind = "";
            int nStartIndex = 0;

            // 解析任务数据
            // 解析出id
            if (nIndex >= strParam.Length)
                return;
            strBeginFind = "<id>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</id>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            nIndex = nEndIndex + strEndFind.Length;

            // 解析出type
            if (nIndex >= strParam.Length)
                return;
            strBeginFind = "<type>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</type>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strType = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            nIndex = nEndIndex + strEndFind.Length;

            // 解析出任务名字
            if (nIndex >= strParam.Length)
                return;
            strBeginFind = "<name>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</name>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strName = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            nIndex = nEndIndex + strEndFind.Length;

            // 解析出State
            if (nIndex >= strParam.Length)
                return;
            strBeginFind = "<state>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</state>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strSate = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            nIndex = nEndIndex + strEndFind.Length;

            // 解析出任务描述
            if (nIndex >= strParam.Length)
                return;
            strBeginFind = "<description>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</description>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strDescription = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            nIndex = nEndIndex + strEndFind.Length;

            // 解析出任务奖励
            if (nIndex >= strParam.Length)
                return;
            strBeginFind = "<prize>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</prize>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strPrize = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            Task_Prize_Info[] arrayPrize = null;
            int nPrizeCount = 0;
            if (!string.IsNullOrEmpty(strPrize))
            {
                string[] split = strPrize.Split(';');
                if (split.Length >= 2)
                {
                    int arrayLen = (int)((float)(split.Length) / 2f);
                    arrayPrize = new Task_Prize_Info[arrayLen];
                    for (int i = 0; i < split.Length; i += 2)
                    {
                        if (string.IsNullOrEmpty(split[i]) || string.IsNullOrEmpty(split[i + 1]))
                        {
                            continue;
                        }

                        Task_Prize_Info prizeInfoNode = new Task_Prize_Info();
                        prizeInfoNode.nPrizeID = Int32.Parse(split[i]);
                        prizeInfoNode.nCount = Int32.Parse(split[i + 1]);
                        arrayPrize[nPrizeCount] = prizeInfoNode;
                        ++nPrizeCount;
                    }
                }
            }
            nIndex = nEndIndex + strEndFind.Length;

            // 解析出任务点击
            if (nIndex >= strParam.Length)
                return;
            strBeginFind = "<hit>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</hit>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strHit = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            nIndex = nEndIndex + strEndFind.Length;

            // 解析出任务前往链接
            if (nIndex >= strParam.Length)
                return;
            strBeginFind = "<clientlink>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</clientlink>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strClientLink = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            nIndex = nEndIndex + strEndFind.Length;

            // 解析出子类型
            if (nIndex >= strParam.Length)
                return;
            strBeginFind = "<subtype>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</subtype>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strSubType = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            nIndex = nEndIndex + strEndFind.Length;

            // 解析出具体类型
            if (nIndex >= strParam.Length)
                return;
            strBeginFind = "<specifictype>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</specifictype>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strSpecificType = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            nIndex = nEndIndex + strEndFind.Length;

            // 解析交互ID
            strBeginFind = "<contacthigh32id>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</contacthigh32id>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strHigh32ContactID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            int nContactHigh32ID = Int32.Parse(strHigh32ContactID);
            nIndex = nEndIndex + strEndFind.Length;

            strBeginFind = "<contactlow32id>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex == -1)
                return;
            nIndex = nBeginIndex;
            strEndFind = "</contactlow32id>";
            nEndIndex = strParam.IndexOf(strEndFind, nIndex);
            if (nEndIndex == -1)
                return;
            nStartIndex = nBeginIndex + strBeginFind.Length;
            string strLow32ContactID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            int nContactLow32ID = Int32.Parse(strLow32ContactID);
            nIndex = nEndIndex + strEndFind.Length;
            long lHigh32 = (((long)nContactHigh32ID) & 0xffffffff) << 32;
            long lLow32 = ((long)nContactLow32ID) & 0xffffffff;
            long llContactID = lHigh32 | lLow32;

            // 无则新建
            int nTaskID = Int32.Parse(strID);
            Task_List_Node updateNode = new Task_List_Node();
            int nFind = m_listTaskData.FindIndex(item => item.nTaskID == nTaskID);
            if (nFind < 0)
            {
                m_listTaskData.Add(updateNode);
            }
            else
            {
                updateNode = m_listTaskData[nFind];
            }
            updateNode.llContactID = llContactID;
            updateNode.nTaskID = nTaskID;
            updateNode.nTaskType = Int32.Parse(strType);
            updateNode.szName = strName;
            updateNode.nTaskState = Int32.Parse(strSate);
            updateNode.szDescription = strDescription;
            updateNode.arrayPrize = (arrayPrize == null ? updateNode.arrayPrize : arrayPrize);
            updateNode.nPrizeCount = (arrayPrize == null ? 0 : nPrizeCount);
            updateNode.szHit = strHit;
            updateNode.nClientLink = Int32.Parse(strClientLink);
            updateNode.nTaskSubType = Int32.Parse(strSubType);
            updateNode.nTaskSpecificType = Int32.Parse(strSpecificType);

            if (UISystem.Instance.GetWnd(WndID.WND_ID_TASK_MAIN) != null && UISystem.Instance.GetWnd(WndID.WND_ID_TASK_MAIN).IsVisible())
            {
                // 显示展开界面
                UTaskUnfoldData data = new UTaskUnfoldData();
                data.msgID = (int)WndMsgID.WND_MSG_TASK_UNFOLD_TASK_SHOW;
                data.nTaskID = updateNode.nTaskID;
                data.llContactID = updateNode.llContactID;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_TASK_SHOW, data);
            }
            // 更新拥有显示
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_HOLD, null);
            // 更新红点
            UpdateTaskRedTip();
        }

        // 刷新任务操作
        public void OnRecvReflushTaskOpt(string strParam)
        {
            if (strParam == null)
            {
                return;
            }

            int nIndex = 0;
            int nBeginIndex = 0;
            int nEndIndex = 0;
            string strBeginFind = "";
            string strEndFind = "";
            int nStartIndex = 0;

            // 解析出剩余发布次数
            strBeginFind = "<publishcount>";
            nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
            if (nBeginIndex != -1)
            {
                nIndex = nBeginIndex;
                strEndFind = "</publishcount>";
                nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                if (nEndIndex != -1)
                {
                    nStartIndex = nBeginIndex + strBeginFind.Length;
                    string strRemainPublishCount = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                    m_nRemainPublishCount = Int32.Parse(strRemainPublishCount);
                    nIndex = nEndIndex + strEndFind.Length;
                }
            }

            // 解析出剩余捕获次数
            if (nIndex < strParam.Length)
            {
                strBeginFind = "<catchcount>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex != -1)
                {
                    nIndex = nBeginIndex;
                    strEndFind = "</catchcount>";
                    nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                    if (nEndIndex != -1)
                    {
                        nStartIndex = nBeginIndex + strBeginFind.Length;
                        string strRemainCatchCount = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                        m_nRemainCatchCount = Int32.Parse(strRemainCatchCount);
                        nIndex = nEndIndex + strEndFind.Length;
                    }
                }
            }
            UpdateTaskSearchSideButton();

            // 解析出剩余发布次数
            if (nIndex < strParam.Length)
            {
                strBeginFind = "<reflushcount>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex != -1)
                {
                    nIndex = nBeginIndex;
                    strEndFind = "</reflushcount>";
                    nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                    if (nEndIndex != -1)
                    {
                        nStartIndex = nBeginIndex + strBeginFind.Length;
                        string strRemainReflushCount = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                        m_nRemainReflushCount = Int32.Parse(strRemainReflushCount);
                        nIndex = nEndIndex + strEndFind.Length;
                    }
                }
            }

            // 解析出刷新时间
            if (nIndex < strParam.Length)
            {
                strBeginFind = "<reflushtime>";
                nBeginIndex = strParam.IndexOf(strBeginFind, nIndex);
                if (nBeginIndex != -1)
                {
                    nIndex = nBeginIndex;
                    strEndFind = "</reflushtime>";
                    nEndIndex = strParam.IndexOf(strEndFind, nIndex);
                    if (nEndIndex != -1)
                    {
                        nStartIndex = nBeginIndex + strBeginFind.Length;
                        string strReflushLeftTime = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                        m_nReflushLeftTime = Int32.Parse(strReflushLeftTime);
                        nIndex = nEndIndex + strEndFind.Length;
                    }
                }
            }

            // 记录接收本地时间
            m_nRecvTime = Time.unscaledTime;

            // 更新操作次数
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_SEARCH, null);

            // 更新刷新显示
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_REFLUSH_CLOSE, null);
        }

        // 玩家发布任务
        public void OnRecvActorPublishContact(IntPtr ptrParam, int nPtrLen)
        {
            // 解析
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_task_actor_publish_contact_c2v data = helper.get<cmd_task_actor_publish_contact_c2v>();
            for (int i = 0; i < data.nCount; i++)
            {
                cmd_task_node_c2v cmdNode = helper.get<cmd_task_node_c2v>();
                Task_List_Node node = new Task_List_Node();
                node.llContactID = cmdNode.llContactID;
                node.nPublishPDBID = cmdNode.nPublishPDBID;
                node.nPublishUserID = cmdNode.nPublishUserID;
                node.nPublishHeadID = cmdNode.nPublishHeadID;
                node.nPublishSex = cmdNode.nPublishSex;
                node.llTagID = cmdNode.llTagID;
                node.nServerTime = cmdNode.nServerTime;
                node.nPublishTime = cmdNode.nPublishTime;
                node.strPublishName = cmdNode.szPublishName;
                node.strEmotionWords = cmdNode.szEmotionWords;
                m_listTaskData.Add(node);
            }
        }

        // 玩家发布任务
        public void OnRecvContactDataDelete(IntPtr ptrParam, int nPtrLen)
        {
            // 解析
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            long llContactID = helper.get<long>();

            UIDialogData data = new UIDialogData(title: ULocalizationService.Instance.Get("UIView", "Task", "ContactLostTitle"),
                        message: ULocalizationService.Instance.Get("UIView", "Task", "ContactLostContent"),
                        buttons: new UIWidgets.DialogActions() 
                        { 
                            { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), ()=>{ConfirmDeleteContactData(llContactID); return true;} },
                            { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), null}
                        });
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }
        // 确认删除
        private bool ConfirmDeleteContactData(long llContactID)
        {
            int nIndex = m_listTaskData.FindIndex(item => item.llContactID == llContactID);
            if (nIndex >= 0)
            {
                // 服务器
                gamelogic_delete_task_list deleteTaskList = new gamelogic_delete_task_list();
                deleteTaskList.nTaskID = m_listTaskData[nIndex].nTaskID;
                ViewEventHelper.Instance.SendCommand<gamelogic_delete_task_list>(GameLogicDef.GVIEWCMD_DELETE_TASK_LIST, ref deleteTaskList);
                // 客户端
                m_listTaskData.RemoveAt(nIndex);

                // 更新拥有任务
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_HOLD, null);
            }
            return true;
        }

        // 完成奖励弹出
        public void OnRecvSendTaskPrize(string strParam)
        {
            if (strParam == null)
            {
                return;
            }

            m_prizeNode.Clear();

            string strBeginFind = "";
            string strEndFind = "";
            int nStartIndex = 0;
            int nBeginIndex = 0;
            int nEndIndex = 0;

            strBeginFind = "<id>";
            strEndFind = "</id>";
            nBeginIndex = strParam.IndexOf(strBeginFind);
            nEndIndex = strParam.IndexOf(strEndFind);
            if (nBeginIndex != -1 && nEndIndex != -1)
            {
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strID = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                if (!string.IsNullOrEmpty(strID))
                {
                    m_prizeNode.nTaskID = Int32.Parse(strID);
                }
            }

            strBeginFind = "<name>";
            strEndFind = "</name>";
            nBeginIndex = strParam.IndexOf(strBeginFind);
            nEndIndex = strParam.IndexOf(strEndFind);
            if (nBeginIndex != -1 && nEndIndex != -1)
            {
                nStartIndex = nBeginIndex + strBeginFind.Length;
                m_prizeNode.szTaskName = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            }

            strBeginFind = "<description>";
            strEndFind = "</description>";
            nBeginIndex = strParam.IndexOf(strBeginFind);
            nEndIndex = strParam.IndexOf(strEndFind);
            if (nBeginIndex != -1 && nEndIndex != -1)
            {
                nStartIndex = nBeginIndex + strBeginFind.Length;
                m_prizeNode.szTaskDescription = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
            }

            strBeginFind = "<binding_gold>";
            strEndFind = "</binding_gold>";
            nBeginIndex = strParam.IndexOf(strBeginFind);
            nEndIndex = strParam.IndexOf(strEndFind);
            if (nBeginIndex != -1 && nEndIndex != -1)
            {
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strBindingGold = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                if (!string.IsNullOrEmpty(strBindingGold))
                {
                    m_prizeNode.nBindingGoldCount = Int32.Parse(strBindingGold);
                }
            }

            strBeginFind = "<exp>";
            strEndFind = "</exp>";
            nBeginIndex = strParam.IndexOf(strBeginFind);
            nEndIndex = strParam.IndexOf(strEndFind);
            if (nBeginIndex != -1 && nEndIndex != -1)
            {
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strExp = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                if (!string.IsNullOrEmpty(strExp))
                {
                    m_prizeNode.nExpCount = Int32.Parse(strExp);
                }
            }

            strBeginFind = "<prize>";
            strEndFind = "</prize>";
            nBeginIndex = strParam.IndexOf(strBeginFind);
            nEndIndex = strParam.IndexOf(strEndFind);
            if (nBeginIndex != -1 && nEndIndex != -1)
            {
                nStartIndex = nBeginIndex + strBeginFind.Length;
                string strPrize = strParam.Substring(nStartIndex, nEndIndex - nStartIndex);
                if (!string.IsNullOrEmpty(strPrize))
                {
                    string[] split = strPrize.Split(';');

                    int[] nPrizeID = new int[split.Length];
                    for (int i = 0; i < split.Length; ++i)
                    {
                        if (string.IsNullOrEmpty(split[i]))
                        {
                            continue;
                        }

                        nPrizeID[m_prizeNode.nCount++] = Int32.Parse(split[i]);
                    }
                    m_prizeNode.nPrizeID = nPrizeID;
                }
            }

            SSchemeSystemTaskConfig config = TaskConfig.Instance.GetSystemTaskConfig(m_prizeNode.nTaskID);
            if (config != null && config.nTaskType == (int)ENTASK_TYPE.ENTASK_TYPE_RANK)
            {
                UPrizePopupData uiData = new UPrizePopupData();
                uiData.strPrizeTitle = m_prizeNode.szTaskName;
                uiData.strPrizeDesc = "";
                uiData.nExpCount = m_prizeNode.nExpCount;
                uiData.nMoneyCount = m_prizeNode.nBindingGoldCount;
                uiData.nPrizeIdList = new List<int>();
                foreach (var item in m_prizeNode.nPrizeID)
                {
                    if (item > 0)
                    {
                        uiData.nPrizeIdList.Add(item);
                    }
                }
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PRIZEPOPUP_OPEN, uiData);

                m_prizeNode.Clear();
            }

        }

        // 拥有任务列表+发布交互列表（排序）
        public void SetListSequence()
        {
            if (m_listTaskData == null || m_listTaskData.Count <= 0)
                return;

            // 设置排序数据
            foreach (Task_List_Node item in m_listTaskData)
            {
                if (item.nTaskID > 0)
                    item.nIsTask = 1;

                if (item.listReply != null && item.listReply.Count > 0)
                    item.nLastReplyTime = item.listReply[0].nReplyTime;
            }

            List<Task_List_Node> tempList = new List<Task_List_Node>(m_listTaskData);
            // 顺序：拥有>最近回复消息
            IEnumerable<Task_List_Node> ListQuery =
                from item in tempList
                orderby item.nIsTask descending, item.nTaskID ascending, item.nLastReplyTime descending
                select item;

            m_listTaskData.Clear();
            foreach (Task_List_Node item in ListQuery)
            {
                m_listTaskData.Add(item);
            }
        }

        // 更新任务红点提示
        private void UpdateTaskRedTip()
        {
            int nCount = 0;
            foreach (var item in m_listTaskData)
            {
                if (item.nTaskType == (int)ENTASK_TYPE.ENTASK_TYPE_RANK)
                {
                    continue;
                }

                if (item.nTaskState == (int)Task_State_Type.TaskStateType_Finish)
                {
                    nCount++;
                }
            }

            RedPointSetCount uiData = new RedPointSetCount();
            uiData.nSubType = 4;
            uiData.nCount = nCount;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_SETREDPOINTTIP, uiData);
        }

        // 任务刷新提示
        private void UpdateTaskSearchSideButton()
        {
            int playerLv = 0;
            if (EntityFactory.MainHeroView != null)
                playerLv = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
            SSchemeSideButtonInfo config = SideButtonConfig.Instance.GetSideButtonConfig((int)ESideButtonItemType.ESBT_TaskSearch);
            if (m_nRemainCatchCount >0 && playerLv >= config.nLowLv && playerLv <= config.nHighLv)
            {
                SideButtonListAddData addMsg = new SideButtonListAddData();
                addMsg.nItemID = (int)ESideButtonItemType.ESBT_TaskSearch;
                addMsg.nButtonIconID = config.nIconID;
                addMsg.strButtonText = config.strName;
                addMsg.strTooltip = config.strHint;
                addMsg.sCustomData = null;
                addMsg.callBack = (customData) =>
                {
                    UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_SHOW, null);
                };
                LogicDataCenter.sideButtonDataManager.AddItem(addMsg);
            }
            else
            {
                SideButtonListRemoveData removeMsg = new SideButtonListRemoveData();
                removeMsg.nItemID = (int)ESideButtonItemType.ESBT_TaskSearch;
                LogicDataCenter.sideButtonDataManager.RemoveItem(removeMsg);
            }
        }
    }

    // 任务引导类
    public class TaskGuideManager
    {
        // 引导处理
        public delegate void Func_Guide_Handler();
        private Dictionary<int, Func_Guide_Handler> m_GuideHandlerMap;
        public Dictionary<int, Func_Guide_Handler> GuideHandlerMap { get { return m_GuideHandlerMap; } }

        // 回调委托
        public delegate void TaskGuideCallBack();

        // 初始化
        public void Init()
        {
            m_GuideHandlerMap = new Dictionary<int, Func_Guide_Handler>();

            SetFuncGuideHandler((int)Task_Guide_Type.TaskGuideType_FirstWin, OnClanFirstWinGuideHandler);
            SetFuncGuideHandler((int)Task_Guide_Type.TaskGuideType_ClanTeam, OnClanTeamGuideHandler);
            SetFuncGuideHandler((int)Task_Guide_Type.TaskGuideType_JoinMatch, OnJoinMatchGuideHandler);

            SetFuncGuideHandler((int)Task_Guide_Type.TaskGuideType_OpenChest, OnOpenChestGuideHandler);
            SetFuncGuideHandler((int)Task_Guide_Type.TaskGuideType_CrownGem, OnCrownGemGuideHandler);
            SetFuncGuideHandler((int)Task_Guide_Type.TaskGuideType_ClanDonate, OnClanDonateGuideHandler);
            SetFuncGuideHandler((int)Task_Guide_Type.TaskGuideType_Mentorship, OnMentorshipGuideHandler);

            SetFuncGuideHandler((int)Task_Guide_Type.TaskGuideType_HeroDetail, OnHeroDetailGuideHandler);

            SetFuncGuideHandler((int)Task_Guide_Type.TaskGuideType_ShopCard, OnPointShopCardGuideHandler);
            SetFuncGuideHandler((int)Task_Guide_Type.TaskGuideType_ShopGoods, OnPointShopGoodsGuideHandler);
            SetFuncGuideHandler((int)Task_Guide_Type.TaskGuideType_ShopPay, OnPointShopPayGuideHandler);
            SetFuncGuideHandler((int)Task_Guide_Type.TaskGuideType_TaskBox, OnTaskBoxGuideHandler);
            SetFuncGuideHandler((int)Task_Guide_Type.TaskGuideType_ShopXp, OnPointShopXpGuideHandler);
            SetFuncGuideHandler((int)Task_Guide_Type.TaskGuideType_ShopGemStone, OnPointShopGemStoneGuideHandler);
        }

        // 添加引导
        private void SetFuncGuideHandler(int nGuideType, Func_Guide_Handler handler)
        {
            m_GuideHandlerMap[nGuideType] = handler;
        }

        // 执行引导
        public void OnProcessTaskGuide(int nGuideType)
        {
            if (m_GuideHandlerMap.ContainsKey(nGuideType))
            {
                m_GuideHandlerMap[nGuideType]();
            }
        }



        // 联盟首胜引导
        public void OnClanFirstWinGuideHandler()
        {
            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
                return;

            string strTitle = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideTitleDes");
            string strMessage = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideDes");
            string strConfirm = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConfirmDes4");
            TaskGuideCallBack handler = ConveneClanMemberCallBack;

            int nPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            int nKinID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KIN);
            if (nKinID == 0)
            {
                strMessage = strMessage + "\n" + ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideJoinKinDes");
                strConfirm = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConfirmDes1");
                handler = OpenKinListCallBack;
            }
            else
            {
                int nClanID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN);
                if (nClanID == 0)
                {
                    if (nPDBID == LogicDataCenter.kinDataManager.KinBaseData.dwShaikhID)
                    {
                        strMessage = strMessage + "\n" + ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideJoinClanDes");
                        strConfirm = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConfirmDes2");
                        handler = OpenClanListCallBack;
                    }
                    else
                    {
                        strMessage = strMessage + "\n" + ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideTipKinDes");
                        strConfirm = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConfirmDes3");
                        handler = NotifyKinShaikhJoinClanCallBack;
                    }
                }
                else
                {
                    strMessage = strMessage + "\n" + ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConveneMemberDes");
                }
            }

            USpeedUI.TooltipAndDialog.UIDialogData data = new USpeedUI.TooltipAndDialog.UIDialogData(
                                   title: strTitle,
                                   message: strMessage,
                                   buttons: new UIWidgets.DialogActions() { { strConfirm, () => { handler(); return true; } } },
                                   modal: true,
                                   modalColor: new Color(0, 0, 0, 0)
                                   );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        // 联盟组队引导
        public void OnClanTeamGuideHandler()
        {
            if (GameLogicAPI.getTeamID() > 0)
            {
                UISystem.Instance.ShowWnd(WndID.WND_ID_WAR_MAIN_FRAME, true);
            }
            else
            {
                string strTitle = ULocalizationService.Instance.Get("UIView", "Task", "ClanTeamGuideTitleDes");
                string strMessage = ULocalizationService.Instance.Get("UIView", "Task", "ClanTeamGuideTaskDes");
                string strConfirm = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConfirmDes4");
                TaskGuideCallBack handler = ConveneClanMemberCallBack;

                strMessage = strMessage + "\n" + ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConveneMemberDes");

                USpeedUI.TooltipAndDialog.UIDialogData data = new USpeedUI.TooltipAndDialog.UIDialogData(
                                   title: strTitle,
                                   message: strMessage,
                                   buttons: new UIWidgets.DialogActions() { { strConfirm, () => { handler(); return true; } } },
                                   modal: true
                                   );
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
            }
        }

        // 加入比赛引导
        public void OnJoinMatchGuideHandler()
        {
            UISystem.Instance.ShowWnd(WndID.WND_ID_WAR_MAIN_FRAME, true);
        }

        public void OnOpenChestGuideHandler()
        {
            UISystem.Instance.ShowWnd(WndID.WND_ID_PRIZECHEST, true);
        }

        public void OnCrownGemGuideHandler()
        {
            UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_CROWN);
            ButtonType.bIsRequestSelf = true;
            ButtonType.nRequestPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);
        }

        public void OnClanDonateGuideHandler()
        {
            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
                return;

            int nClanID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN);
            if (nClanID > 0)
            {
                UIMsgCmdData uiCmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, (int)SocialFrameButtonType.BTN_CLAN, "", IntPtr.Zero, 0);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, uiCmdData);
                return;
            }

            string strTitle = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideTitleDes");
            string strMessage = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideDes");
            string strConfirm = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConfirmDes1");
            TaskGuideCallBack handler = OpenKinListCallBack;

            int nPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            int nKinID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KIN);
            if (nKinID == 0)
            {
                strMessage = strMessage + "\n" + ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideJoinKinDes");
            }
            else
            {
                if (nClanID == 0)
                {
                    if (nPDBID == LogicDataCenter.kinDataManager.KinBaseData.dwShaikhID)
                    {
                        strMessage = strMessage + "\n" + ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideJoinClanDes");
                        strConfirm = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConfirmDes2");
                        handler = OpenClanListCallBack;
                    }
                    else
                    {
                        strMessage = strMessage + "\n" + ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideTipKinDes");
                        strConfirm = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinGuideConfirmDes3");
                        handler = NotifyKinShaikhJoinClanCallBack;
                    }
                }
            }

            USpeedUI.TooltipAndDialog.UIDialogData data = new USpeedUI.TooltipAndDialog.UIDialogData(
                                   title: strTitle,
                                   message: strMessage,
                                   buttons: new UIWidgets.DialogActions() { { strConfirm, () => { handler(); return true; } } },
                                   modal: true
                                   );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
        }

        public void OnMentorshipGuideHandler()
        {
            UIMsgCmdData uiCmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, (int)SocialFrameButtonType.BTN_MENTORSHIP, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, uiCmdData);
        }

        // 英雄晋级
        public void OnHeroDetailGuideHandler()
        {
            UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_HERO);
            ButtonType.bIsRequestSelf = true;
            ButtonType.nRequestPDBID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);
        }

        //  商城卡牌界面
        public void OnPointShopCardGuideHandler()
        {
            PointShopOpenData uiData = new PointShopOpenData();
            uiData.nPageType = (int)PointShopPageBtnType.PageBtn_AD;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOP_OPEN, uiData);
        }

        //  商城金币界面
        public void OnPointShopGoodsGuideHandler()
        {
            PointShopOpenData uiData = new PointShopOpenData();
            uiData.nPageType = (int)PointShopPageBtnType.PageBtn_Card;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOP_OPEN, uiData);
        }

        //  商城充值界面
        public void OnPointShopPayGuideHandler()
        {
            PointShopOpenData uiData = new PointShopOpenData();
            uiData.nPageType = (int)PointShopPageBtnType.PageBtn_Rune;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOP_OPEN, uiData);
        }

        //  任务界面
        public void OnTaskBoxGuideHandler()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_SHOW, null);
        }

        //  商城终结技界面
        public void OnPointShopXpGuideHandler()
        {
            PointShopOpenData uiData = new PointShopOpenData();
            uiData.nPageType = (int)PointShopPageBtnType.PageBtn_Hero;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOP_OPEN, uiData);
        }

        //  商城水晶界面
        public void OnPointShopGemStoneGuideHandler()
        {
            PointShopOpenData uiData = new PointShopOpenData();
            uiData.nPageType = (int)PointShopPageBtnType.PageBtn_Wing;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_POINTSHOP_OPEN, uiData);
        }

        public void OpenKinListCallBack()
        {
            UIMsgCmdData uiCmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, (int)SocialFrameButtonType.BTN_KIN, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, uiCmdData);
        }

        public void OpenClanListCallBack()
        {
            UIMsgCmdData uiCmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, (int)SocialFrameButtonType.BTN_CLAN, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SOCIALCONTACT_OPEN, uiCmdData);
        }

        public void NotifyKinShaikhJoinClanCallBack()
        {
            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
                return;

            int nKinID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KIN);
            if (nKinID == 0)
                return;

            String strMessage = String.Format(ULocalizationService.Instance.Get("UIView", "Task", "TipShaikhJoinClanDes"), LogicDataCenter.kinDataManager.KinBaseData.szShaikhName, ChatMessageManager.ChatItemFlag);
            UIUtil.SendChatMessage(EMChatChannelType.CHAT_CHANNEL_KIN, strMessage);
        }

        public void ConveneClanMemberCallBack()
        {
            // 联盟邀请组队
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_TEAM_CLAN_INVITE_JOIN_TEAM);
        }
    }
}