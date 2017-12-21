using UnityEngine;
using System.Collections;
using System.Collections.Generic;
namespace ImageEffects
{
    /// <summary>
    /// 修改屏幕亮度
    /// </summary>
    [RequireComponent(typeof(Camera))]
    public class ModifyGameBrightness : ImageEffectNode
    {
        public static string ModifyGameBrightnessKey = "_brightess";
        private Material mat;
        private float currBrightessValue = 1.0f;
        private int validShaderLOD = 200;

        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }

        public override void OnActive()
        {

            Init();

        }

        public override  void SetParam(string paramName, float Val)
        {
            currBrightessValue = Val;
        }
        public override bool CheckResources()
        {
            return true;
        }
        public void Init()
        {
           
            mat = new Material(ImageEffectManager.API_GetShader("ImageEffect/Common/ModifyGameBrightness"));
            //mat.hideFlags = HideFlags.HideAndDontSave;
        }

        void ApplyParams()
        {
            mat.SetFloat(ModifyGameBrightnessKey,currBrightessValue);
        }

        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        {
            if (mat)
            {
                ApplyParams();
                Graphics.Blit(source, destination, mat, 0);
                return true;
            }
            return false;
        }
    }
}