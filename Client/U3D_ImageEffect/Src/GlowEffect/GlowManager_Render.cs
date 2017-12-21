using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.Rendering;

namespace ImageEffects
{

    [RequireComponent(typeof(Camera))]
    public partial class GlowManager : ImageEffectNode
    {
        private CommandBuffer m_CommandBuffer = null;

        /// <summary>
        /// 渲染器和RenderInfo
        /// </summary>
        private Dictionary<Renderer, HashSet<GlowRenderInfo>> m_GlowRenderTable = new Dictionary<Renderer, HashSet<GlowRenderInfo>>();

        /// <summary>
        /// 原来的glow材质球和使用到的glow材质球
        /// </summary>
        private Dictionary<Material, Material> Orginal_GlowMatTable = new Dictionary<Material, Material>();

        /// <summary>
        /// 原来的glow材质球和使用到的glow材质球的引用计数，因为同一个材质球会被多个render给引用
        /// </summary>
        private Dictionary<Material, int> Orginal_GlowMatRefCountTable = new Dictionary<Material, int>();

        /// <summary>
        /// 渲染列表
        /// </summary>
        private HashSet<GlowRenderInfo> m_RenderInfoList = new HashSet<GlowRenderInfo>();


        /// <summary>
        /// 网格信息列表
        /// </summary>
        LinkedList<GlowMeshInfo> m_MeshInfoList = new LinkedList<GlowMeshInfo>();

        bool bNeedToProcess = false;

        /// <summary>
        /// 缓存的不透明实例材质球
        /// </summary>
        Queue<Material> OpaqueGlowMaterialInstanceCacheList = new Queue<Material>();

        /// <summary>
        /// 缓存的透明实例材质球
        /// </summary>
        Queue<Material> AlphaGlowMaterialInstanceCacheList = new Queue<Material>();

        /// <summary>
        /// 缓存的CutOutGlow实例材质球
        /// </summary>
        Queue<Material> CutOutGlowMaterialInstanceCacheList = new Queue<Material>();

        /// <summary>
        /// 缓存的CutOutGlow实例材质球
        /// </summary>
        Queue<Material> DefaultGlowMaterialInstanceCacheList = new Queue<Material>();

        /// <summary>
        /// 用于缓存材质Render的材质球实例，画完就立刻回收材质球，因为这个是外部的脚本，脚本删除的时候这里并不知道
        /// </summary>
        List<Material> ScriptMaterialCacheList = new List<Material>();

        /// <summary>
        /// 不透明的glow
        /// </summary>
        private Material OpaqueGlow = null;

        /// <summary>
        /// 透明的glow
        /// </summary>
        private Material AlphaGlow = null;

        /// <summary>
        /// CutOut物体的Glow
        /// </summary>
        private Material CutOutGlow = null;

        /// <summary>
        /// 默认的glowOutLine材质球
        /// </summary>
        private Material DefaultGlowMaterial = null;

        private const string DefaultGlowTag = "DefautGlow";

        private int validShaderLOD = 200;

        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }

        public void InitRender()
        {
            m_CommandBuffer = new CommandBuffer();
            m_CommandBuffer.name = "GlowCommandBuffer";

            if(Application.isPlaying)
            {
                OpaqueGlow = new Material(ImageEffectManager.API_GetShader("Effect/Glow/GlowOpaqueObjects"));
                AlphaGlow = new Material(ImageEffectManager.API_GetShader("Effect/Glow/GlowAlphaObjects"));
                CutOutGlow = new Material(ImageEffectManager.API_GetShader("Effect/Glow/GlowCutOutObjects"));
                DefaultGlowMaterial = new Material(OpaqueGlow);
                DefaultGlowMaterial.EnableKeyword("GLOW_GLOWCOLOR");
            }
        }

        public override void OnSceneCameraInit()
        {
            ImageEffectManager.API_ResigiterFoucusChage(HostCamera, OnFocusChange);
        }

