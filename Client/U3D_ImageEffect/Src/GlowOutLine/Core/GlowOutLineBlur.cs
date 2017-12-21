using System;
using UnityEngine;

namespace ImageEffects
{

    internal class GlowOutLineBlur : BlurBase, IBlur
    {
        protected Material composeMaxMaterial = null;
        private bool maxFallback = false;
        protected Material BlendToScreen = null;
        public GlowOutLineBlur()
        {
            Shader shader = ImageEffectManager.API_GetShader("ImageEffect/Glow/Compose Max");
            if (!shader.isSupported)
            {
                shader = ImageEffectManager.API_GetShader("ImageEffect/Glow/Compose Max Fallback");
                maxFallback = true;
            }
            composeMaxMaterial = new Material(shader);
            //composeMaxMaterial.hideFlags = HideFlags.DontSave;

            Shader BlendShader = ImageEffectManager.API_GetShader("ImageEffect/Glow/GlowOutLine/BlendToScreen");
            BlendToScreen = new Material(BlendShader);
            //BlendToScreen.hideFlags = HideFlags.DontSave;
        }

        public void BlurAndBlitBuffer(RenderTexture sourceBuffer, RenderTexture needToBuffer,RenderTexture destination, RenderTexture extratRenderTexture,GlowSettings settings, bool highPrecision)
        {

            if (needToBuffer == null)
            {
                return;
            }
            int baseResolution = 4;// (int)settings.baseResolution;
            int downsampleResolution = 4;// (int)settings.downsampleResolution;
            int downsampleSteps = 1;//settings.downsampleSteps;
            RenderTexture[] textureArray = new RenderTexture[downsampleSteps * 2];
            RenderTexture source = null;
            RenderTextureFormat format = !highPrecision ? RenderTextureFormat.Default : RenderTextureFormat.ARGBHalf;
            base.downsampleMaterial.SetFloat("_Strength", settings.innerStrength / ((baseResolution != 4) ? ((float) 1) : ((float) 4)));
            RenderTexture dest = RenderTexture.GetTemporary(needToBuffer.width / baseResolution, needToBuffer.height / baseResolution, 0, format);
            RenderTexture texture3 = RenderTexture.GetTemporary(dest.width, dest.height, 0, format);

            Graphics.Blit(needToBuffer, dest, base.downsampleMaterial, (baseResolution != 4) ? 1 : 0);
            base.downsampleMaterial.SetFloat("_Strength", settings.innerStrength / ((downsampleResolution != 4) ? ((float) 1) : ((float) 4)));
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

            RenderTexture texture4 = RenderTexture.GetTemporary(dest.width, dest.height, 0, format);
            Graphics.Blit(needToBuffer, texture4);
            Graphics.Blit(dest, texture4, base.composeMaterial, (int)settings.blendMode);

            RenderTexture.ReleaseTemporary(dest);
            RenderTexture.ReleaseTemporary(texture3);

            BlendToScreen.SetTexture("_DstTex", texture4);
            BlendToScreen.SetTexture("_SrcTex", needToBuffer);
            Graphics.Blit(sourceBuffer, extratRenderTexture, BlendToScreen);


            RenderTexture.ReleaseTemporary(texture4);

            for (int k = 0; k < downsampleSteps; k++)
            {
                RenderTexture.ReleaseTemporary(textureArray[k * 2]);
                RenderTexture.ReleaseTemporary(textureArray[(k * 2) + 1]);
            }
        }
    }
}

