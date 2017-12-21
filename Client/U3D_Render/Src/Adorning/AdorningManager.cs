using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using U3D_Render.CameraControl;

/// <summary>
/// 装饰品管理器，负责所有装饰品的调度
/// </summary>
public class AdorningManager : MonoBehaviour
{
    public Hashtable groups = new Hashtable();

    public static Vector3 vmin;
    public static Vector3 vmax;
    public static Vector3 vsize;
    public static Vector3 vcenter;
    public static Vector3 vdistance;

    [HideInInspector]
    public float VisFactor = 0.30f;
    [HideInInspector]
    public float FadeTime = 0.125f;

    public bool FadeEnable=true;
    public bool supportphysic = true;

    public static int groupsize = 3;
    private int GetKeyByGroup(int x, int y)
    {
        int stride = groupsize * 2 + 1;
        int key = (y + groupsize) * stride + (x + groupsize);
        return key;
    }

    private Vector3 CalPosByGroup(int x, int y)
    {
        float deltax=x * vdistance.x;
        float deltaz=y * vdistance.z;
        Vector3 pos = vcenter;
        pos.x += deltax;
        pos.z += deltaz;
        return pos;
    }

    public GameObject CreateGroup(int x, int y)
    {
        int key = GetKeyByGroup(x, y);
        Vector3 pos = CalPosByGroup(x, y);

        string groupname = "group_(" + x.ToString() + "_" + y.ToString() + ")";
        GameObject gogroup = new GameObject(groupname);
        gogroup.transform.SetPosition( pos);
        gogroup.transform.parent = transform;

        return gogroup;
    }

    public AdorningGroup GetGroup(int x, int y)
    {
        int key = GetKeyByGroup(x, y);

        if (groups.ContainsKey(key))
        {
            AdorningGroup ag = (AdorningGroup)groups[key];
            return ag;
        }
        else
        {
            return null;
        }
    }

    public AdorningGroup GetGroup(Vector3 pos)
    {
        Vector3 vrelate = pos - vmin;
        int x = (int)(vrelate.x / vdistance.x);
        int y = (int)(vrelate.z / vdistance.z);

        x -= groupsize;
        y -= groupsize;

        if (x < -groupsize)
            x = -groupsize;
        if (y < -groupsize)
            y = -groupsize;
        if (x > groupsize)
            x = groupsize;
        if (y > groupsize)
            y = groupsize;

        return GetGroup(x, y);
    }

    public static bool IsLoad(Vector3 pos)
    {
        bool ret = false;
        foreach (AdorningManager am in Instances)
        {
            AdorningGroup ag = am.GetGroup(pos);
            if (ag != null)
            {
                if (ag.loaded == true)
                {
                    ret = true;
                    break;
                }
            }
        }
        return ret;
    }

    public static List<AdorningManager> Instances = new List<AdorningManager>();

