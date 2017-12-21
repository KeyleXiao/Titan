/*******************************************************************
** 文件名:	TeamRightWndView.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-4-12
** 版  本:	1.0
** 描  述:	天赋页界面视图模块
		
********************************************************************/

using DataCenter;
using GameLogic;
using System;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using USpeedUI.TeamRightPart;
using USpeedUI.UEffect;

namespace USpeedUI
{
    public class TimeCountDown : MonoBehaviour
    {
        private long lEndTicks;

        protected float fCurTime;

        protected float fDeltaTime;

        protected float fOriginalTime;

        public virtual void StartCountDown(float nReliveTime, float time, float repeatRate)
        {
            lEndTicks = DateTime.Now.AddSeconds((double)nReliveTime).Ticks;
            this.fCurTime = nReliveTime;
            fOriginalTime = nReliveTime;
            fDeltaTime = repeatRate;
            InvokeRepeating("CountDownRepeatCallBack", time, repeatRate);
        }

        protected virtual void CountDownRepeatCallBack()
        {
            if (lEndTicks <= DateTime.Now.Ticks)
            {
                CountDownCompleteCallBack();
            }

            long leftTick = lEndTicks - DateTime.Now.Ticks;
            fCurTime = (float)leftTick / 10000000;
        }

        protected virtual void CountDownCompleteCallBack()
        {
            CancelInvoke();
            fCurTime = 0;
            fDeltaTime = 0;
            fOriginalTime = 0;
            lEndTicks = 0;
        }
    }

    public class TeamReliveCountDown : TimeCountDown
    {
        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();

            this.gameObject.GetComponent<TeamRightWndItem>().TimeCountText.text = ((int)fCurTime).ToString();
        }

