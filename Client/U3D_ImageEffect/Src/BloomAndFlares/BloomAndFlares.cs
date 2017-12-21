using System;
using UnityEngine;

namespace ImageEffects
{
    public class BloomAndFlares : ImageEffectNode
    {
        private int validShaderLOD = 400;

        //弄一个，有框框，可以勾掉
        public void Start()
        {

        }
        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }
        public enum BlurType
        {
            Standard = 0,
            Sgx = 1,
        }
        private BlurType blurType = BlurType.Sgx;

        [Range(0.0f, 1.5f)]
        public float threshold = 0.7f;
        [Range(0.0f, 2.5f)]
        public float intensity = 0.75f;

        [Range(0.25f, 5.5f)]
        public float blurSize = 1.0f;

        [Range(1, 4)]
        public int blurIterations = 1;
        private Material BloomMaterial = null;

        public override bool CheckResources ()
		{
            CheckSupport (false);

            Shader BloomShader = ImageEffectManager.API_GetShader("ImageEffect/BloomAndFlare/OptmizedBloom");
            BloomMaterial = CheckShaderAndCreateMaterial(BloomShader, BloomMaterial);

            if (!isSupported)
                ReportAutoDisable ();
            return isSupported;
        }

        public override void OnActive()
        {
            base.OnActive();
        }

        public override void OnDeActive()
        {
            if (BloomMaterial)
                DestroyImmediate(BloomMaterial);
        }

        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        {
            if (!ImageEffects.ImageEffectManager.AdvanceImangeEffectEnabled)
            {
                return false;
            }
            if (!CheckResources() || !IsOnValidLOD())
            {
                return false;
            }


            int divider = 2;
            float widthMod = 1.0f;

            BloomMaterial.SetVector ("_Parameter", new Vector4 (blurSize * widthMod, 0.0f, threshold, intensity));
            source.filterMode = FilterMode.Bilinear;

            var rtW= source.width/divider;
            var rtH= source.height/divider;

            // downsample
            RenderTexture rt = RenderTexture.GetTemporary (rtW, rtH, 0, source.format);
            rt.filterMode = FilterMode.Bilinear;
            Graphics.Blit (source, rt, BloomMaterial, 1);

            var passOffs = blurType == BlurType.Standard ? 0 : 2;
            for(int i = 0; i < blurIterations; i++)
			{
                BloomMaterial.SetVector ("_Parameter", new Vector4 (blurSize * widthMod + (i*1.0f), 0.0f, threshold, intensity));

                // vertical blur
                RenderTexture rt2 = RenderTexture.GetTemporary (rtW, rtH, 0, source.format);
                rt2.filterMode = FilterMode.Bilinear;
                Graphics.Blit(rt, rt2, BloomMaterial, 2 + passOffs);
                RenderTexture.ReleaseTemporary (rt);
                rt = rt2;

                // horizontal blur
                rt2 = RenderTexture.GetTemporary (rtW, rtH, 0, source.format);
                rt2.filterMode = FilterMode.Bilinear;
                Graphics.Blit(rt, rt2, BloomMaterial, 3 + passOffs);
                RenderTexture.ReleaseTemporary (rt);
                rt = rt2;
            }

            BloomMaterial.SetTexture ("_Bloom", rt);

            Graphics.Blit (source, destination, BloomMaterial, 0);
            RenderTexture.ReleaseTemporary (rt);

            return true;
        }

    }
}
