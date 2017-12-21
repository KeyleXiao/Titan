using System;
using UnityEngine;

namespace ImageEffects
{

    internal class DefaultBlur : BlurBase, IBlur
    {
        protected Material composeMaxMaterial = null;
        private bool maxFallback = false;

        public DefaultBlur()
        {
            Shader shader = ImageEffectManager.API_GetShader("ImageEffect/Glow/Compose Max");
            if (!shader.isSupported)
            {
                shader = ImageEffectManager.API_GetShader("ImageEffect/Glow/Compose Max Fallback");
                maxFallback = true;
            }
            composeMaxMaterial = new Material(shader);
            //composeMaxMaterial.hideFlags = HideFlags.DontSave;
            InitHighQualityGlow();
        }

        public void BlurAndBlitBuffer_Legacy(RenderTexture sourceBuffer, RenderTexture needToBuffer, RenderTexture destination, RenderTexture extratRenderTexture, GlowSettings settings, bool highPrecision)
        {
            if (sourceBuffer == null)
            {
                return;
            }
            int baseResolution = 4;// (int)settings.baseResolution;
            int downsampleResolution = 4;//(int)settings.downsampleResolution;
            int downsampleSteps = 1;//settings.downsampleSteps;
            RenderTexture[] textureArray = new RenderTexture[downsampleSteps * 2];
            RenderTexture source = null;
            RenderTextureFormat format = !highPrecision ? RenderTextureFormat.Default : RenderTextureFormat.ARGBHalf;
            base.downsampleMaterial.SetFloat("_Strength", settings.innerStrength / ((baseResolution != 4) ? ((float)1) : ((float)4)));
            RenderTexture dest = RenderTexture.GetTemporary(needToBuffer.width / baseResolution, needToBuffer.height / baseResolution, 0, format);
            RenderTexture texture3 = RenderTexture.GetTemporary(dest.width, dest.height, 0, format);
            Graphics.Blit(needToBuffer, dest, base.downsampleMaterial, (baseResolution != 4) ? 1 : 0);
            base.downsampleMaterial.SetFloat("_Strength", settings.innerStrength / ((downsampleResolution != 4) ? ((float)1) : ((float)4)));
            source = dest;
            for (int i = 0; i < downsampleSteps; i++)
            {
                int width = source.width / downsampleResolution;
                int height = source.height / downsampleResolution;
                if ((width == 0) || (height == 0))
                {
                    break;
                }
                textureArray[i * 2] = RenderTexture.GetTemporary(width, height, 0, format);
                textureArray[(i * 2) + 1] = RenderTexture.GetTemporary(width, height, 0, format);
                Graphics.Blit(source, textureArray[i * 2], base.downsampleMaterial, (downsampleResolution != 4) ? 1 : 0);
                source = textureArray[i * 2];
            }
            for (int j = downsampleSteps - 1; j >= 0; j--)
            {
                if (textureArray[j * 2] != null)
                {
                    base.BlurBuffer(textureArray[j * 2], textureArray[(j * 2) + 1]);
                    RenderTexture texture = (j <= 0) ? dest : textureArray[(j - 1) * 2];
                    if (settings.downsampleBlendMode == DownsampleBlendMode.Max)
                    {
                        if (maxFallback)
                        {
                            composeMaxMaterial.SetTexture("_DestTex", texture);
                        }
                        composeMaxMaterial.SetFloat("_Strength", settings.outerStrength / ((((float)j) / 2f) + 1f));
                        Graphics.Blit(textureArray[j * 2], texture, composeMaxMaterial);
                    }
                    else
                    {
                        base.composeMaterial.SetFloat("_Strength", settings.outerStrength / ((((float)j) / 2f) + 1f));
                        Graphics.Blit(textureArray[j * 2], texture, base.composeMaterial, (int)settings.downsampleBlendMode);
                    }
                }
            }
            base.BlurBuffer(dest, texture3);
            base.composeMaterial.SetFloat("_Strength", settings.boostStrength);
            if(null != sourceBuffer)
            {
                composeMaterial.SetTexture("_BlurBuffer", dest);
                Graphics.Blit(sourceBuffer, destination, base.composeMaterial, (int)settings.blendMode);
            }
            else
            {
                Graphics.Blit(dest, destination, base.composeMaterial, (int)settings.blendMode);
            }

            RenderTexture.ReleaseTemporary(dest);
            RenderTexture.ReleaseTemporary(texture3);
            for (int k = 0; k < downsampleSteps; k++)
            {
                RenderTexture.ReleaseTemporary(textureArray[k * 2]);
                RenderTexture.ReleaseTemporary(textureArray[(k * 2) + 1]);
            }
        }

