using System;
using UnityEngine;

namespace ImageEffects
{
    [RequireComponent(typeof(Camera))]
    public class GlobalFog : ImageEffectNode
	{
        public Color fogColor = Color.gray;
        public float SkyBoxBlend = 0.0f;
        [Serializable]
        public class DistanceFogParm
        {
            public bool enabled = true;
            public FogMode fogMode = FogMode.Linear;
            public float startDistance = 10.0f;
            public float EndDistance = 100.0f;
            public float fogDenstiy = 0.05f;
            public bool useRadialDistance = false;
        }

        [Serializable]
        public class HeightFogParm
        {
            public bool enabled = false;
            public float height = 10.0f;
            public float heightDensity = 2.0f;
        }

        public DistanceFogParm distanceFogParam = new DistanceFogParm();

        public HeightFogParm heightFogParam = new HeightFogParm();


        private Shader fogShader = null;
        private Material fogMaterial = null;

        private const int validShaderLOD = 300;

        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }

        public override bool CheckResources ()
		{
            if (!fogShader)
            {
                fogShader = ImageEffectManager.API_GetShader("ImageEffect/GlobalFog");
            }
            CheckSupport (true);

            fogMaterial = CheckShaderAndCreateMaterial (fogShader, fogMaterial);

            if (!isSupported)
                ReportAutoDisable ();
            return isSupported;
        }

        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
		{
            if (CheckResources() == false || (!heightFogParam.enabled && !distanceFogParam.enabled) || !IsOnValidLOD())
            {
                return false;
            }
            Transform camtr = m_Camera.transform;
            float camNear = m_Camera.nearClipPlane;
            float camFar = m_Camera.farClipPlane;
            float camFov = m_Camera.fieldOfView;
            float camAspect = m_Camera.aspect;

            Matrix4x4 frustumCorners = Matrix4x4.identity;

            float fovWHalf = camFov * 0.5f;

            Vector3 toRight = camtr.right * camNear * Mathf.Tan(fovWHalf * Mathf.Deg2Rad) * camAspect;
            Vector3 toTop = camtr.up * camNear * Mathf.Tan(fovWHalf * Mathf.Deg2Rad);

            Vector3 topLeft = (camtr.forward * camNear - toRight + toTop);
            float camScale = topLeft.magnitude * camFar / camNear;

            topLeft.Normalize();
            topLeft *= camScale;

            Vector3 topRight = (camtr.forward * camNear + toRight + toTop);
            topRight.Normalize();
            topRight *= camScale;

            Vector3 bottomRight = (camtr.forward * camNear + toRight - toTop);
            bottomRight.Normalize();
            bottomRight *= camScale;

            Vector3 bottomLeft = (camtr.forward * camNear - toRight - toTop);
            bottomLeft.Normalize();
            bottomLeft *= camScale;

            frustumCorners.SetRow(0, topLeft);
            frustumCorners.SetRow(1, topRight);
            frustumCorners.SetRow(2, bottomRight);
            frustumCorners.SetRow(3, bottomLeft);

            var camPos = camtr.position;
            float FdotC = camPos.y - heightFogParam.height;
            float paramK = (FdotC <= 0.0f ? 1.0f : 0.0f);
            fogMaterial.SetMatrix("_FrustumCornersWS", frustumCorners);
            fogMaterial.SetVector("_CameraWS", camPos);
            fogMaterial.SetVector("_HeightParams", new Vector4(heightFogParam.height, FdotC, paramK, heightFogParam.heightDensity * 0.5f));
            fogMaterial.SetVector("_DistanceParams", new Vector4(-Mathf.Max(distanceFogParam.startDistance, 0.0f), 0.0f,1 - SkyBoxBlend, 0));
            fogMaterial.SetColor("_FogColor", fogColor);
            
            var sceneMode = distanceFogParam.fogMode;
            var sceneDensity = distanceFogParam.fogDenstiy;
            var sceneStart = distanceFogParam.startDistance;
            var sceneEnd = distanceFogParam.EndDistance;
            Vector4 sceneParams;
            bool linear = (sceneMode == FogMode.Linear);
            float diff = linear ? sceneEnd - sceneStart : 0.0f;
            float invDiff = Mathf.Abs(diff) > 0.0001f ? 1.0f / diff : 0.0f;
            sceneParams.x = sceneDensity * 1.2011224087f; // density / sqrt(ln(2)),  Exp2 fog mode
            sceneParams.y = sceneDensity * 1.4426950408f; // density / ln(2), Exp fog mode
            sceneParams.z = linear ? -invDiff : 0.0f;
            sceneParams.w = linear ? sceneEnd * invDiff : 0.0f;
            fogMaterial.SetVector("_SceneFogParams", sceneParams);
            fogMaterial.SetVector("_SceneFogMode", new Vector4((int)sceneMode, distanceFogParam.useRadialDistance ? 1 : 0, 0, 0));

            int pass = 0;
            if (distanceFogParam.enabled && heightFogParam.enabled)
                pass = 0; // distance + height
            else if (distanceFogParam.enabled)
                pass = 1; // distance only
            else
                pass = 2; // height only
            FogBilt(source, destination, fogMaterial, pass);
            return true;
        }

        static void FogBilt(RenderTexture source, RenderTexture dest, Material fxMaterial, int passNr)
        {
            RenderTexture.active = dest;

            fxMaterial.SetTexture("_MainTex", source);

            GL.PushMatrix();
            GL.LoadOrtho();

            fxMaterial.SetPass(passNr);

            GL.Begin(GL.QUADS);

            GL.MultiTexCoord2(0, 0.0f, 0.0f);
            GL.Vertex3(0.0f, 0.0f, 3.0f); // BL

            GL.MultiTexCoord2(0, 1.0f, 0.0f);
            GL.Vertex3(1.0f, 0.0f, 2.0f); // BR

            GL.MultiTexCoord2(0, 1.0f, 1.0f);
            GL.Vertex3(1.0f, 1.0f, 1.0f); // TR

            GL.MultiTexCoord2(0, 0.0f, 1.0f);
            GL.Vertex3(0.0f, 1.0f, 0.0f); // TL

            GL.End();
            GL.PopMatrix();
        }
    }
}
