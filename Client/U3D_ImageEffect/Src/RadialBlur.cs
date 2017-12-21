using UnityEngine;
using UnityEngine.Rendering;
using System;
using System.Collections;
using System.Collections.Generic;
namespace ImageEffects
{


    /// <summary>
    /// 径向模糊
    /// </summary>
    //[ExecuteInEditMode]
    [RequireComponent(typeof(Camera))]
    public class RadialBlur : ImageEffectNode
    {
        public Shader shader;
        public Material mat;

        [Range(0.0f, 1.0f)]
        public float fSampleDist=0.3f;
        [Range(0.0f, 10.0f)]
        public float fSampleStrength=3.0f;

        private int validShaderLOD = 200;

        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }

        public override void OnActive()
        {
            Init();
        }

        public override bool CheckResources()
        {
            return true;
        }



        public void Init()
        {
            if (shader == null)
            {
                shader = ImageEffectManager.API_GetShader("ImageEffect/RadialBlur");
            }

            if (shader)
            {
                if (shader.isSupported)
                {
                    mat = new Material(shader);
                    //mat.hideFlags = HideFlags.HideAndDontSave;
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

        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        {
			if(!IsOnValidLOD())
			{
				return false;
			}

            mat.SetFloat("fSampleDist", fSampleDist);
            mat.SetFloat("fSampleStrength", fSampleStrength);
            source.wrapMode = TextureWrapMode.Clamp;
            Graphics.Blit(source, destination, mat, 0);
            return true;

        }


    
    }

}