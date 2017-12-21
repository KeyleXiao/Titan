//using UnityEngine;
//using System.Collections;

//namespace GlowEffect
//{
//    /// <summary>
//    /// 将绘制的东西呈现到主相机上，RenderImage
//    /// </summary>
//    public class GlowCameraRerenderOnlyHelper : BaseGlowCamera
//    {
//        public GlowCameraRerenderOnly glowCam = null;

//        public override void ImageEffect_PostRender()
//        {
//            if (glowCam != null)
//            {
//                RenderTexture temp;
//                int res = (int)glowCam.glowManager._rerenderResolution;
//                temp = RenderTexture.GetTemporary(((int)glowCam.glowManager.MainCam.pixelWidth) / res, ((int)glowCam.glowManager.MainCam.pixelHeight) / res, 0x10);
//                glowCam.glowManager.glowCam.targetTexture = temp;
//                glowCam.glowManager.glowCam.Render();
//                RenderTexture.active = null;
//                glowCam.blur.BlurAndBlitBuffer(temp, null, glowCam.settings, glowCam.glowManager._highPrecsionActive);
//                RenderTexture.ReleaseTemporary(temp);
//            }
//        }

//    }
//}

