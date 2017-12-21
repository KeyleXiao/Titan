using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.Rendering;

namespace ImageEffects
{
    [RequireComponent(typeof(Camera))]
    public partial class GlowOutLineManager : ImageEffectNode
    {
        /// <summary>
        /// 渲染命令队列
        /// </summary>
        private  CommandBuffer m_CommandBuffer = null;

        /// <summary>
        /// 渲染器和RenderInfo
        /// </summary>
        private  Dictionary<Renderer, HashSet<GlowRenderInfo>> m_OutLineRenderTable = new Dictionary<Renderer, HashSet<GlowRenderInfo>>();

        /// <summary>
        /// 渲染列表
        /// </summary>
        private HashSet<GlowRenderInfo> m_RenderInfoList = new HashSet<GlowRenderInfo>();

        /// <summary>
        /// 原来的glow材质球和使用到的glow材质球
        /// </summary>
        private Dictionary<Material, Material> Orginal_GlowOutLineMatTable = new Dictionary<Material, Material>();


        /// <summary>
        /// 原来的glow材质球和使用到的glow材质球的引用计数，因为同一个材质球会被多个render给引用
        /// </summary>
        private Dictionary<Material, int> Orginal_GlowOutLineMatRefCountTable = new Dictionary<Material, int>();

        /// <summary>
        /// 网格信息列表
        /// </summary>
        LinkedList<GlowMeshInfo> m_MeshInfoList = new LinkedList<GlowMeshInfo>();

        /// <summary>
        /// 用于缓存材质Render的材质球实例，画完就立刻回收材质球，因为这个是外部的脚本，脚本删除的时候这里并不知道
        /// </summary>
        List<Material> ScriptMaterialCacheList = new List<Material>();

        /// <summary>
        /// 缓存的实例材质球
        /// </summary>
        Queue<Material> GlowOutLineMaterialInstanceCacheList = new Queue<Material>();

        private Material GlowOutLineMaterial = null;
        bool bNeedToProcess = false;


        private int validShaderLOD = 200;

        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }

        public void InitRender()
        {
            m_CommandBuffer = new CommandBuffer();
            m_CommandBuffer.name = "GlowOutLineCommandBuffer";
            if(Application.isPlaying)
            {
                Shader s = ImageEffectManager.API_GetShader("Effect/Glow/GlowOutLineObjects");
                GlowOutLineMaterial = new Material(s);
            }
        }

        public override void OnSceneCameraInit()
        {
            ImageEffectManager.API_ResigiterFoucusChage(HostCamera, OnFocusChange);
        }


        public void ClearRender()
        {
            foreach (KeyValuePair<Renderer, HashSet<GlowRenderInfo>> re in m_OutLineRenderTable)
            {
                re.Value.Clear();
            }

            foreach (GlowRenderInfo re in m_RenderInfoList)
            {
                re.hostRender = null;
                DestroyImmediate(re.InstanceMaterial);
            }
            foreach (GlowMeshInfo re in m_MeshInfoList)
            {
                DestroyImmediate(re.InstanceMaterial);
            }

            foreach (Material re in GlowOutLineMaterialInstanceCacheList)
            {
                DestroyImmediate(re);
            }

            foreach (Material mat in ScriptMaterialCacheList)
            {
                DestroyImmediate(mat);
            }

            if(Application.isPlaying)
            {
                DestroyImmediate(GlowOutLineMaterial);
            }


            m_CommandBuffer = null;
            ScriptMaterialCacheList.Clear();
            GlowOutLineMaterialInstanceCacheList.Clear();
            m_OutLineRenderTable.Clear();
            m_RenderInfoList.Clear();
            m_MeshInfoList.Clear();
            Orginal_GlowOutLineMatTable.Clear();
            Orginal_GlowOutLineMatRefCountTable.Clear();
            ImageEffectManager.API_UnResigiterFoucusChage(HostCamera, OnFocusChange);
        }

        void OnFocusChange(bool foc)
        {
            if (foc)
            {
                //重新拷贝一次材质球
                foreach (GlowRenderInfo re in m_RenderInfoList)
                {
                    if (!re.mat)
                    {
                        continue;
                    }
                    re.InstanceMaterial.CopyPropertiesFromMaterial(re.mat);
                    re.InstanceMaterial.shaderKeywords = re.mat.shaderKeywords;
                }
            }

        }
       
