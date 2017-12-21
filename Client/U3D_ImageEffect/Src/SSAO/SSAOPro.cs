#if (UNITY_4_0 || UNITY_4_0_1 || UNITY_4_1 || UNITY_4_2 || UNITY_4_3 || UNITY_4_5 || UNITY_4_6)
#define UNITY_4_X
#else
#define UNITY_5_X
#endif

using UnityEngine;
namespace ImageEffects
{
        using UnityEngine.Rendering;

    [RequireComponent(typeof(Camera))]
    public class SSAOPro :ImageEffectNode
    {
        static Mesh s_Quad;
        public static Mesh quad
        {
            get
            {
                if (s_Quad != null)
                    return s_Quad;

                var vertices = new[]
                {
                    new Vector3(-1f, -1f, 0f),
                    new Vector3( 1f,  1f, 0f),
                    new Vector3( 1f, -1f, 0f),
                    new Vector3(-1f,  1f, 0f)
                };

                var uvs = new[]
                {
                    new Vector2(0f, 0f),
                    new Vector2(1f, 1f),
                    new Vector2(1f, 0f),
                    new Vector2(0f, 1f)
                };

                var indices = new[] { 0, 1, 2, 1, 0, 3 };

                s_Quad = new Mesh
                {
                    vertices = vertices,
                    uv = uvs,
                    triangles = indices
                };
                s_Quad.RecalculateNormals();
                s_Quad.RecalculateBounds();

                return s_Quad;
            }
        }









        public enum SampleCount
        {
            Lowest = 3,
            Low = 6,
            Medium = 10,
            High = 16
        }

        public bool Downsampling = true;
        public bool ForceForwardCompatibility = false;
        public bool AmbientOnly = false;
        public bool HighPrecision = false;
        public SampleCount sampleCount = SampleCount.Medium;

        [Range(0.01f, 1.25f)]
        public float Radius = 0.3f;

        [Range(0f, 16f)]
        public float Intensity = 1.0f;

        public bool DebugAO = false;

        protected Shader mAOShader;
        protected Shader mBlitShader;
        protected Material mAOMat;
        protected Material mBlitMat;

        private bool bManagerActived = false;

        static class Uniforms
        {
            internal static readonly int _Intensity = Shader.PropertyToID("_Intensity");
            internal static readonly int _Radius = Shader.PropertyToID("_Radius");
            internal static readonly int _Downsample = Shader.PropertyToID("_Downsample");
            internal static readonly int _SampleCount = Shader.PropertyToID("_SampleCount");
            internal static readonly int _OcclusionTexture1 = Shader.PropertyToID("_OcclusionTexture1");
            internal static readonly int _OcclusionTexture2 = Shader.PropertyToID("_OcclusionTexture2");
            internal static readonly int _OcclusionTexture = Shader.PropertyToID("_OcclusionTexture");
            internal static readonly int _MainTex = Shader.PropertyToID("_MainTex");
            internal static readonly int _TempRT = Shader.PropertyToID("_TempRT");
        }

        const string k_BlitShaderString = "Hidden/Blit";
        const string k_ShaderString = "Hidden/AmbientOcclusion";


        readonly RenderTargetIdentifier[] m_MRT =
        {
            BuiltinRenderTextureType.GBuffer0, // Albedo, Occ
            BuiltinRenderTextureType.CameraTarget // Ambient
        };

        enum OcclusionSource
        {
            DepthTexture,
            DepthNormalsTexture,
            GBuffer
        }

        OcclusionSource occlusionSource
        {
            get
            {
                if (m_Camera != null)
                {
                    if ((m_Camera.actualRenderingPath == RenderingPath.DeferredShading) && !ForceForwardCompatibility)
                        return OcclusionSource.GBuffer;

                    if (HighPrecision && !(m_Camera.actualRenderingPath == RenderingPath.DeferredShading))
                        return OcclusionSource.DepthTexture;
                }

                return OcclusionSource.DepthNormalsTexture;
            }
        }

        bool ambientOnlySupported
        {
            get { return m_Camera.hdr && AmbientOnly && (m_Camera.actualRenderingPath == RenderingPath.DeferredShading) && !ForceForwardCompatibility; }
        }

        public DepthTextureMode GetCameraFlags()
        {
            var flags = DepthTextureMode.None;

            if (occlusionSource == OcclusionSource.DepthTexture)
                flags |= DepthTextureMode.Depth;

            if (occlusionSource != OcclusionSource.GBuffer)
                flags |= DepthTextureMode.DepthNormals;

            return flags;
        }

        public CameraEvent GetCameraEvent()
        {
            return ambientOnlySupported && !DebugAO
                   ? CameraEvent.BeforeReflections
                   : CameraEvent.BeforeImageEffectsOpaque;
        }