        public void ClearRender()
        {
            foreach (KeyValuePair<Renderer, HashSet<GlowRenderInfo>> re in m_GlowRenderTable)
            {
                re.Value.Clear();
            }
            foreach (GlowRenderInfo re in m_RenderInfoList)
            {
                re.hostRender = null;
                DestroyImmediate(re.InstanceMaterial);
            }
            foreach(GlowMeshInfo re in m_MeshInfoList)
            {
                DestroyImmediate(re.InstanceMaterial);
            }

            foreach (Material re in OpaqueGlowMaterialInstanceCacheList)
            {
                DestroyImmediate(re);
            }

            foreach (Material re in AlphaGlowMaterialInstanceCacheList)
            {
                DestroyImmediate(re);
            }

            foreach (Material re in CutOutGlowMaterialInstanceCacheList)
            {
                DestroyImmediate(re);
            }

            foreach (Material re in DefaultGlowMaterialInstanceCacheList)
            {
                DestroyImmediate(re);
            }

            
            foreach (Material mat in ScriptMaterialCacheList)
            {
                DestroyImmediate(mat);
            }

           

            if(Application.isPlaying)
            {
                DestroyImmediate(DefaultGlowMaterial);
                DestroyImmediate(AlphaGlow);
                DestroyImmediate(OpaqueGlow);
                DestroyImmediate(CutOutGlow);
            }



            m_CommandBuffer = null;
            DefaultGlowMaterialInstanceCacheList.Clear();
            ScriptMaterialCacheList.Clear();
            OpaqueGlowMaterialInstanceCacheList.Clear();
            AlphaGlowMaterialInstanceCacheList.Clear();
            CutOutGlowMaterialInstanceCacheList.Clear();

            m_GlowRenderTable.Clear();
            m_RenderInfoList.Clear();
            m_MeshInfoList.Clear();
            Orginal_GlowMatTable.Clear();
            Orginal_GlowMatRefCountTable.Clear();
            ImageEffectManager.API_UnResigiterFoucusChage(HostCamera, OnFocusChange);
        }

        void OnFocusChange(bool foc)
        {
            if(foc)
            {
                //重新拷贝一次材质球
                foreach (GlowRenderInfo re in m_RenderInfoList)
                {
                    if(!re.mat)
                    {
                        continue;
                    }
                    re.InstanceMaterial.CopyPropertiesFromMaterial(re.mat);
                    re.InstanceMaterial.shaderKeywords = re.mat.shaderKeywords;
                }
            }

        }

