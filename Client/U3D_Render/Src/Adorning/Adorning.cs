using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

public class Adorning : MonoBehaviour
{
    public float alpha;
    public int bAnim = 1;
    private GameObject bodyobj = null;

    private Material[] materials = null;
    private Shader[] oldshaders = null;
    private static Shader alphashader = null;
    private bool bSwitchShader = false;
    private Color[] oldcs = null;
    private float[] oldmodes = null;
    private float[] oldsrcblends = null;
    private float[] olddestblends = null;
    private float[] oldzwrites = null;
    private int[] oldqueues = null;
    bool[] oldalphatest = null;
    bool[] oldalphablend = null;
    bool[] oldalphamul = null;

    Hashtable AnimParts = new Hashtable();
    public struct TransformInfo
    {
        public Vector3 pos;
        public Vector3 angle;
        public Vector3 scale;
        public Collider newcollider;
        public Collider oldcollider;
        public bool oldcolliderenable;
    }

    public void Awake()
    {
        bodyobj = gameObject; //以后应该用下面的异步加载时的方法
        Animator a = GetComponent<Animator>();
        if (a != null && a.runtimeAnimatorController==null)
        {
            ResNode.DestroyRes(ref a);
        }
    }

    //public void OnResLoaded(object sender, System.EventArgs e)
    //{
    //    GameObject obj = (GameObject)sender;
    //    bodyobj = obj.transform.GetChild(0).gameObject;
    //}

    public void SetVisable(bool vis)
    {
        if (vis)
        {
            //LoadResources();
        }

        if (bodyobj != null)
        {
            //obj.SetActive(vis);
            ParticleSystem[] pss = bodyobj.GetComponentsInChildren<ParticleSystem>();
            if (pss != null)
            {
                foreach (ParticleSystem ps in pss)
                {
                    if (vis==false)
                        ps.Stop();
                    else
                        ps.Play();
                }
            }
            Renderer[] mrs = bodyobj.GetComponentsInChildren<Renderer>();
            if (mrs != null)
            {
                foreach (Renderer mr in mrs)
                {
                    mr.enabled = vis;
                }
            }
        }

        if (!vis)
        {
            //UnloadResources();
        }
    }



    Dictionary<Material, Dictionary<string, string>> rs = new Dictionary<Material, Dictionary<string, string>>();
    public static string[] texnames = new string[] { "_MainTex", "_BumpMap", "_OcclusionMap", "_DetailNormalMap", "_DetailAlbedoMap" };

    private void LoadResources()
    {
        MeshRenderer[] renders = bodyobj.GetComponentsInChildren<MeshRenderer>();

        foreach (MeshRenderer mr in renders)
        {
            if (mr.materials != null)
            {
                foreach (Material mat in mr.materials)
                {
                    if (rs.ContainsKey(mat))
                    {
                        Dictionary<string, string> texpaths = rs[mat];
                        foreach (string texname in texnames)
                        {
                            if (mat.HasProperty(texname) && texpaths.ContainsKey(texname))
                            {
                                string path = texpaths[texname];
                                Texture tex = ResourceEx.Load(path, typeof(Texture2D)) as Texture2D;
                                mat.SetTexture(texname, tex);
                            }
                        }
                    }
                }
            }
        }
        rs.Clear();
    }

    private void UnloadResources()
    {
        MeshRenderer[] renders = bodyobj.GetComponentsInChildren<MeshRenderer>();
        List<Material> mats = new List<Material>();
        foreach (MeshRenderer mr in renders)
        {
            if (mr.materials != null)
            {
                mats.AddRange(mr.materials);
            }
        }
        foreach (Material mat in mats)
        {
            Dictionary<string, string> texpaths = new Dictionary<string, string>();
            foreach (string texname in texnames)
            {
                if (mat.HasProperty(texname))
                {
                    Texture tex = mat.GetTexture(texname);
                    string path = ResourceEx.GetPath(tex);
                    texpaths[texname] = path;
                    ResourceEx.Unload(path, typeof(Texture2D));
                    mat.SetTexture(texname, null);
                }
            }
            rs[mat] = texpaths;
        }

    }

    public void SetAlpha(float value)
    {
        float _value = value;
        if (materials != null)
        {
            for(int i=0;i<materials.Length;i++)
            {
                Material material = materials[i];
                if (material.HasProperty("_Color"))
                {
                    Color c = oldcs[i];
                    c.a *= value;
                    material.SetColor("_Color",c);
                    
                }
            }
        }
        alpha = value;
    }


