using UnityEngine;
using UnityEngine.Rendering;
using System;
using System.Collections;
using System.Collections.Generic;
namespace ImageEffects
{


    /// <summary>
    /// 模糊和降低画面亮度
    /// </summary>
    [RequireComponent(typeof(Camera))]
    public class BlurAndDownScreen : ImageEffectNode
    {
        private Shader shader;
        private Material mat;

        [Range(0.0f, 1.0f)]
        public float DrakNess = 0.8f;

        public int BlurSize = 4;

        private int validShaderLOD = 400;

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
                shader = ImageEffectManager.API_GetShader("ImageEffect/BlurAndDownScreen");
            }

            if (shader)
            {
                if (shader.isSupported)
                {
                    if(mat)
                    {
                        Destroy(mat);
                        mat = null;
                    }
                    mat = new Material(shader);
                   // mat.hideFlags = HideFlags.HideAndDontSave;
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
            if (!ImageEffects.ImageEffectManager.AdvanceImangeEffectEnabled)
            {
                return false;
            }
			if( !IsOnValidLOD())
			{
				return false;
			}
            RenderTexture temp = RenderTexture.GetTemporary(source.width, source.height);
            //用Bilinear过度更平滑，更好看
            temp.filterMode = FilterMode.Bilinear;
            Graphics.Blit(source, temp);

            mat.SetFloat("_DrakNess", DrakNess);

            int rtWidth = source.width ;
            int rtHeight = source.height;

            float spread = 1.0f;
            int iterations = 2;

            for (int i = 0; i < 6; i++)
            {
                if (i > 1)
                    spread = 1.0f;
                else
                    spread = 0.5f;

                if (i == 2)
                    spread = 0.75f;


                for (int j = 0; j < iterations; j++)
                {
                    mat.SetFloat("_BlurSize", (BlurSize * 0.5f + j) * spread);
                    //垂直
                    RenderTexture rt2 = RenderTexture.GetTemporary(rtWidth, rtHeight);
                    rt2.filterMode = FilterMode.Bilinear;
                    Graphics.Blit(temp, rt2, mat, 1);

                    Graphics.Blit(rt2, temp, mat, 2);
                    RenderTexture.ReleaseTemporary(rt2);
                }

            }
            Graphics.Blit(temp, destination,mat,0);
            RenderTexture.ReleaseTemporary(temp);
            return true;
        }

   }    
}