using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

namespace Effect
{
    public sealed class Effect_UVTextureAnimator : MonoBehaviour, ISaveSerializedData
    {

        public EffectRunMode RunMode = EffectRunMode.Self;
        public string TexturePropertyName = "_MainTex";
        
        public float fStartDelay = 0.0f;
        public int Rows = 4;
        public int Columns = 4;
        public float Fps = 20;
        public Vector2 SelfTiling = new Vector2();
        public int AniTimes = -1;
        public bool IsReverse = false;
        

        /// <summary>
        /// 是否使用模型UV
        /// </summary>
        public bool UseModelUV = true;
        private bool OldUseModelUV = true;

        /// <summary>
        /// Self模式,使用共享材质球，复制就不会丢失材质球，但是两个相同的会影响到，这个选项只是调试的时候开
        /// </summary>
        public bool UseSharedMaterial = false;

        private int OffsetMat = 0;
        private bool IsRandomOffsetForInctance = false;
        private bool isInizialised;
        private int index;
        private int count, allCount;
        private float deltaFps;
        private bool isVisible;
        private bool isCorutineStarted;
        private bool bPlay = false;

        private List<Material> matInstances = new List<Material>();
        private Dictionary<Material, Vector2> OrginalScale = new Dictionary<Material, Vector2>();

        private Renderer[] m_Renders;
        private Projector[] m_Projectors;
        private bool bStartPlayYield = false;

        public System.Object SaveSerializedData()
        {
            m_Projectors = GetComponents<Projector>();
            List<Material> matlist = new List<Material>();
            foreach (Projector pro in m_Projectors)
            {
                matlist.Add(pro.material);
            }
            m_Projectors = null;
            return matlist;
        }

        public void SetSerializedData(System.Object data)
        {
            if (null == data)
            {
                return;
            }
            List<Material> matlist = data as List<Material>;
            if (null == data)
            {
                Debug.LogError("Effect_UVTextureAnimator-- ConverSerializedData faild");
                return;
            }
            m_Projectors = GetComponents<Projector>();

            if (matlist.Count != m_Projectors.Length)
            {
                Debug.LogError("Effect_UVTextureAnimator-- orginalMaterialMap not match");
                return;
            }
            for (int i = 0; i < m_Projectors.Length; i++)
            {
                m_Projectors[i].material = matlist[i];
            }

            m_Projectors = null;
        }

        public void OnDestroy()
        {
            if(UseSharedMaterial)
            {
                if (m_Renders != null && RunMode == EffectRunMode.Self)
                {
                    if (m_Renders.Length > 0)
                    {
                        for (int i = 0; i < m_Renders.Length; i++)
                        {
                            Renderer r = m_Renders[i];
                            if (!r.sharedMaterial)
                            {
                                continue;
                            }
                            EffectUtily.GabageRender(r, r.sharedMaterial);
                            r.sharedMaterial.SetTextureScale(TexturePropertyName, OrginalScale[r.sharedMaterial]);
                        }

                    }
                }
            }


            OrginalScale.Clear();
            matInstances.Clear();
            matInstances = null;

            m_Renders = null;
            m_Projectors = null;

        }

        public Renderer[] GetRendersOnBakeTime()
        {
            if (!Application.isPlaying && Application.isEditor)
            {
                List<Renderer> RenderList = new List<Renderer>();

                if (RunMode == EffectRunMode.Self)
                {
                    RenderList.AddRange(GetComponents<Renderer>());
                    RenderList.AddRange(GetComponents<LineRenderer>());
                }

                return RenderList.ToArray();
            }
            else
            {
                Debug.LogWarning("GetRendersOnBakeTime只能在编辑器下的非播放模式下调用!");
                return new Renderer[0];
            }

        }