        public void PopulateCommandBuffer(CommandBuffer cb)
        {
            // Material setup
            mAOMat.shaderKeywords = null;
            mAOMat.SetFloat(Uniforms._Intensity, Intensity);
            mAOMat.SetFloat(Uniforms._Radius, Radius);
            mAOMat.SetFloat(Uniforms._Downsample, Downsampling ? 0.5f : 1f);
            mAOMat.SetInt(Uniforms._SampleCount, (int)sampleCount);

            int tw = m_Camera.pixelWidth;
            int th = m_Camera.pixelHeight;
            int ts = Downsampling ? 2 : 1;
            const RenderTextureFormat kFormat = RenderTextureFormat.ARGB32;
            const RenderTextureReadWrite kRWMode = RenderTextureReadWrite.Linear;
            const FilterMode kFilter = FilterMode.Bilinear;

            // AO buffer
            var rtMask = Uniforms._OcclusionTexture1;
            cb.GetTemporaryRT(rtMask, tw / ts, th / ts, 0, kFilter, kFormat, kRWMode);

            // AO estimation
            cb.Blit((Texture)null, rtMask, mAOMat, (int)occlusionSource);

            // Blur buffer
            var rtBlur = Uniforms._OcclusionTexture2;

            // Separable blur (horizontal pass)
            cb.GetTemporaryRT(rtBlur, tw, th, 0, kFilter, kFormat, kRWMode);
            cb.SetGlobalTexture(Uniforms._MainTex, rtMask);
            cb.Blit(rtMask, rtBlur, mAOMat, occlusionSource == OcclusionSource.GBuffer ? 4 : 3);
            cb.ReleaseTemporaryRT(rtMask);

            // Separable blur (vertical pass)
            rtMask = Uniforms._OcclusionTexture;
            cb.GetTemporaryRT(rtMask, tw, th, 0, kFilter, kFormat, kRWMode);
            cb.SetGlobalTexture(Uniforms._MainTex, rtBlur);
            cb.Blit(rtBlur, rtMask, mAOMat, 5);
            cb.ReleaseTemporaryRT(rtBlur);

            if (DebugAO)
            {
                cb.SetGlobalTexture(Uniforms._MainTex, rtMask);
                cb.Blit(rtMask, BuiltinRenderTextureType.CameraTarget, mAOMat, 8);
            }
            else if (ambientOnlySupported)
            {
                cb.SetRenderTarget(m_MRT, BuiltinRenderTextureType.CameraTarget);
                cb.DrawMesh(quad, Matrix4x4.identity, mAOMat, 0, 7);
            }
            else
            {
                var fbFormat = m_Camera.hdr ? RenderTextureFormat.DefaultHDR : RenderTextureFormat.Default;

                int tempRT = Uniforms._TempRT;
                cb.GetTemporaryRT(tempRT, m_Camera.pixelWidth, m_Camera.pixelHeight, 0, FilterMode.Bilinear, fbFormat);
                cb.Blit(BuiltinRenderTextureType.CameraTarget, tempRT, mBlitMat, 0);
                cb.SetGlobalTexture(Uniforms._MainTex, tempRT);
                cb.Blit(tempRT, BuiltinRenderTextureType.CameraTarget, mAOMat, 6);
                cb.ReleaseTemporaryRT(tempRT);
            }

            cb.ReleaseTemporaryRT(rtMask);
        }

        

        public bool prepareMaterials()
        {
            if (mAOShader == null)
            {
                mAOShader = ImageEffectManager.API_GetShader(k_ShaderString);

                if (mAOShader != null)
                {
                    if (mAOMat == null)
                    {
                        mAOMat = new Material(mAOShader);
                        mAOMat.hideFlags = HideFlags.HideAndDontSave;
                    }
                }
            }

            if (mBlitShader == null)
            {
                mBlitShader = ImageEffectManager.API_GetShader(k_BlitShaderString);

                if (mBlitShader != null)
                {
                    if (mBlitMat == null)
                    {
                        mBlitMat = new Material(mBlitShader);
                        mBlitMat.hideFlags = HideFlags.HideAndDontSave;
                    }
                }
            }

            return mAOMat != null && mBlitMat != null;
        }


        private int validShaderLOD = 400;

        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }

        public void Start()
        {
            m_Camera = GetComponent<Camera>();
            // Disable if we don't support image effects
            if (!SystemInfo.supportsImageEffects)
            {
                Debug.LogWarning("Image Effects are not supported on this platform.");
                enabled = false;
                return;
            }

            // Disable if we don't support render textures
            if (SystemInfo.supportsRenderTextures)
            {
            }
            else
            {
                Debug.LogWarning("RenderTextures are not supported on this platform.");
                enabled = false;
                return;
            }

            // Disable the image effect if the shaders can't run on the users graphics card
            if (mAOShader != null && !mAOShader.isSupported)
            {
                Debug.LogWarning("Unsupported shader (SSAO).");
                enabled = false;
                return;
            }
        }

