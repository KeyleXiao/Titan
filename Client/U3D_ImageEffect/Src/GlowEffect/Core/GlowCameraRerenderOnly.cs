using System;
using UnityEngine;
namespace ImageEffects
{
    public class GlowCameraRerenderOnly : BaseGlowCamera
    {
        public void OnPreCull()
        {
            Camera HostCamera = GetComponent<Camera>();
            RenderTexture targetTexture = HostCamera.targetTexture;
            HostCamera.CopyFrom(parentCamera);
            HostCamera.backgroundColor = Color.black;
            HostCamera.clearFlags = CameraClearFlags.Color;
            HostCamera.SetReplacementShader(base.glowOnly, "RenderEffect");
            HostCamera.renderingPath = RenderingPath.VertexLit;
            HostCamera.targetTexture = targetTexture;
            HostCamera.cullingMask = (1 << LayerMask.NameToLayer("LightingEffectOnly"));
        }

        public void OnPreRender()
        {
            Camera HostCamera = GetComponent<Camera>();
            HostCamera.projectionMatrix = base.parentCamera.projectionMatrix;
        }
    }
}

//using System;
//using UnityEngine;
//namespace GlowEffect
//{
//    public class GlowCameraRerenderOnly : BaseGlowCamera
//    {
//        private GlowCameraRerenderOnlyHelper helper;
//        public  void ActivateHelper()
//        {

//            if ((parentCamera != null) && (helper == null))
//            {
//                helper = ImageEffectManager.AddImageEffect<GlowCameraRerenderOnlyHelper>(parentCamera,false);
//                if (null != helper)
//                {
//                    helper.isFullScreenEffect = false;
//                    helper.hideFlags = HideFlags.HideInInspector;
//                    helper.glowCam = this;
//                }
//            }
//        }

//        internal override void Init()
//        {
//            ActivateHelper();
//        }

//        public void OnDeActive()
//        {
//            if (helper)
//            {
//                helper.glowCam = null;
//                ImageEffectManager.DeleteImageEffect(parentCamera, helper);
//            }
//        }

//        public void OnActive()
//        {
//            ActivateHelper();
//        }

//        public override void ImageEffect_PreCull()
//        {
//            RenderTexture targetTexture = glowManager.glowCam.targetTexture;
//            glowManager.glowCam.CopyFrom(parentCamera);
//            glowManager.glowCam.backgroundColor = Color.black;
//            glowManager.glowCam.clearFlags = CameraClearFlags.Color;
//            glowManager.glowCam.SetReplacementShader(glowOnly, "RenderType");
//            glowManager.glowCam.renderingPath = RenderingPath.VertexLit;
//            glowManager.glowCam.targetTexture = targetTexture;
//        }

//        public override void ImageEffect_PreRender()
//        {

//            glowManager.glowCam.projectionMatrix = parentCamera.projectionMatrix;
//        }
//    }
//}


