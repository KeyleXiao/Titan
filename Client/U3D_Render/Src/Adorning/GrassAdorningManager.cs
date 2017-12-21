using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;
using U3D_Render;

public partial class GrassAdorningManager : MonoBehaviour
{
    public bool enable = true;

    public static bool UseAnimation = true;
    public void Awake()
    {
        if(UseAnimation)
        {
            Awake_Animation();
        }
        else
        {
            Awake_Vertex();
        }
        StartCoroutine(TriggerEnumerator());
    }

    public void OnDestroy()
    {
        StopCoroutine(TriggerEnumerator());
    }

    void PerformVisDetection(GameObject go)
    {
        if (UseAnimation)
        {
            PerformVisDetection_Ani(go);
        }
        else
        {
            PerformVisDetection_Vertex(go);
        }

    }


    IEnumerator TriggerEnumerator()
    {
        while (true)
        {
            if (enable)
            {
                ICollection<EntityView> evs = EntityFactory.GetHeroList();
                int i = 0;
                foreach (EntityView ev in evs)
                {
                    
                    if (ev == null || ev.gameObject == null)
                    {
                        i++;
                        continue;
                    }

                    PerformVisDetection(ev.gameObject);

                    if (i % 2 == 0)
                    {
                        yield return new WaitForSeconds(0.1f);
                    }
                    i++;
                }
            }

            yield return new WaitForSeconds(0.1f);
        }

        //while (true)
        //{
        //    if (EntityFactory.MainHero != null)
        //    {
        //        PerformVisDetection(EntityFactory.MainHero);
        //    }
        //    yield return new WaitForSeconds(0.2f);
        //}
    }


    public static List<GrassAdorningManager> Instances = new List<GrassAdorningManager>();
    public static string[] grassgroupnames = new string[] { "grass_COL_INGORE", "Agrass_COL_INGORE" };
    public static void Init()
    { 
        foreach(string grassgroupname in grassgroupnames)
        {
            GameObject gograssgroup = GameObject.Find(grassgroupname);
            if (gograssgroup != null)
            {
                GrassAdorningManager gam = gograssgroup.AddComponent<GrassAdorningManager>();
                Instances.Add(gam);
            }
        }
    }


}
