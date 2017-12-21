using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.Rendering;


public partial class Water : MonoBehaviour
{
    [System.Serializable]
    public class WaterNode
    {
        public BoxCollider hostBoxCollider = null;
        public Renderer render = null;
        public Animator animator = null;
    }

    public bool bBaked = false;

    public List<Renderer> allVisibleRenderList = new List<Renderer>();

    public List<WaterNode> allWaterNode = new List<WaterNode>();

    public List<SceneTextureConfigData> allTextureConfig = new List<SceneTextureConfigData>();

    public GameObject WaterRefObjRoot = null;

    /// <summary>
    /// 水面反射范围
    /// </summary>
    public List<BoxCollider> WaterRefAeraList = new List<BoxCollider>();

    /// <summary>
    /// 水面反射范围根物体
    /// </summary>
    public List<GameObject> WaterRefAeraRootObjList = new List<GameObject>();

    private static Shader WaterRefrctionObjectShader = null;

    private List<ResNode> allResNode = new List<ResNode>();
    /// <summary>
    /// 材质球，多个同样的材质球只需要一个即可
    /// </summary>
    private Dictionary<Material, Material> VisibleRenderInstanceMaterial = new Dictionary<Material, Material>();


    /// <summary>
    /// 水面反射区域与他的GameoObjectRoot
    /// </summary>
    private Dictionary<BoxCollider, GameObject> RefAreaRootObjTable = new Dictionary<BoxCollider, GameObject>();


    private List<Bounds> InstanceBoundsList = new List<Bounds>();
    private List<bool> RefAreaEnabledStateList = new List<bool>();

    /// <summary>
    /// 水物体的层级全部在这
    /// </summary>
    public static int WaterObjLayer = 29;

    /// <summary>
    /// 水裁剪层
    /// </summary>
    static int WaterCullLayer = -1;

    /// <summary>
    /// 优化距离，30
    /// </summary>
    static float OptimitzedDistance = 30;
    public void InitOptimitzed()
    {
        if (!bBaked)
        {
            return;
        }

        if(!WaterRefrctionObjectShader)
        {
            WaterRefrctionObjectShader = AssetBundleManager.GetShader("Scene/Water/WaterRefrctionObject");
            if(!WaterRefrctionObjectShader)
            {
                Debug.LogWarning("找不到水面反射物体shader,关闭水面反射");
                return;
            }
        }


        foreach (Renderer r in allVisibleRenderList)
        {

            Material instanceMat = null;
            if (VisibleRenderInstanceMaterial.TryGetValue(r.sharedMaterial, out instanceMat))
            {
                r.sharedMaterial = instanceMat;
                continue;
            }
            else
            {
                if (GameUtil.isStandardShader(r.sharedMaterial.shader.name))
                {
                    instanceMat = new Material(r.sharedMaterial);
                    instanceMat.name = "WaterRef-" + r.sharedMaterial.name;
                    instanceMat.shader = WaterRefrctionObjectShader;
                    instanceMat.hideFlags = HideFlags.HideAndDontSave;
                }
                else
                {
                    instanceMat = r.sharedMaterial;
                }
                VisibleRenderInstanceMaterial.Add(r.sharedMaterial, instanceMat);
                r.sharedMaterial = instanceMat;
            }
        }

        int cout = WaterRefAeraList.Count;
        for(int i = 0;i < cout;i++)
        {
            RefAreaRootObjTable.Add(WaterRefAeraList[i], WaterRefAeraRootObjList[i]);
            WaterRefAeraList[i].enabled = true;

            Bounds instanceBounds = new Bounds(WaterRefAeraList[i].center, WaterRefAeraList[i].size);
            Vector3 min = WaterRefAeraList[i].transform.localToWorldMatrix.MultiplyPoint(instanceBounds.min);
            Vector3 max = WaterRefAeraList[i].transform.localToWorldMatrix.MultiplyPoint(instanceBounds.max);
            Vector3 center = WaterRefAeraList[i].transform.localToWorldMatrix.MultiplyPoint(instanceBounds.center);

            Vector3 dir = max - center;
            max += dir.normalized * OptimitzedDistance;
            dir = min - center;
            min += dir.normalized * OptimitzedDistance;
            instanceBounds.SetMinMax(min, max);

            RefAreaEnabledStateList.Add(false);
            InstanceBoundsList.Add(instanceBounds);
            WaterRefAeraList[i].enabled = false;
            
        }


        foreach (SceneTextureConfigData cf in allTextureConfig)
        {
            ResNode re = AssetBundleManager.GetAssets(AssetType.Asset_Texture,cf);
            if(null != re)
            {
                allResNode.Add(re);
            }
            m_Renderer.sharedMaterial.SetTexture(cf.ShaderKeyWordName, re.ConvertRes<Texture>());
            
        }
        ResetAllRenders();
        
    }

    public void ResetAllRenders()
    {
        foreach (GameObject go in WaterRefAeraRootObjList)
        {
            go.transform.SetPosition(Vector3.one * 99990);
        }
    }

    private void UpdateOptimiezed(SceneRenderingQuality mode)
    {
        if (!bBaked)
        {
            return;
        }

        if (mode <= SceneRenderingQuality.Fastest)
        {
            return;
        }
        if(!Camera.current)
        {
            return;
        }
        
        Camera cam = Initialize.mainCam;
        if (!cam)
        {
            cam = Camera.current;
            if (!cam)
            {
                return;
            }
        }
        Transform camTrans = cam.transform;
        int cout = WaterRefAeraList.Count;
        for (int i = 0; i < cout; i++)
        {
            Vector3 pos = camTrans.position;
            Bounds instanceBounds = InstanceBoundsList[i];

            float boundsSize = GameUtil.GetSqrDistanceWithOutY(instanceBounds.min, instanceBounds.center);
            float centerDis = GameUtil.GetSqrDistanceWithOutY(instanceBounds.center, pos);


            if (centerDis < boundsSize)
            {
                GameObject go = null;
                if (RefAreaRootObjTable.TryGetValue(WaterRefAeraList[i], out go))
                {
                    go.transform.SetPosition( Vector3.zero);
                }
                RefAreaEnabledStateList[i] = true;
            }
            else
            {
                if(RefAreaEnabledStateList[i])
                {
                    GameObject go = null;
                    if (RefAreaRootObjTable.TryGetValue(WaterRefAeraList[i], out go))
                    {
                        go.transform.SetPosition(Vector3.one * 99990);
                    }
                    RefAreaEnabledStateList[i] = false;
                }
            }
           

        }
    }

    public void ClearOptimitzed()
    {
        if (!bBaked)
        {
            return;
        }

        if (WaterRefrctionObjectShader)
        {
            Resources.UnloadAsset(WaterRefrctionObjectShader);
            WaterRefrctionObjectShader = null;
        }
        if (m_Renderer && m_Renderer.sharedMaterial)
        {
            foreach (SceneTextureConfigData cf in allTextureConfig)
            {

                m_Renderer.sharedMaterial.SetTexture(cf.ShaderKeyWordName, null);

            }
        }

        foreach(ResNode r in allResNode)
        {
            ResNode temp = r;
            AssetBundleManager.DeleteAssets(ref temp,true);
        }
        allResNode.Clear();
        RefAreaEnabledStateList.Clear();
        InstanceBoundsList.Clear();
        RefAreaRootObjTable.Clear();
        VisibleRenderInstanceMaterial.Clear();
    }
}
