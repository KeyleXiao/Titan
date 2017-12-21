/*******************************************************************
** 文件名:	StageManager.cs
** 修改人: 林建聪
** 日  期: 2015/07/23
** 描  述:  与逻辑层状态对应的场景状态管理
** 版  本:	1.0
** 应  用:  
 * 
*******************************************************************/
using UnityEngine;
using System;
using System.Collections;
using Stage_ManagerDef;
using GameLogic;
using USpeedUI;
using DataCenter;
using U3D_Render;
using U3D_Render.SkinCtrl;
using System.Threading;

/// <summary>
/// 与逻辑层状态对应的场景状态管理,游戏场景加载接口在这里
/// </summary>
public class StageManager : BaseStageManager
{
    public static void Init()
    {
        m_Instance = new StageManager();
    }

    public override bool SetStageState(EStage_State _NewState)
    {
        if (CurrentStageState == _NewState)
        {
            return false;
        }
        Trace.Log("StageManager old StageState=" + CurrentStageState);
        onLeave();
        if (onStageStateChanged != null)
        {
            onStageStateChanged.Invoke(CurrentStageState, _NewState);
        }
        CurrentStageState = _NewState;

        onEnter();

        Trace.Log("StageManager current StageState=" + CurrentStageState);
        return true;
    }

    private void onLeave()
    {
        switch (CurrentStageState)
        {
            case EStage_State.ESS_NONE:
                break;
            case EStage_State.ESS_LOGIN:
                onLeaveLoginState();
                break;
            case EStage_State.ESS_CHOOSE:
                onLeaveChooseState();
                break;
            case EStage_State.ESS_LOADSCENE:
                onLeaveLoadSceneState();
                break;
            case EStage_State.ESS_GAME:
                onLeaveGameState();
                break;
            case EStage_State.ESS_MATCHROOM:
                onLeaveMatchRoomState();
                break;
            case EStage_State.ESS_BATTLE:
                onLeaveBattleState();
                break;
        }
    }

    private void onEnter()
    {
        switch (CurrentStageState)
        {
            case EStage_State.ESS_NONE:
                break;
            case EStage_State.ESS_LOGIN:
                onEnterLoginState();
                break;
            case EStage_State.ESS_CHOOSE:
                onEnterChooseState();
                break;
            case EStage_State.ESS_LOADSCENE:
                onEnterLoadSceneState();
                break;
            case EStage_State.ESS_GAME:
                onEnterGameState();
                break;
            case EStage_State.ESS_MATCHROOM:
                onEnterMatchRoomState();
                break;
            case EStage_State.ESS_BATTLE:
                onEnterBattleState();
                break;
        }
    }

