/**********************************************************************

Filename    :	SFManager_Imports.cs
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
public partial class SFManager
{
#if (UNITY_STANDALONE || UNITY_EDITOR || UNITY_METRO) && !UNITY_WP8
	[DllImport("libgfxunity3d")]
	protected static extern void SF_ApplyLanguage(String langName);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_ClearCommandBuffer(int numCommands);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_Destroy();

	[DllImport("libgfxunity3d")]
	protected static extern void SF_DestroyManager();

    //[DllImport("libgfxunity3d")]
    //protected static extern bool SF_DestroyMovie(long movieID);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_Display();

	[DllImport("libgfxunity3d")]
	protected static extern int SF_Init(IntPtr pdata, int size, String version);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_LoadFontConfig(String fontConfigPath);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_ProcessMarkedForDeleteMovies();

	[DllImport("libgfxunity3d")]
	protected static extern void SF_SetAllocateValues(SF_AllocateDelegate sf_allocDelegate);
		
	[DllImport("libgfxunity3d")]
	protected static extern void SF_SetDefaultFontLibName(String fontLib);
		
	[DllImport("libgfxunity3d")]
	protected static extern void SF_SetDisplayInfoDelegate(SF_DisplayInfoDelegate sf_doDelegate);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_SetExternalInterfaceDelegate(SF_ExternalInterfaceDelegate sf_eiDelegate);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_SetLogDelegate(SF_LogDelegate sf_logDelegate);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_SetNewViewport(int ox, int oy, int Width, int Height);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_SetSharedData(IntPtr pCommandOffset, IntPtr pCommandQueue, Int32 id);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_ReplaceTexture(long movieId, String textureName, int textureId, int RTWidth, int RTHeight);

#elif UNITY_IPHONE
	[DllImport("__Internal")]
	protected static extern void SF_ApplyLanguage(String langName);

	[DllImport("__Internal")]
	protected static extern void SF_ClearCommandBuffer(int numCommands);

	[DllImport("__Internal")]
	protected static extern void SF_Destroy();

	[DllImport("__Internal")]
	protected static extern void SF_DestroyManager();

	[DllImport("__Internal")]
	protected static extern bool SF_DestroyMovie(long movieID);

	[DllImport("__Internal")]
	protected static extern bool SF_Display();

	[DllImport("__Internal")]
	protected static extern int SF_Init(IntPtr pdata, int size, String version);

	[DllImport("__Internal")]
	protected static extern void SF_LoadFontConfig(String fontConfigPath);

	[DllImport("__Internal")]
	protected static extern bool SF_ProcessMarkedForDeleteMovies();

	[DllImport("__Internal")]
	protected static extern void SF_SetAllocateValues(SF_AllocateDelegate sf_allocDelegate);
		
	[DllImport("__Internal")]
	protected static extern void SF_SetDefaultFontLibName(String fontLib);
		
	[DllImport("__Internal")]
	protected static extern void SF_SetDisplayInfoDelegate(SF_DisplayInfoDelegate sf_doDelegate);

	[DllImport("__Internal")]
	protected static extern void SF_SetExternalInterfaceDelegate(SF_ExternalInterfaceDelegate sf_eiDelegate);

	[DllImport("__Internal")]
	protected static extern void SF_SetLogDelegate(SF_LogDelegate sf_logDelegate);

	[DllImport("__Internal")]
	protected static extern void SF_SetNewViewport(int ox, int oy, int Width, int Height);

	[DllImport("__Internal")]
	protected static extern void SF_SetSharedData(IntPtr pCommandOffset, IntPtr pCommandQueue, Int32 id);

	[DllImport("__Internal")]
	protected static extern bool SF_ReplaceTexture(long movieId, String textureName, int textureId, int RTWidth, int RTHeight);

#elif UNITY_ANDROID
	[DllImport("gfxunity3d")]
	protected static extern void SF_ApplyLanguage(String langName);

	[DllImport("gfxunity3d")]
	protected static extern void SF_ClearCommandBuffer(int numCommands);

	[DllImport("gfxunity3d")]
	protected static extern void SF_Destroy();

	[DllImport("gfxunity3d")]
	protected static extern void SF_DestroyManager();

	[DllImport("gfxunity3d")]
	protected static extern bool SF_DestroyMovie(long movieID);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_Display();

	[DllImport("gfxunity3d")]
	protected static extern int SF_Init(IntPtr pdata, int size, String version);

	[DllImport("gfxunity3d")]
	protected static extern void SF_LoadFontConfig(String fontConfigPath);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_ProcessMarkedForDeleteMovies();

	[DllImport("gfxunity3d")]
	protected static extern void SF_SetAllocateValues(SF_AllocateDelegate sf_allocDelegate);
		
	[DllImport("gfxunity3d")]
	protected static extern void SF_SetDefaultFontLibName(String fontLib);
		
	[DllImport("gfxunity3d")]
	protected static extern void SF_SetDisplayInfoDelegate(SF_DisplayInfoDelegate sf_doDelegate);

	[DllImport("gfxunity3d")]
	protected static extern void SF_SetExternalInterfaceDelegate(SF_ExternalInterfaceDelegate sf_eiDelegate);

	[DllImport("gfxunity3d")]
	protected static extern void SF_SetLogDelegate(SF_LogDelegate sf_logDelegate);

	[DllImport("gfxunity3d")]
	protected static extern void SF_SetNewViewport(int ox, int oy, int Width, int Height);

	[DllImport("gfxunity3d")]
	protected static extern void SF_SetSharedData(IntPtr pCommandOffset, IntPtr pCommandQueue, Int32 id);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_ReplaceTexture(long movieId, String textureName, int textureId, int RTWidth, int RTHeight);

#elif UNITY_WP8
	public delegate void SF_ApplyLanguage(String langName);
	public static SF_ApplyLanguage sf_applyLanguage;
	public void SetSF_ApplyLanguage(SF_ApplyLanguage func){sf_applyLanguage = func;}

	public delegate void SF_ClearCommandBuffer(int numCommands);
	public static SF_ClearCommandBuffer sf_clearCommandBuffer;
	public void SetSF_ClearCommandBuffer(SF_ClearCommandBuffer func){sf_clearCommandBuffer = func;}

	public delegate void SF_Destroy();
	public static SF_Destroy sf_destroy;
	public void SetSF_Destroy(SF_Destroy func){sf_destroy = func;}

	public delegate void SF_DestroyManager();
	public static SF_DestroyManager sf_destroyManager;
	public void SetSF_DestroyManager(SF_DestroyManager func){sf_destroyManager = func;}

	public delegate bool SF_DestroyMovie(long movieID);
	public static SF_DestroyMovie sf_destroyMovie;
	public void SetSF_DestroyMovie(SF_DestroyMovie func){sf_destroyMovie = func;}

	public delegate bool SF_Display();
	public static SF_Display sf_display;
	public void SetSF_Display(SF_Display func){sf_display = func;}

	public delegate int SF_Init(IntPtr pdata, int size, String version);
	public static SF_Init sf_init;
	public void SetSF_Init(SF_Init func){sf_init = func;}

	public delegate void SF_LoadFontConfig(String fontConfigPath);
	public static SF_LoadFontConfig sf_loadFontConfig;
	public void SetSF_LoadFontConfig(SF_LoadFontConfig func){sf_loadFontConfig = func;}

	public delegate bool SF_ProcessMarkedForDeleteMovies();
	public static SF_ProcessMarkedForDeleteMovies sf_processMarkedForDeleteMovies;
	public void SetSF_ProcessMarkedForDeleteMovies(SF_ProcessMarkedForDeleteMovies func){sf_processMarkedForDeleteMovies = func;}

	public delegate void SF_SetAllocateValues(SF_AllocateDelegate sf_allocDelegate);
	public static SF_SetAllocateValues sf_setAllocateValues;
	public void SetSF_SetAllocateValues(SF_SetAllocateValues func){sf_setAllocateValues = func;}
		
	public delegate void SF_SetDefaultFontLibName(String fontLib);
	public static SF_SetDefaultFontLibName sf_SetDefaultFontLibName;
	public void SetSF_SetDefaultFontLibName(SF_SetDefaultFontLibName func){sf_SetDefaultFontLibName = func;}
		
	public delegate void SF_SetDisplayInfoDelegate(SF_DisplayInfoDelegate sf_doDelegate);
	public static SF_SetDisplayInfoDelegate sf_setDisplayInfoDelegate;
	public void SetSF_SetDisplayInfoDelegate(SF_SetDisplayInfoDelegate func){sf_setDisplayInfoDelegate = func;}

	public delegate void SF_SetExternalInterfaceDelegate(SF_ExternalInterfaceDelegate sf_eiDelegate);
	public static SF_SetExternalInterfaceDelegate sf_setExternalInterfaceDelegate;
	public void SetSF_SetExternalInterfaceDelegate(SF_SetExternalInterfaceDelegate func){sf_setExternalInterfaceDelegate = func;}

	public delegate void SF_SetLogDelegate(SF_LogDelegate sf_logDelegate);
	public static SF_SetLogDelegate sf_setLogDelegate;
	public void SetSF_SetLogDelegate(SF_SetLogDelegate func){sf_setLogDelegate = func;}

	public delegate void SF_SetNewViewport(int ox, int oy, int Width, int Height);
	public static SF_SetNewViewport sf_setNewViewport;
	public void SetSF_SetNewViewport(SF_SetNewViewport func){sf_setNewViewport = func;}

	public delegate void SF_SetSharedData(IntPtr pCommandOffset, IntPtr pCommandQueue, Int32 id);
	public static SF_SetSharedData sf_setSharedData;
	public void SetSF_SetSharedData(SF_SetSharedData func){sf_setSharedData = func;}

	public delegate bool SF_ReplaceTexture(long movieId, String textureName, int textureId, int RTWidth, int RTHeight);
	public static SF_ReplaceTexture sf_replaceTexture;
	public void SetSF_ReplaceTexture(SF_ReplaceTexture func){sf_replaceTexture = func;}

#endif
}

}// namespace Scaleform;
