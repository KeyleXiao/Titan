using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using UnityEngine;
using UnityEngine.Rendering;

namespace ImageEffects
{

    public enum EdgeDetectMode
    {
        TriangleDepthNormals = 0,
        RobertsCrossDepthNormals = 1,
        SobelDepth = 2,
        SobelDepthThin = 3,
        TriangleLuminance = 4,

        Invalid
    }

    [RequireComponent(typeof(Camera))]
    public class EdgeDetectEffectNormals : ImageEffectNode 
    {	

	    public EdgeDetectMode mode  = EdgeDetectMode.SobelDepthThin;
	    public float sensitivityDepth  = 1.0f;
	    public float sensitivityNormals  = 1.0f;
	    public float lumThreshhold  = 0.2f;
	    public float edgeExp  = 1.0f;
	    public float sampleDist  = 1.0f;
	    public float edgesOnly  = 0.0f;
	    public Color edgesOnlyBgColor  = Color.white;

        private Shader shader = null;
	    private Material edgeDetectMaterial  = null;
        private EdgeDetectMode oldMode = EdgeDetectMode.Invalid;

        private ImageEffectManager.CameraRenderFlagsNode flagReference = null;

        private int validShaderLOD = 400;

        public override bool IsOnValidLOD()
        {
            return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
        }

        public override bool CheckResources ()
        {
            if (!shader)
            {
                shader = ImageEffectManager.API_GetShader("Hidden/EdgeDetect");
            }

		    CheckSupport (true);

            edgeDetectMaterial = CheckShaderAndCreateMaterial(shader, edgeDetectMaterial);
		    SetCameraFlag ();

		    if (!isSupported)
			    ReportAutoDisable ();
		    return isSupported;				
	    }

         public override void Awake()
        {
            base.Awake();
	    }

	    void SetCameraFlag ()
        {
            if (mode == oldMode)
            {
                return;
            }

            if (oldMode == EdgeDetectMode.SobelDepth || oldMode == EdgeDetectMode.SobelDepthThin)
            {
                ImageEffectManager.API_DeAttachCameraRenderFlags(m_Camera,ref flagReference);
            }
            else if (oldMode == EdgeDetectMode.TriangleDepthNormals || oldMode == EdgeDetectMode.RobertsCrossDepthNormals)
            {
                ImageEffectManager.API_DeAttachCameraRenderFlags(m_Camera, ref flagReference);
            }	


            if (mode == EdgeDetectMode.SobelDepth || mode == EdgeDetectMode.SobelDepthThin)
            {
                flagReference = ImageEffectManager.API_AttachCameraRenderFlags(m_Camera, DepthTextureMode.Depth);
            }
            else if (mode == EdgeDetectMode.TriangleDepthNormals || mode == EdgeDetectMode.RobertsCrossDepthNormals)
            {
                flagReference = ImageEffectManager.API_AttachCameraRenderFlags(m_Camera, DepthTextureMode.DepthNormals);
            }

            oldMode = mode;
	    }

	    public override void OnActive()
        {

            if (mode == EdgeDetectMode.SobelDepth || mode == EdgeDetectMode.SobelDepthThin)
            {
               flagReference = ImageEffectManager.API_AttachCameraRenderFlags(m_Camera, DepthTextureMode.Depth);
            }
            else if (mode == EdgeDetectMode.TriangleDepthNormals || mode == EdgeDetectMode.RobertsCrossDepthNormals)
            {
                flagReference = ImageEffectManager.API_AttachCameraRenderFlags(m_Camera, DepthTextureMode.DepthNormals);
            }	
          
	    }
	    
        public override void OnDeActive()
        {
            if (mode == EdgeDetectMode.SobelDepth || mode == EdgeDetectMode.SobelDepthThin)
            {
               ImageEffectManager.API_DeAttachCameraRenderFlags(m_Camera,ref flagReference);
            }
            else if (mode == EdgeDetectMode.TriangleDepthNormals || mode == EdgeDetectMode.RobertsCrossDepthNormals)
            {
               ImageEffectManager.API_DeAttachCameraRenderFlags(m_Camera,ref flagReference);
            }
        }

        public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)
        {
            if (!ImageEffects.ImageEffectManager.AdvanceImangeEffectEnabled)
            {
                return false;
            }
		    if (CheckResources () == false) {
			    
			    return false;
		    }
				
		    Vector2 sensitivity  = new Vector2 (sensitivityDepth, sensitivityNormals);		
		    edgeDetectMaterial.SetVector ("_Sensitivity", new Vector4 (sensitivity.x, sensitivity.y, 1.0f, sensitivity.y));		
		    edgeDetectMaterial.SetFloat ("_BgFade", edgesOnly);	
		    edgeDetectMaterial.SetFloat ("_SampleDistance", sampleDist);		
		    edgeDetectMaterial.SetVector ("_BgColor", edgesOnlyBgColor);	
		    edgeDetectMaterial.SetFloat ("_Exponent", edgeExp);
		    edgeDetectMaterial.SetFloat ("_Threshold", lumThreshhold);
		
		    Graphics.Blit (source, destination, edgeDetectMaterial, (int)mode);
            return true;
	    }
    }


}
