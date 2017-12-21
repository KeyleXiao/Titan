//using UnityEngine;
//using System.Collections;
//using System.Collections.Generic;
//using System;


//public class GrassAdorning : MonoBehaviour
//{

//    public float m_angle;
//    public float m_strengthness;
//    public float m_level_velocity;
//    public float m_angular_velocity;
//    public float m_amplitude;
//    public float m_towards;
//    public float m_angular_acceleration;
//    public float m_level_acceleration;

//    private Mesh srcmesh = null;
//    private Vector3[] vertices = null;
//    private float[] weights = null;
//    private Vector3 center = new Vector3(0, 0, 0);
//    private Vector3 boundmin;
//    private Vector3 boundmax;

//    private Mesh newmesh = null;
//    private Vector3[] newverticies;// = new Vector3[srcmesh.vertices.Length];
//    private int[] newtriangles;// = new int[srcmesh.triangles.Length];
//    private Vector3[] newnormals;// = new Vector3[srcmesh.normals.Length];
//    private Vector2[] newuvs;// = new Vector2[srcmesh.uv.Length];
//    private Color[] newcolors;// = new Color[srcmesh.colors.Length];


//    private bool m_init = false;


//    public void Start()
//    {
//        m_strengthness = 0.0f;
//        m_angle = 0.0f;
//        m_towards = 1.0f;
//        m_level_velocity = 0.0f;
//        m_amplitude = 500.0f;
//        m_angular_velocity = 0.0f;
//        m_towards = -1;
//        m_angular_acceleration = 0.0f;
//        m_level_acceleration = 0.0f;

   

//        MeshRenderer mr = GetComponent<MeshRenderer>();
//        if (mr != null)
//        {
//            Bounds b=mr.bounds;
//            Vector3 boundcenter = b.center;
//            Vector3 boundsize = b.size*0.5f;
//            boundmin = boundcenter-boundsize*0.5f;
//            boundmax = boundcenter+boundsize*0.5f;
//        }

//        Animation a = GetComponent<Animation>();
//        if (a != null)
//        {
//            ResNode.DestroyRes(ref a);
//        }
//    }

//    void OnResFadedIn(SceneResInfo obj)
//    {
      
//        MeshRenderer mr = GetComponent<MeshRenderer>();
//        if (mr != null)
//        {
//            if (mr.material != null && mr.material.shader!=null)
//            {
//                //能找到对应的就替换，找不到就不理
//                string sname = mr.material.shader.name;
//                if (sname.StartsWith("Scene/")) return;
//                Shader shader = AssetBundleManager.GetShader("Scene/"+sname);
//                if (shader != null)
//                {
//                    mr.material.shader = shader;
//                }
//            }
//        }
//    }

//    void Init()
//    {
//        MeshFilter mf = GetComponent<MeshFilter>();
//        MeshRenderer mr = GetComponent<MeshRenderer>();

//        if (mf != null && mr != null)
//        {
//            srcmesh = mf.mesh;
//            newmesh = new Mesh();

//            if (srcmesh != null && newmesh != null && srcmesh.vertexCount < 300)
//            {
//                Bounds meshbound = srcmesh.bounds;
//                center = meshbound.center;
//                center.y = meshbound.min.y;
//                vertices = new Vector3[srcmesh.vertices.Length];
//                for (int i = 0; i < srcmesh.vertices.Length; i++)
//                {
//                    vertices[i] = srcmesh.vertices[i] - center;
//                }
//                weights = new float[srcmesh.vertices.Length];
//                for (int i = 0; i < srcmesh.vertices.Length; i++)
//                {
//                    weights[i] = (srcmesh.vertices[i].y - meshbound.min.y) / meshbound.size.y;
//                }
  
//                newverticies = new Vector3[srcmesh.vertices.Length];
//                newtriangles = new int[srcmesh.triangles.Length];
//                newnormals = new Vector3[srcmesh.normals.Length];
//                newuvs = new Vector2[srcmesh.uv.Length];
//                newcolors = new Color[srcmesh.colors.Length];
//                for (int i = 0; i < srcmesh.triangles.Length; i++)
//                {
//                    newtriangles[i] = srcmesh.triangles[i];
//                }
//                for (int i = 0; i < srcmesh.normals.Length; i++)
//                {
//                    newnormals[i] = srcmesh.normals[i];
//                }
//                for (int i = 0; i < srcmesh.uv.Length; i++)
//                {
//                    newuvs[i] = srcmesh.uv[i];
//                }
//                for (int i = 0; i < srcmesh.colors.Length; i++)
//                {
//                    newcolors[i] = srcmesh.colors[i];
//                }

//                newmesh.MarkDynamic();
//                mf.mesh = newmesh;
//            }
//        }

