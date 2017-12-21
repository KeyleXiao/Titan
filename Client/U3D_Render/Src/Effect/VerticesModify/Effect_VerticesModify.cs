/*******************************************************************
** 文件名:	Effect_VerticesModify.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/9/7
** 版  本:	1.0
** 描  述:	顶点修改
** 应  用:   用于做顶点修改相关的特效
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

/// <summary>
/// 顶点修改
/// </summary>
namespace Effect
{
    public class Effect_VerticesModify : MonoBehaviour
    {

        public MaterialOpRunType TargetType = MaterialOpRunType.Self;
        public int HostEntityID = -1;
        public GameObject TargetModel = null;
        /// <summary>
        /// 影响那些渲染器，只有这些渲染器会被影响到，不填表示谁都不影响
        /// </summary>
        public string AffectRenderName = "";


        /// <summary>
        /// 网格Key，只有与这个名字一样的网格才会进行下一步
        /// </summary>
        //[HideInInspector, SerializeField]
        public string MeshKeyName = "";

        /// <summary>
        /// 数据路径名
        /// </summary>
        //[HideInInspector, SerializeField]
        public string DataPatchName = "";

        /// <summary>
        /// 原始网格
        /// </summary>
        public Mesh OrginalMesh
        {
            get
            {
                return m_OrginalMesh;
            }
        }
        private Mesh m_OrginalMesh = null;
        private List<Vector2> m_OrginalUV4Data = new List<Vector2>();
        private List<Color> m_OrginalVerticesColor = new List<Color>();

        private MeshFilter m_MeshFilter = null;
        private SkinnedMeshRenderer m_SkinnedMeshRender = null;
        private ParticleSystemRenderer m_ParticleSystemRender = null;

        //private bool bInit = false;
        //private VerticesModifyData m_DataInstance = null;
        //private bool bApplyVerticesData = false;

        #region RunTime

        public void Start()
        {
            //if (!string.IsNullOrEmpty(MeshKeyName) && !string.IsNullOrEmpty(DataPatchName))
            //{
            //    //if (TargetType == MaterialOpRunType.Self ||
            //    //    TargetType == MaterialOpRunType.PointedModel)
            //    //{
            //    //    Init();
            //    //    Play();
            //    //}

            //}

        }

        public void OnDestroy()
        {
            //Stop();

            //bInit = false;
            //m_OrginalMesh = null;
            //m_MeshFilter = null;
            //m_SkinnedMeshRender = null;
            //m_ParticleSystemRender = null;
            //m_OrginalUV4Data.Clear();
            //m_OrginalVerticesColor.Clear();
            //m_OrginalUV4Data = null;
            //m_OrginalVerticesColor = null;

            //UnLoadData();
        }


        public void UnLoadData()
        {
            //if (m_DataInstance)
            //{
            //    Resources.UnloadAsset(m_DataInstance);
            //    m_DataInstance = null;
            //}
        }
        public bool LoadData()
        {
            //if (!m_DataInstance)
            //{
            //    m_DataInstance = Resources.Load<VerticesModifyData>(DataPatchName);
            //    if (!m_DataInstance)
            //    {
            //        Debug.LogError("在路径下找不到数据:" + DataPatchName);
            //        return false;
            //    }
            //}
            return true;
        }
        public void Init()
        {
        //    if (bInit)
        //    {
        //        return;
        //    }
        //    GameObject affObj = GetAffectObj();
        //    if (!affObj)
        //    {
        //        return;
        //    }
        //    if (!m_OrginalMesh)
        //    {
        //        m_MeshFilter = affObj.GetComponent<MeshFilter>();
        //        if (m_MeshFilter)
        //        {
        //            m_OrginalMesh = m_MeshFilter.sharedMesh;
        //        }
        //    }

        //    if (!m_OrginalMesh)
        //    {
        //        m_SkinnedMeshRender = affObj.GetComponent<SkinnedMeshRenderer>();
        //        if (m_SkinnedMeshRender)
        //        {
        //            m_OrginalMesh = m_SkinnedMeshRender.sharedMesh;
        //        }

        //    }

        //    if (!m_OrginalMesh)
        //    {
        //        m_ParticleSystemRender = affObj.GetComponent<ParticleSystemRenderer>();
        //        if (m_ParticleSystemRender)
        //        {
        //            m_OrginalMesh = m_ParticleSystemRender.mesh;
        //        }

        //    }
        //    affObj = null;
        //    if (!m_OrginalMesh)
        //    {
        //        Debug.LogError("找不到网格!");
        //        return;
        //    }
        //    m_OrginalUV4Data.Clear();
        //    m_OrginalUV4Data.AddRange(m_OrginalMesh.uv4);

        //    m_OrginalVerticesColor.Clear();
        //    m_OrginalVerticesColor.AddRange(m_OrginalMesh.colors);

        //    if (string.IsNullOrEmpty(DataPatchName) && string.IsNullOrEmpty(MeshKeyName))
        //    {
        //        bInit = true;
        //        return;
        //    }

        //    if (!LoadData())
        //    {
        //        return;
        //    }
        //    bInit = true;
        }


        public void Play()
        {
            //if (!bInit)
            //{
            //    Init();
            //}
            //ApplyVerticesData();
        }


        public void Stop()
        {
            //ResetVerticesData();
        }


        /// <summary>
        /// 取得受影响的物体
        /// </summary>
        /// <returns></returns>
        private GameObject GetAffectObj()
        {
            //if (TargetType == MaterialOpRunType.Self)
            //{
            //    return gameObject;
            //}
            //else if (TargetType == MaterialOpRunType.PointedModel)
            //{
            //    return TargetModel;
            //}
            //else if (TargetType == MaterialOpRunType.EntityID)
            //{
            //    if (HostEntityID > 0)
            //    {
            //        U3D_Render.EntityView view = EntityFactory.getEntityViewByID(HostEntityID);
            //        if (null != view)
            //        {
            //            GameObject go = view.SkinControl.GetCurrentModelObject();
            //            if (go)
            //            {
            //                MeshRenderer[] MeshRenders = go.GetComponentsInChildren<MeshRenderer>();
            //                SkinnedMeshRenderer[] SkinnedMeshRenderers = go.GetComponentsInChildren<SkinnedMeshRenderer>();

            //                if (!string.IsNullOrEmpty(AffectRenderName))
            //                {
            //                    for (int i = 0; i < MeshRenders.Length; i++)
            //                    {
            //                        if (AffectRenderName.Equals(MeshRenders[i].gameObject.name))
            //                        {
            //                            return MeshRenders[i].gameObject;
            //                        }
            //                    }
            //                    for (int i = 0; i < SkinnedMeshRenderers.Length; i++)
            //                    {
            //                        if (AffectRenderName.Equals(SkinnedMeshRenderers[i].gameObject.name))
            //                        {
            //                            return SkinnedMeshRenderers[i].gameObject;
            //                        }
            //                    }
            //                }
            //            }
            //        }
            //    }
            //}
            return null;
        }


        /// <summary>
        /// 应用顶点数据，目前有UV4和顶点色，运行时用的
        /// </summary>
        private void ApplyVerticesData()
        {
            //if (!bInit)
            //{
            //    return;
            //}
            //if (!m_DataInstance)
            //{
            //    return;
            //}
            //if (!OrginalMesh.name.Contains(MeshKeyName))
            //{
            //    Debug.LogError("当前顶点修改器与本Mesh不一致,本修改器适配的Mesh名字为:" + MeshKeyName + ",本Mesh名为:" + OrginalMesh.name);
            //    return;
            //}
            //if (!m_DataInstance.MeshKeyName.Equals(MeshKeyName))
            //{
            //    Debug.LogError("当前数据MeshKeyName与本MeshKeyName不一致,数据适配的MeshKeyName为:" + m_DataInstance.MeshKeyName + ",本MeshKeyName名为:" + MeshKeyName);
            //    return;
            //}

            //if (bApplyVerticesData)
            //{
            //    return;
            //}
            //if (m_DataInstance.UV4Data.Count > 0)
            //{
            //    if (m_DataInstance.UV4Data.Count == OrginalMesh.uv.Length)
            //    {
            //        OrginalMesh.uv4 = m_DataInstance.UV4Data.ToArray();
            //    }
            //    else
            //    {
            //        Debug.LogError("UV4 Length is not Conform to the UV0 Length!!!");
            //    }

            //}

            ////如果是粒子系统，则无法使用顶点色功能
            //if (!m_ParticleSystemRender)
            //{
            //    if (m_DataInstance.VerticesColor.Count > 0)
            //    {
            //        if (m_DataInstance.VerticesColor.Count == OrginalMesh.uv.Length)
            //        {
            //            OrginalMesh.colors = m_DataInstance.VerticesColor.ToArray();
            //        }
            //        else
            //        {
            //            Debug.LogError("VertexColor Length is not Conform to the MeshColors Length!!!");
            //        }
            //    }
            //}

            //bApplyVerticesData = true;
        }

        private void ResetVerticesData()
        {
            //if (bInit && bApplyVerticesData)
            //{
            //    OrginalMesh.uv4 = m_OrginalUV4Data.ToArray();
            //    OrginalMesh.colors = m_OrginalVerticesColor.ToArray();
            //    bApplyVerticesData = false;
            //}
        }

        #endregion


        #region 以下方法只能编辑器使用

        /// <summary>
        /// 第四套UV数据,也仅仅只能自定义第四套UV
        /// </summary>
        [HideInInspector, SerializeField]
        public List<Vector2> UV4Data = new List<Vector2>();

        /// <summary>
        /// 顶点颜色
        /// </summary>
        [HideInInspector, SerializeField]
        public List<Color> VerticesColor = new List<Color>();


        private List<Vector2> m_OrginalUV4Data_Editor = new List<Vector2>();
        private List<Color> m_OrginalVerticesColor_Editor = new List<Color>();

        public Renderer CurrentRender
        {
            get
            {
                if (!m_Render)
                {
                    GameObject affectObj = GetAffectObj();
                    if (!affectObj)
                    {
                        return null;
                    }
                    m_Render = affectObj.GetComponent<Renderer>();
                }
                return m_Render;
            }

        }
        public Object rawObject
        {
            get
            {
                if (m_MeshFilter)
                {
                    return m_MeshFilter;
                }

                if (m_SkinnedMeshRender)
                {
                    return m_SkinnedMeshRender;
                }

                if (m_ParticleSystemRender)
                {
                    return m_ParticleSystemRender;
                }

                return null;
            }
        }
        public Mesh CurrentModifyMesh
        {
            get
            {
                if (m_MeshFilter)
                {
                    return m_MeshFilter.sharedMesh;
                }
                else if (m_SkinnedMeshRender)
                {
                    return m_SkinnedMeshRender.sharedMesh;
                }
                else if (m_ParticleSystemRender)
                {
                    return m_ParticleSystemRender.mesh;
                }
                return null;
            }
            set
            {
                if (m_MeshFilter)
                {
                    m_MeshFilter.sharedMesh = value;
                }
                else if (m_SkinnedMeshRender)
                {
                    m_SkinnedMeshRender.sharedMesh = value;
                }
                else if (m_ParticleSystemRender)
                {
                    m_ParticleSystemRender.mesh = value;
                }
            }
        }


        private bool bInitOnVerticesEditor = false;
        private Renderer m_Render = null;


        public void Play_Editor()
        {
            //if (!m_DataInstance)
            //{
            //    m_DataInstance = Resources.Load<VerticesModifyData>(DataPatchName);
            //    if (!m_DataInstance)
            //    {
            //        Debug.LogError("在路径下找不到数据:" + DataPatchName);
            //        return;
            //    }
            //}

            //ApplyVerticesData();
        }


        public void Stop_Editor()
        {
            //ResetVerticesData();
            //UnLoadData();
        }


        public bool InitOnVerticesEditor()
        {
            //if (bInitOnVerticesEditor)
            //{
            //    return true;
            //}
            //GameObject affObj = GetAffectObj();

            //if (!affObj)
            //{
            //    Debug.LogError("找不到物体!");
            //    return false;
            //}
            //if (!m_OrginalMesh)
            //{
            //    m_MeshFilter = affObj.GetComponent<MeshFilter>();
            //    if (m_MeshFilter)
            //    {
            //        m_OrginalMesh = m_MeshFilter.sharedMesh;
            //    }
            //}

            //if (!m_OrginalMesh)
            //{
            //    m_SkinnedMeshRender = affObj.GetComponent<SkinnedMeshRenderer>();
            //    if (m_SkinnedMeshRender)
            //    {
            //        m_OrginalMesh = m_SkinnedMeshRender.sharedMesh;
            //    }

            //}

            //if (!m_OrginalMesh)
            //{
            //    m_ParticleSystemRender = affObj.GetComponent<ParticleSystemRenderer>();
            //    if (m_ParticleSystemRender)
            //    {
            //        m_OrginalMesh = m_ParticleSystemRender.mesh;
            //    }

            //}
            //affObj = null;
            //if (!m_OrginalMesh)
            //{
            //    Debug.LogError("找不到mesh!");
            //    return false;
            //}
            //m_OrginalUV4Data_Editor.Clear();
            //m_OrginalVerticesColor_Editor.Clear();

            ////这两个是null的话，表明这个是最原始的数据,原始数据是没有UV4
            //if (!string.IsNullOrEmpty(DataPatchName) && !string.IsNullOrEmpty(MeshKeyName))
            //{
            //    LoadData();

            //    m_OrginalUV4Data_Editor.AddRange(m_OrginalMesh.uv4);

            //}



            //ResetUV4Data();

            ////粒子系统不支持顶点色
            //if (!m_ParticleSystemRender)
            //{
            //    m_OrginalVerticesColor_Editor.AddRange(m_OrginalMesh.colors);
            //    ResetVerticesColorData();
            //}

            //bInitOnVerticesEditor = true;
            return true;
        }


        /// <summary>
        /// 重设UV4数据
        /// </summary>
        public void ResetUV4Data()
        {
            //if (m_DataInstance)
            //{
            //    //有UV数据，并且UV数据匹配
            //    if (m_DataInstance.UV4Data.Count == OrginalMesh.uv.Length)
            //    {
            //        CurrentModifyMesh.uv4 = m_DataInstance.UV4Data.ToArray();
            //    }
            //}
            //else//使用UV0数据
            //{
            //    UV4Data.Clear();
            //    CurrentModifyMesh.uv4 = CurrentModifyMesh.uv;
            //}

        }


        /// <summary>
        /// 出厂设置
        /// </summary>
        public void ResetUV4FromFactory()
        {
            //CurrentModifyMesh.uv4 = CurrentModifyMesh.uv;

            //UV4Data.Clear();
        }


        public void ClearVerticesEditorCache(bool isInPlayingMode)
        {
            //if (m_MeshFilter)
            //{
            //    m_MeshFilter.sharedMesh = m_OrginalMesh;
            //    m_MeshFilter.sharedMesh.uv4 = m_OrginalUV4Data_Editor.ToArray();
            //    m_MeshFilter.sharedMesh.colors = m_OrginalVerticesColor_Editor.ToArray();

            //}
            //else if (m_SkinnedMeshRender)
            //{
            //    m_SkinnedMeshRender.sharedMesh = m_OrginalMesh;
            //    m_SkinnedMeshRender.sharedMesh.uv4 = m_OrginalUV4Data_Editor.ToArray();
            //    m_SkinnedMeshRender.sharedMesh.colors = m_OrginalVerticesColor_Editor.ToArray();
            //}
            //else if (m_ParticleSystemRender)
            //{
            //    m_ParticleSystemRender.mesh = m_OrginalMesh;
            //    m_ParticleSystemRender.mesh.uv4 = m_OrginalUV4Data_Editor.ToArray();
            //    //m_ParticleSystemRender.mesh.colors = m_OrginalVerticesColor_Editor.ToArray();
            //}

            ////在播放模式下不卸载这个，交给运行时去卸载
            //if (isInPlayingMode)
            //{
            //    UnLoadData();
            //}
            //bInitOnVerticesEditor = false;
        }


        /// <summary>
        /// 保存数据
        /// </summary>
        public void Save()
        {
            //if (OrginalMesh.name.Contains("VerticesEditor"))
            //{
            //    Debug.LogError("保存顶点数据出错!原始网格不正确!赶紧记住这个问题是怎么重现的,然后找程序!");
            //    return;
            //}

            //Vector2[] currentUV4 = CurrentModifyMesh.uv4;
            //UV4Data.Clear();
            //UV4Data.AddRange(currentUV4);

            ////粒子系统不支持顶点色
            //if (!m_ParticleSystemRender)
            //{
            //    Color[] currentColor = CurrentModifyMesh.colors;
            //    VerticesColor.Clear();
            //    VerticesColor.AddRange(currentColor);
            //}


            //MeshKeyName = OrginalMesh.name;

        }


        /// <summary>
        /// 重设顶点数据
        /// </summary>
        public void ResetVerticesColorData()
        {
            ////粒子系统不支持顶点色
            //if (m_ParticleSystemRender)
            //{
            //    return;
            //}

            //if (m_DataInstance)
            //{
            //    //有顶点色数据，并且顶点数据匹配
            //    if (m_DataInstance.VerticesColor.Count == OrginalMesh.uv.Length)
            //    {
            //        CurrentModifyMesh.colors = m_DataInstance.VerticesColor.ToArray();
            //    }
            //}
            //else
            //{
            //    ResetVerticesColorFromFactory();

            //}
        }


        /// <summary>
        /// 出厂设置
        /// </summary>
        public void ResetVerticesColorFromFactory()
        {
            ////粒子系统不支持顶点色
            //if (m_ParticleSystemRender)
            //{
            //    return;
            //}

            ////原始颜色一样
            //if (m_OrginalVerticesColor_Editor.Count == OrginalMesh.uv.Length)
            //{
            //    CurrentModifyMesh.colors = m_OrginalVerticesColor_Editor.ToArray();
            //    return;
            //}

            //Color[] vC = new Color[OrginalMesh.uv.Length];
            //for (int i = 0; i < vC.Length; i++)
            //{
            //    vC[i] = Color.white;
            //}
            //CurrentModifyMesh.colors = vC;
            //VerticesColor.Clear();
        }

        #endregion
    }
}

