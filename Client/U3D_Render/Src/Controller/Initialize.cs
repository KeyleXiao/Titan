#define TickCheck

using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Diagnostics;

using GameLogic;
using U3D_Render;
using ImageEffects;
using USpeedUI;
using DataCenter;
using UnityEngine.Events;

// 游戏流程类型
public enum InitializeState
{
    None = 0,		// 无用的
    Init,			// 初始化
    CheckVer,		// 版本检查(包括下载操作)
    Create,		    // 系统模块创建(调用Game.Create())
    Login,			// 登录
    //Home,		    // 主城
    Play,			// 游戏中
    Logout,			// 登出
    Error,			// 错误状态
    Close,			// 关闭

    MAX,            // 最大值
}

public enum EMouseLockModelType
{
    EMLMT_OFF,
    EMLMT_INWAR,
    EMLMT_INALL,
}

public class Initialize : MonoBehaviour
{
    /// <summary>
    /// LOG文件名，由逻辑层传过来的，通过onCommand_OpenLog赋值
    /// </summary>
    public static string LogFileName = "";

    static public bool isDebugMode = false;

    static public AsyncOperation loading = null;

    static public Initialize Instance = null;

    static public int localClientVersion = 0;

    /// <summary>
    /// 游戏是否在运行
    /// </summary>
    static public bool isOnGameRuning = false;

    /// <summary>
    /// 是否需要重启
    /// </summary>
    static public bool NeedToReStart = false;

    /// <summary>
    /// 窗口句柄
    /// </summary>
    static public IntPtr m_WindowHwnd = IntPtr.Zero;

    
    public static Camera mainCam = null;
    private EMouseLockModelType m_eMouseLockState = EMouseLockModelType.EMLMT_INWAR;
    static System.Threading.Mutex mutex = null;

    private bool isNeedToQuit = false;

    ResNode LoadingWaitngResNode = null;
    public void Awake()
    {
        isOnGameRuning = true;
        m_WindowHwnd = GameLogicAPI.getMainWindHwnd();
        if(!Application.isEditor)
        {
            GameLogicAPI.SetWindowText(Initialize.m_WindowHwnd, "泰坦-大地之子，勇往直前");
        }

        InitDoTween();
        GameLogicAPI.OpenLog();

        if(AssetBundleManager.isVaild)
        {
            LoadingWaitngResNode = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "fa32d80346317d445bf6bdd4f1b4c5da", true);
            if(!ResNode.isNullOrEmpty(LoadingWaitngResNode))
            {
                LoadingWaitngResNode.InstanceMainRes();
            }
        }

        DontDestroyOnLoad(gameObject);
        
        Type[] types = new Type[]
		{
			typeof(Trace),
		};
        CreateModulesSync(types);
        
        if (Time.timeScale < 0.01)
        {
            Time.timeScale = 1f;
        }

        mainCam = Camera.main;
        Instance = this;
        myTransform = this.transform;
        SoliderTarget = myTransform.FindChild("SoldierTarget").gameObject;
        Hero_Camera = Initialize.mainCam.gameObject;
        TargetFrame = myTransform.FindChild("TargetFrame").gameObject;
        Second_Camera = myTransform.FindChild("Second_Camera").gameObject;
        SecondTarget = Second_Camera.transform.FindChild("SecondTarget").gameObject;
        CamInitPos = Hero_Camera.transform.position;
        CamInitQua = Hero_Camera.transform.rotation;
        SetState(InitializeState.Init);

        SceneManager.RegisterSceneLoadFinishEvent(SceneLoadFinish);
        SceneManager.RegisterSceneLoadStartEvent(SceneLoadStart);

        SceneResInfoManager.RegisterActiveSceneChangedEvent(ActiveSceneChanged);

        UISystem.OnResolutionChanged += DoMouseLockInModeType;

