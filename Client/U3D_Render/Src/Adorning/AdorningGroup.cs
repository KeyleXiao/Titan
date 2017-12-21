using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;


public class AdorningGroup : MonoBehaviour
{
    public int x;
    public int y;
    public Vector3 pos;
    public List<Adorning> objs = new List<Adorning>();



    #region alphaswitch

    private List<Adorning> switchedobjs = new List<Adorning>();

    public void SwitchAllShader()
    {
        foreach (Adorning obj in objs)
        {
            switchedobjs.Add(obj);
            obj.SwitchShaders();
        }
    }

    public void RestoreAllShader()
    {
        foreach (Adorning obj in switchedobjs)
        {
            obj.RestoreShaders();
        }
        switchedobjs.Clear();
    }

    public void SetAllAlpha(float alpha)
    {
        foreach (Adorning obj in objs)
        {
            obj.SetAlpha(alpha);
        }
    }

    #endregion

    #region showhide

    public bool visable = true;
    public bool loaded = true;
    public bool fadeenable = true;
    private float delay = 0.0f;
    private float fadetime = 0.25f;

    public void Show(float delay)
    {
        BreakPhysics();
        RestoreAllShader();
        StopCoroutine(HideEnumerator());
        if (delay>0.0f)
        {
            StartCoroutine(ShowEnumerator());
        }
        else
        {
            ShowImmedidity();
        }
    }

    public void Hide(float delay)
    {
        BreakPhysics();
        RestoreAllShader();
        StopCoroutine(ShowEnumerator());
        if (delay>0.0f)
        {
            StartCoroutine(HideEnumerator());
        }
        else
        {
            HideImmedidity();
        }
    }

    IEnumerator ShowEnumerator()
    {
        for (int i = 0; i < objs.Count; i++)
        {
            if (objs[i].alpha > 1.0f) continue;
            if (objs[i].alpha < 0.0f) objs[i].alpha=0.0f;

            objs[i].SetVisable(true);

            bool bImm=false; //如果人跑得很快已经很靠近这个物件组，那就无法再慢慢淡入淡出显示，需要立即全部加载了
            if (EntityFactory.MainHero != null)
            {
                Vector3 vec = EntityFactory.MainHero.transform.position - pos;
                float dis = vec.magnitude;
                if (dis < 25.0f)
                {
                    bImm = true;
                }
            }

            if (!bImm)
            {
                if (fadeenable)
                {
                    objs[i].SwitchShaders();
                    switchedobjs.Add(objs[i]);

                    float delta = Time.deltaTime * (1.0f / fadetime);
                    for (float alpha = objs[i].alpha; alpha < 1.0f; alpha += delta)
                    {
                        objs[i].SetAlpha(alpha);
                        yield return new WaitForEndOfFrame();
                    }

                    switchedobjs.Remove(objs[i]);
                    objs[i].RestoreShaders();
                    yield return new WaitForEndOfFrame();
                }
                else
                {
                    yield return new WaitForSeconds(fadetime);
                }
            }
  
            objs[i].alpha = 2.0f;

        }

        loaded = true;
        yield break;
    }



    IEnumerator HideEnumerator()
    {
        loaded = false;
        for (int i = 0; i < objs.Count; i++)
        {
            if (objs[i].alpha<0.0f) continue;
            if (objs[i].alpha > 1.0f) objs[i].alpha = 1.0f;

            if (fadeenable)
            {
                objs[i].SwitchShaders();
                switchedobjs.Add(objs[i]);

                float delta = Time.deltaTime * (1.0f / fadetime);
                for (float alpha = objs[i].alpha; alpha > 0.0f; alpha -= delta)
                {
                    objs[i].SetAlpha(alpha);
                    yield return new WaitForEndOfFrame();
                }

                switchedobjs.Remove(objs[i]);
                objs[i].RestoreShaders();
                objs[i].SetVisable(false);
                yield return new WaitForEndOfFrame();
            }
            else
            {
                objs[i].SetVisable(false);
                yield return new WaitForSeconds(fadetime);
            }

            objs[i].alpha = -1.0f;
            
        }
        yield break;
    }