        /// <summary>
        /// 拷贝编辑器变量
        /// </summary>
        /// <param name="FromInstance">从哪一个拷贝</param>
        public void CopyInspectorParamsTo(Effect_UVTextureAnimator FromInstance)
        {
            RunMode = FromInstance.RunMode;
            TexturePropertyName = FromInstance.TexturePropertyName;
            
            Rows = FromInstance.Rows;
            Columns = FromInstance.Columns;
            Fps = FromInstance.Fps;
            OffsetMat = FromInstance.OffsetMat;
            SelfTiling = FromInstance.SelfTiling;
            AniTimes = FromInstance.AniTimes;
            fStartDelay = FromInstance.fStartDelay;
            IsReverse = FromInstance.IsReverse;
            IsRandomOffsetForInctance = FromInstance.IsRandomOffsetForInctance;
        }

        public void SetMaterialInstance(List<Material> MatInstances)
        {
            if (RunMode == EffectRunMode.Self)
            {
                return;
            }


            matInstances.Clear();
            matInstances.AddRange(MatInstances);
            InitDefaultVariables();
        }

        public void SetMaterialInstance(HashSet<Material> MatInstances)
        {
            if (RunMode == EffectRunMode.Self)
            {
                return;
            }


            matInstances.Clear();
            matInstances.AddRange(MatInstances);
            InitDefaultVariables();
        }

        public void SetMaterialInstance(Material[] MatInstances)
        {
            if (RunMode == EffectRunMode.Self)
            {
                return;
            }

            matInstances.Clear();
            matInstances.AddRange(MatInstances);
            InitDefaultVariables();
        }

        public void SetMaterialInstance(Material MatInstance)
        {
            if (RunMode == EffectRunMode.Self)
            {
                return;
            }

            matInstances.Clear();
            matInstances.Add(MatInstance);
            InitDefaultVariables();
        }

        public void ClearMaterialInstance()
        {
            matInstances.Clear();
        }


        public void SetRenderQuen(int RenderQuen)
        {
            if (matInstances.Count > 0)
            {
                foreach (Material mat in matInstances)
                {
                    mat.renderQueue = RenderQuen;
                }
            }
        }

        public void Play()
        {
            if (bPlay)
            {
                return;
            }

            if (!bStartPlayYield)
            {
                if (fStartDelay > 0.03f)
                {
                    GameUtil.StartCoroutineHelper(this,DelayPlay());
                    bStartPlayYield = true;
                }
                else
                {
                    PlayImpl();
                }
            }
        }

        IEnumerator DelayPlay()
        {
            yield return new WaitForSeconds(fStartDelay);
            bStartPlayYield = false;
            PlayImpl();
        }

        private void PlayImpl()
        {
            bPlay = true;
            InitDefaultVariables();
            if (!isCorutineStarted)
            {
                GameUtil.StartCoroutineHelper(this,UpdateCorutine());
            }
        }

        public void Stop()
        {
            bPlay = false;
            StopAllCoroutines();
            isCorutineStarted = false;
        }

        public void GenerateSelfMaterial()
        {
            //if (IncludeChildren)
            //{
            //    m_Renders = GetComponentsInChildren<Renderer>();
            //    m_Projectors = GetComponentsInChildren<Projector>();
            //    m_LineRenders = GetComponentsInChildren<LineRenderer>();
            //}
            //else
            {
                m_Renders = GetComponents<Renderer>();
                m_Projectors = GetComponents<Projector>();
            }

            InitMaterial();
            InitDefaultVariables();
            ChangeUseModeUV();
        }

        #region 私有

        public void Start()
        {
            if (RunMode == EffectRunMode.Self)
            {
                GenerateSelfMaterial();
            }

            isInizialised = true;
            isVisible = true;
            GameUtil.StartCoroutineHelper(this,UpdateCorutine());
        }

        private void InitDefaultVariables()
        {
            allCount = 0;
            deltaFps = 1f / Fps;
            count = Rows * Columns;
            index = Columns - 1;
            Vector2 offset = new Vector2((float)index / Columns - (index / Columns),
              1 - (index / Columns) / (float)Rows);
            OffsetMat = !IsRandomOffsetForInctance
              ? OffsetMat - (OffsetMat / count) * count
              : UnityEngine.Random.Range(0, count);
            Vector2 size = SelfTiling == Vector2.zero ? new Vector2(1f / Columns, 1f / Rows) : SelfTiling;

            if (matInstances.Count > 0)
            {
                foreach (Material mat in matInstances)
                {
                    mat.SetTextureScale(TexturePropertyName, size);
                }
            }
        }

