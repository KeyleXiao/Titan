using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.Events;
namespace Effect
{


    
    public partial class Effect_FadeInOutMaterialFloat : MonoBehaviour,ISaveSerializedData
    {

        public bool isPlaying
        {
            get
            {
                return bPlay;
            }
        }


        public EffectRunMode RunMode = EffectRunMode.Self;
        public ValueMode valueMode = ValueMode.Constant;
        public string PropertyName = "_CutOut";
        //private bool IncludeChildren = false;
        public bool Loop = false;

        /// <summary>
        /// Self模式,使用共享材质球，复制就不会丢失材质球，但是两个相同的会影响到，这个选项只是调试的时候开
        /// </summary>
        public bool UseSharedMaterial = false;


        private FloatFadeFinishEvent m_FinishCallBack = new FloatFadeFinishEvent();
        private List<Material> matInstances = new List<Material>();
        private Dictionary<Material, float> OrginalFloatMap = new Dictionary<Material, float>();

        private Renderer []m_Renders;
        private Projector []m_Projectors;

        private bool bPlay = false;

        public void RegisterFinishEvent(UnityAction<Effect_FadeInOutMaterialFloat> ev)
        {
            if(null != m_FinishCallBack)
            {
                if(null != ev)
                {
                    m_FinishCallBack.AddListener(ev);
                }
            }
        }

        public void UnRegisterFinishEvent(UnityAction<Effect_FadeInOutMaterialFloat> ev)
        {
            if (null != m_FinishCallBack)
            {
                if (null != ev)
                {
                    m_FinishCallBack.RemoveListener(ev);
                }
            }
        }

        public void InvokeFinishEvent()
        {
            if (null != m_FinishCallBack)
            {
                m_FinishCallBack.Invoke(this);
            }
        }

        public void ClearFinishEvent()
        {
            if (null != m_FinishCallBack)
            {
                m_FinishCallBack.RemoveAllListeners();
            }
        }

        public System.Object SaveSerializedData()
        {
            if (RunMode == EffectRunMode.Self)
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
            return null;
        }

        public void SetSerializedData(System.Object data)
        {
            if(null == data)
            {
                return;
            }
            if (RunMode == EffectRunMode.Self)
            {
                List<Material> matlist = data as List<Material>;
                if(null == data)
                {
                    Debug.LogError("Effect_FadeInOutMaterialFloat-- ConverSerializedData faild");
                    return;
                }
                m_Projectors = GetComponents<Projector>();

                if(matlist.Count != m_Projectors.Length)
                {
                    Debug.LogError("Effect_FadeInOutMaterialFloat-- orginalMaterialMap not match");
                    return;
                }
                for (int i = 0; i < m_Projectors.Length; i++)
                {
                    m_Projectors[i].material = matlist[i];
                }

                m_Projectors = null;
            }
        }

        public void Start()
        {
            if (RunMode == EffectRunMode.Self)
            {
                GenerateSelfMaterial();
            }

            FadeTimer = 0.0f;
        }

