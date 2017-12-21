using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.Rendering;

namespace ImageEffects
{
    public class GlowOutLineObject : GlowObject
    {
        public static Dictionary<Renderer, GlowObject> list = new Dictionary<Renderer, GlowObject>();

        public void Awake()
        {
            if (glowTarget != GlowTarget.Self)
            {
                Init(list);
                return;
            }
            StartCoroutine(WaitForGlowOutLineInstance());
        }

        IEnumerator WaitForGlowOutLineInstance()
        {
            while (!(GlowOutLineManager.InstanceList.Count > 0))
            {
                yield return new WaitForSeconds(0.05f);
            }

            List<Renderer> reList = new List<Renderer>();
            reList.AddRange(GetComponentsInChildren<MeshRenderer>());
            foreach (Renderer r in reList)
            {
                if (GlowOutLineManager.IsGlowOutLineMaterial(r.sharedMaterial))
                {
                    foreach (GlowOutLineManager instance in GlowOutLineManager.InstanceList)
                    {
                        instance.PushRender(r, r.sharedMaterial);
                    }
                   
                }
            }
        }

        protected override void Init(Dictionary<Renderer, GlowObject> list)
        {
            Transform SkinDataObj = transform.Find("SkinDataObj");

            {
                MeshRenderer[] rs = gameObject.GetComponentsInChildren<MeshRenderer>();
                if (rs == null) return;
                foreach (MeshRenderer r in rs)
                {
                    if (SkinDataObj && r.transform.IsChildOf(SkinDataObj))
                    {
                        continue;
                    }
                    GlowMono m = r.gameObject.AddComponent<GlowMono>();
                    m.obj = this;
                    m.list = list;
                    ms.Add(m);
                }
            }
            {
                SkinnedMeshRenderer[] rs = gameObject.GetComponentsInChildren<SkinnedMeshRenderer>();
                if (rs == null) return;
                foreach (SkinnedMeshRenderer r in rs)
                {
                    if (SkinDataObj && r.transform.IsChildOf(SkinDataObj))
                    {
                        continue;
                    }
                    GlowMono m = r.gameObject.AddComponent<GlowMono>();
                    m.obj = this;
                    m.list = list;
                    ms.Add(m);
                }
            }
        }
    }
}
