//using UnityEngine;
//using System.Collections;

//namespace Effect
//{
//    public class ShieldBase : MonoBehaviour
//    {
//        public Effect_FadeInOutMaterialColor FadeInOutMaterialColor;
//        public Effect_FadeInOutMaterialFloat FadeInOutMaterialFloat;
//        public Effect_UVTextureAnimator UVTextureAnimator;
//        public Effect_UVScroll UVScroll = null;
//        public Effect_MaterialGradientColor GradientColor = null;
//        protected Collider m_Collider;

//        public virtual void Start()
//        {
//            m_Collider = GetComponent<Collider>();
//            if (!m_Collider)
//            {
//                Debug.LogError(name + "上找不到Collider组件,请注意");
//                enabled = false;
//                return;
//            }
//            //m_Collider.isTrigger = true;
//            transform.gameObject.layer = LayerMask.NameToLayer(Config.LayerLightingEffectOnly);
//        }

//        public virtual void OnDestroy()
//        {
//            FadeInOutMaterialColor = null;
//            FadeInOutMaterialFloat = null;
//            UVTextureAnimator = null;
//            UVScroll = null;
//            m_Collider = null;
//        }

//        /// <summary>
//        /// 被击中
//        /// </summary>
//        /// <param name="hitPoint">位置</param>
//        /// <param name="bPrecisionPoint">是否是精确的点，如果为false，会调用Collider的点重新计算</param>
//        public virtual void OnHit(Vector3 hitPoint,bool bPrecisionPoint = true)
//        {
//            Debug.LogError("必须实现这个函数!");
//        }

//        public virtual void Play()
//        {
//            Debug.LogError("必须实现这个函数!");
//        }

//        public virtual void Stop()
//        {
//            Debug.LogError("必须实现这个函数!");
//        }

//    }

//}
