using System;
using UnityEngine;


namespace ImageEffects
{
    [RequireComponent(typeof(Camera))]
    public class FXAA : ImageEffectNode
    {
        public Shader shader;
        private Material mat;
        private int validShaderLOD = 400;

        private bool isActived = false;

        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }

        Shader topNameShader = null;
        Material blendMat = null;
        Shader blendShader = null;
        Camera topNameCam = null;
        GameObject obj = null;
        bool updateCamera = true;
        public override bool CheckResources()
        {
            if (!shader)
            {
                shader = ImageEffectManager.API_GetShader("Hidden/FXAA Preset 2");
            }
            CheckSupport(true);

            if (mat == null)
                mat = CheckShaderAndCreateMaterial(shader, mat);

            if (!isSupported)
                ReportAutoDisable();

            //新的支持头顶文字不反锯齿
            if (topNameShader == null)
                topNameShader = ImageEffectManager.API_GetShader("Effect/BillBoradFXAA");

            if (blendShader == null)
            {
                blendShader = ImageEffectManager.API_GetShader("Hidden/BlendNameShader");
                blendMat = CheckShaderAndCreateMaterial(blendShader, blendMat);
            }

            if (topNameCam == null)
            {
                obj = new GameObject("CpyCam");
                topNameCam = obj.AddComponent<Camera>();
            }

            if (updateCamera)
            {
                obj.transform.position = gameObject.transform.position;
                obj.transform.rotation = gameObject.transform.rotation;

                topNameCam.CopyFrom(GetComponent<Camera>());
                topNameCam.renderingPath = RenderingPath.VertexLit;

                topNameCam.clearFlags = CameraClearFlags.Color;
                topNameCam.backgroundColor = new Color(0, 0, 0, 0);
                topNameCam.enabled = false;
                topNameCam.cullingMask = LayerMask.GetMask("TopName"); //1 << 21;;
            }

            if (topNameShader == null)
                Debug.Log("头顶文字Shader没有！！");
            //end新的支持头顶文字不反锯齿

            return isSupported;
        }

        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        {
            //ImageEffectManager.AdvanceImangeEffectEnabled的机制有问题，没法激活下面效果的Active和Deactive处理
            if (!ImageEffects.ImageEffectManager.AdvanceImangeEffectEnabled)
            {
                if (isActived)
                    OnDeActive();

                return false;
            }
            if (false == CheckResources() || !IsOnValidLOD())
            {
                return false;
            }

            if (isActived == false)
                OnActive();

            //float rcpWidth = 1.0f / Screen.width;
            //float rcpHeight = 1.0f / Screen.height;

            //mat.SetVector("_rcpFrame", new Vector4(rcpWidth, rcpHeight, 0, 0));
            //mat.SetVector("_rcpFrameOpt", new Vector4(rcpWidth * 2, rcpHeight * 2, rcpWidth * 0.5f, rcpHeight * 0.5f));

            source.anisoLevel = 4;
            Graphics.Blit(source, destination, mat);
            source.anisoLevel = 0;

            if (SystemInfo.supportedRenderTargetCount >= 2)
            {
                RenderTexture namesTex = RenderTexture.GetTemporary(source.width, source.height, 24, RenderTextureFormat.ARGB32);
                RenderTexture nameDepthTex = RenderTexture.GetTemporary(source.width, source.height, 24, RenderTextureFormat.RHalf);

                if (nameDepthTex != null && nameDepthTex != null)
                {
                    //topNameCam.targetTexture = namesTex;

                    RenderBuffer[] buffs = { namesTex.colorBuffer, nameDepthTex.colorBuffer };
                    topNameCam.SetTargetBuffers(buffs, namesTex.depthBuffer);

                    topNameCam.RenderWithShader(topNameShader, null);
                    topNameCam.targetTexture = null;

                    blendMat.SetTexture("_DepthTex", nameDepthTex);

                    Graphics.Blit(namesTex, destination, blendMat);
                    RenderTexture.ReleaseTemporary(namesTex);
                    RenderTexture.ReleaseTemporary(nameDepthTex);
                }
            }
            else
            {
                if (isActived)
                {
                    Camera cam = GetComponent<Camera>();
                    if (cam != null)
                    {
                        cam.cullingMask = (cam.cullingMask | (LayerMask.GetMask("TopName")));
                    }
                }
            }

            return true;
        }

        public override void OnActive()
        {
            isActived = true;
            Camera cam = GetComponent<Camera>();
            if (cam != null)
            {
                cam.cullingMask = (cam.cullingMask & ~(LayerMask.GetMask("TopName")));
            }
        }

        public override void OnDeActive()
        {
            isActived = false;
            Camera cam = GetComponent<Camera>();
            if (cam != null)
            {
                cam.cullingMask = (cam.cullingMask | (LayerMask.GetMask("TopName")));
            }
        }
    }



}