        //将显示层配置文档初始化提前避免启用多线程时配置文档未初始化
        //ViewConfigManager.Init();
    }

    void ActiveSceneChanged(SceneResInfoManager cam)
    {
        Initialize.mainCam = cam.SceneCamera;
    }

    void SceneLoadFinish(string sceneName)
    {
        //场景加载，更新摄像机
        Initialize.mainCam = Camera.main;
        if (!Initialize.mainCam)
        {
            return;
        }
        Hero_Camera = Initialize.mainCam.gameObject;
    }

    void SceneLoadStart(string sceneName)
    {

    }

    /// <summary>
    /// 游戏窗口焦点切换事件
    /// </summary>
    public static bool isOnFocus = true;
    public class ApplicationFocusChangeEvent : UnityEvent<bool> { }
    public static ApplicationFocusChangeEvent onApplicationFocusChangeEvent = new ApplicationFocusChangeEvent();
    public void OnApplicationFocus(bool focusStatus)
    {
        isOnFocus = focusStatus;
        //if(SoldierCamera.MainInstance<SoldierCamera>() != null && SoldierCamera.MainInstance<SoldierCamera>().cameraMode == SoldierCamera.CameraControlMode.GodEyeControl)
        //{
        //    GameLogicConfigApi.bIsLockInWindow = isOnFocus;
        //}

        if(isOnFocus)
        {
            DoMouseLockInModeType();
        }
        else
        {
            GameLogicConfigApi.bIsLockInWindow = false;
        }

        //如果失去焦点,并且不能是编辑器
        if (!isOnFocus && !Application.isEditor && FPS.Value > 30)
        {
            QualitySettings.vSyncCount = 0;
            Application.targetFrameRate = 30;
        }
        else if (!Application.isEditor)
        {
            QualitySettings.vSyncCount = 0;
            Application.targetFrameRate = Screen.currentResolution.refreshRate;
        }
        ImageEffectManager.API_ApplicationFocusChanged(isOnFocus);
        //最后才告诉其他地方处理焦点事件吧
        onApplicationFocusChangeEvent.Invoke(isOnFocus);
    }

    [HideInInspector]
    public bool bCreated = false;

    private Transform myTransform = null;

    public GameObject SoliderTargetGO
    {
        get{ return SoliderTarget;}
    }
    private GameObject SoliderTarget = null;
    private GameObject Hero_Camera = null;
    private GameObject TargetFrame = null;
    private GameObject Second_Camera = null;
    private GameObject SecondTarget = null;

    //----临时代码
    //记录相机初始位置和角度，返回选择界面时重新赋值，以后需要调整的
    Vector3 CamInitPos;
    Quaternion CamInitQua;
    //----临时代码

    /// <summary>
    /// 当前要加载的场景名字
    /// </summary>
    private string strLoadSceneName;
    public string LoadSceneName
    {
        set
        {
            this.strLoadSceneName = value;
        }
    }

    /// <summary>
    /// 创建通用逻辑模块，主场景和战斗场景均使用，依赖配置表，通常情况下需要更新资源后再执行
    /// </summary>
    public IEnumerator CreateEnumerator()
    {
        bCreated = false;

        Type[] types = new Type[]
		{
			typeof(AssetSystem),
			typeof(ShaderLib),
            typeof(SceneManager),
            typeof(NatureManager),
            typeof(WarFogManager),
		};

        CreateModulesAsyn(types);

        //wait another coroutine finish
        while (true)
        {
            yield return new WaitForEndOfFrame();
            bool bOK = (CreatingModules.Count == 0);
            if (bOK)
            {
                break;
            }
        }

        yield return new WaitForSeconds(0.1f);

        if (!Application.isEditor && System.IO.File.Exists("BreakPoint.txt"))
        {
            GameUtil.MsgBox(m_WindowHwnd, "挂断点", "调试", 0);
        }

        if (!RenderViewAPI.Start())
        {
            Trace.LogError("Game Start Service failure");
            yield break;
        }
        Trace.Log("Game Start Service successed");
        // 初始化
        gameObject.AddComponent<GameViewCommandHandler>();

        OnDeviceStateChage += OnDeviceStateChange;

        // 初始化游戏接口
        IntPtrHelper helper = new IntPtrHelper();
        //GameLogicAPI.Start(ASpeedGame.Data.GameSettingsXml.GameSettingsXmlManager.Instance.GameSettingsModel.enableMultiThread.AValue,
        //    helper.toPtr(ref RenderViewAPI.g_RenderView));
        GameLogicAPI.Start(helper.toPtr(ref RenderViewAPI.g_RenderView));
        yield return new WaitForSeconds(1.0f);
        PrintTickCheck("初始化逻辑层API，至少等了1秒", CostThreshold_Lv1,_bStart:true);

        // 显示层配置价值
        ViewConfigManager.Init();
        PrintTickCheck("ViewConfigManager.Init");


        //资源是要配置初始化
        AssetBundleManager.Init();
        PrintTickCheck("AssetBundleManager.Init");



        LogicDataCenter.Init();
        PrintTickCheck("LogicDataCenter.Init");



        ImageSetting.Init();
        PrintTickCheck("ImageSetting.Init");

        ImageEffectManager.API_GameStart();
        PrintTickCheck("ImageEffectManager.Init");

        //GameViewCommandHandler.Instance.Start();
        MouseCursorManager.Instance.InitMouseCursorManager();
        PrintTickCheck("MouseCursorManager.Init");

        // 初始化实体接口,依赖上面的物件管理器所以顺序不能换
        EntityFactory.Init();
        PrintTickCheck("EntityFactory.Init");


        PrefabManager.Init();
        PrintTickCheck("PrefabManager.Init");


        SkinManager.Init();
        PrintTickCheck("SkinManager.Init");




        SoundManager.Init();
        PrintTickCheck("SoundManager.Init");

        UISystem.Init();
        PrintTickCheck("UISystem.Init");

        UTopNameManager.Init();
        PrintTickCheck("UTopNameManager.Init");

        USpeedUI.Blood.UBloodManager.Init();
        PrintTickCheck("UBloodManager.Init");



        InputManager.Init();
        PrintTickCheck("InputManager.Init");



        Effect.EffectNode.InitTargetCache();
        PrintTickCheck("Effect.EffectNode.InitTargetCache.Init");

        GUIDE.GuideManager.Init();
        PrintTickCheck(" GUIDE.GuideManager.Init");

        SceneEffectManager.Init();
        PrintTickCheck("SceneEffectManager.Init");
        if (LightingEffectFactory.Instance)
        {
            LightingEffectFactory.Instance.InitWhenGameStart();
        }
        PrintTickCheck("LightingEffectFactory.Init");

        SafeZoneEffectManager.Init();
        PrintTickCheck("SafeZoneEffectManager.Init");

        ImageSetting.TraceSettings();

        bCreated = true;

        List<string> argList = new List<string>();
        argList.AddRange(System.Environment.GetCommandLineArgs());

        CkeckUpdateOk(ref argList);
        PrintTickCheck("CkeckUpdateOk");

        CheckMultclient();
        PrintTickCheck("CheckMultclient");

        StageManager.Init();
        AssetBundleManager.DeleteAssets(ref LoadingWaitngResNode, true);
    }

    private void CheckMultclient()
    {
        if (!Application.isEditor && !System.IO.File.Exists("multclient.txt"))
        {
            //禁止多开
            bool ret = false;
            string dataPath = Application.dataPath;
            dataPath = dataPath.Replace("\\", "/");
            dataPath = dataPath.Replace("/", "");
            mutex = new System.Threading.Mutex(true, "glacierTTClientGameMutex", out ret);
            //创建成功
            if (ret)
            {
                mutex.ReleaseMutex();

            }
            else
            {
                GameUtil.MsgBox(IntPtr.Zero, "游戏已经运行!", "错误", 0x00000000 | 0x00000010);
                isNeedToQuit = true;
            }
        }
    }

    private void CkeckUpdateOk(ref List<string> argList)
    {
        if (!Application.isEditor && !System.IO.File.Exists("updateok.txt"))
        {
            bool isUpdateOk = false;
            //1是updateok
            if (argList.Count > 1)
            {
                string str = argList[1];
                if (str.Contains("updateok"))
                {
                    isUpdateOk = true;
                }
            }
            //1是版本号
            if (argList.Count > 2)
            {
                string str = argList[2];
                int.TryParse(str, out localClientVersion);
            }

            if (!isUpdateOk)
            {
                GameUtil.MsgBox(IntPtr.Zero, "请通过启动器运行游戏!", "错误", 0x00000000 | 0x00000010);
                isNeedToQuit = true;
            }
        }
    }

    // Update is called once per frame
    public void Update()
    {
        UpdateDeviceLost();
        if (!mainCam)
        {
            mainCam = Camera.main;
        }
        // 帧
        GameFrameManager.OnUpdate();
        // 定时器
        TimerManager.OnUpdate();
        DoTask();
        // 当前正在载入地图，不要处理消息
        if (loading != null)
        {
            if (!loading.isDone)
            {
                ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_PROGRESSING, (int)(loading.progress * 100));
                return;
            }
            else
            {
                loading = null;
                ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_PROGRESSING, 100);
                Trace.Log("load level asyn ok!");
            }
        }

        if (bCreated)
        {
            GameLogicAPI.onFrameUpdate();
        }
        FuntionShortCutCtrl nFunctionCtr = FuntionShortCutCtrl.GetInstance();
        if (InputManager.Raw_GetKeyUp(nFunctionCtr.GetKeyCodeByIndex(FuntionShortCutSlotIndex.ControlTraceOpen)))
        {
            if (!string.IsNullOrEmpty(LogFileName))
            {
                Process.Start(LogFileName);
            }
        }

        SkinManager.OnUpdate();
        SafeZone.Update();
        SafeZoneEffectManager.Update();
        if(isNeedToQuit)
        {
            bForbidQuit = false;
            Application.Quit();
        }


        if(NeedToReStart)
        {
            enabled = false;
            NeedToReStart = false;
            ReStartApplication();
            return;
        }

        GUIDE.GuideManager.Update();

        // 通知逻辑层处理消息
        //ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_FRAME_UPDATE);
    }

    public bool ForbidQuit
    {
        get { return bForbidQuit; }
        set { bForbidQuit = value; }
    }

    private bool bForbidQuit = true;
    public void OnApplicationQuit()
    {
        if (bForbidQuit && !Application.isEditor)
        {
            UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_SYSTEMSETTING_QUITGAME, null);
            Application.CancelQuit();
        }
        else
        {
            GameQuit();
        }
    }

    private void GameQuit()
    {
        DG.Tweening.DOTween.isQuitting = true;
        SaveUnityTXTLog();
        InputManager.Clear();
        EntityFactory.Instance.Clear();
        UTopNameManager.Instance.Clear();
        USpeedUI.Blood.UBloodManager.Instance.Clear();
        SoundManager.Clear();
        SkinManager.Clear();
        PrefabManager.Clear();
        ImageSetting.Clear();
        if (AnimationEffectFactory.Instance)
        {
            AnimationEffectFactory.Instance.UnLoadAnimEffectData();
        }

        if (LightingEffectFactory.Instance)
        {
            LightingEffectFactory.Instance.ClearCache();
            LightingEffectFactory.Instance.ClearWhenGameEnd();
        }

        if (Application.isEditor)
        {
            Shader.globalMaximumLOD = int.MaxValue;
        }

        ImageEffectManager.API_GameEnd();
        SafeZoneEffectManager.Clear();
        //清除资源
        AssetBundleManager.Clear();
        SceneResInfoManager.ClearOnAppQuit();
        Resources.UnloadUnusedAssets();
        //GC.Collect();
    }

    public void OnDestroy()
    {


        isOnGameRuning = false;
        OnDeviceStateChage -= OnDeviceStateChange;
        OnDeviceStateChage = null;
        mainCam = null;
        Effect.EffectNode.ClearTargetCache();
        ViewConfigManager.Destroy();
        RenderViewAPI.Stop();
        GameLogicAPI.Stop();

        SceneManager.UnRegisterSceneLoadFinishEvent(SceneLoadFinish);

        MouseCursorManager.Instance.DestroyMouseCursroManager();
        FuntionShortCutCtrl.Destroy();
        GUIDE.GuideManager.Destroy();
        SceneEffectManager.Destroy();
    }

    public void OnGUI()
    {

        if (PrefamceInfo.Instance)
        {
            PrefamceInfo.Instance.OnGUIInterial();
        }

        //if (m_nState == InitializeState.Login)
        //{
        //    if (SceneManager.Instance!=null && SceneManager.Instance.SceneName == "Login")
        //    {
        //strStepID = GUI.TextField(new Rect(100, 100, 200, 35), strStepID);
        //int.TryParse(strStepID, out nStepID);
        //if (GUI.Button(new Rect(100, 50, 200, 35), "设置新手引导步骤"))
        //{
        //    cmd_entity_set_noviceguide msgInfo = new cmd_entity_set_noviceguide();
        //    msgInfo.nNoviceGuideType = nStepID;

        //    EntityEventHelper.Instance.SendCommand<cmd_entity_set_noviceguide>(EntityFactory.MainHeroID, EntityLogicDef.ENTITY_CMD_ENTITY_SET_NOVICEGUIDE, ref msgInfo);
        //    return;
        //}

        //        if (GUI.Button(new Rect(100, 100, 200, 35), "Load Cest01"))
        //        {
        //            SceneManager.Instance.LoadScene("Cest01");
        //            return;
        //        }

        //        if (GUI.Button(new Rect(100, 150, 200, 35), "Load Herocamp"))
        //        {
        //            SceneManager.Instance.LoadScene("Herocamp");
        //            return;
        //        }

        //        if (GUI.Button(new Rect(100, 200, 200, 35), "Load Scene_xuedi"))
        //        {
        //            SceneManager.Instance.LoadScene("Scene_xuedi");
        //            return;
        //        }

        //        if (GUI.Button(new Rect(100, 250, 200, 35), "Load Scene_ChengLi"))
        //        {
        //            SceneManager.Instance.LoadScene("Scene_ChengLi");
        //            return;
        //        }

        //        if (GUI.Button(new Rect(100, 300, 200, 35), "Load Scene_TaoYuan"))
        //        {
        //            SceneManager.Instance.LoadScene("Scene_TaoYuan");
        //            return;
        //        }
        //    }
        //}
    }



    Hashtable CreatingModules = new Hashtable();
    public void CreateModulesSync(Type[] types)
    {
        CreatingModules.Clear();
        foreach (Type type in types)
        {
            if (type == null)
            {
                continue;
            }
            int progress = -1;
            CreatingModules[type] = progress;
        }

        foreach (Type type in types)
        {
            if (type == null)
            {
                continue;
            }
            string strType = type.ToString();
            Trace.Log("initialization " + strType + " begin!");
            int t1 = Api.GetTickCount();
            Component com = gameObject.GetComponent(type);
            if (com == null)
            {
                com = gameObject.AddComponent(type);
            }
            if (com != null)
            {
                System.Reflection.MethodInfo mi = type.GetMethod("Create");
                if (mi != null)
                {
                    mi.Invoke(com, null);
                }
            }
            int t2 = Api.GetTickCount();
            int dt = t2 - t1;
            if (com != null)
            {
                Trace.Log("initialization " + strType + " end ok! time=" + dt + "ms");
            }
            else
            {
                Trace.Error("initialization " + strType + " end fail! time=" + dt + "ms");
            }
            {
                int progress = 999;
                CreatingModules[type] = progress;
            }
        }
        CreatingModules.Clear();
    }

    public void CreateModulesAsyn(Type[] types)
    {
        StartCoroutine(CreateModulesEnumerator(types));
    }

    public IEnumerator CreateModulesEnumerator(Type[] types)
    {
        CreatingModules.Clear();
        foreach (Type type in types)
        {
            if (type == null)
            {
                continue;
            }
            int progress = -1;
            CreatingModules[type] = progress;
        }

        foreach (Type type in types)
        {
            if (type == null)
            {
                continue;
            }
            {
                int progress = 0;
                CreatingModules[type] = progress;
            }
            string strType = type.ToString();
            Trace.Log("initialization " + strType + " begin!");
            int t1 = Api.GetTickCount();
            Component com = gameObject.GetComponent(type);
            if (com == null)
            {
                com = gameObject.AddComponent(type);
            }
            if (com != null)
            {
                System.Reflection.MethodInfo miasyn = type.GetMethod("CreateAsyn");
                System.Reflection.MethodInfo miprogress = type.GetMethod("GetProgress");
                System.Reflection.MethodInfo mi = type.GetMethod("Create");
                if (miasyn != null && miprogress != null)
                {
                    miasyn.Invoke(com, null);
                    //wait for result
                    while (true)
                    {
                        yield return new WaitForEndOfFrame();
                        int progress = (int)miprogress.Invoke(com, null);
                        CreatingModules[type] = progress;
                        if (progress >= 100)
                        {
                            break;
                        }
                    }
                }
                else if (mi != null)
                {
                    mi.Invoke(com, null);
                }
            }
            int t2 = Api.GetTickCount();
            int dt = t2 - t1;
            if (com != null)
            {
                Trace.Log("initialization " + strType + " end ok! time=" + dt + "ms");
            }
            else
            {
                Trace.Error("initialization " + strType + " end fail! time=" + dt + "ms");
            }

            {
                int progress = 100;
                CreatingModules[type] = progress;
            }

            yield return new WaitForEndOfFrame();

            {
                int progress = 999;
                CreatingModules[type] = progress;
            }
        }
        CreatingModules.Clear();
    }

    #region 游戏流程类型
    private InitializeState m_nState = InitializeState.None;

    /// <summary>
    /// 改变游戏流程
    /// </summary>
    /// <param name="nState">当前状态</param>
    /// <returns>是否成功</returns>
    public bool SetState(InitializeState nState)
    {
        if (nState == m_nState)
        {
            return false;
        }

        // 当游戏流程退出
        OnExit(m_nState);

        // 改变流程
        //Trace.Log("InitializeState.SetState():" + m_nState.ToString() + "->" + nState.ToString());
        m_nState = nState;

        // 当游戏流程进入
        OnEnter(m_nState);

        return true;
    }

    /// <summary>
    /// 当游戏流程进入
    /// </summary>
    /// <param name="nState">新状态</param>
    /// <param name="nOldState">旧状态</param>
    private void OnEnter(InitializeState nState)
    {
        // 流程
        switch (nState)
        {
            case InitializeState.None:		// 无用的
                {

                }
                break;
            case InitializeState.Init:		// 初始化
                {
                    //播放Logo动画
                    //Game.m_singleton.UIManager.LogoUI.ShowWindow();
                    Initialize.mainCam.backgroundColor = Color.black;
                }
                break;
            case InitializeState.CheckVer:		// 版本检查(包括下载操作)
                {
                    // 创建资源下载管理器
                    ResDownManager rdm = gameObject.AddComponent<ResDownManager>();
                    // 创建资源版本管理器
                    ResVerManager rvm = gameObject.AddComponent<ResVerManager>();
                    rvm.StartVerCheck();
                }
                break;
            case InitializeState.Create:		 // 系统模块创建(调用Game.Create())
                {
                    // 偷偷在登录界面后台用协程加载
                    gameObject.AddComponent<LightingEffectFactory>();
                    gameObject.AddComponent<AnimationEffectFactory>();
#if USE_TIMMER_BEHAVIOUR
            gameObject.AddComponent<UpdateBehaviourOnTimerManager>();
#endif
                    // 启动创建游戏所有模块        
                    StartCoroutine(CreateEnumerator());
                }
                break;
            case InitializeState.Login:		// 登录
                {


                    //if (SFGFxMovieManager.Instance != null)
                    //{
                    //    SFGFxMovieManager.Instance.InitGameCreateDefaultMovie();
                    //}
                    //// 播放登陆音乐
                    //PlayLoginAudio(true);

                    //// 启动登录管理流程
                    //m_loginManager = gameObject.AddComponent<LoginManager>();
                    //m_loginManager.StartEx(Game.m_singleton.bIsDebug);




                    //Initialize.mainCam.backgroundColor = new Color(125.0f / 255.0f, 125.0f / 255.0f, 255.0f / 255.0f);
                }
                break;
            //case InitializeState.Home:		// 主城
            //{

            //}
            //    break;
            case InitializeState.Play:			// 游戏中
                {
                }
                break;
            case InitializeState.Logout:		// 登出
                {
                }
                break;
            case InitializeState.Error:		// 错误
                {
                }
                break;
            case InitializeState.Close:		// 关闭
                {
                }
                break;
        }

    }

    /// <summary>
    /// 驱动游戏流程
    /// </summary>
    private void DoTask()
    {

        //////////////////////////////////////////////////////////////////////////
        // 流程
        switch (m_nState)
        {
            case InitializeState.None:		// 无用的
                {
                }
                break;
            case InitializeState.Init:		// 初始化
                {
                    //if (Game.m_singleton.UIManager.LogoUI.bIsShowNormal)
                    {
                        SetState(InitializeState.Create);
                    }
                }
                break;
            case InitializeState.CheckVer:		// 版本检查
                {
                    if (ResVerManager.bResVerMgrIsOver)
                    {
                        SetState(InitializeState.Create);
                        break;
                    }
                }
                break;
            case InitializeState.Create:		 // 系统模块创建(调用Game.Create())
                {
                    if (bCreated)
                    {
                        SetState(InitializeState.Login);
                        break;
                    }
                }
                break;
            case InitializeState.Login:		// 登录
                {
                    //if (m_loginManager.IsLogin)
                    //{
                    //    SetState(ClientState.Home);
                    //    break;
                    //}
                }
                break;
            case InitializeState.Play:			// 副本游戏中
                {
                }
                break;
            case InitializeState.Logout:	// 登出
                {
                }
                break;
            case InitializeState.Error:		// 错误
                {
                }
                break;
            case InitializeState.Close:		// 关闭
                {
                }
                break;
        }

        //////////////////////////////////////////////////////////////////////////


    }

    /// <summary>
    /// 当游戏流程退出
    /// </summary>
    /// <param name="nState">旧状态</param>
    private void OnExit(InitializeState nState)
    {
        // 流程
        switch (nState)
        {
            case InitializeState.None:		// 无用的
                {
                }
                break;
            case InitializeState.Init:		// 初始化
                {
                }
                break;
            case InitializeState.CheckVer:		// 版本检查(包括下载操作)
                {
                    ResVerManager rvm = GetComponent<ResVerManager>();
                    if (rvm != null)
                    {
                        ResNode.DestroyRes(ref rvm);
                    }
                    ResDownManager rdm = GetComponent<ResDownManager>();
                    if (rdm != null)
                    {
                        ResNode.DestroyRes(ref rdm);
                    }
                }
                break;
            case InitializeState.Create:		 // 系统模块创建(调用Game.Create())
                {
                }
                break;
            case InitializeState.Login:		// 登录
                {
                    //ResNode.DestroyRes(ref m_loginManager);
                    //登录完成之后需要隐藏登陆窗口和注销事件订阅
                    //Game.m_singleton.UIManager.LoginUI.HideWindow();
                    //Game.m_singleton.UIManager.LoginUI.UnRegLoginEvent();
                }
                break;
            //case InitializeState.Home:		// 主城游戏
            //{
            //}
            //break;
            case InitializeState.Play:			// 副本游戏中
                {
                    //Game.m_singleton.EctypeManager.CloseEctype();
                }
                break;
            case InitializeState.Logout:		// 登出
                {
                }
                break;
            case InitializeState.Error:		// 错误
                {
                }
                break;
            case InitializeState.Close:		// 关闭
                {
                }
                break;
        }
    }
    #endregion

    #region 场景加载部分
    public void ResetGameManager()
    {
        if(SoliderTarget)
        {
            SoliderTarget.transform.parent = this.myTransform;
        }
        
        if(TargetFrame)
        {
            TargetFrame.transform.parent = this.myTransform;
        }
        
        if(Second_Camera)
        {
            Second_Camera.transform.parent = this.myTransform;
        }
       
        if(SecondTarget)
        {
            SecondTarget.transform.parent = this.myTransform;
        }
        
        if(Hero_Camera)
        {
            Hero_Camera.transform.SetPosition( CamInitPos);
            Hero_Camera.transform.rotation = CamInitQua;
        }

    }
    #endregion


    private static void SaveUnityTXTLog()
    {
        //不要保存了，现在dmp足够，不需要这信息
        //string path = Application.dataPath;
        //if (!System.IO.File.Exists(path + "/output_log.txt"))
        //{
        //    return;
        //}
        
        //string finalPath = path + "/UnityTXTLog";
        //if(!System.IO.Directory.Exists(finalPath))
        //{
        //    System.IO.Directory.CreateDirectory(finalPath);
        //}
        //else
        //{
        //   int fileCount = System.IO.Directory.GetFiles(finalPath).Length;
        //    if(fileCount > 20)
        //    {
        //        System.IO.Directory.Delete(finalPath, true);
        //        System.IO.Directory.CreateDirectory(finalPath);
        //    }
        //}
        //string fileName = "output_log" + "[" + System.DateTime.Now.Month + "-" + System.DateTime.Now.Day + "-" + System.DateTime.Now.Hour + "-" + System.DateTime.Now.Minute + "].txt";
        //System.IO.File.Copy(path + "/output_log.txt", finalPath + "/" + fileName, true);
    }

    private void InitDoTween()
    {
        DG.Tweening.DOTween.Init();
    }

    public void DoMouseLockInModeType()
    {
        GameLogicConfigApi.LockMouseInWindow(JudgeIsMouseLock());
    }

    public bool JudgeIsMouseLock()
    {
        bool bMouseLock = false;
        switch (m_eMouseLockState)
        {
            case EMouseLockModelType.EMLMT_OFF:
                {
                    bMouseLock = false;
                }
                break;
            case EMouseLockModelType.EMLMT_INALL:
                {
                    bMouseLock = true;
                }
                break;
            case EMouseLockModelType.EMLMT_INWAR:
                {
                    bMouseLock = GameLogicAPI.isInWarScene() > 0;
                }
                break;

            default:
                break;
        }

        //UnityEngine.Debug.Log( "bMouseLock" + bMouseLock);
        return bMouseLock;
    }

    public EMouseLockModelType MouseLockModelType
    {
        set { m_eMouseLockState = value; }
        get { return m_eMouseLockState; }
    }

   #region 设备丢失相关
    //因为Unity无法设备丢失接口，使用这种蛋疼的方式
    public static System.Action<bool> OnDeviceStateChage = null;

    public static int LostDeviceCheckFrame = 3;
    public static int currLostDeviceCheckTimer = -1;
    public static bool isDeviceLost = false;
    static bool LastDeviceState = false;

    /// <summary>
    /// 最大设备丢失时间，由于unity无法解决锁屏后的内存泄露问题，所以，不能无限制的锁屏，经过测试，锁屏三小时后
    /// 不在运行，直接退出游戏，并且提示"与服务器断开连接，请重新登录游戏".
    /// </summary>
    private static float maxLostTime = 60 * 60 * 3;
    private static float currLostTime = 0.0f;

    void ReStartApplication()
    {
        ForbidQuit = false;
        Application.Quit();

        string LauncherPath = Application.dataPath;
        LauncherPath = LauncherPath.Replace("\\", "/");
        LauncherPath = LauncherPath.Replace("Bin/neice/TTClient_Data", string.Empty);

        ProcessStartInfo info = new ProcessStartInfo();
        info.WorkingDirectory = LauncherPath;
        LauncherPath += "泰坦.exe";
        info.FileName = LauncherPath;

        if (System.IO.File.Exists(LauncherPath))
        {
            if (null != mutex)
            {
                mutex.Close();
                mutex = null;
            }
            if (null == Process.Start(info))
            {
                UnityEngine.Debug.LogWarning("启动器:" + LauncherPath);
                GameUtil.MsgBox(m_WindowHwnd, "启动游戏失败,请手动启动游戏", "提示", 0);
            }
        }
        else
        {
            UnityEngine.Debug.LogWarning("找不到启动器:" + LauncherPath + ",启动失败");
        }
    }
    
    void UpdateDeviceLost()
    {
        //只有windows的运行时才会有这个
        if(Application.platform != RuntimePlatform.WindowsPlayer)
        {
            return;
        }

        if(isDeviceLost)
        {
            currLostTime += Time.deltaTime;
            if (currLostTime > maxLostTime)
            {
                GameUtil.MsgBox(m_WindowHwnd, "与服务器断开连接,请重新启动游戏.\n(长时间锁屏容易导致游戏断线,挂机时请不要锁屏)", "提示", 0);
                ReStartApplication();
            }

            currLostDeviceCheckTimer--;
            if(currLostDeviceCheckTimer <= 0)
            {
                currLostTime = 0.0f;
                isDeviceLost = false;
            }
        }
        if(LastDeviceState != isDeviceLost)
        {
            LastDeviceState = isDeviceLost;
            UnityEngine.Debug.Log("DeviceStateChange,IsLost:" + isDeviceLost);
            if(null != OnDeviceStateChage)
            {
                OnDeviceStateChage(isDeviceLost);
            }
        }
    }
    #endregion

    public void OnDeviceStateChange(bool _bDeviceLost)
    {
        if(!_bDeviceLost)
        {
            ImageSetting.ResetCurrentResolution();
        }
    }

    public void CheckGameVersionID(int nVersion)
    {
        if (Application.isEditor || System.IO.File.Exists("updateok.txt"))
            return;

        if(nVersion == 0)
        {
            Trace.LogError("服务器下发游戏最新版本号为" + nVersion);
        }

        if(localClientVersion != nVersion)
        {
            Trace.LogError("版本号对不上！本地版本号为：" + localClientVersion + "服务器最新版本号为：" + nVersion);
            GameUtil.MsgBox(IntPtr.Zero, "游戏版本不是最新，请通过启动器更新游戏!", "错误", 0x00000000 | 0x00000010);
            isNeedToQuit = true;
        }
    }

    /// <summary>
    /// 任务栏闪烁
    /// </summary>
    /// <param name="_nFlashCount">闪烁次数</param>
    public void FlashWindow(uint _nFlashCount = UInt32.MaxValue)
    {
        FLASHWINFO fInfo = new FLASHWINFO();

        fInfo.cbSize = Convert.ToUInt32(Marshal.SizeOf(fInfo));
        fInfo.hwnd = m_WindowHwnd;
        fInfo.dwFlags = (uint)(FlashWType.FLASHW_TRAY | FlashWType.FLASHW_TIMERNOFG);
        fInfo.uCount = _nFlashCount; 
        fInfo.dwTimeout = 0;
        GameLogicAPI.FlashWindowEx(ref fInfo);
    }


    #region 耗时相关
    private uint m_nTickCheckCount = 0;
    public const uint CostThreshold_Lv0 = 50;
    public const uint CostThreshold_Lv1 = 100;
    /// <summary>
    /// 开始耗时统计,必须与EndTickCheck成对使用
    /// </summary>
    /// <param name="_strDebugInfo"></param>
    public static void StartTickCheck(string _strDebugInfo = "")
    {
#if TickCheck
        if (!UnityEngine.Debug.isDebugBuild)
            return;

        Instance.m_nTickCheckCount = GameLogicAPI.getTickCount();
        if(!string.IsNullOrEmpty(_strDebugInfo))
            Trace.Log("开始计时, "+_strDebugInfo + ",当前时间:" + Instance.m_nTickCheckCount);
#endif
    }

    public static void EndTickCheck(string _strDebugInfo="")
    {
#if TickCheck
        if (!UnityEngine.Debug.isDebugBuild)
            return;

        Instance.m_nTickCheckCount = GameLogicAPI.getTickCount();
        if (!string.IsNullOrEmpty(_strDebugInfo))
            Trace.Log("结束计时, " + _strDebugInfo + ",当前时间:" + Instance.m_nTickCheckCount);
#endif

    }


    /// <summary>
    /// 截取耗时并打印
    /// </summary>
    /// <param name="_strDebugInfo">额外信息打印</param>
    /// <param name="_nCostThreshold">耗时阀值</param>
    /// <param name="_bStart">重置耗时</param>
    public static void PrintTickCheck(string _strDebugInfo,uint _nCostThreshold = CostThreshold_Lv1, bool _bStart = false)
    {
#if TickCheck
        if (!UnityEngine.Debug.isDebugBuild)
            return;

        uint newCount = 0;
        if (_bStart)
        {
            Instance.m_nTickCheckCount = GameLogicAPI.getTickCount();
            newCount = Instance.m_nTickCheckCount;
        }
        else
        {
            newCount = GameLogicAPI.getTickCount();
        }     
        
        uint deltaCount = newCount - Instance.m_nTickCheckCount;

        Instance.m_nTickCheckCount = newCount;

        if(deltaCount >= _nCostThreshold)
            Trace.LogWarning("截取计时, "+ _strDebugInfo + " ,阀值： "+_nCostThreshold+"，耗时:" + deltaCount);
#endif
    }
    #endregion
}
