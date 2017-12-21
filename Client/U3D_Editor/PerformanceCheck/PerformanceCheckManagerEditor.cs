/*******************************************************************
** 文件名:	PerformanceCheckManagerEditor.cs
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
using UnityEditor;

[CustomEditor(typeof(PerformanceCheckManager))]
public class PerformanceCheckManagerEditor : Editor
{

    PerformanceCheckManager instance = null;
    public void OnEnable()
    {
        instance = target as PerformanceCheckManager;
    }

    public override void OnInspectorGUI()
    {
        if(GUILayout.Button("Bake"))
        {
            Clear();
            Bake();
        }
    }

    private void Clear()
    {
        GameObject go = GameObject.Find("PerformanceCheckManagerRoot");

        for (int i = 0; i < instance.flowShareData.ImageEffectList.Count; i++)
        {
            GameObject.DestroyImmediate(instance.flowShareData.ImageEffectList[i]);
        }


        GameObject.DestroyImmediate(go);
        instance.flowShareData = null;
    }

    private void Bake()
    {
        GameObject root = new GameObject();
        root.name = "PerformanceCheckManagerRoot";
        string modelPath = "SceneOptimitzedConfig/PerformanceCheckModel";
        instance.flowShareData = new FlowShareData();
        GameObject model = Resources.Load<GameObject>(modelPath);
        if(!model)
        {
            Debug.LogError("找不到性能检测模型，路径:" + modelPath);
            return;
        }
        string skinModelPath = "SceneOptimitzedConfig/PerformanceCheckSkinModel";
        GameObject skinModel = Resources.Load<GameObject>(skinModelPath);
        if (!skinModel)
        {
            Debug.LogError("找不到性能检测模型，路径:" + skinModelPath);
            return;
        }

        string TransparentPath = "SceneOptimitzedConfig/PerformanceCheckTransparentModel";
        GameObject TransparentModel = Resources.Load<GameObject>(TransparentPath);
        if (!TransparentModel)
        {
            Debug.LogError("找不到性能检测模型，路径:" + skinModelPath);
            return;
        }

        GameObject LightObj = GameObject.Find("FitstSunLight");

        if(!LightObj)
        {
            Debug.LogError("找不到性能检测的FitstSunLight" );
            return;
        }

        Light sunLight = LightObj.GetComponent<Light>();
        if(!sunLight)
        {
            sunLight = LightObj.AddComponent<Light>();
        }
        sunLight.type = LightType.Directional;
        instance.flowShareData.shareSunLight = sunLight;
        sunLight.enabled = false;
        for (int i = 0; i < PerformanceCheckManager.MaxOpaqueModelCount; i++)
        {
            GameObject go = GameObject.Instantiate<GameObject>(model);
            go.transform.SetParent(root.transform);
            MeshRenderer re = go.GetComponent<MeshRenderer>();
            re.enabled = false;
            instance.flowShareData.OpaqueModelList.Add(re);

            //MeshFilter mf = go.GetComponent<MeshFilter>();
            //Mesh ms = new Mesh();
            //ms.name = "OpaqueTempMesh-" + i;
            //Vector3[] vet = new Vector3[mf.sharedMesh.vertexCount];
            //mf.sharedMesh.vertices.CopyTo(vet, 0);
            //int[] tri = new int[mf.sharedMesh.triangles.Length];
            //mf.sharedMesh.triangles.CopyTo(tri, 0);

            //Vector2[] uvs = new Vector2[mf.sharedMesh.uv.Length];
            //mf.sharedMesh.uv.CopyTo(uvs,0);
            //ms.vertices = vet;
            //ms.triangles = tri;
            //ms.uv = uvs;

            //mf.sharedMesh = ms;

            //MeshFilter mf = go.GetComponent<MeshFilter>();
            //Mesh ms = GameObject.Instantiate(mf.sharedMesh);
            //ms.name = "OpaqueTempMesh-" + i;
            //mf.sharedMesh = ms;
        }

        for (int i = 0; i < PerformanceCheckManager.MaxTransparentModelCount; i++)
        {
            GameObject go = GameObject.Instantiate<GameObject>(TransparentModel);
            go.transform.SetParent(root.transform);
            MeshRenderer re = go.GetComponent<MeshRenderer>();
            re.enabled = false;
            instance.flowShareData.TransparentModelList.Add(re);

            //MeshFilter mf = go.GetComponent<MeshFilter>();
            //Mesh ms =GameObject.Instantiate(mf.sharedMesh);
            //ms.name = "TransparentTempMesh-" + i;
            //mf.sharedMesh = ms;
        }

        for (int i = 0; i < PerformanceCheckManager.MaxSkinnedModelCount; i++)
        {
            GameObject go = GameObject.Instantiate<GameObject>(skinModel);
            go.transform.SetParent(root.transform);
            SkinnedMeshRenderer []re = go.GetComponentsInChildren<SkinnedMeshRenderer>();
            //foreach (SkinnedMeshRenderer r in re)
            //{
            //    Mesh ms = GameObject.Instantiate(r.sharedMesh);
            //    ms.name = "SkinnedTempMesh-" + i;
            //    r.sharedMesh = ms;
            //}

            go.SetActive(false);
            instance.flowShareData.SkinnedMeshModelList.AddRange(re);
            instance.flowShareData.SkinnedObjList.Add(go);


        }

        GameObject cam = Camera.main.gameObject;
        for (int i = 0; i < PerformanceCheckManager.MaxImageEffectCout; i++)
        {
            PerformanceCheckImageEffect pci = cam.AddComponent<PerformanceCheckImageEffect>();
            pci.enabled = false;
            instance.flowShareData.ImageEffectList.Add(pci);
        }


        //PerformanceCheck_UI checkUI = GameObject.FindObjectOfType<PerformanceCheck_UI>();
        //if (!checkUI)
        //{
        //    Debug.LogError("找不到UI:PerformanceCheckUI_Waiting");
        //    return;
        //}

        //PerformanceCheckUI_Waiting waiting = GameObject.FindObjectOfType<PerformanceCheckUI_Waiting>();
        //if (!waiting)
        //{
        //    Debug.LogError("找不到UI:PerformanceCheckUI_Waiting");
        //    return;
        //}

        //PerformanceCheckUI_Reslut reslut = GameObject.FindObjectOfType<PerformanceCheckUI_Reslut>();
        //if (!reslut)
        //{
        //    Debug.LogError("找不到UI:PerformanceCheckUI_Reslut");
        //    return;
        //}

        //PerformanceCheckUI_BackGround background = GameObject.FindObjectOfType<PerformanceCheckUI_BackGround>();
        //if (!background)
        //{
        //    Debug.LogError("找不到UI:PerformanceCheckUI_BackGround");
        //    return;
        //}

        //PerformanceCheckUI_BreakTips breaktips = GameObject.FindObjectOfType<PerformanceCheckUI_BreakTips>();
        //if (!breaktips)
        //{
        //    Debug.LogError("找不到UI:PerformanceCheckUI_BreakTips");
        //    return;
        //}
        //instance.flowShareData.CheckUI = checkUI;
        //instance.flowShareData.ui_Reslut = reslut;
        //instance.flowShareData.ui_Waiting = waiting;
        //instance.flowShareData.CheckingBackground = background;
        //instance.flowShareData.breakTips = breaktips;

        UnityEditor.SceneManagement.EditorSceneManager.SaveScene(UnityEditor.SceneManagement.EditorSceneManager.GetActiveScene());
        EditorApplication.SaveAssets();
    }
}



