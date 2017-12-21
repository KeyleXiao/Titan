using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.Rendering;

namespace ImageEffects
{
    public class GlowMono : MonoBehaviour
    {
        public GlowObject obj;
        private Renderer r;
        public Dictionary<Renderer, GlowObject> list;

        public void Start()
        {
            if (list == null)
            {
                Destroy(this);
                return;
            }
            r = GetComponent<Renderer>();
            OnBecameVisible();
        }

        public void OnDestroy()
        {
            OnBecameInvisible();
            r = null;
        }

        public void OnBecameVisible()
        {
            if (r == null || obj == null || list==null) return;
            list[r] = obj;
        }

        public void OnBecameInvisible()
        {
            if (r == null || list==null) return;
            list.Remove(r);
        }
    }

    public class GlowObject : MonoBehaviour
    {
        public GlowTarget glowTarget = GlowTarget.OutSidePointed;

        public Color color = Color.white;
        [Range(0.0f, 10.0f)]
        public float strength = 1.0f;
        [Range(0.0f, 1.0f)]
        public float AlphaCutoff = 0.5f;
        public List<GlowMono> ms = new List<GlowMono>();

        protected virtual void Init(Dictionary<Renderer, GlowObject> list)
        {
            if(glowTarget == GlowTarget.Self)
            {
                return;
            }
            Renderer[] rs = gameObject.GetComponentsInChildren<Renderer>();
            if (rs == null) return;
            foreach (Renderer r in rs)
            {
                GlowMono m = r.gameObject.AddComponent<GlowMono>();
                m.obj = this;
                m.list = list;
                ms.Add(m);
            }
        }

        public void OnDestroy()
        {
            if (glowTarget != GlowTarget.Self)
            {
                foreach (GlowMono m in ms)
                {
                    Destroy(m);
                }
            }

        }
    }


}