        /// <summary>
        /// 推入网格，比如带子系统，绘制的是网格，不是Render
        /// </summary>
        /// <param name="m">网格</param>
        /// <param name="matrix">矩阵</param>
        /// <param name="mat">材质球</param>
        public void PushMesh(Mesh m, Matrix4x4 matrix, Material mat)
        {
            if(DebugMode)
            {
                return;
            }
            GlowMeshInfo info = new GlowMeshInfo();
            info.mesh = m;
            info.matrix = matrix;
            info.mat = mat;
            string GlowTags = mat.GetTag("RenderEffect", false);
            info.MaterialTag = GlowTags;
            Material instanceMat = null;
            
            //有同一个材质球了,直接用同一个，引用计数+1
            if(Orginal_GlowMatTable.TryGetValue(mat,out instanceMat))
            {
                info.InstanceMaterial = instanceMat;
                Orginal_GlowMatRefCountTable[mat]++;
            }
            else
            {
                info.InstanceMaterial = GetGlowMaterialFromTag(GlowTags);
                info.InstanceMaterial.CopyPropertiesFromMaterial(info.mat);
                info.InstanceMaterial.shaderKeywords = mat.shaderKeywords;
                Orginal_GlowMatTable.Add(info.mat, info.InstanceMaterial);
                Orginal_GlowMatRefCountTable.Add(info.mat, 1);
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
            if (!m_GlowRenderTable.ContainsKey(r))
            {
                GlowRenderInfo info = new GlowRenderInfo();
                info.mat = mat;
                info.subMeshIndex = subMeshIndex;
                info.hostRender = r;
                string GlowTags = mat.GetTag("RenderEffect", false);

                Material instanceMat = null;
                //有同一个材质球了,直接用同一个，引用计数+1
                if (Orginal_GlowMatTable.TryGetValue(mat, out instanceMat))
                {
                    info.InstanceMaterial = instanceMat;
                    Orginal_GlowMatRefCountTable[mat]++;
                }
                else
                {
                    info.InstanceMaterial = GetGlowMaterialFromTag(GlowTags);
                    info.InstanceMaterial.CopyPropertiesFromMaterial(info.mat);
                    info.InstanceMaterial.shaderKeywords = mat.shaderKeywords;
                    Orginal_GlowMatTable.Add(info.mat, info.InstanceMaterial);
                    Orginal_GlowMatRefCountTable.Add(info.mat, 1);
                }

                info.MaterialTag = GlowTags;
                matSet = new HashSet<GlowRenderInfo>();
                matSet.Add(info);
                m_RenderInfoList.Add(info);
                m_GlowRenderTable.Add(r, matSet);

            }
            else
            {
                matSet = m_GlowRenderTable[r];
                GlowRenderInfo info = new GlowRenderInfo();
                info.mat = mat;
                info.subMeshIndex = subMeshIndex;
                info.hostRender = r;
                string GlowTags = mat.GetTag("RenderEffect", false);

                Material instanceMat = null;
                //有同一个材质球了,直接用同一个，引用计数+1
                if (Orginal_GlowMatTable.TryGetValue(mat, out instanceMat))
                {
                    info.InstanceMaterial = instanceMat;
                    Orginal_GlowMatRefCountTable[mat]++;
                }
                else
                {
                    info.InstanceMaterial = GetGlowMaterialFromTag(GlowTags);
                    info.InstanceMaterial.CopyPropertiesFromMaterial(info.mat);
                    info.InstanceMaterial.shaderKeywords = mat.shaderKeywords;
                    Orginal_GlowMatTable.Add(info.mat, info.InstanceMaterial);
                    Orginal_GlowMatRefCountTable.Add(info.mat, 1);
                }

                info.MaterialTag = GlowTags;
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

            if (m_GlowRenderTable.ContainsKey(r))
            {
                HashSet<GlowRenderInfo> matSet = m_GlowRenderTable[r];
                foreach(GlowRenderInfo re in matSet)
                {
                    re.hostRender = null;
                    m_RenderInfoList.Remove(re);
                    if (Orginal_GlowMatTable.ContainsKey(re.mat))
                    {
                        int refCout = Orginal_GlowMatRefCountTable[re.mat];
                        refCout--;
                        if(refCout <= 0)
                        {
                            Orginal_GlowMatTable.Remove(re.mat);
                            Orginal_GlowMatRefCountTable.Remove(re.mat);
                            CacheGlowMaterial(re.InstanceMaterial, re.MaterialTag);

                        }
                        else
                        {
                            Orginal_GlowMatRefCountTable[re.mat] = refCout;
                        }
                    }
                    

                }
                matSet.Clear();
                m_GlowRenderTable.Remove(r);
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

            if (Orginal_GlowMatTable.ContainsKey(info.mat))
            {
                int refCout = Orginal_GlowMatRefCountTable[info.mat];
                refCout--;
                if (refCout <= 0)
                {
                    Orginal_GlowMatTable.Remove(info.mat);
                    Orginal_GlowMatRefCountTable.Remove(info.mat);
                    CacheGlowMaterial(info.InstanceMaterial, info.MaterialTag);

                }
                else
                {
                    Orginal_GlowMatRefCountTable[info.mat] = refCout;
                }
            }
        }

        /// <summary>
        /// 这个材质球是不是Glow材质球
        /// </summary>
        /// <param name="m"></param>
        /// <returns></returns>
        public static bool IsGlowMaterial(Material m)
        {
            if(!m)
            {
                return false;
            }

            string glowTag = m.GetTag("RenderEffect", false);
            if (glowTag.Contains("Glow"))
            {
                return true;
            }
            return false;
        }


        /// <summary>
        /// 根据材质球，取出对应的glow
        /// </summary>
        /// <param name="glowTag"></param>
        /// <returns></returns>
        public Material GetGlowMaterialFromTag(string glowTag)
        {
            Material s = null;
            switch (glowTag)
            {
                case "Glow":
                    if (!OpaqueGlow)
                    {
                        Debug.LogWarning("OpaqueGlow is null!");
                        return s;
                    }

                    if(OpaqueGlowMaterialInstanceCacheList.Count > 0)
                    {
                        s = OpaqueGlowMaterialInstanceCacheList.Dequeue();
                    }
                    else
                    {
                        s = new Material(OpaqueGlow);
                        //s.hideFlags = HideFlags.HideAndDontSave;
                    }

                    break;
                case "GlowTransparent":
                    if (!AlphaGlow)
                    {
                        Debug.LogWarning("AlphaGlow is null!");
                        return s;
                    }

                    if (AlphaGlowMaterialInstanceCacheList.Count > 0)
                    {
                        s = AlphaGlowMaterialInstanceCacheList.Dequeue();
                    }
                    else
                    {
                        s = new Material(AlphaGlow);
                        //s.hideFlags = HideFlags.HideAndDontSave;
                    }

                    break;
                case "GlowTransparentCutout":

                    if (!CutOutGlow)
                    {
                        Debug.LogWarning("CutOutGlow is null!");
                        return s;
                    }
                    if (CutOutGlowMaterialInstanceCacheList.Count > 0)
                    {
                        s = CutOutGlowMaterialInstanceCacheList.Dequeue();
                    }
                    else
                    {
                        s = new Material(CutOutGlow);
                        //s.hideFlags = HideFlags.HideAndDontSave;
                    }

                    break;
                case DefaultGlowTag:

                    if (!DefaultGlowMaterial)
                    {
                        Debug.LogWarning("DefaultGlowMaterial is null!");
                        return s;
                    }
                    if (DefaultGlowMaterialInstanceCacheList.Count > 0)
                    {
                        s = DefaultGlowMaterialInstanceCacheList.Dequeue();
                    }
                    else
                    {
                        s = new Material(DefaultGlowMaterial);
                        //s.hideFlags = HideFlags.HideAndDontSave;
                        s.EnableKeyword("GLOW_GLOWCOLOR");
                    }

                    break;

                    
            }
            return s;
        }


        public void CacheGlowMaterial(Material mat ,string tag)
        {
            switch (tag)
            {
                case "Glow":
                    OpaqueGlowMaterialInstanceCacheList.Enqueue(mat);
                    break;
                case "GlowTransparent":
                    AlphaGlowMaterialInstanceCacheList.Enqueue(mat);

                    break;
                case "GlowTransparentCutout":

                    CutOutGlowMaterialInstanceCacheList.Enqueue(mat);
                    break;

                case DefaultGlowTag:
                    DefaultGlowMaterialInstanceCacheList.Enqueue(mat);
                    break;
            }
        }

        /// <summary>
        /// 处理所有的渲染器
        /// </summary>
        /// <param name="source"></param>
        /// <param name="destination"></param>
        /// <param name="depthBuffer"></param>
        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        {

            if (!DebugMode)
            {
                if (m_GlowRenderTable.Count > 0 || m_MeshInfoList.Count > 0 || GlowNormalObject.list.Count > 0)
                {
                    bNeedToProcess = false;
                    m_CommandBuffer.Clear();

                    RenderTexture temp = null;
                    if (!GlowManager.isSupportedHighQualityGlow || ImageEffects.ImageEffectManager.ImageEffectShaderLOD < 300)
                    {
                        temp = RenderTexture.GetTemporary(source.width, source.height);
                    }
                    else
                    {
                        temp = RenderTexture.GetTemporary(source.width, source.height,0,RenderTextureFormat.ARGBHalf);
                    }

                    Graphics.SetRenderTarget(temp.colorBuffer, depthBuffer);
                    GL.Clear(false, true, Color.black);

                    DrawScriptRenderer();
                    DrawRenderer();
                    DrawMesh();
                    if (bNeedToProcess)
                    {
                        blur.BlurAndBlitBuffer(source,temp, destination,null, settings, highPrecision);
                    }

                    RenderTexture.ReleaseTemporary(temp);
                    return bNeedToProcess;
                }
            }
            else
            {
                //debug走回原来的replaceShader
                m_GlowCamReuse.screenRt = source;
                m_GlowCamReuse.depthBuffer = depthBuffer;
                m_GlowCamReuse.destion = destination;
                m_GlowCamReuse.HostCamera.Render();
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
                if (!r || !r.gameObject.activeSelf || !r.enabled || !r.isVisible || !re.mat)
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

                //回收
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
        /// 绘制GlowNormalObject的渲染器
        /// </summary>
        private void DrawScriptRenderer()
        {
            if(GlowNormalObject.list.Count <= 0)
            {
                return;
            }
            ScriptMaterialCacheList.Clear();
            bool bHaveAvildRender = false;
            foreach (KeyValuePair<Renderer, GlowObject> re in GlowNormalObject.list)
            {
                Renderer r = re.Key;
                GlowObject glowInfo = re.Value;
                if (!glowInfo)
                {
                    continue;
                }
                //渲染器可用，可见
                if (!glowInfo.enabled || !r || !r.gameObject.activeSelf || !r.enabled || !r.isVisible || !glowInfo)
                {
                    continue;
                }

                bNeedToProcess = true;
                bHaveAvildRender = true;

                Material mat = GetGlowMaterialFromTag(DefaultGlowTag);
                mat.SetColor("_GlowColor", glowInfo.color);
                mat.SetFloat("_GlowStrength", glowInfo.strength);
                m_CommandBuffer.DrawRenderer(r, mat);

                ScriptMaterialCacheList.Add(mat);
            }

            if(bHaveAvildRender)
            {
                Graphics.ExecuteCommandBuffer(m_CommandBuffer);
                m_CommandBuffer.Clear();

                //画完就立刻回收材质球，因为这个是外部的脚本，脚本删除的时候这里并不清楚
                foreach (Material mat in ScriptMaterialCacheList)
                {
                    CacheGlowMaterial(mat, DefaultGlowTag);
                }
            }
        }


        #region 更新材质球

        public bool ContainsMaterial(Material mat)
        {
            Material result = null;
            return Orginal_GlowMatTable.TryGetValue(mat, out result);
        }

        public void UpdateKeyWorld(Material mat, string KeyWorld,bool Enabled)
        {
            Material result = null;
            Orginal_GlowMatTable.TryGetValue(mat, out result);
            if(result)
            {
                if(Enabled)
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
            Orginal_GlowMatTable.TryGetValue(mat, out result);
            if (result)
            {
                result.SetColor(colorProperty, value);
            }
        }

        public void UpdateFloat(Material mat, string floatProperty, float value)
        {
            Material result = null;
            Orginal_GlowMatTable.TryGetValue(mat, out result);
            if (result)
            {
                result.SetFloat(floatProperty, value);
            }
        }

        public void UpdateUVAnimation(Material mat, string textureProperty, Vector2 uvTitle)
        {
            Material result = null;
            Orginal_GlowMatTable.TryGetValue(mat, out result);
            if (result)
            {
                result.SetTextureScale(textureProperty, uvTitle);
            }
        }

        public void UpdateUVScroll(Material mat, string textureProperty, Vector2 scroll)
        {
            Material result = null;
            Orginal_GlowMatTable.TryGetValue(mat, out result);
            if (result)
            {
                result.SetTextureOffset(textureProperty, scroll);
            }
        }
        #endregion
    }
}