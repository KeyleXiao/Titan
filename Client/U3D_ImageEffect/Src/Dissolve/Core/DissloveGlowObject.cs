using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.Rendering;

namespace ImageEffects
{
    public class DissloveGlowObject:GlowObject
    {
        public static Dictionary<Renderer, GlowObject> list = new Dictionary<Renderer, GlowObject>();

        public void Awake()
        {
            if (glowTarget != GlowTarget.Self)
            {
                Init(list);
                return;
            }
            StartCoroutine(WaitForDissloveGlowInstance());
        }

        IEnumerator WaitForDissloveGlowInstance()
        {
            while (!(DissolveGlowManager.InstanceList.Count > 0))
            {
                yield return new WaitForSeconds(0.05f);
            }

            List<Renderer> reList = new List<Renderer>();
            reList.AddRange(GetComponentsInChildren<MeshRenderer>());
            foreach (Renderer r in reList)
            {
                if (DissolveGlowManager.IsDissloveGlowMaterial(r.sharedMaterial))
                {
                    foreach (DissolveGlowManager instance in DissolveGlowManager.InstanceList)
                    {
                        instance.PushRender(r, r.sharedMaterial);
                    }
                    
                }
                
            }
        }
    }
}
