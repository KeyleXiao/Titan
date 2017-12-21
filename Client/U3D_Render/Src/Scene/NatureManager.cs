using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;

public class Spring : MonoBehaviour
{ }

public class Rain : MonoBehaviour
{ }

public class Snow : MonoBehaviour
{ }



public class NatureManager: MonoBehaviour
{
    public static NatureManager Instance = null;

    public SortedDictionary<float, Dictionary<string, float>> nodes = new SortedDictionary<float, Dictionary<string, float>>();

    public void Awake()
    {
        Instance = this;
    }

    public void OnDestroy()
    {
        Instance = null;
    }

    static string[] nodenames = new string[] { "Fall Leaves", "Lightning Bugs", "Snow Dust", "SnowSet", "RainSet", "Rain Mist" };
    GameObject natureobj = null;
    Dictionary<string, Transform> naturetrans = null;

    void GenerateSnowRainSpring()
    {
        GameObject prefab = Resources.Load("UniStorm/Prefabs/Nature") as GameObject;
        if (prefab == null) return;
        natureobj = ResNode.InstantiateRes(prefab);
        natureobj.transform.SetPosition(new Vector3(9999, 9999, 9999));
        Transform[] ts = natureobj.GetComponentsInChildren<Transform>();
        naturetrans= new Dictionary<string, Transform>();
        naturetrans.Clear();
        if (ts != null)
        {
            foreach (Transform t in ts)
            {
                naturetrans[t.name] = t;
            }
        }

        Transform tspring = natureobj.transform.FindChild("Spring");
        if (tspring != null) tspring.gameObject.AddComponent<Spring>();
        //tspring.gameObject.SetActive(false);

        Transform train = natureobj.transform.FindChild("Rain");
        if (train != null) train.gameObject.AddComponent<Rain>();
        //train.gameObject.SetActive(false);

        Transform tsnow = natureobj.transform.FindChild("Snow");
        if (tsnow != null) tsnow.gameObject.AddComponent<Snow>();
        //tsnow.gameObject.SetActive(false);

        Dictionary<string, float> time0 = new Dictionary<string, float>();

        Dictionary<string, float> time20 = new Dictionary<string, float>();

        Dictionary<string, float> time30 = new Dictionary<string, float>();
        //time30["Fall Leaves"] = 15;
        time30["Lightning Bugs"] = 1000;

        Dictionary<string, float> time40 = new Dictionary<string, float>();

        Dictionary<string, float> time60 = new Dictionary<string, float>();

        Dictionary<string, float> time70 = new Dictionary<string, float>();
        time70["Snow Dust"] = 25;
        time70["SnowSet"] = 300;


        Dictionary<string, float> time80 = new Dictionary<string, float>();

        Dictionary<string, float> time100 = new Dictionary<string, float>();

        Dictionary<string, float> time110 = new Dictionary<string, float>();
        time110["RainSet"] = 1000;
        time110["Rain Mist"] = 25;

        Dictionary<string, float> time120 = new Dictionary<string, float>();

        nodes.Clear();
        nodes.Add(0, time0);
        nodes.Add(20, time20);
        nodes.Add(30, time30);
        nodes.Add(40, time40);
        nodes.Add(60, time60);
        nodes.Add(70, time70);
        nodes.Add(80, time80);
        nodes.Add(100, time100);
        nodes.Add(110, time110);
        nodes.Add(120, time120);
    }

    void GenerateClouds()
    {
        //if (GameObject.Find("CloudsToy Mngr") != null) return;

        //GameObject cloudtopprefab = Resources.Load("CloudsToy/Prefabs/CloudsToy Mngr") as GameObject;
        //if (cloudtopprefab == null) return;
        //GameObject cloudtop = ResNode.InstantiateRes(cloudtopprefab);
        //cloudtop.name = "CloudsToy Mngr";
    }

