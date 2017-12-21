using System;
using System.Collections.Generic;
using UnityEngine;

namespace ImageEffects
{
    //[ExecuteInEditMode]
    [RequireComponent(typeof(Camera))]
    public class FullScreenSaturation : ImageEffectNode
    {


        private Material ccMaterial;


        public Shader simpleColorCorrectionCurvesShader = null;

        public float saturation = 0.5f;

        private int validShaderLOD = 100;

        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }

        public override void OnActive()
        {

            Init();

        }


        public void Init()
        {
            if (simpleColorCorrectionCurvesShader == null)
            {
                simpleColorCorrectionCurvesShader = ImageEffectManager.API_GetShader("ImageEffect/FullScreenSaturation");
            }

            if (simpleColorCorrectionCurvesShader)
            {
                if (simpleColorCorrectionCurvesShader.isSupported)
                {
                    ccMaterial = CheckShaderAndCreateMaterial(simpleColorCorrectionCurvesShader, ccMaterial);
                }
                else
                {
                    enabled = false;
                }
            }
            else
            {
                enabled = false;
            }
        }

        public override bool CheckResources()
        {
            CheckSupport(false);

            return true;
        }

   



        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        {
            if (CheckResources() == false || !IsOnValidLOD())
            {
                return false;
            }

            ccMaterial.SetFloat("_Saturation", saturation);

            Graphics.Blit(source, destination, ccMaterial);
            return true;

        }
    }

}