        public Renderer[] GetRendersOnBakeTime()
        {
            if (!Application.isPlaying && Application.isEditor)
            {
                List<Renderer> RenderList = new List<Renderer>();

                if(RunMode == EffectRunMode.Self)
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
        public void CopyInspectorParamsTo(Effect_FadeInOutMaterialFloat FromInstance)
        {
            RunMode = FromInstance.RunMode;
            valueMode = FromInstance.valueMode;
            curve.keys = FromInstance.curve.keys;
            curve_DuartionTime = FromInstance.curve_DuartionTime;
            cure_DelayTime = FromInstance.cure_DelayTime;
            curveScale = FromInstance.curveScale;
            PropertyName = FromInstance.PropertyName;
            Loop = FromInstance.Loop;
            FadeInDealy = FromInstance.FadeInDealy;
            FadeInDuartion = FromInstance.FadeInDuartion;
            FadeInFrom = FromInstance.FadeInFrom;
            FadeInTo = FromInstance.FadeInTo;
            FadeOutDelay = FromInstance.FadeOutDelay;
            FadeOutDuartion = FromInstance.FadeOutDuartion;
            FadeOutFrom = FromInstance.FadeOutFrom;
            FadeOutTo = FromInstance.FadeOutTo;
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
        }

        public void Stop()
        {

            Stop_Constant();
            Stop_Curve();
            bPlay = false;
        }

        

        public void Play()
        {
            Stop();

            bPlay = true;
            ResetValue();
        }

        public void SetMaterialInstance(List<Material> MatInstances)
        {
            if (RunMode == EffectRunMode.Self)
            {
                return;
            }
            if (MatInstances == null || MatInstances.Count <= 0)
            {
                return;
            }
            matInstances.Clear();
            OrginalFloatMap.Clear();
            foreach (Material mat in MatInstances)
            {
                if (mat)
                {
                    if (mat.HasProperty(PropertyName))
                    {
                        OrginalFloatMap.Add(mat, mat.GetFloat(PropertyName));
                        matInstances.Add(mat);
                    }
                    else
                    {
                        Debug.LogWarning(mat.name + "材质没有成员:" + PropertyName);
                    }
                }

            }
            InitDefaultVariables();
        }

        public void SetMaterialInstance(HashSet<Material> MatInstances)
        {
            if (RunMode == EffectRunMode.Self)
            {
                return;
            }
            matInstances.Clear();
            OrginalFloatMap.Clear();
            if (MatInstances == null || MatInstances.Count <= 0)
            {
                return;
            }

            foreach (Material mat in MatInstances)
            {
                if (mat)
                {
                    if (mat.HasProperty(PropertyName))
                    {
                        OrginalFloatMap.Add(mat, mat.GetFloat(PropertyName));
                        matInstances.Add(mat);
                    }
                    else
                    {
                        Debug.LogWarning(mat.name + "材质没有成员:" + PropertyName);
                    }
                }

            }
            InitDefaultVariables();
        }

        public void SetMaterialInstance(Material[] MatInstances)
        {
            if (RunMode == EffectRunMode.Self)
            {
                return;
            }
            matInstances.Clear();
            OrginalFloatMap.Clear();
            if (MatInstances == null || MatInstances.Length <= 0)
            {
                return;
            }
            foreach (Material mat in MatInstances)
            {
                if (mat)
                {
                    if (mat.HasProperty(PropertyName))
                    {
                        OrginalFloatMap.Add(mat, mat.GetFloat(PropertyName));
                        matInstances.Add(mat);
                    }
                    else
                    {
                        Debug.LogWarning(mat.name + "材质没有成员:" + PropertyName);
                    }
                }

            }
            InitDefaultVariables();
        }

        public void SetMaterialInstance(Material MatInstance)
        {
            if (RunMode == EffectRunMode.Self)
            {
                return;

            }
            matInstances.Clear();
            OrginalFloatMap.Clear();
            if (!MatInstance)
            {
                return;
            }

            if (!MatInstance.HasProperty(PropertyName))
            {
                return;
            }
            OrginalFloatMap.Add(MatInstance, MatInstance.GetFloat(PropertyName));
            matInstances.Add(MatInstance);
            InitDefaultVariables();
        }

        public void ClearMaterialInstance()
        {
            matInstances.Clear();
            OrginalFloatMap.Clear();
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

        void ResetValue()
        {
            if(valueMode == ValueMode.Constant)
            {
                Reset_Constant();
            }
            else if(valueMode == ValueMode.Curve)
            {
                Reset_Curve();
            }
            InitDefaultVariables();
        }




        private void InitMaterial()
        {
            if (matInstances.Count > 0) return;

            OrginalFloatMap.Clear();
            if (m_Renders.Length > 0)
            {
                for(int i = 0;i < m_Renders.Length;i++)
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
                            OrginalFloatMap.Add(r.sharedMaterial, r.sharedMaterial.GetFloat(PropertyName));
                            EffectUtily.DispatchRender(r, r.sharedMaterial, i);
                        }
                        else
                        {
                            matInstances.Add(r.material);
                            OrginalFloatMap.Add(r.material, r.material.GetFloat(PropertyName));
                            EffectUtily.DispatchRender(r, r.material, i);

                        }
                    }
                }

            }
            else
            {
                
                if (m_Projectors.Length > 0)
                {
                    // Debug.LogWarning("Projector暂不支持分发渲染器,以后会实现");
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
                        OrginalFloatMap.Add(r.material, r.material.GetFloat(PropertyName));
                    }

                }
                
            }
            if (matInstances.Count <= 0) return;

            InitDefaultVariables();
        }

        private void InitDefaultVariables()
        {
            if (matInstances.Count <= 0) return;

            if(valueMode == ValueMode.Constant)
            {
                InitDefaultVariables_Constant();
            }
            else if (valueMode == ValueMode.Curve)
            {
                InitDefaultVariables_Curve();
            }
        }

        //public void OnEnable()
        //{
        //    InitDefaultVariables();
        //}

        public void Update()
        {
            if (!bPlay)
            {
                return;
            }

            if(matInstances.Count <= 0)
            {
                bPlay = false;
                InvokeFinishEvent();
                return;
            }

            if(valueMode == ValueMode.Constant)
            {
                Update_Constant();
            }
            else if(valueMode == ValueMode.Curve)
            {
                Update_Curve();
            }
        }

        public void OnDestroy()
        {
            if (RunMode != EffectRunMode.Self)
            {
                return;
            }

            if (UseSharedMaterial)
            {
                if (null != m_Renders && m_Renders.Length > 0)
                {
                    for (int i = 0; i < m_Renders.Length; i++)
                    {
                        Renderer r = m_Renders[i];
                        if (!r.sharedMaterial)
                        {
                            continue;
                        }
                        EffectUtily.GabageRender(r, r.sharedMaterial);
                        if (OrginalFloatMap.ContainsKey(r.sharedMaterial))
                        {
                            r.sharedMaterial.SetFloat(PropertyName, OrginalFloatMap[r.sharedMaterial]);
                        }
                    }
                }

            }

            OrginalFloatMap.Clear();
            m_Renders = null;
            m_Projectors = null;
            matInstances.Clear();
        }
    }
}

