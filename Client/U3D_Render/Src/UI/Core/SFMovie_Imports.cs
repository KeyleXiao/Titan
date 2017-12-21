/**********************************************************************

Filename    :	SFMovie_Imports.cs
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
using System.Reflection;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace Scaleform
{

// For a class or pointer to class to be passed to unmanaged code, it must have
// StructLayout Attribute.
[StructLayout(LayoutKind.Sequential)]
public partial class Movie: UnityEngine.MonoBehaviour
{
#if (UNITY_STANDALONE || UNITY_EDITOR || UNITY_METRO) && !UNITY_WP8
	[DllImport("libgfxunity3d")]
	protected static extern int SF_Advance(long movieID, float deltaTime, bool advanceWhilePaused = false);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_ApplyRenderTexture(long MovieID, long NativeID);

	[DllImport("libgfxunity3d")]
	protected static extern int SF_GetAVMVersion(long movieID);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_CreateArray(long movieId, [Out] Value pvalManaged);

	[DllImport("libgfxunity3d")]
	public static extern bool SF_CreateFunction(Value funcContext1, String path, IntPtr fptr, IntPtr pValQueue);

	[DllImport("libgfxunity3d")]
	protected static extern int SF_CreateMovie(IntPtr param);

    [DllImport("libgfxunity3d")]
    protected static extern bool SF_DestroyMovie(long movieID);

	[DllImport("libgfxunity3d")]
	public static extern int SF_CreateMovieInstance(IntPtr movieDefID, IntPtr param);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_CreateObject(long movieId, [Out] Value pvalManaged, String className, int numValues, IntPtr mvalArray);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_DoHitTest(long movieID, float x, float y, int hitTestType);

	[DllImport("libgfxunity3d")]
	protected static extern float SF_GetFrameRate(int MovieID);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_GetVariable(long MovieID, [Out] Value v, String varPath);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_GetViewport(long MovieID, [Out] SFViewport vp);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_HandleCharEvent(long movieID, UInt32 wchar);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_HandleKeyEvent(long movieID, SFKey.Code cd, SFKeyModifiers.Modifiers mod, UInt32 down, int keyboardIndex);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_HandleMouseEvent(long movieID, float x, float y, int icase, int buttonType, float scrollDelta);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_HandleTouchEvent(long movieID, int fid, float x, float y, int icase);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_Invoke3(long movieID, String methodName, int numVal, IntPtr iptr, [Out] Value retVal);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_IsPaused(long MovieID);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_NotifyNativeManager(long movieID, MovieLifeCycleEvents ev);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_SetBackgroundAlpha(long movieID, float bgAlpha);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_SetDepth(long MovieID, int depth);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_SetFocus(long movieID, bool focus);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_SetPaused(long MovieID, bool pause);

	[DllImport("libgfxunity3d")]
	protected static extern float SF_SetVariable(long MovieID, String varPath, Value v, SetVarType type);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_SetViewport(long MovieID, SFViewport vp);
			
	[DllImport("libgfxunity3d")]
	protected static extern IntPtr SF_GetMovieDef(long MovieID);
			

#elif UNITY_IPHONE
	[DllImport("__Internal")]
	protected static extern int SF_Advance(long movieID, float deltaTime, bool advanceWhilePaused = false);

	[DllImport("__Internal")]
	protected static extern void SF_ApplyRenderTexture(long MovieID, long NativeID);

	[DllImport("__Internal")]
	protected static extern int SF_GetAVMVersion(long movieID);

	[DllImport("__Internal")]
	protected static extern bool SF_CreateArray(long movieId, [Out] Value pvalManaged);

	[DllImport("__Internal")]
	protected static extern int SF_CreateMovie(IntPtr param);

	[DllImport("__Internal")]
	public static extern int SF_CreateMovieInstance(IntPtr movieDefID, IntPtr param);

	[DllImport("__Internal")]
	protected static extern bool SF_CreateObject(long movieId, [Out] Value pvalManaged, String className, int numValues, IntPtr mvalArray);

	[DllImport("__Internal")]
	protected static extern bool SF_DoHitTest(long movieID, float x, float y, int hitTestType);

	[DllImport("__Internal")]
	protected static extern float SF_GetFrameRate(int MovieID);

	[DllImport("__Internal")]
	protected static extern bool SF_GetVariable(long MovieID, [Out] Value v, String varPath);

	[DllImport("__Internal")]
	protected static extern bool SF_GetViewport(long MovieID, [Out] SFViewport vp);

	[DllImport("__Internal")]
	protected static extern bool SF_HandleCharEvent(long movieID, UInt32 wchar);

	[DllImport("__Internal")]
	protected static extern bool SF_HandleKeyEvent(long movieID, SFKey.Code cd, SFKeyModifiers.Modifiers mod, UInt32 down, int keyboardIndex);

	[DllImport("__Internal")]
	protected static extern bool SF_HandleMouseEvent(long movieID, float x, float y, int icase, int buttonType, float scrollDelta);

	[DllImport("__Internal")]
	protected static extern bool SF_HandleTouchEvent(long movieID, int fid, float x, float y, int icase);

	[DllImport("__Internal")]
	protected static extern bool SF_Invoke3(long movieID, String methodName, int numVal, IntPtr iptr, [Out] Value retVal);

	[DllImport("__Internal")]
	protected static extern bool SF_IsPaused(long MovieID);

	[DllImport("__Internal")]
	protected static extern void SF_NotifyNativeManager(long movieID, MovieLifeCycleEvents ev);

	[DllImport("__Internal")]
	protected static extern bool SF_SetBackgroundAlpha(long movieID, float bgAlpha);

	[DllImport("__Internal")]
	protected static extern bool SF_SetDepth(long MovieID, int depth);

	[DllImport("__Internal")]
	protected static extern void SF_SetFocus(long movieID, bool focus);

	[DllImport("__Internal")]
	protected static extern void SF_SetPaused(long MovieID, bool pause);

	[DllImport("__Internal")]
	protected static extern float SF_SetVariable(long MovieID, String varPath, Value v, SetVarType type);

	[DllImport("__Internal")]
	protected static extern bool SF_SetViewport(long MovieID, SFViewport vp);
			
	[DllImport("__Internal")]
	protected static extern IntPtr SF_GetMovieDef(long MovieID);
			
#elif UNITY_ANDROID
	[DllImport("gfxunity3d")]
	protected static extern int SF_Advance(long movieID, float deltaTime, bool advanceWhilePaused = false);

	[DllImport("gfxunity3d")]
	protected static extern void SF_ApplyRenderTexture(long MovieID, long NativeID);

	[DllImport("gfxunity3d")]
	protected static extern int SF_GetAVMVersion(long movieID);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_CreateArray(long movieId, [Out] Value pvalManaged);

	[DllImport("gfxunity3d")]
	protected static extern int SF_CreateMovie(IntPtr param);

	[DllImport("gfxunity3d")]
	public static extern int SF_CreateMovieInstance(IntPtr movieDefID, IntPtr param);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_CreateObject(long movieId, [Out] Value pvalManaged, String className, int numValues, IntPtr mvalArray);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_DoHitTest(long movieID, float x, float y, int hitTestType);

	[DllImport("gfxunity3d")]
	protected static extern float SF_GetFrameRate(int MovieID);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_GetVariable(long MovieID, [Out] Value v, String varPath);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_GetViewport(long MovieID, [Out] SFViewport vp);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_HandleCharEvent(long movieID, UInt32 wchar);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_HandleKeyEvent(long movieID, SFKey.Code cd, SFKeyModifiers.Modifiers mod, UInt32 down, int keyboardIndex);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_HandleMouseEvent(long movieID, float x, float y, int icase, int buttonType, float scrollDelta);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_HandleTouchEvent(long movieID, int fid, float x, float y, int icase);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_Invoke3(long movieID, String methodName, int numVal, IntPtr iptr, [Out] Value retVal);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_IsPaused(long MovieID);

	[DllImport("gfxunity3d")]
	protected static extern void SF_NotifyNativeManager(long movieID, MovieLifeCycleEvents ev);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_SetBackgroundAlpha(long movieID, float bgAlpha);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_SetDepth(long MovieID, int depth);

	[DllImport("gfxunity3d")]
	protected static extern void SF_SetFocus(long movieID, bool focus);

	[DllImport("gfxunity3d")]
	protected static extern void SF_SetPaused(long MovieID, bool pause);

	[DllImport("gfxunity3d")]
	protected static extern float SF_SetVariable(long MovieID, String varPath, Value v, SetVarType type);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_SetViewport(long MovieID, SFViewport vp);
			
	[DllImport("gfxunity3d")]
	protected static extern IntPtr SF_GetMovieDef(long MovieID);			
#elif UNITY_WP8
	public delegate int SF_Advance(long movieID, float deltaTime, bool advanceWhilePaused = false);
	public static SF_Advance sf_advance;
	public void SetSF_Advance(SF_Advance func){sf_advance = func;}

	public delegate void SF_ApplyRenderTexture(long MovieID, long NativeID);
	public static SF_ApplyRenderTexture sf_applyRenderTexture;
	public void SetSF_ApplyRenderTexture(SF_ApplyRenderTexture func){sf_applyRenderTexture = func;}

	public delegate int SF_GetAVMVersion(long movieID);
	public static SF_GetAVMVersion sf_getAVMVersion;
	public void SetSF_GetAVMVersion(SF_GetAVMVersion func){sf_getAVMVersion = func;}

	public delegate bool SF_CreateArray(long movieId, [Out] Value pvalManaged);
	public static SF_CreateArray sf_createArray;
	public void SetSF_CreateArray(SF_CreateArray func){sf_createArray = func;}

	public delegate bool SF_CreateFunction(Value funcContext1, String path, IntPtr fptr, IntPtr pValQueue);
	public static SF_CreateFunction sf_createFunction;
	public void SetSF_CreateFunction(SF_CreateFunction func){sf_createFunction = func;}

	public delegate int SF_CreateMovie(IntPtr param);
	public static SF_CreateMovie sf_createMovie;
	public void SetSF_CreateMovie(SF_CreateMovie func){sf_createMovie = func;}

	public delegate int SF_CreateMovieInstance(IntPtr movieDefID, IntPtr param);
	public static SF_CreateMovieInstance sf_createMovieInstance;
	public void SetSF_CreateMovieInstance(SF_CreateMovieInstance func){sf_createMovieInstance = func;}

	public delegate bool SF_CreateObject(long movieId, [Out] Value pvalManaged, String className, int numValues, IntPtr mvalArray);
	public static SF_CreateObject sf_createObject;
	public void SetSF_CreateObject(SF_CreateObject func){sf_createObject = func;}

	public delegate bool SF_DoHitTest(long movieID, float x, float y, int hitTestType);
	public static SF_DoHitTest sf_doHitTest;
	public void SetSF_DoHitTest(SF_DoHitTest func){sf_doHitTest = func;}

	public delegate float SF_GetFrameRate(int MovieID);
	public static SF_GetFrameRate sf_getFrameRate;
	public void SetSF_GetFrameRate(SF_GetFrameRate func){sf_getFrameRate = func;}

	public delegate bool SF_GetVariable(long MovieID, [Out] Value v, String varPath);
	public static SF_GetVariable sf_getVariable;
	public void SetSF_GetVariable(SF_GetVariable func){sf_getVariable = func;}

	public delegate bool SF_GetViewport(long MovieID, [Out] SFViewport vp);
	public static SF_GetViewport sf_getViewport;
	public void SetSF_GetViewport(SF_GetViewport func){sf_getViewport = func;}

	public delegate bool SF_HandleCharEvent(long movieID, UInt32 wchar);
	public static SF_HandleCharEvent sf_handleCharEvent;
	public void SetSF_HandleCharEvent(SF_HandleCharEvent func){sf_handleCharEvent = func;}

	public delegate bool SF_HandleKeyEvent(long movieID, SFKey.Code cd, SFKeyModifiers.Modifiers mod, UInt32 down, int keyboardIndex);
	public static SF_HandleKeyEvent sf_handleKeyEvent;
	public void SetSF_HandleKeyEvent(SF_HandleKeyEvent func){sf_handleKeyEvent = func;}

	public delegate bool SF_HandleMouseEvent(long movieID, float x, float y, int icase, int buttonType, float scrollDelta);
	public static SF_HandleMouseEvent sf_handleMouseEvent;
	public void SetSF_HandleMouseEvent(SF_HandleMouseEvent func){sf_handleMouseEvent = func;}

	public delegate bool SF_HandleTouchEvent(long movieID, int fid, float x, float y, int icase);
	public static SF_HandleTouchEvent sf_handleTouchEvent;
	public void SetSF_HandleTouchEvent(SF_HandleTouchEvent func){sf_handleTouchEvent = func;}

	public delegate bool SF_Invoke3(long movieID, String methodName, int numVal, IntPtr iptr, [Out] Value retVal);
	public static SF_Invoke3 sf_invoke3;
	public void SetSF_Invoke3(SF_Invoke3 func){sf_invoke3 = func;}

	public delegate bool SF_IsPaused(long MovieID);
	public static SF_IsPaused sf_isPaused;
	public void SetSF_IsPaused(SF_IsPaused func){sf_isPaused = func;}

	public delegate void SF_NotifyNativeManager(long movieID, MovieLifeCycleEvents ev);
	public static SF_NotifyNativeManager sf_notifyNativeManager;
	public void SetSF_NotifyNativeManager(SF_NotifyNativeManager func){sf_notifyNativeManager = func;}

	public delegate bool SF_SetBackgroundAlpha(long movieID, float bgAlpha);
	public static SF_SetBackgroundAlpha sf_setBackgroundAlpha;
	public void SetSF_SetBackgroundAlpha(SF_SetBackgroundAlpha func){sf_setBackgroundAlpha = func;}

	public delegate bool SF_SetDepth(long MovieID, int depth);
	public static SF_SetDepth sf_setDepth;
	public void SetSF_SetDepth(SF_SetDepth func){sf_setDepth = func;}

	public delegate void SF_SetFocus(long movieID, bool focus);
	public static SF_SetFocus sf_setFocus;
	public void SetSF_SetFocus(SF_SetFocus func){sf_setFocus = func;}

	public delegate void SF_SetPaused(long MovieID, bool pause);
	public static SF_SetPaused sf_setPaused;
	public void SetSF_SetPaused(SF_SetPaused func){sf_setPaused = func;}

	public delegate float SF_SetVariable(long MovieID, String varPath, Value v, SetVarType type);
	public static SF_SetVariable sf_setVariable;
	public void SetSF_SetVariable(SF_SetVariable func){sf_setVariable = func;}

	public delegate bool SF_SetViewport(long MovieID, SFViewport vp);
	public static SF_SetViewport sf_setViewport;
	public void SetSF_SetViewport(SF_SetViewport func){sf_setViewport = func;}
			
	public delegate IntPtr SF_GetMovieDef(long MovieID);
	public static SF_GetMovieDef sf_getMovieDef;
	public void SetSF_GetMovieDef(SF_GetMovieDef func)sf_getMovieDef = func;}
#endif
}

}// namespace Scaleform
