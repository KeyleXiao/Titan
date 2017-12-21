/*******************************************************************
** 文件名:	UMediatorGameWaitingRoom.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-6-25
** 版  本:	1.0
** 描  述:	选人大厅
		
********************************************************************/
using System.Collections.Generic;
using GameLogic;
using System;
using System.Runtime.InteropServices;
using DataCenter;
using USpeedUI.TooltipAndDialog;
using UnityEngine;
using System.Collections;
using U3D_Render;
using Stage_ManagerDef;
using ASpeedGame.Data.SelectArrayTipPage;
using U3D_Render.Common;
using ASpeedGame.Data.HeroXPskillConfig;
using Match_ManagerDef;
using ASpeedGame.Data.HeroTypeConfig;
namespace USpeedUI.GameWaitingRoomPart
{
    public enum SelectPhase
    {
        SELECTPHASE_DISABLE = 1,
        SELECTPHASE_SELECT,
        SELECTPHASE_WAIT_DISABLEHERO,
    };

    public class GameRoomSkinSelectCache
    {
        public int nHeroID = 0;
        public int nSkinID = 0;

        public void Clear()
        {
            nHeroID = 0;
            nSkinID = 0;
        }
    }

    public class GameWaitingRoomWnd : UIFillWnd<GameWaitingRoomView>
    {
        private EMatchRoomState m_CurRoomState = EMatchRoomState.MatchRoomState_Wait;
        private EMHallRoomType m_RoomType = EMHallRoomType.HallRoomType_Unknown;
        private EMMatchType m_MatchType = EMMatchType.MatchType_None;
        private bool m_bLocalRoomOwner = false;
        private int m_nCampPlayerCount = 0;
        private int m_LocalCamp = 0;
        private Dictionary<int, int> dicCampTeamHeroTip = new Dictionary<int, int>();
        private bool m_bDelayJoin = false;
        private int  m_nDelayJoinTime = 0;
        private GameRoomSkinSelectCache m_RoomSkinCache = new GameRoomSkinSelectCache() ;
        private bool m_bOnlineLegendCup = false;

        public int LocalCamp {set {m_LocalCamp = value;} get {return m_LocalCamp;}}
        public GameRoomSkinSelectCache SkinSelectCache { get { return m_RoomSkinCache; } }
        public EMatchRoomState RoomState { get { return m_CurRoomState; } }
        public int CampPlayerCount { get { return m_nCampPlayerCount; } }
        public bool LocalRoomOwner { get { return m_bLocalRoomOwner; } }

        // 获取ID
        public override WndID GetID()
        {
            return WndID.WND_ID_GAMEWAITINGROOMVIEW;
        }

        // 获取资源路径
        public override String GetPrefabPath()
        {
            return "GameWaitingRoomView/GameWaitingRoomView";
        }

        // 获取所在层次
        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FILL;
        }

        public override WndMutexLevelDef GetMutexLevel()
        {
            return WndMutexLevelDef.WND_Mutex_Highest;
        }

