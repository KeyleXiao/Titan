using System;
using UnityEngine;
namespace ImageEffects
{
    [Serializable]
    public class GlowSettings
    {
        public Resolution baseResolution = Resolution.Quarter;
        public BlendMode blendMode = BlendMode.Additive;
        public float blurSpread = 0.6f;
        public float boostStrength = 1f;
        public DownsampleBlendMode downsampleBlendMode = DownsampleBlendMode.Max;
        public DownsampleResolution downsampleResolution = DownsampleResolution.Quarter;
        public int downsampleSteps = 1;
        public AnimationCurve falloff = AnimationCurve.EaseInOut(0f, 0f, 1f, 1f);
        public float falloffScale = 1f;
        public float innerStrength = 1f;
        public int iterations = 3;
        public bool normalize = true;
        public float outerStrength = 1f;
        public int radius = 4;

        //以下是高质量等级的参数

        //public bool UseHightQualityGlow = false;


        public float HighQualityGlowIntensity = 0.05f;
        public float HighQualityGlowBlurSize = 4.0f;

    }
}

