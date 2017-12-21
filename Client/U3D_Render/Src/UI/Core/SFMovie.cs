/**********************************************************************

Filename    :	SFMovie.cs
Content     :	Movie Wrapper
Created     :   
Authors     :   Ankur Mohan

Copyright   :   Copyright 2012 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.
 
***********************************************************************/

using UnityEngine;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Scaleform
{

[StructLayout(LayoutKind.Explicit,CharSet=CharSet.Ansi)]
class test
{
    public test(int _a, int _b)
    {a = _a; b = _b;
    }
    [FieldOffset(0), MarshalAs( UnmanagedType.I4)] public int a;
    [FieldOffset(4), MarshalAs( UnmanagedType.I4)]public int b;
}

/// <summary>
/// ActionScript Version
/// </summary>
public enum AVMVersion: int
{
	Error	= 0,
	AS2		= 1,
	AS3		= 2
}
    /// <summary>
/// The type of hit testing performed by the HitTest function.
    /// </summary>
public enum HitTestType : int
{
    HitTest_Bounds = 0,			//Hit testing will be done based on movie clip bounding boxes.
    HitTest_Shapes = 1,			//Hit testing will be done based on shape geometry, returning true if the shape is hit. 
    HitTest_ButtonEvents = 2,	//Hit testing will be similar to internal logic used for movie clips with button event handlers. A shape will be hit-tested as true only if it belongs to a button or a movie clip with button state event handlers.
    HitTest_ShapesNoInvisible = 3 //Hit testing will be similar to one with HitTest_Shapes flag, the only difference is HitTest will ignore currently invisible shapes. 
};
		
/// <summary>
/// Viewport for movie rendering.
/// </summary>
[StructLayout(LayoutKind.Sequential)]
public class SFViewport
{
    public int OX;		//Origin X ccordinate of viewport.
    public int OY;		//Origin Y coordiante of viewport.
    public int Width;	//Height of the viewport.
    public int Height;	//Weight of the viewport.	
}
 
/// <summary>
/// The Movie class corresponds to the C++ Scaleform movie and provides methods to interact with the Scaleform movie. Lifetime of the underlying C++ movie is controled by the C# movie. The C++ movie is automatically destroyed when the C# movie is garbage collected
/// </summary>

public partial class Movie : UnityEngine.MonoBehaviour
{
	/// <summary>
	/// String name corresponding to the movie.
	/// </summary>
    public	String      MovieName;
	/// <summary>
	/// The C++ movie pointer cast as an integer.
	/// </summary>
    public	long        MovieID;
	/// <summary>
	/// Scaleform Manager
	/// </summary>
	public SFManager	SFMgr;
	/// <summary>
	/// Is the movie focused or not. 
	/// </summary>
	public bool			IsFocused;
	/// <summary>
	/// Should the movie be advanced when the game is paused. 
	/// </summary>
	public bool			AdvanceWhenGamePaused;
	/// <summary>
	/// If true, the viewport will automatically be obtained from the device. If not, the viewport provided during movie creation will be used instead. Typically, setting this to true is sufficient. This way if the viewport is resized (in editor or windowed mode), the movie viewport will readjust accordingly. For render-to-texture movies, the viewport provided during movie creation is always used. 
	/// </summary>
	public bool			IsAutoManageViewport;
	/// <summary>
	/// Used for creating movies out of a memory buffer. Unity API functions can be used to read the swf/gfx file from the disc into a memory buffer.
	/// </summary>
	public IntPtr		pDataUnmanaged;
	/// <summary>
	/// Scalemode. Refer to Flash documentation for details.
	/// </summary>
	public ScaleModeType TheScaleModeType;
	/// <summary>
	/// Used for render-to-texture movies. Points to the SFRTT class that created this movie.
	/// </summary>
	public SFRTT		mRenderTexture;
	/// <summary>
	/// Specifies the rendering order of movies. Movies with a lower depth are rendered first.
	/// </summary>
	public int			Depth; 		

	/// <summary>
	/// Movie Definition. Can be used to get SWFVersion, Movie Width/Height etc. See MovieDef class for details
	/// </summary>
	public MovieDef		MovieDef;
    private	Boolean     MarkForRelease;
    
	private	SFViewport	ViewPort = new SFViewport();

	/// <summary>
	/// Used to notify native code that a movie is about to be created or destroyed. If a movie is about to be destroyed, no point			advancing it.
	/// </summary>
	public enum MovieLifeCycleEvents : int
	{
		Movie_Created = 0,
		Movie_Destroyed,
	}

	/// <summary>
	/// Default constructor.
	/// </summary>
		
    public Movie()
    {
        MovieID = 0;
		Depth	= 0;
    }

    private static Dictionary<System.Type, Movie> _singleton = new Dictionary<System.Type, Movie>();

    public static T GetSingleton<T>() where T : Movie
    {
        System.Type t = typeof(T);
        if (_singleton.ContainsKey(t))
        {
            Movie movie = _singleton[t];
            return movie as T;
        }
        else
        {
            return null;
        }
    } 

			
	/// <summary>
	/// Movie constructor, moviedef is created automatically. 
	/// </summary>
	/// <param name="sfmgr">Scaleform manager.</param>
	/// <param name="creationParams">Movie creation parameters.</param>
    public virtual void Init(SFGFxMovieManager _parent, SFManager sfmgr, SFMovieCreationParams creationParams)
	{
        //Debug.Log(creationParams.MovieName);

		if(sfmgr == null)
		{
			MovieID = 0;
			return;
		}
				
		//1. Set parameters like name and viewport
    	MovieName               = creationParams.MovieName;
    	ViewPort.OX             = creationParams.OX;
    	ViewPort.OY             = creationParams.OY;
    	ViewPort.Width          = creationParams.Width;
    	ViewPort.Height         = creationParams.Height;
		pDataUnmanaged			= creationParams.pData;
		Depth					= creationParams.Depth; // Rendering order. Movie with Depth + 1 will render on top of Movie with Depth
    	MovieID                 = 0; // Assigned when the C++ Movie is created. 
    	MarkForRelease          = false;
    	SFMgr                   = sfmgr;
    	IsFocused               = false;
    	AdvanceWhenGamePaused   = false;
    	IsAutoManageViewport    = creationParams.IsAutoManageViewport;
		TheScaleModeType		= creationParams.TheScaleModeType;

		int cpSize = Marshal.SizeOf(typeof(SFMovieCreationParams));
		IntPtr pCreationParams = Marshal.AllocCoTaskMem(cpSize);
		Marshal.StructureToPtr(creationParams, pCreationParams, false);	
		MovieID = SF_CreateMovie(pCreationParams);
		
		if(MovieID == 0)
		{
			return;
		}

		//3. Add into SFManager
		SFMgr.AddMovie(this);
        _singleton[this.GetType()] = this;
	}


	/// <summary>
	/// We list all the cases that can arise during movie destruction. 
	/// 1. Editor Mode:
	///		1.a: The user creates and destroys movies as the game simulation is running. A movie can be destroyed by calling Destroy() on the movie derived class. This removes the movie from the list of movies maintained by the manager, so that there are no more references pointing to the movie. It also instructs the runtime to not advance/display the movie anymore. The actual C++ movie is destroyed when the movie is garbage collected. However, this destruction also doesn't take place immediately. When the movie destructor is called, the movie is put on a release list instead of being destroyed immediately. This is because the garbage collector is run on a different thread and movie destruction should take place on the advance thread. The release list is serviced during game update, which is what drives advance. Also see additional note below. If you want the garbage collection cycle to run immediately, call GC.Collect(). 
	///		1.b: The user creates a number of movies during the game simulation, and then stops the game simulation by pushing the play button in the editor. In this case, the game object to which the manager is attached gets destroyed. However the manager can get destroyed before the movies so and movies don't get a chance to put themselves on the release queue which could lead to memory leaks. To address this situation, we call SFMgr.ForceDestroyMovies() in SFCamera.OnDestroy(), which informs C++ that the movies should be destroyed. Since the movies can hold rendering resources, the actual destruction of the C++ movie could need the render thread to tick a couple of times, hence you might see the movies still being held in memory even after pausing the game simulation. These movies will get destroyed when you run the game simulation again or close the editor. In case of render-texture movies, you might see refCount == (rs->m_Texture ? 1 : 0) messages. This is because the underlying C++ movie keeps a reference to the render texture and may not be destroyed rightaway when the simulation is stopped. These movies will be destroyed when the simulation is run again or if the editor window is closed.
	///		1.c: The user destroys all movies when the game simulation is running. This can be achieved by calling SFManager.DoCleanup();
	///		
	///2. Standalone application
	///2.a same as 1.a above
	///2.b In standalone mode, there is no notion of starting and stopping the game simulation and the destruction of movies is handled internally when the scaleform runtime is destroyed.

	/// Movie destructor. It is important to note the following threading issue: We can't destroy the underlying C++ Movie object in		finalize method, because this Finalize is called from the .Net garbage collector thread, and we can only destroy C++ Movie objects		from the main thread. Therefore, we inform the SFManager that this movie is to be deleted, and it's put on a release queue. This		queue is cleared (see ReleaseMoviesMarkedForRelease method on the SFManager) during the update function, which is invoked on the main	thread. 
	/// </summary>
    public virtual void OnDestroy()
    {
        Trace.Log("SFMovieEx Begin Destroy:" + MovieName);
        if (MovieID != 0)
        {
            _singleton.Remove(this.GetType());
            SFMgr.RemoveMovie(this);
#if UNITY_WP8
			sf_notifyNativeManager(movie.MovieID, MovieLifeCycleEvents.Movie_Destroyed);
#else
            SF_NotifyNativeManager(MovieID, MovieLifeCycleEvents.Movie_Destroyed);
#endif
            MarkForRelease = true;
            SF_DestroyMovie(MovieID);
            MovieID = 0;
            if (pDataUnmanaged != IntPtr.Zero)
            {
                Marshal.FreeCoTaskMem(pDataUnmanaged);
                pDataUnmanaged = IntPtr.Zero;
            }

        }

        SFMgr = null;

        Trace.Log("SFMovieEx End Destroy:" + MovieName);
    }
			
		


	/// <summary>
	/// Creates a Value out of a string.
	/// </summary>
	/// <param name="t"> String. </param>
	/// <returns>Created Value.</returns>
	public Value CreateValue(String t)
	{
		return new Value(t, MovieID);
	}

	/// <summary>
	/// Creates a Value from a Boolean.
	/// </summary>
	/// <param name="t"> Boolean. </param>
	/// <returns>Created Value.</returns>
	public Value CreateValue(Boolean t)
	{
		return new Value(t, MovieID);
	}

	/// <summary>
	/// Creates a Value from a Double.
	/// </summary>
	/// <param name="t"> Double. </param>
	/// <returns>Created Value.</returns>
	public Value CreateValue(Double t)
	{
		return new Value(t, MovieID);
	}

	/// <summary>
	/// Creates a Value from a Unsigned Int.
	/// </summary>
	/// <param name="t"> Unsigned Int </param>
	/// <returns>Created Value</returns>
	public Value CreateValue(UInt32 t)
	{
		return new Value(t, MovieID);
	}
	/// <summary>
	/// Creates a Value from a Signed Int.
	/// </summary>
	/// <param name="t"> Signed Int. </param>
	/// <returns>Created Value.</returns>Signed Int
	public Value CreateValue(Int32 t)
	{
		return new Value(t, MovieID);
	}

	/// <summary>
	/// Creates a Value from an Object.
	/// </summary>
	/// <param name="t"> Object.</param>
	/// <returns>Created Value.</returns>		
	public Value CreateValue(object o)
	{
		if(o is int)
		{
			return new Value((int)o, MovieID);	
		}
		else if(o is float)
		{
			//Error: float => object X=> Double. 
			//Good: float => object => Single => Double
			Single s = (Single)o;
			return new Value((Double)s, MovieID);			
		}
		else if(o is double)
		{
			return new Value((Double)o, MovieID);		
		}
		else if(o is String)
		{
			return new Value((String)o, MovieID);			
		}
		else if(o is bool)
		{
			return new Value((bool)o, MovieID);			
		}
				
		return new Value(0, MovieID);
	}

	/// <summary>
	/// Returns the movie id, an integer representation of the pointer to the C++ Movie.
	/// </summary>
	/// <returns>Movie id.</returns>	
    public long GetID() 
    { 
        return MovieID; 
    }
    
	/// <summary>
	/// Sets the movie id.
	/// </summary>
	/// <param name="id">Movie id.</param>
    public void SetID(long id)
    {
        MovieID = id; 
    }
    
	/// <summary>
	/// Used to notify the movie of the SFManager's demise.
	/// </summary>
    public void OnSFManagerDied()
    {
        // The SFManager just died, set our reference to null. 
        SFMgr = null;
    }
			
	/// <summary>
	/// Gets the flash resource located at varPath wrapped in the Value v.  
	/// </summary>
	/// <param name="v">Wrapper for the flash resource, use Value API to interact with the resource.</param>
	/// <param name="varPath">Resource path, must be relative to the flash movie root.</param>
	/// <returns>true if the resource was found, false otherwise.</returns>
	public bool GetVariable(out Value v, String varPath)
	{
		Value result = new Value();
#if UNITY_WP8
		if(sf_getVariable(MovieID, result, varPath))
#else
		if(SF_GetVariable(MovieID, result, varPath))
#endif
		{
			v = result;
			return true;
		}
		
		v = null;
		return false;		
	}
	
	/// <summary>
	/// SetVariable Type.
	/// </summary>
		
    public enum SetVarType : int
    {
		/// <summary>
		/// Sets variable only if target clip is found.
		/// </summary>
        SV_Normal = 0,    
		/// <summary>
		/// Sets variable if target clip is found, otherwise queues a setvariable call until the clip is created at path.
		/// </summary>
        SV_Sticky,     
        /// <summary>
		/// Sets variable applied to this and all future clips at given path.
        /// </summary>
        SV_Permanent  
    };
	
	/// <summary>
	/// Sets the variable at path varPath to value v according to SetVarType (see SetVarType enum for details). This can be used to set the value of text fields, variables, and properties of named nested characters within the movie.
	/// </summary>
	/// <param name="varPath">path of the variable to be set.</param>
	/// <param name="v">Value wrapper for the source.</param>
	/// <param name="type">Type of the SetVariable, see SetVarType enum doc for more info.</param>
		
	public void SetVariable(String varPath, Value v, SetVarType type)
	{
#if UNITY_WP8
		sf_setVariable(MovieID, varPath, v, type);
#else
		SF_SetVariable(MovieID, varPath, v, type);
#endif
	}

    /// <summary>
    /// Pauses/UnPauses a movie.
    /// </summary>
    /// <param name="paused">A flag to determine whether to pause or not.</param>
    public void SetPaused(bool paused)
    {
        SF_SetPaused(MovieID, paused);
    }

    /// <summary>
    /// Indicates whether the movie is paused or not.
    /// </summary>
    /// <returns>True if movie is paused.</returns>
    public bool IsPaused()
    {
        return SF_IsPaused(MovieID);
	}
	
	/// <summary>
	/// Returns the MovieDef. NOTE: It's up to the caller to ensure the MovieID is still valid otherwise this may cause a crash.
	/// </summary>
	/// <returns>MovieDef</returns>
	public MovieDef GetMovieDef()
	{
		if (MovieDef == null)
		{
			MovieDef = new MovieDef(SF_GetMovieDef(MovieID));
		}
		return MovieDef;
	}
	
	/// <summary>
	/// Returns the viewport as calculated by internal C++ code.
	/// </summary>
	/// <param name="vp">stores the internal viewport.</param>
	/// <returns>True if viewport is successfully returned, false otherwise.</returns>
		
	public bool GetViewport(ref SFViewport vp)
    {
		SFViewport result = new SFViewport();
#if UNITY_WP8
		if(sf_getViewport(MovieID, result))
#else
		if(SF_GetViewport(MovieID, result))
#endif
		{
			ViewPort = result;
			vp = ViewPort;
			return true;
		}

		return false;
    }
	
	/// <summary>
	/// Sets the movie viewport.
	/// </summary>
	/// <param name="ox">OriginX.</param>
	/// <param name="oy">OriginY.</param>
	/// <param name="width">Width of the viewport.</param>
	/// <param name="height">Height of the viewport.</param>
		
    public void SetViewport(int ox, int oy, int width, int height)
    {
        ViewPort.OX = ox; ViewPort.OY = oy; ViewPort.Width = width;
        ViewPort.Height = height;
#if UNITY_WP8
		sf_setViewport(MovieID, ViewPort);
#else
		SF_SetViewport(MovieID, ViewPort);
#endif
    }

	/// <summary>
	/// Returns the movie depth.
	/// </summary>
	/// <returns> Movie depth (rendering order).</returns>
	public int GetDepth()
	{
		return Depth;
	}

	/// <summary>
	/// Sets the movie depth. Takes care of setting the depth for the underlying C++ movie.
	/// </summary>
	/// <param name="depth">The movie depth.</param>
	public void SetDepth(int depth)
	{
		Depth = depth;
#if UNITY_WP8
		sf_setDepth(MovieID, Depth);
#else
		SF_SetDepth(MovieID, Depth);
#endif
		// Now reorder the movie list
		SFMgr.ReorderMovies();
	}
	
	/// <summary>
	/// Swap depths.
	/// </summary>
	/// <param name="mv">Movie to swap depths with.</param>
	public void SwapDepths(Movie mv)
	{
		int tmp		= Depth;
		SetDepth(mv.Depth);
		mv.SetDepth(tmp);
	}

	/// <summary>
	/// Typically overridden in the derived class for movie specific update actions.
	/// </summary>
    public virtual void Update()
    {
        // Override in derived class for movie specific update actions
    }

	/// <summary>
	/// Returns the ActionScript version (AS2/AS3). 1= AS2, 2= AS3
	/// </summary>
	/// <returns> ActionScript Version </returns>
	public AVMVersion GetAVMVersion()
	{
		if (MovieID == 0) return (AVMVersion)(0);
		return (AVMVersion)SF_GetAVMVersion(MovieID);
	}

	/// <summary>
	/// Movie advance.
	/// </summary>
	/// <param name="deltaTime">Time elapsed since the last time Advance was called. If deltaTime isgreaterthan 1/MovieFrameRate, playhead will move to the next frame.</param>
    public virtual  void Advance(float deltaTime)
    {
        if (MovieID == 0) return;
        if (!MarkForRelease)
        {
			int errCode; 
            if (AdvanceWhenGamePaused)
            {
                // Advance the movie automatically.
#if UNITY_WP8
               errCode = sf_advance(MovieID, 0, true);
#else
               errCode = SF_Advance(MovieID, 0, true);
#endif
            }
            else
            {
#if UNITY_WP8
				errCode = sf_advance(MovieID, deltaTime);
#else
                errCode = SF_Advance(MovieID, deltaTime);
#endif
            }
			if ((SFUnityErrorCodes)(errCode) == SFUnityErrorCodes.IncompatibleRenderer)
			{
				UnityEngine.Debug.Log("Unity and Scaleform Plug-in are using incompatible renderer type");
			}
        }
    }

	/// <summary>
	/// Checks if the (x,y) lies on this movie, according to HitTestType. Transforms (x,y) to render texture space if the movie is a render-to-texture. 
	/// The origin is in the lower left (0,0) and the upper right is (Screen.width, Screen.height).
	/// </summary>
	/// <param name="x">x coordinate.</param>
	/// <param name="y">y coordinate.</param>
	/// <param name="hitTestType">See doc for HitTestType enum for details.</param>
	/// <returns>True if hit test succeeded, false otherwise.</returns>
    public bool DoHitTest(float x, float y, HitTestType hitTestType)
    {
        if (MovieID == 0)
        {
            return false;
        }
		float xx = x; float yy = y;

        if (mRenderTexture)
        {
            RaycastHit hit;
            if (Initialize.mainCam!=null && Physics.Raycast(Initialize.mainCam.ScreenPointToRay(new Vector2(xx, yy)), out hit))
            {
                Renderer hitRenderer = hit.collider.GetComponent<Renderer>();
                MeshCollider meshCollider = hit.collider as MeshCollider;
                if (!(hit.collider is MeshCollider) || hitRenderer == null || 
					    meshCollider == null || this.mRenderTexture.GetComponent<Collider>() != meshCollider)
                {
                    return false;
                }
					 
				float rtX = (mRenderTexture.RenderTextureWidth * hit.textureCoord.x);
				float rtY = (mRenderTexture.RenderTextureHeight * hit.textureCoord.y);
				return SF_DoHitTest(MovieID, rtX, rtY, (int)hitTestType);
            }
            return false;
        }
		else
		{
			GetViewport(ref ViewPort);
			
			// remap the coordinates to have the origin in the upper left
			yy = Screen.height - yy;
			
			// Adjust according to viewport
			xx -= ViewPort.OX;
			yy -= ViewPort.OY;
		}

#if UNITY_WP8
        return sf_doHitTest(MovieID, xx, yy, (int)hitTestType);
#else
        return SF_DoHitTest(MovieID, xx, yy, (int)hitTestType);
#endif
    }


    /// <summary>
    /// Checks if the (x,y) lies on this movie, according to HitTestType. Transforms (x,y) to render texture space if the movie is a render-to-texture. 
    /// The origin is in the lower left (0,0) and the upper right is (Screen.width, Screen.height).
    /// </summary>
    /// <param name="x">scaleform x coordinate.</param>
    /// <param name="y">scaleform y coordinate.</param>
    /// <param name="hitTestType">See doc for HitTestType enum for details.</param>
    /// <returns>True if hit test succeeded, false otherwise.</returns>
    public bool DoHitTestEx(float x, float y, HitTestType hitTestType)
    {
        if (MovieID == 0)
        {
            return false;
        }
        float xx = x; float yy = y;

        if (mRenderTexture)
        {
            RaycastHit hit;
            if (Initialize.mainCam != null && Physics.Raycast(Initialize.mainCam.ScreenPointToRay(new Vector2(xx, Screen.height - yy)), out hit))
            {
                Renderer hitRenderer = hit.collider.GetComponent<Renderer>();
                MeshCollider meshCollider = hit.collider as MeshCollider;
                if (!(hit.collider is MeshCollider) || hitRenderer == null ||
                        meshCollider == null || this.mRenderTexture.GetComponent<Collider>() != meshCollider)
                {
                    return false;
                }

                float rtX = (mRenderTexture.RenderTextureWidth * hit.textureCoord.x);
                float rtY = (mRenderTexture.RenderTextureHeight * hit.textureCoord.y);
                return SF_DoHitTest(MovieID, rtX, rtY, (int)hitTestType);
            }
            return false;
        }
        else
        {
            GetViewport(ref ViewPort);

            // remap the coordinates to have the origin in the upper left

            // Adjust according to viewport
#if UNITY_EDITOR
            xx -= ViewPort.OX;
            yy -= ViewPort.OY ;// HACK, This value is from the real viewport(bigger than game screen) and the tool bar on top.
#else
			xx -= ViewPort.OX;
            yy -= ViewPort.OY;
#endif
        }

#if UNITY_WP8
        return sf_doHitTest(MovieID, xx, yy, (int)hitTestType);
#else
        return SF_DoHitTest(MovieID, xx, yy, (int)hitTestType);
#endif
    }

	/// <summary>
	/// Sets focus.
	/// </summary>
	/// <param name="focus">focus.</param>
    public void SetFocus(bool focus)
    {
        if (MovieID == 0)
        {
            return;
        }
        IsFocused = focus;
#if UNITY_WP8
        sf_setFocus(MovieID, focus);
#else
        SF_SetFocus(MovieID, focus);
#endif
    }

	/// <summary>
	/// Used for render-to-texture movies. Notifies the movie about the SFRTT derived class that governs it. This class is typically attached to a gameobject by the user. See the render-to-texture demo for setup info. 
	/// </summary>
	/// <param name="rtt">SFRTT derived class.</param>
    public void SetRTT(SFRTT rtt)
    {
        mRenderTexture = rtt;
    }

	/// <summary>
	/// Returns the associated SFRTT derived class.
	/// </summary>
	/// <returns></returns>
    public SFRTT GetRTT() { return mRenderTexture; }
	
	/// <summary>
	/// Used to determine if the movie is a render-to-texture movie.
	/// </summary>
	/// <returns>True if movie is a render-to-texture movie, false otherwise.</returns>
    public bool IsRTTMovie()
    {
        return mRenderTexture == null ? false : true;
    }
			
    /// <summary>
    /// Set a render texture for this movie. This can be used to override an existing render texture.
    /// </summary>
    /// <param name="rtt">RenderTexture to apply to the movie</param>
	public void ApplyRenderTexture(RenderTexture rtt)
	{
#if !(UNITY_3_5)
        if (SystemInfo.graphicsDeviceVersion[0] == 'D')
        {
#if UNITY_WP8
			sf_applyRenderTexture (MovieID, (long)rtt.GetNativeTexturePtr());
#else
            if (!rtt)
            {
                SF_ApplyRenderTexture(MovieID, 0);
            }
            else
            {
                SF_ApplyRenderTexture(MovieID, (long)rtt.GetNativeTexturePtr());
            }
			
#endif
		}
#endif
	}
	
	/// <summary>
	/// Can be used to control if the movie responds to key events. If you don't want the movie to respond to keyevents, override this function in the derived class and return false.
	/// </summary>
	/// <returns>True if the movie responds to keyevents, false otherwise.</returns>
    public virtual bool AcceptKeyEvents()
    {
        // Can check for various conditions here. Derived classes can override this function as well.
        return IsFocused;
    }

	/// <summary>
	/// Can be used to control if the movie responds to char events. If you don't want the movie to respond to char events, override this function in the derived class and return false.
	/// </summary>
	/// <returns>True if the movie responds to char events, false otherwise.</returns>
    public virtual bool AcceptCharEvents()
    {
        // Can check for various conditions here. Derived classes can override this function as well
        return IsFocused;
    }

	/// <summary>
	/// Can be used to control if the movie responds to mouse events. If you don't want the movie to respond to mouse events, override this function in the derived class and return false.
	/// </summary>
	/// <returns>True if the movie responds to mouse events, false otherwise.</returns>
    public virtual bool AcceptMouseEvents(int icase)
    {
        // Can check for various conditions here. Derived classes can override this function as well
        // Check if the mouse event is over the movie viewport.
        return true;
    }

	/// <summary>
	/// Can be used to control if the movie responds to touch events. If you don't want the movie to respond to touch events, override this function in the derived class and return false.
	/// </summary>
	/// <returns>True if the movie responds to touch events, false otherwise.</returns>
	public virtual bool AcceptTouchEvents()
	{
		// Can check for various conditions here. Derived classes can override this function as well
		// Check if the mouse event is over the movie viewport.
		return true;
	}
    /// <summary>
    /// Passes mouse events to Scaleform runtime for processing. The function takes care of adjusting the screen space mouse coordinates according to the movie viewport. If the movie is a render-to-texture movie, the function converts screen space coordinates to texture space
    /// </summary>
    /// <param name="x"> x mouse position.</param>
    /// <param name="y"> y mouse position.</param>
    /// <param name="icase"> Indicates whether the mouse event is mouse down/up/move.</param>
    /// <param name="buttonType"> Indicates left/middle/right mouse button.</param>
    /// <param name="scrollDelta"> Mouse wheel delta (delta > 0 scroll away from the user; delta < 0 scroll towards the user)</param>
    /// <param name="breturnOnHandled">In general, mouse events are sent to all movies. Set this flag to true if you don't want other movies to handle mouse events.</param>
    /// <returns> Indicates whether the mouse event was processed successfully by the runtime.</returns>
    public virtual bool HandleMouseEvent(float x, float y, int icase, int buttonType, float scrollDelta, ref bool breturnOnHandled)
    {
        if (MovieID == 0)
        {
            return false;
        }
        
        if (AcceptMouseEvents(icase))
        {
            // Adjust according to viewport
            float xx = x;
            float yy = y;
            if (mRenderTexture != null && Initialize.mainCam != null)
            {
				RaycastHit hit;
        		if (Physics.Raycast(Initialize.mainCam.ScreenPointToRay(new Vector2(x, Screen.height - y)), out hit))
				{
          			Renderer hitRenderer = hit.collider.gameObject.GetComponent<Renderer>();
                    MeshCollider meshCollider = hit.collider as MeshCollider;
                    if (!(hit.collider is MeshCollider) || hitRenderer == null || meshCollider == null)
                    {
                        return false;
                    }

                    SFRTT rtt = hit.collider.gameObject.GetComponent("SFRTT") as SFRTT;
                    if (rtt == null)
                    {
                        return false;
                    }
	                float rttWidth 	= hitRenderer.material.mainTexture.width;
	                float rttHeight = hitRenderer.material.mainTexture.height;
	                float hitCoordX = hit.textureCoord.x * rttWidth;
	                float hitCoordY = rttHeight - hit.textureCoord.y * rttHeight;
	                xx = hitCoordX;
	                yy = hitCoordY;
                    yy -= 32; // HACK
				}
            }
            else
            {
				//Update viewport in case viewport is internally changed.
				GetViewport(ref ViewPort);
				//UnityEngine.Debug.Log(String.Format("GF Viewport [{0},{1}, {2}, {3}] \n", ViewPort.OX, ViewPort.OY, ViewPort.Width, ViewPort.Height));
#if UNITY_EDITOR
				xx -= ViewPort.OX ;
                yy -= ViewPort.OY + 24 ;// HACK, This value is from the real viewport(bigger than game screen) and the tool bar on top.
#else
				xx -= ViewPort.OX;
                yy -= ViewPort.OY;
#endif
			}
#if UNITY_WP8
			if (sf_handleMouseEvent(MovieID, xx, yy, icase, buttonType, scrollDelta))
#else
			if (SF_HandleMouseEvent(MovieID, xx, yy, icase, buttonType, scrollDelta))
#endif
            {
                return true;
            }
        }
        return false;
    }

	/// <summary>
	/// Passes key events to Scaleform runtime for processing.
	/// </summary>
	/// <param name="cd"> The scaleform keycode corresponding the Unity key code. See SFKey.cs for the mapping table.</param>
	/// <param name="mod"> Key Modifiers.</param>
	/// <param name="down"> indicates whether the key event is up/down.</param>
	/// <param name="breturnOnHandled">In general, key events are sent to all movies. Set this flag to true if you don't want other movies to handle key events.</param>
	/// <param name="keyboardIndex"> Indicates the keyboard index in case multiple keyboards are connected.</param>
	/// <returns> Indicates whether the key event was processed successfully by the runtime.</returns>
    public virtual bool HandleKeyEvent(SFKey.Code cd, SFKeyModifiers.Modifiers mod, UInt32 down, ref bool breturnOnHandled, int keyboardIndex = 0)
    {
        if (MovieID == 0)
        {
            return false;
        }
        if (AcceptKeyEvents())
        {
#if UNITY_WP8
            return sf_handleKeyEvent(MovieID, cd, mod, down, keyboardIndex);
#else
            return SF_HandleKeyEvent(MovieID, cd, mod, down, keyboardIndex);
#endif
        }
        return false;
    }

	/// <summary>
	/// Passes char events to Scaleform runtime for processing.
	/// </summary>
	/// <param name="wchar"> char code corresponding to the keystroke.</param>
	/// <param name="breturnOnHandled">In general, char events are sent to all movies. Set this flag to true if you don't want other movies to handle char events.</param>
	/// <returns> Indicates whether the key event was processed successfully by the runtime.</returns>
    public virtual bool HandleCharEvent(UInt32 wchar, ref bool breturnOnHandled)
    {
        if (MovieID == 0)
        {
            return false;
        }
        if (AcceptCharEvents())
        {
			if (wchar == 10) wchar = 13;
#if UNITY_WP8
            return sf_handleCharEvent(MovieID, wchar);
#else					
            return SF_HandleCharEvent(MovieID, wchar);
#endif
        }
        return false;
    }

	/// <summary>
	/// Passes touch events to Scaleform runtime for processing.
	/// </summary>
	/// <param name="fingerId"> fingerid, used for handling multi-touch input.</param>
	/// <param name="x"> x touch position.</param>
	/// <param name="y"> y touch position.</param>
	/// <param name="icase"> 1: TouchBegin, 2: TouchMove, 3: TouchEnd</param>
	/// <param name="breturnOnHandled">In general, touch events are sent to all movies. Set this flag to true if you don't want other movies to handle mouse events.</param>
	/// <returns> Indicates whether the touch event was processed successfully by the runtime.</returns>
    public virtual bool HandleTouchEvent(int fingerId, float x, float y, int icase, ref bool breturnOnHandled)
    {
		if (AcceptTouchEvents())
        {
            // Adjust according to viewport
            float xx = x - ViewPort.OX;
    	   	float yy = y;
#if UNITY_WP8
			return sf_handleTouchEvent(MovieID, fingerId, xx, yy, icase);
#else
            return SF_HandleTouchEvent(MovieID, fingerId, xx, yy, icase);
#endif
        }
        return false;
    }
    
	/// <summary>
	/// Sets the background transparency.
	/// </summary>
	/// <param name="alpha"> Background alpha.</param>
    public void SetBackgroundAlpha(float alpha)
    {
        if (MovieID == 0)
        {
            return;
        }
#if UNITY_WP8
        sf_setBackgroundAlpha(MovieID, alpha);
#else
        SF_SetBackgroundAlpha(MovieID, alpha);
#endif
    }

	private IntPtr Serialize(params object[] objs)
	{
		int valueSize = Marshal.SizeOf(typeof(Value));
		int IntPtrSize = Marshal.SizeOf(typeof(IntPtr));
		int IntSize = Marshal.SizeOf(typeof(int));
		int numElem = objs.Length;
		IntPtr ptr = IntPtr.Zero;

		if (numElem > 0)
		{
			ptr = Marshal.AllocCoTaskMem(valueSize * numElem);
			for (int i = 0; i < numElem; i++)
			{
				Value val;

				object currentObj = objs[i];
				if (!(currentObj is Value))
				{
					val = CreateValue(currentObj);
				}
				else
				{
					val = (Value)currentObj;
				}
				// Can't add an integer offset to IntPtr as you would with C/C++ pointer 
				IntPtr data = new IntPtr(ptr.ToInt32() + valueSize * i);
				Marshal.WriteIntPtr(data, val.GetInternalData());
				data = new IntPtr(data.ToInt32() + IntPtrSize);
				Marshal.WriteInt32(data, (int)val.GetType());
				data = new IntPtr(data.ToInt32() + IntSize);
				Marshal.WriteInt64(data, (long)val.GetMovieId());
			}
		}
		return ptr;
	}
	
	/// <summary>
	/// Used for injecting callback functions into the ActionScript VM. Currently doesn't work as Delegates don't work on all systems.
	/// </summary>
	/// <param name="funcContext"></param>
	/// <param name="funcName"></param>
	/// <param name="fptr"></param>
	/// <param name="pValueQueue"></param>
	/// <returns></returns>
#if !(UNITY_IPHONE) && !(UNITY_ANDROID)		
	public bool CreateFunction(Value funcContext, String funcName, IntPtr fptr, IntPtr pValueQueue)
	{
#if UNITY_WP8
		return sf_createFunction(funcContext, funcName, fptr, pValueQueue);
#else
		return SF_CreateFunction(funcContext, funcName, fptr, pValueQueue);
#endif
	}
#endif //!(UNITY_IPHONE) && !(UNITY_ANDROID)
	
	/// <summary>
	/// Used to create an ActionScript object of a given class.  
	/// 
	/// </summary>
	/// <param name="className"> ClassName. Fully qualified class name (for eg., flash.geom.matrix) should be provided if the class to be created is in a package.</param>
	/// <param name="objs">parameters to be passed to a constructor of className.</param>
	/// <returns>Returns the created ActionScript object wrapped in a Value.</returns>
	/// <remarks> 
    /// Usage: The following class defined in Scores.as can be generated from C# using <c> Value val = CreateObject("Scores", "somestring", 0, 2); </c>
	/// <code>package {
	/// public class Scores
	/// 	{
	/// 		public	var Name:String;
	/// 		public	var Score:Number;
	///			public 	var Code:Number
	/// 		public  var IsValid : Boolean;
	/// 		public  var Type : ScoreType;
	/// 		public function Scores(name:String, score:Number, code: Number)		
	/// 		{
	/// 			Name 	= name;
	/// 			Score 	= score;
	/// 			Code	= code;	
	/// 			Type 	= new ScoreType("TestScore");
	/// 		}
	/// }
    /// </code>
    /// Note that in order to pull in the class definition, you might have to declare a dummy variable of type Scores in your flash file.<c> var dummy:Scores;</c>
	/// Also see the Direct Access API for more details. 
	///</remarks>
		
	public Value CreateObject(String className, params object[] objs)
    {
		Value pvalManaged = new Value();
		int numElem = objs.Length;
		IntPtr ptr = Serialize(objs);
#if UNITY_WP8
		sf_createObject(MovieID, pvalManaged, className, numElem, ptr);
#else
        SF_CreateObject(MovieID, pvalManaged, className, numElem, ptr);
#endif
        Marshal.FreeCoTaskMem(ptr);
        return pvalManaged;
    }
	
	/// <summary>
	/// Used to create an ActionScript array from script
	/// </summary>
	/// <returns> The actionscript array wrapped in a value</returns>
	/// <remarks> Usage: Value arr = CreateArray(); 
	/// See DAPI Demo for more details</remarks>
	
		
	public Value CreateArray()
	{
		Value pvalManaged = new Value();
#if UNITY_WP8
        if(sf_createArray(MovieID, pvalManaged))
#else
        if(SF_CreateArray(MovieID, pvalManaged))
#endif
		{
			return pvalManaged;
		}
				
		return null;
	}
	
	/// <summary>
	/// Used to call an ActionScript method. 
	/// </summary>
	/// <param name="methodName"> Function name. The function should be defined in the movie clip on which Invoke is being called</param>
	/// <param name="objs"> Array of parameters</param>
	/// <returns> Return value wrapped in a Value </returns>
	/// <remarks> Sample usage: 
    /// <code>Invoke("funcName", {"hello", 1, true}); here funcName accepts a string, number and boolean arguments. </code></remarks>
		
	public Value Invoke(String methodName, params object[] objs)
    {
		int numElem = objs.Length;
		IntPtr ptr = Serialize(objs);
		Value retVal = new Value();
#if UNITY_WP8
		bool result = sf_invoke3(MovieID, methodName, numElem, ptr, retVal);
#else
        bool result = SF_Invoke3(MovieID, methodName, numElem, ptr, retVal);
#endif
        Marshal.FreeCoTaskMem(ptr);
		if (result)
			return retVal; // indicates that method was successfully invoked, but doesn't return anything
		else
			return null; // indicates there was a problem invoking the method.
    }



    private float RescaleValueWithOffset(float value, float scale, float offset)
    {
        // remove the offset
        value -= offset * 0.5f;

        // normalize value
        value /= offset;

        // scale up to the targetScale
        value *= scale;

        // restore the offset
        value += offset * 0.5f;

        return value;
    }

    /// <summary>
    /// Used to convert Stage coordinates to Unity screen coordinates
    /// </summary>
    /// <param name="stagePoint"> A point in Stage coordinates (z is ignored)</param>
    /// <returns>A point in Unity Screen Space.</returns>
    /// <remarks> Sample usage: 
    /// <code>
    /// double x = stageX, y = stageY;
    /// Vector3 stagePoint = new Vector3((float)x, (float)y, 0.0f);
    /// Vector3 screenPoint = StagePointToScreenPoint(stagePoint);</code></remarks>
    public Vector3 StagePointToScreenPoint(Vector3 stagePoint)
    {
        float x = stagePoint.x;
        float y = stagePoint.y;

        // fetch the viewport for this movie
        SFViewport vp = new SFViewport();
        GetViewport(ref vp);

        float movieWidth = GetMovieDef().GetWidth();
        float movieHeight = GetMovieDef().GetHeight();

        float stageX = x;
        float stageY = y;

        float scaleX = (vp.Width / movieWidth);
        float scaleY = (vp.Height / movieHeight);

        float deltaX = (vp.Width - movieWidth);
        float deltaY = (vp.Height - movieHeight);

        x = (int)(stageX * scaleX);
        y = (int)(stageY * scaleY);

        float stageAspectRatio = (movieHeight / movieWidth);
        float screenAspectRatio = (Screen.height / (float)Screen.width);

        // NOTE: It is intentional that the SM_NoBorder and SM_ShowAll cases have similar,
        // yet opposite handling based on the aspect ratio differences.
        switch (TheScaleModeType)
        {
            case ScaleModeType.SM_ExactFit:
                {
                    // Do nothing.
                    break;
                }
            case ScaleModeType.SM_NoBorder:
                {
                    // The original aspect ratio is kept, but content may be scaled and clipped.
                    if (screenAspectRatio < stageAspectRatio)
                    {
                        y = RescaleValueWithOffset(y, (Screen.width * stageAspectRatio), Screen.height);
                    }
                    else if (screenAspectRatio > stageAspectRatio)
                    {
                        x = RescaleValueWithOffset(x, (Screen.height / stageAspectRatio), Screen.width);
                    }

                    break;
                }
            case ScaleModeType.SM_NoScale:
                {
                    // The size of the content is fixed to the native resolution of the movie clip.
                    x = stageX;
                    x += (deltaX * 0.5f);
                    y = stageY;
                    y += (deltaY * 0.5f);
                    break;
                }
            case ScaleModeType.SM_ShowAll:
                {
                    // Scales the content to the viewport while maintaining the original aspect ratio. (default)
                    if (screenAspectRatio < stageAspectRatio)
                    {
                        x = RescaleValueWithOffset(x, (Screen.height / stageAspectRatio), Screen.width);
                    }
                    else if (screenAspectRatio > stageAspectRatio)
                    {
                        y = RescaleValueWithOffset(y, (Screen.width * stageAspectRatio), Screen.height);
                    }
                    break;
                }
        }

        // Match Unity's Screen space origin (lower left)
        y = Screen.height - y;

        return new Vector3(x, y, 0F);
    }

    public  Vector3 ScreenPointToStagePoint(Vector3 screenPoint)
    {
        float x = screenPoint.x;
        float y = screenPoint.y;

        float movieWidth = GetMovieDef().GetWidth();
        float movieHeight = GetMovieDef().GetHeight();

        // fetch the viewport for this movie
        SFViewport vp = new SFViewport();
        GetViewport(ref vp);

        float ScreenWidth = Screen.width;
        float ScreenHeight = Screen.height;

        float stageX = x;
        float stageY = y;

        float scaleX = (movieWidth / vp.Width);
        float scaleY = (movieHeight / vp.Height);

        float deltaX = (movieWidth - vp.Width);
        float deltaY = (movieHeight - vp.Height);

        x = stageX * scaleX;
        y = stageY * scaleY;

        float stageAspectRatio = (movieHeight / movieWidth);
        float screenAspectRatio = (ScreenHeight / ScreenWidth);

        switch (TheScaleModeType)
        {
            case ScaleModeType.SM_ExactFit:
                {
                    break;
                }
            case ScaleModeType.SM_NoBorder:
                {
                    if (screenAspectRatio < stageAspectRatio)
                    {
                        y = RescaleValueWithOffset(y, (movieWidth * screenAspectRatio), movieHeight);
                    }
                    else if (screenAspectRatio > stageAspectRatio)
                    {
                        x = RescaleValueWithOffset(x, (movieHeight / screenAspectRatio), movieWidth);
                    }
                    break;
                }
            case ScaleModeType.SM_NoScale:
                {
                    x = stageX;
                    x += (deltaX * 0.5f);
                    y = stageY;
                    y += (deltaY * 0.5f);
                    break;
                }
            case ScaleModeType.SM_ShowAll:
                {
                    if (screenAspectRatio < stageAspectRatio)
                    {
                        x = RescaleValueWithOffset(x, (movieHeight / screenAspectRatio), movieWidth);
                    }
                    else if (screenAspectRatio > stageAspectRatio)
                    {
                        y = RescaleValueWithOffset(y, (movieWidth * screenAspectRatio), movieHeight);
                    }
                    break;
                }
        }

        y = movieHeight - y;

        return new Vector3(x, y, 0F);
    }

    public virtual void SyncTickCountToAS(double _nDelta)
    {

    }
}

} // namespace Scaleform;
