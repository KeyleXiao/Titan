using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using DataCenter;
using ASpeedGame.Data.TaskConfig;
using GameLogic;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;
using UIWidgets;
using ASpeedGame.Data.TaskViewTimeConfig;
using ASpeedGame.Data.ShieldWorld;
using USpeedUI.UWidgets;
using Data.ActorPrizeConfig;
using U3D_Render.Common;
using USpeedUI.PlayerFrame;
using USpeedUI.UEffect;
using DG.Tweening;
using USpeedUI.LegendCup;
using War;
using Data.UISoundConfig;
using ASpeedGame.Data.RunePage;

namespace USpeedUI.TaskWnd
{
    // 任务展开界面Wnd
    public class TaskUnfoldWnd : UIBaseWnd<TaskUnfoldWndView>
    {
        // 获取WndID
        public override WndID GetID()
        {
            return WndID.WND_ID_TASK_UNFOLD;
        }

        // 获取预制体路径
        public override string GetPrefabPath()
        {
            return "Task/TaskUnfoldView";
        }

        // 获取UI所在层
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();
            m_isKeyClose = true;

            // 订阅消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_TASK_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_PUBLISH_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_UPDATE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_DELETE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_CLOSE_OTHER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_FINISH, this);

            UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.EnterLeft, KeyStatus.Up, this);
            UISystem.Instance.RegisterKeyMessage(FuntionShortCutSlotIndex.EnterRight, KeyStatus.Up, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            // 退订消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_TASK_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_PUBLISH_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_UPDATE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_DELETE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_CLOSE_OTHER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_UNFOLD_FINISH, this);

            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.EnterLeft, KeyStatus.Up, this);
            UISystem.Instance.UnregisterKeyMessage(FuntionShortCutSlotIndex.EnterRight, KeyStatus.Up, this);
        }

        // 接收消息
        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_TASK_UNFOLD_TASK_SHOW:
                    {
                        UTaskUnfoldData cmdData = (UTaskUnfoldData)data;

                        SetVisible(true);
                        if (m_wndView != null)
                        {
                            m_wndView.OnShowUnfoldTaskWnd(cmdData.nTaskID);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_UNFOLD_PUBLISH_SHOW:
                    {
                        UTaskUnfoldData cmdData = (UTaskUnfoldData)data;

                        SetVisible(true);
                        if (m_wndView != null)
                        {
                            m_wndView.OnShowUnfoldPublishWnd(cmdData.llContactID);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_UNFOLD_UPDATE:
                    {
                        if (m_wndView != null && IsVisible() && m_wndView.LogicFrame.gameObject.activeSelf)
                        {
                            m_wndView.UpdateUnfoldWndView();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_UNFOLD_DELETE:
                    {
                        if (m_wndView != null && IsVisible())
                        {
                            m_wndView.OnMsgDeleteTask();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_MAIN_CLOSE_OTHER:
                    {
                        if (m_wndView != null && IsVisible())
                        {
                            m_wndView.OnClickCloseBtn();
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_UNFOLD_FINISH:
                    {
                        if (m_wndView != null && IsVisible())
                        {
                            m_wndView.OnMsgFinishTask();
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public override bool OnKeyMessage(FuntionShortCutSlotIndex index, KeyStatus status)
        {
            base.OnKeyMessage(index, status);

            if (m_wndView == null)
                return false;

            if ((status == KeyStatus.Up) && (index == FuntionShortCutSlotIndex.EnterLeft || index == FuntionShortCutSlotIndex.EnterRight))
                m_wndView.OnClickEnterKey();

            return true;
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
    }

    // 任务奖励Item
    public class TaskUnfoldPrizeItem : MonoBehaviour
    {
        public Image prizeImage;
        public Text prizeName;
        public Text prizeNum;
        public UTooltipTrigger prizeTooltipTrigger;

        public Image ClanIcon;

        // 显示
        public void SetData(int nPrizeID, int nPrizeCount,bool bFirstWinType = false, bool bShowClan = false, bool bShowNoVave = false)
        {
            SSchemeActorPrizeConfig config = new SSchemeActorPrizeConfig();
            config = ActorPrizeConfig.Instance.GetActorPrizeConfig(nPrizeID);
            if (config != null)
            {
                if (config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_GOLD && bFirstWinType)
                {
                    nPrizeCount = config.nPrizeParam[0];
                }

                if (nPrizeCount > 1)
                {
                    prizeName.text = String.Format("{0}+{1}", config.strPrizeName, nPrizeCount);
                }
                else
                {
                    prizeName.text = config.strPrizeName;
                }
                //prizeNum.text = nPrizeCount.ToString();
                prizeNum.gameObject.SetActive(false);
                if (config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HERO)
                {
                    prizeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_TASK_UNFOLD, 1, 3, config.nPrizeParam[0]);
                }
                else if (config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROCARD)
                {
                    prizeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_TASK_UNFOLD, 1, 7, config.nPrizeParam[0]);
                }
                else if (config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_RUNE)
                {
                    SSchemeRuneConfig runeConfig = RunePage.Instance.allRunePageTable[config.nPrizeParam[0]] as SSchemeRuneConfig;
                    if (runeConfig != null)
                        prizeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_Rune, WndID.WND_ID_TASK_UNFOLD, runeConfig.nRuneInlayType, runeConfig.nRuneIconId);
                }
                else if (config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROXPSKILL)
                {
                    prizeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_TASK_UNFOLD, config.nPrizeType, config.nPrizeIcon);
                }
                else if (config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROSKIN)
                {
                    SkinInfoScheme skinConfig = new SkinInfoScheme();
                    if (GameLogicAPI.GetHeroSkinInfo(config.nPrizeParam[1], ref skinConfig))
                    {
                        prizeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_TASK_UNFOLD, skinConfig.nSkinIconID, 3, config.nPrizeParam[0]);
                    }
                }
                else
                {
                    prizeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_TASK_UNFOLD, config.nPrizeType, config.nPrizeIcon);
                }
                //prizeImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_TASK_UNFOLD, config.nPrizeType, config.nPrizeIcon);

                String szDesc = config.strPrizeDesc;
                UBB.toHtml(ref szDesc, UBB_FORMAT_TYPE.UGUI);
                prizeTooltipTrigger.SetText(UTooltipParamName.BodyText, szDesc);

                //prizeName.gameObject.SetActive(!bShowClan);
                ClanIcon.gameObject.SetActive(bShowClan);
                if (bShowNoVave)
                {
                    prizeImage.color = UDefines.CommomColor(ECommonColor.ECC_Gray2);
                }
                else
                {
                    prizeImage.color = UDefines.CommomColor(ECommonColor.ECC_White);
                }
            }
            this.gameObject.SetActive(true);
        }

        // 隐藏
        public void Clear()
        {
            this.gameObject.SetActive(false);
        }
    }

    // 任务展开界面View
    public class TaskUnfoldWndView : UIBaseWndView
    {
        public Image PublisherHead;
        public GameObject PublisherGo;
        public Text PublisherName;
        public Text EmotionWords;
        public Image PublisherSex;
        public UPopupMenuHandle PublishMenuHandle;

        public Text TaskName;
        public GameObject TaskGo;
        public Text TaskDesc;
        public Text Description;
        public URichText WarRecordRT;
        public URichText DeleteRT;
        public URichText GotoRT;
        public URichText FinishRT;
        public Text FirstWinReflushTime;
        public RectTransform TaskPrizeFrame;
        public GameObject TaskPrizeTemplate;
        public Image[] TaskStarLevel;
        private TaskUnfoldPrizeItem[] m_arrTaskPrizeItem;
        private const int TASK_PRIZE_ITEM_COUNT = 3;
        private float m_fFirstWinCoolTime = 0.0f;// 60秒刷新

        public GameObject InputFieldGo;
        public InputField ResponseIF;
        public Text IFCount;
        public Text SendBtnDesc;

        public GameObject TaskReplyGo;
        public UListTaskReply TaskReplyList;

        private Task_List_Node m_unfoldTask;
        public Task_List_Node UnfoldTask { get { return m_unfoldTask; } }
        private float m_fReplyCoolTime = 0.0f;//5秒冷却时间

        // 光效
        public Transform LogicFrame;
        public Transform deleteAttachPoint;
        private UEffectParamBase deleteParam;
        public Transform finishAttachPoint;
        private UEffectParamBase finishParam;
        public GameObject finishBg;
        // 奖励
        public GameObject PrizePopupGo;
        public RectTransform FinishPrizeFrame;
        public GameObject FinishPrizeTemplate;
        private LegendCupPrizeItem[] m_arrFinishPrizeItem;

        private float m_fDeleteCoolTime = 0.0f;
        private float m_fFinishCoolTime = 0.0f;

        // 初始化
        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            TaskDesc.text = ULocalizationService.Instance.Get("UIView", "Task", "TaskDesc");
            ResponseIF.placeholder.GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "Task", "InputWords");

            IFCount.text = "0/40";
            SendBtnDesc.text = ULocalizationService.Instance.Get("UIView", "Common", "Send");

            WarRecordRT.text = "<a href=WarRecord color=ffca27>" + ULocalizationService.Instance.Get("UIView", "Task", "WarRecord") + "</a>";
            WarRecordRT.onHrefClick.AddListener(OnClickWarRecordRT);
            DeleteRT.text = "<a href=Delete color=a5dbef>" + ULocalizationService.Instance.Get("UIView", "Common", "Delete") + "</a>";
            DeleteRT.onHrefClick.AddListener(OnClickDeleteRT);
            GotoRT.text = "<a href=Goto color=a5dbef>" + ULocalizationService.Instance.Get("UIView", "Common", "GoTo") + "</a>";
            GotoRT.onHrefClick.AddListener(OnClickGotoRT);
            FinishRT.text = "<a href=Finish color=ffca27>" + ULocalizationService.Instance.Get("UIView", "Task", "FinishTask") + "</a>";
            FinishRT.onHrefClick.AddListener(OnClickFinishRT);

            // 初始化列表奖励
            m_arrTaskPrizeItem = new TaskUnfoldPrizeItem[TASK_PRIZE_ITEM_COUNT];
            for (int i = 0; i < TASK_PRIZE_ITEM_COUNT; i++)
            {
                GameObject prizeItemGo = ResNode.InstantiateRes(TaskPrizeTemplate);
                if (!prizeItemGo)
                    return false;

                m_arrTaskPrizeItem[i] = prizeItemGo.GetComponent<TaskUnfoldPrizeItem>();
                if (!m_arrTaskPrizeItem[i])
                    return false;

                prizeItemGo.name = String.Format("TaskPrizeItem{0}", i + 1);
                RectTransform taskItemRect = prizeItemGo.transform as RectTransform;
                taskItemRect.SetParent(TaskPrizeFrame, false);
            }
            TaskPrizeTemplate.gameObject.SetActive(false);

            // 初始化弹出奖励
            m_arrFinishPrizeItem = new LegendCupPrizeItem[TASK_PRIZE_ITEM_COUNT];
            for (int i = 0; i < TASK_PRIZE_ITEM_COUNT; i++)
            {
                GameObject prizeItemGo = ResNode.InstantiateRes(FinishPrizeTemplate);
                if (!prizeItemGo)
                    return false;

                m_arrFinishPrizeItem[i] = prizeItemGo.GetComponent<LegendCupPrizeItem>();
                if (!m_arrFinishPrizeItem[i])
                    return false;

                prizeItemGo.name = String.Format("FinishPrizeItem{0}", i + 1);
                RectTransform taskItemRect = prizeItemGo.transform as RectTransform;
                taskItemRect.SetParent(FinishPrizeFrame, false);
            }
            FinishPrizeTemplate.gameObject.SetActive(false);

            return true;
        }

        // 显示任务展开界面
        public void OnShowUnfoldTaskWnd(int nTaskID)
        {
            List<Task_List_Node> listTask = LogicDataCenter.taskDataManager.ListTaskData;
            if (listTask == null)
                return;
            int nIndex = listTask.FindIndex(item => item.nTaskID == nTaskID);
            if (nIndex < 0)
                return;
            m_unfoldTask = listTask[nIndex];

            // 更新显示
            UpdateUnfoldWndView();
        }

        // 显示发布展开界面
        public void OnShowUnfoldPublishWnd(long llContactID)
        {
            List<Task_List_Node> listTask = LogicDataCenter.taskDataManager.ListTaskData;
            if (listTask == null)
                return;
            int nIndex = listTask.FindIndex(item => item.llContactID == llContactID);
            if (nIndex < 0)
                return;
            m_unfoldTask = listTask[nIndex];

            // 更新显示
            UpdateUnfoldWndView();
        }

        // 更新展开界面
        public void UpdateUnfoldWndView()
        {
            ClearView();

            // 解析数据(首胜奖励)
            bool bFirstWinType = m_unfoldTask.nTaskType == (int)ENTASK_TYPE.ENTASK_TYPE_FIRSTWIN;
            int nFirstWinState = 0;
            if (bFirstWinType && !String.IsNullOrEmpty(m_unfoldTask.szHit) && m_unfoldTask.szHit.Length > 1)
            {
                string strFirstWinState = m_unfoldTask.szHit.Substring(1, 1);
                nFirstWinState = Int32.Parse(strFirstWinState);
            }
            if (bFirstWinType)
            {
                m_unfoldTask.szDescription = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinNormalDesc");
                if (nFirstWinState == 1)
                {
                    m_unfoldTask.szDescription = ULocalizationService.Instance.Get("UIView", "Task", "FirstWinHalfDesc");
                }
            }

            // 设置显示
            if (m_unfoldTask.llContactID <= 0)
            {
                InputFieldGo.SetActive(false);
                TaskReplyGo.SetActive(false);
                this.gameObject.GetComponent<RectTransform>().sizeDelta = new Vector2(774.0f, 240.0f);
            }
            else if (m_unfoldTask.listReply.Count <= 0)
            {
                InputFieldGo.SetActive(true);
                TaskReplyGo.SetActive(false);
                this.gameObject.GetComponent<RectTransform>().sizeDelta = new Vector2(774.0f, 370.0f);
            }
            else
            {
                InputFieldGo.SetActive(true);
                TaskReplyGo.SetActive(true);
                this.gameObject.GetComponent<RectTransform>().sizeDelta = new Vector2(774.0f, 586.0f);
            }
            WarRecordRT.gameObject.SetActive(m_unfoldTask.llTagID > 0);
            FinishRT.gameObject.SetActive(m_unfoldTask.nTaskState == (int)Task_State_Type.TaskStateType_Finish);
            GotoRT.gameObject.SetActive(m_unfoldTask.nClientLink > 0 && m_unfoldTask.nTaskState == (int)Task_State_Type.TaskStateType_Accept);

            // 设置任务
            if (String.IsNullOrEmpty(m_unfoldTask.szName))
                TaskName.text = String.Format(ULocalizationService.Instance.Get("UIView", "Task", "UnfoldTitle"), m_unfoldTask.strPublishName);
            else
                TaskName.text = m_unfoldTask.szName;

            TaskGo.SetActive(m_unfoldTask.nTaskID > 0);
            string strPoccess = "";
            if (!String.IsNullOrEmpty(m_unfoldTask.szHit))
                strPoccess = m_unfoldTask.szHit;
            else if (m_unfoldTask.nTaskState == (int)Task_State_Type.TaskStateType_Finish)
                strPoccess = "(1/1)";
            else
                strPoccess = "(0/1)";
            string strDesc = String.Format("{0}，{1}{2}", m_unfoldTask.szDescription, ULocalizationService.Instance.Get("UIView", "Task", "NowPoccess"), strPoccess);
            UBB.toHtml(ref strDesc);
            Description.text = strDesc;

            if (TaskStarLevel.Length == (int)Task_Level.Level_3)
            {
                int nTaskLevel = TaskConfig.Instance.GetTaskLevel(m_unfoldTask.nTaskID, m_unfoldTask.nTaskType, m_unfoldTask.nTaskSubType, m_unfoldTask.nTaskSpecificType); ;
                TaskStarLevel[0].gameObject.SetActive(nTaskLevel >= (int)Task_Level.Level_2);
                TaskStarLevel[1].gameObject.SetActive(nTaskLevel >= (int)Task_Level.Level_2);
                TaskStarLevel[2].gameObject.SetActive(nTaskLevel >= (int)Task_Level.Level_3);
            }

            // 设置首胜,特殊处理
            if (m_unfoldTask.nTaskState == (int)Task_State_Type.TaskStateType_End)
            {
		        Description.text = m_unfoldTask.szDescription;

                int nRemianTime = LogicDataCenter.taskDataManager.FirstWinRemainTime;
                int nHour = nRemianTime / 3600;
                int nMin = nRemianTime % 3600 / 60;
                FirstWinReflushTime.text = String.Format("{0}{1:D2}:{2:D2}", ULocalizationService.Instance.Get("UIView", "Task", "FirstWinRemainDesc"), nHour, nMin);
                FirstWinReflushTime.gameObject.SetActive(true);
                m_fFirstWinCoolTime = Time.unscaledTime;
            }
            else
            {
                FirstWinReflushTime.gameObject.SetActive(false);
            }

            // 设置发布者
            PublisherGo.SetActive(m_unfoldTask.llContactID > 0);
            PublisherHead.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_TASK_UNFOLD, 1, 1);
            PublisherName.text = m_unfoldTask.strPublishName;
            EmotionWords.text = m_unfoldTask.strEmotionWords;
            if (m_unfoldTask.nPublishSex < (int)PERSON_SEX.SEX_MAX && m_unfoldTask.nPublishSex >= (int)PERSON_SEX.SEX_MALE)
            {
                PublisherSex.gameObject.SetActive(true);
                PublisherSex.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_KinSex, WndID.WND_ID_TASK_UNFOLD, m_unfoldTask.nPublishSex + 1);
                PublisherHead.gameObject.SetActive(true);
                PublisherHead.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_PlayerHead, WndID.WND_ID_TASK_UNFOLD, 1, m_unfoldTask.nPublishSex + 1);
            }
            else
            {
                PublisherSex.gameObject.SetActive(false);
                PublisherHead.gameObject.SetActive(false);
            }
            int nSelfID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
            int nTargetID = m_unfoldTask.nPublishPDBID;

            // 右键弹窗
            if (nTargetID > 0 && nTargetID != nSelfID)
            {
                UPopupAction PopupAction = new UPopupAction();
                PopupAction.Add(UPopupItemDataAddFriend.GetDescString(), new UPopupItemDataAddFriend(m_unfoldTask.nPublishUserID, m_unfoldTask.strPublishName));
                PopupAction.Add(UPopupItemDataGetPlayerDetail.GetDescString(), new UPopupItemDataGetPlayerDetail(nTargetID));
                PopupAction.Add(UPopupItemDataTeamInvite.GetDescString(), new UPopupItemDataTeamInvite(nTargetID));
                PopupAction.Add(UPopupItemDataTeamRequest.GetDescString(), new UPopupItemDataTeamRequest(nTargetID));
                PopupAction.Add(UPopupItemDataBuddyChat.GetDescString(), new UPopupItemDataBuddyChat(nTargetID));
                PopupAction.Add(UPopupItemDataPrivateChat.GetDescString(), new UPopupItemDataPrivateChat(m_unfoldTask.strPublishName));

                PublishMenuHandle.HandlePopupAction = PopupAction;
            }

            // 设置回复
            int nReplyCount = m_unfoldTask.listReply.Count;
            TaskReplyList.DataSource.Clear();
            ObservableList<UListItemTaskReply> dataSource = new ObservableList<UListItemTaskReply>();
            foreach (var item in m_unfoldTask.listReply)
            {
                UListItemTaskReply node = new UListItemTaskReply();
                node.nIndex = nReplyCount--;
                node.nServerTime = m_unfoldTask.nServerTime;
                node.node = item;
                dataSource.Add(node);
            }
            TaskReplyList.DataSource = dataSource;

            // 设置奖励
            for (int i = 0; i < TASK_PRIZE_ITEM_COUNT; i++)
            {
                if (m_unfoldTask.arrayPrize != null && i < m_unfoldTask.arrayPrize.Length)
                {
                    // 第二个首胜奖励
                    bool bShowClan = m_unfoldTask.nTaskType == (int)ENTASK_TYPE.ENTASK_TYPE_FIRSTWIN && i >= 1;
                    // 第二个首胜奖励，且完成1态
                    bool bShowNoHave = bShowClan && m_unfoldTask.nTaskState == (int)Task_State_Type.TaskStateType_Finish && nFirstWinState == 1;

                    m_arrTaskPrizeItem[i].SetData(m_unfoldTask.arrayPrize[i].nPrizeID, m_unfoldTask.arrayPrize[i].nCount, bFirstWinType, bShowClan, bShowNoHave);
                }
                else
                {
                    m_arrTaskPrizeItem[i].Clear();
                }
            }
            PrizePopupGo.SetActive(false);
        }

        // 设置首胜定时器刷新,特殊处理
        public override void Update()
        {
            base.Update();

            if (Time.unscaledTime - m_fFirstWinCoolTime > 60.0f)
            {
                if (m_unfoldTask.nTaskState == (int)Task_State_Type.TaskStateType_End)
                {
                    int nRemianTime = LogicDataCenter.taskDataManager.FirstWinRemainTime;
                    int nHour = nRemianTime / 3600;
                    int nMin = nRemianTime % 3600 / 60;
                    FirstWinReflushTime.text = String.Format("{0}{1:D2}:{2:D2}", ULocalizationService.Instance.Get("UIView", "Task", "FirstWinRemainDesc"), nHour, nMin);
                }

                m_fFirstWinCoolTime = Time.unscaledTime;
            }
        }

        // 检测是否关闭界面
        public bool CheckCloseUnfoldWnd()
        {
            // 数据不存在表明已领取, 首胜为结束状态表明已领取 
            List<Task_List_Node> listTask = LogicDataCenter.taskDataManager.ListTaskData;
            if (listTask == null)
                return false;
            int nIndex = listTask.FindIndex(item => item.nTaskID == m_unfoldTask.nTaskID);
            if (nIndex < 0)
            {
                return true;
            }
            else if (listTask[nIndex].nTaskState == (int)Task_State_Type.TaskStateType_End)
            {
                return true;
            }
            return false;    
        }

        // 战绩链接
        public void OnClickWarRecordRT(string eventName)
        {
            if (eventName == "WarRecord")
            {
                UIButtonType ButtonType = new UIButtonType((int)PlayerSubWndType.WND_WAR);
                int nActorID = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
                ButtonType.bIsRequestSelf = (nActorID == m_unfoldTask.nPublishPDBID);
                ButtonType.nRequestPDBID = m_unfoldTask.nPublishPDBID;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERBUTTONFRAME_OPENFRAME, ButtonType);

                WarStatisticViewSingleWarData uiData = new WarStatisticViewSingleWarData();
                uiData.nEntryType = (int)ViewSingleWarType.VSWT_CHATWND;
                uiData.lWarID = m_unfoldTask.llTagID;
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_PLAYERWARSTATISTIC_VIEWSINGLEWAR, uiData);
            }
        }

        // 删除链接
        public void OnClickDeleteRT(string eventName)
        {
            if (eventName == "Delete")
            {
                float fNow = Time.unscaledTime;
                if (fNow - m_fDeleteCoolTime < 3.0f)
                    return;
                m_fDeleteCoolTime = fNow;

                gamelogic_delete_task_list deleteTaskList = new gamelogic_delete_task_list();
                deleteTaskList.nTaskID = m_unfoldTask.nTaskID;
                ViewEventHelper.Instance.SendCommand<gamelogic_delete_task_list>(GameLogicDef.GVIEWCMD_DELETE_TASK_LIST, ref deleteTaskList);

                SSchemeUSoundData soundData = new SSchemeUSoundData();
                if (UISoundConfig.Instance.GetData("task", "Button_taskDelete_click", ref soundData))
                {
                    SoundManager.CreateUISound(soundData.USoundID);
                }
            }
        }

        // 引导链接
        public void OnClickGotoRT(string eventName)
        {
            if (eventName == "Goto")
            {
                LogicDataCenter.taskDataManager.OnTaskGuide(m_unfoldTask.nClientLink);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_TASK_MAIN_ONCLICK_GOTO, null);
            }
        }

        // 完成链接
        public void OnClickFinishRT(string eventName)
        {
            if (eventName == "Finish")
            {
                float fNow = Time.unscaledTime;
                if (fNow - m_fFinishCoolTime < 3.0f)
                    return;
                m_fFinishCoolTime = fNow;

                gamelogic_finish_task finishTask = new gamelogic_finish_task();
                finishTask.nTaskID = m_unfoldTask.nTaskID;
                ViewEventHelper.Instance.SendCommand<gamelogic_finish_task>(GameLogicDef.GVIEWCMD_FINISH_TASK, ref finishTask);

                SSchemeUSoundData soundData = new SSchemeUSoundData();
                if (UISoundConfig.Instance.GetData("task", "Button_taskGetreward_click", ref soundData))
                {
                    SoundManager.CreateUISound(soundData.USoundID);
                }
            }
        }

        // 回撤键按下
        public void OnClickEnterKey()
        {
            if (ResponseIF.isFocused)
            {
                OnClickSendBtn();
            }
        }

        // 发送响应
        public void OnClickSendBtn()
        {
            if (String.IsNullOrEmpty(ResponseIF.text))
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LUA_TASK_NO_INPUT);
                return;
            }

            string strResponse = ResponseIF.text;
            if (ShieldWord.Instance.filterShieldWord(ref strResponse))
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LUA_TASK_REPLY_CONTAIN_SHIELD_WORD);
                return;
            }
                
            // 5秒回复冷却时间
            if (Time.unscaledTime - m_fReplyCoolTime < 5.0f)
            {
                UIUtil.ShowSystemMessage(EMChatTipID.CHAT_TIP_LUA_TASK_REPLY_QUICK);
                return;
            }
            m_fReplyCoolTime = Time.unscaledTime;

            cmd_task_add_reply_v2c replyData;
            replyData.llContactID = m_unfoldTask.llContactID;
            replyData.nCurReplyCount = m_unfoldTask.listReply.Count;
            replyData.szReplyContent = ResponseIF.text;
            ViewEventHelper.Instance.SendCommand<cmd_task_add_reply_v2c>(GameLogicDef.GVIEWCMD_REPLY_TASK, ref replyData);

            // 清空输入
            ResponseIF.placeholder.GetComponent<Text>().text = ULocalizationService.Instance.Get("UIView", "Task", "InputWords");
            ResponseIF.text = "";
        }
        
        // 输入响应
        public void OnInputChange()
        {
            IFCount.text = String.Format("{0}/40", ResponseIF.text.Length);
        }

        // 关闭按钮
        public void OnClickCloseBtn()
        {
            LogicDataCenter.taskDataManager.PrizeNode.Clear();

            ClearView();

            SetVisible(false);
        }

        public void ClearView()
        {
            // 删除光效
            if (deleteParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref deleteParam);
                deleteParam = null;
            }
            if (finishParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref finishParam);
                finishParam = null;
            }

            LogicFrame.gameObject.SetActive(true);
            finishBg.SetActive(false);
            PrizePopupGo.SetActive(false);

            for (int i = 0; i < TASK_PRIZE_ITEM_COUNT; i++)
            {
                m_arrFinishPrizeItem[i].Clear();
            }
        }

        // 完成任务消息响应
        public void OnMsgFinishTask()
        {
            if (CheckCloseUnfoldWnd())
            {
                //Sequence logicSequence = DOTween.Sequence();
                //logicSequence.AppendInterval(0.4f).OnComplete(OnFinishSequenceComplete);
                //// 光效
                //if (finishParam != null)
                //{
                //    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref finishParam);
                //    finishParam = null;
                //}
                //finishParam = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_TaskUnfold_Finish, _tfAttachParent: finishAttachPoint, _bAutoDestroy: false);
                //UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref finishParam);

                // 隐藏
                LogicFrame.gameObject.SetActive(false);
                // 显示
                finishBg.GetComponent<RectTransform>().sizeDelta = new Vector2(1920.0f, 1500.0f);
                finishBg.SetActive(true);

                OnFinishSequenceComplete();
            }
        }

        // 动画结束回调
        private void OnFinishSequenceComplete()
        {
            Task_Prize_Node prizeNode = LogicDataCenter.taskDataManager.PrizeNode;
            if (prizeNode.nTaskID > 0)
            {
                // 弹出奖励
                int nIndex = 0;
                if (prizeNode.nPrizeID != null)
                {
                    for (int i = 0; i < prizeNode.nPrizeID.Length && nIndex < TASK_PRIZE_ITEM_COUNT; i++)
                    {
                        if (prizeNode.nPrizeID[i] <= 0)
                            continue;
                        m_arrFinishPrizeItem[nIndex++].SetData(prizeNode.nPrizeID[i], ECupPrizeType.ECPT_Personal, UEffectPrefabType.UEPT_TaskUnfold_Finish);
                    }
                }
                if (prizeNode.nBindingGoldCount > 0 && nIndex < TASK_PRIZE_ITEM_COUNT)
                {
                    m_arrFinishPrizeItem[nIndex++].SetData(prizeNode.nBindingGoldCount, ECupPrizeType.ECPT_Gold, UEffectPrefabType.UEPT_TaskUnfold_Finish);
                }
                if (prizeNode.nExpCount > 0 && nIndex < TASK_PRIZE_ITEM_COUNT)
                {
                    m_arrFinishPrizeItem[nIndex++].SetData(prizeNode.nExpCount, ECupPrizeType.ECPT_Exp, UEffectPrefabType.UEPT_TaskUnfold_Finish);
                }
                if (nIndex < TASK_PRIZE_ITEM_COUNT)
                {
                    for (int i = nIndex; i < TASK_PRIZE_ITEM_COUNT; i++)
                    {
                        m_arrFinishPrizeItem[nIndex++].Clear();
                    }
                }
                PrizePopupGo.SetActive(true);

                prizeNode.Clear();
            }
            else
            {
                finishBg.SetActive(false);
            }
        }

        // 删除任务消息响应
        public void OnMsgDeleteTask()
        {
            if (CheckCloseUnfoldWnd())
            {
                Vector3 target = new Vector3(1.0f, 0.0f, 1.0f);
                Sequence logicSequence = DOTween.Sequence();
                logicSequence.Append(LogicFrame.DOScale(target, 0.2f)).AppendInterval(0.3f).OnComplete(OnDeleteSequenceComplete);

                // 光效
                if (deleteParam != null)
                {
                    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref deleteParam);
                    deleteParam = null;
                }
                deleteParam = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_TaskUnfold_Delete, _tfAttachParent: deleteAttachPoint, _bAutoDestroy: false);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref deleteParam);
            }
        }

        // 动画结束回调
        private void OnDeleteSequenceComplete()
        {
            // 关闭
            OnClickCloseBtn();
            // 还原
            LogicFrame.localScale = new Vector3(1.0f, 1.0f, 1.0f);
        }


    }
}