        private void InitMaterial()
        {

            if (matInstances.Count > 0) return;

            if (m_Renders.Length > 0)
            {
                for (int i = 0; i < m_Renders.Length; i++)
                {
                    Renderer r = m_Renders[i];
                    if (!r.sharedMaterial)
                    {
                        continue;
                    }
                    if (RunMode == EffectRunMode.Self)
                    {
                        if (UseSharedMaterial)
                        {
                            matInstances.Add(r.sharedMaterial);
                            EffectUtily.DispatchRender(r, r.sharedMaterial, i);
                            OrginalScale.Add(r.sharedMaterial, r.sharedMaterial.GetTextureScale(TexturePropertyName));
                        }
                        else
                        {
                            matInstances.Add(r.material);
                            EffectUtily.DispatchRender(r, r.material, i);
                        }
                    }

                }
            }
            else
            {
                if (m_Projectors.Length > 0)
                {
                    //Debug.LogWarning("不支持分发渲染器,以后会实现");
                    foreach (Projector r in m_Projectors)
                    {
                        if (!r.material)
                        {
                            continue;
                        }
                        if (!r.material.name.EndsWith("(Instance)"))
                        {
                            r.material = new Material(r.material) { name = r.material.name + " (Instance)" };
                        }
                        matInstances.Add(r.material);
                    }

                }
            }
        }

        #region CorutineCode

        public void OnEnable()
        {
            if (!isInizialised)
                return;
            InitDefaultVariables();
            isVisible = true;
            if (!isCorutineStarted)
                GameUtil.StartCoroutineHelper(this,UpdateCorutine());
        }

        public void OnDisable()
        {
            isCorutineStarted = false;
            isVisible = false;
            StopAllCoroutines();
        }

        public void OnBecameVisible()
        {
            isVisible = true;
            if (!isCorutineStarted)
                GameUtil.StartCoroutineHelper(this,UpdateCorutine());
        }

        public void OnBecameInvisible()
        {
            isVisible = false;
        }

        private IEnumerator UpdateCorutine()
        {
            isCorutineStarted = true;
            while (isVisible)
            {
                UpdateCorutineFrame();
                if (AniTimes > 0 && allCount > count * AniTimes)
                {
                    break;
                }
                yield return new WaitForSeconds(deltaFps);
            }
            isCorutineStarted = false;
        }

        #endregion CorutineCode

        private void UpdateCorutineFrame()
        {
            if (matInstances.Count <= 0 || !bPlay)
                return;

            ++allCount;
            if (IsReverse)
                --index;
            else
                ++index;
            if (index >= count)
                index = 0;

                Vector2 offset;
                if (IsRandomOffsetForInctance)
                {
                    var idx = index + OffsetMat;
                    offset = new Vector2((float)idx / Columns - (idx / Columns),
                      1 - (idx / Columns) / (float)Rows);
                }
                else
                {
                    offset = new Vector2((float)index / Columns - (index / Columns),
                      1 - (index / Columns) / (float)Rows);
                }

                foreach (Material mat in matInstances)
                {
                    mat.SetTextureOffset(TexturePropertyName, offset);
                    EffectUtily.UpdateUVAnimation(mat, TexturePropertyName, offset);
                }

                if (OldUseModelUV != UseModelUV)
                {
                    ChangeUseModeUV();
                }
        }

        private void ChangeUseModeUV()
        {

            if (matInstances.Count > 0)
            {
                foreach (Material mat in matInstances)
                {
                    if (UseModelUV)
                    {
                        mat.EnableKeyword("USE_MODE_UV_ON");
                        mat.DisableKeyword("USE_MODE_UV_OFF");
                    }
                    else
                    {
                        mat.EnableKeyword("USE_MODE_UV_OFF");
                        mat.DisableKeyword("USE_MODE_UV_ON");
                    }
                }
            }
            OldUseModelUV = UseModelUV;
        }

        #endregion
    }
}

