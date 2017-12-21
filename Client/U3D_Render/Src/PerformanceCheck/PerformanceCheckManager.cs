/*******************************************************************
** 文件名:	PerformanceCheckManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:   性能检测脚本
** 应  用:  	跟随物体

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using ASpeedGame.Data.PlayerSettingLocalConfig;
using UnityEngine.UI;

public enum CheckFlow
{
    None,

    //WaitingForUserSelect,
    HardWareSupport,
    GeometricRender,

    Finish,
}

[System.Serializable]
public class FlowShareData
{
    public List<MeshRenderer> OpaqueModelList = new List<MeshRenderer>();
    public List<MeshRenderer> TransparentModelList = new List<MeshRenderer>();
    public List<SkinnedMeshRenderer> SkinnedMeshModelList = new List<SkinnedMeshRenderer>();
    public List<GameObject> SkinnedObjList = new List<GameObject>();
    public List<PerformanceCheckImageEffect> ImageEffectList = new List<PerformanceCheckImageEffect>();



    public Light shareSunLight = null;

    [System.NonSerialized]
    public PerformanceCheck_UI CheckUI = null;

    [System.NonSerialized]
    public int oldAsyncCout = 0;
    [System.NonSerialized]
    public int oldTargetFrame = 0;
    [System.NonSerialized]
    public Camera shareCamera = null;

    [System.NonSerialized]
    public int ScreenPixels = 0;
    [System.NonSerialized]
    public int currentCheckTime = 0;
    [System.NonSerialized]
    public bool StartCheckTimer = false;
}

public partial class PerformanceCheckManager : MonoBehaviour
{
    public static int MaxOpaqueModelCount = 100;
    public static int MaxTransparentModelCount = 700;
    public static int MaxSkinnedModelCount = 20;
    public static int MaxImageEffectCout = 40;

    //public static int MaxOpaqueModelCount = 150;
    //public static int MaxTransparentModelCount = 700;
    //public static int MaxSkinnedModelCount = 40;
    //public static int MaxImageEffectCout = 50;

    public static float LogicUpdateTime = 0.003f;//逻辑帧更新的时间
    public static int HoldDuringCheckTime = 15;

    public static int PassLevleFps = 45;
    public static string confingFileName = "PerformanceCheckInfo.csv";

    public static PerformanceCheckManager Instance = null;
    public static ImageQuality BreakCheckQuality = ImageQuality.Fastest;

    public FlowShareData flowShareData = new FlowShareData();

    private Dictionary<CheckFlow, PerformanceCheckFlowBase> FlowTable = new Dictionary<CheckFlow, PerformanceCheckFlowBase>();
    public CheckFlow currFlow = CheckFlow.None;
    private PerformanceCheckFlowBase currFlowInstance = null;
    private const float updateTime = 1.0f;
    private WaitForSeconds updateIntervel = new WaitForSeconds(updateTime);

    private string[] TransparentShaderArray = new string[] { "Effect/Particles/Hard-Additive", "Effect/Particles/Alpha Blended_Mask", "Effect/Other/AlphaRim" ,
                                                             "Effect/FlowLight/Standard (Specular setup)","Effect/Distortion/CullOff" ,"Effect/Dissolve/Advance"};
    private string[] OpaqueShaderArray = new string[] { "Scene/Optitmized/Standard_Optitmized", "Scene/Optitmized/LightingMap/Standard_Optitmized",
                                                        "Scene/Optitmized/Nature/SimpleVegetation_LM_CullOff"};
   

    private long LogicUpdateTicks = 0;

    public static int currPerformanceCheckVersion = 1;
    public static string LoginSceneName = "Login";
    static System.Threading.Mutex mutex = null;
    
    ResNode UIResNode = null;
    public void Start()
    {
        if(!CheckMultclient())
        {
            
            Application.Quit();
            return;
        }

        if (!CkeckUpdateOk())
        {
            Application.Quit();
            return;
        }
        Instance = this;
        Initialize.m_WindowHwnd = GameLogicAPI.getMainWindHwnd();
        AssetBundleManager.Init();


        UIResNode = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, "f8aafccc7fff6e34490e09b186458dd8", true);

        if (ResNode.isNullOrEmpty(UIResNode))
        {
            Debug.LogError("找不到性能检测UI资源,以最低配置运行");
            if (AssetBundleManager.isCanLoadLevel(LoginSceneName))
            {
                AssetBundleManager.LoadLevel(LoginSceneName);
            }


            return;
        }
        flowShareData.CheckUI = UIResNode.InstanceMainRes<PerformanceCheck_UI>();
        if (!flowShareData.CheckUI)
        {
            Debug.LogError("找不到性能检测UI,以最低配置运行");
            if (AssetBundleManager.isCanLoadLevel(LoginSceneName))
            {
                AssetBundleManager.LoadLevel(LoginSceneName);
            }
            return;
        }

        ImageEffects.ImageEffectManager.API_ResetSceneBrightness();

        FlowTable.Add(CheckFlow.HardWareSupport, new PerformanceCheckFlow_HardWareSupport());
        FlowTable.Add(CheckFlow.GeometricRender, new PerformanceCheckFlow_RenderingLevel());
        FlowTable.Add(CheckFlow.Finish, new PerformanceCheckFlow_Finish());
        ViewConfigManager.SetDefaultViewConfigPath();

        if (flowShareData.CheckUI)
            flowShareData.CheckUI.Init();

        //本地已经有这个配置了，直接进游戏
        if (GetPerformanceCheckVersion() == currPerformanceCheckVersion)
        {
            if (AssetBundleManager.isCanLoadLevel(LoginSceneName))
            {
                AssetBundleManager.LoadLevel(LoginSceneName);
            }
        }
        else
        {         
            InitData();
            if (flowShareData.CheckUI)
                flowShareData.CheckUI.StartCheck();

            ChangeFlow(CheckFlow.HardWareSupport);
            StartCoroutine(RunPerformanceCheck());
        }
    }

    public void Update()
    {
        //模拟逻辑真更新时间
        LogicUpdateTicks = System.DateTime.Now.Ticks;
        while(true)
        {
            double sec = GameUtil.TicksToSecond(System.DateTime.Now.Ticks - LogicUpdateTicks);
            if(sec > LogicUpdateTime)
            {
                break;
            }
        }
    }

    private void InitData()
    {
        
        Camera m_Camera = Camera.main;
        flowShareData.shareCamera = m_Camera;
        RenderSettings.ambientMode = UnityEngine.Rendering.AmbientMode.Flat;
        RenderSettings.ambientSkyColor = Color.white;
        //int ScreenPixels = Mathf.FloorToInt(Screen.width * Screen.height / 4f);
        //float[] ScreenWidthPoints = new float[ScreenPixels];
        //float[] ScreenHeightPoints = new float[ScreenPixels];
        //for (int i = 0; i < ScreenPixels; i++)
        //{
        //    ScreenWidthPoints[i] = HaltonSequence(i, 2);
        //    ScreenHeightPoints[i] = HaltonSequence(i, 3);
        //}

        int posIndex = 0;
        int cout = flowShareData.OpaqueModelList.Count;

        for (int i = 0; i < cout; i++)
        {
            MeshRenderer re = flowShareData.OpaqueModelList[i];
          
            string shaderName = OpaqueShaderArray[i % OpaqueShaderArray.Length];
            if (shaderName.Contains("LightingMap"))
            {
                re.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
            }
            Material mat = new Material(re.sharedMaterial);
            mat.shader = AssetBundleManager.GetShader(shaderName);
            mat.color = new Color(Random.Range(0.0f, 1.0f), Random.Range(0.0f, 1.0f), Random.Range(0.0f, 1.0f));
            mat.SetColor("_EmissionColor", re.material.color);
            re.sharedMaterial = mat;
            Vector3 pos = Vector3.zero;
            pos.x = 0.5f;
            pos.y = 0.5f;
            pos.z = Random.Range(m_Camera.nearClipPlane + 15, m_Camera.nearClipPlane + 100);

            re.transform.position = m_Camera.ViewportToWorldPoint(pos);
        }
        posIndex += flowShareData.OpaqueModelList.Count; 
        cout = flowShareData.TransparentModelList.Count;

        for (int i = 0; i < cout; i++)
        {
            MeshRenderer re = flowShareData.TransparentModelList[i];
            Material mat = new Material(re.sharedMaterial);
            mat.shader = AssetBundleManager.GetShader(TransparentShaderArray[i % TransparentShaderArray.Length]);
            mat.color = new Color(Random.Range(0.0f, 1.0f), Random.Range(0.0f, 1.0f), Random.Range(0.0f, 1.0f));
            mat.SetColor("_EmissionColor", re.material.color);
            re.sharedMaterial = mat;
            Vector3 pos = Vector3.zero;
            pos.x = 0.5f;
            pos.y = 0.5f;
            pos.z = Random.Range(m_Camera.nearClipPlane + 15, m_Camera.nearClipPlane + 100);
            re.transform.position = m_Camera.ViewportToWorldPoint(pos);
        }
        posIndex += flowShareData.TransparentModelList.Count; 
        cout = flowShareData.SkinnedMeshModelList.Count;

        cout = flowShareData.SkinnedObjList.Count;

        for (int i = 0; i < cout; i++)
        {
            Vector3 pos = Vector3.zero;
            pos.x = 0.5f;
            pos.y = 0.5f;
            pos.z = Random.Range(m_Camera.nearClipPlane + 15, m_Camera.nearClipPlane + 100);
            flowShareData.SkinnedObjList[i].transform.position = m_Camera.ViewportToWorldPoint(pos);

        }
       
    }

    private float HaltonSequence(int index, int b)
    {
        float res = 0f;
        float f = 1f / b;
        int i = index;
        while (i > 0)
        {
            res = res + f * (i % b);
            i = Mathf.FloorToInt(i / b);
            f = f / b;
        }
        return res;
    }

    public void ChangeFlow(CheckFlow newFlow)
    {
        if(currFlow == newFlow)
        {
            return;
        }
        PerformanceCheckFlowBase temp = null;
        if(currFlow != CheckFlow.None)
        {
            if (FlowTable.TryGetValue(currFlow, out temp))
            {
                temp.OnLeave();
            }
            else
            {
                Debug.LogError("找不到flow:" + newFlow);
            }
        }

        if (newFlow != CheckFlow.None)
        {
            if (FlowTable.TryGetValue(newFlow, out temp))
            {
                temp.OnEnter();
            }
            else
            {
                Debug.LogError("找不到flow:" + newFlow);
            }
        }

        currFlow = newFlow;
        currFlowInstance = temp;
    }

    public void OnDestroy()
    {
        AssetBundleManager.DeleteAssets(ref UIResNode, true);
        foreach(PerformanceCheckFlowBase temp in FlowTable.Values)
        {
            temp.Destroy();
        }
        FlowTable.Clear();
        Instance = null;
    }

    IEnumerator RunPerformanceCheck()
    {
        while(true)
        {
            yield return updateIntervel;
            if (null != currFlowInstance)
            {
                currFlowInstance.OnUpdate(updateTime);

                if (flowShareData.StartCheckTimer)
                {
                    flowShareData.currentCheckTime--;
                    if (flowShareData.currentCheckTime <= 0)
                    {
                        flowShareData.currentCheckTime = 0;
                    }

                    flowShareData.CheckUI.OnUpdate();

                }
            }
        }
    }

    private int GetPerformanceCheckVersion()
    {
        int version = -1;
        string confingFileNamePath = ViewConfigManager.GetConfigFullPath(PerformanceCheckManager.confingFileName, false);
        if(!System.IO.File.Exists(confingFileNamePath))
        {
            return version;
        }

        string stSchemeAllText = System.IO.File.ReadAllText(confingFileNamePath);
        //去掉\n,因为ScpReader以"\r"分割
         stSchemeAllText = stSchemeAllText.Replace("\n", "");
        //再干掉最后的"\r"
        stSchemeAllText = stSchemeAllText.Remove(stSchemeAllText.Length - 1, 0);
        ScpReader packageReader = new ScpReader(stSchemeAllText, "per", 0);
        version = packageReader.GetInt(0, 1, -1);

        return version;
    }


    private bool CheckMultclient()
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
                mutex.Close();
                ret = true;
              
            }
            else
            {
                GameUtil.MsgBox(System.IntPtr.Zero, "游戏已经运行!", "错误", 0x00000000 | 0x00000010);
                ret = false;
            }
            return ret;
        }
        return true;
    }

    private bool CkeckUpdateOk()
    {
        if (!Application.isEditor && !System.IO.File.Exists("updateok.txt"))
        {
            List<string> argList = new List<string>();
            argList.AddRange(System.Environment.GetCommandLineArgs());
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

            if (!isUpdateOk)
            {
                GameUtil.MsgBox(System.IntPtr.Zero, "请通过启动器运行游戏!", "错误", 0x00000000 | 0x00000010);
                isUpdateOk = false;
            }
            return isUpdateOk;
        }

        return true;
    }
}



