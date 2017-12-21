/*******************************************************************
** 文件名:	SFGFxMovieManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2014/11/11
** 版  本:	1.0
** 描  述:	ScaleForm GFx 管理类
** 应  用:  	目前有 创建、移除、获取GFx 功能

**************************** 修改记录 ******************************
** 修改人:  谭强均
** 日  期: 2014/11/26
** 描  述: 1.增加泛型约束，已经统一缩放模式
 *        2.增加一个DynamicSwf Movie，动态创建的组件如：血条等，都放子啊这个swf文件中
 *        3.更改scaleform启动顺序，在Awake中。
 *        4.移植旧的UI
********************************************************************
 ***************************** 修改记录 ******************************
** 修改人:  林建聪
** 日  期: 2015/2/9
** 描  述: 1.创建Window时如果已有，则直接返回

********************************************************************/
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using System.IO;
using System.Collections;
using System.Collections.Generic;
using Scaleform;

public class SFGFxMovieManager : SFCamera
{

    /// <summary>
    /// 缩放方式。所有的Movie都用这一个缩放模式
    /// </summary>
    public static ScaleModeType GfxScaleType = ScaleModeType.SM_ExactFit;

    /// <summary>
    /// GFx默认背景颜色
    /// </summary>
    public static Color32 DefaultBGColor = new Color32(255, 255, 255, 0);

    /// <summary>
    /// GFx容器
    /// </summary>
    private Dictionary<string, Movie> windowRegistryList = new Dictionary<string, Movie>();

    /// <summary>
    /// GFx 实例
    /// </summary>
    private static SFGFxMovieManager instance = null;

    public static SFGFxMovieManager Instance
    {
        get
        {
            return instance;
        }
    }

    private MouseCtrl MyHeroMouseCtrl = null;

    /// <summary>
    /// 鼠标点击检测忽略列表
    /// </summary>
    private List<long> IgnoreHitTestMovieList = new List<long>();

    /// <summary>
    /// GFx的层级，层级高的在层级低的上面；同一层级，先创建的在后创建的上面
    /// </summary>
    public enum EGFxMovieLayer
    {
        EGFxLayer_Default = 1,//界面创建时默认在这层
        EGFxLayer_PopupLayer,//弹出式界面使用这层
        EGFxLayer_Base = EGFxLayer_PopupLayer + 100,//基本的界面层，鼠标，飘字在这层
        EGFxLayer_Loading,//显示载入界面时应该在所有界面的上层
    }
    //目前游戏就这个地方用来OnGUI方法，先在本类执行的时候存下Event数据，方便其他地方使用
    public static Event currentEvent { get; set; }

    /// <summary>
    /// 检测传来的Movie是否不为空且是否已经注册好SF的变量
    /// </summary>
    /// <param name="_movie">要检测的Movie</param>
    /// <returns>true：Movie可用；false：Movie暂不可用</returns>
    public static bool CheckTheMovieCanUse(Movie _movie)
    {
        bool res = false;
        SFMovieEx tmpMovie = _movie as SFMovieEx;
        if (tmpMovie == null)
        {
            return res;
        }

        res = tmpMovie.IsRegisterMovieSuccess ? true : false;

        if (!res)
        {
            Trace.LogWarning("这里有个消息不能调用界面功能，请修改!\n" + System.Environment.StackTrace);
        }
        return res;
    }

    public Movie GetWindow(string _name)
    {
        if (windowRegistryList.ContainsKey(_name))
        {
            return windowRegistryList[_name];
        }
        return null;
    }

    public static T GetWindow<T>() where T : Movie
    {
        return Movie.GetSingleton<T>();
    }

    public static void SetVisible<T>(bool _visible) where T : Movie
    {
        T window = SFGFxMovieManager.GetWindow<T>();
        if (window != null)
        {
            (window as SFMovieEx).SetMovieVisible(_visible);
        }
    }

