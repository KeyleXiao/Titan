using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.Events;
namespace Effect
{
    public partial class Effect_ModifSceneBrightness : MonoBehaviour
    {
        public ValueMode valueMode = ValueMode.Constant;
        public bool Loop = false;

        private SceneBrightnesFadeFinishEvent m_FinishCallBack = new SceneBrightnesFadeFinishEvent();
        private bool bPlay = false;

        private static Effect_ModifSceneBrightness activeInstance = null;

        public void RegisterFinishEvent(UnityAction<Effect_ModifSceneBrightness> ev)
        {
            if (null != m_FinishCallBack)
            {
                if (null != ev)
                {
                    m_FinishCallBack.AddListener(ev);
                }
            }
        }

        public void UnRegisterFinishEvent(UnityAction<Effect_ModifSceneBrightness> ev)
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

        public void Stop()
        {
            if (activeInstance == this)
            {
                activeInstance = null;
                ImageEffects.ImageEffectManager.API_ResetSceneBrightness();
            }
            Stop_Constant();
            Stop_Curve();
            bPlay = false;
        }

        public void Play()
        {
            if(activeInstance)
            {
                return;
            }

            Stop();
            activeInstance = this;
            bPlay = true;
            ResetValue();
        }

        void ResetValue()
        {
            if (activeInstance != this)
            {
                return;
            }
            if (valueMode == ValueMode.Constant)
            {
                Reset_Constant();
            }
            else if (valueMode == ValueMode.Curve)
            {
                Reset_Curve();
            }
            InitDefaultVariables();
        }

        private void InitDefaultVariables()
        {
            if (activeInstance != this)
            {
                return;
            }
            if (valueMode == ValueMode.Constant)
            {
                InitDefaultVariables_Constant();
            }
            else if (valueMode == ValueMode.Curve)
            {
                InitDefaultVariables_Curve();
            }
        }

        public void Update()
        {
            if (!bPlay)
            {
                return;
            }

            if (activeInstance != this)
            {
                return;
            }
            if (valueMode == ValueMode.Constant)
            {
                Update_Constant();
            }
            else if (valueMode == ValueMode.Curve)
            {
                Update_Curve();
            }
        }

        public void OnDestroy()
        {
            if (activeInstance == this)
            {
                activeInstance = null;
            }
        }

    }
}

