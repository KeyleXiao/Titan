using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using USpeedUI.UWidgets;
using UnityEngine.UI;
using UnityEngine;
using UIWidgets;
using DataCenter;
using U3D_Render.Common;
using GameLogic;
using Data.ActorPrizeConfig;
using War;
using Match_ManagerDef;
using ASpeedGame.Data.MatchSeasonPrizeConfig;
using USpeedUI.UEffect;

namespace USpeedUI.WarMainFrame
{
    public class RankDetailWnd : UIFillWnd<RankDetailWndView>
    {
        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_RANK_DETAIL;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "WarWnd/RankDetailView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            //return WndLayerDef.WND_LAYER_FILL;
            return WndLayerDef.WND_LAYER_POPUP02;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RANK_DETIAL_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_RANK_DETAIL_RECEIVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_HOLD, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RANK_DETIAL_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_RANK_DETAIL_RECEIVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_TASK_MAIN_UPDATE_HOLD, this);
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            UIMsgCmdData data = msgData as UIMsgCmdData;
            switch (msgID)
            {
                case WndMsgID.WND_MSG_RANK_DETIAL_SHOW:
                    {
                        if (m_wndView != null && m_wndView.IsVisible())
                            return;

                        SetVisible(true);

                        OnRankDetialWndShow();
                    }
                    break;
                case WndMsgID.WND_MSG_RANK_DETAIL_RECEIVE:
                    {
                        if (m_wndView != null)
                        {
                            UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
                            m_wndView.OnReceiveRankDetail(cmdData.ptrParam, cmdData.nPtrLen);
                        }
                    }
                    break;
                case WndMsgID.WND_MSG_TASK_MAIN_UPDATE_HOLD:
                    {
                        if (m_wndView != null)
                        {
                            UIMsgCmdData cmdData = (UIMsgCmdData)msgData;
                            m_wndView.OnTaskUpdate();
                        }
                    }
                    break;

                default:
                    break;
            }
        }

        // 请求赛季详细数据
        private void OnRankDetialWndShow()
        {
            EntityEventHelper.Instance.SendCommand(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_REQ_SEASON_DETAIL);
        }

    }

    public class IRankDetailFrame : UCustomUIBehaviour
    {
        public virtual void Init(RankDetailWndView view) { }

        public virtual void Clear() { }
    }

    // 段位
    public class RankDetailScoreFrame : IRankDetailFrame
    {
        public GameObject RankInvalidFrame;

        public GameObject RankFrame;
        public Image RankImg;
        public Text RankGradeDes;
        public Transform RankStarFrame;

        public void SetData()
        {
            if (EntityFactory.MainHeroView == null)
                return;
            int nMatchType = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_MATCHTYPE);
            int nRankScore = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKSCORE);
            int nRankGrade = EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_RANKGRADE);
            
            if (nRankScore <= 0)
            {
                RankInvalidFrame.SetActive(true);
                RankFrame.SetActive(false);
                return;
            }

            int nRankIcon = GameLogicAPI.getRankIconIDByGrade(nMatchType, nRankGrade);
            int nRankStar = GameLogicAPI.getActorRankStar(nMatchType, nRankScore);
            int nMaxRankStar = GameLogicAPI.getActorCurRankMaxStar(nMatchType, nRankScore);
            IntPtr ptr = GameLogicAPI.getActorRankNameByGrade(nMatchType, nRankGrade);
            string strRankName = "";
            if (ptr != IntPtr.Zero)
                strRankName = IntPtrHelper.Ptr2Str(ptr);

            RankInvalidFrame.SetActive(false);
            RankFrame.SetActive(true);
            RankImg.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankImage, WndID.WND_ID_WAR_MAIN_FRAME, 1, nRankIcon);
            RankGradeDes.text = strRankName;
            for (int index = 0; index < RankStarFrame.childCount; ++index)
            {
                RankStarFrame.GetChild(index).gameObject.SetActive(true);
                if (nRankStar > 0)
                {
                    RankStarFrame.GetChild(index).GetComponent<Image>().sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankStarImage, WndID.WND_ID_WAR_MAIN_FRAME, 2);
                    nRankStar--;
                    nMaxRankStar--;
                }
                else if (nMaxRankStar > 0)
                {
                    RankStarFrame.GetChild(index).GetComponent<Image>().sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_RankStarImage, WndID.WND_ID_WAR_MAIN_FRAME, 1);
                    nMaxRankStar--;
                }
                else
                {
                    RankStarFrame.GetChild(index).gameObject.SetActive(false);
                }
            }
        }
    }

    // 任务
    public class RankDetailTaskFrame : IRankDetailFrame
    {
        public Text TaskDesc;
        public Text GetBtnDesc;
        public Button GetBtn;
        public UTooltipTrigger Trigger;

        private int m_nTaskID;

        public void SetData()
        {
            List<Task_List_Node> listTask = LogicDataCenter.taskDataManager.ListTaskData;
            if (listTask == null)
                return;
            int nIndex = listTask.FindIndex(item => item.nTaskType == (int)ENTASK_TYPE.ENTASK_TYPE_RANK);
            if (nIndex < 0)
            {
                TaskDesc.text = "周任务全部完成";
                GetBtn.interactable = false;
                return;
            }

            Task_List_Node task = listTask[nIndex];
            m_nTaskID = task.nTaskID;

            string strTaskDesc = String.Format("参加<color=#ffca27>{0}</color>场领取", task.szHit);
            UBB.toHtml(ref strTaskDesc);
            TaskDesc.text = strTaskDesc;

            GetBtnDesc.text = "领取";

            GetBtn.interactable = (task.nTaskState == (int)Task_State_Type.TaskStateType_Finish);

            string strTip = ULocalizationService.Instance.Get("UIView", "RankSeason", "TaskTip");
            UBB.toHtml(ref strTip);
            Trigger.SetText(UTooltipParamName.BodyText, strTip);
        }

        public void OnClickGetBtn()
        {
            if (m_nTaskID <= 0)
            {
                return;
            }
            
            gamelogic_finish_task finishTask = new gamelogic_finish_task();
            finishTask.nTaskID = m_nTaskID;
            ViewEventHelper.Instance.SendCommand<gamelogic_finish_task>(GameLogicDef.GVIEWCMD_FINISH_TASK, ref finishTask);
        }
    }

    // 荣誉
    public class RankDetailHonorFrame : IRankDetailFrame
    {
        public Text TitleDesc;
        public Text SubTitleDesc;

        public Text WinProDesc;
        public Text WinCountDesc;
        public Text ContinueWinCountDesc;
        public Text MvpCountDesc;
        public Text SuperGodCountDesc;
        public Text Kill5CountDesc;
        public Text Kill4CountDesc;
        public Text Kill3CountDesc;

        public Text WinPro;
        public Text WinCount;
        public Text ContinueWinCount;
        public Text MvpCount;
        public Text SuperGodCount;
        public Text Kill5Count;
        public Text Kill4Count;
        public Text Kill3Count;

        public void SetData(cmd_entity_rank_season_detail data)
        {
            TitleDesc.text = "赛季成绩";

            SSchemeSeasonPrizeConfig config = MatchSeasonPrizeConfig.Instance.GetSeasonPrizeData(data.wPrizeConfigID, data.wTopRankGrade);
            if (config != null)
            {
                DateTime endTime = new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Unspecified).AddSeconds(data.dwEndTime);
                endTime = endTime.ToLocalTime();
                string strSubTitle = String.Format(" / {0}截止时间：<color=#ffca27>{1}月{2}日</color>", config.szSeasonDesc, endTime.Month, endTime.Day);
                UBB.toHtml(ref strSubTitle);
                SubTitleDesc.text = strSubTitle;
                SubTitleDesc.gameObject.SetActive(true);
            }
            else
            {
                SubTitleDesc.gameObject.SetActive(false);
            }

            WinProDesc.text = " / 胜率";
            WinCountDesc.text = " / 胜场";
            ContinueWinCountDesc.text = " / 最高连胜";
            MvpCountDesc.text = " / 全场最佳";
            SuperGodCountDesc.text = " / 超神";
            Kill5CountDesc.text = " / 五连破";
            Kill4CountDesc.text = " / 四连破";
            Kill3CountDesc.text = " / 三连破";

            float fWinPro = data.wMatchCount > 0 ? (float)data.wWinCount / data.wMatchCount : 0;
            WinPro.text = String.Format("{0:F0}%", fWinPro * 100);
            WinCount.text = data.wWinCount.ToString();
            ContinueWinCount.text = data.wContinueWinCount.ToString();
            MvpCount.text = data.wMvpCount.ToString();
            SuperGodCount.text = data.wSuperGodCount.ToString();
            Kill5Count.text = data.wKill5Count.ToString();
            Kill4Count.text = data.wKill4Count.ToString();
            Kill3Count.text = data.wKill3Count.ToString();
        }
    }

    // 奖励
    public class RankDetialPrizeItem : UCustomUIBehaviour
    {
        public Image PrizeIcon;
        public Text PrizeCount;
        public Text FinishCondition;
        public UTooltipTrigger Trigger;

        public Text FinishState;

        public void SetData(int nPrizeID, int nConditionGrade, int nConditionWinCount, int nWinCount, bool bHaveState, string strCondition)
        {
            SSchemeActorPrizeConfig config = ActorPrizeConfig.Instance.GetActorPrizeConfig(nPrizeID);
            if (config == null)
            {
                this.gameObject.SetActive(false);
                return;
            }

            this.gameObject.SetActive(true);

            // 头像
            if (config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HERO)
            {
                PrizeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_POINTSHOP, 1, 3, config.nPrizeParam[0]);
            }
            else if (config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROCARD)
            {
                PrizeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_POINTSHOP, 1, 7, config.nPrizeParam[0]);
            }
            else if (config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROSKIN)
            {
                SkinInfoScheme skinConfig = new SkinInfoScheme();
                if (GameLogicAPI.GetHeroSkinInfo(config.nPrizeParam[1], ref skinConfig))
                    PrizeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_POINTSHOP, skinConfig.nSkinIconID, 3, config.nPrizeParam[0]);
            }
            else if (config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_HEROXPSKILL)
            {
                PrizeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_POINTSHOP, config.nPrizeType, config.nPrizeIcon);
            }
            else
            {
                PrizeIcon.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_ActorPrize, WndID.WND_ID_POINTSHOP, config.nPrizeType, config.nPrizeIcon);
            }

            // 数量
            int nCount = 1;
            if (config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_GOLD || config.nPrizeType == (int)EMActorPrizeType.ACTOR_PRIZE_ACTOREXP)
                nCount= config.nPrizeParam[0];
            PrizeCount.text = nCount.ToString();

            // Tip
            string strTip = config.strPrizeName;
            UBB.toHtml(ref strTip);
            Trigger.SetText(UTooltipParamName.BodyText, strTip);

            // FinishCondition
            UBB.toHtml(ref strCondition);
            FinishCondition.text = strCondition;

            // FinishState
            string str1 = nWinCount >= nConditionWinCount ? "完成" : "未完成";
            string str2 = String.Format("{0}<br><color=#ffca27>{1}/{2}</color>", str1, nWinCount, nConditionWinCount);
            UBB.toHtml(ref str2);
            FinishState.text = str2;
            FinishState.gameObject.SetActive(bHaveState);
        }
    }

    public class RankDetailPrizeFrame : IRankDetailFrame
    {
        public Text TitleDesc;
        public Text SubTitleDesc;

        public RankDetialPrizeItem[] PrizeItems;

        public void SetData(cmd_entity_rank_season_detail data)
        {   
            TitleDesc.text = "赛季奖励";
            SubTitleDesc.text = " / 赛季结束将按照本赛季最高段位发放奖励";

            // 隐藏
            for (int i = 0; i < PrizeItems.Length; i++)
            {
                PrizeItems[i].gameObject.SetActive(false);
            }

            // 设置
            SSchemeSeasonPrizeConfig config = MatchSeasonPrizeConfig.Instance.GetSeasonPrizeData(data.wPrizeConfigID, data.wTopRankGrade);
            if (config == null)
                return;

            int nIndex = 0;
            for (int i = 0; i < config.arrNormalPrize.Length && nIndex < PrizeItems.Length; i++)// 基础
            {
                PrizeItems[nIndex].SetData(config.arrNormalPrize[i], data.wTopRankGrade, config.arrExtraCondition[1], data.wWinCount, false, config.szNormalCondition);
                nIndex++;
            }

            for (int i = 0; i < config.arrExtraPrize.Length && nIndex < PrizeItems.Length; i++)// 额外
            {
                PrizeItems[nIndex].SetData(config.arrExtraPrize[i], config.arrExtraCondition[0], config.arrExtraCondition[1], data.wWinCount, true, config.szExtralCondition);
                nIndex++;
            }
        }
    }

    // 杂项
    public class RankDetailMiscFrame : IRankDetailFrame
    {
        public Text Title;
        public UTooltipTrigger Trigger;

        public Text LadderBtnDesc;
        public Text RecordBtnDesc;
        public Text StartBtnDesc;

        private RankDetailWndView m_wndView;

        public override void Init(RankDetailWndView view)
        {
            base.Init(view);

            m_wndView = view;
        }

        public void SetData()
        {
            Title.text = "排位赛";

            string strTip = ULocalizationService.Instance.Get("UIView", "RankSeason", "TitleTip");
            UBB.toHtml(ref strTip);
            Trigger.SetText(UTooltipParamName.BodyText, strTip);

            LadderBtnDesc.text = "排行榜";
            RecordBtnDesc.text = "历史记录";
            StartBtnDesc.text = "开始匹配";
        }

        public void OnClickLadderBtn()
        {
            UISystem.Instance.ShowWnd(WndID.WND_ID_CHART, true);
        }

        public void OnClickRecordBtn()
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RANK_RECORD_SHOW);
        }

        public void OnClickStartBtn() 
        {


            UIMsgCmdData cmdData = new UIMsgCmdData((int)WndMsgID.WND_MSG_RANK_INVITE_SHOW, (int)EMMatchType.MatchType_Rank, "", IntPtr.Zero, 0);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_RANK_INVITE_SHOW, cmdData);

            m_wndView.SetVisible(false);
        }

        public void OnClickReturnBtn()
        {
            m_wndView.SetVisible(false);
        }
    }

    public class RankDetailWndView : UIBaseWndView
    {
        public Image BaseEffect;

        public RankDetailScoreFrame     ScoreFrame;
        public RankDetailTaskFrame      TaskFrame;
        public RankDetailHonorFrame     HonorFrame;
        public RankDetailPrizeFrame     PrizeFrame;
        public RankDetailMiscFrame      MiscFrame;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            UEffectParamBase materialEffectParam = new UEffectMaterialParam(UEffectMaterialType.EMT_Blur, BaseEffect, true);
            if (UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectMaterial, ref materialEffectParam))
            {
                (materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetColor("_TintColor", new Color(0, 0, 0, 0.20f));
                (materialEffectParam as UEffectMaterialParam).ReturnEffectMaterial.SetFloat("_Vibrancy", 0f);
            }

            MiscFrame.Init(this);

            return true;
        }

        public void OnReceiveRankDetail(IntPtr ptrParam, int nPtrLen)
        {
            cmd_entity_rank_season_detail detail = IntPtrHelper.toData<cmd_entity_rank_season_detail>(ptrParam);

            ScoreFrame.SetData();
            TaskFrame.SetData();
            HonorFrame.SetData(detail);
            PrizeFrame.SetData(detail);
            MiscFrame.SetData();
        }

        public void OnTaskUpdate()
        {
            TaskFrame.SetData();
        }
    }
}
