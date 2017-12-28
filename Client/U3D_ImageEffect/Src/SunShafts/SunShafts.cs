using System;
using UnityEngine;

namespace ImageEffects
{

    [RequireComponent (typeof(Camera))]
    public class SunShafts : ImageEffectNode
    {
        public enum SunShaftsResolution
        {
            Low = 0,
            Normal = 1,
            High = 2,
        }

        public enum ShaftsScreenBlendMode
        {
            Screen = 0,
            Add = 1,
        }


        public SunShaftsResolution resolution = SunShaftsResolution.Low;

        public Transform sunTransform;
        public int radialBlurIterations = 2;
        public Color sunColor = Color.white;
        public Color sunThreshold = new Color(0.87f,0.74f,0.65f);
        public float sunShaftBlurRadius = 2.5f;
        public float sunShaftIntensity = 1.15f;

        public float maxRadius = 0.75f;
        public float maxDistance = 20f;

        public Shader sunShaftsShader;
        private Material sunShaftsMaterial;

        public Shader simpleClearShader;
        private Material simpleClearMaterial;

        public Cubemap skyMask;

        public bool hasArea = false;

        //OBB结构
        public Vector3 area_u0 = Vector3.right;
        public Vector3 area_u1 = Vector3.forward;
        public Vector3 area_Center = Vector3.zero;
        public float area_e0 = 1;
        public float area_e1 = 1;
        private bool canUseMask = true;

        private int validShaderLOD = 200;

        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }

        public override bool CheckResources ()
        {
            CheckSupport (true);

            if (null == sunShaftsShader)
            {
                sunShaftsShader = ImageEffectManager.API_GetShader("ImageEffect/SunShaft/SunShaftsComposite");
            }
            if (null == simpleClearShader)
            {
                simpleClearShader = ImageEffectManager.API_GetShader("ImageEffect/Common/SimpleClear");
            }
            sunShaftsMaterial = CheckShaderAndCreateMaterial (sunShaftsShader, sunShaftsMaterial);
            simpleClearMaterial = CheckShaderAndCreateMaterial (simpleClearShader, simpleClearMaterial);

            if (!isSupported)
                ReportAutoDisable ();
            canUseMask = false;
            var skybox = RenderSettings.skybox;
            if (skybox)
            {
                if (skybox.HasProperty("_Tint"))
                {
                    canUseMask = true;
                }
            }
            return isSupported;
        }

        public override void OnActive()
        {
            API_AttachCameraRenderFlags(DepthTextureMode.Depth);
            canUseMask = false;
            var skybox = RenderSettings.skybox;
            if (skybox)
            {
                if (skybox.HasProperty("_Tint"))
                {
                    canUseMask = true;
                }
            }
        }

