using System;
using UnityEngine;
using System.Collections.Generic;
namespace ImageEffects
{

    /// <summary>
    /// Glow渲染器信息
    /// </summary>
    public class GlowRenderInfo
    {
        public Material mat = null;
        public int subMeshIndex = 0;
        public Material InstanceMaterial = null;
        public Renderer hostRender = null;
        public string MaterialTag = string.Empty;
    }

    /// <summary>
    /// Glow网格信息
    /// </summary>
    public class GlowMeshInfo
    {
        public Material InstanceMaterial = null;
        public Material mat = null;
        public Matrix4x4 matrix;
        public Mesh mesh;
        public string MaterialTag = string.Empty;
    }

    //[ExecuteInEditMode]
    [RequireComponent (typeof(Camera))]
    public class ImageEffectNode : MonoBehaviour
	{
        protected bool  supportHDRTextures = true;
        protected bool  supportDX11 = false;
        protected bool  isSupported = true;
        protected Camera  m_Camera = null;
        protected List<ImageEffectManager.CameraRenderFlagsNode> flagReferenceList = new List<ImageEffectManager.CameraRenderFlagsNode>();
        protected Material CheckShaderAndCreateMaterial ( Shader s, Material m2Create)
		{
            if (!s)
			{
                Debug.Log("Missing shader in " + ToString ());
                enabled = false;
                return null;
            }

            if (s.isSupported && m2Create && m2Create.shader == s)
                return m2Create;

            if (!s.isSupported)
			{
                NotSupported ();
                Debug.Log("The shader " + s.ToString() + " on effect "+ToString()+" is not supported on this platform!");
                return null;
            }
            else
			{
                m2Create = new Material (s);
                //m2Create.hideFlags = HideFlags.DontSave;
                if (m2Create)
                    return m2Create;
                else return null;
            }
        }

        public virtual bool ImageEffect_RenderImage(RenderTexture src, RenderTexture dst,RenderBuffer depthBuffer)
        {
            Debug.LogWarning(ToString() + "必须实现ImageEffect_RenderImage函数！");
            return false;
        }

        /// <summary>
        /// LOD是否有效，每个后处理起效的LOD都不一样，要在子类重写
        /// </summary>
        /// <returns></returns>
        public virtual bool IsOnValidLOD()
        {
            Debug.LogWarning(ToString() + "必须实现IsOnValidLOD函数");
            return false;
        }

       public virtual void SetParam(string paramName, float Val)
       {
       }

        //给一个框
        public void OnEnable()
        {

        }

        public virtual void OnActive()
        {

        }

        public virtual void OnDeActive()
        {

        }

        public virtual void OnSceneCameraInit()
        {

        }

        public bool CheckSupport ()
		{
            return CheckSupport (false);
        }


        public virtual bool CheckResources ()
		{
            Debug.LogWarning ("CheckResources () for " + ToString() + " should be overwritten.");
            return isSupported;
        }

        public virtual void Awake()
        {           
            m_Camera = GetComponent<Camera>();
            ImageEffectManager.API_SetDirty(m_Camera);
            CheckResources();
        }


        protected bool CheckSupport (bool needDepth)
		{
            isSupported = true;
            supportHDRTextures = SystemInfo.SupportsRenderTextureFormat(RenderTextureFormat.ARGBHalf);
            supportDX11 = SystemInfo.graphicsShaderLevel >= 50 && SystemInfo.supportsComputeShaders;

            if (!SystemInfo.supportsImageEffects || !SystemInfo.supportsRenderTextures)
			{
                NotSupported ();
                return false;
            }

            if (needDepth && !SystemInfo.SupportsRenderTextureFormat (RenderTextureFormat.Depth))
			{
                NotSupported ();
                return false;
            }

            return true;
        }

        protected bool CheckSupport (bool needDepth,  bool needHdr)
		{
            if (!CheckSupport(needDepth))
                return false;

            if (needHdr && !supportHDRTextures)
			{
                NotSupported ();
                return false;
            }

            return true;
        }