    /// <summary>
    /// 创建一个Window
    /// </summary>
    /// <typeparam name="T">创建的是那个类</typeparam>
    /// <param name="_name">swf名字，用于区分各个不同的swf</param>
    /// <param name="_fullPatch">全路径,相对于streamingasset文件夹</param>
    /// <param name="_backgroundColor">背景颜色(255为单位)</param>
    /// <param name="_bVisibleOnCreate">是否创建的时候在画布上显示出来</param>
    /// <param name="_bIgnoreMovie">是否加入忽略列表，目前用于按esc隐藏界面功能</param>
    /// <param name="_modeType">movie的缩放方式</param>
    /// <param name="_fromByteStream">从字节流中创建</param>
    /// <param name="_renderTexture">是否使用Rt</param>
    /// <param name="_scaleModeType">缩放模式</param>
    /// <param name="_movieName">电影名称，暂时忽略</param>
    /// <returns></returns>
    public T CreateWindow<T>(string _name, string _fullPatch, Color32 _backgroundColor, bool _bVisibleOnCreate = false, ScaleModeType _modeType = ScaleModeType.SM_ShowAll, bool _fromByteStream = true, RenderTexture _renderTexture = null, string _movieName = null) where T : Movie
    {
        if (GetWindow(_name) != null)
        {
            return GetWindow(_name) as T;
        }

        T newMovie = default(T);


        newMovie = GfxUtil.CreateSwf<T>(this, _fullPatch, _fromByteStream, _backgroundColor, _renderTexture, _modeType, _movieName);

        if (null == newMovie)
        {
            return null;
        }

        SFMovieEx MovieEx = newMovie as SFMovieEx;
        MovieEx.IsVisibleOnCreate = _bVisibleOnCreate;

        windowRegistryList.Add(_name, newMovie);
        return newMovie;
    }


    public void DestroyWindow(string _name)
    {
        Movie destroyMovie = GetWindow(_name);
        if (destroyMovie != null)
        {
            windowRegistryList.Remove(_name);
            RemoveMovieFromIgnoreHitTestList(destroyMovie);
            GameObject.Destroy(destroyMovie.gameObject);
        }
    }


    //获取玩家鼠标操作
    private bool InitHeroCtrl()
    {
        if (MyHeroMouseCtrl == null && EntityFactory.MainHeroID != 0)
        {
            GameObject mainHeroGO = EntityFactory.getEntityGameObjectByID(EntityFactory.MainHeroID);
            if (mainHeroGO == null)
            {
                Trace.LogWarning("获取主英雄失败 英雄ID:" + EntityFactory.MainHeroID);
            }
            else
            {
                MyHeroMouseCtrl = mainHeroGO.GetComponent<MouseCtrl>();
            }
        }
        return MyHeroMouseCtrl == null ? false : true;
    }

    public void UseHeroMouseCtrl()
    {
        if (InitHeroCtrl())
        {
            MyHeroMouseCtrl.switchToState(MouseState.HeroControl);
        }
        else
        {
            Trace.LogWarning("warning get HeroMouseCtrl failed");
        }
    }

    public void UseUIMouseCtrl()
    {
        if (InitHeroCtrl())
        {
            //MyHeroMouseCtrl.switchToState(MouseState.UIControl);
            MyHeroMouseCtrl.switchToState(MouseState.HeroControl);
        }
        else
        {
            Trace.LogWarning("warning get HeroMouseCtrl failed");
        }
    }

    public Movie DoHitTestEx(HitTestType _hitTestType = HitTestType.HitTest_ShapesNoInvisible)
    {

        if (SFMgr == null)
            return null;

        Vector2 MouseLastPos = GetMousePos();

        Movie mouseIsOverUIElement = SFMgr.DoHitTestEx(MouseLastPos.x, MouseLastPos.y, _hitTestType);
        if (null != mouseIsOverUIElement)
        {
            //UI选择屏蔽血条，飘字，头顶名，箭头
            long movieId = mouseIsOverUIElement.GetID();
            if (IgnoreHitTestMovieList.Contains(movieId))
            {
                return null;
            }
        }

        return mouseIsOverUIElement;
    }



    /// <summary>
    /// 添加将要忽略鼠标点击的movie
    /// </summary>
    /// <param name="_movie">要忽略的movie</param>
    public void AddMovieToIgnoreHitTestList(Movie _movie)
    {
        long movieID = _movie.GetID();
        if (movieID == 0)
        {
            Trace.LogWarning("Ignore HitTest movie id is 0,add to list failed");
            return;
        }
        if (!IgnoreHitTestMovieList.Contains(movieID))
        {
            IgnoreHitTestMovieList.Add(movieID);
        }
    }

