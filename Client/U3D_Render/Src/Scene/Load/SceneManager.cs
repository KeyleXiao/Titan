using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

public partial class SceneManager : MonoBehaviour
{
    static public float MAX_SCENE_SIZE=1000.0f; //场景边界坐标最大值，用于判断位置合法性

	/// <summary>
	/// 当前场景名称
	/// </summary>
	private string m_SceneName = "";

    public int m_curMapID = 0;

    public Vector3 m_vecBornPos = Vector3.zero;

	/// <summary>
	/// 场景加载队列
	/// </summary>
    private Queue<string> m_LoadList = new Queue<string>();
	/// <summary>
	/// 下一个场景要加载的资源包Id列表
	/// </summary>
	//private List<int> m_iNextPackIdList = new List<int>();

	public static SceneManager Instance = null;

    /// <summary>
    /// 是否在加载场景
    /// </summary>
    public static bool isLoadingScene
    {
        get
        {
            return m_isLoadingScene;
        }
        set
        {
            AssetBundleManager.isLoadingScene = value;
            m_isLoadingScene = value;
        }
    }
    private static bool m_isLoadingScene = false;
    

	/// <summary>
	/// 场景加载异步接口
	/// </summary>
	private AsyncOperation m_AsyncOperation = null;

	/// <summary>
	/// 加载场景进度
	/// </summary>
	private int m_nProgress = -1;
    private float m_fSmoothProgress = -1.0f;
    long sceneLoadBegainTime = 0;


    private WaitForSeconds section = new WaitForSeconds(1.5f);
    private WaitForEndOfFrame endofframe = new WaitForEndOfFrame();

    /// <summary>
    /// 服务器等待时间,2秒是延迟
    /// </summary>
    private static float serverWaitngTime = 90 - 2;
    private double currentLoadingTime = 0;
	public void Awake()
    {
		Instance = this;
	}

    public  void Update()
    {
        if (m_nProgress >= 100 || m_nProgress <= 0)
        {
            m_fSmoothProgress = m_nProgress;
        }
        else
        {
            m_fSmoothProgress = m_fSmoothProgress * 0.9f + m_nProgress * 0.1f;
        }

        if (m_nProgress > 100 || m_nProgress < 0)
        {
            if (m_LoadList.Count > 0)
            {
                string scenename = m_LoadList.Dequeue();
                DirectLoadScene(scenename);
                return;
            }
        }

    }

	//场景加载完成
	public bool LoadFinish()
	{
		return m_nProgress>100 && m_LoadList.Count<=0;
	}
		
	//获取场景加载进度
	public float GetLoadProgress()
	{
		return (float)m_nProgress / 100.0f;
	}

    //获取平滑的场景加载进度
    public float GetSmoothProgress()
    {
        return m_fSmoothProgress / 100.0f;
    }
		
    // 加载场景 场景静态ID
    public bool LoadScene(string scenename, int mapID, Vector3 BornVector)
    {
        if (m_SceneName == scenename ||
            string.IsNullOrEmpty(scenename))
		{
			return true;
		}

        m_curMapID = mapID;
        m_vecBornPos = BornVector;
		m_LoadList.Enqueue (scenename);
		return true;
    }

    // 获得当前场景id
    public int getCurrentMapId()
    {
        return m_curMapID;
    }

    // 场景是否加载好
    public bool IsLoaded(string scenename)
    {
        return m_SceneName == scenename;
    }
		
	//直接加载场景
	private bool DirectLoadScene(string scenename)
	{
		m_AsyncOperation = null;

        PlayerPrefs.SetString("mapname", scenename);
        m_SceneName = scenename;

		StartCoroutine(DirectLoadSceneEnumerator());

		return true;
	}

			
	private void AdvanceProgress(int progress)
	{
		m_nProgress = progress;
        if (m_nProgress >= 100 || m_nProgress <= 0)
        {
            m_fSmoothProgress = m_nProgress;
        }
        else
        {
            m_fSmoothProgress = m_fSmoothProgress * 0.9f + m_nProgress * 0.1f;
        }

        InvokeSceneProgressEvent((float)progress / 100.0f);
	}

	private IEnumerator DirectLoadSceneEnumerator()
	{
		AdvanceProgress (0);
        yield return endofframe;

        currentLoadingTime = 0;
        OnStart();


        yield return Worker_Ready();

        yield return Worker_Scene();

        yield return Worker_SceneTexture();

        yield return Worker_Caching();

        yield return Worker_Finish();

        OnFinish();

        yield return endofframe;
        yield return endofframe;

		AdvanceProgress (100);
        yield return endofframe;

		m_nProgress = 99999;
	}

    // 当前场景名称
    public string SceneName
    {
        get { return m_SceneName; }
    }
		

	public void OnStart()
	{
        InvokeSceneLoadStartEvent(m_SceneName);

        //ImageEffects.ImageEffectManager.OnSceneLoadBegin();
        NatureManager.Instance.DestroyAll();
        sceneLoadBegainTime = DateTime.Now.Ticks;
        Debug.Log("SceneLoad::DoTask 开始加载场景加载，场景名称 = " + m_SceneName);
	}

    public void OnFinish()
	{
        double loadSceneFinishTime = DateTime.Now.Ticks;
        ResourceEx.Collect();

        AdorningManager.Init(); 
        GrassAdorningManager.Init();

        ImageSetting.OnSceneLoadFinish();
        ImageSetting.Apply(true);
        //ImageSetting.ApplySceneImageSetting(SceneResInfoManager.CurrentActiveInstance, true);
        LightingEffectFactory.DisableDeadEffect();

        // 场景地图资源加载完后开始播放背景音乐
        //SoundManager.CreateUISound(StageManager.Instance.getSoundID());
        

        if (SoldierCamera.MainInstance() != null)
        {
            SoldierCamera.MainInstance<SoldierCamera>().updateCameraParam(m_curMapID, -1);
        }

        loadSceneFinishTime = DateTime.Now.Ticks - loadSceneFinishTime;
        loadSceneFinishTime *= 0.0000001f;
        Debug.Log("场景加载完成阶段，耗时:" + loadSceneFinishTime);


        double time = DateTime.Now.Ticks - sceneLoadBegainTime;
        time *= 0.0000001f;
        Debug.Log("SceneLoad::DoTask 资源加载成功，场景名称 = " + m_SceneName + "，耗时:" + time);

        InvokeSceneLoadFinishEvent(m_SceneName);


        StartCoroutine(WaitTransitionFinish());

        InvokeSceneTransitionStartEvent(m_SceneName);

	}

    IEnumerator WaitTransitionFinish()
    {

        while (!EntityFactory.MainHero)
        {
            yield return endofframe;
        }
        yield return section;

        InvokeSceneTransitionFinshEvent(m_SceneName);
    }
}