    public void SwitchShaders()
    {
        if (alphashader == null)
        {
            alphashader = (Shader)Resources.Load("ModelFadeInEx");
        }

        MeshRenderer[] renders = bodyobj.GetComponentsInChildren<MeshRenderer>();
        List<Material> mats = new List<Material>();
        foreach (MeshRenderer mr in renders)
        {
            if (mr.materials != null)
            {
                mats.AddRange(mr.materials);
            }
        }
        materials = mats.ToArray();
        mats = null;

        int rnumber = materials.Length;
        if (rnumber > 0)
        {
            oldshaders = new Shader[rnumber];
            for (int j = 0; j < rnumber; j++)
            {
                oldshaders[j] = materials[j].shader;
            }

            oldcs = new Color[rnumber];
            for (int j = 0; j < rnumber; j++)
            {
                Color c = new Color(1,1,1,1);
                if (materials[j].HasProperty("_Color"))
                {
                    c = materials[j].GetColor("_Color");
                }
                oldcs[j] = c;
            }

            oldmodes = new float[rnumber];
            for (int j = 0; j < rnumber; j++)
            {
                float mode = 0.0f;
                if (materials[j].HasProperty("_Mode"))
                {
                    mode = materials[j].GetFloat("_Mode");
                }
                oldmodes[j] = mode;
            }

            oldsrcblends = new float[rnumber];
            for (int j = 0; j < rnumber; j++)
            {
                float srcblend = 1.0f;
                if (materials[j].HasProperty("_SrcBlend"))
                {
                    srcblend = materials[j].GetFloat("_SrcBlend");
                }
                oldsrcblends[j] = srcblend;
            }

            olddestblends = new float[rnumber];
            for (int j = 0; j < rnumber; j++)
            {
                float destblend = 0.0f;
                if (materials[j].HasProperty("_DstBlend"))
                {
                    destblend = materials[j].GetFloat("_DstBlend");
                }
                olddestblends[j] = destblend;
            }

            oldzwrites = new float[rnumber];
            for (int j = 0; j < rnumber; j++)
            {
                float zwrite = 1.0f;
                if (materials[j].HasProperty("_ZWrite"))
                {
                    zwrite = materials[j].GetFloat("_ZWrite");
                }
                oldzwrites[j] = zwrite;
            }

            oldqueues = new int[rnumber];
            for (int j = 0; j < rnumber; j++)
            {
                oldqueues[j] = materials[j].renderQueue;
            }

            oldalphatest = new bool[rnumber];
            for (int j = 0; j < rnumber; j++)
            {
                oldalphatest[j] = materials[j].IsKeywordEnabled("_ALPHATEST_ON");
            }

            oldalphablend = new bool[rnumber];
            for (int j = 0; j < rnumber; j++)
            {
                oldalphablend[j] = materials[j].IsKeywordEnabled("_ALPHABLEND_ON");
            }

            oldalphamul = new bool[rnumber];
            for (int j = 0; j < rnumber; j++)
            {
                oldalphamul[j] = materials[j].IsKeywordEnabled("_ALPHAPREMULTIPLY_ON");
            }
        }

        if (materials != null && alphashader != null)
        {
            foreach (Material material in materials)
            {
                material.shader = alphashader;
                if (material.HasProperty("_Mode"))
                {
                    material.SetFloat("_Mode",3.0f);
                }
                if (material.HasProperty("_SrcBlend"))
                {
                    material.SetFloat("_SrcBlend", (float)UnityEngine.Rendering.BlendMode.SrcAlpha);
                }
                if (material.HasProperty("_DstBlend"))
                {
                    material.SetFloat("_DstBlend", (float)UnityEngine.Rendering.BlendMode.OneMinusSrcAlpha);
                }
                if (material.HasProperty("_ZWrite"))
                {
                    material.SetFloat("_ZWrite", 0.0f);
                }
                material.renderQueue = 3000;
                material.DisableKeyword("_ALPHATEST_ON");
                material.DisableKeyword("_ALPHABLEND_ON");
                material.EnableKeyword("_ALPHAPREMULTIPLY_ON");
                //material.SetInt("_SrcBlend", (int)UnityEngine.Rendering.BlendMode.SrcAlpha);
                //material.SetInt("_DstBlend", (int)UnityEngine.Rendering.BlendMode.OneMinusSrcAlpha);
                //material.SetInt("_ZWrite", 0);


            }
        }

        bSwitchShader = true;
    }

