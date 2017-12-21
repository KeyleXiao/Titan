using System;
using UnityEngine;
namespace ImageEffects
{
    public class GlowOutLineCamera : MonoBehaviour
    {
        internal IBlur _blur;
        private Shader _GlowOutLineOnly;
        public GlowOutLineManager glowOutLineManager;
        public bool highPrecision;
        public Camera parentCamera;

        //其实是Glow的另一个，所以设置用GlowSettings
        public GlowSettings settings;
        internal virtual void Init()
        {
        }

        internal IBlur blur
        {
            get
            {
                return _blur;
            }
            set
            {
                _blur = value;
            }
        }

        public Shader glowOutLineOnly
        {
            get
            {
                if (_GlowOutLineOnly == null)
                {
                    _GlowOutLineOnly = ImageEffectManager.API_GetShader("Effect/Glow/GlowOutLineObjects");
                }
                return _GlowOutLineOnly;
            }
        }
    }
}