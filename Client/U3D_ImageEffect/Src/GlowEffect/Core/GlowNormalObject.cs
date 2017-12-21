using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.Rendering;

namespace ImageEffects
{
    public class GlowNormalObject : GlowObject 
    {
        public static Dictionary<Renderer, GlowObject> list = new Dictionary<Renderer, GlowObject>();

        public void Awake()
        {
            if (glowTarget != GlowTarget.Self)
            {
                Init(list);
                return;
            }
            StartCoroutine(WaitForGlowInstance());
        }


        IEnumerator WaitForGlowInstance()
        {

            while (!(GlowManager.InstanceList.Count > 0))
            {
                yield return new WaitForSeconds(0.05f);
            }

            List<Renderer> reList = new List<Renderer>();
            reList.AddRange(GetComponentsInChildren<MeshRenderer>());
            foreach (Renderer r in reList)
            {
                if (GlowManager.IsGlowMaterial(r.sharedMaterial))
                {
                    foreach (GlowManager instance in GlowManager.InstanceList)
                    {
                        instance.PushRender(r, r.sharedMaterial);
                    }

                }

            }
        }
    }
}