        public void OnDestroy()
        {
            if (mAOMat != null)
                DestroyImmediate(mAOMat);

            if (mBlitMat != null)
                DestroyImmediate(mBlitMat);
        }

        CommandBuffer cb;
        CameraEvent camEvent;
        void OnPreRender()
        {
            if (!enabled || !ImageEffects.ImageEffectManager.AdvanceImangeEffectEnabled || !bManagerActived)
            {
                if (cb != null)
                {
                    m_Camera.RemoveCommandBuffer(camEvent, cb);
                    cb.Dispose();
                }

                cb = null;
                return;
            }

            if (cb == null)
            {
                cb = new CommandBuffer();
                camEvent = GetCameraEvent();
                m_Camera.AddCommandBuffer(camEvent, cb);
            }

            // Command buffer-based effects should be set-up here
            cb.Clear();
            PopulateCommandBuffer(cb);
        }

        void OnDisable()
        {
            bool oldActived = bManagerActived;
            OnDeActive();
            bManagerActived = oldActived;
        }

        //[ImageEffectOpaque]
        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        {
            return false;
        }
        ////[ImageEffectOpaque]
        //public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        //{
        //    if (!ImageEffects.ImageEffectManager.AdvanceImangeEffectEnabled)
        //    {
        //        return false;
        //    }
        //    // Fail checks
        //    if (ShaderSSAO == null || !IsOnValidLOD())
        //    {
        //        return false;
        //    }

        //    // Shader keywords & pass ID
        //    int ssaoPass = SetShaderStates();
        //    Matrix4x4 invviewprjmatrix;
        //    if (gpuprj)
        //    {
        //        Matrix4x4 viewmatrix = m_Camera.worldToCameraMatrix;
        //        Matrix4x4 prjmatrix = GL.GetGPUProjectionMatrix(m_Camera.projectionMatrix, false);
        //        Matrix4x4 viewprjmatrix = prjmatrix * viewmatrix;
        //        invviewprjmatrix = viewprjmatrix.inverse;
        //    }
        //    else
        //    {
        //        invviewprjmatrix = (m_Camera.projectionMatrix * m_Camera.worldToCameraMatrix).inverse;
        //    }

        //    Material.SetMatrix("_InverseViewProject", invviewprjmatrix);
        //    Material.SetMatrix("_CameraModelView", m_Camera.cameraToWorldMatrix);

        //    Material.SetTexture("_NoiseTex", NoiseTexture);
        //    Material.SetVector("_Params1", new Vector4(NoiseTexture == null ? 0f : NoiseTexture.width, Radius, Intensity, Distance));
        //    Material.SetVector("_Params2", new Vector4(Bias, LumContribution, CutoffDistance, CutoffFalloff));
        //    Material.SetColor("_OcclusionColor", OcclusionColor);

        //    // Prep work
        //    int d = BlurDownsampling ? Downsampling : 1;
        //    RenderTexture rt1 = RenderTexture.GetTemporary(source.width / d, source.height / d, 0, RenderTextureFormat.ARGB32);
        //    RenderTexture rt2 = RenderTexture.GetTemporary(source.width / Downsampling, source.height / Downsampling, 0, RenderTextureFormat.ARGB32);
        //    Graphics.Blit(rt1, rt1, Material, 0); // Clear

        //    // SSAO
        //    Graphics.Blit(source, rt1, Material, ssaoPass);

        //    // Horizontal blur
        //    Material.SetVector("_Direction", new Vector2(1f / source.width, 0f));
        //    Graphics.Blit(rt1, rt2, Material, 5);

        //    // Vertical blur
        //    Material.SetVector("_Direction", new Vector2(0f, 1f / source.height));

        //    if (!DebugAO)
        //    {

        //        Graphics.Blit(rt2, rt1, Material, 5);
        //        Material.SetTexture("_SSAOTex", rt1);
        //        Graphics.Blit(source, destination, Material, 6);
        //    }
        //    else
        //    {
        //        Graphics.Blit(rt2, destination, Material, 5);
        //    }

        //    RenderTexture.ReleaseTemporary(rt1);
        //    RenderTexture.ReleaseTemporary(rt2);
        //    return true;
        //}

        public override bool CheckResources()
        {
            return prepareMaterials();
        }

        public override void OnActive()
        {
            API_AttachCameraRenderFlags(GetCameraFlags());
            bManagerActived = true;
        }

        public override void OnDeActive()
        {
            if (cb != null)
            {
                m_Camera.RemoveCommandBuffer(camEvent, cb);
                cb.Dispose();
            }

            cb = null;
            DeAttachAllCameraRenderFlags();

            bManagerActived = false;
        }
 
    }
}