    /// <summary>
    /// 移除要忽略鼠标点击的movie
    /// </summary>
    /// <param name="_movie">要移除的movie</param>
    public void RemoveMovieFromIgnoreHitTestList(Movie _movie)
    {
        long movieID = _movie.GetID();
        if (movieID == 0)
        {
            return;
        }
        if (IgnoreHitTestMovieList.Contains(movieID))
        {
            IgnoreHitTestMovieList.Remove(movieID);
        }
    }

    public GameObject uigroup = null;

    WaitForSeconds updateIntervel = new WaitForSeconds(0.05f);
    IEnumerator SyncLogicLayerTickCount()
    {
        while (true)
        {
            double nSyncTickCount = Convert.ToDouble(GameLogicAPI.getSyncTickCount());
            GetSFManager().DeltaSyncTickCount = nSyncTickCount - GetSFManager().LastSyncTickCount;
            GetSFManager().LastSyncTickCount = nSyncTickCount;
            GetSFManager().SyncTickCountToAS();
            //每1/4秒同步Tick到AS
            yield return updateIntervel;
        }
    }

    public void InitGameCreateDefaultMovie()
    {
        this.GetSFManager().LastSyncTickCount = Convert.ToDouble(GameLogicAPI.getSyncTickCount());
        StartCoroutine("SyncLogicLayerTickCount");

        //SFGFxMovieManager.Instance.CreateWindow<GFxLoginMenu>("GFxLoginMenu", "GFx/LoginWin.swf", SFGFxMovieManager.DefaultBGColor, true);
        //SFGFxMovieManager.Instance.CreateWindow<GFxHeroCreateWin>("GFxHeroCreateWin", "GFx/HeroCreateWin.swf", SFGFxMovieManager.DefaultBGColor);

        //初始化UI共用界面
        //SFGFxMovieManager.Instance.CreateWindow<GFxBaseHud>("GFxBaseHud", "GFx/BaseHud.swf", SFGFxMovieManager.DefaultBGColor);

        //SFGFxMovieManager.Instance.CreateWindow<GfxDynamicMovie>("GfxDynamicMovie", "GFx/DynamicMovie.swf", SFGFxMovieManager.DefaultBGColor, true,ScaleModeType.SM_ExactFit);

        //SFGFxMovieManager.Instance.CreateWindow<GfxBloodMovie>("GfxBloodMovie", "GFx/BloodMovie.swf", new Color32(0, 0, 0, 0), false,ScaleModeType.SM_ExactFit, true, GfxBloodMovie.BloodRenderTexture);

        //SFGFxMovieManager.Instance.CreateWindow<TopNameMovie>("TopNameMovie", "GFx/TopNameMovie.swf", new Color32(0, 0, 0, 0), false,ScaleModeType.SM_ExactFit, true, TopNameMovie.TopNameRenderTexture);


        //SFGFxMovieManager.Instance.CreateWindow<FloatFontMovie>("FloatFontMovie", "GFx/FloatFontMovie.swf", new Color32(0, 0, 0, 0), false,
        //                                                ScaleModeType.SM_ExactFit, true, FloatFontMovie.FloatFontRenderTexture, null);

        //SFGFxMovieManager.Instance.CreateWindow<FloatFontMovie>("FloatFontMovie", "GFx/FloatFontMovie.swf", new Color32(0, 0, 0, 0));

        //SFGFxMovieManager.Instance.CreateWindow<GFxTooltipHud>("GFxTooltipHud", "GFx/TooltipHud.swf", SFGFxMovieManager.DefaultBGColor);

        // SFGFxMovieManager.Instance.CreateWindow<GFxLoadingWin>("GFxLoadingWin", "GFx/LoadingWin.swf", SFGFxMovieManager.DefaultBGColor);

        //SFGFxMovieManager.Instance.CreateWindow<GFxFloatText>("GFxFloatText", "GFx/FloatText.swf", SFGFxMovieManager.DefaultBGColor);

        //SFGFxMovieManager.Instance.CreateWindow<GFxMapSignal>("GFxMapSignal", "GFx/MapSignal.swf", SFGFxMovieManager.DefaultBGColor);

        //SFGFxMovieManager.Instance.CreateWindow<GFxDragonBall>("GFxDragonBall", "GFx/DragonBall.swf", SFGFxMovieManager.DefaultBGColor);

        //GfxUtil.CaculUIScale();

        //GfxLableManager.Init();
        //GfxBloodManager.Init();
        //TopNameManager.Init();

        //FloatFontManager.Init();

        // SelectedTargetTips.CreateInstance();

    }

