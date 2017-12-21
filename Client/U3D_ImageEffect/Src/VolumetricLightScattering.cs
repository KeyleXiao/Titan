using UnityEngine;
using UnityEngine.Rendering;
using System;
using System.Collections;
using System.Collections.Generic;
namespace ImageEffects
{

    public class VolumetricLightObject : GlowObject, IComparable<VolumetricLightObject>  
    {
        public static Dictionary<Renderer, GlowObject> list = new Dictionary<Renderer, GlowObject>();

         [Range(0.0f, 1.0f)]
        public float rangestart=0.3f;
         [Range(0.0f, 1.0f)]
        public float rangeend=0.5f;
         [Range(-1.0f, 1.0f)]
         public float depthbias = 0.0f;


         [Range(0.0f, 2.0f)]
         public float Density = 0.75f;
         [Range(0.0f, 2.0f)]
         public float Decay = 0.9f;
         public Vector3 offset = new Vector3(0, 0, 0);
         public Vector3 dir = new Vector3(0,-1,0);


         public int CompareTo(VolumetricLightObject obj)
         {
             return this.strength > obj.strength ? 1 : -1;
         }

         public void Awake()
        {
            Init(list);
        }
    }

    /// <summary>
    /// È«ÆÁ±ä»Ò
    /// </summary>

    [RequireComponent(typeof(Camera))]
    public class VolumetricLightScattering : ImageEffectNode
    {
        public Shader shader;
        public Material mat;



        public List<VolumetricLightObject> list=new List<VolumetricLightObject>();

        private int validShaderLOD = 200;

        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }

        public override void OnActive()
        {
            Init();
        }

        public override bool CheckResources()
        {
            return true;
        }

        public void Init()
        {
            if (shader == null)
            {
                shader = ImageEffectManager.API_GetShader("ImageEffect/VolumetricLightScattering");
            }

            if (shader)
            {
                if (shader.isSupported)
                {
                    mat = new Material(shader);
                   // mat.hideFlags = HideFlags.HideAndDontSave;
                }
                else
                {
                    enabled = false;
                }
            }
            else
            {
                enabled = false;
            }
        }

        float deltaTime = 0.0f;
        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        {
            if (!ImageEffects.ImageEffectManager.AdvanceImangeEffectEnabled)
            {
                return false;
            }
            list.Clear();
            foreach (KeyValuePair<Renderer, GlowObject> pair in VolumetricLightObject.list)
            {
                VolumetricLightObject node = (VolumetricLightObject)pair.Value;
                Vector3 prjpos = m_Camera.WorldToViewportPoint(node.transform.position);
                if (prjpos.x > -1.1f && prjpos.x < 1.1f && prjpos.y > -1.1f && prjpos.y < 1.1f && prjpos.z > 0.0f)
                {
                    list.Add(node);
                }
            }

            list.Sort((x, y) => -x.CompareTo(y));
            deltaTime += (Time.deltaTime - deltaTime) * 0.02f;
            float deltaValue = deltaTime * 1000.0f;
            int LimitNumber = 10;
            if (deltaValue > 60.0f) LimitNumber = 2;
            else if (deltaValue > 45.0f) LimitNumber = 3;
            else if (deltaValue > 30.0f) LimitNumber = 4;
            if (list.Count > LimitNumber)
            {
                list.RemoveRange(LimitNumber,list.Count-LimitNumber);
            }

            if (VolumetricLightObject.list.Count <= 0 || !IsOnValidLOD())
            {
                return false;
            }

            mat.SetFloat("_sw", Screen.width);
            mat.SetFloat("_sh", Screen.height);

            RenderTexture temp1 = RenderTexture.GetTemporary(source.width / div1, source.height / div1, 0);
            RenderTexture temp2 = RenderTexture.GetTemporary(source.width / div2, source.height / div2, 0);

            RenderTexture lastact = RenderTexture.active;
            RenderTexture.active = temp2;
            GL.Clear(false, true, Color.black);
            RenderTexture.active = lastact;


     

            foreach (VolumetricLightObject n in list)
            {
                drawmask(source,temp1,temp2, depthBuffer, mat, n);
            }

            mat.SetPass(2);
            mat.SetTexture("_AlphaTex", temp2);
            Graphics.Blit(source, destination, mat, 2);

            RenderTexture.ReleaseTemporary(temp1);
            RenderTexture.ReleaseTemporary(temp2);
            return true;	
        }

        public int div1 = 1;
        public int div2 = 4;

        public void drawmask(RenderTexture source, RenderTexture temp1, RenderTexture temp2, RenderBuffer depthBuffer, Material mat, VolumetricLightObject node)
        {
            Camera cam = m_Camera;
            //Transform camtr = cam.transform;

            Matrix4x4 viewmatrix = cam.worldToCameraMatrix;
            Matrix4x4 prjmatrix = GL.GetGPUProjectionMatrix(cam.projectionMatrix, false);
            Matrix4x4 viewprjmatrix = prjmatrix * viewmatrix;
            Matrix4x4 invviewprjmatrix = viewprjmatrix.inverse;
            mat.SetMatrix("_invviewprjmatrix",invviewprjmatrix);
            Vector3 prjpos = viewprjmatrix.MultiplyPoint(node.transform.position);


            //var camPos = camtr.position;
            //mat.SetVector("_CameraWS", camPos);


      
            mat.SetPass(0);
            Vector3 worldpos = node.transform.position + node.offset;
            Vector3 screenpt = m_Camera.WorldToScreenPoint(worldpos);
            Vector4 pt1 = new Vector4(screenpt.x, screenpt.y, screenpt.z, 1.0f);
            mat.SetVector("_lightscreenpt", pt1);
            Vector3 prjpt = m_Camera.WorldToViewportPoint(worldpos);
            Vector4 pt2 = new Vector4(prjpt.x, prjpt.y, prjpt.z, 1.0f);
            mat.SetVector("_lightprjpt", pt2);

            float maxsize = Mathf.Max(Screen.width, Screen.height);
            mat.SetFloat("_rstart", node.rangestart * maxsize);
            mat.SetFloat("_rend", node.rangeend * maxsize);
            mat.SetColor("_color", node.color);
            mat.SetFloat("_strength", node.strength);
            mat.SetFloat("_depth", prjpos.z+node.depthbias);
            mat.SetFloat("_Cutoff", node.AlphaCutoff);

            Vector3 screenpt2 = m_Camera.WorldToScreenPoint(worldpos + Vector3.Normalize(node.dir) * 10.0f);
            Vector3 screendir = Vector3.Normalize(screenpt2 - screenpt);
            Vector4 dir = new Vector4(screendir.x, screendir.y, screendir.z, 1.0f);
            mat.SetVector("_dir", dir);

            if (div1 > 1)
                Graphics.SetRenderTarget(temp1);
            else
                Graphics.SetRenderTarget(temp1.colorBuffer, depthBuffer);
            GL.Clear(false, true, Color.black);
            Graphics.Blit(source, mat, 0);


            mat.SetPass(1);
            float Weight = 1.0f;
            float Exposure = 0.05f;
            mat.SetFloat("Exposure", Exposure);
            mat.SetFloat("Density", node.Density);
            mat.SetFloat("Weight", Weight);
            mat.SetFloat("Decay", node.Decay);
            mat.SetTexture("_AlphaTex", temp1);

            Graphics.Blit(source, temp2, mat, 1);


        }
    }
}