        public void BlurAndBlitBuffer(RenderTexture rbuffer, RenderTexture needToBuffer, RenderTexture destination, RenderTexture extratRenderTexture, GlowSettings settings, bool highPrecision)
        {
            if (!GlowManager.isSupportedHighQualityGlow ||
                ImageEffectManager.ImageEffectShaderLOD < 300)
            {
                BlurAndBlitBuffer_Legacy(rbuffer, needToBuffer, destination, extratRenderTexture,settings, highPrecision);
            }
            else
            {
                HighQualityGlow_RenderImage(rbuffer, needToBuffer, destination, extratRenderTexture,settings);
            }
        }

        private Shader HighQualityGlowShader;
        private Material HighQualityGlowMaterial;

        void InitHighQualityGlow()
        {
            if (Application.isPlaying && GlowManager.isSupportedHighQualityGlow)
            {
                HighQualityGlowShader = ImageEffectManager.API_GetShader("ImageEffect/Glow/HighQualityGlow");
                if (!HighQualityGlowShader)
                {
                    Debug.LogError("找不到HighQualityGlowShader - ImageEffect/Glow/HighQualityGlow");
                    return;
                }

                if (!HighQualityGlowMaterial)
                {
                    HighQualityGlowMaterial = new Material(HighQualityGlowShader);
                    //HighQualityGlowMaterial.hideFlags = HideFlags.HideAndDontSave;
                }
            }

        }

        void HighQualityGlow_RenderImage(RenderTexture sourceBuffer, RenderTexture needToBuffer, RenderTexture destination, RenderTexture extratRenderTexture, GlowSettings settings)
        {
            if(sourceBuffer == null)
            {
                return;
            }
            if (!GlowManager.isSupportedHighQualityGlow)
            {
                Graphics.Blit(sourceBuffer, destination);
                return;
            }

            //RenderTexture temp = RenderTexture.GetTemporary(source.width, source.height, 0, RenderTextureFormat.ARGBHalf);
            //temp.filterMode = FilterMode.Bilinear;
            //Graphics.Blit(source, temp);

            HighQualityGlowMaterial.SetFloat("_GlowIntensity", Mathf.Exp(settings.HighQualityGlowIntensity) - 1.0f);
            RenderTextureFormat rtf = ImageEffectManager.ImageEffectShaderLOD < 300 ? RenderTextureFormat.Default : RenderTextureFormat.ARGBHalf;
            int rtWidth = needToBuffer.width / 4;
            int rtHeight = needToBuffer.height / 4;

            RenderTexture downsampled;
            downsampled = needToBuffer;

            float spread = 1.0f;
            int iterations = 2;
           
            for (int i = 0; i < 4; i++)
            {
                RenderTexture rt = RenderTexture.GetTemporary(rtWidth, rtHeight, 0, rtf);
                rt.filterMode = FilterMode.Bilinear;

                Graphics.Blit(downsampled, rt, HighQualityGlowMaterial, 1);

                downsampled = rt;

                if (i > 1)
                    spread = 1.0f;
                else
                    spread = 0.5f;

                if (i == 2)
                    spread = 0.75f;


                for (int j = 0; j < iterations; j++)
                {
                    HighQualityGlowMaterial.SetFloat("_BlurSize", (settings.HighQualityGlowBlurSize * 0.5f + j) * spread);

                    //垂直
                    RenderTexture rt2 = RenderTexture.GetTemporary(rtWidth, rtHeight, 0, rtf);
                    rt2.filterMode = FilterMode.Bilinear;
                    Graphics.Blit(rt, rt2, HighQualityGlowMaterial, 2);
                    RenderTexture.ReleaseTemporary(rt);
                    rt = rt2;

                    //水平
                    rt2 = RenderTexture.GetTemporary(rtWidth, rtHeight, 0, rtf);
                    rt2.filterMode = FilterMode.Bilinear;
                    Graphics.Blit(rt, rt2, HighQualityGlowMaterial, 3);
                    RenderTexture.ReleaseTemporary(rt);
                    rt = rt2;
                }

                switch (i)
                {
                    case 0:
                        HighQualityGlowMaterial.SetTexture("_GlowTex0", rt);
                        break;
                    case 1:
                        HighQualityGlowMaterial.SetTexture("_GlowTex1", rt);
                        break;
                    case 2:
                        HighQualityGlowMaterial.SetTexture("_GlowTex2", rt);
                        break;
                    case 3:
                        HighQualityGlowMaterial.SetTexture("_GlowTex3", rt);
                        break;
                    case 4:
                        HighQualityGlowMaterial.SetTexture("_GlowTex4", rt);
                        break;
                    case 5:
                        HighQualityGlowMaterial.SetTexture("_GlowTex5", rt);
                        break;
                    default:
                        break;
                }

                RenderTexture.ReleaseTemporary(rt);

                rtWidth /= 2;
                rtHeight /= 2;
            }
           // RenderTexture.ReleaseTemporary(temp);
            HighQualityGlowMaterial.SetTexture("_BlurBuffer", needToBuffer);
            Graphics.Blit(sourceBuffer, destination, HighQualityGlowMaterial, 0);
        }
    }
}