        Material GetGlowOutLineMaterialInstance()
        {
            Material mat = null;
            if(GlowOutLineMaterial)
            {
                if(GlowOutLineMaterialInstanceCacheList.Count > 0)
                {
                    mat = GlowOutLineMaterialInstanceCacheList.Dequeue();
                }
                else
                {
                    mat = new Material(GlowOutLineMaterial);
                    //mat.hideFlags = HideFlags.HideAndDontSave;
                }
            }
            return mat;
        }

        /// <summary>
        /// 推入网格，比如带子系统，绘制的是网格，不是Render
        /// </summary>
        /// <param name="m">网格</param>
        /// <param name="matrix">矩阵</param>
        /// <param name="mat">材质球</param>
        public void PushMesh(Mesh m, Matrix4x4 matrix, Material mat)
        {
            if (DebugMode)
            {
                return;
            }
            GlowMeshInfo info = new GlowMeshInfo();
            info.mesh = m;
            info.matrix = matrix;
            info.mat = mat;
            if (GlowOutLineMaterial)
            {
                Material instanceMat = null;
                //有同一个材质球了,直接用同一个，引用计数+1
                if (Orginal_GlowOutLineMatTable.TryGetValue(mat, out instanceMat))
                {
                    info.InstanceMaterial = instanceMat;
                    Orginal_GlowOutLineMatRefCountTable[mat]++;
                }
                else
                {
                    info.InstanceMaterial = GetGlowOutLineMaterialInstance();
                    info.InstanceMaterial.CopyPropertiesFromMaterial(info.mat);
                    info.InstanceMaterial.shaderKeywords = mat.shaderKeywords;
                    Orginal_GlowOutLineMatTable.Add(info.mat, info.InstanceMaterial);
                    Orginal_GlowOutLineMatRefCountTable.Add(info.mat, 1);
                }
            }
            m_MeshInfoList.AddLast(info);
        }


        /// <summary>
        /// 推入渲染器，只正常的渲染器
        /// </summary>
        /// <param name="r">渲染器</param>
        /// <param name="mat">材质球</param>
        /// <param name="subMeshIndex">子网格索引</param>
        public  void PushRender(Renderer r,Material mat,int subMeshIndex = 0)
        {
            if (DebugMode)
            {
                return;
            }
            HashSet<GlowRenderInfo> matSet = null;
            if(!m_OutLineRenderTable.ContainsKey(r))
            {
                GlowRenderInfo info = new GlowRenderInfo();
                info.mat = mat;
                info.subMeshIndex = subMeshIndex;
                info.hostRender = r;
                if (GlowOutLineMaterial)
                {
                    Material instanceMat = null;
                    //有同一个材质球了,直接用同一个，引用计数+1
                    if (Orginal_GlowOutLineMatTable.TryGetValue(mat, out instanceMat))
                    {
                        info.InstanceMaterial = instanceMat;
                        Orginal_GlowOutLineMatRefCountTable[mat]++;
                    }
                    else
                    {
                        info.InstanceMaterial = GetGlowOutLineMaterialInstance();
                        info.InstanceMaterial.CopyPropertiesFromMaterial(info.mat);
                        info.InstanceMaterial.shaderKeywords = mat.shaderKeywords;
                        Orginal_GlowOutLineMatTable.Add(info.mat, info.InstanceMaterial);
                        Orginal_GlowOutLineMatRefCountTable.Add(info.mat, 1);
                    }
                }

                matSet = new HashSet<GlowRenderInfo>();
                matSet.Add(info);
                m_OutLineRenderTable.Add(r, matSet);
                m_RenderInfoList.Add(info);
            }
            else
            {
                matSet = m_OutLineRenderTable[r];
                GlowRenderInfo info = new GlowRenderInfo();
                info.mat = mat;
                info.subMeshIndex = subMeshIndex;
                info.hostRender = r;
                if (GlowOutLineMaterial)
                {
                    Material instanceMat = null;
                    //有同一个材质球了,直接用同一个，引用计数+1
                    if (Orginal_GlowOutLineMatTable.TryGetValue(mat, out instanceMat))
                    {
                        info.InstanceMaterial = instanceMat;
                        Orginal_GlowOutLineMatRefCountTable[mat]++;
                    }
                    else
                    {
                        info.InstanceMaterial = GetGlowOutLineMaterialInstance();
                        info.InstanceMaterial.CopyPropertiesFromMaterial(info.mat);
                        info.InstanceMaterial.shaderKeywords = mat.shaderKeywords;
                        Orginal_GlowOutLineMatTable.Add(info.mat, info.InstanceMaterial);
                        Orginal_GlowOutLineMatRefCountTable.Add(info.mat, 1);
                    }
                }

                matSet.Add(info);
                m_RenderInfoList.Add(info);
                
            }
        }


