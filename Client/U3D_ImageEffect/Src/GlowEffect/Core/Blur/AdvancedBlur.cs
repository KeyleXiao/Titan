using System;
using UnityEngine;
namespace ImageEffects
{
    //public class AdvancedBlur : IBlur
    //{
    //    protected Material blurMaterial = null;
    //    protected Material composeMaterial = null;
    //    protected Material downsampleMaterial = null;
    //    private Matrix4x4 offsetsMatrix;
    //    private Matrix4x4 weightsMatrix;

    //    public AdvancedBlur()
    //    {
    //        Shader shader = ImageEffectManager.API_GetShader("ImageEffect/Glow/Compose");
    //        Shader shader2 = ImageEffectManager.API_GetShader("ImageEffect/Glow/Advanced Blur");
    //        Shader shader3 = ImageEffectManager.API_GetShader("ImageEffect/Glow/Downsample");
    //        composeMaterial = new Material(shader);
    //        composeMaterial.hideFlags = HideFlags.DontSave;
    //        blurMaterial = new Material(shader2);
    //        blurMaterial.hideFlags = HideFlags.DontSave;
    //        downsampleMaterial = new Material(shader3);
    //        downsampleMaterial.hideFlags = HideFlags.DontSave;
    //    }

    //    public void BlurAndBlitBuffer(RenderTexture rbuffer, RenderTexture destination, GlowSettings settings, bool highPrecision,bool bOptimitzed = true)
    //    {
    //        int radius = settings.radius;
    //        float[] numArray = new float[radius + 2];
    //        float num2 = 1f / ((float) (radius + 1));
    //        float num3 = 0f;
    //        for (int i = 0; i <= radius; i++)
    //        {
    //            float num5 = settings.falloff.Evaluate(1f - (i * num2));
    //            numArray[i] = num5;
    //            num3 += num5;
    //        }
    //        num3 = (num3 * 2f) - numArray[0];
    //        for (int j = 0; j <= radius; j++)
    //        {
    //            numArray[j] *= settings.falloffScale;
    //        }
    //        if (settings.normalize)
    //        {
    //            for (int n = 0; n <= radius; n++)
    //            {
    //                numArray[n] /= num3;
    //            }
    //        }
    //        int num8 = Mathf.CeilToInt(((float) radius) / 2f);
    //        weightsMatrix[0] = numArray[0];
    //        offsetsMatrix[0] = 0f;
    //        for (int k = 0; k <= (num8 - 1); k++)
    //        {
    //            float num10 = numArray[(k * 2) + 1];
    //            float num11 = numArray[(k * 2) + 2];
    //            float num12 = (k * 2) + 1;
    //            float num13 = (k * 2) + 2;
    //            weightsMatrix[k + 1] = num10 + num11;
    //            offsetsMatrix[k + 1] = ((num12 * num10) + (num13 * num11)) / weightsMatrix[k + 1];
    //        }
    //        int baseResolution = (int) settings.baseResolution;
    //        downsampleMaterial.SetFloat("_Strength", 1f / ((baseResolution != 4) ? ((float) 1) : ((float) 4)));
    //        RenderTexture dest = RenderTexture.GetTemporary(rbuffer.width / baseResolution, rbuffer.height / baseResolution, 0, !highPrecision ? RenderTextureFormat.Default : RenderTextureFormat.ARGBHalf);
    //        RenderTexture texture2 = RenderTexture.GetTemporary(dest.width, dest.height, 0, !highPrecision ? RenderTextureFormat.Default : RenderTextureFormat.ARGBHalf);
    //        Graphics.Blit(rbuffer, dest, downsampleMaterial, (baseResolution != 4) ? 1 : 0);
    //        int passOffset = (num8 * 2) - 2;
    //        for (int m = 0; m < settings.iterations; m++)
    //        {
    //            BlurBuffer(dest, texture2, passOffset);
    //        }
    //        composeMaterial.SetFloat("_Strength", settings.boostStrength);
    //        Graphics.Blit(dest, destination, composeMaterial, (int) settings.blendMode);
    //        RenderTexture.ReleaseTemporary(dest);
    //        RenderTexture.ReleaseTemporary(texture2);
    //    }

    //    protected void BlurBuffer(RenderTexture buffer, RenderTexture buffer2, int passOffset)
    //    {
    //        blurMaterial.SetMatrix("_offsets", offsetsMatrix);
    //        blurMaterial.SetMatrix("_weights", weightsMatrix);
    //        Graphics.Blit(buffer, buffer2, blurMaterial, passOffset);
    //        Graphics.Blit(buffer2, buffer, blurMaterial, passOffset + 1);
    //    }
    //}
}