    public void Build()
    {
        //RenderSettings.fog = false;
        //RenderSettings.fogColor = new Color(112.0f / 255.0f, 127.0f / 255.0f, 131.0f / 255.0f);
        //RenderSettings.fogStartDistance = 25.0f;
        //RenderSettings.fogEndDistance = 125.0f;

        //这里要先加到一个临时列表，因为新生成的groupobj也是当前gameobject.transform的子对象
        List<Transform> objs = new List<Transform>();
        //Renderer[] mrs = transform.GetComponentsInChildren<Renderer>();
        //if (mrs == null)
        //{
        //    return;
        //}
        //int n = mrs.Length;
        //for (int i = 0; i < n; i++)
        //{
        //    Transform tchild = mrs[i].transform;
        //    objs.Add(tchild);
        //}

        int n = transform.childCount;
        for (int i = 0; i < n; i++)
        {
            Transform tchild = transform.GetChild(i);
            if (tchild.lossyScale.x > 0.1f && tchild.lossyScale.z > 0.1f)//对于缩放系数为负的物件只能整体处理，unity对于缩放为负处理可能有bug
            {
                Renderer[] mrs = tchild.GetComponentsInChildren<Renderer>();
                foreach (Renderer mr in mrs)
                {
                    objs.Add(mr.transform);
                }
            }
            else
            {
                objs.Add(tchild);
            }
        }
        
        //对临时列表中的对象进行分组
        Hashtable groupobjs = new Hashtable();
        for (int i = 0; i < objs.Count; i++)
        {
            Transform tchild = objs[i];
            Vector3 pos = tchild.position;
            MeshRenderer mr = tchild.gameObject.GetComponent<MeshRenderer>();
            if (mr != null && mr.bounds!=null)
            {
                pos = mr.bounds.center;
            }
            Vector3 vrelate = pos - vmin;
            int x = (int)(vrelate.x / vdistance.x);
            int y = (int)(vrelate.z / vdistance.z);

            x -= groupsize;
            y -= groupsize;

            if (x < -groupsize)
                x = -groupsize;
            if (y < -groupsize)
                y = -groupsize;
            if (x > groupsize)
                x = groupsize;
            if (y > groupsize)
                y = groupsize;

            GameObject gogroup = null;
            int key = GetKeyByGroup(x, y);
            if (!groupobjs.ContainsKey(key))
            {
                gogroup = CreateGroup(x, y);
                groupobjs[key] = gogroup;
            }
            else
            {
                gogroup = (GameObject)groupobjs[key];
            }
   
            tchild.parent = gogroup.transform;
        }

        // 对分组对象构建AdorningGroup脚本
        foreach (DictionaryEntry keypair in groupobjs)
        {
            int key = (int)keypair.Key;
            GameObject gogroup = (GameObject)keypair.Value;

            int stride = groupsize * 2 + 1;
            int x = key % stride;
            int y = key / stride;

            AdorningGroup ag = gogroup.AddComponent<AdorningGroup>();
            ag.x = x - groupsize;
            ag.y = y - groupsize;
            ag.pos = gogroup.transform.position;
            groups[key] = ag;
        }

        // 合并每个分组中的网格，减少批次
        foreach (DictionaryEntry keypair in groups)
        {
            int key = (int)keypair.Key;
            AdorningGroup ag = (AdorningGroup)keypair.Value;
            //ag.Combine();
        }

        // 加到列表
        foreach (DictionaryEntry keypair in groups)
        {
            int key = (int)keypair.Key;
            AdorningGroup ag = (AdorningGroup)keypair.Value;
            GameObject gogroup = ag.gameObject;

            for (int i = 0; i < gogroup.transform.childCount; i++)
            {
                Transform tchild = gogroup.transform.GetChild(i);
                Adorning adorning = tchild.gameObject.AddComponent<Adorning>();
                adorning.alpha = 2.0f;
                ag.objs.Add(adorning);
            }
        }

        //最后处理没分组的
        List<Transform> ungrouplist = new List<Transform>();
        for (int i = 0; i < transform.childCount; i++)
        {
            Transform tchild = transform.GetChild(i);
            if (tchild.gameObject.GetComponent<AdorningGroup>() == null)
            {
                ungrouplist.Add(tchild);
            }
        }
        GameObject ungroupparent = new GameObject("ungroup");
        ungroupparent.transform.parent = transform;
        foreach (Transform tchild in ungrouplist)
        {
            tchild.parent = ungroupparent.transform;
        }

    }

    public void Update()
    {
        GameObject gohero = EntityFactory.MainHero;
        if (gohero != null)
        {
            Vector3 vhero = gohero.transform.position;
            if (SoldierCamera.MainInstance<SoldierCamera>() != null && SoldierCamera.MainInstance<SoldierCamera>().cameraMode == CameraMode.MapGodControl)
            {
                vhero = CameraUtil.GetGodEyePos();
            }
            Vector3 vtotallen = vsize;
            vtotallen.y = 0.0f;
            float ftotallen = vtotallen.magnitude;
            float fvislen = ftotallen * VisFactor;

            foreach (DictionaryEntry keypair in groups)
            {
                int key = (int)keypair.Key;
                AdorningGroup ag = (AdorningGroup)keypair.Value;

                Vector3 vdir = vhero - ag.pos;
                vdir.y = 0.0f;
                float flen = vdir.magnitude;

                bool bVis = flen < fvislen;
                ag.SetVisable(bVis, 0.1f,FadeTime,FadeEnable);
            }
        }
    }