        // 初始化
        public override bool Init()
        {
            base.Init();

            m_isKeyClose = true;
            m_bHaveEscCallBack = true;
            // 订阅聊天消息更新消息
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_MYSELFENTERROOM, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSELECTHEROINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSELECTSUMMERINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMALLSUMMONERINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTATECHANGE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTARTRULE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULEBANHERO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULEBANHEROLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMGLOBLEBANHEROLIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULESELECTHERO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULECANSELECTHERO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);


            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_PRE_SELECT_LIST, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEHERO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMROMOVETIMECOUNT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMKICKPLAYER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMPLAYERCONFIRM, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMALLPLAYERCONFIRM, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTARTSELECTHERO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSUMMERTEAMALL, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ENTERROOM, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERRLOCK, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERCHANGE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERRULEBAN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOM_READY_TO_FIGHT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROLESLOT, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSUMMERTEAMSINGLE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMTALENTINROOMCLOSE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_OTHERENTERROOM, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_REMOVEAllROOMINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROOMOWNER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOHALL, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMHEROSKILLINFO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEGENDCUPWAITCOUNTDOWN, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SHOW, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMBUYHERO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_HEROSKINVIEW_UPDATESKINDATA, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOSCENE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_BAN_HERO_COUNTDOWN, this);

            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SINGLE_BAN_HERO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ALL_BAN_HERO, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_BAN_HERO_FLAG, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_MYSELFENTERROOMOB, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_OTHERENTERROOMOB, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROLESLOTOB, this);

            return true;
        }

        // 销毁
        public override void Destroy()
        {
            base.Destroy();

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_MYSELFENTERROOM, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSELECTHEROINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSELECTSUMMERINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMALLSUMMONERINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTATECHANGE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTARTRULE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULEBANHERO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULEBANHEROLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMGLOBLEBANHEROLIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULESELECTHERO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULECANSELECTHERO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEHERO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMROMOVETIMECOUNT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMKICKPLAYER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMPLAYERCONFIRM, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMALLPLAYERCONFIRM, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTARTSELECTHERO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSUMMERTEAMALL, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ENTERROOM, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERRLOCK, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERCHANGE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERRULEBAN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOM_READY_TO_FIGHT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROLESLOT, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSUMMERTEAMSINGLE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMTALENTINROOMCLOSE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_OTHERENTERROOM, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_HALLROOMINFOVIEW_REMOVEAllROOMINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROOMOWNER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOHALL, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMHEROSKILLINFO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEGENDCUPWAITCOUNTDOWN, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SHOW, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMBUYHERO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_HEROSKINVIEW_UPDATESKINDATA, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOSCENE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_PRE_SELECT_LIST, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_BAN_HERO_COUNTDOWN, this);

            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SINGLE_BAN_HERO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ALL_BAN_HERO, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_BAN_HERO_FLAG, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_MYSELFENTERROOMOB, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_OTHERENTERROOMOB, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROLESLOTOB, this);
            
        }

        // 接受消息
        public override void OnMessage(WndMsgID msgID, UIMsgData msgData)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        if (m_wndView == null)
                        {
                            this.LoadView();
                        }

                    }
                    break;
                case WndMsgID.WND_MSG_COMMON_LOGINMOBA:
                    {
                        if (m_wndView == null)
                            return;

                        CloseComboListView();
                        CloseSkinSound();
                        this.UnloadView();
                        SceneManager.Instance.UnLoadSelectHeroScene();
                        dicCampTeamHeroTip.Clear();                       
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_MYSELFENTERROOM:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiSelfRoom = (UIMsgCmdData)msgData;
                        cmd_match_room_role_data data = IntPtrHelper.toData<cmd_match_room_role_data>(uiSelfRoom.ptrParam);
                        MySelfEnterRoom(data);

                        Initialize.Instance.FlashWindow();
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_OTHERENTERROOM:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiOtherRoom = (UIMsgCmdData)msgData;
                        cmd_match_room_role_data data = IntPtrHelper.toData<cmd_match_room_role_data>(uiOtherRoom.ptrParam);
                        OtherRoleEnterRoom(data);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_MYSELFENTERROOMOB:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiSelfRoom = (UIMsgCmdData)msgData;
                        cmd_match_room_ob_data data = IntPtrHelper.toData<cmd_match_room_ob_data>(uiSelfRoom.ptrParam);
                        MySelfObEnterRoom(data);

                        Initialize.Instance.FlashWindow();
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_OTHERENTERROOMOB:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiOtherRoom = (UIMsgCmdData)msgData;
                        cmd_match_room_ob_data data = IntPtrHelper.toData<cmd_match_room_ob_data>(uiOtherRoom.ptrParam);
                        OtherObEnterRoom(data);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSELECTHEROINFO:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoomHero = (UIMsgCmdData)msgData;
                        InitSelectHeroFrameInfo(uiRoomHero.ptrParam, uiRoomHero.nPtrLen);

                        // 匹配吃鸡，直接调到loading界面
                        if (m_MatchType == EMMatchType.MatchType_MultiCampMatch && m_RoomType == EMHallRoomType.HallRoomType_Macth)
                        {
                            UISystem.Instance.ShowWnd(WndID.WND_ID_LOADINGWARVIEW, true);
                        }

                        if (m_bDelayJoin)
                            m_wndView.SetDelayJoin(m_nDelayJoinTime);
                        else
                            SetVisible(true);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSELECTSUMMERINFO:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiSummerInfo = (UIMsgCmdData)msgData;
                        SetInitSelectSummonerSkill(uiSummerInfo.ptrParam, uiSummerInfo.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTATECHANGE:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoomState = (UIMsgCmdData)msgData;
                        SetMatchRoomState((EMatchRoomState)uiRoomState.nParam);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTARTRULE:
                    {
                        if (m_wndView == null)
                            return;

                        //不能选择英雄
                        m_wndView.SetSelectHeroListLock(true);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULEBANHERO:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoomRule = (UIMsgCmdData)msgData;
                        MatchRoomRuleBanHero(uiRoomRule.ptrParam, uiRoomRule.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULEBANHEROLIST:
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMGLOBLEBANHEROLIST:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoomRule = (UIMsgCmdData)msgData;
                        m_wndView.SetBanHeroData(uiRoomRule.ptrParam, uiRoomRule.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULECANSELECTHERO:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoomRule = (UIMsgCmdData)msgData;
                        cmd_match_room_hero_can_select data = IntPtrHelper.toData<cmd_match_room_hero_can_select>(uiRoomRule.ptrParam);
                        MatchRoomRuleSetHeroLock(data);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMALLSUMMONERINFO:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoomRule = (UIMsgCmdData)msgData;
                        SetAllSummonerSkill(uiRoomRule.ptrParam, uiRoomRule.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMRULESELECTHERO:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoomRule = (UIMsgCmdData)msgData;
                        MatchRoomRuleSelectHero(uiRoomRule.ptrParam, uiRoomRule.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEHERO:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoomRule = (UIMsgCmdData)msgData;
                        cmd_match_room_hero_change data = IntPtrHelper.toData<cmd_match_room_hero_change>(uiRoomRule.ptrParam);
                        SetChangeTeamItemHeroIcon(data);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_PRE_SELECT_LIST:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoomRule = (UIMsgCmdData)msgData;
                        IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(uiRoomRule.ptrParam, uiRoomRule.nPtrLen);
                        cmd_pre_select_head data = helper.get<cmd_pre_select_head>();
                        for (int n = 0; n < data.nCount; ++n)
                        {
                            cmd_match_room_hero_change preHeroData = helper.get<cmd_match_room_hero_change>();
                            SetChangeTeamItemHeroIcon(preHeroData);
                        }
                    }
                    break;
                    
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMPLAYERCONFIRM:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoomConfirm = (UIMsgCmdData)msgData;
                        cmd_match_room_confirm_hero data = IntPtrHelper.toData<cmd_match_room_confirm_hero>(uiRoomConfirm.ptrParam);
                        m_wndView.SetTeamItemHeroConfirm(data.nCamp, data.nPlayerSlotID);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMALLPLAYERCONFIRM:
                    {
                        if (m_wndView == null)
                            return;
                        Initialize.Instance.FlashWindow();
                        SoundPlayHandler.StopUISceneSound(false);
                        SoundPlayHandler.ClearData();
                        SoundManager.CreateUISound((int)SoundIDIndex.GameWaitingRoomReadyTimer);
                        UIMsgCmdData uiRoomAllConfirm = (UIMsgCmdData)msgData;
                        m_wndView.SetShowEnterWarTime(uiRoomAllConfirm.nParam);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSTARTSELECTHERO:
                    {
                        if (m_wndView == null)
                            return;
                        Initialize.Instance.FlashWindow();

                        UIMsgCmdData uiRoomSelectHero = (UIMsgCmdData)msgData;
                        m_wndView.SetShowSelectHeroTime(uiRoomSelectHero.nParam);
                        m_wndView.SetStartButtonToWaitForSelectState();
                        if (m_wndView.IsVisible())
                        {
                            SoundPlayHandler.StopUISceneSound(false);
                            SoundPlayHandler.PlayUISceneSound((int)SoundIDIndex.GameWaitingRoomHeroSelect);
                        }
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSUMMERTEAMALL:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoom = (UIMsgCmdData)msgData;
                        SetAllTeamItemSummerInfo(uiRoom.ptrParam, uiRoom.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ENTERROOM:
                    {
                        if (m_wndView == null)
                            return;
                      
                        UIMsgCmdData uiRoomConfirm = (UIMsgCmdData)msgData;
                        cmd_match_hall_add_room data = IntPtrHelper.toData<cmd_match_hall_add_room>(uiRoomConfirm.ptrParam);
                        CreateGameModeLayout(data);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERCHANGE:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoom = (UIMsgCmdData)msgData;
                        cmd_match_room_computer_hero_change data = IntPtrHelper.toData<cmd_match_room_computer_hero_change>(uiRoom.ptrParam);
                        m_wndView.SetComputerHeadIconChange(data);
                        UpdateTeamSelectHeroTip(data.nCamp, data.nPlayerSlotID, data.nHeroID);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERRLOCK:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoom = (UIMsgCmdData)msgData;
                        cmd_match_room_computer_hero_can_select data = IntPtrHelper.toData<cmd_match_room_computer_hero_can_select>(uiRoom.ptrParam);
                        bool bSelect = data.bCanSelect <= 0 ? false : true;
                        m_wndView.SetCampComputerCanUse(data.nCamp, bSelect, data.nHeroID);
                        SetCampSelectHeroCanUseByComputer(data.nCamp, bSelect, data.nHeroID);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCPMPUTERRULEBAN:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoom = (UIMsgCmdData)msgData;

                        MatchRoomRuleBanComputerList(uiRoom.ptrParam, uiRoom.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMKICKPLAYER:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoom = (UIMsgCmdData)msgData;
                        cmd_match_room_hero_exit data = IntPtrHelper.toData<cmd_match_room_hero_exit>(uiRoom.ptrParam);
                        if (data.bIsOb > 0)
                        {
                            m_wndView.OnMsgDeleteOb(data.nPos);
                        }
                        else
                        {
                            int nCamp = data.nPos / m_nCampPlayerCount;
                            int nSlot = data.nPos % m_nCampPlayerCount;
                            m_wndView.SetTeamItemClear(nCamp, nSlot);
                            UpdateTeamSelectHeroTip(nCamp, nSlot, 0);
                        }
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROOMOWNER:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoom = (UIMsgCmdData)msgData;
                        cmd_match_room_master_change data = IntPtrHelper.toData<cmd_match_room_master_change>(uiRoom.ptrParam);
                        SetRoomMasterChange(data);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROLESLOT:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoom = (UIMsgCmdData)msgData;
                        cmd_match_room_hero_pos_change data = IntPtrHelper.toData<cmd_match_room_hero_pos_change>(uiRoom.ptrParam);
                        SetRoomTeamItemChangePosition(uiRoom.nParam, data);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMCHANGEROLESLOTOB:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoom = (UIMsgCmdData)msgData;
                        cmd_match_room_ob_pos_change_oc data = IntPtrHelper.toData<cmd_match_room_ob_pos_change_oc>(uiRoom.ptrParam);
                        SetRoomObChangePosition(uiRoom.nParam, data);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOM_READY_TO_FIGHT:
                    {
                        if (m_wndView == null)
                            return;

                        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_READY_TO_FIGHT);
                        m_wndView.SetHideTime();
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMSUMMERTEAMSINGLE:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoom = (UIMsgCmdData)msgData;
                        cmd_Summoner_Skill_Change data = IntPtrHelper.toData<cmd_Summoner_Skill_Change>(uiRoom.ptrParam);
                        data.nPos = data.nPos >= m_nCampPlayerCount ? data.nPos - m_nCampPlayerCount : data.nPos;
                        m_wndView.SetTeamItemSummerInfo(data.nCamp, data.nPos, data.nSummonerSkill[0], data.nSummonerSkill[1]);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMTALENTINROOMCLOSE:
                    {
                        if (m_wndView == null)
                            return;

                        m_wndView.UpdateTalentComBoxData();
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOHALL:
                    {
                        if (m_wndView == null)
                            return;

                        if(m_wndView.IsVisible())
                            CloseByOtherPlayer();
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMHEROSKILLINFO:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoom = (UIMsgCmdData)msgData;
                        m_wndView.SetHeroSkillInfo(uiRoom.ptrParam, uiRoom.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SHOW:
                    {
                        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_WAR_MAIN_CLOSE, null);

                        if (m_wndView == null)
                            return;
                        
                        SetVisible(true);
                        m_bDelayJoin = false;
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMBUYHERO:
                    {
                        if (m_wndView == null || !IsVisible() || msgData == null)
                            break;

                        PlayerHeroUpdateData data = msgData as PlayerHeroUpdateData;
                        if (data == null)
                            break;

                        int nHeroID = data.nHeroID;
                        if (nHeroID <= 0)
                            break;

                        bool bHaveHero = LogicDataCenter.playerSystemDataManager.CheckIsHaveHero(nHeroID);
                        if (!bHaveHero)
                            break;

                        if (!UpdateRoomHeroList(nHeroID))
                            break;
                    }
                    break;
                case WndMsgID.WND_MSG_HEROSKINVIEW_UPDATESKINDATA:
                    {
                        if (m_wndView == null || !IsVisible())
                        {
                            break;
                        }

                        PlayerHeroSkinUpdateData Data = msgData as PlayerHeroSkinUpdateData;
                        if (Data.nSkinID <= 0 || Data.nHeroID <= 0)
                            break;

                        bool bHaveSkin = LogicDataCenter.heroSkinDataManager.CheckIsHaveSkin(Data.nHeroID, Data.nSkinID);
                        bool bUpdate = bHaveSkin;
                        if (!bUpdate)
                        {
                            break;
                        }

                        UpdateRoomSkinList(Data.nHeroID, Data.nSkinID);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEGENDCUPWAITCOUNTDOWN:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoomWaitCountdown = (UIMsgCmdData)msgData;
                        cmd_legendcup_room_wait_countdown data = IntPtrHelper.toData<cmd_legendcup_room_wait_countdown>(uiRoomWaitCountdown.ptrParam);

                        m_wndView.SetLegendCupTimeCountDown(data.nCountdown);
                    }
                    break;

                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_BAN_HERO_COUNTDOWN:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiRoomWaitCountdown = (UIMsgCmdData)msgData;
                        m_wndView.SetBanHeroTimeCountDown(uiRoomWaitCountdown.nParam);
                    }
                    break;

                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ROOMLEAVETOSCENE:
                    {
                        if (m_wndView == null || !IsVisible())
                            return;

                        ResetRoomViewData();
                        UISystem.Instance.ShowWnd(WndID.WND_ID_PLAYERBUTTONFRAME, true);
                        SetVisible(false);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SINGLE_BAN_HERO:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiBanHeroData = (UIMsgCmdData)msgData;
                        m_wndView.RecvSingleBanHero(uiBanHeroData.ptrParam, uiBanHeroData.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_ALL_BAN_HERO:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiBanHeroData = (UIMsgCmdData)msgData;
                        m_wndView.RecvAllBanHero(uiBanHeroData.ptrParam, uiBanHeroData.nPtrLen);
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_BAN_HERO_FLAG:
                    {
                        if (m_wndView == null)
                            return;

                        UIMsgCmdData uiBanHeroFlag = (UIMsgCmdData)msgData;
                        m_wndView.RecvBanHeroFlag(uiBanHeroFlag.nParam);
                    }
                    break;

            #region 新手引导部分

                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SELECTHERO_ADDGUIDEWIDGET:
                    {
                        if(m_wndView != null)
                        {
                            UGuideWidgetMsgData cmdData = (UGuideWidgetMsgData)msgData;
                            if (cmdData != null)
                            {
                                m_wndView.AddSelectHeroGuideUIWidget(cmdData);
                            }
                        }
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SELECTHERO_REMOVEGUIDEWIDGET:
                    {
                        if (m_wndView != null)
                        {
                            UGuideWidgetMsgData cmdData = (UGuideWidgetMsgData)msgData;
                            if (cmdData != null)
                            {
                                m_wndView.RemoveSelectHeroGuideUIWidget(cmdData);
                            }
                        }
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_STARTBTN_ADDGUIDEWIDGET:
                    {
                        if (m_wndView != null)
                        {
                            UGuideWidgetMsgData cmdData = (UGuideWidgetMsgData)msgData;
                            if (cmdData != null)
                            {
                                m_wndView.AddStartBtnGuideUIWidget(cmdData);
                            }
                        }
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_STARTBTN_REMOVEGUIDEWIDGET:
                    {
                        if (m_wndView != null)
                        {
                            UGuideWidgetMsgData cmdData = (UGuideWidgetMsgData)msgData;
                            if (cmdData != null)
                            {
                                m_wndView.RemoveStartBtnGuideUIWidget(cmdData);
                            }
                        }
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_GUIDE_ACTIONBEGIN:
                    {
                        this.m_isKeyClose = false;
                        if(m_wndView != null)
                        {
                            m_wndView.GuideActionBegin();
                        }
                    }
                    break;
                case WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_GUIDE_ACTIONEND:
                    {
                        this.m_isKeyClose = true;
                        if(m_wndView != null)
                        {
                            m_wndView.GuideActionEnd();
                        }
                    }
                    break;
				#endregion
                default:
                    break;
            }
        }

        public void InitXPSkillFrame(int nHeroID)
        {
            if (m_wndView == null)
                return;

            m_wndView.ClearXPSkillFrame();
            List<SSchemeHeroXPskillConfig> configList = HeroXPskillConfig.Instance.GetHeroXPskillConfigList(nHeroID);
            if (configList == null)
                return;

            int nInitXpID = 0;
            bool bCanUseHero = false;
            ActorHeroInfo info = LogicDataCenter.playerSystemDataManager.GetActorAllHero();
            if (info.dicEntityHeroInfo.ContainsKey(nHeroID))
            {
                cmd_entity_hero_info heroInfo = info.dicEntityHeroInfo[nHeroID];
                nInitXpID = heroInfo.nUseXPSkillID;
                bCanUseHero = heroInfo.bIsBuy > 0 || heroInfo.bIsWeekFree > 0;
            }
            else if (info.dicEntityNoHaveHeroInfo.ContainsKey(nHeroID))
            {
                nInitXpID = info.dicEntityNoHaveHeroInfo[nHeroID].nUseXPSkillID;
            }

            int nSlotID = 1;
            cmd_Hero_XP_Skill_Info data = new cmd_Hero_XP_Skill_Info();
            SSchemeHeroXPskillConfig initConfig = new SSchemeHeroXPskillConfig();
            foreach (SSchemeHeroXPskillConfig config in configList)
            {
                bool bHaveXp = LogicDataCenter.heroXPSkillDataManager.GetHeroXPSkillInfo(nHeroID, config.nXPSkillID, ref data);
                bool bLock = !bHaveXp;

                m_wndView.SetAllXpSkill(nSlotID++, config.nIconID, config.nXPSkillID, bLock, bHaveXp ? config.szNoLockDesc : config.szLockDesc,nHeroID);

                if(config.nXPSkillID == nInitXpID)
                    initConfig = config;
            }

            m_wndView.SetInitSelectXpSkill(1, initConfig.nIconID, nInitXpID, initConfig.szNoLockDesc, bCanUseHero);
        }

        private void SetCampSelectHeroCanUseByComputer(int nCamp, bool bSelect, int nHeroId)
        {
            bool bSelfCamp = nCamp == LocalCamp;
            m_wndView.SetCampSelectHeroCanUseByComputer(!bSelect, nHeroId, bSelfCamp);
        }

        private void SetRoomTeamItemChangePosition(int nLocal, cmd_match_room_hero_pos_change data)
        {
            m_wndView.SetRoomTeamItemChangePosition(data.nCampFrom, data.nCampTo, data.nPlayerSlotIDFrom, data.nPlayerSlotIDTo);
            LocalCamp = nLocal > 0 ? data.nCampTo : LocalCamp;
            m_wndView.UpdateSelectHeroFrameLockByComputer(LocalCamp, m_nCampPlayerCount);
            UpdateTeamSelectHeroTipForChangePosition();
        }

        private void SetRoomObChangePosition(int nSelf, cmd_match_room_ob_pos_change_oc data)
        {
            if (data.bSrcIsOBPos > 0)
            {
                m_wndView.OnMsgDeleteOb(data.nSrcPos);

                int nCamp = data.nDesPos / m_nCampPlayerCount;
                int nSlot = data.nDesPos % m_nCampPlayerCount;
                m_wndView.SetOb2Team(nCamp, nSlot, nSelf > 0, false, data.szPlayerName);

                if (nSelf > 0)
                {
                    m_wndView.SetGameModeNotOb();
                }
            }
            else
            {
                int nCamp = data.nSrcPos / m_nCampPlayerCount;
                int nSlot = data.nSrcPos % m_nCampPlayerCount;
                m_wndView.SetTeamItemClear(nCamp, nSlot);

                cmd_match_room_ob_data node;
                node.nObPos = data.nDesPos;
                node.nActorID = data.nActorID;
                node.nHeadID = data.nHeadID;
                node.nSex = data.nSex;
                node.szPlayerName = data.szPlayerName;
                m_wndView.OnMsgAddOb(node);

                if (nSelf > 0)
                {
                    m_wndView.SetGameModeOb();
                }
            }

            UpdateTeamSelectHeroTipForChangePosition();

            int nDescCamp = data.bDesIsOBPos > 0 ? -1 : data.nDesPos / m_nCampPlayerCount;
            m_LocalCamp = nSelf > 0 ? nDescCamp : m_LocalCamp;

            if (nSelf > 0)
            {
                m_bLocalRoomOwner = false;
            }
        }

        private void UpdateTeamSelectHeroTipForChangePosition()
        {                 
            SelectArrayTipPage SelectTip = SelectArrayTipPage.Instance;
            if (SelectTip == null)
            {
                return;
            }

            m_wndView.ClearCampTeamSelectTip();       
            dicCampTeamHeroTip.Clear();
            for (int nSlotId = 0; nSlotId < m_nCampPlayerCount; nSlotId++)
            {
                dicCampTeamHeroTip[nSlotId] = SelectTip.GetHeroType(m_wndView.GetTeamItemHeroId(m_LocalCamp, nSlotId));
            }

            SelectTip.SetSelectTypeInfo(dicCampTeamHeroTip);
            for (int nTipTypeIndex = 0; nTipTypeIndex < (int)HeroType.HEROTYPE_ALL; nTipTypeIndex++)
            {
                string sTipDesc = "";
                bool bShow = SelectTip.IsTeamSelectArrayNeedShow(ref sTipDesc, nTipTypeIndex);
                if (!bShow)
                {
                    continue;
                }

                m_wndView.SetCampTeamSelectTip(nTipTypeIndex, sTipDesc);
            }

            int nCampPlayerCount = m_wndView.GetTeamCampPlayerCount(m_LocalCamp);
            int nLimitCount = SelectTip.GetHeroCampTipLimit((int)MatchType.MatchType_One);
            if (nLimitCount <= -1 || nCampPlayerCount < nLimitCount)
            {
                m_wndView.ClearCampTeamSelectTip();
            }

        }

        private void CreateGameModeLayout(cmd_match_hall_add_room data)
        {
            m_wndView.ResetRoomEnterViewData();
            m_nCampPlayerCount = data.nMaxPlayerNum/2;
            m_wndView.SetGamePatternCampPlayerCount(m_nCampPlayerCount);       
            m_wndView.SetGameModelWarId(data.nConfigWarID);
            m_wndView.SetSelectListInit();
            m_MatchType = (EMMatchType)data.nMatchTypeID;
            if (data.nRoomType == (int)EMHallRoomType.HallRoomType_LegendCup)
            {
                m_RoomType = EMHallRoomType.HallRoomType_LegendCup;
                m_bOnlineLegendCup = data.bOnlineLegendCup == 1 ? true : false;
            }
            else
            {
                m_RoomType = data.bIsMatchRoom > 0 ? EMHallRoomType.HallRoomType_Macth : EMHallRoomType.HallRoomType_Custom;
            }
            

            if (data.bIsMatchRoom == 1 && data.bIsDelayJoin == 1)
            {
                m_bDelayJoin = true;
                m_nDelayJoinTime = data.nDelayTime;
            }


            m_wndView.SetGameWaitingRoomID(data.nRoomIndex, m_RoomType == EMHallRoomType.HallRoomType_Custom);

            m_wndView.updateBenefitCard();

            m_wndView.BottomFunctionPart.SetRuneAndTalentComBobox();

            if (m_RoomType == EMHallRoomType.HallRoomType_LegendCup && !m_bOnlineLegendCup)
            {
                m_wndView.SetOfflineLegendCupDesc();
            }

            m_wndView.OnMsgInitOb(data.bPermitOb > 0, data.nMaxObNum);
        }

        private void SetAllTeamItemSummerInfo(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_war_personInfo_count obj = helper.get<cmd_war_personInfo_count>();
            for (int i = 0; i < obj.nCount; ++i)
            {
                cmd_Summoner_Skill_Change data = helper.get<cmd_Summoner_Skill_Change>();
                if (data.nCamp > 1)
                {
                    continue;
                }

                if (data.nPos >= 10)
                {
                    continue;
                }

                data.nPos = data.nPos >= m_nCampPlayerCount ? data.nPos - m_nCampPlayerCount : data.nPos;
                m_wndView.SetTeamItemSummerInfo(data.nCamp, data.nPos, data.nSummonerSkill[0], data.nSummonerSkill[1]);
            }
        }

        private void SetChangeTeamItemHeroIcon(cmd_match_room_hero_change data)
        {
            data.nHeroStarLv = data.nHeroStarLv > 100 ? 4 : data.nHeroStarLv;
            data.nHeroStaticLv = data.nHeroStaticLv > 100 ? 4 : data.nHeroStaticLv;
            data.nHeroStarLv = data.nHeroStarLv < -100 ? 4 : data.nHeroStarLv;
            data.nHeroStaticLv = data.nHeroStaticLv < -100 ? 4 : data.nHeroStaticLv;
            m_wndView.SetChangeTeamItemHeroIcon(data.nCamp, data.nPlayerSlotID, data.nHeroID, data.nHeroStarLv, data.nHeroStaticLv, data.bServerRand > 0);

            UpdateTeamSelectHeroTip(data.nCamp, data.nPlayerSlotID, data.nHeroID);
        }

        public void InitSelectHeroFrameInfo(IntPtr nHeroHeadSlotInfo, int nPtrLen)
        {
            m_wndView.AddRoomSelectHeroItemData(nHeroHeadSlotInfo, nPtrLen);
            m_wndView.SetRoomSelectHeroItemData();
            m_wndView.InitSelectHeroPart();
        }

        private void SetRoomMasterChange(cmd_match_room_master_change data)
        {
            m_bLocalRoomOwner = IsLocalRoomOwner(data.nSlef, true);

            m_wndView.SetGameRoomModel(m_RoomType, m_bLocalRoomOwner, m_bOnlineLegendCup);

            m_wndView.SetRoomMasterChanges(data.nCamp, data.nPlayerSlotID);
        }

        private void MySelfEnterRoom(cmd_match_room_role_data data)
        {
            bool bRoomOwner = data.bIsMaster == 0 ? false : true;
            m_bLocalRoomOwner = IsLocalRoomOwner(1, bRoomOwner);
            m_wndView.SetGameRoomModel(m_RoomType, m_bLocalRoomOwner, m_bOnlineLegendCup);
            m_LocalCamp = data.nCamp;

            m_wndView.SetGameModeNotOb();
            m_wndView.SetRoleEnterRoom(true, bRoomOwner, data.nCamp, data.nHeroID, data.nPlayerSlotID, data.szPlayerName, LocalCamp, false);

            // 杯赛玩家不显示Ob界面
            if (m_RoomType == EMHallRoomType.HallRoomType_LegendCup)
            {
                m_wndView.ObPart.gameObject.SetActive(false);
            }
        }

        private void OtherRoleEnterRoom(cmd_match_room_role_data data)
        {
            bool bRoomOwner = data.bIsMaster == 0 ? false : true;
            bool bIsComputerPlayer = data.bIsComputerPlayer == 0 ? false : true;
            m_wndView.SetRoleEnterRoom(false, bRoomOwner, data.nCamp, data.nHeroID, data.nPlayerSlotID, data.szPlayerName,LocalCamp, bIsComputerPlayer );
        }

        private void MySelfObEnterRoom(cmd_match_room_ob_data data)
        {
            bool bRoomOwner = false;
            m_bLocalRoomOwner = IsLocalRoomOwner(1, bRoomOwner);
            m_LocalCamp = -1;
            m_wndView.SetGameRoomModel(m_RoomType, m_bLocalRoomOwner, m_bOnlineLegendCup);

            m_wndView.SetGameModeOb();
            m_wndView.OnMsgAddOb(data);
        }

        private void OtherObEnterRoom(cmd_match_room_ob_data data)
        {
            m_wndView.OnMsgAddOb(data);
        }

        private void ResetRoomViewData()
        {
            if (m_wndView == null)
            {
                return;
            }

            SetMatchRoomState((int)EMatchRoomState.MatchRoomState_Wait);
            this.dicCampTeamHeroTip.Clear();
            m_RoomSkinCache.Clear();
            m_wndView.ResetRoomViewData();
        }

        public void SetMatchRoomState(EMatchRoomState nState)
        {
            m_CurRoomState = nState;
            m_wndView.SetMatchRoomState(m_CurRoomState);
            if (m_CurRoomState == EMatchRoomState.MatchRoomState_ReadyGo)
            {
                SendSelectHeroSkinMessage();
                m_wndView.SelectHeroPart.ButtonSelectTypeGroup.SetItemSelectedItem(1, true);
                m_wndView.BottomFunctionPart.BuyButton.gameObject.SetActive(false);

            }
        }

        public override bool OnEscCallBack()
        {
            if (m_wndView != null && m_wndView.banHeroView != null && m_wndView.banHeroView.IsActive())
            {
                m_wndView.banHeroView.gameObject.SetActive(false);
                return true;
            }

            TooltipAndDialog.UIDialogData data = new TooltipAndDialog.UIDialogData
                        (
                            title: ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "ExitRoomTitle"),
                            message: getRoomExitDes(),
                            buttons: new UIWidgets.DialogActions() 
                            { { ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "ConfirmBtn"),() =>
                                        {
                                            if (m_CurRoomState < EMatchRoomState.MatchRoomState_ReadyGo)
                                            {
                                                CloseByMySelf();
                                                SetVisible(false);
                                            }
                                            return true;
                                        }   
                              }, 

                              { ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "CancelBtn"), () =>
                                        {
                                            return true;
                                        }   
                              }
                            },
                            modal: true
                        );
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_DIALOG, data);
            return true;
        }

        public string getRoomExitDes()
        {
            string sDes = "";
            switch (m_RoomType)
            {
                case EMHallRoomType.HallRoomType_MacthMachine:
                case EMHallRoomType.HallRoomType_Macth:
                    {
                        sDes = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "ExitRoomDes");
                    }
                    break;

                case EMHallRoomType.HallRoomType_Custom:
                    {
                        if (m_CurRoomState == EMatchRoomState.MatchRoomState_Wait)
                        {
                            sDes = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "WaitExitRoomDes");
                        }
                        else
                        {
                            sDes = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "ExitRoomDes");
                        }
                    }
                    break;

                case EMHallRoomType.HallRoomType_LegendCup:
                    {
                        sDes = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "LegendCupExitRoomDes");
                    }
                    break;
                default:
                    {
                        sDes = ULocalizationService.Instance.Get("UIView", "GameWaitingRoom", "WaitExitRoomDes");
                    }
                    break;
            }

            return sDes;
        }

        public void CloseByMySelf()
        {
            SendGameRoomLeaveToHalltMesseage();
            ResetRoomViewData();

            if (m_RoomType == EMHallRoomType.HallRoomType_LegendCup)
            {
                UISystem.Instance.ShowWnd(WndID.WND_ID_PLAYERBUTTONFRAME, true);
            }
        }

        public void CloseByOtherPlayer()
        {
            ResetRoomViewData();
            this.SetVisible(false);
        }

        public EMHallRoomType GetRoomType()
        {
            return m_RoomType;
        }

        public EMMatchType GetMatchType()
        {
            return m_MatchType;
        }
        
        private void SendGameRoomLeaveToHalltMesseage()
        {
            //通知逻辑层要离开房间
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ROOM_PLAYER_EXIT);
        }

        // bRoomOwner是否房主   LocalRoomOwner自身是否房主
        private bool IsLocalRoomOwner(int bLocal, bool bRoomOwner)
        {
            if (bLocal != 0 && bRoomOwner)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        public void SetGamePatternCampPlayerCount(int nCampPlayerCount)
        {
            if (m_wndView == null)
                return;

            m_wndView.SetGamePatternCampPlayerCount(nCampPlayerCount);
        }

        public void SetInitSelectSummonerSkill(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_count cmdCount = helper.get<cmd_count>();

            for (int i = 0; i < cmdCount.nCount; ++i)
            {
                if (i > 0)
                {
                    continue;
                }

                cmd_SummonerSkill_Info cmdInfo = helper.get<cmd_SummonerSkill_Info>();
                m_wndView.SetInitSelectSummonerSkill(i + 1, cmdInfo.nIconId, cmdInfo.nSkillId, cmdInfo.summonerSkillDesc);
            }    
        }

        public void SetAllSummonerSkill(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_count cmdCount = helper.get<cmd_count>();

            for (int i = 0; i < cmdCount.nCount; ++i)
            {
                cmd_SmAllSkillInfo cmdInfo = helper.get<cmd_SmAllSkillInfo>();
                bool bLock = cmdInfo.nIsUnlock <= 0 ? false : true;
                m_wndView.SetAllSummerSkill(i + 1, cmdInfo.nIconId, cmdInfo.nId, bLock, cmdInfo.summonerSkillDesc);
            }

            m_wndView.SelectDefaultSummerSkill();   
        }

        #region 禁人部分目前待续#
        //某个玩家进入禁止英雄状态
        public void MatchRoomRuleBanHero(IntPtr ptrParam, int nLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nLen);
            cmd_match_room_ban_hero_count obj = helper.get<cmd_match_room_ban_hero_count>();

            // 重置10个位置高亮的部分
            setAllPosNotBright();

            int nNeedTime = 0;
            int nCamp = 0;
            SelectPhase nSelectState = SelectPhase.SELECTPHASE_WAIT_DISABLEHERO;
            for (int i = 0; i < obj.nCount; ++i)
            {
                cmd_match_room_rule_ban_hero cmdData = helper.get<cmd_match_room_rule_ban_hero>();
                if (nNeedTime == 0) nNeedTime = cmdData.nTimeInterval;
                if (nCamp == 0) nCamp = cmdData.nCamp;
                if (cmdData.bIsSelf == 1)
                {
                    // CanSelectHero = true;
                    // CanChooseDisableHero = true;
                    // 如果是轮到本地玩家去选择禁用英雄， 则解除这个锁
                    //  SetSelectHeroListLock(false);

                    nSelectState = SelectPhase.SELECTPHASE_DISABLE;
                }
                //高亮哪个位置可以禁用英雄
                // SetChooseDisablePos(cmdData.nCamp, cmdData.nPos);
            }
            // 启用选择禁用英雄的倒计时
            // ShowSelectDisableHeroTime(nNeedTime, nCamp, (int)nSelectState);
        }

        private void setAllPosNotBright()
        {
            //m_wndView.setAllPosNotBright();
        }

        // 禁用机器人
        public void MatchRoomRuleBanComputerList(IntPtr ptrParam, int nParam)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nParam);

            cmd_count cmdCount = helper.get<cmd_count>();

            for (int i = 0; i < cmdCount.nCount; ++i)
            {
                int nHeroId = helper.getInt();

                //将两个阵营全都禁用
                m_wndView.SetCampComputerCanUse(0, false, nHeroId);
                m_wndView.SetCampComputerCanUse(1, false, nHeroId);
            }

        }

        //开始选择英雄
        public void MatchRoomRuleSelectHero(IntPtr ptrParam, int nPtrLen)
        {
            IntPtrVaArgParseHelper helper = new IntPtrVaArgParseHelper(ptrParam, nPtrLen);
            cmd_count cmdCount = helper.get<cmd_count>();

            // 重置10个位置高亮的部分
            setAllPosNotBright();

            int nTime = 0;
            int nCamp = 0;
            for (int i = 0; i < cmdCount.nCount; ++i)
            {
                cmd_match_room_rule_ban_hero cmdInfo = helper.get<cmd_match_room_rule_ban_hero>();
                if (nTime == 0) nTime = cmdInfo.nTimeInterval;
                if (nCamp == 0) nCamp = cmdInfo.nCamp;
                if (cmdInfo.bIsSelf == 1)
                {
                    //CanSelectHero = true;
                    //准备按钮开启
                    //  SetHeroConfirmBtnVisible();
                    //  SetSelectHeroListLock(false);
                }

                //高亮哪个位置可以禁用英雄
                // SetChooseDisablePos(cmdInfo.nCamp, cmdInfo.nPos);


            }
            //启用选择英雄倒计时
            // ShowSelectDisableHeroTime(nTime, nCamp, (int)SelectPhase.ENUM_SELECT);
        }

        // 禁止选英雄的个数
        public void MatchRoomRuleSetHeroLock(cmd_match_room_hero_can_select data)
        {

            bool bLock = data.bCanSelect == 0 ? true : false;
            m_wndView.SetSelectCanHeroSelect(bLock, data.nHeroID);
        }
        #endregion


        public void SendPlayerChangeTeamSlotMesseg(cmd_match_room_ob_pos_change data)
        {
            if (m_CurRoomState != EMatchRoomState.MatchRoomState_Wait)
            {
                return;
            }

            ViewEventHelper.Instance.SendCommand<cmd_match_room_ob_pos_change>(GameLogicDef.GVIEWCMD_MATCH_ROOM_HERO_POS_CHANGE, ref data);
        }

        public void SendSelectHeroMessage(int nHeroIconId)
        {
            if (m_CurRoomState != EMatchRoomState.MatchRoomState_SelectHero)
                return;

            LogicDataCenter.playerSystemDataManager.bInReconnect = false;
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ROOM_HERO_CHANGE, nHeroIconId);
        }

        public void SendPreSelectHeroMessage(int nHeroIconId)
        {
            if (m_CurRoomState != EMatchRoomState.MatchRoomState_Wait)
                return;

            LogicDataCenter.playerSystemDataManager.bInReconnect = false;
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ROOM_PRE_SELECT_HERO, nHeroIconId);
        }

        public void SendSelectHeroSkinMessage()
        {
            if (m_CurRoomState != EMatchRoomState.MatchRoomState_ReadyGo)
                return;

            if (m_RoomSkinCache.nHeroID <= 0 || m_RoomSkinCache.nSkinID <= 0)
                return;

            if (!LogicDataCenter.heroSkinDataManager.CheckIsHaveSkin(m_RoomSkinCache.nHeroID, m_RoomSkinCache.nSkinID))
                return;

            cmd_matchroom_selectheroskin cmdData = new cmd_matchroom_selectheroskin();
            cmdData.nHeroID = m_RoomSkinCache.nHeroID;
            cmdData.nSkinID = m_RoomSkinCache.nSkinID;
            ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ROOM_SELECT_HERO_SKIN, ref cmdData);
        }

        public void CacheSelectSkinInfo(int nHeroID,int nSkinID)
        {
            m_RoomSkinCache.nHeroID = nHeroID;
            m_RoomSkinCache.nSkinID = nSkinID;
            
            if (m_CurRoomState == EMatchRoomState.MatchRoomState_ReadyGo)
            {
                SendSelectHeroSkinMessage();
            }
        }

        private void UpdateTeamSelectHeroTip(int nCamp, int nSlotId, int nHeroIconId)
        {           
            if (nCamp != m_LocalCamp)
            {
                return;
            }

            SelectArrayTipPage SelectTip = SelectArrayTipPage.Instance;
            if (SelectTip == null)
            {
                return;
            }

            m_wndView.ClearCampTeamSelectTip();             
            dicCampTeamHeroTip[nSlotId] = SelectTip.GetHeroType(nHeroIconId);
            SelectTip.SetSelectTypeInfo(dicCampTeamHeroTip);
            for (int nTipTypeIndex = 0; nTipTypeIndex < (int)HeroType.HEROTYPE_ALL; nTipTypeIndex++)
            {
                string sTipDesc = "";
                bool bShow = SelectTip.IsTeamSelectArrayNeedShow(ref sTipDesc, nTipTypeIndex);
                if (!bShow)
                {
                    continue;
                }

                m_wndView.SetCampTeamSelectTip(nTipTypeIndex, sTipDesc);
            }

            int nCampPlayerCount = m_wndView.GetTeamCampPlayerCount(m_LocalCamp);
            int nLimitCount = SelectTip.GetHeroCampTipLimit((int)MatchType.MatchType_One);
            if (nLimitCount <= -1 || nCampPlayerCount < nLimitCount)
            {
                m_wndView.ClearCampTeamSelectTip();   
            }
        }

        public bool IsThisStateInRoom(EMatchRoomState state)
        {
            if (m_CurRoomState == state)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        private void CloseComboListView()
        {
            m_wndView.CloseComboListView();
        }

        private void CloseSkinSound()
        {
            m_wndView.ClearSkinSound();
        }

        protected override void PreSetVisible(bool _bVisible)
        {
            base.PreSetVisible(_bVisible);

            FullViewForceConditionHide FullData = new FullViewForceConditionHide();
            FullData.bForceHide = _bVisible;
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_GAMEWAITINGROOMVISILBLE, FullData);
        }

        protected override void PostSetVisible(bool _bVisible)
        {
            base.PostSetVisible(_bVisible);
            if (_bVisible)
            {
                if (m_wndView != null)
                    m_wndView.gameObject.SetActive(false);
                SceneManager.Instance.LoadSelectHeroScene(SetRoomShowAfterLoadScene);
                SoundPlayHandler.PlayUISceneSound((int)SoundIDIndex.GameWaitingRoomEnter);
            }
            else
            {
                SceneManager.Instance.UnLoadSelectHeroScene();
                SoundPlayHandler.StopUISceneSound();
            }
            SoundManager.Forbid3DSound(_bVisible);
        }

        public void SetRoomShowAfterLoadScene(SceneResInfoManager info)
        {

            if (m_wndView != null)
                m_wndView.gameObject.SetActive(true);

            if (m_wndView == null)
                SetVisible(true);

            UISystem.Instance.SendWndMessage(WndMsgID.WND_ID_GAMEWAITINGROOMVIEW_SHOWAFTERLOADSCENE, null);

            if (m_wndView)
            {
                m_wndView.SelectTransForm = info.SceneRoot.transform;
                m_wndView.InitSelectHeroPart();
            }         
        }

        private void UpdateRoomSkinList(int nHeroID,int m_nUpdateSkinID)
        {
            m_wndView.UpdateRoomSkinList(nHeroID,m_nUpdateSkinID);
        }

        private bool UpdateRoomHeroList(int m_nUpdateHeroID)
        {
            return m_wndView.UpdateRoomHeroList(m_nUpdateHeroID);
        }
    }
}
