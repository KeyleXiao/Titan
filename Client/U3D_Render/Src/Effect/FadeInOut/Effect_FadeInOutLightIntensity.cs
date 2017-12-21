using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.Events;
namespace Effect
{
    public partial class Effect_FadeInOutLightIntensity : MonoBehaviour
    {
        public Light HostLight = null;
        public ValueMode valueMode = ValueMode.Constant;
        public bool Loop = false;

        private LightFadeFinishEvent m_FinishCallBack = new LightFadeFinishEvent();

        private bool bPlay = false;
        private float orgianlValue = 0;

        public void RegisterFinishEvent(UnityAction<Effect_FadeInOutLightIntensity> ev)
        {
            if (null != m_FinishCallBack)
            {
                if (null != ev)
                {
                    m_FinishCallBack.AddListener(ev);
                }
            }
        }

        public void UnRegisterFinishEvent(UnityAction<Effect_FadeInOutLightIntensity> ev)
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
            if(valueMode == ValueMode.Constant)
            {
                Reset_Constant();
            }
            else if(valueMode == ValueMode.Curve)
            {

            }
            InitDefaultVariables();
        }





        private void InitDefaultVariables()
        {
            if (!HostLight) return;

            if (valueMode == ValueMode.Constant)
            {
                InitDefaultVarables_Constant();
            }
            else if (valueMode == ValueMode.Curve)
            {

            }

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

            if (valueMode == ValueMode.Constant)
            {
                Update_Constant();
            }
            else if (valueMode == ValueMode.Curve)
            {

            }

        }

    }
}