//        m_init = true;
//    }

//    public void OnDestroy()
//    {
//        vertices = null;
//        if (newmesh != null)
//        {
//            ResNode.DestroyRes(ref newmesh);
//        }
//    }

//    bool lasttrigger = false;
//    public void PerformVisDetection(GameObject go)
//    {
//        //MeshRenderer mr = GetComponent<MeshRenderer>();
//        //if (go != null && mr!=null)
//        //{
//        //    Vector3 testpos = go.transform.position;
//        //    testpos.y = mr.bounds.center.y;
//        //    bool curtrigger = mr.bounds.Contains(testpos);
//        //    if (curtrigger != lasttrigger)
//        //    {
//        //        if (curtrigger)
//        //        {
//        //            OnEnter();
//        //        }
//        //        else
//        //        {
//        //            OnLeave();
//        //        }
//        //        lasttrigger = curtrigger;
//        //    }
//        //}

//        Vector3 testpos = go.transform.position;
//        bool curtrigger = false;
//        if (testpos.x >= boundmin.x && testpos.z >= boundmin.z
//            && testpos.x <= boundmax.x && testpos.z <= boundmax.z)
//        {
//            curtrigger = true;
//        }
//        if (curtrigger != lasttrigger)
//        {
//            if (curtrigger)
//            {
//                OnEnter();
//            }
//            else
//            {
//                OnLeave();
//            }
//            lasttrigger = curtrigger;
//        }
//    }

//    private void OnEnter()
//    {
//        StartAction(30.0f);
//    }

//    private void OnLeave()
//    {
//        StopAction();
//    }

//    public void Update()
//    {

//        if (m_level_velocity < 0.01f && m_strengthness < 0.01f && m_angular_velocity<0.01f)
//        {
//            return;
//        }

//        if (m_init == false)
//        {
//            Init();
//        }

//        if (srcmesh == null || newmesh == null)
//        {
//            return;
//        }
//        if (srcmesh.vertexCount>300)
//        {
//            return;
//        }

//        float t = Time.deltaTime;

//        m_angular_velocity += m_angular_acceleration * t;
//        m_level_velocity += m_level_acceleration * t;
//        m_angular_velocity = Mathf.Clamp(m_angular_velocity, 0.0f, 10.0f);
//        m_level_velocity = Mathf.Clamp(m_level_velocity, -500.0f, 2000.0f);

//        m_strengthness += m_level_velocity * t;
//        m_angle += t * 5.0f * m_towards * m_angular_velocity;
//        while (m_angle < 0.0f)
//        {
//            m_angle += 360.0f;
//        }

//        if (m_strengthness > m_amplitude)
//        {
//            StopAction();
//        }
//        m_strengthness = Mathf.Clamp(m_strengthness, 0.0f, m_amplitude);

//        Vector3 vNormal;
//        Vector3 vBinormal;
//        Vector3 vTangent;
//        float fcos = Mathf.Cos(m_angle);
//        float fsin = Mathf.Sin(m_angle); 
//        vNormal.y = 1000.0f;
//        vNormal.x = m_strengthness * fcos;
//        vNormal.z = m_strengthness * fsin;
//        vNormal.Normalize();
//        vTangent.x = fcos;
//        vTangent.y = 0.0f;
//        vTangent.z = fsin;
//        vBinormal = Vector3.Cross(vNormal,vTangent);
//        vBinormal.Normalize();

//        for (int i = 0; i < vertices.Length; i++)
//        {
//            Vector3 dest = vertices[i].z * vTangent + vertices[i].y * vNormal + vertices[i].x * vBinormal + center;
//            Vector3 src = vertices[i];
//            newverticies[i] = Vector3.Lerp(src, dest, weights[i]);
//        }

//        newmesh.vertices = newverticies;
//        newmesh.SetIndices(newtriangles, MeshTopology.Triangles, 0);
//        newmesh.uv = newuvs;
//        newmesh.normals = newnormals;
     
//    }

//    public void StartAction(float amplitude)
//    {
//        m_towards = -1;
//        m_angular_acceleration = 0.0f;
//        m_level_acceleration = 0.0f;
//        if (m_level_velocity < 0.0f)
//        {
//            m_level_velocity = 0.0f;
//        }
//        if (m_angular_velocity < 0.0f)
//        {
//            m_angular_velocity = 0.0f;
//        }
//        m_level_velocity += 1000;
//        m_angular_velocity = 2.0f;
//        m_amplitude = amplitude * 20.0f;
//    }

//    public void StopAction()
//    {
//        m_level_acceleration = 0.0f;
//        m_level_velocity = - 500.0f;
//        m_angular_acceleration = -1.0f;
//    }
//}