    public void CreateMobaMovie()
    {
        bDestroyMobaWindow = false;


        //SFGFxMovieManager.Instance.CreateWindow<GFxGameHud>("GFxGameHud", "GFx/GameHud.swf", SFGFxMovieManager.DefaultBGColor);
        //SetVisible<GFxGameHud>(true);

        //4次左右(第四次开始崩）
        //SFGFxMovieManager.Instance.CreateWindow<GFxMinimap>("GFxMinimap", "GFx/Minimap.swf", SFGFxMovieManager.DefaultBGColor, false);
        //SetVisible<GFxMinimap>(true);


        //3次左右崩(第三次结束就崩)
        //      SFGFxMovieManager.Instance.CreateWindow<GFxTeamRightHud>("GFxTeamRightHud", "GFx/WarTeamRightHud.swf", SFGFxMovieManager.DefaultBGColor, false);
        //SFGFxMovieManager.Instance.CreateWindow<GFxWarUponRightHud>("GFxWarUponRightHud", "GFx/TeamInfoTopRight.swf", SFGFxMovieManager.DefaultBGColor, false);
        //pop up movie
        //3次左右崩(第三次结束就崩)
        //SFGFxMovieManager.Instance.CreateWindow<GFxDragonBall>("GFxDragonBall", "GFx/DragonBall.swf", SFGFxMovieManager.DefaultBGColor);
        //4次左右(第四次一进战场就崩)
        //5次左右(第五次一进战场就崩)
        //SFGFxMovieManager.Instance.CreateWindow<GFxWarPauseMenu>("GFxWarPauseMenu", "GFx/WarGameSetting.swf", SFGFxMovieManager.DefaultBGColor);
        //3次左右(第三次一进战场就崩)
        //SFGFxMovieManager.Instance.CreateWindow<GFxWarGoodsStore>("GFxWarGoodsStore", "GFx/WarGoodsStore.swf", SFGFxMovieManager.DefaultBGColor);
        //4次左右(第四次一进战场就崩）
        //SFGFxMovieManager.Instance.CreateWindow<GFxWarHeroDetailedInfoPanel>("GFxWarHeroDetailedInfoPanel", "GFx/WarHeroDetailedInfoPanel.swf", SFGFxMovieManager.DefaultBGColor);


    }
    public void DestroyMobaMovie()
    {
        //SFGFxMovieManager.SetVisible<GFxMinimap>(false);
        //SFGFxMovieManager.SetVisible<GFxGameHud>(false);



        //SFGFxMovieManager.SetVisible<GFxWarHeroDetailedInfoPanel>(false);

        //StartCoroutine(DelayDestroyMobaWindow());
    }

    public void DestroyWarTable()
    {
        //这里暂时把所有界面隐藏掉先
        //StartCoroutine(DelayDestroyWarTable());
    }

    public float DelayDestroyTime = 2.0f;
    private bool bDestroyMobaWindow;
    private IEnumerator DelayDestroyMobaWindow()
    {
        if (bDestroyMobaWindow)
            yield break;
        //Debug.Log("Destory Minimap");
        //GFxMinimapManager.Instance.DestoryMinimap();
        //yield return new WaitForSeconds(DelayDestroyTime);

        //Debug.Log("Destory GFxGameHud");
        //SFGFxMovieManager.Instance.DestroyWindow("GFxGameHud");
        //yield return new WaitForSeconds(DelayDestroyTime);

        //Debug.Log("Destory GFxDragonBall");
        //SFGFxMovieManager.Instance.DestroyWindow("GFxDragonBall");
        //yield return new WaitForSeconds(DelayDestroyTime);

        //Debug.Log("Destory GFxWarRecordTable");
        //SFGFxMovieManager.Instance.DestroyWindow("GFxWarRecordTable");
        //yield return new WaitForSeconds(DelayDestroyTime);




        //Debug.Log("Destory GFxTeamRightHud");
        //SFGFxMovieManager.Instance.DestroyWindow("GFxTeamRightHud");
        //yield return new WaitForSeconds(DelayDestroyTime);

        //Debug.Log("Destory GFxWarGoodsStore");
        //SFGFxMovieManager.Instance.DestroyWindow("GFxWarGoodsStore");
        //yield return new WaitForSeconds(DelayDestroyTime);

        //Debug.Log("Destory GFxWarPauseMenu");
        //SFGFxMovieManager.Instance.DestroyWindow("GFxWarPauseMenu");
        //yield return new WaitForSeconds(DelayDestroyTime);
        //Debug.Log("Destory GFxWarBottomCenterHud");


        //Debug.Log("Destory GFxWarHeroDetailedInfoPanel");
        //SFGFxMovieManager.Instance.DestroyWindow("GFxWarHeroDetailedInfoPanel");
        //yield return new WaitForSeconds(DelayDestroyTime);


        //Debug.Log("Destory GfxLableManager");
        //GfxLableManager.DestroyALL();
        //yield return new WaitForSeconds(DelayDestroyTime);

        //Debug.Log("Destory GfxBloodManager");
        //GfxBloodManager.DestroyALL();
        //yield return new WaitForSeconds(DelayDestroyTime);

        //Debug.Log("Destory TopNameManager");
        //TopNameManager.DestroyALL();

        //FloatFontManager

        bDestroyMobaWindow = true;
    }