        protected void ReportAutoDisable ()
		{
            Debug.LogWarning ("The image effect " + ToString() + " has been disabled as it's not supported on the current platform.");
        }

        protected void NotSupported ()
		{
            enabled = false;
            isSupported = false;
            return;
        }


        protected void DrawBorder (RenderTexture dest, Material material)
		{
            float x1;
            float x2;
            float y1;
            float y2;

            RenderTexture.active = dest;
            bool  invertY = true; // source.texelSize.y < 0.0ff;
            // Set up the simple Matrix
            GL.PushMatrix();
            GL.LoadOrtho();

            for (int i = 0; i < material.passCount; i++)
            {
                material.SetPass(i);

                float y1_; float y2_;
                if (invertY)
                {
                    y1_ = 1.0f; y2_ = 0.0f;
                }
                else
                {
                    y1_ = 0.0f; y2_ = 1.0f;
                }

                // left
                x1 = 0.0f;
                x2 = 0.0f + 1.0f/(dest.width*1.0f);
                y1 = 0.0f;
                y2 = 1.0f;
                GL.Begin(GL.QUADS);

                GL.TexCoord2(0.0f, y1_); GL.Vertex3(x1, y1, 0.1f);
                GL.TexCoord2(1.0f, y1_); GL.Vertex3(x2, y1, 0.1f);
                GL.TexCoord2(1.0f, y2_); GL.Vertex3(x2, y2, 0.1f);
                GL.TexCoord2(0.0f, y2_); GL.Vertex3(x1, y2, 0.1f);

                // right
                x1 = 1.0f - 1.0f/(dest.width*1.0f);
                x2 = 1.0f;
                y1 = 0.0f;
                y2 = 1.0f;

                GL.TexCoord2(0.0f, y1_); GL.Vertex3(x1, y1, 0.1f);
                GL.TexCoord2(1.0f, y1_); GL.Vertex3(x2, y1, 0.1f);
                GL.TexCoord2(1.0f, y2_); GL.Vertex3(x2, y2, 0.1f);
                GL.TexCoord2(0.0f, y2_); GL.Vertex3(x1, y2, 0.1f);

                // top
                x1 = 0.0f;
                x2 = 1.0f;
                y1 = 0.0f;
                y2 = 0.0f + 1.0f/(dest.height*1.0f);

                GL.TexCoord2(0.0f, y1_); GL.Vertex3(x1, y1, 0.1f);
                GL.TexCoord2(1.0f, y1_); GL.Vertex3(x2, y1, 0.1f);
                GL.TexCoord2(1.0f, y2_); GL.Vertex3(x2, y2, 0.1f);
                GL.TexCoord2(0.0f, y2_); GL.Vertex3(x1, y2, 0.1f);

                // bottom
                x1 = 0.0f;
                x2 = 1.0f;
                y1 = 1.0f - 1.0f/(dest.height*1.0f);
                y2 = 1.0f;

                GL.TexCoord2(0.0f, y1_); GL.Vertex3(x1, y1, 0.1f);
                GL.TexCoord2(1.0f, y1_); GL.Vertex3(x2, y1, 0.1f);
                GL.TexCoord2(1.0f, y2_); GL.Vertex3(x2, y2, 0.1f);
                GL.TexCoord2(0.0f, y2_); GL.Vertex3(x1, y2, 0.1f);

                GL.End();
            }

            GL.PopMatrix();
        }

        protected void API_AttachCameraRenderFlags(DepthTextureMode mod)
        {
            ImageEffectManager.CameraRenderFlagsNode reslut = ImageEffectManager.API_AttachCameraRenderFlags(m_Camera, mod);
            if(null != reslut)
            {
                flagReferenceList.Add(reslut);
            }
            
        }

        protected void DeAttachAllCameraRenderFlags()
        {
            for(int i = 0; i < flagReferenceList.Count;i++)
            {
                ImageEffectManager.CameraRenderFlagsNode nod = flagReferenceList[i];
                ImageEffectManager.API_DeAttachCameraRenderFlags(m_Camera,ref nod);
            }

            flagReferenceList.Clear();
        }

    }
}