        /// <summary>
        /// 移除渲染器
        /// </summary>
        /// <param name="r"></param>
        /// <param name="mat"></param>
        /// <returns></returns>
        public bool RemoveRender(Renderer r, Material mat)
        {
            if (DebugMode)
            {
                return true;
            }
            if(m_OutLineRenderTable.ContainsKey(r))
            {
                HashSet<GlowRenderInfo>  matSet = m_OutLineRenderTable[r];

                foreach (GlowRenderInfo re in matSet)
                {
                    re.hostRender = null;
                    m_RenderInfoList.Remove(re);

                    if (Orginal_GlowOutLineMatTable.ContainsKey(re.mat))
                    {
                        int refCout = Orginal_GlowOutLineMatRefCountTable[re.mat];
                        refCout--;
                        if (refCout <= 0)
                        {
                            Orginal_GlowOutLineMatTable.Remove(re.mat);
                            Orginal_GlowOutLineMatRefCountTable.Remove(re.mat);

                            GlowOutLineMaterialInstanceCacheList.Enqueue(re.InstanceMaterial);
                        }
                        else
                        {
                            Orginal_GlowOutLineMatRefCountTable[re.mat] = refCout;
                        }
                    }
                }
                matSet.Clear();
                m_OutLineRenderTable.Remove(r);
                return true;
            }
            return false;
        }

        public void RemoveMesh(GlowMeshInfo info)
        {
            if (DebugMode)
            {
                return ;
            }

            if (Orginal_GlowOutLineMatTable.ContainsKey(info.mat))
            {
                int refCout = Orginal_GlowOutLineMatRefCountTable[info.mat];
                refCout--;
                if (refCout <= 0)
                {
                    Orginal_GlowOutLineMatTable.Remove(info.mat);
                    Orginal_GlowOutLineMatRefCountTable.Remove(info.mat);

                    GlowOutLineMaterialInstanceCacheList.Enqueue(info.InstanceMaterial);
                }
                else
                {
                    Orginal_GlowOutLineMatRefCountTable[info.mat] = refCout;
                }
            }

        }

        /// <summary>
        /// 这个材质球是不是GlowOutLine材质球
        /// </summary>
        /// <param name="m"></param>
        /// <returns></returns>
        public static bool IsGlowOutLineMaterial(Material m)
        {
            if(!m)
            {
                return false;
            }
            string glowOutLineTag = m.GetTag("GlowOutLine", false);
            if (glowOutLineTag.Equals("true"))
            {
                return true;
            }
            return false;
        }


        /// <summary>
        /// 处理所有的渲染器
        /// </summary>
        /// <param name="source"></param>
        /// <param name="destination"></param>
        /// <param name="depthBuffer"></param>
        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        {
            if(!DebugMode)
            {
                if (m_OutLineRenderTable.Count > 0 || m_MeshInfoList.Count > 0 || GlowOutLineObject.list.Count > 0)
                {
                    bNeedToProcess = false;
                    m_CommandBuffer.Clear();

                    RenderTexture temp = RenderTexture.GetTemporary(source.width , source.height );
                    RenderTexture temp2 = RenderTexture.GetTemporary(source.width / 2, source.height / 2);

                    Graphics.SetRenderTarget(temp.colorBuffer, depthBuffer);
                    GL.Clear(false, true, Color.black);

                    DrawScriptRenderer();
                    DrawRenderer();
                    DrawMesh();
                    if (bNeedToProcess)
                    {
                        blur.BlurAndBlitBuffer(source, temp, temp2, destination, settings, highPrecision);

                        RenderTexture.ReleaseTemporary(temp);
                        RenderTexture.ReleaseTemporary(temp2);
                        return bNeedToProcess;
                    }
                    RenderTexture.ReleaseTemporary(temp);
                    RenderTexture.ReleaseTemporary(temp2);
                }
            }
            else
            {
                m_GlowOutLineCamReuse.screenRt = source;
                m_GlowOutLineCamReuse.depthBuffer = depthBuffer;
                m_GlowOutLineCamReuse.destion = destination;
                m_GlowOutLineCamReuse.HostCamera.Render();
                return true;
            }
            return false;
        }


        /// <summary>
        /// 绘制渲染器
        /// </summary>
        private void DrawRenderer()
        {
            foreach (GlowRenderInfo re in m_RenderInfoList)
            {
                Renderer r = re.hostRender;
                //渲染器可用，可见
                if (!r || !r.gameObject.activeSelf || !r.enabled || !r.isVisible)
                {
                    continue;
                }

                bNeedToProcess = true;
                m_CommandBuffer.DrawRenderer(r, re.InstanceMaterial, re.subMeshIndex);
            }

            Graphics.ExecuteCommandBuffer(m_CommandBuffer);
            m_CommandBuffer.Clear();
        }


