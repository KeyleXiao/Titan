using System;
using UnityEngine;

namespace ImageEffects
{

    //internal class GlowCameraRerender : BaseGlowCamera
    //{
    //    public void OnPreCull()
    //    {
    //        Camera HostCamera = GetComponent<Camera>();
    //        HostCamera.CopyFrom(base.parentCamera);
    //        HostCamera.backgroundColor = Color.black;
    //        HostCamera.clearFlags = CameraClearFlags.SolidColor;
    //        HostCamera.renderingPath = RenderingPath.VertexLit;
    //        //base.camera.depth = base.parentCamera.depth + 0.1f;
    //        HostCamera.cullingMask = (1 << LayerMask.NameToLayer("LightingEffectOnly"));
    //        HostCamera.SetReplacementShader(base.glowOnly, "RenderEffect");
            
    //    }

    //    public void OnPreRender()
    //    {
    //        Camera HostCamera = GetComponent<Camera>();
    //        HostCamera.projectionMatrix = base.parentCamera.projectionMatrix;
    //    }

    //    public void OnRenderImage(RenderTexture source, RenderTexture destination)
    //    {
    //        base.blur.BlurAndBlitBuffer(source, destination, base.settings, base.highPrecision);
    //    }
    //}
}
//using System;
//using UnityEngine;

//namespace GlowEffect
//{

//    public class GlowCameraRerender : BaseGlowCamera
//    {
//        private GlowCameraRerenderHelper helper;

//        [HideInInspector]
//        public RenderTexture screenRt;
//        private RenderTexture tmpRt;

//        void Awake()
//        {
//            isFullScreenEffect = false;
//        }

//        public void ActivateHelper()
//        {
//            if ((parentCamera != null) && (helper == null))
//            {
//                helper = ImageEffectManager.AddImageEffect<GlowCameraRerenderHelper>(parentCamera, true);
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

//        public override void ImageEffect_PostRender()
//        {
//            blur.BlurAndBlitBuffer(tmpRt, screenRt, settings, highPrecision);
//            RenderTexture.ReleaseTemporary(tmpRt);
//        }

//        public override void ImageEffect_PreCull()
//        {
//            ActivateHelper();
//            if (screenRt == null)
//            {
//                return;
//            }

//            glowManager.glowCam.CopyFrom(parentCamera);
//            glowManager.glowCam.backgroundColor = Color.black;
//            glowManager.glowCam.clearFlags = CameraClearFlags.Nothing;
//            glowManager.glowCam.SetReplacementShader(glowOnly, "RenderEffect");
//            glowManager.glowCam.renderingPath = RenderingPath.VertexLit;

//            tmpRt = RenderTexture.GetTemporary(screenRt.width, screenRt.height);
//            RenderTexture.active = tmpRt;
//            GL.Clear(false, true, Color.black);
//            glowManager.glowCam.targetTexture = tmpRt;
//        }

//        public override void ImageEffect_PreRender()
//        {
//            glowManager.glowCam.projectionMatrix = parentCamera.projectionMatrix;
//        }
//    }
//}