        public override void OnDeActive()
        {

           DeAttachAllCameraRenderFlags();
        }

        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        {
            if (!ImageEffects.ImageEffectManager.AdvanceImangeEffectEnabled)
            {
                return false;
            }
            if (CheckResources()==false || !IsOnValidLOD()) {
                return false;
            }

            //根据人物距离控制强度，如果足够远就关闭
            float fallOff = 0;
            if (hasArea)
            {
                ImageEffectManager result = null;
                ImageEffectManager.ImageEffectManagerTable.TryGetValue(m_Camera, out result);
                
                if (result != null)
                {
                    if (result.hero != null)
                    {
                        float distance = GetDistanceToOBB(result.hero.transform.position);
                        fallOff = distance / maxDistance;
                        fallOff = Mathf.Clamp01(fallOff);
                        //Debug.Log("sun distance: " + distance);
                    }
                }
                //Debug.Log("sun fallOff: " + fallOff);
            }


            int divider = 4;
            if (resolution == SunShaftsResolution.Normal)
                divider = 2;
            else if (resolution == SunShaftsResolution.High)
                divider = 1;

            Vector3 v = Vector3.one * 0.5f;
            if (sunTransform)
                v = m_Camera.WorldToViewportPoint(sunTransform.position);
            else
                v = new Vector3(0.5f, 0.5f, 0.0f);

            if (v.x < -0.15 || v.x > 1.15 || v.y < -0.15 || v.y > 1.15 || fallOff>=1)
            {
                return false;
            }

            int rtW = source.width / divider;
            int rtH = source.height / divider;

            RenderTexture lrColorB;
            RenderTexture lrDepthBuffer = RenderTexture.GetTemporary (rtW, rtH, 0);

            // mask out everything except the skybox
            // we have 2 methods, one of which requires depth buffer support, the other one is just comparing images

            sunShaftsMaterial.SetVector ("_BlurRadius4", new Vector4 (1.0f, 1.0f, 0.0f, 0.0f) * sunShaftBlurRadius );
            sunShaftsMaterial.SetVector ("_SunPosition", new Vector4 (v.x, v.y, v.z, maxRadius));
            sunShaftsMaterial.SetVector ("_SunThreshold", sunThreshold);

            var skybox = RenderSettings.skybox;
            if (skybox != null && skyMask != null && canUseMask)
            {
                sunShaftsMaterial.SetTexture("_SkyCubemap", skyMask);
                sunShaftsMaterial.SetColor("_SkyTint", skybox.GetColor("_Tint"));
                sunShaftsMaterial.SetFloat("_SkyExposure", skybox.GetFloat("_Exposure"));
                sunShaftsMaterial.SetFloat("_SkyRotation", skybox.GetFloat("_Rotation"));
                sunShaftsMaterial.EnableKeyword("ENABLE_SKY_MASK");
            }else
            {
                sunShaftsMaterial.DisableKeyword("ENABLE_SKY_MASK");
            }
            //Graphics.Blit(source, lrDepthBuffer, sunShaftsMaterial, 2);

            var cam = m_Camera;
            var camtr = cam.transform;
            var camNear = cam.nearClipPlane;
            var camFar = cam.farClipPlane;

            var tanHalfFov = Mathf.Tan(cam.fieldOfView * Mathf.Deg2Rad / 2);
            var toRight = camtr.right * camNear * tanHalfFov * cam.aspect;
            var toTop = camtr.up * camNear * tanHalfFov;

            var v_tl = camtr.forward * camNear - toRight + toTop;
            var v_tr = camtr.forward * camNear + toRight + toTop;
            var v_br = camtr.forward * camNear + toRight - toTop;
            var v_bl = camtr.forward * camNear - toRight - toTop;

            var v_s = v_tl.magnitude * camFar / camNear;

            // Draw screen quad.
            RenderTexture.active = lrDepthBuffer;


            sunShaftsMaterial.SetTexture("_MainTex", source);
            sunShaftsMaterial.SetPass(2);

            GL.PushMatrix();
            GL.LoadOrtho();
            GL.Begin(GL.QUADS);

            GL.MultiTexCoord2(0, 0, 0);
            GL.MultiTexCoord(1, v_bl.normalized * v_s);
            GL.Vertex3(0, 0, 0.1f);

            GL.MultiTexCoord2(0, 1, 0);
            GL.MultiTexCoord(1, v_br.normalized * v_s);
            GL.Vertex3(1, 0, 0.1f);

            GL.MultiTexCoord2(0, 1, 1);
            GL.MultiTexCoord(1, v_tr.normalized * v_s);
            GL.Vertex3(1, 1, 0.1f);

            GL.MultiTexCoord2(0, 0, 1);
            GL.MultiTexCoord(1, v_tl.normalized * v_s);
            GL.Vertex3(0, 1, 0.1f);

            GL.End();
            GL.PopMatrix();


            // radial blur:

            radialBlurIterations = Mathf.Clamp (radialBlurIterations, 1, 4);

            float ofs = sunShaftBlurRadius * (1.0f / 768.0f);

            sunShaftsMaterial.SetVector ("_BlurRadius4", new Vector4 (ofs, ofs, 0.0f, 0.0f));
            sunShaftsMaterial.SetVector ("_SunPosition", new Vector4 (v.x, v.y, v.z, maxRadius));

            for (int it2 = 0; it2 < radialBlurIterations; it2++ ) {
                // each iteration takes 2 * 6 samples
                // we update _BlurRadius each time to cheaply get a very smooth look

                lrColorB = RenderTexture.GetTemporary (rtW, rtH, 0);
                Graphics.Blit (lrDepthBuffer, lrColorB, sunShaftsMaterial, 1);
                RenderTexture.ReleaseTemporary (lrDepthBuffer);
                ofs = sunShaftBlurRadius * (((it2 * 2.0f + 1.0f) * 6.0f)) / 768.0f;
                sunShaftsMaterial.SetVector ("_BlurRadius4", new Vector4 (ofs, ofs, 0.0f, 0.0f) );

                lrDepthBuffer = RenderTexture.GetTemporary (rtW, rtH, 0);
                Graphics.Blit (lrColorB, lrDepthBuffer, sunShaftsMaterial, 1);
                RenderTexture.ReleaseTemporary (lrColorB);
                ofs = sunShaftBlurRadius * (((it2 * 2.0f + 2.0f) * 6.0f)) / 768.0f;
                sunShaftsMaterial.SetVector ("_BlurRadius4", new Vector4 (ofs, ofs, 0.0f, 0.0f) );
            }

            // put together:

            //if (v.z >= 0.0f)
                sunShaftsMaterial.SetVector ("_SunColor", new Vector4 (sunColor.r, sunColor.g, sunColor.b, sunColor.a) * sunShaftIntensity*(1- fallOff));
            //else
            //    sunShaftsMaterial.SetVector ("_SunColor", Vector4.zero); // no backprojection !
            sunShaftsMaterial.SetTexture ("_ColorBuffer", lrDepthBuffer);
            Graphics.Blit (source, destination, sunShaftsMaterial, 0);

            RenderTexture.ReleaseTemporary (lrDepthBuffer);
            return true;
        }

        private float GetDistanceToOBB(Vector3 heroPos)
        {
            Vector3 v = heroPos - area_Center;
            float dist = 0;

            float excess = 0;
            float tempD = Vector3.Dot(area_u0, v);   //向量点乘单位向量等于到该方向的投影长度
            if (tempD > area_e0) excess = tempD - area_e0;
            if (tempD < -area_e0) excess = tempD + area_e0;	//Dot出来的可能是负数
            dist += excess * excess;

            tempD = Vector3.Dot(area_u1, v);   //向量点乘单位向量等于到该方向的投影长度
            if (tempD > area_e1) excess = tempD - area_e1;
            if (tempD < -area_e1) excess = tempD + area_e1;	//Dot出来的可能是负数
            dist += excess * excess;
            dist = Mathf.Sqrt(dist);
            return dist;
        }
    }
}
