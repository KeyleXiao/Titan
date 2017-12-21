using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using U3D_Render;
using USpeedUI;
using USpeedUI.UWidgets;
using UnityEngine.UI;
using DataCenter;
using UnityEngine;
using UnityEngine.EventSystems;
using GameLogic;
using UIWidgets;
using DG.Tweening;
using ASpeedGame.Data.TaskViewTimeConfig;
using ASpeedGame.Data.TaskConfig;
using USpeedUI.TooltipAndDialog;
using USpeedUI.UEffect;

namespace USpeedUI.TaskWnd
{
    // 任务主界面Wnd
    public class TaskMainWnd : UIFillWnd<TaskMainWndView>
    {
        // 获取WndID
        public override WndID GetID()
        {
            return WndID.WND_ID_TASK_MAIN;
        }

        // 获取预制体路径
        public override string GetPrefabPath()
        {
            return "Task/TaskMainView";
        }

        // 获取UI所在层
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FILL;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();
            m_isKeyClose = true;

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_HOLD, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_REDTIP, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_SEARCH, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_ONCLICK_GOTO, this);
            
            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_HOLD, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_REDTIP, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_SEARCH, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_ONCLICK_GOTO, this);
        }

        // 接收消息
        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_TASK_MAIN_SHOW:
                    {
                        if (GameLogicAPI.getContactTaskSwitch() <= 0)
                        {
                            UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LOGIN_SERVER_MESSAGE, "该模块在测试中，敬请期待。。。");
                            return;
                        }

                        if (IsVisible())
                            return;

                        SetVisible(true);

                        ReqContactTaskReply();
                        if (m_wndView != null)
                        {
                            m_wndView.OnShowTaskMainWnd();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_MAIN_UPDATE_HOLD:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnUpdateTaskHoldFrame();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_MAIN_UPDATE_REDTIP:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnUpdateRedTip();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_MAIN_UPDATE_SEARCH:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnUpdateSearchFrame();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_MAIN_ONCLICK_GOTO:
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.OnClickCloseBtn();
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        // ESC退出
        public override bool OnCloseKey()
        {
            base.OnCloseKey();

            if (m_wndView != null)
            {
                m_wndView.OnClickCloseBtn();
            }

            return true;
        }

        // 服务器请求交互任务回复
        private void ReqContactTaskReply()
        {
            List<Task_List_Node> taskList = LogicDataCenter.taskDataManager.ListTaskData;

            cmd_task_view_reply_v2c data = new cmd_task_view_reply_v2c();
            data.arrContactId = new long[30];
            data.arrRelpyCount = new int[30];

            int nIndex = 0;
            foreach (var item in taskList)
            {
                // 交互任务
                if (item.llContactID > 0 && nIndex < 30)
                {
                    data.arrContactId[nIndex] = item.llContactID;
                    data.arrRelpyCount[nIndex] = item.listReply.Count;
                    nIndex++;
                }
            }
            data.nTaskCount = nIndex;

            ViewEventHelper.Instance.SendCommand<cmd_task_view_reply_v2c>(GameLogicDef.GVIEWCMD_VIEW_TASK_REPLY, ref data);
        }
    }

    // 任务Item基类
    public abstract class ISingleTaskItem : UCustomUIBehaviour, IPointerClickHandler, IPointerEnterHandler, IPointerExitHandler
    {
        public Image NormalIcon;
        public Image SelectIcon;

        public virtual void OnPointerClick(PointerEventData eventData)
        {

        }

        public virtual void OnPointerEnter(PointerEventData eventData)
        {
            SelectIcon.gameObject.SetActive(true);
        }

        public virtual void OnPointerExit(PointerEventData eventData)
        {
            SelectIcon.gameObject.SetActive(false);
        }
    }

    // 任务搜索模块
    public class TaskSearchItem : ISingleTaskItem
    {
        private cmd_task_pre_catch_c2v m_data;
        private TaskSearchFrame m_taskSearchFrame;

        public GameObject itemAttachPoint;
        private UEffectParamBase itemParam;

        public void SetWndView(TaskSearchFrame view)
        {
            m_taskSearchFrame = view;
        }

        // 随机位置，保证三个Item不重叠随机
        public void SetRandPosition(int nIndex)
        {
            int nTime = (int)Time.unscaledTime;
            System.Random ran = new System.Random(nTime + nIndex);

            // 取（150,450）随机半径
            int nRandRadius = ran.Next(150, 450);

            // 取（-30,30）（60,120）（150，210）随机角度
            int nMinAngle = 90 * nIndex - 30;
            int nMaxAngle = 90 * nIndex + 30;
            int nRandAngle = ran.Next(nMinAngle, nMaxAngle);

            // 设置坐标
            float fRandRadian = nRandAngle * Mathf.PI / 180;
            float fX = nRandRadius * Mathf.Cos(fRandRadian);
            float fY = nRandRadius * Mathf.Sin(fRandRadian);
            this.transform.localPosition = new Vector3(fX, fY, 0.0f);
        }

        public void SetData(cmd_task_pre_catch_c2v data)
        {
            m_data = data;

            // 图标
            int nTaskLevel = TaskConfig.Instance.GetTaskLevel(data.nTaskID, data.nTaskType, data.nTaskSubType, data.nTaskSpecificType);
            if (nTaskLevel == (int)Task_Level.Level_3)
            {
                if (data.nTaskType == (int)ENTASK_TYPE.ENTASK_TYPE_CONTACT)
                {
                    NormalIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "subType_gold_normal_" + data.nTaskSubType, WndID.WND_ID_TASK_MAIN);
                    SelectIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "subType_gold_select_" + data.nTaskSubType, WndID.WND_ID_TASK_MAIN);
                }
                else
                {
                    NormalIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "type_gold_normal_" + data.nTaskType, WndID.WND_ID_TASK_MAIN);
                    SelectIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "type_gold_select_" + data.nTaskType, WndID.WND_ID_TASK_MAIN);
                }
            }
            else
            {
                if (data.nTaskType == (int)ENTASK_TYPE.ENTASK_TYPE_CONTACT)
                {
                    NormalIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "subType_normal_" + data.nTaskSubType, WndID.WND_ID_TASK_MAIN);
                    SelectIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "subType_select_" + data.nTaskSubType, WndID.WND_ID_TASK_MAIN);
                }
                else
                {
                    NormalIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "type_normal_" + data.nTaskType, WndID.WND_ID_TASK_MAIN);
                    SelectIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "type_select_" + data.nTaskType, WndID.WND_ID_TASK_MAIN);
                }
            }


            SelectIcon.gameObject.SetActive(false);

            // 光效
            if (itemParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref itemParam);
            }
            itemParam = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_TaskMain_TaskTemplate, _tfAttachParent: itemAttachPoint.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref itemParam);

            this.gameObject.SetActive(true);
        }

        public void Clear()
        {
            // 关闭光效
            if (itemParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref itemParam);
            }

            this.gameObject.SetActive(false);
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            // 请求获取
            cmd_task_catch_v2c msg = new cmd_task_catch_v2c();
            msg.nTaskID = m_data.nTaskID;
            msg.llContactID = m_data.llContactID;
            msg.nTaskType = m_data.nTaskType;
            msg.nTaskSubType = m_data.nTaskSubType;
            msg.nTaskSpecificType = m_data.nTaskSpecificType;
            msg.nPublisherDBID = m_data.nPublisherDBID;
            ViewEventHelper.Instance.SendCommand<cmd_task_catch_v2c>(GameLogicDef.GVIEWCMD_CATCH_TASK, ref msg);

            // 清理数据，下次使用无误
            LogicDataCenter.taskDataManager.ListPreRequestTask.Clear();

            // 清除数据
            m_taskSearchFrame.ClearData();
        }
    }

    public class TaskSearchFrame : UCustomUIBehaviour
    {
        public Text CatchBtnDesc;
        public Text PublishBtnDesc;
        public Text CatchCountDesc;
        public Text PublishCountDesc;

        public GameObject TaskSearchTemplate;

        private TaskSearchItem[] m_listSearchTask;
        private const int SEARCH_TASK_COUNT = 3;

        private TaskMainWndView m_wndView;
        private float m_fCatchTime = 0.0f;

        public Transform searchAttachPoint;
        private UEffectParamBase searchParam;

        public Image CatchBtnEffect;
        private Sequence CatchBtnEffectSequence;

        Sequence m_logicSequence = null;

        // 蹩脚的需求，三次点击判定，本来模态窗口能逻辑清晰解决，策划非要飘字处理
        private bool m_bCatchConfirmPopWndShow = false;

        // 初始化数据
        protected override void Awake()
        {
            base.Awake();

            CatchBtnDesc.text = ULocalizationService.Instance.Get("UIView", "Task", "CatchBtnDesc");
            PublishBtnDesc.text = ULocalizationService.Instance.Get("UIView", "Task", "PublishBtnDesc");

            //SearchBg.gameObject.SetActive(false);

            m_listSearchTask = new TaskSearchItem[SEARCH_TASK_COUNT];
            for (int i = 0; i < SEARCH_TASK_COUNT; i++)
            {
                GameObject taskItemObj = ResNode.InstantiateRes(TaskSearchTemplate);
                if (!taskItemObj)
                    return;

                m_listSearchTask[i] = taskItemObj.GetComponent<TaskSearchItem>();
                if (!m_listSearchTask[i])
                    return;

                taskItemObj.name = String.Format("TaskSearch{0}", i + 1);
                m_listSearchTask[i].SetWndView(this);
                RectTransform taskItemRect = taskItemObj.transform as RectTransform;
                taskItemRect.SetParent(this.transform, false);
                taskItemObj.SetActive(false);
            }
            TaskSearchTemplate.gameObject.SetActive(false);

            CatchBtnEffect.color = Color.clear;
        }

        // 设置父节点
        public void SetWndView(TaskMainWndView view)
        {
            m_wndView = view;
        }

        // 设置显示
        public void SetData()
        {
            CatchCountDesc.text = String.Format(ULocalizationService.Instance.Get("UIView", "Task", "RemainCountDesc"), LogicDataCenter.taskDataManager.RemainCatchCount);
            PublishCountDesc.text = String.Format(ULocalizationService.Instance.Get("UIView", "Task", "RemainCountDesc"), LogicDataCenter.taskDataManager.RemainPublishCount);

            if (LogicDataCenter.taskDataManager.RemainCatchCount > 0)
            {
                if (CatchBtnEffectSequence == null)
                {
                    Color baseColor = Color.white;
                    baseColor.a = 0f;
                    CatchBtnEffect.color = baseColor;
                    CatchBtnEffectSequence = DOTween.Sequence();
                    CatchBtnEffectSequence.Append(DOTween.ToAlpha(() => CatchBtnEffect.color, x => CatchBtnEffect.color = x, 1f, 0.5f));
                    CatchBtnEffectSequence.Append(DOTween.ToAlpha(() => CatchBtnEffect.color, x => CatchBtnEffect.color = x, 0f, 0.5f).SetDelay(0.2f));
                    CatchBtnEffectSequence.SetLoops(-1, LoopType.Restart);
                    CatchBtnEffectSequence.OnKill(() => CatchBtnEffectSequence = null);
                }
            }
            else
            {
                CatchBtnEffect.color = Color.clear;
                if (CatchBtnEffectSequence != null)
                    CatchBtnEffectSequence.Kill(true);
            }
        }

        // 清空数据
        public void ClearData()
        {
            // 搜索后不获取关闭，强制随机一个
            List<cmd_task_pre_catch_c2v> preCatchList = LogicDataCenter.taskDataManager.ListPreRequestTask;
            int nCount = preCatchList.Count;
            if (nCount > 0)
            {
                // 自动获取一个
                int nSeed = (int)Time.unscaledTime;
                System.Random ran = new System.Random(nSeed);
                int nRand = ran.Next();
                nRand = nRand % nCount;

                cmd_task_catch_v2c msg = new cmd_task_catch_v2c();
                msg.nTaskID = preCatchList[nRand].nTaskID;
                msg.llContactID = preCatchList[nRand].llContactID;
                msg.nTaskType = preCatchList[nRand].nTaskType;
                msg.nTaskSubType = preCatchList[nRand].nTaskSubType;
                msg.nTaskSpecificType = preCatchList[nRand].nTaskSpecificType;
                msg.nPublisherDBID = preCatchList[nRand].nPublisherDBID;
                ViewEventHelper.Instance.SendCommand<cmd_task_catch_v2c>(GameLogicDef.GVIEWCMD_CATCH_TASK, ref msg);

                preCatchList.Clear();
            }

            for (int i = 0; i < m_listSearchTask.Length; i++)
            {
                m_listSearchTask[i].Clear();
            }

            // 关闭光效
            if (searchParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref searchParam);
                searchParam = null;
            }

            m_bCatchConfirmPopWndShow = false;
            m_fCatchTime = 0;
            LogicDataCenter.taskDataManager.PreCatchMsgId = 0;
        }

        // 清理
        public void Clear()
        {
            ClearData();
            // 搜索按钮效果
            CatchBtnEffect.color = Color.clear;
            if (CatchBtnEffectSequence != null)
                CatchBtnEffectSequence.Kill(true);
            // 关闭动画
            if (m_logicSequence != null)
            {
                m_logicSequence.Kill();
            }
        }

        // 搜索按钮响应(三次点击确认,真恶心)
        public void OnClickCatchBtn()
        {
            // 确认弹窗已弹出，引导先确认
            if (m_bCatchConfirmPopWndShow)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LUA_TASK_CONFIRM_CATCH_ONE_TASK);
                return;
            }

            // 判是否选择上次随机结果
            if (LogicDataCenter.taskDataManager.ListPreRequestTask.Count > 0 && m_listSearchTask[0].gameObject.activeSelf)
            {
                UIDialogData data = new UIDialogData(title: ULocalizationService.Instance.Get("UIView", "Task", "SearchMsgBoxTitle"),
                            message: ULocalizationService.Instance.Get("UIView", "Task", "SearchMsgBoxDesc"),
                            buttons: new UIWidgets.DialogActions() 
                            { 
                                { ULocalizationService.Instance.Get("UIView", "Common", "ConfirmBtn"), OnConfirmBtnClick },
                                { ULocalizationService.Instance.Get("UIView", "Common", "CancelBtn"), OnCancelBtnClick}
                            },
                            closeButtonAction: new UnityEngine.Events.UnityAction(OnCloseBtnClick));
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);

                m_bCatchConfirmPopWndShow = true;
                return;
            }

            ConfirmClickCatchBtn();
        }

        private bool OnConfirmBtnClick()
        {
            if (LogicDataCenter.taskDataManager.ListPreRequestTask.Count <= 0)
                return false;

            int nSeed = (int)Time.unscaledTime;
            System.Random ran = new System.Random(nSeed);
            int nRand = ran.Next();
            nRand = nRand % SEARCH_TASK_COUNT;
            m_listSearchTask[nRand].OnPointerClick(null);

            m_bCatchConfirmPopWndShow = false;
            return true;
        }

        private bool OnCancelBtnClick()
        {
            m_bCatchConfirmPopWndShow = false;
            return true;
        }

        private void OnCloseBtnClick()
        {
            m_bCatchConfirmPopWndShow = false;
        }

        private bool ConfirmClickCatchBtn()
        {
            // 3秒搜索时间
            if (Time.unscaledTime - m_fCatchTime < 3.0f)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LUA_TASK_SEARCHING);
                return false;
            }
            m_fCatchTime = Time.unscaledTime;

            // 判搜索次数，显示刷新界面
            if (LogicDataCenter.taskDataManager.RemainCatchCount <= 0)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_REFLUSH_SHOW, null);
                return false;
            }

            // 判任务总数量
            List<Task_List_Node> taskList = LogicDataCenter.taskDataManager.ListTaskData;
            if (taskList == null)
                return false;
            int nCount = 0;
            foreach (var item in taskList)
            {
                if (item.nTaskID > 0)
                {
                    nCount++;
                }
            }
            if (nCount >= 15)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LUA_TASK_MAX);
                return false;
            }

            // 发送预获取请求
            cmd_task_pre_catch_v2c msg = new cmd_task_pre_catch_v2c();
            msg.arrContactId = new long[10];
            int nIndex = 0;
            for (int i = 0; i < taskList.Count && nIndex < 10; i++)
            {
                // taskList[i].llContactID保证交互，taskList[i].nTaskID保证任务
                if (taskList[i].llContactID > 0 && taskList[i].nTaskID > 0)
                {
                    msg.arrContactId[nIndex++] = taskList[i].llContactID;
                }
            }
            msg.nCount = nIndex;
            ViewEventHelper.Instance.SendCommand<cmd_task_pre_catch_v2c>(GameLogicDef.GVIEWCMD_PRE_CATCH_TASK, ref msg);

            // 关闭其他界面
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_CLOSE_OTHER, null);

            LogicDataCenter.taskDataManager.ListPreRequestTask.Clear();

            for (int i = 0; i < m_listSearchTask.Length; i++)
            {
                m_listSearchTask[i].Clear();
            }

            // 光效
            m_logicSequence = DOTween.Sequence();
            m_logicSequence.AppendInterval(3.0f).OnComplete(OnEffectComplete);
            if (searchParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref searchParam);
                searchParam = null;
            }
            searchParam = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_TaskUnfold_Search, _tfAttachParent: searchAttachPoint, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref searchParam);

            return true;
        }

        // 动画结束回调
        private void OnEffectComplete()
        {
            // 关闭光效
            if (searchParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref searchParam);
                searchParam = null;
            }

            List<cmd_task_pre_catch_c2v> listTask = LogicDataCenter.taskDataManager.ListPreRequestTask;
            if (listTask != null && listTask.Count > 0)                 // 有结果
            {
                for (int i = 0; i < listTask.Count && i < m_listSearchTask.Length; i++)
                {
                    m_listSearchTask[i].SetRandPosition(i);
                    m_listSearchTask[i].SetData(listTask[i]);
                }
            }
            else if (LogicDataCenter.taskDataManager.PreCatchMsgId > 0)// 无结果
            {
                UIUtil.ShowSystemMessage((EMChatTipID)LogicDataCenter.taskDataManager.PreCatchMsgId);
                LogicDataCenter.taskDataManager.PreCatchMsgId = 0;
            }
        }

        // 发布按钮响应
        public void OnClickPublishBtn()
        {
            // 判次数，显示刷新界面
            if (LogicDataCenter.taskDataManager.RemainPublishCount <= 0)
            {
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_REFLUSH_SHOW, null);
                return;
            }

            m_wndView.PublishFrame.SetData();
        }
    }

    // 任务发布模块
    public class TaskPublishItem : ISingleTaskItem
    {
        public Text SubTypeDesc;

        private int m_nSubTypeID;
        private TaskPublishFrame m_publishFrame;

        public void SetWndView(TaskPublishFrame view)
        {
            m_publishFrame = view;
        }

        public void SetData(int nSubTypeId, string strSubTypeDesc)
        {
            if (nSubTypeId <= 0 || String.IsNullOrEmpty(strSubTypeDesc))
                return;

            m_nSubTypeID = nSubTypeId;
            NormalIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "subType_normal_" + nSubTypeId, WndID.WND_ID_TASK_MAIN);
            SelectIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "subType_select_" + nSubTypeId, WndID.WND_ID_TASK_MAIN);
            SubTypeDesc.text = strSubTypeDesc;
            SelectIcon.gameObject.SetActive(false);

            this.gameObject.SetActive(true);
        }

        public void SetDeselect()
        {
            SelectIcon.gameObject.SetActive(false);
        }

        public override void OnPointerClick(PointerEventData eventData)
        {
            // 关闭模态窗口
            m_publishFrame.Hide();

            // 显示发布界面
            UIMsgCmdData cmdData = UIMsgCmdDataHelper.Instance.create(WndMsgID.WND_MSG_TASK_PUBLISH_SHOW, m_nSubTypeID, null);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_PUBLISH_SHOW, cmdData);
        }
    }

    public class TaskPublishFrame : UCustomUIBehaviour
    {
        public RectTransform PublishItemFrame;
        public GameObject PublishItemTemplate;

        private TaskPublishItem[] m_listPublishItem;
        private int m_nModelKey;

        // 初始化
        protected override void Awake()
        {
            base.Awake();

            // 初始化发布子类型模块
            Dictionary<int, List<SSchemeContactTaskConfig>> mapTaskConfig = TaskConfig.Instance.MapContactTaskConfig;
            if (mapTaskConfig == null)
                return;
            int nItemCount = mapTaskConfig.Count;
            if (nItemCount <= 0)
                return;

            m_listPublishItem = new TaskPublishItem[nItemCount];
            for (int i = 0; i < nItemCount; i++)
            {
                GameObject publishItemObj = ResNode.InstantiateRes(PublishItemTemplate);
                if (!publishItemObj)
                    return;

                m_listPublishItem[i] = publishItemObj.GetComponent<TaskPublishItem>();
                if (!m_listPublishItem[i])
                    return;

                publishItemObj.name = String.Format("TaskPublish{0}", i + 1);
                m_listPublishItem[i].SetWndView(this);
                RectTransform publishItemRect = publishItemObj.transform as RectTransform;
                publishItemRect.SetParent(PublishItemFrame, false);

                KeyValuePair<int, List<SSchemeContactTaskConfig>> pairItem = mapTaskConfig.ElementAt(i);
                if (pairItem.Value.Count > 0)
                {
                    m_listPublishItem[i].SetData(pairItem.Key, pairItem.Value[0].strSubTypeName);
                }
            }
            PublishItemTemplate.gameObject.SetActive(false);
        }

        // 显示设置
        public void SetData()
        {
            if (IsActive())
                return;

            m_nModelKey = ModalHelper.Open(this, null, new Color(0.15f, 0.15f, 0.15f, 0f), Hide, this.transform as RectTransform);
            this.gameObject.SetActive(true);
        }

        // 模态点击响应
        public void Hide()
        {
            ModalHelper.Close(m_nModelKey);
            this.gameObject.SetActive(false);

            if (m_listPublishItem == null)
                return;
            foreach (var item in m_listPublishItem)
            {
                item.SetDeselect();
            }
        }
    }

    // 任务拥有模块
    enum HoldViewBtnState
    {
        HVBS_NONE,

        HVBS_TASK,      // 拥有任务
        HVBS_CONTACT,   // 已发布任务

        HVBS_MAX,
    }

    public class TaskHoldItem : ISingleTaskItem
    {
        public Text TaskDesc;
        //public URedPointTip RedTip;
        public Image NewReplyIcon;
        public Text ReplyCountDesc;

        private int m_nTaskID;
        private long m_llContactID;
        private int m_nTaskState;
        private TaskHoldFrame m_taskHoldFrame;
        private float m_fFirstWinCoolTime = 0.0f;// 60秒刷新

        public Transform attachPoint;
        private UEffectParamBase finishParam;
        

        // 设置父节点
        public void SetWndView(TaskHoldFrame view)
        {
            m_taskHoldFrame = view;
        }

        // 设置显示
        public void SetData(Task_List_Node node)
        {
            m_nTaskID = node.nTaskID;
            m_llContactID = node.llContactID;
            m_nTaskState = node.nTaskState;
            // 图标
            if (node.nTaskID <= 0)
            {
                NormalIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "chat_normal", WndID.WND_ID_TASK_MAIN);
                SelectIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "chat_select", WndID.WND_ID_TASK_MAIN);
            }
            else
            {
                int nTaskLevel = TaskConfig.Instance.GetTaskLevel(node.nTaskID, node.nTaskType, node.nTaskSubType, node.nTaskSpecificType);
                if (nTaskLevel == (int)Task_Level.Level_3)
                {
                    if (node.nTaskType == (int)ENTASK_TYPE.ENTASK_TYPE_CONTACT)
                    {
                        NormalIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "subType_gold_normal_" + node.nTaskSubType, WndID.WND_ID_TASK_MAIN);
                        SelectIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "subType_gold_select_" + node.nTaskSubType, WndID.WND_ID_TASK_MAIN);
                    }
                    else
                    {
                        NormalIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "type_gold_normal_" + node.nTaskType, WndID.WND_ID_TASK_MAIN);
                        SelectIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "type_gold_select_" + node.nTaskType, WndID.WND_ID_TASK_MAIN);
                    }
                }
                else
                {
                    if (node.nTaskType == (int)ENTASK_TYPE.ENTASK_TYPE_CONTACT)
                    {
                        NormalIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "subType_normal_" + node.nTaskSubType, WndID.WND_ID_TASK_MAIN);
                        SelectIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "subType_select_" + node.nTaskSubType, WndID.WND_ID_TASK_MAIN);
                    }
                    else
                    {
                        NormalIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "type_normal_" + node.nTaskType, WndID.WND_ID_TASK_MAIN);
                        SelectIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Task, "type_select_" + node.nTaskType, WndID.WND_ID_TASK_MAIN);
                    }
                }
            }
            SelectIcon.gameObject.SetActive(false);
            // 描述
            TaskDesc.color = UDefines.CommomColor(ECommonColor.ECC_Blue1);
            if (node.nTaskID <= 0)
            {
                DateTime publishTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(node.nPublishTime);
                publishTime = publishTime.ToLocalTime();
                int nGapTime = node.nServerTime - node.nPublishTime;
                if (nGapTime < 60)//1分钟
                {
                    TaskDesc.text = String.Format("{0}秒前", nGapTime);
                }
                else if (nGapTime < 3600)//1小时
                {
                    TaskDesc.text = String.Format("{0}分钟前", nGapTime / 60);
                }
                else if (nGapTime < 24 * 3600)//1天
                {
                    TaskDesc.text = String.Format("{0}小时前", nGapTime / 3600);
                }
                else if (nGapTime < 2 * 24 * 3600)//2天
                {
                    TaskDesc.text = String.Format("昨天");
                }
                else if (nGapTime < 3 * 24 * 3600)//3天
                {
                    TaskDesc.text = String.Format("前天");
                }
                else
                {
                    TaskDesc.text = String.Format("{0}-{1}-{2}", publishTime.Year, publishTime.Month, publishTime.Day);
                }
            }
            else if (node.nTaskState == (int)Task_State_Type.TaskStateType_End)// 设置首胜,特殊处理
            {
                int nRemianTime = LogicDataCenter.taskDataManager.FirstWinRemainTime;
                int nHour = nRemianTime / 3600;
                int nMin = nRemianTime % 3600 / 60;
                TaskDesc.text = String.Format("{0}{1:D2}:{2:D2}", ULocalizationService.Instance.Get("UIView", "Task", "FirstWinRemainDesc"), nHour, nMin);
                m_fFirstWinCoolTime = Time.unscaledTime;
            }
            else if (node.nTaskState == (int)Task_State_Type.TaskStateType_Finish)
            {
                TaskDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "Finish");
                TaskDesc.color = UDefines.CommomColor(ECommonColor.ECC_Green2);
            }
            else
            {
                TaskDesc.text = String.IsNullOrEmpty(node.szHit) ? "(0/1)" : node.szHit;
            }
            // 回复数
            ReplyCountDesc.text = node.listReply.Count.ToString();
            ReplyCountDesc.gameObject.SetActive(node.nTaskID <= 0);
            // 红点
            OnUpdateRedTip();
            // 光效
            if (node.nTaskID > 0 && node.nTaskState == (int)Task_State_Type.TaskStateType_Finish)
            {
                if (finishParam == null)
                {
                    finishParam = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_TaskMain_FinishState, _tfAttachParent: attachPoint, _bAutoDestroy: false);
                    UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref finishParam);
                }
            }
            else
            {
                if (finishParam != null)
                {
                    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref finishParam);
                    finishParam = null;
                }
            }

            this.gameObject.SetActive(true);
        }

        // 设置首胜定时器刷新,特殊处理
        public void Update()
        {
            if (Time.unscaledTime - m_fFirstWinCoolTime > 60.0f)
            {
                if (m_nTaskState == (int)Task_State_Type.TaskStateType_End)
                {
                    int nRemianTime = LogicDataCenter.taskDataManager.FirstWinRemainTime;
                    int nHour = nRemianTime / 3600;
                    int nMin = nRemianTime % 3600 / 60;
                    TaskDesc.text = String.Format("{0}{1:D2}:{2:D2}", ULocalizationService.Instance.Get("UIView", "Task", "FirstWinRemainDesc"), nHour, nMin);
                }

                m_fFirstWinCoolTime = Time.unscaledTime;
            }
        }

        // 清空显示
        public void Clear()
        {
            // 关闭光效
            if (finishParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref finishParam);
                finishParam = null;
            }

            this.gameObject.SetActive(false);
        }

        // 更新红点提示
        public void OnUpdateRedTip()
        {
            int nCount = 0;

            do
            {
                if (m_llContactID <= 0)
                    break;

                List<Task_List_Node> taskList = LogicDataCenter.taskDataManager.ListTaskData;
                int nIndex = taskList.FindIndex(item => item.llContactID == m_llContactID);
                if (nIndex < 0)
                    break;

                int nLastViewTime = TaskViewTimeConfig.Instance.GetTaskViewTime(taskList[nIndex].llContactID);
                //if (nLastViewTime <= 0)
                //    return;

                for (int i = 0; i < taskList[nIndex].listReply.Count; i++)
                {
                    if (taskList[nIndex].listReply[i].nReplyTime > nLastViewTime)
                    {
                        nCount++;
                    }
                }

            } while (false);

            //RedTip.SetTipCount(nCount);
            NewReplyIcon.gameObject.SetActive(nCount > 0);
        }

        // 点击事件
        public override void OnPointerClick(PointerEventData eventData)
        {
            //// 显示展开界面
            if (m_llContactID > 0)
            {
                cmd_task_view_base_v2c msg = new cmd_task_view_base_v2c();
                msg.llContactID = m_llContactID;
                ViewEventHelper.Instance.SendCommand<cmd_task_view_base_v2c>(GameLogicDef.GVIEWCMD_VIEW_TASK_BASE, ref msg);
            }
            else
            {
                UTaskUnfoldData data = new UTaskUnfoldData();
                data.msgID = (int)WndMsgID.WND_MSG_TASK_UNFOLD_TASK_SHOW;
                data.nTaskID = m_nTaskID;
                data.llContactID = m_llContactID;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_TASK_SHOW, data);
            }
        }
    }

    public class TaskHoldFrame : UCustomUIBehaviour
    {
        public Text SubTitle;

        public RectTransform TaskItemFrame;
        public GameObject TaskItemTemplate;
        private const int SINGLE_PAGE_TASK_COUNT = 21;
        
        public Text NoDataDesc;

        public Text HoldBtnDesc;

        private TaskHoldItem[] m_listTaskItem;
        private HoldViewBtnState m_nBtnState;
        private bool m_bInitShow;

        // 初始化
        protected override void Awake()
        {
            base.Awake();

            SubTitle.text = ULocalizationService.Instance.Get("UIView", "Task", "TaskList");
            NoDataDesc.text = ULocalizationService.Instance.Get("UIView", "Task", "NoTaskData");
            
            // 初始化任务列表模块
            m_listTaskItem = new TaskHoldItem[SINGLE_PAGE_TASK_COUNT];
            for (int i = 0; i < SINGLE_PAGE_TASK_COUNT; i++)
            {
                GameObject taskItemObj = ResNode.InstantiateRes(TaskItemTemplate);
                if (!taskItemObj)
                    return;

                m_listTaskItem[i] = taskItemObj.GetComponent<TaskHoldItem>();
                if (!m_listTaskItem[i])
                    return;

                taskItemObj.name = String.Format("TaskHold{0}", i + 1);
                m_listTaskItem[i].SetWndView(this);
                RectTransform taskItemRect = taskItemObj.transform as RectTransform;
                taskItemRect.SetParent(TaskItemFrame, false);
            }
            TaskItemTemplate.gameObject.SetActive(false);

            //m_nCurTaskPage = 1;
            m_nBtnState = HoldViewBtnState.HVBS_TASK;
            HoldBtnDesc.text = ULocalizationService.Instance.Get("UIView", "Task", "MyContact");
            m_bInitShow = true;
        }

        // 设置显示
        public void SetData()
        {
            // 首次打开排序 后续改变不排
            if (m_bInitShow)
            {
                LogicDataCenter.taskDataManager.SetListSequence();
                m_bInitShow = false;
            }

            if (m_nBtnState == HoldViewBtnState.HVBS_TASK)
            {
                SetTaskData();
            }
            else if (m_nBtnState == HoldViewBtnState.HVBS_CONTACT)
            {
                SetPublishContactData();
            }
        }

        // 设置自身拥有任务显示
        public void SetTaskData()
        {
            List<Task_List_Node> nodeList = LogicDataCenter.taskDataManager.ListTaskData;
            if (nodeList == null || nodeList.Count <= 0)
            {
                ClearView();
                return;
            }

            NoDataDesc.gameObject.SetActive(false);

            // 这需求不想说什么,任务全显示，发布只显示有新回复,的前6条
            int nIndex = 0;
            int nPublishViewCount = 0;
            for (int i = 0; i < nodeList.Count; i++)
            {
                // 排位任务不显示在任务界面
                if (nodeList[i].nTaskType == (int)ENTASK_TYPE.ENTASK_TYPE_RANK)
                    continue;

                int nLastViewTime = TaskViewTimeConfig.Instance.GetTaskViewTime(nodeList[i].llContactID);
                if (nodeList[i].nTaskID > 0)// 显示任务
                {
                    m_listTaskItem[nIndex++].SetData(nodeList[i]);
                }
                else if (nodeList[i].nTaskID <= 0 && nPublishViewCount < 6 && nodeList[i].listReply.Count > 0 && nodeList[i].listReply[0].nReplyTime > nLastViewTime)// 显示有未读回复的发布
                {
                    m_listTaskItem[nIndex++].SetData(nodeList[i]);
                    nPublishViewCount++;
                }
            }

            // 显示控制
            if (nIndex == 0)
            {
                NoDataDesc.text = ULocalizationService.Instance.Get("UIView", "Task", "NoTaskData");
                NoDataDesc.gameObject.SetActive(true);
            }

            if (nIndex < SINGLE_PAGE_TASK_COUNT)
            {
                for (int j = nIndex; j < SINGLE_PAGE_TASK_COUNT; j++)
                {
                    m_listTaskItem[j].Clear();
                }
            }
        }

        // 设置自身发布瓶子显示
        public void SetPublishContactData()
        {
            List<Task_List_Node> nodeList = LogicDataCenter.taskDataManager.ListTaskData;
            if (nodeList == null || nodeList.Count <= 0)
            {
                ClearView();
                return;
            }

            NoDataDesc.gameObject.SetActive(false);

            int nIndex = 0;
            for (int i = 0; i < nodeList.Count; i++)
            {
                if (nodeList[i].nTaskID <= 0 && nodeList[i].listReply.Count > 0)
                {
                    m_listTaskItem[nIndex++].SetData(nodeList[i]);
                }
            }

            // 显示控制
            if (nIndex == 0)
            {
                NoDataDesc.text = ULocalizationService.Instance.Get("UIView", "Task", "NoPublishData");
                NoDataDesc.gameObject.SetActive(true);
            }

            if (nIndex < SINGLE_PAGE_TASK_COUNT)
            {
                for (int j = nIndex; j < SINGLE_PAGE_TASK_COUNT; j++)
                {
                    m_listTaskItem[j].Clear();
                }
            }
        }

        // 清除显示
        public void ClearView()
        {
            for (int i = 0; i < SINGLE_PAGE_TASK_COUNT; i++)
            {
                m_listTaskItem[i].Clear();
            }

            if (m_nBtnState == HoldViewBtnState.HVBS_TASK)
            {
                NoDataDesc.text = ULocalizationService.Instance.Get("UIView", "Task", "NoTaskData");
            }
            else if (m_nBtnState == HoldViewBtnState.HVBS_CONTACT)
            {
                NoDataDesc.text = ULocalizationService.Instance.Get("UIView", "Task", "NoPublishData");
            }
            NoDataDesc.gameObject.SetActive(true);
        }

        // 清空数据
        public void ClearData()
        {
            for (int i = 0; i < SINGLE_PAGE_TASK_COUNT; i++)
            {
                m_listTaskItem[i].Clear();
            }
            
            NoDataDesc.gameObject.SetActive(true);

            HoldBtnDesc.text = ULocalizationService.Instance.Get("UIView", "Task", "MyContact");
            m_nBtnState = HoldViewBtnState.HVBS_TASK;
            m_bInitShow = true;
        }

        // 更新红点显示
        public void OnUpdateRedTip()
        {
            for (int i = 0; i < SINGLE_PAGE_TASK_COUNT; i++)
            {
                m_listTaskItem[i].OnUpdateRedTip();
            }
        }

        // 查看自身拥有瓶子
        public void OnClickMyContactBtn()
        {
            if (m_nBtnState == HoldViewBtnState.HVBS_TASK)
            {
                m_nBtnState = HoldViewBtnState.HVBS_CONTACT;
                HoldBtnDesc.text = ULocalizationService.Instance.Get("UIView", "Task", "MyTask");

                SetPublishContactData();
            }
            else if (m_nBtnState == HoldViewBtnState.HVBS_CONTACT)
            {
                m_nBtnState = HoldViewBtnState.HVBS_TASK;
                HoldBtnDesc.text = ULocalizationService.Instance.Get("UIView", "Task", "MyContact");

                SetTaskData();
            }
        }
    }

    // 任务主界面View
    public class TaskMainWndView : UIBaseWndView
    {
        public TaskSearchFrame SearchFrame;
        public TaskPublishFrame PublishFrame;
        public TaskHoldFrame HoldFrame;
        public Text ReturnBtnDesc;

        // 光效
        public GameObject bgAttachPoint;
        private UEffectParamBase bgParam;

        // 初始化
        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            SearchFrame.SetWndView(this);
            ReturnBtnDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "Return");

            return true;
        }

        // 显示任务主界面
        public void OnShowTaskMainWnd()
        {
            SearchFrame.SetData();
            HoldFrame.SetData();

            // 背景光效
            if (bgParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref bgParam);
            }
            bgParam = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_TaskMain_BgImage, _tfAttachParent: bgAttachPoint.transform, _bAutoDestroy: false);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref bgParam);
        }

        // 更新任务主界面
        public void OnUpdateTaskHoldFrame()
        {
            HoldFrame.SetData();
        }

        // 更新红点显示
        public void OnUpdateRedTip()
        {
            HoldFrame.OnUpdateRedTip();
        }

        // 更新搜索界面
        public void OnUpdateSearchFrame()
        {
            SearchFrame.SetData();
        }

        // 关闭按钮
        public void OnClickCloseBtn()
        {
            SearchFrame.Clear();
            HoldFrame.ClearData();
            SetVisible(false);

            // 关闭其他子界面
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_CLOSE_OTHER, null);

            // 关闭光效
            if (bgParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref bgParam);
            }
        }
    }
}
