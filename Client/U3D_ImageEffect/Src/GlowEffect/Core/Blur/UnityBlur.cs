using System;
using UnityEngine;
namespace ImageEffects
{
    using System;
    using UnityEngine;

    //public class UnityBlur : IBlur
    //{
    //    private Material composeMaterial = null;
    //    private Material material = null;

    //    internal UnityBlur()
    //    {
    //        material = new Material(ImageEffectManager.API_GetShader("ImageEffect/Glow/BlurEffectConeTap"));
    //        composeMaterial = new Material(ImageEffectManager.API_GetShader("ImageEffect/Glow/Compose"));
    //    }

    //    public void BlurAndBlitBuffer(RenderTexture source, RenderTexture destination, GlowSettings settings, bool highPrecision, bool bOptimitzed = true)
    //    {
    //        material.SetFloat("_Strength", settings.innerStrength / 4f);
    //        composeMaterial.SetFloat("_Strength", settings.boostStrength);
    //        RenderTexture dest = RenderTexture.GetTemporary(source.width / 4, source.height / 4, 0, !highPrecision ? RenderTextureFormat.Default : RenderTextureFormat.ARGBHalf);
    //        RenderTexture texture2 = RenderTexture.GetTemporary(source.width / 4, source.height / 4, 0, !highPrecision ? RenderTextureFormat.Default : RenderTextureFormat.ARGBHalf);
    //        DownSample4x(source, dest);
    //        bool flag = true;
    //        for (int i = 0; i < settings.iterations; i++)
    //        {
    //            if (flag)
    //            {
    //                FourTapCone(dest, texture2, i, settings.blurSpread);
    //            }
    //            else
    //            {
    //                FourTapCone(texture2, dest, i, settings.blurSpread);
    //            }
    //            flag = !flag;
    //        }
    //        if (flag)
    //        {
    //            Graphics.Blit(dest, destination, composeMaterial);
    //        }
    //        else
    //        {
    //            Graphics.Blit(texture2, destination, composeMaterial);
    //        }
    //        RenderTexture.ReleaseTemporary(dest);
    //        RenderTexture.ReleaseTemporary(texture2);
    //    }

    //    private void DownSample4x(RenderTexture source, RenderTexture dest)
    //    {
    //        float y = 1f;
    //        Vector2[] offsets = new Vector2[] { new Vector2(-y, -y), new Vector2(-y, y), new Vector2(y, y), new Vector2(y, -y) };
    //        Graphics.BlitMultiTap(source, dest, material, offsets);
    //    }

    //    private void FourTapCone(RenderTexture source, RenderTexture dest, int iteration, float blurSpread)
    //    {
    //        float y = 0.5f + (iteration * blurSpread);
    //        Vector2[] offsets = new Vector2[] { new Vector2(-y, -y), new Vector2(-y, y), new Vector2(y, y), new Vector2(y, -y) };
    //        Graphics.BlitMultiTap(source, dest, material, offsets);
    //    }
    //}
}

