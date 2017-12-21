using UnityEngine;
using System.Collections;

namespace ImageEffects
{
    [RequireComponent(typeof(Camera))]
    public class VignetteAndChromaticAberration : ImageEffectNode
    {

        public enum AberrationMode
        {
            Simple = 0,
            Advanced = 1,
        }

        public AberrationMode mode = AberrationMode.Simple;

        public float intensity = 0.375f; // intensity == 0 disables pre pass (optimization)
        public float chromaticAberration = 0.2f;
        public float axialAberration = 0.5f;

        public float blur = 0.0f; // blur == 0 disables blur pass (optimization)
        public float blurSpread = 0.75f;

        public float luminanceDependency = 0.25f;

        public float blurDistance = 2.5f;

        private Shader vignetteShader;
        private Material vignetteMaterial;

        private Shader separableBlurShader;
        private Material separableBlurMaterial;

        private Shader chromAberrationShader;
        private Material chromAberrationMaterial;

        private int validShaderLOD = 400;

        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }

        public override bool CheckResources()
        {
            CheckSupport(false);

            vignetteShader = ImageEffectManager.API_GetShader("ImageEffect/Chromatic_Vignette");
            separableBlurShader = ImageEffectManager.API_GetShader("ImageEffect/Chromatic_Blur");
            chromAberrationShader = ImageEffectManager.API_GetShader("ImageEffect/ChromaticAberration");

            vignetteMaterial = CheckShaderAndCreateMaterial(vignetteShader, vignetteMaterial);
            separableBlurMaterial = CheckShaderAndCreateMaterial(separableBlurShader, separableBlurMaterial);
            chromAberrationMaterial = CheckShaderAndCreateMaterial(chromAberrationShader, chromAberrationMaterial);

            if (!isSupported)
                ReportAutoDisable();
            return isSupported;
        }

        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        {
            if (!ImageEffects.ImageEffectManager.AdvanceImangeEffectEnabled)
            {
                return false;
            }
            if (CheckResources() == false|| !IsOnValidLOD())
            {
                return false;	
            }

            int rtW = source.width;
            int rtH = source.height;

            bool doPrepass = (Mathf.Abs(blur) > 0.0f || Mathf.Abs(intensity) > 0.0f);

            float widthOverHeight = (1.0f * rtW) / (1.0f * rtH);
            float oneOverBaseSize = 1.0f / 512.0f;

            RenderTexture color = null;
            RenderTexture color2a = null;
            RenderTexture color2b = null;

            if (doPrepass)
            {
                color = RenderTexture.GetTemporary(rtW, rtH, 0, source.format);

                // Blur corners
                if (Mathf.Abs(blur) > 0.0f)
                {
                    int downScale = 4;
                    color2a = RenderTexture.GetTemporary(rtW / downScale, rtH / downScale, 0, source.format);

                    Graphics.Blit(source, color2a, chromAberrationMaterial, 0);

                    for (int i = 0; i < 2; i++)
                    {	// maybe make iteration count tweakable
                        separableBlurMaterial.SetVector("offsets", new Vector4(0.0f, blurSpread * oneOverBaseSize, 0.0f, 0.0f));
                        color2b = RenderTexture.GetTemporary(rtW / downScale, rtH / downScale, 0, source.format);
                        Graphics.Blit(color2a, color2b, separableBlurMaterial);
                        RenderTexture.ReleaseTemporary(color2a);

                        separableBlurMaterial.SetVector("offsets", new Vector4(blurSpread * oneOverBaseSize / widthOverHeight, 0.0f, 0.0f, 0.0f));
                        color2a = RenderTexture.GetTemporary(rtW / downScale, rtH / downScale, 0, source.format);
                        Graphics.Blit(color2b, color2a, separableBlurMaterial);
                        RenderTexture.ReleaseTemporary(color2b);
                    }
                }

                vignetteMaterial.SetFloat("_Intensity", intensity);		// intensity for vignette
                vignetteMaterial.SetFloat("_Blur", blur);					// blur intensity
                vignetteMaterial.SetTexture("_VignetteTex", color2a);	// blurred texture

                Graphics.Blit(source, color, vignetteMaterial, 0);			// prepass blit: darken & blur corners
            }

            chromAberrationMaterial.SetFloat("_ChromaticAberration", chromaticAberration);
            chromAberrationMaterial.SetFloat("_AxialAberration", axialAberration);
            chromAberrationMaterial.SetVector("_BlurDistance", new Vector2(-blurDistance, blurDistance));
            chromAberrationMaterial.SetFloat("_Luminance", 1.0f / Mathf.Max(Mathf.Epsilon, luminanceDependency));

            if (doPrepass) color.wrapMode = TextureWrapMode.Clamp;
            else source.wrapMode = TextureWrapMode.Clamp;
            Graphics.Blit(doPrepass ? color : source, destination, chromAberrationMaterial, mode == AberrationMode.Advanced ? 2 : 1);

            RenderTexture.ReleaseTemporary(color);
            RenderTexture.ReleaseTemporary(color2a);
            return true;
        }

    }
}

