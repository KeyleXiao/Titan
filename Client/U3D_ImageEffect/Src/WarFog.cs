using UnityEngine;
using System.Collections;
using System.Collections.Generic;
namespace ImageEffects
{
    public struct WarFogNode
    {
        public Vector3 pos;
        public float rangestart;
        public float rangeend;
    }

    /// <summary>
    /// È«ÆÁ±ä»Ò
    /// </summary>

    [RequireComponent(typeof(Camera))]
    public class WarFog : ImageEffectNode
    {
        [System.NonSerialized]
        public Shader FogShader;
        private Material mat;
        //public float RangeStart = 0.25f;
        //public float RangeEnd = 0.35f;
        public float Base = 0.35f;
        public float dislimit = 100.0f;
        private List<WarFogNode> Node2DList = new List<WarFogNode>();
        private List<WarFogNode> Node3DList = new List<WarFogNode>();

        private int validShaderLOD = 100;

        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }

        public void Clear()
        {
            Node2DList.Clear();
            Node3DList.Clear();
        }

        public void Add(Vector3 pos, float rangestart, float rangeend, bool b3d)
        {
            WarFogNode node;
            node.pos = pos;
            node.rangestart = rangestart;
            node.rangeend = rangeend;
            if (b3d)
                Node3DList.Add(node);
            else
                Node2DList.Add(node);
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
            if (FogShader == null)
            {
                FogShader = ImageEffectManager.API_GetShader("ImageEffect/WarFog");
            }

            if (FogShader)
            {
                if (FogShader.isSupported)
                {
                    if (mat)
                    {
                        Destroy(mat);
                        mat = null;
                    }
                    mat = new Material(FogShader);
                    //mat.hideFlags = HideFlags.HideAndDontSave;
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


        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        {

            mat.SetFloat("_base", Base);
            mat.SetFloat("_sw", Screen.width);
            mat.SetFloat("_sh", Screen.height);

            RenderTexture tempsrc = RenderTexture.GetTemporary(source.width, source.height);
            RenderTexture tempdest = RenderTexture.GetTemporary(source.width, source.height);
            if (mat)
            {
                Graphics.Blit(null, tempsrc, mat, 2);
                Graphics.Blit(null, tempdest, mat, 2);
            }

            RenderTexture[] ret = new RenderTexture[2];
            ret[0] = tempsrc;
            ret[1] = tempdest;

            RenderTexture finaldest = tempdest;
            int index = 0;

            for (int i = 0; i < Node3DList.Count; i++)
            {
                WarFogNode n = Node3DList[i];
                Vector3 vec=n.pos - m_Camera.transform.position;
                vec.y=0.0f;
                float dis = vec.magnitude;
                if (dis < dislimit)
                {
                    drawmask3d(ret[index], ret[1 - index], mat, n);
                    finaldest = ret[1 - index];
                    index = 1 - index;
                }
            }

            float maxsize = Mathf.Max(Screen.width, Screen.height);
            for (int i = 0; i < Node2DList.Count; i++)
            {
                WarFogNode n = Node2DList[i];
                float padding = n.rangeend * maxsize+10;
                if (n.pos.x>-padding && n.pos.x<Screen.width+padding && n.pos.y>-padding && n.pos.y<Screen.height+padding)
                {
                    drawmask2d(ret[index], ret[1 - index], mat, n);
                    finaldest = ret[1 - index];
                    index = 1 - index;
                }
            }

            mat.SetTexture("_AlphaTex", finaldest);
            if (mat)
            {
                Graphics.Blit(source, destination, mat, 0);
            }
            else
            {
                Graphics.Blit(source, destination);
              
            }

            RenderTexture.ReleaseTemporary(tempsrc);
            RenderTexture.ReleaseTemporary(tempdest);
            return true;	
        }

        public void drawmask2d(RenderTexture source, RenderTexture destination, Material mat, WarFogNode node)
        {
            Vector4 pt = new Vector4(node.pos.x, node.pos.y, node.pos.z, 1.0f);
            mat.SetVector("_pt", pt);
            float maxsize = Mathf.Max(Screen.width, Screen.height);
            mat.SetFloat("_rstart", node.rangestart * maxsize);
            mat.SetFloat("_rend", node.rangeend * maxsize);
            if (mat)
            {
                Graphics.Blit(source, destination, mat, 1);
            }
        }

        public void drawmask3d(RenderTexture source, RenderTexture destination, Material mat, WarFogNode node)
        {
            Camera cam = m_Camera;
            Transform camtr = cam.transform;

            Matrix4x4 viewmatrix = cam.worldToCameraMatrix;
            Matrix4x4 prjmatrix = GL.GetGPUProjectionMatrix(cam.projectionMatrix,false);
            Matrix4x4 viewprjmatrix = prjmatrix * viewmatrix;
            Matrix4x4 invviewprjmatrix = viewprjmatrix.inverse;
            mat.SetMatrix("_invviewprjmatrix",invviewprjmatrix);

            var camPos = camtr.position;
            mat.SetVector("_CameraWS", camPos);
            Vector4 pt = new Vector4(node.pos.x, node.pos.y, node.pos.z, 1.0f);
            mat.SetVector("_pt", pt);
            float maxsize = Mathf.Max(Screen.width, Screen.height);
            mat.SetFloat("_rstart", node.rangestart * maxsize);
            mat.SetFloat("_rend", node.rangeend * maxsize);

            Graphics.Blit(source, destination, mat, 3);
        }
    }
}