using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.Rendering;

namespace ImageEffects
{
    
    [RequireComponent(typeof(Camera))]
    public partial class GlowManager : ImageEffectNode
    {
        public static  bool isSupportedHighQualityGlow = false;

        private bool LastUseHightQuality = false;
        public static bool CheckSupportHightQualityGlow()
        {
            isSupportedHighQualityGlow = true;

            if (!SystemInfo.supportsImageEffects || !SystemInfo.supportsRenderTextures || !SystemInfo.SupportsRenderTextureFormat(RenderTextureFormat.ARGBHalf))
            {
                isSupportedHighQualityGlow = false;
            }

            return isSupportedHighQualityGlow;
        }
    }
}