    private IEnumerator DelayDestroyWarTable()
    {
        yield return new WaitForSeconds(DelayDestroyTime);
        //Debug.Log("Destory GFxWarRecordTable");
        //yield return new WaitForSeconds(DelayDestroyTime);
    }

    private bool bDestroyMMOWindows;
    public void CreateMMOMovie()
    {
        bDestroyMMOWindows = false;

        //SFGFxMovieManager.Instance.CreateWindow<GFxMMODefaultHud>("GFxMMODefaultHud", "GFx/MMODefaultHud.swf", SFGFxMovieManager.DefaultBGColor);
        // SFGFxMovieManager.Instance.CreateWindow<GFxGameHallWin>("GFxGameHallWin", "GFx/GameHallWin.swf", SFGFxMovieManager.DefaultBGColor);
        //  SFGFxMovieManager.Instance.CreateWindow<GFxGameCreateingRoomWin>("GFxGameCreateingRoomWin", "GFx/GameCreatingRoomWin.swf", SFGFxMovieManager.DefaultBGColor);
        //if (!Config.UseNewSlot)
        //{
        //    SFGFxMovieManager.Instance.CreateWindow<GFxGameWaitingRoomWin>("GFxGameWaitingRoomWin", "GFx/GameWaitingRoomWin.swf", SFGFxMovieManager.DefaultBGColor);
        //}
        //SFGFxMovieManager.Instance.CreateWindow<GFxGameWaitingRoomWin>("GFxGameWaitingRoomWin", "GFx/GameWaitingRoomWin.swf", SFGFxMovieManager.DefaultBGColor);
        //SFGFxMovieManager.Instance.CreateWindow<GFxGameHighLadder>("GFxGameHighLadder", "GFx/GameHighLadder.swf", SFGFxMovieManager.DefaultBGColor);
        //SFGFxMovieManager.Instance.CreateWindow<GFxSummonerFrame>("GFxSummonerFrame", "GFx/SummonerFrame.swf", SFGFxMovieManager.DefaultBGColor);
    }
    public void DestroyMMOMovie()
    {
        //SFGFxMovieManager.GetWindow<GFxMMODefaultHud>().SetMovieVisible(false);
        // SFGFxMovieManager.GetWindow<GFxGameHallWin>().SetMovieVisible(false);
        // SFGFxMovieManager.GetWindow<GFxGameCreateingRoomWin>().SetMovieVisible(false);
        //if (!Config.UseNewSlot)
        //{
          //  SFGFxMovieManager.GetWindow<GFxGameWaitingRoomWin>().SetMovieVisible(false);
       // }
        //  SFGFxMovieManager.GetWindow<GFxGameWaitingRoomWin>().SetMovieVisible(false);
        //StartCoroutine(DelayDestroyMMOWindow());
    }
    private IEnumerator DelayDestroyMMOWindow()
    {
        if (bDestroyMMOWindows)
            yield break;

        //SFGFxMovieManager.Instance.DestroyWindow("GFxMMODefaultHud");
        //yield return new WaitForSeconds(DelayDestroyTime);

        //Debug.Log("Destory GFxGameHallWin");


        //Debug.Log("Destory GFxGameCreateingRoomWin");
        //Debug.Log("Destory GFxGameWaitingRoomWin");
        //if (!Config.UseNewSlot)
        //{
        //    SFGFxMovieManager.Instance.DestroyWindow("GFxGameWaitingRoomWin");
        //}
        // SFGFxMovieManager.Instance.DestroyWindow("GFxGameWaitingRoomWin");
        //yield return new WaitForSeconds(DelayDestroyTime);

        //Debug.Log("Destory GFxSummonerFrame");

        bDestroyMMOWindows = true;
    }
    #region Unity消息
    void Awake()
    {
#if (UNITY_STANDALONE_WIN || UNITY_STANDALONE_OSX || UNITY_EDITOR) && !UNITY_WP8
        SF_SetKey("SWT8ZXI1296WQ468K0J643JM94RWI0WYKE0B562Z62KOUGM86W64QHFNCYBLOWA");
#elif UNITY_IPHONE
        SF_SetKey("");
#elif UNITY_ANDROID
        SF_SetKey("");
#elif UNITY_WP8
        sf_setKey("");
#endif

        //For GL based platforms - Sets a number to use for Unity specific texture management.  Adjust this number if
        //you start to experience black and/or mssing textures.
#if UNITY_WP8
        sf_setTextureCount(500);
#else
        SF_SetTextureCount(500);
#endif

        //初始化SFManager参数
        InitParams.TheToleranceParams.Epsilon = 1e-5f;
        InitParams.TheToleranceParams.CurveTolerance = 1.0f;
        InitParams.TheToleranceParams.CollinearityTolerance = 10.0f;
        InitParams.TheToleranceParams.IntersectionEpsilon = 1e-3f;
        InitParams.TheToleranceParams.FillLowerScale = 0.0707f;
        InitParams.TheToleranceParams.FillUpperScale = 100.414f;
        InitParams.TheToleranceParams.FillAliasedLowerScale = 10.5f;
        InitParams.TheToleranceParams.FillAliasedUpperScale = 200.0f;
        InitParams.TheToleranceParams.StrokeLowerScale = 10.99f;
        InitParams.TheToleranceParams.StrokeUpperScale = 100.01f;
        InitParams.TheToleranceParams.HintedStrokeLowerScale = 0.09f;
        InitParams.TheToleranceParams.HintedStrokeUpperScale = 100.001f;
        InitParams.TheToleranceParams.Scale9LowerScale = 10.995f;
        InitParams.TheToleranceParams.Scale9UpperScale = 100.005f;
        InitParams.TheToleranceParams.EdgeAAScale = 0.95f;
        InitParams.TheToleranceParams.MorphTolerance = 0.001f;
        InitParams.TheToleranceParams.MinDet3D = 10.001f;
        InitParams.TheToleranceParams.MinScale3D = 10.05f;

        InitParams.UseSystemFontProvider = false;
        base.SFAwake();
        instance = this;
        DontDestroyOnLoad(gameObject);
        Trace.Log("Gfx初始化完成");
        uigroup = new GameObject("uigroup");
        DontDestroyOnLoad(uigroup);
    }

    void Start()
    {

    }


    void Update()
    {
        if (windowRegistryList.Count > 0)
        {
            base.SFUpdate();
            PopupMovieManager.Instance.Updata();
        }
    }



    void OnDestroy()
    {
        //GfxBloodManager.Clear();
        //TopNameManager.Clear();
        PopupMovieManager.Instance.Destroy();
        IgnoreHitTestMovieList.Clear();
        windowRegistryList.Clear();
        StopCoroutine("SyncLogicLayerTickCount");
        base.SFOnDestroy();

        if (uigroup != null)
        {
            GameObject.Destroy(uigroup);
        }
    }


    void OnGUI()
    {
        currentEvent = Event.current;
        if (windowRegistryList.Count > 0)
        {
            base.SFOnGUI();
        }
    }

    void LateUpdate()
    {
        GfxUtil.CaculUIScale();
        //GfxBloodManager.Update();
        //TopNameManager.Update();
        //FloatFontManager.Update();

        SelectedTargetTips.Update();

        if (windowRegistryList.Count > 0)
        {
            base.SFLateUpdate();
        }

    }

    #endregion
}