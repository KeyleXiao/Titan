using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.Rendering;

namespace ImageEffects
{
    [RequireComponent(typeof(Camera))]
    public partial class DissolveGlowManager : ImageEffectNode
    {
        /// <summary>
        /// 命令buffer
        /// </summary>
        private CommandBuffer m_CommandBuffer = null;

        /// <summary>
        /// 渲染器和RenderInfo
        /// </summary>
        private Dictionary<Renderer, HashSet<GlowRenderInfo>> m_DissloveGlowRenderTable = new Dictionary<Renderer, HashSet<GlowRenderInfo>>();

        /// <summary>
        /// 渲染列表
        /// </summary>
        private HashSet<GlowRenderInfo> m_RenderInfoList = new HashSet<GlowRenderInfo>();

        /// <summary>
        /// 原来的glow材质球和使用到的glow材质球
        /// </summary>
        private Dictionary<Material, Material> Orginal_DissloveGlowMatTable = new Dictionary<Material, Material>();

        /// <summary>
        /// 原来的glow材质球和使用到的glow材质球的引用计数，因为同一个材质球会被多个render给引用
        /// </summary>
        private Dictionary<Material, int> Orginal_DissloveGlowMatRefCountTable = new Dictionary<Material, int>();

        /// <summary>
        /// Mesh渲染列表
        /// </summary>
        LinkedList<GlowMeshInfo> m_MeshInfoList = new LinkedList<GlowMeshInfo>();

        /// <summary>
        /// 缓存的实例材质球
        /// </summary>
        Queue<Material> DissloveOpaqueGlowMaterialInstanceCacheList = new Queue<Material>();

        /// <summary>
        /// 缓存的实例材质球
        /// </summary>
        Queue<Material> DissloveAlphaGlowMaterialInstanceCacheList = new Queue<Material>();


        
        /// <summary>
        /// 默认的材质球，用于脚本DissolveGlow
        /// </summary>
        private Material DefaultDissolveGlowMaterial = null;

        private Material GlowDissloveOpaqueMaterial = null;
        private Material GlowDissloveAlphaMaterial = null;
        bool bNeedToProcess = false;
        private int validShaderLOD = 500;

        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }


        public void InitRender()
        {
            m_CommandBuffer = new CommandBuffer();
            m_CommandBuffer.name = "DissolveGlowCommandBuffer";
            if(Application.isPlaying)
            {
                Shader s = ImageEffectManager.API_GetShader("Effect/Glow/GlowDissloveOpaqueObjects");
                GlowDissloveOpaqueMaterial = new Material(s);

                s = ImageEffectManager.API_GetShader("Effect/Glow/GlowDissloveAlphaObjects");
                GlowDissloveAlphaMaterial = new Material(s);

                DefaultDissolveGlowMaterial = new Material(s);
            }
            
        }
        public override void OnSceneCameraInit()
        {
            ImageEffectManager.API_ResigiterFoucusChage(HostCamera, OnFocusChange);
        }

