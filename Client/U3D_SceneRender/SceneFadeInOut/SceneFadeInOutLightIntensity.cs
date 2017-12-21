using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.Events;
namespace Effect
{
    public partial class SceneFadeInOutLightIntensity : MonoBehaviour
    {
        public Light HostLight = null;
        public bool Loop = false;

        private SceneLightFadeFinishEvent m_FinishCallBack = new SceneLightFadeFinishEvent();

        private bool bPlay = false;
        private float orgianlValue = 0;

        public void RegisterFinishEvent(UnityAction<SceneFadeInOutLightIntensity> ev)
        {
            if (null != m_FinishCallBack)
            {
                if (null != ev)
                {
                    m_FinishCallBack.AddListener(ev);
                }
            }
        }

        public void UnRegisterFinishEvent(UnityAction<SceneFadeInOutLightIntensity> ev)
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
            if(HostLight)
            {
                orgianlValue = HostLight.intensity;
            }
            
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


        void ResetValue()
        {
            Reset_Constant();
            InitDefaultVariables();
        }





        private void InitDefaultVariables()
        {
            if (!HostLight) return;


            InitDefaultVarables_Constant();

        }


        public void Update()
        {
            if (!bPlay)
            {
                return;
            }

            if (!HostLight)
            {
                bPlay = false;
                InvokeFinishEvent();
                return;
            }

           Update_Constant();

        }

    }
}

