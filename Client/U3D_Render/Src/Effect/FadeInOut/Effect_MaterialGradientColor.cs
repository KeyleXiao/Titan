using UnityEngine;
using System.Collections;
using System.Collections.Generic;
namespace Effect
{
    public partial class Effect_MaterialGradientColor : MonoBehaviour,ISaveSerializedData
    {
        public string ColorName = "_TintColor";
        public Gradient gradColor = new Gradient();
        public EffectRunMode RunMode = EffectRunMode.Self;
        public bool Loop = false;
        public float DelayTime = 0.0f;
        public float DuartionTime = 0.03f;

        private bool bCanElvate = false;
        private float currDuatrtion = 0.0f;

        /// <summary>
        /// Self模式,使用共享材质球，复制就不会丢失材质球，但是两个相同的会影响到，这个选项只是调试的时候开
        /// </summary>
        public bool UseSharedMaterial = false;
        private List<Material> matInstances = new List<Material>();

        private Renderer[] m_Renders;
        private Projector[] m_Projectors;

        private bool bPlay = false;
        private Dictionary<Material, Color> OrginalColorMap = new Dictionary<Material, Color>();
        Coroutine waitingForStart_cor = null;

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
            if (null == data)
            {
                return;
            }
            if (RunMode == EffectRunMode.Self)
            {
                List<Material> matlist = data as List<Material>;
                if (null == data)
                {
                    Debug.LogError("Effect_MaterialGradientColor-- ConverSerializedData faild");
                    return;
                }
                m_Projectors = GetComponents<Projector>();

                if (matlist.Count != m_Projectors.Length)
                {
                    Debug.LogError("Effect_MaterialGradientColor-- orginalMaterialMap not match");
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
        public void CopyInspectorParamsTo(Effect_MaterialGradientColor FromInstance)
        {
            DuartionTime = FromInstance.DuartionTime;
            gradColor = FromInstance.gradColor;
            RunMode = FromInstance.RunMode;
            ColorName = FromInstance.ColorName;
            Loop = FromInstance.Loop;
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
            if (null != waitingForStart_cor)
            {
                StopCoroutine(waitingForStart_cor);
            }
            currDuatrtion = 0.0f;
            bCanElvate = false;
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
            OrginalColorMap.Clear();
            foreach (Material mat in MatInstances)
            {
                if (mat)
                {
                    OrginalColorMap.Add(mat, mat.GetColor(ColorName));
                }
              
            }
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
            OrginalColorMap.Clear();

            if (MatInstances == null || MatInstances.Count <= 0)
            {
                return;
            }

            foreach (Material mat in MatInstances)
            {
                if (mat)
                {
                    OrginalColorMap.Add(mat, mat.GetColor(ColorName));
                }

            }
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
            OrginalColorMap.Clear();
            if (MatInstances == null || MatInstances.Length <= 0)
            {
                return;
            }

            matInstances.Clear();
            OrginalColorMap.Clear();
            foreach (Material mat in MatInstances)
            {
                if (mat)
                {
                    OrginalColorMap.Add(mat, mat.GetColor(ColorName));
                }

            }
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
            OrginalColorMap.Clear();

            if (!MatInstance)
            {
                return;
            }
            OrginalColorMap.Add(MatInstance, MatInstance.GetColor(ColorName));
            matInstances.Add(MatInstance);
            InitDefaultVariables();
        }

        public void ClearMaterialInstance()
        {
            matInstances.Clear();
            OrginalColorMap.Clear();
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
            if (DelayTime > 0.03f)
            {
                bCanElvate = false;
                waitingForStart_cor = GameUtil.StartCoroutineHelper(this,WaitForStart_Cureve());
            }
            else
            {
                if (DuartionTime < 0)
                {
                    DuartionTime = 0.03f;
                }
                bCanElvate = true;
            }
            currDuatrtion = 0.0f;
            InitDefaultVariables();
        }


        private void InitMaterial()
        {
            if (matInstances.Count > 0) return;
            OrginalColorMap.Clear();
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
                            OrginalColorMap.Add(r.sharedMaterial, r.sharedMaterial.GetColor(ColorName));
                            EffectUtily.DispatchRender(r, r.sharedMaterial, i);
                        }
                        else
                        {
                            matInstances.Add(r.material);
                            OrginalColorMap.Add(r.material, r.material.GetColor(ColorName));
                            EffectUtily.DispatchRender(r, r.material, i);
                        }
                    }



                }
            }
            else
            {

                if (m_Projectors.Length > 0)
                {
                    //Debug.LogWarning("Projector暂不支持分发渲染器,以后会实现");
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
                        OrginalColorMap.Add(r.material, r.material.GetColor(ColorName));
                    }

                }

            }
            if (matInstances.Count <= 0) return;

            InitDefaultVariables();
        }

        private void InitDefaultVariables()
        {
            if (null == matInstances || matInstances.Count <= 0) return;
            foreach (Material mat in matInstances)
            {
                if (!mat)
                {
                    continue;
                }
                mat.SetColor(ColorName, OrginalColorMap[mat]);
                EffectUtily.UpdateColor(mat, ColorName, OrginalColorMap[mat]);
            }

        }



        public void Update()
        {
            if (!bPlay)
            {
                return;
            }

            if (matInstances.Count <= 0)
            {
                bPlay = false;
                return;
            }

            if (!bCanElvate)
            {
                return;
            }
            if (currDuatrtion < DuartionTime)
            {
                currDuatrtion += Time.deltaTime;
                float time = currDuatrtion / DuartionTime;
                Color CurrentColor = gradColor.Evaluate(time);
                foreach (Material mat in matInstances)
                {
                    if (!mat)
                    {
                        continue;
                    }
                    mat.SetColor(ColorName, CurrentColor);
                    EffectUtily.UpdateColor(mat, ColorName, CurrentColor);
                }
            }
            else
            {
               
                if (Loop)
                {
                    ResetValue();
                }
                else
                {
                    Stop();
                }

            }
        }

        IEnumerator WaitForStart_Cureve()
        {
            yield return new WaitForSeconds(DelayTime);
            bCanElvate = true;
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
                        r.sharedMaterial.SetColor(ColorName, OrginalColorMap[r.sharedMaterial]);
                    }
                }
            }

            OrginalColorMap.Clear();
            m_Renders = null;
            m_Projectors = null;
            matInstances.Clear();
        }

    }
}