        /// <summary>
        /// 绘制网格
        /// </summary>
        private void DrawMesh()
        {
            if (m_MeshInfoList.Count > 0)
            {
                foreach (GlowMeshInfo info in m_MeshInfoList)
                {
                    //材质球有效
                    if (!info.mat)
                    {
                        continue;
                    }

                    bNeedToProcess = true;
                    m_CommandBuffer.DrawMesh(info.mesh, info.matrix, info.InstanceMaterial);
                }

                Graphics.ExecuteCommandBuffer(m_CommandBuffer);

                foreach (GlowMeshInfo info in m_MeshInfoList)
                {
                    //材质球有效
                    if (!info.mat)
                    {
                        continue;
                    }

                    RemoveMesh(info);
                }

                m_CommandBuffer.Clear();
                m_MeshInfoList.Clear();
            }

        }


        /// <summary>
        /// 绘制GlowOutLineObject的渲染器
        /// </summary>
        private void DrawScriptRenderer()
        {
            if (GlowOutLineObject.list.Count <= 0)
            {
                return;
            }
            
            ScriptMaterialCacheList.Clear();
            bool bHaveAvildRender = false;
            foreach (KeyValuePair<Renderer, GlowObject> re in GlowOutLineObject.list)
            {
                Renderer r = re.Key;
                GlowObject glowInfo = re.Value;
                if(!glowInfo)
                {
                    continue;
                }
                //渲染器可用，可见
                if ( !glowInfo.enabled || !r || !r.gameObject.activeSelf || !r.enabled || !r.isVisible || !glowInfo)
                {
                    continue;
                }

                bNeedToProcess = true;
                bHaveAvildRender = true;
                Material mat = GetGlowOutLineMaterialInstance();
                mat.SetColor("_Color", glowInfo.color);
                mat.SetFloat("_ColorStrength", glowInfo.strength);
                mat.SetFloat("_Cutoff", glowInfo.AlphaCutoff);
                m_CommandBuffer.DrawRenderer(r, mat);

                ScriptMaterialCacheList.Add(mat);
            }

            if (bHaveAvildRender)
            {
                Graphics.ExecuteCommandBuffer(m_CommandBuffer);
                m_CommandBuffer.Clear();

                //画完就立刻回收材质球，因为这个是外部的脚本，脚本删除的时候这里并不清楚
                foreach (Material mat in ScriptMaterialCacheList)
                {
                    GlowOutLineMaterialInstanceCacheList.Enqueue(mat);
                }
            }

            
        }

        #region 更新材质球

        public bool ContainsMaterial(Material mat)
        {
            Material result = null;
            return Orginal_GlowOutLineMatTable.TryGetValue(mat, out result);
        }

        public void UpdateKeyWorld(Material mat, string KeyWorld, bool Enabled)
        {
            Material result = null;
            Orginal_GlowOutLineMatTable.TryGetValue(mat, out result);
            if (result)
            {
                if (Enabled)
                {
                    result.EnableKeyword(KeyWorld);
                }
                else
                {
                    result.DisableKeyword(KeyWorld);
                }
            }

        }

        public void UpdateColor(Material mat, string colorProperty, Color value)
        {
            Material result = null;
            Orginal_GlowOutLineMatTable.TryGetValue(mat, out result);
            if (result)
            {
                result.SetColor(colorProperty, value);
            }
        }

        public void UpdateFloat(Material mat, string floatProperty, float value)
        {
            Material result = null;
            Orginal_GlowOutLineMatTable.TryGetValue(mat, out result);
            if (result)
            {
                result.SetFloat(floatProperty, value);
            }
        }

        public void UpdateUVAnimation(Material mat, string textureProperty, Vector2 uvTitle)
        {
            Material result = null;
            Orginal_GlowOutLineMatTable.TryGetValue(mat, out result);
            if (result)
            {
                result.SetTextureScale(textureProperty, uvTitle);
            }
        }

        public void UpdateUVScroll(Material mat, string textureProperty, Vector2 scroll)
        {
            Material result = null;
            Orginal_GlowOutLineMatTable.TryGetValue(mat, out result);
            if (result)
            {
                result.SetTextureOffset(textureProperty, scroll);
            }
        }
        #endregion
    }
}