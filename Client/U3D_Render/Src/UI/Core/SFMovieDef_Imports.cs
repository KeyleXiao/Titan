/**********************************************************************

Filename    :	SFMovieDef_Imports.cs
Content     :	
Created     :   
Authors     :   Ankur Mohan

Copyright   :   Copyright 2013 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.
 
***********************************************************************/

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Reflection;
using System.Runtime.InteropServices;

namespace Scaleform
{

// For a class or pointer to class to be passed to unmanaged code, it must have
// StructLayout Attribute.
[StructLayout(LayoutKind.Sequential)]
public partial class MovieDef : System.Object
{
#if (UNITY_STANDALONE || UNITY_EDITOR || UNITY_METRO) && !UNITY_WP8
	[DllImport("libgfxunity3d")]
	protected static extern int SF_GetFrameCount(IntPtr pMovieDef);

	[DllImport("libgfxunity3d")]
	protected static extern float SF_GetFrameRate2(IntPtr pMovieDef);

	[DllImport("libgfxunity3d")]
	[return: MarshalAs(UnmanagedType.LPStr)]
	protected static extern IntPtr SF_GetFileURL(IntPtr pMovieDef, [Out] IntPtr size);

	[DllImport("libgfxunity3d")]
	protected static extern float SF_GetMovieHeight(IntPtr pMovieDef);

	[DllImport("libgfxunity3d")]
	protected static extern float SF_GetMovieWidth(IntPtr pMovieDef);

	[DllImport("libgfxunity3d")]
	protected static extern int SF_GetVersion(IntPtr pMovieDef);

#elif UNITY_IPHONE
	[DllImport("__Internal")]
	protected static extern int SF_GetFrameCount(IntPtr pMovieDef);

	[DllImport("__Internal")]
	protected static extern float SF_GetFrameRate2(IntPtr pMovieDef);

	[DllImport("__Internal")]
	[return: MarshalAs(UnmanagedType.LPStr)]
	protected static extern IntPtr SF_GetFileURL(IntPtr pMovieDef, [Out] IntPtr size);

	[DllImport("__Internal")]
	protected static extern float SF_GetMovieHeight(IntPtr pMovieDef);

	[DllImport("__Internal")]
	protected static extern float SF_GetMovieWidth(IntPtr pMovieDef);

	[DllImport("__Internal")]
	protected static extern int SF_GetVersion(IntPtr pMovieDef);

#elif UNITY_ANDROID
	[DllImport("gfxunity3d")]
	protected static extern int SF_GetFrameCount(IntPtr pMovieDef);

	[DllImport("gfxunity3d")]
	protected static extern float SF_GetFrameRate2(IntPtr pMovieDef);

	[DllImport("gfxunity3d")]
	[return: MarshalAs(UnmanagedType.LPStr)]
	protected static extern IntPtr SF_GetFileURL(IntPtr pMovieDef, [Out] IntPtr size);

	[DllImport("gfxunity3d")]
	protected static extern float SF_GetMovieHeight(IntPtr pMovieDef);

	[DllImport("gfxunity3d")]
	protected static extern float SF_GetMovieWidth(IntPtr pMovieDef);

	[DllImport("gfxunity3d")]
	protected static extern int SF_GetVersion(IntPtr pMovieDef);

#elif UNITY_WP8
	public delegate int SF_GetFrameCount(IntPtr pMovieDef);
	public static SF_GetFrameCount sf_getFrameCount;
	public void SetSF_GetFrameCount(SF_GetFrameCount func){sf_getFrameCount = func;}

	public delegate float SF_GetFrameRate2(IntPtr pMovieDef);
	public static SF_GetFrameRate2 sf_getFrameRate2;
	public void SetSF_GetFrameRate2(SF_GetFrameRate2 func){sf_getFrameRate2 = func;}

	public delegate IntPtr SF_GetFileURL(IntPtr pMovieDef, [Out] IntPtr size);
	public static SF_GetFileURL sf_getFileURL;
	public void SetSF_GetFileURL(SF_GetFileURL func){sf_getFileURL = func;}

	public delegate float SF_GetMovieHeight(IntPtr pMovieDef);
	public static SF_GetMovieHeight sf_getMovieHeight;
	public void SetSF_GetMovieHeight(SF_GetMovieHeight func){sf_getMovieHeight = func;}

	public delegate float SF_GetMovieWidth(IntPtr pMovieDef);
	public static SF_GetMovieWidth sf_getMovieWidth;
	public void SetSF_GetMovieWidth(SF_GetMovieWidth func){sf_getMovieWidth = func;}

	public delegate int SF_GetVersion(IntPtr pMovieDef);
	public static SF_GetVersion sf_getVersion;
	public void SetSF_GetVersion(SF_GetVersion func){sf_getVersion = func;}

#endif
}

}//namespace Scaleform
