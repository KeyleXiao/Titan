using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace Effect
{
    public class UVProjector : MonoBehaviour
    {
        public int TileX = 1;
        public int TileY = 1;
        public float Speed = 1.0f;

        private Material mat = null;
        private Projector r = null;
        private float lasttime = 0.0f;
        private int framecount = 0;
        private bool bPlay = false;

        public bool IsPlay()
        {
            return bPlay;
        }

        public void Play()
        {
            bPlay = true;
            framecount = 0;
        }

        public void Stop()
        {
            bPlay = false;
        }

        public void Update()
        {
            if (!bPlay)
            {
                return;
            }
            if (r == null)
            {
                r = GetComponent<Projector>();
            }
            if (r == null) { return; };

            mat = r.material;

            string textpro = "_ShadowTex";
            if (mat.HasProperty(textpro))
            {
                Vector2 scale;
                scale.x = 1.0f / TileX;
                scale.y = 1.0f / TileY;
                mat.SetTextureScale(textpro, scale);

                int framex = framecount % TileX;
                int framey = framecount / TileX;

                Vector2 offset;
                offset.x = framex * scale.x;
                offset.y = (1.0f - scale.y) - framey * scale.y;
                mat.SetTextureOffset(textpro, offset);
            }

            float curtime = Time.time;
            float frametime = 1.0f / Speed / 32.0f;
            if (curtime - lasttime > frametime)
            {
                framecount = (framecount + 1) % (TileX * TileY);
                lasttime = curtime;
            }

        }
    }

}