    List<GameObject> birdgroups = new List<GameObject>();
    GameObject birdswarmgroups = null;
    void GenerateBirdGroups()
    {
        birdgroups.Clear();
        GameObject birdgroupprefab = Resources.Load("Birds Swarm/Swarm Orbit Center") as GameObject;
        if (birdgroupprefab == null) return;
        if (birdswarmgroups == null) birdswarmgroups = new GameObject("birdgroups");

        int n = 10;
        float range = 100.0f;
        for (int i = 0; i < n; i++)
        {
            GameObject birdgroup = ResNode.InstantiateRes(birdgroupprefab);
            birdgroup.SetActive(true);
            birdgroup.name = "birdgroup_" + i.ToString();
            Vector3 pos;
            pos.x = UnityEngine.Random.Range(-range, range);
            pos.y = UnityEngine.Random.Range(5, 10);
            pos.z = UnityEngine.Random.Range(-range, range);
            birdgroup.transform.SetPosition(pos);
            birdgroup.transform.parent = birdswarmgroups.transform;
            birdgroups.Add(birdgroup);
        }
    }

    public void GenerateAll()
    {
       if(GameLogicAPI.isMainMap(SceneManager.Instance.getCurrentMapId()))
       {
           GenerateClouds();
           GenerateBirdGroups();
           GenerateSnowRainSpring();
       }


        SetEnable("SwarmCenter", true);
        SetEnable("Rain", false);
        SetEnable("Snow", false);
        SetEnable("Spring", false);
    }

    public void DestroyAll()
    {
        natureobj = null;
        naturetrans = null;
    }

    private Type GetTypeFromAssemblies(string typename)
    {
        Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();
        foreach (Assembly assembly in assemblies)
        {
            Type t = assembly.GetType(typename);
            if (t != null) return t;
        }
        return null;
    }

    float fmod(float a, float b)
    {
        return a - Mathf.Floor(a / b) * b;
    }

    public float t=0.0f;
    public void Update()
    {
        if (natureobj == null) return;

        if (EntityFactory.MainHero != null)
        {
            natureobj.transform.parent = EntityFactory.MainHero.transform;
            natureobj.transform.localPosition = new Vector3(0, -5, 0);
        }


        t = fmod(Time.time,240)/2;

        float[] keys=new float[nodes.Keys.Count];
        nodes.Keys.CopyTo(keys, 0);
        for (int i = 0; i < keys.Length-1; i++)
        {
            int i1 = i;
            int i2 = i + 1;
            float t1 = keys[i1];
            float t2 = keys[i2];
            Dictionary<string, float> n1 = nodes[t1];
            Dictionary<string, float> n2 = nodes[t2];

            if (t >= t1 && t <= t2)
            {
                float alpha = (t - t1) / (t2 - t1);

                foreach (string nodename in nodenames)
                {
                    float v1 = 0.0f;
                    if (n1.ContainsKey(nodename))
                    {
                        v1 = n1[nodename];
                    }
                    float v2 = 0.0f;
                    if (n2.ContainsKey(nodename))
                    {
                        v2 = n2[nodename];
                    }

                    if (naturetrans.ContainsKey(nodename))
                    {
                        ParticleSystem ps = naturetrans[nodename].GetComponent<ParticleSystem>();
                        if (ps != null)
                        {
                            ParticleSystem.EmissionModule emm = ps.emission;
                            emm.rate = v2 * alpha + v1 * (1 - alpha);
                        }
                    }
                }
            }
        }
        
    }

    public void SwitchEnable(string typestring)
    {
        Type t = GetTypeFromAssemblies(typestring);
        if (t != null)
        {
            UnityEngine.Object[] objs = Resources.FindObjectsOfTypeAll(t);
            if (objs != null)
            {
                foreach (UnityEngine.Object obj in objs)
                {
                    Component c = obj as Component;
                    c.gameObject.SetActive(!c.gameObject.activeSelf);
                }
            }
        }
    }

    public void SetEnable(string typestring,bool v)
    {
        Type t = GetTypeFromAssemblies(typestring);
        if (t != null)
        {
            UnityEngine.Object[] objs = Resources.FindObjectsOfTypeAll(t);
            if (objs != null)
            {
                foreach (UnityEngine.Object obj in objs)
                {
                    Component c = obj as Component;
                    c.gameObject.SetActive(v);
                }
            }
        }
    }
}
