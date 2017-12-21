using System;
using UnityEngine;
namespace ImageEffects
{
    public class GlowCameraReuseHelper : ImageEffectNode
    {
        public GlowCameraReuse glowCam;
        public void OnPreCull()
        {
            if (glowCam == null)
            {
                if (Application.isPlaying)
                {
                    UnityEngine.Object.Destroy(this);
                }
                else
                {
                    UnityEngine.Object.DestroyImmediate(this);
                }
            }
        }

        public override bool CheckResources()
        {
            return true;
        }

        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        {
            if (glowCam == null)
            {
                OnPreCull();
            }
            else if (glowCam.glowManager.CheckSupport())
            {
                glowCam.screenRt = source;
                glowCam.HostCamera.Render();
                Graphics.Blit(source, destination);
            }
            return true;
        }
    }
}

//using System;
//using UnityEngine;
//namespace GlowEffect
//{
//    public class GlowCameraReuseHelper : BaseGlowCamera
//    {
//        public GlowCameraReuse glowCam;

//        public override void ImageEffect_RenderImage(RenderTexture src, RenderTexture dst)
//        {
//            if (glowCam.glowManager.CheckSupport())
//            {
//                glowCam.screenRt = src;
//                glowCam.gameObject.camera.Render();
//                Graphics.Blit(src, dst);
//            }
//        }

//        //public void OnRenderImage(RenderTexture source, RenderTexture destination)
//        //{
//        //    if (glowCam.glowManager.CheckSupport())
//        //    {
//        //        glowCam.screenRt = source;
//        //        glowCam.gameObject.camera.Render();
//        //        Graphics.Blit(source, destination);
//        //    }
//        //}
//    }
//}