        public void ClearRender()
        {
            foreach (KeyValuePair<Renderer, HashSet<GlowRenderInfo>> re in m_DissloveGlowRenderTable)
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

            foreach (Material re in DissloveOpaqueGlowMaterialInstanceCacheList)
            {
                DestroyImmediate(re);
            }

            foreach (Material re in DissloveAlphaGlowMaterialInstanceCacheList)
            {
                DestroyImmediate(re);
            }
            
            if(Application.isPlaying)
            {
                DestroyImmediate(GlowDissloveOpaqueMaterial);
                DestroyImmediate(GlowDissloveAlphaMaterial);
                DestroyImmediate(DefaultDissolveGlowMaterial);
            }

            DissloveOpaqueGlowMaterialInstanceCacheList.Clear();
            DissloveAlphaGlowMaterialInstanceCacheList.Clear();
            m_MeshInfoList.Clear();
            m_CommandBuffer = null;
            m_DissloveGlowRenderTable.Clear();
            m_RenderInfoList.Clear();
            Orginal_DissloveGlowMatTable.Clear();
            Orginal_DissloveGlowMatRefCountTable.Clear();
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

        Material GetDissloveGlowMaterialInstance(string glowTag)
        {
            Material mat = null;
            switch(glowTag)
            {
                case "OpaqueDissolveGolw":

                    if (GlowDissloveOpaqueMaterial)
                    {
                        if (DissloveOpaqueGlowMaterialInstanceCacheList.Count > 0)
                        {
                            mat = DissloveOpaqueGlowMaterialInstanceCacheList.Dequeue();
                        }
                        else
                        {
                            mat = new Material(GlowDissloveOpaqueMaterial);
                            //mat.hideFlags = HideFlags.HideAndDontSave;
                        }
                    }

                    break;
                case "TransparentDissolveGolw":


                    if (GlowDissloveAlphaMaterial)
                    {
                        if (DissloveAlphaGlowMaterialInstanceCacheList.Count > 0)
                        {
                            mat = DissloveAlphaGlowMaterialInstanceCacheList.Dequeue();
                        }
                        else
                        {
                            mat = new Material(GlowDissloveAlphaMaterial);
                            //mat.hideFlags = HideFlags.HideAndDontSave;
                        }
                    }

                    break;
            }

            return mat;
        }

        public void CacheDissloveGlowMaterial(Material mat, string tag)
        {
            switch (tag)
            {
                case "OpaqueDissolveGolw":
                    DissloveOpaqueGlowMaterialInstanceCacheList.Enqueue(mat);
                    break;
                case "TransparentDissolveGolw":
                    DissloveAlphaGlowMaterialInstanceCacheList.Enqueue(mat);

                    break;
            }
        }


        /// <summary>
        /// 推入网格，比如带子系统，绘制的是网格，不是Render
        /// </summary>
        /// <param name="m">网格</param>
        /// <param name="matrix">矩阵</param>
        /// <param name="mat">材质球</param>
        public void PushMesh(Mesh m,Matrix4x4 matrix,Material mat)
        {
            if (DebugMode)
            {
                return;
            }
            GlowMeshInfo info = new GlowMeshInfo();
            info.mesh = m;
            info.matrix = matrix;
            info.mat = mat;
            string GlowTags = mat.GetTag("DissolveGolw", false);
            info.MaterialTag = GlowTags;
            if (GlowDissloveOpaqueMaterial)
            {
                Material instanceMat = null;
                //有同一个材质球了,直接用同一个，引用计数+1
                if (Orginal_DissloveGlowMatTable.TryGetValue(mat, out instanceMat))
                {
                    info.InstanceMaterial = instanceMat;
                    Orginal_DissloveGlowMatRefCountTable[mat]++;
                }
                else
                {
                    info.InstanceMaterial = GetDissloveGlowMaterialInstance(GlowTags);
                    info.InstanceMaterial.CopyPropertiesFromMaterial(info.mat);
                    info.InstanceMaterial.shaderKeywords = mat.shaderKeywords;
                    Orginal_DissloveGlowMatTable.Add(info.mat, info.InstanceMaterial);
                    Orginal_DissloveGlowMatRefCountTable.Add(info.mat, 1);
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
        public void PushRender(Renderer r, Material mat, int subMeshIndex = 0)
        {
            if (DebugMode)
            {
                return;
            }
            HashSet<GlowRenderInfo> matSet = null;
            if (!m_DissloveGlowRenderTable.ContainsKey(r))
            {
                GlowRenderInfo info = new GlowRenderInfo();
                info.mat = mat;
                info.subMeshIndex = subMeshIndex;
                info.hostRender = r;
                string GlowTags = mat.GetTag("DissolveGolw", false);
                info.MaterialTag = GlowTags;
                if (GlowDissloveOpaqueMaterial)
                {
                    Material instanceMat = null;
                    //有同一个材质球了,直接用同一个，引用计数+1
                    if (Orginal_DissloveGlowMatTable.TryGetValue(mat, out instanceMat))
                    {
                        info.InstanceMaterial = instanceMat;
                        Orginal_DissloveGlowMatRefCountTable[mat]++;
                    }
                    else
                    {
                        info.InstanceMaterial = GetDissloveGlowMaterialInstance(GlowTags);
                        info.InstanceMaterial.CopyPropertiesFromMaterial(info.mat);
                        info.InstanceMaterial.shaderKeywords = mat.shaderKeywords;
                        Orginal_DissloveGlowMatTable.Add(info.mat, info.InstanceMaterial);
                        Orginal_DissloveGlowMatRefCountTable.Add(info.mat, 1);
                    }
                }

                matSet = new HashSet<GlowRenderInfo>();
                matSet.Add(info);
                m_DissloveGlowRenderTable.Add(r, matSet);
                m_RenderInfoList.Add(info);

            }
            else
            {
                matSet = m_DissloveGlowRenderTable[r];
  
                GlowRenderInfo info = new GlowRenderInfo();
                info.mat = mat;
                info.subMeshIndex = subMeshIndex;
                info.hostRender = r;
                string GlowTags = mat.GetTag("DissolveGolw", false);
                info.MaterialTag = GlowTags;
                if (GlowDissloveOpaqueMaterial)
                {
                    Material instanceMat = null;
                    //有同一个材质球了,直接用同一个，引用计数+1
                    if (Orginal_DissloveGlowMatTable.TryGetValue(mat, out instanceMat))
                    {
                        info.InstanceMaterial = instanceMat;
                        Orginal_DissloveGlowMatRefCountTable[mat]++;
                    }
                    else
                    {
                        info.InstanceMaterial = GetDissloveGlowMaterialInstance(GlowTags);
                        info.InstanceMaterial.CopyPropertiesFromMaterial(info.mat);
                        info.InstanceMaterial.shaderKeywords = mat.shaderKeywords;
                        Orginal_DissloveGlowMatTable.Add(info.mat, info.InstanceMaterial);
                        Orginal_DissloveGlowMatRefCountTable.Add(info.mat, 1);
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
            if (m_DissloveGlowRenderTable.ContainsKey(r))
            {
                HashSet<GlowRenderInfo> matSet = m_DissloveGlowRenderTable[r];
                foreach(GlowRenderInfo re in matSet)
                {
                    re.hostRender = null;
                    m_RenderInfoList.Remove(re);
                    if (Orginal_DissloveGlowMatTable.ContainsKey(re.mat))
                    {
                        int refCout = Orginal_DissloveGlowMatRefCountTable[re.mat];
                        refCout--;
                        if (refCout <= 0)
                        {
                            Orginal_DissloveGlowMatTable.Remove(re.mat);
                            Orginal_DissloveGlowMatRefCountTable.Remove(re.mat);
                            CacheDissloveGlowMaterial(re.InstanceMaterial, re.MaterialTag);
                        }
                        else
                        {
                            Orginal_DissloveGlowMatRefCountTable[re.mat] = refCout;
                        }
                    }
                }
                matSet.Clear();
                m_DissloveGlowRenderTable.Remove(r);
                return true;
            }

            return false;
        }

        public void RemoveMesh(GlowMeshInfo info)
        {
            if (DebugMode)
            {
                return;
            }

            if (Orginal_DissloveGlowMatTable.ContainsKey(info.mat))
            {
                int refCout = Orginal_DissloveGlowMatRefCountTable[info.mat];
                refCout--;
                if (refCout <= 0)
                {
                    Orginal_DissloveGlowMatTable.Remove(info.mat);
                    Orginal_DissloveGlowMatRefCountTable.Remove(info.mat);
                    CacheDissloveGlowMaterial(info.InstanceMaterial, info.MaterialTag);
                }
                else
                {
                    Orginal_DissloveGlowMatRefCountTable[info.mat] = refCout;
                }
            }
        }

        /// <summary>
        /// 这个材质球是不是DissloveGlow材质球
        /// </summary>
        /// <param name="m"></param>
        /// <returns></returns>
        public static bool IsDissloveGlowMaterial(Material m)
        {
            if(!m)
            {
                return false;
            }
            string glowTag = m.GetTag("DissolveGolw", false);
            if (glowTag.Contains("DissolveGolw"))
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
            if (!ImageEffects.ImageEffectManager.AdvanceImangeEffectEnabled)
            {
                return false;
            }
            if (!DebugMode)
            {
                if (m_DissloveGlowRenderTable.Count > 0 || m_MeshInfoList.Count > 0 || DissloveGlowObject.list.Count > 0)
                {
                    bNeedToProcess = false;
                    m_CommandBuffer.Clear();

                    RenderTexture temp = RenderTexture.GetTemporary(source.width , source.height );

                    Graphics.SetRenderTarget(temp.colorBuffer, depthBuffer);
                    GL.Clear(false, true, Color.black);

                    DrawScriptRenderer();
                    DrawRenderer();
                    DrawMesh();
                    if (bNeedToProcess)
                    {
                        blur.BlurAndBlitBuffer(source, temp, destination,null, settings, highPrecision);
                    }

                    RenderTexture.ReleaseTemporary(temp);
                    return bNeedToProcess;

                }
            }
            else
            {

                m_DissloveGlowCamReuse.screenRt = source;
                m_DissloveGlowCamReuse.depthBuffer = depthBuffer;
                m_DissloveGlowCamReuse.destion = destination;
                m_DissloveGlowCamReuse.HostCamera.Render();
            
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
                    CacheDissloveGlowMaterial(info.InstanceMaterial, info.MaterialTag);
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
        /// 绘制DissolveGlow的渲染器
        /// </summary>
        private void DrawScriptRenderer()
        {
            if(DissloveGlowObject.list.Count <= 0)
            {
                return;
            }

            Debug.LogWarning("暂不支持脚本DissloveGlow！");
            return;

        }

        #region 更新材质球

        public bool ContainsMaterial(Material mat)
        {
            Material result = null;
            return Orginal_DissloveGlowMatTable.TryGetValue(mat, out result);
        }

        public void UpdateKeyWorld(Material mat, string KeyWorld, bool Enabled)
        {
            Material result = null;
            Orginal_DissloveGlowMatTable.TryGetValue(mat, out result);
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
            Orginal_DissloveGlowMatTable.TryGetValue(mat, out result);
            if (result)
            {
                result.SetColor(colorProperty, value);
            }
        }

        public void UpdateFloat(Material mat, string floatProperty, float value)
        {
            Material result = null;
            Orginal_DissloveGlowMatTable.TryGetValue(mat, out result);
            if (result)
            {
                result.SetFloat(floatProperty, value);
            }
        }

        public void UpdateUVAnimation(Material mat, string textureProperty, Vector2 uvTitle)
        {
            Material result = null;
            Orginal_DissloveGlowMatTable.TryGetValue(mat, out result);
            if (result)
            {
                result.SetTextureScale(textureProperty, uvTitle);
            }
        }

        public void UpdateUVScroll(Material mat, string textureProperty, Vector2 scroll)
        {
            Material result = null;
            Orginal_DissloveGlowMatTable.TryGetValue(mat, out result);
            if (result)
            {
                result.SetTextureOffset(textureProperty, scroll);
            }
        }
        #endregion
    }
}