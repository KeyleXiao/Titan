/**********************************************************************

Filename    :   SFRTT.cs
Content     :   Inherits from MonoBehaviour
Created     :   
Authors     :   Ryan Holtz, Ankur Mohan

Copyright   :   Copyright 2012 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.
 
***********************************************************************/
using UnityEngine;
using System.Runtime.InteropServices;
using System;
using System.IO;
using System.Collections;


namespace Scaleform
{

/// <summary>
///  Attach a SFRTT derived class to a gameobject to enable render-to-texture functionality. 
/// </summary>
/* The render-to-texture technique can be used to render a flash movie to a render texture instead of the backbuffer. 
 * You can either attach a rendertexture created in Unity to the RenderTexture member, or have a rendertexture automatically created during the Start function according to the specified parameters. This rendertexture can then be applied to any object in the scene.
 * This class also handles hit testing by transforming mouse coordinates to the coordinate system of the gameobject. 
 */ 
public partial class SFRTT : MonoBehaviour
{
    // Publicly-accessible values (settable in the Unity editor)
	
    /// <summary>
    /// Name of the flash file to be used as the render-to-texture movie. 
    /// </summary>
    public string SwfName;

	    /// <summary>
    /// The class name of the Movie derived class. This is used to handle externalinterface callbacks implemented in the Movie derived class.If not MovieClassName is specified, a movie of type Movie will be instantiated. This movie will advance/display correctly, but will not respond to any externalinterface callbacks.
    /// </summary>
	public string MovieClassName;

	
    /// <summary>
    /// Color used to clear the RenderTexture before rendering the SWF.
    /// </summary>
    public Color32 ClearColor;
    
	/// <summary>
    /// RenderTexture Width. Note that RenderTextureWidth must be = RenderTextureHeight, otherwise render-to-texture doesn't work. This appears to be a Unity limitation.
    /// </summary>
	public int RenderTextureWidth;

	/// <summary>
    /// RenderTexture Height. Note that RenderTextureWidth must be = RenderTextureHeight, otherwise render-to-texture doesn't work. This appears to be a Unity limitation.
    /// </summary>
    public int RenderTextureHeight;
	
	
    /// <summary>
    /// RenderTexture OriginX.
    /// </summary>
	public int OriginX;

	 /// <summary>
    /// RenderTexture OriginY.
    /// </summary>
	public int OriginY;
    
	
    /// <summary>
    /// The RenderTexture. The user can either provide one created in Unity, or leave it null. If RenderTexture is null, one will be created during the Start function according to the specified parameters.
    /// </summary>
    public RenderTexture RenderTexture;

	
    /// <summary>
    /// Represents the RenderTexture movie.
    /// </summary>
    protected Movie RTTMovie;

    protected Collider TheCollider;

    /// <summary>
    /// In this function, a RenderTexture is created if not specified by the user and the renderer.material.maintexture of the gameobject is set to the RenderTexture.
    /// </summary>
    public virtual void Start()
    {
    	UnityEngine.Debug.Log("Start: Initializing RTT: " + SwfName);
        // The idea is to use the existing RTT if the user has already set one in the Unity editor. If not, we'll go ahead 
        // and create one.
        if (RenderTexture)
        {
		// Creating a RenderTexture in Unity doesn't automatically create the corresponding hw texture.
			if (!RenderTexture.IsCreated())
				RenderTexture.Create();
            RenderTextureHeight = RenderTexture.height;
            RenderTextureWidth  = RenderTexture.width;
        }
        if (RenderTextureWidth == 0 || RenderTextureHeight == 0)
        {
            UnityEngine.Debug.Log("RenderTexture width or height is zero. Can't instantiate RTT.");
            return;
        }

		if (RenderTextureWidth != RenderTextureHeight)
		{
			UnityEngine.Debug.Log("RenderTexture width must be the same as the height. This is a Unity limitation");
			RenderTexture = null;
			return;
		}
		
		if (OriginX < 0 || OriginY < 0 || OriginX > RenderTextureWidth || OriginY > RenderTextureHeight)
		{
			UnityEngine.Debug.Log("RenderTexture Origin coordinates must be set properly");
			RenderTexture = null;
			return;
		}
        if (!RenderTexture)
        {
            RenderTexture = new RenderTexture(RenderTextureWidth, RenderTextureHeight, 16, RenderTextureFormat.Default, RenderTextureReadWrite.Default);
            RenderTexture.Create();
            GetComponent<Renderer>().material.mainTexture = RenderTexture;
        }
        TheCollider = GetComponent("Collider") as Collider;
        UnityEngine.Debug.Log(TheCollider);
    }

    
    public virtual void OnDestroy()
    {
        GetComponent<Renderer>().material.mainTexture = null;
        RenderTexture = null;
        GameObject.Destroy(RTTMovie.gameObject);
        RTTMovie = null;
    }
   
	
    /// <summary>
    /// Device Reset is handled here. If the device was reset, the RenderTexture is recreated and applied to the movie. Must call base.Update() from any derived classes.
    /// </summary>
    public virtual void Update()
    {
#if UNITY_WP8
        if (sf_wasDeviceReset())
#else
        if (SF_WasDeviceReset())
#endif
        {
			RenderTexture.Create ();
			RTTMovie.ApplyRenderTexture (RenderTexture);
        }
    }
	
    /// <summary>
    /// Return the mesh collider component. Mesh Collider is used to transform screenspace mouse coordinates to texture space.
    /// </summary>
    /// <returns></returns>
    public Collider GetCollider()
    {
        return TheCollider;
    }

	
    /// <summary>
    /// The RenderTexture movie is created here. The movie is of movieClassType if one is specified, otherwise of type Movie.
    /// </summary>
    /// <param name="camera"></param>
    /// <param name="movieClassType"></param>
    /// <returns></returns>
    public bool CreateRenderMovie(SFCamera camera, Type movieClassType)
    {
#if (UNITY_3_5) && UNITY_STANDALONE_WIN
		if (SystemInfo.graphicsDeviceVersion[0] == 'D')
		{
			return false;
		}
#endif
        if (RenderTexture == null)
        {
            UnityEngine.Debug.Log("RenderTexture is null, failure to create RenderMovie. Check if you specified all the parameters properly.");
            return false;
        }
        if (RTTMovie == null || RTTMovie.MovieID == 0)
        {
            if (camera)
            {
                SFMovieCreationParams creationParams = SFCamera.CreateRTTMovieCreationParams(SwfName, 1, OriginX, OriginY, RenderTexture, ClearColor);
                creationParams.TheScaleModeType      = ScaleModeType.SM_ExactFit;
                creationParams.IsInitFirstFrame      = false;
                creationParams.IsAutoManageViewport  = false;
                // Has the user specified a movie subclass?
                if (movieClassType != null)
                {
                    SFManager sfMgr = camera.GetSFManager();
                    RTTMovie = Activator.CreateInstance(movieClassType, sfMgr, creationParams) as Movie;
                    if (RTTMovie != null)
                    {
                        RTTMovie.SetRTT(this);
                    }
                    return true;
                }
            }
        }
        return false;
    }
}

} // namespace Scaleform