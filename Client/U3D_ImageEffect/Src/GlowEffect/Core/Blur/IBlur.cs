using System;
using UnityEngine;

namespace ImageEffects
{

    public interface IBlur
    {
        void BlurAndBlitBuffer(RenderTexture rbuffer,RenderTexture needToBuffer, RenderTexture destination,RenderTexture extratRenderTexture, GlowSettings settings, bool highPrecision);
    }
}

