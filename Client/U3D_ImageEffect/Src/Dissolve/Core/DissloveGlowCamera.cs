using System;
using UnityEngine;
namespace ImageEffects
{
    public class DissloveGlowCamera : MonoBehaviour
    {
        internal IBlur _blur;
        private Shader _DissloveGlowOnly;
        public DissolveGlowManager dissolveGlowManager;
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

        public Shader DissloveglowOnly
        {
            get
            {
                if (_DissloveGlowOnly == null)
                {
                    _DissloveGlowOnly = ImageEffectManager.API_GetShader("Effect/Glow/GlowDissloveObjects");
                }
                return _DissloveGlowOnly;
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