    public static string[] scenegroupnames = new string[] { "ScenesMeshs", "ScenesEffects", "ScenesTree", "ScenesEffect", "ScenesStone" };

    public static void Init()
    {
        vmin = new Vector3(9999999.0f, 9999999.0f, 9999999.0f);
        vmax = -vmin;

        foreach (string scenegroupname in scenegroupnames)
        {
            GameObject goscene = GameObject.Find(scenegroupname);
            if (goscene != null)
            {
                Renderer[] rs = goscene.GetComponentsInChildren<Renderer>();
                //int n = goscene.transform.childCount;
                //for (int i = 0; i < n; i++)
                foreach(Renderer r in rs)
                {
                    //Transform tchild = goscene.transform.GetChild(i);
                    Vector3 pos = r.transform.position;
                    MeshRenderer mr = r.gameObject.GetComponent<MeshRenderer>(); //要有中心轴的一定是meshrender，粒子系统有renderer却非meshrender
                    if (mr != null && mr.bounds != null)
                    {
                        pos = mr.bounds.center;
                    }

                    if (pos.x > vmax.x)
                        vmax.x = pos.x;
                    if (pos.y > vmax.y)
                        vmax.y = pos.y;
                    if (pos.z > vmax.z)
                        vmax.z = pos.z;

                    if (pos.x < vmin.x)
                        vmin.x = pos.x;
                    if (pos.y < vmin.y)
                        vmin.y = pos.y;
                    if (pos.z < vmin.z)
                        vmin.z = pos.z;
                }
            }
        }

        vcenter = (vmax + vmin) * 0.5f;
        vmax = (vmax - vcenter) * 1.1f + vcenter;
        vmin = (vmin - vcenter) * 1.1f + vcenter;
        vsize = vmax - vmin;
        float groupstride = (1 + groupsize * 2);
        vdistance = vsize / groupstride;

        Instances.Clear();
        foreach(string scenegroupname in scenegroupnames)
        {
            GameObject goscene = GameObject.Find(scenegroupname);
            if (goscene != null)
            {
                AdorningManager am = goscene.GetComponent<AdorningManager>();
                if (am == null)
                {
                    am=goscene.AddComponent<AdorningManager>();
                    am.Build();
                }
                if (am!= null)
                {
                    Instances.Add(am);
                }
                if (scenegroupname == "ScenesEffects" || scenegroupname == "ScenesEffect")
                {
                    am.FadeEnable = false;
                }
                if (scenegroupname == "ScenesEffects" || scenegroupname == "ScenesEffect" || scenegroupname == "ScenesTree")
                {
                    am.VisFactor = 0.25f;
                    am.supportphysic = false;
                }
                else
                {
                    am.VisFactor = 0.35f;
                    am.supportphysic = true;
                }
            }
        }

    }

    #region 物理破坏

    List<AdorningGroup> animedgroups = new List<AdorningGroup>();

    public void BreakPhysicsSingle()  //中断物理破坏
    {
        if (!supportphysic)
        {
            return;
        }

        foreach (AdorningGroup ag in animedgroups)
        {
            ag.BreakPhysics();
        }
        animedgroups.Clear();
    }



    public void ApplyPhysicsSingle(Vector3 center, float range, Vector3 towards)  //应用物理破坏
    {
        if (!supportphysic)
        {
            return;
        }

        Vector3 vtotallen = vsize;
        vtotallen.y = 0.0f;
        float ftotallen = vtotallen.magnitude;

        foreach (DictionaryEntry keypair in groups)
        {
            int key = (int)keypair.Key;
            AdorningGroup ag = (AdorningGroup)keypair.Value;

            Vector3 vdir = center - ag.pos;
            vdir.y = 0.0f;
            float flen = vdir.magnitude;

            bool bOK = flen < range;
            if (bOK)
            {
                ag.ApplyPhysics(center,range,towards);
                animedgroups.Add(ag);
            }
        }
    }

    public static void BreakPhysics()
    {
        foreach (AdorningManager am in Instances)
        {
            am.BreakPhysicsSingle();
        }
    }

    public static void ApplyPhysics(Vector3 center, float range, Vector3 towards)
    {
        foreach (AdorningManager am in Instances)
        {
            am.ApplyPhysicsSingle(center,range,towards);
        }
    }

    #endregion
}