    public void RestoreShaders()
    {
        if (!bSwitchShader)
        {
            return;
        }
        if (materials != null && oldshaders != null)
        {
            for (int j = 0; j < materials.Length; j++)
            {
                materials[j].shader = oldshaders[j];
                if (materials[j].HasProperty("_Color"))
                {
                    materials[j].SetColor("_Color",oldcs[j]);
                }
                if (materials[j].HasProperty("_Mode"))
                {
                    materials[j].SetFloat("_Mode", oldmodes[j]);
                }
                if (materials[j].HasProperty("_SrcBlend"))
                {
                    materials[j].SetFloat("_SrcBlend", oldsrcblends[j]);
                }
                if (materials[j].HasProperty("_DstBlend"))
                {
                    materials[j].SetFloat("_DstBlend", olddestblends[j]);
                }
                if (materials[j].HasProperty("_ZWrite"))
                {
                    materials[j].SetFloat("_ZWrite", oldzwrites[j]);
                }
                materials[j].renderQueue = oldqueues[j];
                if (oldalphatest[j])
                    materials[j].EnableKeyword("_ALPHATEST_ON");
                else
                    materials[j].DisableKeyword("_ALPHATEST_ON");
                if (oldalphablend[j])
                    materials[j].EnableKeyword("_ALPHABLEND_ON");
                else
                    materials[j].DisableKeyword("_ALPHABLEND_ON");
                if (oldalphamul[j])
                    materials[j].EnableKeyword("_ALPHAPREMULTIPLY_ON");
                else
                    materials[j].DisableKeyword("_ALPHAPREMULTIPLY_ON");
            }
        }

        materials = null;
        oldshaders = null;
        bSwitchShader = false;
    }

    public void ApplyForce(Vector3 center, Vector3 dir, float factor)
    {
        if (bAnim == 0 || bodyobj == null)
        {
            return;
        }

        AnimParts.Clear();

        if (bodyobj.transform.childCount > 0 && bAnim == 1)
        {
            for (int i = 0; i < bodyobj.transform.childCount; i++)
            {
                GameObject part = bodyobj.transform.GetChild(i).gameObject;
                ApplyForceSub(part, center, dir, factor);
            }
        }
        else
        {
            ApplyForceSub(bodyobj, center, dir, factor);
        }
    }

    private void ApplyForceSub(GameObject part, Vector3 center, Vector3 dir, float factor)
    {
        TransformInfo info = new TransformInfo();
        info.pos = part.transform.position;
        info.angle = part.transform.eulerAngles;
        info.scale = part.transform.localScale;

        Vector3 pos = part.transform.position;
        //pos.x += UnityEngine.Random.Range(-0.5f,0.5f);
        pos.y += UnityEngine.Random.Range(0.0f, 1.0f);
        //pos.z += UnityEngine.Random.Range(-0.5f,0.5f);
        part.transform.SetPosition( pos);
        Rigidbody rb = part.GetComponent<Rigidbody>();
        if (rb == null)
        {
            rb = part.AddComponent<Rigidbody>();
        }
        //		rb.constraints=RigidbodyConstraints.FreezePositionX | 
        //			RigidbodyConstraints.FreezePositionY | 
        //				RigidbodyConstraints.FreezePositionZ;

        //A low Drag value makes an object seem heavy. A high one makes it seem light. Typical values for Drag are between .001 (solid block of metal) and 10 (feather). 
        //阻力越大，物体看起来越轻，阻力越小，物体看起来越重。阻力的典型值在0.001（砖头）到10（羽毛）之间。
        //rb.drag = 1000000.0f;
        //rb.angularDrag = 1000000.0f;

        //rb.useGravity = false;

        info.oldcollider = part.GetComponent<Collider>();
        if (info.oldcollider != null)
        {
            info.oldcolliderenable = info.oldcollider.enabled;
            info.oldcollider.enabled = false;
        }

        SphereCollider collider = part.AddComponent<SphereCollider>(); 
        collider.radius *= factor;
        info.newcollider = collider;

        AnimParts[part] = info;
        rb.AddForce(dir);
    }

    public void StopForce()
    {
        if (bAnim == 0 || bodyobj == null)
        {
            return;
        }

        foreach (DictionaryEntry keypair in AnimParts)
        {
            GameObject part = (GameObject)keypair.Key;
            StopForceSub(part);
        }
        AnimParts.Clear();

    }

    private void StopForceSub(GameObject part)
    {
        Rigidbody rb = part.GetComponent<Rigidbody>();
        if (rb != null)
        {
            DestroyImmediate(rb);
        }

        //		BattleAdorningPart bap = part.GetComponent<BattleAdorningPart> ();
        //		if (bap != null) {
        //			DestroyImmediate(bap);
        //		}

        if (AnimParts.ContainsKey(part))
        {
            TransformInfo info = (TransformInfo)AnimParts[part];
            part.transform.localScale = info.scale;
            part.transform.eulerAngles = info.angle;
            part.transform.SetPosition( info.pos);

            if (info.newcollider != null)//删除物理破坏用collider
            {
                DestroyImmediate(info.newcollider);
            }

            if (info.oldcollider != null)//恢复原来的collider
            {
                info.oldcollider.enabled = info.oldcolliderenable;
            }
            
        }
    }
}