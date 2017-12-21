/*******************************************************************
** 文件名:	WaterEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	播放特效编辑器

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using System.Collections;
using UnityEditor;
using System.Collections.Generic;
namespace Effect
{
    [CustomEditor(typeof(Water))]
    public class WaterEditor : Editor
    {
        SerializedProperty BakeCullMask = null;
        Water pa;
        Vector2 BoundsPos = Vector2.zero;

        Dictionary<BoxCollider, GameObject> RefAreaRootObjTable = new Dictionary<BoxCollider, GameObject>();
        HashSet<Renderer> renderCheck = new HashSet<Renderer>();
        static string[] WaterTexturesPropertyNames = new string[] { "_Fresnel","_BumpMap", "_ReflectiveColor" };
        public void OnEnable()
        {
            pa = target as Water;
            BakeCullMask = serializedObject.FindProperty("m_BakeCullMask");
        }
        public override void OnInspectorGUI()
        {
            bool bDirty = false;
            pa.m_TextureSize = EditorGUILayout.IntField("贴图大小:" , pa.m_TextureSize);
            pa.m_ClipPlaneOffset = EditorGUILayout.FloatField("物体到水面的距离:", pa.m_ClipPlaneOffset);
            pa.EdgeBlendStength = EditorGUILayout.Slider("水面边缘柔和:",pa.EdgeBlendStength,0,10);

            if(!Application.isPlaying)
            {
                EditorGUI.BeginChangeCheck();
                EditorGUILayout.PropertyField(BakeCullMask, new GUIContent("显示层:"));
                if (EditorGUI.EndChangeCheck())
                {
                    serializedObject.ApplyModifiedProperties();
                }
            }

            EditorGUILayout.LabelField("反射数目:" + pa.allVisibleRenderList.Count);
            BoundsPos = EditorGUILayout.BeginScrollView(BoundsPos);

            for (int i = 0; i < pa.WaterRefAeraList.Count;i++ )
            {
                BoxCollider bc = pa.WaterRefAeraList[i];
                if(!bc)
                {
                    pa.WaterRefAeraList.RemoveAt(i);
                    continue;
                }
                EditorGUILayout.BeginHorizontal("box");
                bc = EditorGUILayout.ObjectField(bc, typeof(BoxCollider), true) as BoxCollider;
                if(GUILayout.Button("删除"))
                {
                    GameObject.DestroyImmediate(bc.gameObject);
                    pa.WaterRefAeraList.RemoveAt(i);
                }
                EditorGUILayout.EndHorizontal();
            }

            EditorGUILayout.EndScrollView();

            if(GUILayout.Button("增加反射区域"))
            {
                GameObject go = new GameObject();
                go.name = "WaterRefArea--" + pa.WaterRefAeraList.Count;
                go.transform.localScale = Vector3.one * 5.0f;
                go.transform.SetParent(pa.transform);
                Vector3 dir = EditorCamera.GetRotation() * Vector3.forward;
                go.transform.localRotation = Quaternion.identity;
                go.transform.position = EditorCamera.GetPosition() + dir * 10;
                go.layer = Water.WaterObjLayer;
                BoxCollider box = go.AddComponent<BoxCollider>();
                
                box.isTrigger = true;
                pa.WaterRefAeraList.Add(box);

                Selection.activeGameObject = go;
            }
            
            if (GUILayout.Button("Bake"))
            {
                AssetBundleImporter.BeginPrefabAssetImport();
                BakeRender(pa);
                AssetBundleImporter.EndPrefabAssetImport();
                bDirty = true;
            }

            if(GUILayout.Button("Clear"))
            {
                ClearBake(pa);
                bDirty = true;
            }

            if(GUILayout.Button("Init"))
            {
                Water.Init();
            }
            bDirty |= GUI.changed;
            if (bDirty)
            {
                EditorUtility.SetDirty(pa);
            }
        }

        public void ClearBake(Water hostWater)
        {
            foreach (Renderer r in hostWater.allVisibleRenderList)
            {
                if (r)
                {
                    DestroyImmediate(r.gameObject);
                }
            }
            hostWater.allVisibleRenderList.Clear();
            hostWater.allWaterNode.Clear();
            hostWater.WaterRefAeraRootObjList.Clear();
            GameObject go = GameObject.Find("WaterRefObjRoot");
            if (go)
            {
                DestroyImmediate(go);
            }
            if (hostWater.WaterRefObjRoot)
            {
                DestroyImmediate(hostWater.WaterRefObjRoot);
            }
            hostWater.WaterRefObjRoot = null;
            hostWater.bBaked = false;
        }

        public void BakeRender(Water hostWater)
        {
            RefAreaRootObjTable.Clear();
            ClearBake(hostWater);
            renderCheck.Clear();
            hostWater.allTextureConfig.Clear();
            hostWater.WaterRefObjRoot = new GameObject();
            hostWater.WaterRefObjRoot.name = "WaterRefObjRoot";
            hostWater.WaterRefObjRoot.transform.position = Vector3.zero;
            hostWater.WaterRefObjRoot.transform.rotation = Quaternion.identity;
            hostWater.WaterRefObjRoot.transform.localScale = Vector3.one;
            hostWater.allVisibleRenderList.Clear();
            List<MeshRenderer> MeshRendersList = new List<MeshRenderer>();
            List<SkinnedMeshRenderer> SkinnedMeshRenderList = new List<SkinnedMeshRenderer>();
            MeshRendersList.AddRange(GameObject.FindObjectsOfType<MeshRenderer>());
            SkinnedMeshRenderList.AddRange(GameObject.FindObjectsOfType<SkinnedMeshRenderer>());

            MeshRenderer waterRender = hostWater.GetComponent<MeshRenderer>();
            Collider waterCollider = hostWater.GetComponent<Collider>();

            if (waterRender)
            {
                if (!waterCollider)
                {
                    waterCollider = hostWater.gameObject.AddComponent<MeshCollider>();
                }

                if(waterRender.sharedMaterial)
                {
                    foreach(string spro in WaterTexturesPropertyNames)
                    {
                        if(waterRender.sharedMaterial.HasProperty(spro))
                        {
                            Texture t = waterRender.sharedMaterial.GetTexture(spro);
                            if(t)
                            {
                                string patch = AssetDatabase.GetAssetPath(t);
                                if(!string.Empty.Equals(patch))
                                {
                                    ResConfigData cof = AssetBundleImporter.MakeTextureAssetBundleFromReference (patch,string.Empty);
                                    SceneTextureConfigData cd = new SceneTextureConfigData();
                                    cd.AssetBundleName = cof.AssetBundleName;
                                    cd.AssetGUID = cof.AssetGUID;
                                    cd.AssetName = cof.AssetName;
                                    cd.ShaderKeyWordName = spro;
                                    if(null != cof)
                                    {
                                        hostWater.allTextureConfig.Add(cd);
                                    }
                                }
                            }
                        }
                    }
                }
                foreach(BoxCollider box in hostWater.WaterRefAeraList)
                {
                    GameObject go = new GameObject();
                    go.name = box.gameObject.name;
                    go.transform.SetParent(hostWater.WaterRefObjRoot.transform);
                    go.transform.localPosition = Vector3.zero;
                    go.transform.rotation = Quaternion.identity;
                    go.transform.localScale = Vector3.one;
                    hostWater.WaterRefAeraRootObjList.Add(go);
                    RefAreaRootObjTable.Add(box, go);
                }
                BakeRender_MeshRender(MeshRendersList, hostWater,waterRender, waterCollider);
                BakeRender_SkinnedMeshRender(SkinnedMeshRenderList, hostWater, waterRender,waterCollider);
            }
            hostWater.bBaked = true;
        }

        private  void BakeRender_MeshRender(List<MeshRenderer> renderList,Water hostWater,
                                                   MeshRenderer WaterRender,Collider WaterColliderBounds)
        {
            foreach (BoxCollider box in hostWater.WaterRefAeraList)
            {
                box.enabled = true;
                foreach (Renderer r in renderList)
                {
                    if (r == WaterRender)
                    {
                        continue;
                    }

                    if (((1 << r.gameObject.layer) & hostWater.m_BakeCullMask.value) == 0 ||
                        r.gameObject.layer == 4 || !r.enabled || !r.gameObject.activeInHierarchy ||
                        r.gameObject.name.Contains("middle_terrain")/*不要中景*/)
                    {
                        continue;
                    }
                    BoxCollider bc = null;
                    bc = r.GetComponent<BoxCollider>();
                    if (!bc)
                    {
                        bc = r.gameObject.AddComponent<BoxCollider>();
                    }
                    if (bc.bounds.Intersects(box.bounds))
                    {
                        //在水面之上
                        if (bc.bounds.max.y - WaterColliderBounds.bounds.min.y > hostWater.m_ClipPlaneOffset)
                        {
                            if (!renderCheck.Contains(r))
                            {
                                Renderer InstanceRender = GameObject.Instantiate(r);
                                if (InstanceRender)
                                {
                                    InstanceRender.gameObject.name = "WaterRefObj--" + r.gameObject.name;
                                    InstanceRender.transform.SetParent(RefAreaRootObjTable[box].transform);
                                    InstanceRender.transform.position = r.transform.position;
                                    InstanceRender.transform.rotation = r.transform.rotation;
                                    InstanceRender.transform.localScale = r.transform.lossyScale;
                                    InstanceRender.gameObject.layer = Water.WaterObjLayer;
                                    InstanceRender.gameObject.isStatic = false;
                                    InstanceRender.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
                                    InstanceRender.receiveShadows = false;
                                    InstanceRender.reflectionProbeUsage = UnityEngine.Rendering.ReflectionProbeUsage.Off;
                                    Collider col = InstanceRender.GetComponent<Collider>();
                                    if (col)
                                    {
                                        GameObject.DestroyImmediate(col);
                                    }
                                    GameObject[] deleteArray = new GameObject[InstanceRender.transform.childCount];
                                    for (int i = 0; i < InstanceRender.transform.childCount; i++)
                                    {
                                        deleteArray[i] = InstanceRender.transform.GetChild(i).gameObject;
                                    }

                                    for (int i = 0; i < deleteArray.Length; i++)
                                    {
                                        GameObject.DestroyImmediate(deleteArray[i]); 
                                    }

                                    Water.WaterNode wn = new Water.WaterNode();
                                    wn.render = r;
                                    wn.animator = null;
                                    wn.hostBoxCollider = box;

                                    hostWater.allWaterNode.Add(wn);
                                    hostWater.allVisibleRenderList.Add(InstanceRender);
                                    renderCheck.Add(r);
                                }

                            }
                        }
                    }
                    GameObject.DestroyImmediate(bc);
                }
                box.enabled = false;
            }
        }

        private void BakeRender_SkinnedMeshRender(List<SkinnedMeshRenderer> renderList,Water hostWater,
                                                        MeshRenderer WaterRender, Collider WaterColliderBounds)
        {
            foreach (BoxCollider box in hostWater.WaterRefAeraList)
            {
                box.enabled = true;
                foreach (Renderer r in renderList)
                {
                    if (r == WaterRender)
                    {
                        continue;
                    }

                    if (((1 << r.gameObject.layer) & hostWater.m_BakeCullMask.value) == 0 ||
                        r.gameObject.layer == 4 || !r.enabled || !r.gameObject.activeInHierarchy ||
                        r.gameObject.name.Contains("middle_terrain")/*不要中景*/)
                    {
                        continue;
                    }
                    BoxCollider bc = null;
                    bc = r.GetComponent<BoxCollider>();
                    if (!bc)
                    {
                        bc = r.gameObject.AddComponent<BoxCollider>();
                    }
                    if (bc.bounds.Intersects(box.bounds))
                    {
                        //在水面之上
                        if (bc.bounds.max.y - WaterColliderBounds.bounds.min.y > hostWater.m_ClipPlaneOffset)
                        {
                            if (!renderCheck.Contains(r))
                            {
                                Animator ana = r.GetComponentInParent<Animator>();
                                if(!ana)
                                {
                                    Debug.LogWarning("SkinnedMeshRender上没有Animator,忽略:" + r.name);
                                    continue;
                                }

                                if(!ana.runtimeAnimatorController || !ana.avatar)
                                {
                                    Debug.LogWarning("SkinnedMeshRender上的AnimatorController或者Avator为null,忽略:" + r.name);
                                    continue;
                                }

                                GameObject skinInstance = GameObject.Instantiate<GameObject>(ana.gameObject);
                                if(!skinInstance)
                                {
                                    continue;
                                }

                                Renderer []smrs = skinInstance.GetComponentsInChildren<Renderer>();

                                if(smrs.Length > 1)
                                {
                                    Debug.LogWarning(ana.gameObject.name + "有多个Render,水反射不支持多个Render");
                                    GameObject.DestroyImmediate(skinInstance);
                                    continue;
                                }

                                Renderer InstanceRender = smrs[0];
                                if (InstanceRender)
                                {
                                    skinInstance.gameObject.name = "WaterRefObj--" + r.gameObject.name;
                                    skinInstance.transform.SetParent(RefAreaRootObjTable[box].transform);
                                    skinInstance.transform.position = ana.transform.position;
                                    skinInstance.transform.rotation = ana.transform.rotation;
                                    skinInstance.transform.localScale = ana.transform.lossyScale;
                                    GameUtil.SetLayer(Water.WaterObjLayer,skinInstance, true);
                                    skinInstance.isStatic = false;
                                    InstanceRender.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
                                    InstanceRender.receiveShadows = false;
                                    InstanceRender.reflectionProbeUsage = UnityEngine.Rendering.ReflectionProbeUsage.Off;
                                    Collider col = InstanceRender.GetComponent<Collider>();
                                    if (col)
                                    {
                                        GameObject.DestroyImmediate(col);
                                    }
                                    GameObject[] deleteArray = new GameObject[InstanceRender.transform.childCount];
                                    for (int i = 0; i < InstanceRender.transform.childCount; i++)
                                    {
                                        deleteArray[i] = InstanceRender.transform.GetChild(i).gameObject;
                                    }

                                    for (int i = 0; i < deleteArray.Length; i++)
                                    {
                                        GameObject.DestroyImmediate(deleteArray[i]);
                                    }
                                    Water.WaterNode wn = new Water.WaterNode();
                                    wn.render = r;
                                    wn.animator = skinInstance.GetComponent<Animator>();
                                    wn.hostBoxCollider = box;
                                    hostWater.allWaterNode.Add(wn);
                                    hostWater.allVisibleRenderList.Add(InstanceRender);
                                    renderCheck.Add(r);
                                }

                            }
                        }
                    }
                    GameObject.DestroyImmediate(bc);
                }
                box.enabled = false;
            }
        }
    }

}