        protected override void CountDownCompleteCallBack()
        {
            base.CountDownCompleteCallBack();

            TeamRightWndItem item = this.gameObject.GetComponent<TeamRightWndItem>();
            item.TimeCountText.gameObject.SetActive(false);
            item.DieMask.SetActive(false);
            item.HPBar.gameObject.SetActive(true);
            item.MPBar.gameObject.SetActive(true);
        }
    }

    public class OtherTeamReliveCountDown : TimeCountDown
    {

        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();

            this.gameObject.GetComponent<TeamRightWndItem>().TimeCountText.text = ((int)fCurTime).ToString();
        }

        protected override void CountDownCompleteCallBack()
        {
            base.CountDownCompleteCallBack();

            TeamRightWndItem item = this.gameObject.GetComponent<TeamRightWndItem>();
            item.TimeCountText.gameObject.SetActive(false);
            item.DieMask.SetActive(false);
        }
    }

    public class MaxSkillIconCountDown : TimeCountDown
    {
        protected override void CountDownRepeatCallBack()
        {
            base.CountDownRepeatCallBack();

            TeamRightWndItem item = this.gameObject.GetComponent<TeamRightWndItem>();
            item.SkillIconImage.fillAmount = (fOriginalTime - fCurTime) / (fOriginalTime);
        }
    }

    public class TeamRightWndItem : MonoBehaviour
    {
        public Image HeroIconImage;
        public Image SkillIconImage;
        public Slider HPBar;
        public Slider MPBar;
        public Text TimeCountText;
        public Text LevelText;
        public GameObject DieMask;
        //private bool m_bHasPlayer = false;

        //public bool HasPlayer
        //{
        //    set { m_bHasPlayer = value;}
        //    get { return m_bHasPlayer; }
        //}
    }

    public class TeamRightWndView : UIBaseWndView, ITimerHanlder
    {
        enum EN_GameHud_Timer
        {
            ENGT_GameTimer = 0,     // 游戏时间定时器id
        }

        TeamRightWnd TeamRightWnd;
        [SerializeField]
        public TeamRightWndItem[] m_ArrayMyTeam;
        [SerializeField]
        public TeamRightWndItem[] m_ArrayOtherTeam;
        public int nOtherTeamDieSlotId = 0;
        private Dictionary<int, UEffectParamBase> dicOldUEffectParam = new Dictionary<int, UEffectParamBase>();
        public GameObject[] AICommandBtn;
        public Sprite[] AIBtnIcon;

        public Text gameTime;
        public Text ourKillNum;
        public Text opponentKillNum;

        float m_nextUpdateTime = 0.0f;
        public override void Start()
        {
            if (m_wnd == null)//测试用
            {   //{
                //    TeamRightWnd = new TeamRightWnd();
                //    TeamRightWnd.m_wndView = this;
            }
            else
            {
                TeamRightWnd = (TeamRightWnd)m_wnd;
            }
            //ShowMyTeamReliveTime(10, 0);
            //bool a = false;
            //SetMyTeamSkillIcon(20, 0, ref a);
            //ShowOtherTeamReliveTime(10, 0)
            return;
        }

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);

            for (int i = 0; i < AICommandBtn.Length; i++)
            {
                AICommandBtn[i].SetActive(false);
            }

            gameTime.text = "00:00";
            ourKillNum.text = "0";
            opponentKillNum.text = "0";
            TimerManager.SetTimer(this, (int)EN_GameHud_Timer.ENGT_GameTimer, 1.0f);

            return true;
        }

        public override void Update()
        {
            base.Update();
            if (Time.unscaledTime < m_nextUpdateTime)
            {
                return;
            }

            if (EntityFactory.MainHeroView == null || EntityFactory.MainHeroView.Property == null)
            {
                return;
            }

            // 设置阵营数据

            ourKillNum.text = LogicDataCenter.recordTableDataManager.getWarSameCampPersonKillNum(EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP)).ToString();
            opponentKillNum.text = LogicDataCenter.recordTableDataManager.getWarOtherCampPersonKillNum(EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CAMP)).ToString();
            m_nextUpdateTime = Time.unscaledTime + 0.5f;
        }

        public void SetMyTeamItemViewShow(int nSlotId, int nHeroId, int nLevel)
        {
            if (IsOutOfMyTeamArray(nSlotId))
            {
                return;
            }

            TeamRightWndItem myTeamItem = m_ArrayMyTeam[nSlotId];
            //if (myTeamItem.HasPlayer)
            //{
            //    return;
            //}

            //myTeamItem.HasPlayer = true;
            myTeamItem.gameObject.SetActive(true);
            myTeamItem.HeroIconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_TEAMRIGHTVIEW, 1, 6, nHeroId);
            myTeamItem.LevelText.text = nLevel.ToString();
        }

        public void SetOtherTeamItemViewShow(int nSlotId, int nHeroId, int nLevel)
        {
            if (IsOutOfOtherTeamArray(nSlotId))
            {
                return;
            }

            TeamRightWndItem otherTeamItem = m_ArrayOtherTeam[nSlotId];
            //if (otherTeamItem.HasPlayer)
            //{
            //    return;
            //}

            //otherTeamItem.HasPlayer = true;
            otherTeamItem.gameObject.SetActive(true);
            otherTeamItem.HeroIconImage.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_TEAMRIGHTVIEW, 1, 6, nHeroId);
            otherTeamItem.LevelText.text = nLevel.ToString();
        }

        public void SetMyTeamHPChange(int nHPMax, int nHPValue, int nSlotId)
        {
            if (IsOutOfMyTeamArray(nSlotId))
            {
                return;
            }

            if (nHPMax <= 0)
            {
                return;
            }

            m_ArrayMyTeam[nSlotId].HPBar.value = (float)nHPValue / nHPMax;
        }

        public void SetMyTeamMPChange(int nMPMax, int nMPValue, int nSlotId)
        {
            if (IsOutOfMyTeamArray(nSlotId))
            {
                return;
            }

            if (nMPMax <= 0)
            {
                return;
            }

            m_ArrayMyTeam[nSlotId].MPBar.value = (float)nMPValue / nMPMax;
        }

        public void SetMyTeamLevelChange(int nLevel, int nSlotId)
        {
            if (IsOutOfMyTeamArray(nSlotId))
            {
                return;
            }

            m_ArrayMyTeam[nSlotId].LevelText.text = nLevel.ToString();
        }

		public void SetOtherTeamLevelChagne(int nLevel, int nSlotId)
		{
			if (IsOutOfMyTeamArray(nSlotId))
			{
				return;
			}

			m_ArrayOtherTeam[nSlotId].LevelText.text = nLevel.ToString();
		}

        public void ShowMyTeamReliveTime(int nReliveTime, int nSlotId)
        {
            if (IsOutOfMyTeamArray(nSlotId))
            {
                return;
            }

            TeamRightWndItem myTeamItem = m_ArrayMyTeam[nSlotId];
            myTeamItem.TimeCountText.gameObject.SetActive(true);
            myTeamItem.DieMask.SetActive(true);
            myTeamItem.TimeCountText.text = nReliveTime.ToString();
            myTeamItem.HPBar.gameObject.SetActive(false);
            myTeamItem.MPBar.gameObject.SetActive(false);

            myTeamItem.gameObject.GetComponent<TeamReliveCountDown>().StartCountDown(nReliveTime, 1.0f, 1.0f);
        }

        public void SetMyTeamSkillIcon(int nCoolTime, int nSlotId, ref bool bFirstLearn)
        {
            if (IsOutOfMyTeamArray(nSlotId))
            {
                return;
            }

            TeamRightWndItem myTeamItem = m_ArrayMyTeam[nSlotId];

            if (nCoolTime <= 0)
            {
                if (bFirstLearn)
                {
                    myTeamItem.SkillIconImage.fillAmount = 1;
                    bFirstLearn = false;
                }
            }
            else
            {
                myTeamItem.gameObject.GetComponent<MaxSkillIconCountDown>().StartCountDown(nCoolTime, 0.5f, 0.5f);
            }
        }

        public void ShowOtherTeamReliveTime(int nReliveTime, int nSlotID)
        {
            if (IsOutOfOtherTeamArray(nSlotID))
            {
                return;
            }

            TeamRightWndItem otherTeamItem = m_ArrayOtherTeam[nSlotID];
            otherTeamItem.DieMask.SetActive(true);
            otherTeamItem.gameObject.SetActive(true);
            otherTeamItem.TimeCountText.gameObject.SetActive(true);
            otherTeamItem.TimeCountText.text = nReliveTime.ToString();

            otherTeamItem.gameObject.GetComponent<OtherTeamReliveCountDown>().StartCountDown(nReliveTime, 1.0f, 1.0f);
            nOtherTeamDieSlotId++;
        }

        public void ReduceOtherTeamSlot()
        {
            nOtherTeamDieSlotId--;
            if (nOtherTeamDieSlotId < 0)
            {
                nOtherTeamDieSlotId = 0;
            }
        }

        public void OnMyTeamItemClick(int nSlotId)
        {
            if (TeamRightWnd == null)
            {
                return;
            }

            TeamRightWnd.SendTeamRightClickPlayerUID(nSlotId);
        }

        public bool IsOutOfMyTeamArray(int nSlotId)
        {
            if (nSlotId < 0 || nSlotId >= m_ArrayMyTeam.Length)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public bool IsOutOfOtherTeamArray(int nSlotId)
        {
            if (nSlotId < 0 || nSlotId >= m_ArrayOtherTeam.Length)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public void SetXpSkillFireEffect(int nSlotId)
        {
            if (IsOutOfMyTeamArray(nSlotId))
            {
                return;
            }

            if (dicOldUEffectParam.ContainsKey(nSlotId))
            {
                return;
            }

            TeamRightWndItem myTeamItem = m_ArrayMyTeam[nSlotId];

            UEffectParamBase param = new UEffectPrefabParam(_eType: UEffectPrefabType.UEPT_UTeamRight_XpSkillFire, _tfAttachParent: myTeamItem.HeroIconImage.transform);
            UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref param);

            dicOldUEffectParam[nSlotId] = param;
            UEffectPrefabParam paramPrefab = (UEffectPrefabParam)param;
            paramPrefab.EffectGameObject.transform.localPosition = new Vector3(0, 0, 0);
        }

        public void DestoryXpSkillFireEffect(int nSlotId)
        {
            if (IsOutOfMyTeamArray(nSlotId))
            {
                return;
            }

            if (!dicOldUEffectParam.ContainsKey(nSlotId))
            {
                return;
            }

            UEffectParamBase param = dicOldUEffectParam[nSlotId];
            UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref param);
            dicOldUEffectParam.Remove(nSlotId);
        }

        public override void Destroy()
        {
            TimerManager.KillTimer(this, (int)EN_GameHud_Timer.ENGT_GameTimer);
            base.Destroy();
            dicOldUEffectParam.Clear();
        }

        // 服务器下发AICommand消息  C++  ---->  C#
        // 1.初始化(init)，AI角色的AIComman按钮
        // 2.更新(update)，玩家上线、下线消息，是否隐藏AICommand按钮
        public void SetMyTeamAICommandBtn(bool bIsAI, int nSlotId)
        {
            // 暂时关闭该功能
            return;

            //if (IsOutOfMyTeamArray(nSlotId))
            //{
            //    return;
            //}

            //if (bIsAI)
            //{
            //    AICommandBtn[nSlotId].SetActive(true);
            //}
            //else
            //{
            //    AICommandBtn[nSlotId].SetActive(false);
            //}
        }

        // 通知服务器控制相应AI  C#  ---->  C++
        public void OnAICommandBtnClick(int nSlotId)
        {
            if (IsOutOfMyTeamArray(nSlotId))
            {
                return;
            }

            if (!LogicDataCenter.teamRightWndDataManager.MyTeamEntityIdTable.ContainsKey(nSlotId))
            {
                return;
            }
            int nUID = (int)LogicDataCenter.teamRightWndDataManager.MyTeamEntityIdTable[nSlotId];

            if (LogicDataCenter.teamRightWndDataManager.BIsJoinFollow[nSlotId])
            {
                // 图片变为加入跟随
                AICommandBtn[nSlotId].GetComponent<Image>().sprite = AIBtnIcon[0];

                cmd_war_ai_command_break_follow data = new cmd_war_ai_command_break_follow();
                data.uidTarget = nUID;
                EntityEventHelper.Instance.SendCommand<cmd_war_ai_command_break_follow>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_AI_COMMAND_BREAK_FOLLOW, ref data);
            }
            else
            {
                // 图片变为取消跟随
                AICommandBtn[nSlotId].GetComponent<Image>().sprite = AIBtnIcon[1];

                cmd_war_ai_command_follow data = new cmd_war_ai_command_follow();
                data.uidTarget = nUID;
                EntityEventHelper.Instance.SendCommand<cmd_war_ai_command_follow>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_AI_COMMAND_FOLLOW, ref data);
            }

            LogicDataCenter.teamRightWndDataManager.BIsJoinFollow[nSlotId] = !LogicDataCenter.teamRightWndDataManager.BIsJoinFollow[nSlotId];
        }

        public void OnTimer(int nTimerID)
        {
            switch (nTimerID)
            {
                case (int)EN_GameHud_Timer.ENGT_GameTimer:
                    {
						uint nWarGameTimeMs = GameLogicAPI.getSyncTickCount() - LogicDataCenter.warInfoDataManager.WarStartTime;
						LogicDataCenter.warInfoDataManager.WarPlayTimeSec = nWarGameTimeMs / 1000;

                        uint nMin, nSec;
                        string strMin, strSec;
                        nMin = LogicDataCenter.warInfoDataManager.WarPlayTimeSec / 60;
                        nSec = LogicDataCenter.warInfoDataManager.WarPlayTimeSec % 60;
                        strMin = nMin.ToString("00");
                        strSec = nSec.ToString("00");

                        //更新游戏时间
                        gameTime.text = strMin + ":" + strSec;
                    }
                    break;
                default:
                    break;
            }
        }
    }
}