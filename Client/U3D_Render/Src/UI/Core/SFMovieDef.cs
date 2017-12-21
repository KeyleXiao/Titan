/**********************************************************************

Filename    :	SFMovieDef.cs
Content     :	MovieDef Wrapper
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

    /// <summary>
    /// MovieDef contains properties such as version information, width, height, frame count and so on that represent the "movie definition". 
    /// </summary>
public partial class MovieDef : System.Object
{
	/// <summary>
	/// String corresponding to the Movie name. 
	/// </summary>
	public SFMovieCreationParams CreationParams;
			
	/// <summary>
	/// Points to the C++ moviedef.
	/// </summary>
	protected	IntPtr pMovieDef 		= IntPtr.Zero; 
	protected 	IntPtr pCreationParams 	= IntPtr.Zero;
				
	/// <summary>
	/// Creates a movie definition from a given movie name. The movie name must be the absolute pathname of the SWF or GFX file on the system.
	/// </summary>
	/// <param name="movieName"> Moviename corresponds to the absolute pathname of the swf/gfx file on the system.</param>
	public MovieDef(SFMovieCreationParams creationParams)
	{
		// This is not yet implemented.
		pMovieDef = IntPtr.Zero;
	}			
			
	/// <summary>
	/// Creates a movie definition from a given movie name. The movie name must be the absolute pathname of the SWF or GFX file on the system.
	/// </summary>
	/// <param name="movieDef">movieDef is a value returned by SF_GetMovieDef.</param>
	public MovieDef(IntPtr movieDef)
	{
		pMovieDef = movieDef;
	}
			
	~MovieDef()
	{
	}
			
	/// <summary>
	/// Creates a movie instance according to the specified movie creation parameters. 
	/// </summary>
	/// <param name="creationParams"> Movie Creation Parameters such as viewport size, type of actionscript VM to initialize.</param>
	/// <returns>C++ movie pointer cast as an integer. </returns>
	public long CreateInstance(SFMovieCreationParams creationParams)
	{
		if (pMovieDef == IntPtr.Zero)
		{
			return 0;
		}

		long movieId = 0;
		
		if (pCreationParams == IntPtr.Zero)
		{
			int cpSize = Marshal.SizeOf(typeof(SFMovieCreationParams));
			pCreationParams = Marshal.AllocCoTaskMem(cpSize);
			Marshal.StructureToPtr(CreationParams, pCreationParams, false);		
		}
#if UNITY_WP8
		movieId = Movie.sf_createMovieInstance(pMovieDef, pCreationParams);
#else
		movieId = Movie.SF_CreateMovieInstance(pMovieDef, pCreationParams);
#endif
//		Marshal.DestroyStructure(pCreationParams, typeof(SFMovieCreationParams));

		return movieId;
	}
	/// <summary>
	/// Returns the version number of the SWF.
	/// </summary>
	/// <returns>Version number of the SWF.</returns>
	public int GetVersion()
	{
#if UNITY_WP8
		return sf_getVersion(pMovieDef);
#else
		return SF_GetVersion(pMovieDef);
#endif
	}

	/// <summary>
	/// Returns movie width.
	/// </summary>
	/// <returns>movie width.</returns>
	public float GetWidth()
	{
#if UNITY_WP8
		return sf_getMovieWidth(pMovieDef);
#else
		return SF_GetMovieWidth(pMovieDef);
#endif
	}

	/// <summary>
	/// Returns movie height.
	/// </summary>
	/// <returns>movie height.</returns>
	public float GetHeight()
	{
#if UNITY_WP8
		return sf_getMovieHeight(pMovieDef);
#else
		return SF_GetMovieHeight(pMovieDef);
#endif
	}

	/// <summary>
	/// Returns number of frames in the movie.
	/// </summary>
	/// <returns>number of frames.</returns>
	public int GetFrameCount()
	{
#if UNITY_WP8
		return sf_getFrameCount(pMovieDef);
#else
		return SF_GetFrameCount(pMovieDef);
#endif
	}

	/// <summary>
	/// Returns the movie frame rate.
	/// </summary>
	/// <returns>movie frame rate.</returns>
	public float GetFrameRate()
	{
#if UNITY_WP8
		return sf_getFrameRate2(pMovieDef);
#else
		return SF_GetFrameRate2(pMovieDef);
#endif
	}

	/// <summary>
	/// The URL of the flash file. Typically corresponds to the location of the SWF on the disk.
	/// </summary>
	/// <returns> Flash file URL.</returns>
	public String GetFileURL()
	{
		int sz = 0;
		IntPtr szLoc = Marshal.AllocCoTaskMem(4);
#if UNITY_WP8
		IntPtr ptr = sf_getFileURL(pMovieDef, szLoc);
#else
		IntPtr ptr = SF_GetFileURL(pMovieDef, szLoc);
#endif
		sz = Marshal.ReadInt32(szLoc);
		if (sz != -1)
		{
			byte[] dest = new byte[sz+1];
			Marshal.Copy(ptr, dest, 0, sz);
			UnityEngine.Debug.Log(dest);
#if NETFX_CORE
			return System.Text.Encoding.UTF8.GetString(dest, 0, dest.Length);
#else
			return System.Text.Encoding.UTF8.GetString(dest);
#endif
		}
		return null;
	}
}
} //namespace Scaleform
