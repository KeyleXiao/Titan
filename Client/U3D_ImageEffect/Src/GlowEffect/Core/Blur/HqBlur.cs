
using System;
using UnityEngine;
namespace ImageEffects
{
    //internal class HqBlur : BlurBase, IBlur
    //{
    //    public void BlurAndBlitBuffer(RenderTexture rbuffer, RenderTexture destination, GlowSettings settings, bool highPrecision, bool bOptimitzed = true)
    //    {
    //        int baseResolution = (int) settings.baseResolution;
    //        base.downsampleMaterial.SetFloat("_Strength", settings.innerStrength / ((baseResolution != 4) ? ((float) 1) : ((float) 4)));
    //        base.composeMaterial.SetFloat("_Strength", settings.boostStrength);
    //        RenderTexture dest = RenderTexture.GetTemporary(rbuffer.width / baseResolution, rbuffer.height / baseResolution, 0, !highPrecision ? RenderTextureFormat.Default : RenderTextureFormat.ARGBHalf);
    //        RenderTexture texture2 = RenderTexture.GetTemporary(dest.width, dest.height, 0, !highPrecision ? RenderTextureFormat.Default : RenderTextureFormat.ARGBHalf);
    //        Graphics.Blit(rbuffer, dest, base.downsampleMaterial, (baseResolution != 4) ? 1 : 0);
    //        for (int i = 0; i < settings.iterations; i++)
    //        {
    //            base.BlurBuffer(dest, texture2);
    //        }
    //        Graphics.Blit(dest, destination, base.composeMaterial, (int) settings.blendMode);
    //        RenderTexture.ReleaseTemporary(dest);
    //        RenderTexture.ReleaseTemporary(texture2);
    //    }
    //}
}

