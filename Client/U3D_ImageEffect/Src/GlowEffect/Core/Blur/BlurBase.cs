using System;
using UnityEngine;
namespace ImageEffects
{
    public abstract class BlurBase
    {
        protected Material blurMaterial = null;
        protected Vector2[] blurOffsetsHorizontal = new Vector2[] { new Vector2(0f, 0f), new Vector2(-1.384615f, 0f), new Vector2(1.384615f, 0f), new Vector2(-3.230769f, 0f), new Vector2(3.230769f, 0f) };
        protected Vector2[] blurOffsetsVertical = new Vector2[] { new Vector2(0f, 0f), new Vector2(0f, -1.384615f), new Vector2(0f, 1.384615f), new Vector2(0f, -3.230769f), new Vector2(0f, 3.230769f) };
        protected Material composeMaterial = null;
        protected Material downsampleMaterial = null;
        private bool glMode = true;

        public BlurBase()
        {
            Shader compaseShader = ImageEffectManager.API_GetShader("ImageEffect/Glow/Compose");
            Shader blurShader = ImageEffectManager.API_GetShader("ImageEffect/Glow/Blur GL");
            if (!blurShader.isSupported)
            {
                glMode = false;
                blurShader = ImageEffectManager.API_GetShader("ImageEffect/Glow/Blur");
            }
            Shader downSampleShader = ImageEffectManager.API_GetShader("ImageEffect/Glow/Downsample");
            composeMaterial = new Material(compaseShader);
            //composeMaterial.hideFlags = HideFlags.DontSave;
            blurMaterial = new Material(blurShader);
           // blurMaterial.hideFlags = HideFlags.DontSave;
            downsampleMaterial = new Material(downSampleShader);
           // downsampleMaterial.hideFlags = HideFlags.DontSave;
        }

        protected void BlurBuffer(RenderTexture buffer, RenderTexture buffer2)
        {
            if (glMode)
            {
                Graphics.BlitMultiTap(buffer, buffer2, blurMaterial, blurOffsetsHorizontal);
                Graphics.BlitMultiTap(buffer2, buffer, blurMaterial, blurOffsetsVertical);
            }
            else
            {
                blurMaterial.SetFloat("_offset1", 1.384615f);
                blurMaterial.SetFloat("_offset2", 3.230769f);
                Graphics.Blit(buffer, buffer2, blurMaterial, 0);
                Graphics.Blit(buffer2, buffer, blurMaterial, 1);
            }
        }
    }
}

