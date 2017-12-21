//using UnityEngine;
//using System.Collections;
//namespace GlowEffect
//{
//    public class GlowCameraRerenderHelper : BaseGlowCamera
//    {
//        public GlowCameraRerender glowCam;
//        public override void ImageEffect_RenderImage(RenderTexture src, RenderTexture dst)
//        {
//            if (glowCam.glowManager.CheckSupport())
//            {
//                glowCam.screenRt = src;
//                glowCam.gameObject.camera.Render();
//                Graphics.Blit(src, dst);
//            }
//        }
//    }

//}
