using System;
using UnityEngine;

namespace ImageEffects
{


//[ExecuteInEditMode]
[RequireComponent (typeof(Camera))]
public class DepthOfFieldScatter : ImageEffectNode 
{	
    public bool ShowFocus  = false;
	public float focalLength  = 10.0f;
	public float focalSize  = 0.05f; 
	public float aperture  = 11.5f;
	public Transform focalTransform  = null;
	public float maxBlurSize  = 2.0f;
	
	
	public float foregroundOverlap  = 1.0f;
	
	private Material dofHdrMaterial  = null;

	private float focalDistance01  = 10.0f;	
	private float internalBlurWidth  = 1.0f;

    private int validShaderLOD = 300;

    public override bool IsOnValidLOD()
    {
        return ImageEffectManager.ImageEffectShaderLOD >= validShaderLOD;
    }

    public void OnDestroy()
    {
        if(dofHdrMaterial) DestroyImmediate(dofHdrMaterial);
		dofHdrMaterial = null;
    }

	public override bool CheckResources ()
    {
		CheckSupport (true); // only requires depth, not HDR
			
		dofHdrMaterial = CheckShaderAndCreateMaterial (ImageEffectManager.API_GetShader("ImageEffect/DepthOfField"), dofHdrMaterial); 

		if(!isSupported)
			ReportAutoDisable ();

		return isSupported;		  
	}

	public override void OnActive ()
    {
        API_AttachCameraRenderFlags(DepthTextureMode.Depth);
	}

    public override void OnDeActive()
	{
        DeAttachAllCameraRenderFlags();
		
	}


	float FocalDistance01 (float worldDist ) 
    {
		return m_Camera.WorldToViewportPoint((worldDist-m_Camera.nearClipPlane) * m_Camera.transform.forward + m_Camera.transform.position).z / (m_Camera.farClipPlane-m_Camera.nearClipPlane);	
	}

	private void WriteCoc (RenderTexture fromTo , bool fgDilate)
    {
		dofHdrMaterial.SetTexture("_FgOverlap", null); 
		// capture full coc in alpha channel (fromTo is not read, but bound to detect screen flip)
	    Graphics.Blit (fromTo, fromTo, dofHdrMaterial,  0);	
	}

	public override bool ImageEffect_RenderImage(RenderTexture source, RenderTexture destination, RenderBuffer depthBuffer)	
    {
        if (!ImageEffects.ImageEffectManager.AdvanceImangeEffectEnabled)
        {
            return false;
        }
		if(!CheckResources () || !IsOnValidLOD()) {
			return false; 
		}

		// clamp & prepare values so they make sense

		if (aperture < 0.0f) aperture = 0.0f;
		if (maxBlurSize < 0.1f) maxBlurSize = 0.1f;
		focalSize = Mathf.Clamp(focalSize, 0.0f, 2.0f);
		internalBlurWidth = Mathf.Max(maxBlurSize, 0.0f); 
					
		// focal & coc calculations

        ImageEffectManager result = null;
        ImageEffectManager.ImageEffectManagerTable.TryGetValue(m_Camera, out result);

        if (result != null)
        {
            if (result.hero != null)
            {
                focalTransform = result.hero.transform;
            }
        }
        focalDistance01 = (focalTransform) ? (m_Camera.WorldToViewportPoint(focalTransform.position)).z / (m_Camera.farClipPlane) : FocalDistance01(focalLength);
		dofHdrMaterial.SetVector ("_CurveParams", new Vector4 (1.0f, focalSize, aperture/10.0f, focalDistance01));
			
		if(ShowFocus) 
		{
			WriteCoc (source, true);
			Graphics.Blit (source, destination, dofHdrMaterial, 2);
		}		
		else 
		{ 		
			source.filterMode = FilterMode.Bilinear;
			WriteCoc (source, true);
			dofHdrMaterial.SetTexture("_FgOverlap", null);
            dofHdrMaterial.SetVector("_Offsets", Vector4.one * (2 * internalBlurWidth));
			Graphics.Blit (source, destination, dofHdrMaterial, 1);

		}
        return true;
	}	
}

}