    void ShowImmedidity()
    {
        for (int i = 0; i < objs.Count; i++)
        {
            objs[i].SetVisable(true);
            objs[i].alpha = 2.0f;
        }
        loaded = true;
    }

    void HideImmedidity()
    {
        loaded = false;
        for (int i = 0; i < objs.Count; i++)
        {
            objs[i].SetVisable(false);
            objs[i].alpha = -1.0f;
        }
    }

    public void SetVisable(bool vis, float delay, float fadetime,bool fadeenable)
    {
        this.fadetime = fadetime;
        this.delay = delay;
        this.fadeenable = fadeenable;
        if (this.visable == false && vis == true)//onenter
        {
            Show(delay);
        }
        else if (this.visable == true && vis == false)//onleave
        {
            Hide(delay);
        }
        this.visable = vis;
    }

    #endregion

    #region combine

    public class CombineGroup
    {
        public bool colliderenable=false;
        public bool breakable = true;
        public List<GameObject> gos=new List<GameObject>();
        public List<CombineInstance> cis=new List<CombineInstance>();
    }

    public void Combine()
    {
        Matrix4x4 myTransform = transform.worldToLocalMatrix;

        Dictionary<Material, CombineGroup> combines = new Dictionary<Material, CombineGroup>();
        MeshRenderer[] meshRenderers = GetComponentsInChildren<MeshRenderer>();
        if (meshRenderers != null)
        {
            foreach (MeshRenderer meshRenderer in meshRenderers)
            {
                foreach (Material material in meshRenderer.sharedMaterials)
                {
                    if (material != null && !combines.ContainsKey(material))
                    {
                        combines.Add(material, new CombineGroup());
                    }
                }
            }
        }


        Dictionary<GameObject, GameObject> processedgos = new Dictionary<GameObject, GameObject>();
        MeshFilter[] meshFilters = GetComponentsInChildren<MeshFilter>();
        foreach (MeshFilter filter in meshFilters)
        {
            if (filter.sharedMesh == null)
                continue;

            Material m = filter.GetComponent<Renderer>().sharedMaterial;
            if (combines.ContainsKey(m) == false)
            {
                continue;
            }

            CombineGroup cg = combines[m];

            CombineInstance ci = new CombineInstance();
            ci.mesh = filter.sharedMesh;
            ci.transform = myTransform * filter.transform.localToWorldMatrix;
            cg.cis.Add(ci);
            cg.gos.Add(filter.gameObject);

            if (filter.gameObject.GetComponent<MeshCollider>() != null)
            {
                cg.colliderenable = true;
            }

            filter.GetComponent<Renderer>().enabled = false;
            processedgos[filter.gameObject] = filter.gameObject;
        }

        int nDividedNumber = 4;
        foreach (Material m in combines.Keys)
        {
            CombineGroup cg = combines[m];

            int nSubGroup = cg.cis.Count / nDividedNumber;
            for (int i = 0; i < nSubGroup; i++)
            {
                List<CombineInstance> cis = new List<CombineInstance>();
                List<GameObject> gos = new List<GameObject>();
                for (int k = i * nDividedNumber; k < i * nDividedNumber + nDividedNumber; k++)
                {
                    if (k >= cg.cis.Count)
                    {
                        break;
                    }
                    cis.Add(cg.cis[k]);
                    gos.Add(cg.gos[k]);
                }

                GameObject combinedgo = new GameObject("Combined mesh");
                combinedgo.transform.parent = transform;
                combinedgo.transform.localPosition = Vector3.zero;
                combinedgo.transform.localRotation = Quaternion.identity;
                combinedgo.transform.localScale = Vector3.one;
                combinedgo.isStatic = true;
                MeshFilter filter = combinedgo.AddComponent<MeshFilter>();
                filter.mesh.CombineMeshes(cis.ToArray(), true, true);

                MeshRenderer renderer = combinedgo.AddComponent<MeshRenderer>();
                renderer.material = m;

                if (cg.colliderenable)
                {
                    MeshCollider mc = combinedgo.AddComponent<MeshCollider>();
                }

                string combinedname = "";
                for (int k = 0; k < gos.Count; k++)
                {
                    combinedname += gos[k].name + ";";
                }
                combinedgo.name = combinedname;
            }

        }

        foreach (MeshFilter filter in meshFilters)
        {
            GameObject go=filter.gameObject;
            if (processedgos.ContainsKey(go))
            {
                DestroyImmediate(go);
            }
        }

        List<GameObject> deletelist = new List<GameObject>();
        for (int i = 0; i < transform.childCount; i++)
        {
            Transform tchild = transform.GetChild(i);
            if (tchild.childCount <= 0 && tchild.gameObject.GetComponent<MeshFilter>()==null)
            {
                deletelist.Add(tchild.gameObject);
            }
        }

        foreach (GameObject go in deletelist)
        {
            DestroyImmediate(go);
        }
    }

 
    #endregion

