/*******************************************************************
** 文件名:	WarBroadcastMessageMgrWnd.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	周坤
** 日  期:	2016-6-23
** 版  本:	1.0
** 描  述:	战场广播界面逻辑模块(接发消息)+视图模块(显示数据)	
********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Timers;
using GameLogic;
using UnityEngine;
using UnityEngine.UI;
using DG.Tweening;
using DataCenter;
using USpeedUI.UEffect;
using USpeedUI.UExtensions;
using System.Collections;
using U3D_Render.Common;
using USpeedUI.TooltipAndDialog;

namespace USpeedUI.WarBroadcastMessageMgr
{
    enum ESpecialContributionType
    {
        ESCT_THRILLSAVE,				// 惊现救治
        ESCT_DEADLYCONTROL,				// 夺命控制
        ESCT_TEAMBATTLECONTROL,			// 团控
    }

    enum BroadcastType
    {
        EMBT_KILLCOUNT,
        //EMBT_COUNTACEKILL,
        EMBT_IMPORTANTEV,
        EMBT_ACEKILL,
        //EMBT_IMPORTANTACEKILL,
        EMBT_MONSTERRELIVE,
        EMBT_MONSTERDIE,
        EMBT_ALIVEASSIST_1,
        EMBT_ALIVEASSIST_2,
        EMBT_SPECIALCONTRIBUTION_1,
        EMBT_SPECIALCONTRIBUTION_2,
    }

    enum WarKillSoundType
    {
        WKST_NONE,
        WKST_FIRSTBLOOD,
        WKST_SELF2Kill_Quick,
        WKST_SELF3Kill_Quick,
        WKST_SELF4Kill_Quick,
        WKST_SELF5Kill_Quick,
        WKST_ACE,
        WKST_VICTORY,
        WKST_ENMY2Kill_Quick,
        WKST_ENMY3Kill_Quick,
        WKST_ENMY4Kill_Quick,
        WKST_ENMY5Kill_Quick,

        WKST_SELF3Kill_NoDie = 30,
        WKST_SELF4Kill_NoDie = 33,
        WKST_SELF5Kill_NoDie = 36,
        WKST_SELF6Kill_NoDie = 39,
        WKST_SELF7Kill_NoDie = 42,
        WKST_SELF8Kill_NoDie = 45,

        WKST_ENMY3Kill_NoDie = 28,
        WKST_ENMY4Kill_NoDie = 31,
        WKST_ENMY5Kill_NoDie = 34,
        WKST_ENMY6Kill_NoDie = 37,
        WKST_ENMY7Kill_NoDie = 40,
        WKST_ENMY8Kill_NoDie = 43,

        WKST_SELFCONTINUOUSASSIST = 61,
        WKST_SELFEXCELLENTASSIST ,
        WKST_SELFGODLIKEASSIST ,
        WKST_SELFSTUNKILL ,
        WKST_SELFEMERGENTHEAL ,
        WKST_SELFETEAMCONTROL1,
        WKST_SELFETEAMCONTROL2,
        WKST_SELFETEAMCONTROL3,
    }

	public struct BroadcastMsgData
    {
        public int nType;
        public int isSameCamp;
        public int nTimeKillCount;
        public int nIsOver;
        public int nIsFirstBlood;
        public int nMurdererEntityType;     //杀人者
        public int nMurdererHeroID;
        public int nMasterEntityType;       //被杀者
        public int nMasterHeroID;
        public string MurdererName;
        public string MurdererVocation;
        public string MasterName;
        public string MasterVocation;
        public string szDes;
        public int nSoundID;
        public int nAddTalentPoint;
        public int nPriority;
        public bool bChatShow;
        public int  nReserve;
        public int[] nAssistArray;
        public bool bMainHeroOperate;
    }



    public class BroadcastTipSmallKillerItem : MonoBehaviour
    {
        public Image HeadPortraitBase;
        public Image HeadPortrait;

        public void SetData(UWarBroadcastMessageMgrWndView view, bool bSameCamp, int nHeroID)
        {
            if (bSameCamp)
                HeadPortraitBase.sprite = view.GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TipSmallHead_Self);
            else
                HeadPortraitBase.sprite = view.GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TipSmallHead_Enmy);

            HeadPortrait.sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_BROADCASTMESSAGEMGR, 1, 4, nHeroID);
        }
    }

    public class BroadcastTipItem : MonoBehaviour
    {
        public GameObject SmallKillerObj;
        public RectTransform SmallKillerFrame;
        public Image KillerHeadPortrait;
        public Image KillerHeadPortraitBase;
        public Image DeadHeadPortrait;
        public Image DeadHeadPortraitBase;
        public Image KillIcon;
        public Image KillBase;

        public void SetData(UWarBroadcastMessageMgrWndView view, BroadcastMsgData data)
        {
            bool bSameCamp = data.isSameCamp > 0;

            for (int i = 0; i < data.nAssistArray.Length; i++)
            {
                if (data.nAssistArray[i] <= 0)
                    continue;

                GameObject killerObj = ResNode.InstantiateRes(SmallKillerObj);
                if (!killerObj)
                    return;

				//RectTransform killerRect = killerObj.transform as RectTransform;
				killerObj.transform.SetParent(SmallKillerFrame, false);
                BroadcastTipSmallKillerItem killerItem = killerObj.GetComponent<BroadcastTipSmallKillerItem>();
                if (killerItem == null)
                    return;
                killerItem.SetData(view, bSameCamp, data.nAssistArray[i]);

                killerObj.SetActive(true);
            }
            SmallKillerObj.SetActive(false);

            KillIcon.sprite = view.GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TipKillIcon);
            KillerHeadPortrait.sprite = view.GetKillerHeadPortrait(data);
            DeadHeadPortrait.sprite = view.GetDeathHeadPortrait(data);
            if (bSameCamp)
            {
                KillerHeadPortraitBase.sprite = view.GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TipBigHead_Self);
                DeadHeadPortraitBase.sprite = view.GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TipBigHead_Enmy);
                KillBase.sprite = view.GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TipFrame_Self);
            }
            else
            {
                KillerHeadPortraitBase.sprite = view.GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TipBigHead_Enmy);
                DeadHeadPortraitBase.sprite = view.GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TipBigHead_Self);
                KillBase.sprite = view.GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TipFrame_Enmy);
            }
        }
    }

    public class UWarBroadcastMessageMgrWnd : UIPopupWnd<UWarBroadcastMessageMgrWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_WAR_BROADCASTMESSAGEMGR;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_POPUP01;
        }

        public override string GetPrefabPath()
        {
            return "DynamicState_Moba/WarBroadcastMessageMgrView/WarBroadcastMessageMgrView";
        }

        public override bool Init()
        {
            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_KILLCOUNT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_IMPORTANTEV, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_MONSTERRELIVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_MONSTERDIE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_ALIVEASSIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_SPECAILCONTRIBUTION, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_PLAYER_RECONNECTION, this);

            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();

            // 退订聊天消息更新消息
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, this);
            
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_WAREND, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_KILLCOUNT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_IMPORTANTEV, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_MONSTERRELIVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_MONSTERDIE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_ALIVEASSIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_SPECAILCONTRIBUTION, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_PLAYER_RECONNECTION, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER:
                    {
                        this.SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE:
                case WndMsgID.WND_MSG_COMMON_WAREND:
                    {
                        this.UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_KILLCOUNT:
                    {                       
                        if (m_wndView == null)
                            return;
                        m_wndView.DoKillCountBroadCast(msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_IMPORTANTEV:
                    {
                        if (m_wndView == null)
                            return;
                        m_wndView.DoImportantEvBroadCast(msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_MONSTERRELIVE:
                    {
                        if (m_wndView == null)
                            return;
                        DoMonsterRelieveBroadCast(msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_MONSTERDIE:
                    {
                        if (m_wndView == null)
                            return;
                        DoMonsterDieBroadCast(msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_ALIVEASSIST:
                    {
                        if (m_wndView == null)
                            return;

                        m_wndView.DoAliveAssistBrodCast(msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_SPECAILCONTRIBUTION:
                    {
                        if (m_wndView == null)
                            return;
                        m_wndView.DoSpecialContributionBrodCast(msgData);
                    }
                    break;
                case WndMsgID.WND_MSG_WARBROADCASTMESSAGEMGR_PLAYER_RECONNECTION:
                    {
                        if (m_wndView == null)
                            return;
                        m_wndView.DoPlayerReconnection(msgData as UIReconnectData);
                    }
                    break;
                default:
                    break;
            }
        }

        private void DoMonsterRelieveBroadCast(UIMsgData msgData)
        {
            UIMsgCmdData uiShowMonsterReliveData = (UIMsgCmdData)msgData;
            cmd_war_important_monsterliveinfo data = IntPtrHelper.toData<cmd_war_important_monsterliveinfo>(uiShowMonsterReliveData.ptrParam);

            BroadcastMsgData monsterReliveData = new BroadcastMsgData();
            monsterReliveData.nType = (int)BroadcastType.EMBT_MONSTERRELIVE;
            monsterReliveData.szDes = data.szDes;
            monsterReliveData.nSoundID = data.nSoundID;
            m_wndView.AddNewKillBroadCast(monsterReliveData);
        }

        private void DoMonsterDieBroadCast(UIMsgData msgData)
        {
            UIMsgCmdData uiShowMonsterDieData = (UIMsgCmdData)msgData;
            cmd_war_important_monsterdietxtinfo data = IntPtrHelper.toData<cmd_war_important_monsterdietxtinfo>(uiShowMonsterDieData.ptrParam);

            BroadcastMsgData monsterDieData = new BroadcastMsgData();
            monsterDieData.nType = (int)BroadcastType.EMBT_MONSTERDIE;
            monsterDieData.szDes = data.szDes;
            monsterDieData.nSoundID = data.nSoundID;
            m_wndView.AddNewKillBroadCast(monsterDieData);
        }
 
    }

    public class UWarBroadcastMessageMgrWndView : UIBaseWndView
    {
        public GameObject TimeKillFrame;
        public GameObject AceKillFrame;
        public GameObject NormalKillFrame;
        public GameObject FirstBloodFrame;
        public GameObject MoreKillFrame;
        public GameObject GodFrame;
        public GameObject MonsterReliveFrame;
        public GameObject MonsterDieFrame;
        public GameObject Assist1Frame;
        public GameObject Assist2Frame;
        public GameObject SpecialContribution1Frame;
        public GameObject SpecialContribution2Frame;
        public Image TimeKillIcon;
        public Image TimeKillerHeadPortrait;
        public Transform TimeKillEffectAttach;
        private UEffectParamBase m_effectParam;

        public Image AceKillIcon;
        public Image AceKillerHeadPortrait;

        public Image FirstBloodIcon;
        public Image FirstBloodHeadPortrait;

        public Image NormalKillerIcon;
        public Image NormalDeathIcon;
        public Image NormalKillerHeadPortrait;
        public Image NormalDeathHeadPortrait;
        public Text  NormalCombatInfo1;
        public Text  NormalCombatInfo2;
        public Text  NormalCombatInfo3;
        public Image NormalKillIcon;

        //public Image MoreKillHeadIcon;
        public Image MoreKillIcon;
        public Image MoreKillHeadPortrait;
        public Text  MoreKillCombatInfo1;
        public Text  MoreKillCombatInfo2;
        public Text  MoreKillCombatInfo3;
        public UGradient MoreKillInfo2;
        public UGradient MoreKillInfo3;
        public Image GodHeadIcon;
        public Image GodHeadPortrait;
        public Image GodKillDes;
        public Text  GodCombatInfo1;
        public Text  GodCombatInfo2;

        public Text MonsterReliveInfo;

        public Text MonsterDieInfo;


        public Text Assist1NameText;
        public Image Assist1HeadIcon;
        public Image Assist2NumSprite;

        public Image Contribution1WordImage;
        public Image Contribution2HeadImage;
        public Image Contribution2FrameImage;
        public Text Contribution2WordText;

        public RectTransform BroadcastTipFrame;
        public GameObject BroadcatTipItemTemplate;
        private List<BroadcastMsgData> m_broadcastTipData;
        private const int BroadcastTipItemViewCount = 3;

        private List<BroadcastMsgData> LastBroadKill;
        private string redColor = "255+0+0";     //"<color=#ff0000ff>";
        private string blueColor = "0+255+255";  //"<color=#00ffffff>";
        private string goldColor = "255+240+15"; //"<color=#fff00fff>";
        private string strColorMur = null;
        private string strColorMas = null;
        private bool m_isStopCountdown = false;

        public override bool Init(IUIWnd wnd)
        {
            base.Init(wnd);
            LastBroadKill = new List<BroadcastMsgData>();
            m_broadcastTipData = new List<BroadcastMsgData>();
            return true;
        }

        public override void Destroy()
        {
            LastBroadKill.Clear();
            m_broadcastTipData.Clear();
            base.Destroy();
        }

        public void DoKillCountBroadCast(UIMsgData msgData)
        {
            UIMsgCmdData uiBroadKillCountData = (UIMsgCmdData)msgData;
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(uiBroadKillCountData.ptrParam, uiBroadKillCountData.nPtrLen);

            cmd_war_entity_killcountinfo data = helper.get<cmd_war_entity_killcountinfo>();

            BroadcastMsgData countData = new BroadcastMsgData();
            countData.nType = (int)BroadcastType.EMBT_KILLCOUNT;
            countData.isSameCamp = data.isSampCampKill;
            countData.nTimeKillCount = data.nTimeKillCount;
            countData.nMurdererHeroID = data.nMurdererHeroID;
            countData.nMasterHeroID = data.nMasterHeroID;
            countData.MurdererName = data.MurdererName;
            countData.MurdererVocation = data.MurdererVocationName;
            countData.MasterName = data.MasterName;
            countData.MasterVocation = data.MasterVocationName;
            countData.nPriority = 6 - data.nTimeKillCount;
            countData.bMainHeroOperate = data.uidMurderer == EntityFactory.MainHeroID;
            countData.nAssistArray = new int[data.nAssistCount];

            // 读取助攻者列表
            List<cmd_war_assist_info> assistList = new List<cmd_war_assist_info>();
            for (int i = 0; i < data.nAssistCount; ++i)
            {
                cmd_war_assist_info assistInfo = helper.get<cmd_war_assist_info>();
                assistList.Add(assistInfo);

                countData.nAssistArray[i] = assistInfo.nHerorID;
            }

            if (countData.nTimeKillCount > 1 && countData.nTimeKillCount <= 5)
            {
                AddNewKillBroadCast(countData);
            }

            if (data.nIsAce != 0)
            {
                BroadcastMsgData aceCountData = new BroadcastMsgData();
                aceCountData.nType = (int)BroadcastType.EMBT_ACEKILL;
                aceCountData.isSameCamp = data.isSampCampKill;
                aceCountData.nMurdererHeroID = data.nMurdererHeroID;
                aceCountData.nMurdererEntityType = data.MurdererEntityType;
                AddNewKillBroadCast(aceCountData);
            }

            AddBroadCastTipItem(countData);
        }

        public void DoImportantEvBroadCast(UIMsgData msgData)
        {
            UIMsgCmdData uiBroadImportantEVData = (UIMsgCmdData)msgData;
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(uiBroadImportantEVData.ptrParam, uiBroadImportantEVData.nPtrLen);

            cmd_war_important_entiydieinfo data = helper.get<cmd_war_important_entiydieinfo>();

            BroadcastMsgData importantData = new BroadcastMsgData();
            importantData.nType = (int)BroadcastType.EMBT_IMPORTANTEV;
            importantData.isSameCamp = data.isSampCampKill;
            importantData.nTimeKillCount = data.nKillCount;
            importantData.nMurdererHeroID = data.nMurdererHeroID;
            importantData.nMasterHeroID = data.nMasterHeroID;
            importantData.nMurdererEntityType = data.MurdererEntityType;
            importantData.nMasterEntityType = data.MasterEntityType;
            importantData.nIsOver = data.bIsOver;
            importantData.MurdererName = data.MurdererName;
            importantData.MasterName = data.MasterName;
            importantData.nSoundID = data.nSoundID;
            importantData.nIsFirstBlood = data.bIsFirstBlood;
            importantData.nAddTalentPoint = data.nAddTalentPoint;
            importantData.MurdererVocation = data.MurdererVocationName;
            importantData.MasterVocation = data.MasterVocationName;
            importantData.nPriority = 5;
            importantData.bMainHeroOperate = data.uidMurderer == EntityFactory.MainHeroID;
            importantData.nAssistArray = new int[data.nAssistCount];

            // 读取助攻者列表
            List<cmd_war_assist_info> assistList = new List<cmd_war_assist_info>();
            for (int i = 0; i < data.nAssistCount; ++i)
            {
                cmd_war_assist_info assistInfo = helper.get<cmd_war_assist_info>();
                assistList.Add(assistInfo);

                importantData.nAssistArray[i] = assistInfo.nHerorID;
            }

            AddNewKillBroadCast(importantData);

            if (data.bIsAce != 0)
            {
                BroadcastMsgData aceImportantData = new BroadcastMsgData();
                aceImportantData.nType = (int)BroadcastType.EMBT_ACEKILL;
                aceImportantData.isSameCamp = data.isSampCampKill;
                aceImportantData.nMurdererHeroID = data.nMurdererHeroID;
                aceImportantData.nMurdererEntityType = data.MurdererEntityType;
                AddNewKillBroadCast(aceImportantData);
            }

            AddBroadCastTipItem(importantData);
        }

        public void DoAliveAssistBrodCast(UIMsgData msgData)
        {
            PlayerAliveAssistData uiData = (PlayerAliveAssistData)msgData;
            BroadcastMsgData broadcastMsgData = new BroadcastMsgData();
            broadcastMsgData.nType = (int)uiData.nAssistType;
            broadcastMsgData.szDes = uiData.szDesc;
            broadcastMsgData.nSoundID = uiData.nSoundID;
            broadcastMsgData.MasterName = uiData.nPlayerName;
            broadcastMsgData.nMasterHeroID = uiData.nHeroID;
            broadcastMsgData.bChatShow = uiData.bChatShow;
            broadcastMsgData.isSameCamp = uiData.bSameCamp ? 1 : 0;
            broadcastMsgData.nPriority = 6;
            broadcastMsgData.nReserve = uiData.nAssistCount;


            AddNewKillBroadCast(broadcastMsgData);
        }

        public void DoSpecialContributionBrodCast(UIMsgData msgData)
        {
            UIMsgCmdData uiData = (UIMsgCmdData)msgData;
            cmd_War_Special_Contribution data = IntPtrHelper.toData<cmd_War_Special_Contribution>(uiData.ptrParam);

            BroadcastMsgData broadcastMsgData = new BroadcastMsgData();
            string sDesc = null;
            int nBroadType = -1;
            int nReverse = -1;
            if (data.nType == (int)ESpecialContributionType.ESCT_THRILLSAVE)
            {
                sDesc = "惊现救治";
                broadcastMsgData.nSoundID = (int)WarKillSoundType.WKST_SELFEMERGENTHEAL;
                nBroadType = (int)BroadcastType.EMBT_SPECIALCONTRIBUTION_1;
                nReverse = data.nType;
            }
            else if (data.nType == (int)ESpecialContributionType.ESCT_DEADLYCONTROL)
            {
                sDesc = "夺命控制";
                broadcastMsgData.nSoundID = (int)WarKillSoundType.WKST_SELFSTUNKILL;
                nBroadType = (int)BroadcastType.EMBT_SPECIALCONTRIBUTION_1;
                nReverse = data.nType;
            }
            else if (data.nType == (int)ESpecialContributionType.ESCT_TEAMBATTLECONTROL)
            {
                sDesc = "控" + data.nParam1 + "人";
                broadcastMsgData.nSoundID = (int)WarKillSoundType.WKST_SELFETEAMCONTROL1 + data.nParam1 - 3;
                broadcastMsgData.nPriority = 6;
                nBroadType = (int)BroadcastType.EMBT_SPECIALCONTRIBUTION_2;
                nReverse = data.nParam1;
            }

            sDesc = "<color=red>" + sDesc + "</color>";
            data.szOperatorName = "<color=red>" + data.szOperatorName + "</color>";
            broadcastMsgData.nType = nBroadType;
            broadcastMsgData.szDes = sDesc;
            broadcastMsgData.MasterName = data.szOperatorName;
            broadcastMsgData.nMasterHeroID = data.nOperatorHeroID;
            broadcastMsgData.isSameCamp = 1;
            broadcastMsgData.bChatShow = false;
            broadcastMsgData.nReserve = nReverse;

            AddNewKillBroadCast(broadcastMsgData);
        }

        public void AddNewKillBroadCast(BroadcastMsgData msgData)
        {
            LastBroadKill.Add(msgData);
            if (LastBroadKill.Count == 1)
            {
                
                GameObject baseIns = GetBroadcastIns(LastBroadKill[0]);
                if (baseIns == null)
                    return;

                baseIns.SetActive(true);
                SetBroadcastIns(LastBroadKill[0]);

                CaptureMoment(LastBroadKill[0]);

                Invoke("BroadcastComplete", 3.0f);

                //Tweener baseTweener = baseIns.GetComponent<CanvasGroup>().DOFade(0, 2.5f).SetDelay(0.5f);
                //baseTweener.OnComplete(() => BroadcastComplete(LastBroadKill[0]));
            }
        }

        public void DoPlayerReconnection(UIReconnectData data)
        {
            InputManager.Available = false;
            StopCoroutine(StartCountdown());
            StartCoroutine(StartCountdown());
        }

        public bool OnSureBtnClicked()
        {
            StopCoroutine(StartCountdown());
            m_isStopCountdown = true;
            InputManager.Available = true;
            return true;
        }

        public void OnCloseBtnClicked()
        {
            OnSureBtnClicked();
        }

        public IEnumerator StartCountdown()
        {
            m_isStopCountdown = false;
            for (int i = 10; i > 0; --i)
            {
                if (m_isStopCountdown)
                    yield break;
                string content = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "ContinuePlayContent");
                UIWidgets.DialogActions buttons = new UIWidgets.DialogActions() { { String.Format(ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "ContinuePlaySureBtn"), i), OnSureBtnClicked } };
                UBB.toHtml(ref content);
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG,
                    new UIDialogData(
                        title: ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "ContinuePlayTitle"),
                        message: content,
                        buttons: buttons,
                        modal: true,
                        hideByModalClick: false,
                        mutex: true,
                        closeButtonAction: OnCloseBtnClicked,
                        eDialogType: EDialogType.WarBroadcastPlayerContinuePlay));
                yield return new WaitForSeconds(1.0f);
            }

            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG_HIDE, new UIDialogHide(EDialogType.WarBroadcastPlayerContinuePlay));
            InputManager.Available = true;
            yield break;
        }

        private void SetBroadcastIns(BroadcastMsgData msgData)
        {
            switch (msgData.nType)
            {
                case (int)BroadcastType.EMBT_KILLCOUNT:
                    {
                        SetKillCountData(msgData);
                    }
                    break;
                case (int)BroadcastType.EMBT_IMPORTANTEV:
                    {
                        SetImportantEVData(msgData);
                    }
                    break;
                case (int)BroadcastType.EMBT_ACEKILL:
                    {
                        SetAceKillData(msgData);
                    }
                    break;
                case (int)BroadcastType.EMBT_MONSTERRELIVE:
                    {
                        SetMonsterReliveData(msgData);
                    }
                    break;
                case (int)BroadcastType.EMBT_MONSTERDIE:
                    {
                        SetMonsterDieData(msgData);
                    }
                    break;
                case (int)BroadcastType.EMBT_ALIVEASSIST_1:
                    {
                        SetAliveAssistData1(msgData);
                    }
                    break;
                case (int)BroadcastType.EMBT_ALIVEASSIST_2:
                    {
                        SetAliveAssistData2(msgData);
                    }
                    break;
                case (int)BroadcastType.EMBT_SPECIALCONTRIBUTION_1:
                    {
                        SetSpecialContributionData1(msgData);
                    }
                    break;
                case (int)BroadcastType.EMBT_SPECIALCONTRIBUTION_2:
                    {
                        SetSpecialContributionData2(msgData);
                    }
                    break;
                default:
                    break;
            }
        }

        private GameObject GetBroadcastIns(BroadcastMsgData msgData)
        {
            GameObject baseKillIns = null;
            switch (msgData.nType)
            {
                case (int)BroadcastType.EMBT_KILLCOUNT:
                    {
                        baseKillIns = TimeKillFrame;
                    }
                    break;
                case (int)BroadcastType.EMBT_IMPORTANTEV:
                    {
                        if (msgData.nIsFirstBlood > 0)
                        {
                            baseKillIns = FirstBloodFrame;
                        }
                        else if (msgData.nIsOver > 0)
                        {
                            baseKillIns = NormalKillFrame;
                        }
                        else
                        {
                            if (msgData.nTimeKillCount >= 0 && msgData.nTimeKillCount < 2)
                            {
                                baseKillIns = NormalKillFrame;
                            }
                            else if (msgData.nTimeKillCount >= 3 && msgData.nTimeKillCount <= 7)
                            {
                                baseKillIns = MoreKillFrame;
                            }
                            else
                            {
                                baseKillIns = GodFrame;
                            }

                        }
                    }
                    break;
                case (int)BroadcastType.EMBT_ACEKILL:
                    {
                        baseKillIns = AceKillFrame;
                    }
                    break;
                case (int)BroadcastType.EMBT_MONSTERRELIVE:
                    {
                        baseKillIns = MonsterReliveFrame;
                    }
                    break;
                case (int)BroadcastType.EMBT_MONSTERDIE:
                    {
                        baseKillIns = MonsterDieFrame;
                    }
                    break;
                case (int)BroadcastType.EMBT_ALIVEASSIST_1:
                    {
                        baseKillIns = Assist1Frame;
                    }
                    break;
                case (int)BroadcastType.EMBT_ALIVEASSIST_2:
                    {
                        baseKillIns = Assist2Frame;
                    }
                    break;
                case (int)BroadcastType.EMBT_SPECIALCONTRIBUTION_1:
                    {
                        baseKillIns = SpecialContribution1Frame;
                    }
                    break;
                case (int)BroadcastType.EMBT_SPECIALCONTRIBUTION_2:
                    {
                        baseKillIns = SpecialContribution2Frame;
                    }
                    break;
                default:
                    break;
            }
            return baseKillIns;
        }

        public void BroadcastComplete()
        {
            if (m_effectParam != null)
            {
                UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_effectParam);
                m_effectParam = null;
            }
            //preBaseIns.GetComponent<CanvasGroup>().alpha = 1;
            GameObject preBaseIns = GetBroadcastIns(LastBroadKill[0]);
            preBaseIns.SetActive(false);

            LastBroadKill.RemoveAt(0);
            LastBroadKill.Sort((x, y) => { if (x.nPriority == y.nPriority) return 0; else return (x.nPriority > y.nPriority) ? -1 : 1; });
            if (LastBroadKill.Count != 0)
            {

                GameObject nowBaseIns = GetBroadcastIns(LastBroadKill[0]);
                if (nowBaseIns == null)
                    return;

                nowBaseIns.SetActive(true);
                SetBroadcastIns(LastBroadKill[0]);

                CaptureMoment(LastBroadKill[0]);

                Invoke("BroadcastComplete", 3.0f);
                //Tweener nowTweener = nowBaseIns.GetComponent<CanvasGroup>().DOFade(0, 2.5f).SetDelay(0.5f);
                //nowTweener.OnComplete(() => BroadcastComplete(LastBroadKill[0]));
            }
        }


        private void SetKillCountData(BroadcastMsgData data)
        {
            bool bSameCamp = data.isSameCamp > 0;
            // 设置背景头像
            string strKillDesc1 = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "KillNum_2");
            string strKillDesc2 = null;
            if (data.nTimeKillCount > 1 && data.nTimeKillCount <= 5)
            {
                // 设置背景
                TimeKillIcon.gameObject.SetActive(false);
                UEffectPrefabType param = UEffectPrefabType.UEPT_None;
                switch (data.nTimeKillCount)
                {
                    case 2:
                        {
                            if (bSameCamp)
                                param = UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_2Kill_B;
                            else
                                param = UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_2Kill_R;
                            strKillDesc2 = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "DoubleKill");
                        }
                        break;
                    case 3:
                        {
                            if (bSameCamp)
                                param = UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_3Kill_B;
                            else
                                param = UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_3Kill_R;
                            strKillDesc2 = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "TribleKill");
                        }
                        break;
                    case 4:
                        {
                            if (bSameCamp)
                                param = UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_4Kill_B;
                            else
                                param = UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_4Kill_R;
                            strKillDesc2 = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "QuadraKill");
                        }
                        break;
                    case 5:
                        {
                            if (bSameCamp)
                                param = UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_5Kill_B;
                            else
                                param = UEffectPrefabType.UEPT_WarBroadcastMgrView_TimeKillFrame_5Kill_R;
                            strKillDesc2 = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "PentaKill");
                        }
                        break;
                    default:
                        break;
                }
                if (m_effectParam != null)
                {
                    UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref m_effectParam);
                    m_effectParam = null;
                }
                m_effectParam = new UEffectPrefabParam(_eType: param, _tfAttachParent: TimeKillFrame.transform, _bAutoDestroy: false);
                UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref m_effectParam);

                // 设置头像
                TimeKillerHeadPortrait.sprite = GetKillerHeadPortrait(data);
            }

            // 设置聊天广播
            strColorMur = bSameCamp ? blueColor : redColor;
            strColorMas = bSameCamp ? redColor : blueColor;
            string str1 = data.MurdererName + "(" + data.MurdererVocation + ")";
            string str2 = strKillDesc1;
            string str3 = data.MasterName + "(" + data.MasterVocation + ")";
            string str4 = strKillDesc2;
            string str = String.Format("{0}[text color={1}]{2}[/text]{3}{4}[text color={5}]{6}[/text]{7}", ChatMessageManager.ChatItemFlag, strColorMur, str1, str2, ChatMessageManager.ChatItemFlag, strColorMas, str3, str4);
            ShowChatMessage(str);

            // 设置音效
            const int nENMYoffset = 6;
            if (data.nTimeKillCount > 1 && data.nTimeKillCount <= 5)
            {
                int nSoundID = bSameCamp? data.nTimeKillCount : (data.nTimeKillCount + nENMYoffset);
                SoundManager.CreateUISound(nSoundID);
            }
        }

        private void SetImportantEVData(BroadcastMsgData data)
        {
            bool bSameCamp = data.isSameCamp > 0;

            strColorMur = bSameCamp ? blueColor : redColor;
            strColorMas = bSameCamp ? redColor : blueColor;
            string str1, str2, str3, str4, str5, str;

            if (data.nIsFirstBlood > 0)
            {
                if (bSameCamp)
                    FirstBloodIcon.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_FirstBlood_Self);
                else
                    FirstBloodIcon.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_FirstBlood_Enmy);
                FirstBloodHeadPortrait.sprite = GetKillerHeadPortrait(data);
                // 设置聊天广播
                str1 = data.MurdererName + "(" + data.MurdererVocation + ")";
                str2 = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "FirstBload");
                str3 = data.nAddTalentPoint.ToString();
                str = String.Format("{0}[text color={1}]{2}[/text]{3}{4}[text color={5}]{6}[/text]", ChatMessageManager.ChatItemFlag, strColorMur, str1, str2, ChatMessageManager.ChatItemFlag, goldColor, str3);
                ShowChatMessage(str);
                SoundManager.CreateUISound((int)WarKillSoundType.WKST_FIRSTBLOOD);
                return;
            }

            // 设置背景图片、头像图片、文字
            int nSoundID = 0;
            string strKillInfo1 = null;
            string strKillInfo2 = null;
            if (data.nIsOver != 0)
            {
                // 设置聊天广播
                strKillInfo1 = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "OveredPre");
                strKillInfo2 = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "OveredBack");
                str1 = data.MurdererName + "(" + data.MurdererVocation + ")";
                str2 = strKillInfo1;
                str3 = data.MasterName + "(" + data.MasterVocation + ")";
                str4 = strKillInfo2;
                str5 = data.nAddTalentPoint.ToString();
                str = String.Format("{0}[text color={1}]{2}[/text]{3}{4}[text color={5}]{6}[/text]{7}{8}[text color={9}]{10}[/text]",
                    ChatMessageManager.ChatItemFlag, strColorMur, str1, str2, ChatMessageManager.ChatItemFlag, strColorMas, str3, str4, ChatMessageManager.ChatItemFlag, goldColor, str5);

                NormalCombatInfo1.text = data.MurdererName;
                NormalCombatInfo2.text = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "IsKilled");
                NormalCombatInfo3.text = data.MasterName;

                SetNormalFrame(data);
            }
            else
            {
                if (data.nTimeKillCount >= 0 && data.nTimeKillCount < 3)
                {
                    // 设置聊天广播
                    strKillInfo1 = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "KillNum_2");

                    str1 = string.IsNullOrEmpty(data.MurdererVocation) ? data.MurdererName : data.MurdererName + "(" + data.MurdererVocation + ")";
                    //str1 = data.MurdererName + "(" + data.MurdererVocation + ")";
                    str2 = strKillInfo1;
                    //str3 = data.MasterName + "(" + data.MasterVocation + ")";
                    str3 = string.IsNullOrEmpty(data.MasterVocation) ? data.MasterName : data.MasterName + "(" + data.MasterVocation + ")";
                    str = String.Format("{0}[text color={1}]{2}[/text]{3}{4}[text color={5}]{6}[/text]",
                        ChatMessageManager.ChatItemFlag, strColorMur, str1, str2, ChatMessageManager.ChatItemFlag, strColorMas, str3);

                    if (data.nMasterEntityType == 2 && data.nAddTalentPoint > 0)
                    {
                        str1 = string.IsNullOrEmpty(data.MurdererVocation) ? data.MurdererName : data.MurdererName + "(" + data.MurdererVocation + ")";
                        //str1 = data.MurdererName + "(" + data.MurdererVocation + ")";
                        str2 = strKillInfo1;
                        //str3 = data.MasterName + "(" + data.MasterVocation + ")";
                        str3 = string.IsNullOrEmpty(data.MasterVocation) ? data.MasterName : data.MasterName + "(" + data.MasterVocation + ")";
                        str4 = "(" + ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "TeamBounty");
                        str5 = data.nAddTalentPoint.ToString();
                        str = String.Format("{0}[text color={1}]{2}[/text]{3}{4}[text color={5}]{6}[/text]{7}{8}[text color={9}]{10}[/text]{11}",
                            ChatMessageManager.ChatItemFlag, strColorMur, str1, str2, ChatMessageManager.ChatItemFlag, strColorMas, str3, str4, ChatMessageManager.ChatItemFlag, goldColor, str5, ")");
                    }

                    NormalCombatInfo1.text = data.MurdererName;
                    NormalCombatInfo2.text = strKillInfo1;
                    NormalCombatInfo3.text = data.MasterName;

                    SetNormalFrame(data);
                }
                else if (data.nTimeKillCount >= 3 && data.nTimeKillCount < 8)
                {
                    strKillInfo1 = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "IsAlready");
                    strKillInfo2 = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "KillNum_" + data.nTimeKillCount);
                    str1 = data.MurdererName + "(" + data.MurdererVocation + ")";
                    str2 = strKillInfo1 + strKillInfo2;
                    str = String.Format("{0}[text color={1}]{2}[/text]{3}",
                        ChatMessageManager.ChatItemFlag, strColorMur, str1, str2);

                    MoreKillHeadPortrait.sprite = GetKillerHeadPortrait(data);
                    MoreKillCombatInfo1.text = data.MurdererName;
                    MoreKillCombatInfo2.text = strKillInfo1;
                    MoreKillCombatInfo3.text = strKillInfo2;

                    if (bSameCamp)
                    {
                        SetMoreKillColor("#00fcff", "");
                        nSoundID = (int)WarKillSoundType.WKST_SELF3Kill_NoDie + data.nTimeKillCount - 3;
                        MoreKillIcon.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TimeKill_Self);
                    }
                    else
                    {
                        SetMoreKillColor("#ff302e", "");
                        nSoundID = (int)WarKillSoundType.WKST_ENMY3Kill_NoDie + data.nTimeKillCount - 3;
                        MoreKillIcon.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TimeKill_Enmy);
                    } 
                }
                else
                {
                    strKillInfo1 = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "IsAlready");
                    strKillInfo2 = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "KillNum_8");
                    str1 = data.MurdererName + "(" + data.MurdererVocation + ")";
                    str2 = strKillInfo1 + strKillInfo2;
                    str = String.Format("{0}[text color={1}]{2}[/text]{3}",
                        ChatMessageManager.ChatItemFlag, strColorMur, str1, str2);

                    GodHeadPortrait.sprite = GetKillerHeadPortrait(data);
                    GodCombatInfo1.text = data.MurdererName;
                    GodCombatInfo2.text = strKillInfo1;

                    Color clr = new Color();
                    if (bSameCamp)
                    {
                        ColorUtility.TryParseHtmlString("#00b2ff", out clr);
                        nSoundID = (int)WarKillSoundType.WKST_SELF8Kill_NoDie;
                        GodHeadIcon.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_GodKill_Self);
                        GodKillDes.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_GodKillDes_Self);
                    }
                    else
                    {
                        ColorUtility.TryParseHtmlString("#e92632", out clr);
                        nSoundID = (int)WarKillSoundType.WKST_ENMY8Kill_NoDie;
                        GodHeadIcon.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_GodKill_Enmy);
                        GodKillDes.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_GodKillDes_Enmy);
                    }
                    GodCombatInfo2.color = clr;
                }
            }

            // 设置聊天广播
            ShowChatMessage(str);

            // 设置音效
            if (nSoundID != 0)
            {
                SoundManager.CreateUISound(nSoundID);
            }
        }

        private void SetAceKillData(BroadcastMsgData data)
        {
            // 设置头像
            AceKillerHeadPortrait.sprite = GetKillerHeadPortrait(data);

            // 设置聊天文字广播
            string strDesc, strColor;
            if (data.isSameCamp > 0)
            {
                AceKillIcon.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_Ace_Self);
                strDesc = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "AceEnemy");
                strColor = blueColor;
            }
            else
            {
                AceKillIcon.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_Ace_Enmy);
                strDesc = ULocalizationService.Instance.Get("UIView", "BroadcastMessageMgr", "AceSelf");
                strColor = redColor;
            }
            string str = String.Format("{0}[text color={1}]{2}[/text]", ChatMessageManager.ChatItemFlag, strColor, strDesc);
            ShowChatMessage(str);

            // 团灭广播
            SoundManager.CreateUISound((int)WarKillSoundType.WKST_ACE);
        }

        private void SetMonsterReliveData(BroadcastMsgData data)
        {
            // 设置音效
            SoundManager.CreateUISound(data.nSoundID);

            // 设置文字
            MonsterReliveInfo.text = data.szDes;

            // 设置聊天广播
            ShowChatMessage(data.szDes);
        }

        private void SetMonsterDieData(BroadcastMsgData data)
        {
            // 设置音效
            SoundManager.CreateUISound(data.nSoundID);

            // 设置文字
            MonsterDieInfo.text = data.szDes;

            // 设置聊天广播
            ShowChatMessage(data.szDes);
        }

        private void SetSpecialContributionData1(BroadcastMsgData msgData)
        {
            int nTypeIndex = -1;
            if (msgData.nReserve == (int)ESpecialContributionType.ESCT_THRILLSAVE)
            {
                nTypeIndex = (int)EDynamicStateMoba.EDSM_BroadcastMessage_ThrillSave;
            }
            else if (msgData.nReserve == (int)ESpecialContributionType.ESCT_DEADLYCONTROL)
            {
                nTypeIndex = (int)EDynamicStateMoba.EDSM_BroadcastMessage_DeadlyControl;
            }

            if (nTypeIndex == -1)
                return;

            if (msgData.bChatShow)
                ShowChatMessage(msgData.MasterName + msgData.szDes);

            SoundManager.CreateUISound(msgData.nSoundID);

            this.Contribution1WordImage.sprite = GetSpriteDynamicState(nTypeIndex);
        }

        private void SetSpecialContributionData2(BroadcastMsgData msgData)
        {
            int nTypeIndex = -1;
            string[] colorList = new string[3] { "2292B6FF", "8A2266FF", "9C882CFF" };
            string szColor = "";
            if (msgData.nReserve == 3)
            {
                nTypeIndex = (int)EDynamicStateMoba.EDSM_BroadcastMessage_TeamControlThree;
                szColor = colorList[0];
            }
            else if (msgData.nReserve == 4)
            {
                nTypeIndex = (int)EDynamicStateMoba.EDSM_BroadcastMessage_TeamControlFour;
                szColor = colorList[1];
            }
            else if (msgData.nReserve == 5)
            {
                nTypeIndex = (int)EDynamicStateMoba.EDSM_BroadcastMessage_TeamControlFive;
                szColor = colorList[2];
            }

            if (nTypeIndex == -1||string.IsNullOrEmpty(szColor))
                return;

            if (msgData.bChatShow)
                ShowChatMessage(msgData.MasterName + msgData.szDes);

            SoundManager.CreateUISound(msgData.nSoundID);

            Contribution2HeadImage.sprite = GetDeathHeadPortrait(msgData);
            this.Contribution2FrameImage.sprite = GetSpriteDynamicState(nTypeIndex);
            this.Contribution2FrameImage.SetNativeSize();
            //Contribution2WordText.text = "团控" + msgData.nReserve + "人";
            //Contribution2WordText.GetComponent<Outline>().effectColor = ColorExtension.FromHex(szColor);
        }

        private void SetAliveAssistData1(BroadcastMsgData msgData)
        {
            Assist1HeadIcon.gameObject.SetActive(msgData.nMasterHeroID > 0);
            if (msgData.nMasterHeroID <= 0)
            {
                return;
            }

            if (msgData.bChatShow)
                ShowChatMessage(msgData.MasterName + msgData.szDes);

            SoundManager.CreateUISound(msgData.nSoundID);
            this.Assist1NameText.text = msgData.MasterName;
            Assist1NameText.gameObject.SetActive(!string.IsNullOrEmpty(msgData.MasterName));

            Assist1HeadIcon.sprite = GetDeathHeadPortrait(msgData);
        }

        private void SetAliveAssistData2(BroadcastMsgData msgData)
        {
            if (msgData.nReserve != 5 && msgData.nReserve != 10)
                return;

            if (msgData.bChatShow)
                ShowChatMessage(msgData.MasterName + msgData.szDes);

            SoundManager.CreateUISound(msgData.nSoundID);

            EDynamicStateMoba eTypeIndex = msgData.nReserve == 5 ? EDynamicStateMoba.EDSM_BroadcastMessage_FiveAssist : EDynamicStateMoba.EDSM_BroadcastMessage_TenAssist;
            this.Assist2NumSprite.sprite = GetSpriteDynamicState((int)eTypeIndex);
        }


        public Sprite GetSpriteDynamicState(int nIndex)
        {
            return USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_DynamicState_Moba, WndID.WND_ID_WAR_BROADCASTMESSAGEMGR, nIndex);
        }

        public Sprite GetKillerHeadPortrait(BroadcastMsgData data)
        {
            Sprite sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_BROADCASTMESSAGEMGR, 1, 4, data.nMurdererHeroID);
            if (data.nMurdererEntityType == 2)
            {
                switch (data.nMurdererHeroID)
                {
                    case 1:
                        {
                            sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_Soldier);
                        }
                        break;
                    case 2:
                        {
                            if (data.isSameCamp > 0)
                                sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TowelOur);
                            else
                                sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TowelEnemy);
                        }
                        break;
                    case 3:
                        {
                            sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_LargeDragon);
                        }
                        break;
                    case 4:
                        {
                            sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_SmallDragon);
                        }
                        break;
                    //case 5:
                    //    {
                    //        sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_Monster);
                    //    }
                    //    break;
                    default:
                        {
                            sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MonsterHeadPortrait, WndID.WND_ID_WAR_BROADCASTMESSAGEMGR, data.nMurdererHeroID, 2);
                        }
                        break;
                }
            }
            return sprite;
        }

        public Sprite GetDeathHeadPortrait(BroadcastMsgData data)
        {
            Sprite sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_HeadPortrait, WndID.WND_ID_WAR_BROADCASTMESSAGEMGR, 1, 4, data.nMasterHeroID);
            if (data.nMasterEntityType == 2)
            {
                switch (data.nMasterHeroID)
                {
                    case 1:
                        {
                            sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_Soldier);
                        }
                        break;
                    case 2:
                        {
                            if (data.isSameCamp > 0)
                                sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TowelOur);
                            else
                                sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_TowelEnemy);
                        }
                        break;
                    case 3:
                        {
                            sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_LargeDragon);
                        }
                        break;
                    case 4:
                        {
                            sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_SmallDragon);
                        }
                        break;
                    //case 5:
                    //    {
                    //        sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_Monster);
                    //    }
                    //    break;
                    default:
                        {
                            sprite = USpriteManager.Instance.GetSprite(USpriteManager.ESpriteType.EST_MonsterHeadPortrait, WndID.WND_ID_WAR_BROADCASTMESSAGEMGR, data.nMasterHeroID, 2);
                        }
                        break;
                }
            }
            return sprite;
        }

        private void ShowChatMessage(string strShowMsg)
        {
            if (strShowMsg == null)
                return;
                
            gamelogic_show_chat_message msgData = new gamelogic_show_chat_message();
            msgData.channel = (int)EMChatChannelType.CHAT_CHANNEL_WARSCENE;
            msgData.message = strShowMsg;
            LogicDataCenter.chatMessageManager.AddChatMessage(ref msgData);
        }

        private void SetMoreKillColor(string strColor, string strGradient)
        {
            Color clr = new Color();
            ColorUtility.TryParseHtmlString(strColor, out clr);
            MoreKillCombatInfo2.color = clr;
            MoreKillCombatInfo3.color = clr;
            
            //TODO 更新为建聪自己写的组件
            //ColorUtility.TryParseHtmlString(strGradient, out clr);
            //MoreKillInfo2.SetGradientColorKey(clr,Color.black);
            //MoreKillInfo3.SetGradientColorKey(clr, Color.black);
            //MoreKillInfo2.vertex1 = clr;
            //MoreKillInfo3.vertex1 = clr;
        }

        private void SetNormalFrame(BroadcastMsgData data)
        {
            NormalKillerHeadPortrait.sprite = GetKillerHeadPortrait(data);
            NormalDeathHeadPortrait.sprite = GetDeathHeadPortrait(data);
            Color clr1, clr2;
            ColorUtility.TryParseHtmlString("#ffffff", out clr1);//白
            ColorUtility.TryParseHtmlString("#e92632", out clr2);//红
            if (data.isSameCamp > 0)
            {
                NormalKillerIcon.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_KillHead_Self);
                NormalDeathIcon.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_KillHead_Enmy);
                NormalCombatInfo1.color = clr1;
                NormalCombatInfo3.color = clr2;
                NormalKillIcon.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_KillBottom_Self);
            }
            else
            {
                NormalKillerIcon.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_KillHead_Enmy);
                NormalDeathIcon.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_KillHead_Self);
                NormalCombatInfo1.color = clr2;
                NormalCombatInfo3.color = clr1;
                NormalKillIcon.sprite = GetSpriteDynamicState((int)EDynamicStateMoba.EDSM_BroadcastMessage_KillBottom_Enmy);
            }
        }


        public void AddBroadCastTipItem(BroadcastMsgData msgData)
        {
            // List加入msgData  if count < 3  实例化 显示
            m_broadcastTipData.Add(msgData);
            if (m_broadcastTipData.Count <= BroadcastTipItemViewCount)
            {
                CreateBroadcastTipObj(msgData);
            }  
        }

        private void OnBroadCastTipItemComplete(GameObject tipObj)
        {
            // list去掉第一项 Frame ResNode.DestroyRes 第一项 if count >= 3 实例化 第三项
            m_broadcastTipData.RemoveAt(0);
            ResNode.DestroyRes(ref tipObj);

            if (m_broadcastTipData.Count >= BroadcastTipItemViewCount)
            {
                CreateBroadcastTipObj(m_broadcastTipData[2]);
            }
        }

        private void CreateBroadcastTipObj(BroadcastMsgData data)
        {
            GameObject tipObj = ResNode.InstantiateRes(BroadcatTipItemTemplate);
            if (!tipObj)
                return;
            RectTransform tipRect = tipObj.transform as RectTransform;
            if (!tipRect)
                return;
            tipRect.SetParent(BroadcastTipFrame, false);
            BroadcastTipItem tipItem = tipObj.GetComponent<BroadcastTipItem>();
            if (!tipItem)
                return;
            tipItem.SetData(this, data);

            tipObj.SetActive(true);
            Tweener baseTweener = tipObj.GetComponent<CanvasGroup>().DOFade(0, 1).SetDelay(4);
            baseTweener.OnComplete(() => OnBroadCastTipItemComplete(tipObj));
        }

        private void CaptureMoment(BroadcastMsgData msgData)
        {
            if (!msgData.bMainHeroOperate)
                return;

            if(msgData.nType == (int)BroadcastType.EMBT_KILLCOUNT)
            {
                if (msgData.nTimeKillCount > 0 && msgData.nTimeKillCount < 6)
                {
                    //三杀及以上截图
                    if(msgData.nTimeKillCount > 2)
                        CaptureScreen.GetInstance().CaptureScreenImage(new SScreenShotImgInfo(msgData.nMurdererHeroID, (EMScreenShotType)msgData.nTimeKillCount));

                    ScreenREC.GetInstance().SignValid(msgData.nTimeKillCount, msgData.nMurdererHeroID);
                }
                    
            }

            if(msgData.nType == (int)BroadcastType.EMBT_IMPORTANTEV)
            {
                if (GodFrame && GodFrame.activeInHierarchy && msgData.nTimeKillCount < 9)
                {
                    CaptureScreen.GetInstance().CaptureScreenImage(new SScreenShotImgInfo(msgData.nMurdererHeroID, EMScreenShotType.EMSS_God));
                    //ScreenREC.GetInstance().SignValid(EMScreenShotType.EMSS_God, msgData.nMurdererHeroID);
                }
                ScreenREC.GetInstance().SignValid(1, msgData.nMurdererHeroID);    
            }
        }

    }
}
