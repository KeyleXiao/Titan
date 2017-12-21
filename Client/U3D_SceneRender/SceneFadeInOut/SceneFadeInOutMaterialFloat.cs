using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.Events;
namespace Effect
{



    public partial class SceneFadeInOutMaterialFloat : MonoBehaviour
    {

        public bool isPlaying
        {
            get
            {
                return bPlay;
            }
        }

        public string PropertyName = "_CutOut";
        //private bool IncludeChildren = false;
        public bool Loop = false;

        /// <summary>
        /// Self模式,使用共享材质球，复制就不会丢失材质球，但是两个相同的会影响到，这个选项只是调试的时候开
        /// </summary>
        public bool UseSharedMaterial = false;


        private SceneFloatFadeFinishEvent m_FinishCallBack = new SceneFloatFadeFinishEvent();
        private List<Material> matInstances = new List<Material>();
        private Dictionary<Material, float> OrginalFloatMap = new Dictionary<Material, float>();

        private Renderer []m_Renders;
        private Projector []m_Projectors;

        private bool bPlay = false;

        public void RegisterFinishEvent(UnityAction<SceneFadeInOutMaterialFloat> ev)
        {
            if(null != m_FinishCallBack)
            {
                if(null != ev)
                {
                    m_FinishCallBack.AddListener(ev);
                }
            }
        }

        public void UnRegisterFinishEvent(UnityAction<SceneFadeInOutMaterialFloat> ev)
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





        public void Start()
        {
            FadeTimer = 0.0f;
        }


        /// <summary>
        /// 拷贝编辑器变量
        /// </summary>
        /// <param name="FromInstance">从哪一个拷贝</param>
        public void CopyInspectorParamsTo(SceneFadeInOutMaterialFloat FromInstance)
        {
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
            Reset_Constant();
            InitDefaultVariables();
        }




        private void InitMaterial()
        {

            InitDefaultVariables();
        }

        private void InitDefaultVariables()
        {
            if (matInstances.Count <= 0) return;

            InitDefaultVariables_Constant();
        }

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
           Update_Constant();
        }
    }
}