    #region physic

    float fStartTime = -9999.0f;
    float DistanceThre = 25.0f;
    bool bAnim = false;

    public static float sizefactor = 0.15f;
    public static float forcefactor = 300.0f;
    public static float maxvelocity = 10.0f;


    public void BreakPhysics()  //中断物理破坏
    {
        StopCoroutine("PhysicAnimEnumerator");
        if (bAnim)
        {
            StopForceAll();
            RestoreAllShader();
        }
    }

    public void ApplyPhysics(Vector3 center,float range, Vector3 towards)  //应用物理破坏
    {
        if (loaded == false)//没显示全不应用物理
        {
            return;
        }

        float curTime = Time.time;
        if (curTime - fStartTime < 10.0f)
        {
            //return;
        }

        BreakPhysics();

        StartCoroutine(PhysicAnimEnumerator(center));
    }

    private void ApplyForceAll(Vector3 center)
    {
        for (int i = gameObject.transform.childCount - 1; i >= 0; i--)
        {
            GameObject go = gameObject.transform.GetChild(i).gameObject;

            Adorning ba = go.GetComponent<Adorning>();
            if (ba != null)
            {
                Vector3 dir = go.transform.position - center;
                float len = dir.magnitude;
                if (len < DistanceThre)
                {
                    float alpha = 1.0f - Mathf.Clamp01(len / DistanceThre);
                    dir.Normalize();
                    dir *= (alpha * forcefactor);
                    ba.ApplyForce(center, dir, sizefactor);
                }
            }
        }
    }

    private void StopForceAll()
    {
        for (int i = gameObject.transform.childCount - 1; i >= 0; i--)
        {
            GameObject go = gameObject.transform.GetChild(i).gameObject;

            Adorning ba = go.GetComponent<Adorning>();
            if (ba != null)
            {
                ba.StopForce();
            }
        }
    }

    private IEnumerator PhysicAnimEnumerator(Vector3 center)
    {
        fStartTime = Time.time;
        bAnim = true;
        ApplyForceAll(center);
        yield return new WaitForSeconds(3.0f);

        SwitchAllShader();
        for (float alpha = 1.0f; alpha > 0.0f; alpha -= Time.deltaTime)
        {
            SetAllAlpha(alpha);
            yield return new WaitForEndOfFrame();
        }

        StopForceAll();

        for (float alpha = 0.0f; alpha < 1.0f; alpha += Time.deltaTime)
        {
            SetAllAlpha(alpha);
            yield return new WaitForEndOfFrame();
        }
        RestoreAllShader();

        bAnim = false;
    }

    #endregion
}


