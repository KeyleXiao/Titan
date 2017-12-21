/*******************************************************************
** 文件名:	SceneResInfoManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息 管理器
** 应  用:   场景资源信息，用于做场景资源管理

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public partial class SceneResInfoManager : MonoBehaviour
{
    private static List<MeshRenderer> currColliderMeshRender = new List<MeshRenderer>();

    private static Dictionary<int, Material> Layer_colliderMeshMaterial = new Dictionary<int,Material>();

    public static bool ShowMeshColliderFlags = true;

    public static void ShowMeshCollider()
    {
        foreach(MeshRenderer re in currColliderMeshRender)
        {
            if(!re)
            {
                continue;
            }
            re.enabled = true;
        }

        foreach (SceneResInfoManager instance in SceneResInfoManager.AllInstance)
        {
            if (instance && instance.SceneRoot)
            {
                foreach (SceneResInfo info in instance.ResInfoList)
                {
                    if (info)
                    {
                        info.hostRender.gameObject.layer = LayerMask.NameToLayer(Config.LayerUI);
                    }
                }

                foreach (SceneResInfo info in instance.GrassResInfoList)
                {
                    if (info)
                    {
                        info.hostRender.gameObject.layer = LayerMask.NameToLayer(Config.LayerUI);
                    }
                }
            }
        }
    }

    public static void HideMeshCollider()
    {
        foreach (MeshRenderer re in currColliderMeshRender)
        {
            if (!re)
            {
                continue;
            }
            re.enabled = false;
        }

        foreach (SceneResInfoManager instance in SceneResInfoManager.AllInstance)
        {
            if (instance && instance.SceneRoot)
            {
                foreach (SceneResInfo info in instance.ResInfoList)
                {
                    if (info)
                    {
                        info.hostRender.gameObject.layer = info.OrginalLayer;
                    }
                }

                foreach (SceneResInfo info in instance.GrassResInfoList)
                {
                    if (info)
                    {
                        info.hostRender.gameObject.layer = info.OrginalLayer;
                    }
                }
            }
        }
    }

    private static Material GetLayerMaterial(int layer)
    {
        Material mat = null;
        if(Layer_colliderMeshMaterial.TryGetValue(layer,out mat))
        {
            if(!mat)
            {
                mat = new Material(AssetBundleManager.GetShader("Scene/Unlit/Texture"));
                float c = layer / 255f;
                mat.SetColor("_Color", c * 20 * Color.white);
            }
        }
        else
        {
            mat = new Material(AssetBundleManager.GetShader("Scene/Unlit/Texture"));
            float c = layer / 255f;
            mat.SetColor("_Color", c * 20 * Color.white);
            Layer_colliderMeshMaterial.Add(layer,mat);
        }
        return mat;
    }

    public static void RefershAllCollider()
    {
        currColliderMeshRender.Clear();
        foreach(SceneResInfoManager instance in SceneResInfoManager.AllInstance)
        {
            if(instance && instance.SceneRoot)
            {

                Collider[] allColliders = instance.SceneRoot.GetComponentsInChildren<Collider>();
                foreach (Collider col in allColliders)
                {
                    RefershCollider(col);
                }
                foreach(SceneResInfo info in instance.ResInfoList)
                {
                    if(info)
                    {
                        info.UpdateCollider();
                    }
                }

                foreach (SceneResInfo info in instance.GrassResInfoList)
                {
                    if (info)
                    {
                        info.UpdateCollider();
                    }
                }
                allColliders = null;
            }
        }


    }

    public static Collider RefershCollider(Collider col)
    {
        Collider reslut = null;
        reslut = RefershMeshCollider(col as MeshCollider);
        if (!reslut)
        {
            reslut = RefershBoxCollider(col as BoxCollider);
            if (!reslut)
            {
                reslut = RefershSphereCollider(col as SphereCollider);
            }
        }

        if(reslut)
        {
           // Debug.Log("colliderLayer:" + LayerMask.LayerToName(reslut.gameObject.layer) + ",colliderName:" + reslut.gameObject.name + ",collderEnabled:" + reslut.enabled);
        }
        return reslut;
    }


    private static Collider RefershMeshCollider(MeshCollider msh)
    {
        if(!msh)
        {
            return msh;
        }

        if (msh.sharedMesh)
        {
            if (msh.sharedMesh.isReadable)
            {
                Mesh mesh = msh.sharedMesh;

                msh.sharedMesh = null;
                string name = msh.gameObject.name;
                Transform parent = msh.gameObject.transform.parent;
                Vector3 localPosition = msh.gameObject.transform.localPosition;
                Quaternion localRoation = msh.gameObject.transform.localRotation;
                Vector3 localScale = msh.gameObject.transform.localScale;
                int layer = msh.gameObject.layer;

                GameObject go = msh.gameObject;

                SceneInfo info =go.GetComponent<SceneInfo>(); 
                if(info)
                {
                    Debug.LogWarning(go.name + "碰撞体上不能有:" + info.GetType() + "组件");
                    return msh;

                }

                GameObject.DestroyImmediate(go);

                go = new GameObject();
                go.name = name;
                go.transform.parent = parent;
                go.transform.localPosition = localPosition;
                go.transform.localRotation = localRoation;
                go.transform.localScale = localScale;
                go.layer = layer;
                go.AddComponent<MeshFilter>().sharedMesh = mesh;
                MeshRenderer re = go.AddComponent<MeshRenderer>();
                re.enabled = ShowMeshColliderFlags;
                re.sharedMaterial = GetLayerMaterial(go.layer);
                currColliderMeshRender.Add(re);

                msh = go.AddComponent<MeshCollider>();
                msh.sharedMesh = mesh;
               // Debug.Log("刷新MeshCollider:" + ",triangles:" + msh.sharedMesh.triangles.Length + ",vertexCout:" + msh.sharedMesh.vertexCount);
            }
            else
            {
                Debug.LogWarning("not readable:" + (msh.transform.parent ? msh.transform.parent.name : msh.gameObject.name));
            }
        }
        else
        {
            Debug.LogWarning("missing Mesh:" + (msh.transform.parent ? msh.transform.parent.name : msh.gameObject.name));
        }
        return msh;
    }

    private static Collider RefershSphereCollider(SphereCollider msh)
    {
        if(!msh)
        {
            return msh;
        }
        Vector3 center = msh.center;
        float raduis = msh.radius;

        string name = msh.gameObject.name;
        Transform parent = msh.gameObject.transform.parent;
        Vector3 localPosition = msh.gameObject.transform.localPosition;
        Quaternion localRoation = msh.gameObject.transform.localRotation;
        Vector3 localScale = msh.gameObject.transform.localScale;
        int layer = msh.gameObject.layer;

        GameObject go = msh.gameObject;
        GameObject.DestroyImmediate(go);

        go = GameObject.CreatePrimitive(PrimitiveType.Sphere);
        go.name = name;
        go.transform.parent = parent;
        go.transform.localPosition = localPosition;
        go.transform.localRotation = localRoation;
        go.transform.localScale = localScale;
        go.layer = layer;
        MeshRenderer re = go.GetComponent<MeshRenderer>();
        re.enabled = ShowMeshColliderFlags;
        re.sharedMaterial = GetLayerMaterial(go.layer);
        currColliderMeshRender.Add(re);

        msh = go.GetComponent<SphereCollider>();
        msh.center = center;
        msh.radius = raduis;
        //Debug.Log("刷新SphereCollider:" + ",center:" + center + ",raduis:" + raduis);
        return msh;
    }

    private static Collider RefershBoxCollider(BoxCollider msh)
    {
        if (!msh)
        {
            return msh;
        }
        Vector3 center = msh.center;
        Vector3 raduis = msh.size;
        string name = msh.gameObject.name;
        Transform parent = msh.gameObject.transform.parent;
        Vector3 localPosition = msh.gameObject.transform.localPosition;
        Quaternion localRoation = msh.gameObject.transform.localRotation;
        Vector3 localScale = msh.gameObject.transform.localScale;
        int layer = msh.gameObject.layer;

        GameObject go = msh.gameObject;
        GameObject.DestroyImmediate(go);

        go = GameObject.CreatePrimitive(PrimitiveType.Cube);
        go.name = name;
        go.transform.parent = parent;
        go.transform.localPosition = localPosition;
        go.transform.localRotation = localRoation;
        go.transform.localScale = localScale;
        go.layer = layer;

        MeshRenderer re = go.GetComponent<MeshRenderer>();
        re.enabled = ShowMeshColliderFlags;
        re.sharedMaterial = GetLayerMaterial(go.layer);
        currColliderMeshRender.Add(re);

        msh = go.GetComponent<BoxCollider>();
        msh.center = center;
        msh.size = raduis;
       // Debug.Log("刷新BoxCollider:" + ",center:" + center + ",size:" + raduis);
        return msh;
    }

    public static void ClearColliderDebugData()
    {
        foreach(Material mat in Layer_colliderMeshMaterial.Values)
        {
            GameObject.DestroyImmediate(mat);
        }
        
        currColliderMeshRender = null;
    }
}