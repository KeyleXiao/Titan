using System;
using UnityEngine;
namespace ImageEffects
{
    public class BaseGlowCamera : MonoBehaviour
    {
        internal IBlur _blur;
        private Shader _glowOnly;
        public GlowManager glowManager;
        public bool highPrecision;
        public Camera parentCamera;
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

        public Shader glowOnly
        {
            get
            {
                if (_glowOnly == null)
                {
                    _glowOnly = ImageEffectManager.API_GetShader("Effect/Glow/GlowObjects");
                }
                return _glowOnly;
            }
        }
    }
}

//using GlowEffect.Blur;
//using System;
//using UnityEngine;
//namespace GlowEffect
//{
//    public class BaseGlowCamera : ImageEffectNode
//    {
//        internal IBlur _blur;
//        private Shader _glowOnly;
//        public GlowEffect.GlowManager glowManager;
//        public bool highPrecision;
//        public Camera parentCamera;
//        public GlowSettings settings;

//        internal virtual void Init()
//        {
//        }

//        internal IBlur blur
//        {
//            get
//            {
//                return _blur;
//            }
//            set
//            {
//                _blur = value;
//            }
//        }

//        protected Shader glowOnly
//        {
//            get
//            {
//                if (_glowOnly == null)
//                {
//                    _glowOnly = ImageEffectManager.API_GetShader("Effect/Glow/GlowObjects");
//                }
//                return _glowOnly;
//            }
//        }
//    }
//}