    //进入登录状态的操作
    private void onEnterLoginState()
    {
        FuntionShortCutCtrl.GetInstance().ClearAndRest();
        LogicDataCenter.onEnterLoginState();
        CurStageSubState = EStage_SubState.ESSS_NONE;

        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, null);
    }

    //离开登录状态的操作
    private void onLeaveLoginState()
    {
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_LEAVE, null);
    }

    //进入选择静态角色状态的操作
    private void onEnterChooseState()
    {
        CurStageSubState = EStage_SubState.ESSS_NONE;

        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_CHOOSESTATE_ENTER, null);
    }
    //离开选择角色状态的操作
    private void onLeaveChooseState()
    {
        //if (LoadingWindow != null)
        //{
        //    LoadingWindow.EnterMovie(GFxLoadingWin.ELoadingType.ELW_Default);
        //}
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_CHOOSESTATE_LEAVE, null);

    }

    //进入游戏状态的操作
    private void onEnterGameState()
    {
        //GameContextManager.UIContextView.SpawnContextView(EContextViewType.EGameContextView);
        //LogicDataCenter.playerSystemDataManager.Reset();
        CurStageSubState = EStage_SubState.ESSS_NONE;

        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, null);
        LogicDataCenter.ClearInEnterStaticGameState();
    }
    //离开游戏状态的操作
    private void onLeaveGameState()
    {
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, null);
    }

    //进入房间状态的操作
    private void onEnterMatchRoomState()
    {
        CurStageSubState = EStage_SubState.ESSS_NONE;

        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_ENTER, null);
    }
    //离开房间状态的操作
    private void onLeaveMatchRoomState()
    {
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMOM_MATCHROOMSTATE_LEAVE, null);
    }

    //准备切换Moba场景前的操作
    public override void onReadyToLoadMobaScene()
    {
        Initialize.Instance.ResetGameManager();

        LogicDataCenter.onEnterLoginMobaState();
        
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_LOGINMOBA, null);
    }

    //进入Moba战场状态的操作
    private void onEnterBattleState()
    {
        //////////////////////////////////////////////////////////////////////////
        //加载Moba战场所需要的配置文件
        //注意：目前没写卸载战场的配置文件部分
        // 暂时加的参数之后要修改
        //ViewConfigManager.LoadMobaConfigs(5);

        //////////////////////////////////////////////////////////////////////////
        //创建Moba战场需要的界面
        //注意：目前还没写卸载战场界面部分
        //全部默认为显示
        // if (CurrentStageState == EStage_State.ESS_BATTLE)
        // {
        //  LoadingWindow.LeaveMovie();
        //}

        //一进战场统一关闭死亡光效
        ImageEffects.ImageEffectManager.API_DisableImageEffect<ImageEffects.FullScreenSaturation>();
        ImageEffects.ImageEffectManager.API_EnableImageEffect<ImageEffects.DepthOfFieldScatter>();
        //一进战场统一关闭战争迷雾
        WarFogManager.Instance.End();
        //Debug.Log("Create Game Hud...");
        CurStageSubState = EStage_SubState.ESSS_WAR;

        LogicDataCenter.onEnterBattleState();

        EnterBattle();
        // UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, null);
    }
    void EnterBattle()
    {
        Game_ManagedDef.EGameMode gamemode = GameLogicAPI.getCurGameMode();
        IsMoba = gamemode == Game_ManagedDef.EGameMode.EGM_MCAMP ? false : true;
        IsReConnect = GameLogicAPI.IsContinuePlay();
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, null);
        if (IsReConnect | IsMoba)
        {
             UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER_PlayMovie, null);
        }
        else
        {
            //实体还未创建
            if (EntityFactory.MainHeroView==null)
            {
                Debug.Log("EnterBattle 实体还未创建");
                EntityFactory.CreateMainHeroEntityEvent += OnHeroEntityLoadComplete;
            }
            else 
            {
                //实体已创建
                Debug.Log("EnterBattle 实体已创建");
                OnHeroEntityLoadCompleteLoacl(EntityFactory.MainHeroView);
            }
        }
    }
    bool IsReConnect = false;//是否重连
    bool IsMoba = false;//是否为moba
    GameObject MainHerpObj = null;//主角Obj
    EntityView MainHerpEv = null;//主角Entity
    SkillCtrl MainSkillCtrl = null;//控制管理
    MouseCtrl MainMouseCtrl = null;//鼠标管理
    EntitySkinControl m_SkinControl = null;//模型管理
    /// <summary>
    /// 实体加载完成回调
    /// </summary>
    /// <param name="ev">实体</param>
    public void OnHeroEntityLoadComplete(CreateMainHeroEntityEventArgs e)
    {
        EntityFactory.CreateMainHeroEntityEvent -= OnHeroEntityLoadComplete;
        OnHeroEntityLoadCompleteLoacl(EntityFactory.getEntityViewByID(e.MainHeroUID));
    }
    public void OnHeroEntityLoadCompleteLoacl(EntityView e)
    {
        MainHerpEv = e;
        MainHerpObj = MainHerpEv.gameObject;
        if (MainHerpObj.GetComponent<CreatureStateMachine>().soliderController == null)
        {
            Debug.Log("EntityLoadComplete 模型未创建");
            //模型还未创建
            //注册
            m_SkinControl = MainHerpObj.GetComponent<EntitySkinControl>();
            m_SkinControl.RegsiterSkinChangedEvent(OnSkinChanged);
        }
        else
        {
            Debug.Log("EntityLoadComplete 模型已经创建");
            //模型已经创建
            OnSkinChangedLoacl();
        }
    }
    /// <summary>
    /// 模型加载完成初始化回调
    /// </summary>
    /// <param name="sk"></param>
    /// <param name="nPart"></param>
    protected void OnSkinChanged(SkinInstance sk, int nPart)
    {
        m_SkinControl.UnRegsiterSkinChangedEvent(OnSkinChanged);
        OnSkinChangedLoacl();
    }
    private void OnSkinChangedLoacl()
    {
        //关闭玩家控制输入***
        MainSkillCtrl = MainHerpObj.GetComponent<SkillCtrl>();
        MainSkillCtrl.enable = false;
        //关闭玩家鼠标输入***
        MainMouseCtrl = MainHerpObj.GetComponent<MouseCtrl>();
        MainMouseCtrl.enabled = false;
        //想要播放动画
        PlayBattleMovie();
        MainHerpObj.SetActive(false);
    }
    /// <summary>
    /// 播放开头动画（最快下一帧开始播放）
    /// </summary>
    private void PlayBattleMovie()
    {
        LightingEffectManager lem = EntityFactory.EffectGroup.GetComponent<LightingEffectManager>();
        if (lem != null)
        {
            LightEffectParam param = new LightEffectParam();
            param.srcObject = MainHerpObj;
            param.sourcePos = MainHerpObj.transform.position;
            param.targetPos = MainHerpObj.transform.position;

             lem.AddLighting(343, param);
            //lem.RegisterEffectComplete(343, OnMovieFinished);
        }
        LightingEffectManager lem2 = EntityFactory.GloabaleManager;
        if (lem2!=null)
        {
            EffectHelper.Play(ref MainHerpEv, 333, MainHerpEv.ID, 0, MainHerpObj.transform.position);
            lem2.RegisterEffectStart(333, OnMovieStart);
            lem2.RegisterEffectComplete(333, OnMovieFinished);
        }
    }
    /// <summary>
    /// 开头动画播放开始回调
    /// </summary>
    private void OnMovieStart()
    {
        //隐藏loading界面
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER_PlayMovie, null);
    }
    /// <summary>
    /// 开头动画播放完成回调
    /// </summary>
    public void OnMovieFinished()
    {
        //开启所有已生成
        if (MainHerpEv != null)
        {
            MainHerpObj.SetActive(true);
            MainHerpObj = null;
            MainHerpEv = null;
        }
        //开启玩家控制输入***
        if (MainSkillCtrl != null)
        {
            MainSkillCtrl.enable = true;
            MainSkillCtrl = null;
        }
        if (MainMouseCtrl != null)
        {
            MainMouseCtrl.enabled = true;
            MainMouseCtrl = null;
        }
        if (m_SkinControl != null)
        {
            //m_SkinControl.UnRegsiterSkinChangedEvent(OnSkinChanged);
            m_SkinControl = null;
        }
        LogicDataCenter.onEnterBattleState();

       //UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_ENTER, null);
        
        //EntityFactory.CreateMainHeroEntityEvent -= OnHeroEntityLoadComplete;
        //MainHerpEv.LightingEffectManager.UnRegisterEffectComplete(333, OnMovieFinished);

        //开启音效
        //开启光效
    }


    private void onLeaveBattleState()
    {
        //Debug.Log("Leave Battle State...");

        CurPermissionState = EStage_PermissionState.ESPS_NORMAL;

        USpeedUI.Blood.UBloodManager.Instance.DestroyALL();

        UTopNameManager.Instance.PreWarmTopName();

        ViewConfigManager.UnLoadMobaConfigs();

        LogicDataCenter.onLeaveBattleState();

        InputManager.Available = true;

        SoundManager.DeleteAll();

        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_BATTLESTATE_LEAVE, null);
    }

    private void onEnterLoadSceneState()
    {
        //Debug.Log("onEnterMigrateState");

        CurStageSubState = EStage_SubState.ESSS_NONE;

        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMOM_LOADSCENESTATE_ENTER, null);

        SoundManager.CreateUISound((int)SoundIDIndex.MapLogin);
    }

    private void onLeaveLoadSceneState()
    {
        //Debug.Log("onLeaveMigrateState");

        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMOM_LOADSCENESTATE_LEAVE, null);

        SoundManager.DeleteSound((int)SoundIDIndex.MapLogin);
        SoundPlayHandler.PlaySceneSound(getSoundID());
    }
    ///////////////////////////////////////////////////////////////////
    // 场景加载部分
    private game_LoadSceneInfo curLoadSceneInfo = new game_LoadSceneInfo();

    public override int getSoundID()
    {
        return curLoadSceneInfo.nSoundID;
    }

    public override int getMapID()
    {
        return curLoadSceneInfo.nMapID;
    }

    public override void LoadSceneBegin(game_LoadSceneInfo _data)
    {
        Trace.Log("Load Scene Begin:" + _data.strSceneName);

        SceneManager.RegisterSceneProgressEvent(LoadingScene);
        SceneManager.RegisterSceneLoadStartEvent(SceneLoadStart);
        if (SceneManager.Instance.IsLoaded(_data.strSceneName))
        {
            ViewEventHelper.Instance.SendCommand((GameLogic.GameLogicDef)_data.nCmdID);
            //LoadingWindow.LeaveMovie();
            //GameContextManager.UCommonService.SetFullScreenState(EViewState.EVS_Pop, 0);
            UFullScreenStateData data = new UFullScreenStateData(_bShow: false);
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_FULLSCREEN_CHANGEIMAGE, data);
        }
        else
        {
            Vector3 BornVector = new Vector3();
            BornVector.x = _data.fBornPosX;
            BornVector.y = _data.fBornPosY;
            BornVector.z = _data.fBornPosZ;

            SceneManager.Instance.LoadScene(_data.strSceneName, _data.nMapID, BornVector);
        }

        //SceneManager.SceneTransitionStart += MatchScsenTranstitionStart;
        //SceneManager.SceneTransitionFinish += MatchScsenTranstitionFinish;
        curLoadSceneInfo = _data;
        UISystem.Instance.MatchWnd();
    }

    public override void LoadSceneEnd()
    {
        SceneManager.UnRegisterSceneProgressEvent(LoadingScene);
        SceneManager.UnRegisterSceneLoadStartEvent(SceneLoadStart);
        curLoadSceneInfo.nCmdID = -1;
        Trace.Log("Load Scene End:");
    }

    private void LoadingScene(float pro)
    {
        //告知逻辑层加载进度
        ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_LOAD_SCENE_PROGREESE_RATE, (int)(pro * 100));

        //if (LoadingWindow != null)
        //{
        //    LoadingWindow.SetIndicatorCurValue = e.progress * 100;
        //}
        UIMsgCmdData data = new UIMsgCmdData((int)WndMsgID.WND_MSG_COMMOM_LOADING_VALUE, (int)(pro * 100), string.Empty, IntPtr.Zero, 0);
        UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMOM_LOADING_VALUE, data);

        if (pro >= 1)
        {
            ViewEventHelper.Instance.SendCommand((GameLogic.GameLogicDef)curLoadSceneInfo.nCmdID);
            if ((int)GameLogicDef.GVIEWCMD_MATCH_LOAD_SCENE != curLoadSceneInfo.nCmdID)
            {
                // Debug.Log("LoadingScene finish...");
                //if (LoadingWindow != null)
                //{
                //    LoadingWindow.LeaveMovie();
                //}
                //GameContextManager.UCommonService.SetFullScreenState(EViewState.EVS_Pop, 0);
            }
        }
    }

    void MatchScsenTranstitionStart(string sceneName)
    {
        InputManager.Available = false;
        SceneManager.UnRegisterSceneTransitionStartEvent(MatchScsenTranstitionStart);

    }
    void MatchScsenTranstitionFinish(string sceneName)
    {
        InputManager.Available = true;
        SceneManager.UnRegisterSceneTransitionFinshEvent(MatchScsenTranstitionFinish);
    }

    void SceneLoadStart(string sceneName)
    {
        if (curLoadSceneInfo.nCmdID == -1)
            return;
        UTopNameManager.Instance.DestroyALL();
        if (curLoadSceneInfo.nCmdID == (int)GameLogicDef.GVIEWCMD_MATCH_LOAD_SCENE)
        {
            //Debug.Log("Create Game Hud...");
            USpeedUI.Blood.UBloodManager.Instance.PreWarmBlood();

        }

    }
}