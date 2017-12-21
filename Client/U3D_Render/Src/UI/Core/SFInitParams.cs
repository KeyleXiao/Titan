/**********************************************************************

Filename    :	SFInitParams.cs
Content     :	Inititialization parameters for Scaleform runtime
Created     :   
Authors     :   Ankur Mohan

Copyright   :   Copyright 2012 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.
 
***********************************************************************/

using UnityEngine;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

namespace Scaleform
{

/// <summary>
    /// ImageFormat class is used to set the image formats you expect to be handled by Scaleform.
/// </summary>
    /* This class is used to set the image formats you expect to be handled by Scaleform. Typically, setting the default format is sufficient. The default format adds all image handlers provided in Scaleform to the ImageFileHandlerRegistry. 
     * The actual handlers added are dependent on platform support, but may include any of SIF, DDS, TGA, PVR, PNG, KTX, JPEG, GTX, GXT. 
     * If your content only contains images of a particular format, you can prevent other image loaders to be linked in by setting "TheImageFormat" member of InitParams. For example,
     * InitParams.TheImageFormat = ImageFormat.IMG_JPG | ImageFormat.IMG_KTX;
     */ 
public class ImageFormat
{
	public const UInt32 IMG_Default = 0xffffffff;
	public const UInt32 IMG_JPG = 1;
	public const UInt32 IMG_PNG = 1<<1; 
	public const UInt32 IMG_DDS = 1<<2;
	public const UInt32 IMG_TGA = 1<<3; 
	public const UInt32 IMG_SIF = 1<<4; 
	public const UInt32 IMG_PVR = 1<<5; 
	public const UInt32 IMG_KTX = 1<<6; 
	public const UInt32 IMG_GXT = 1<<7;
	public const UInt32 IMG_GTX = 1<<8;
	public const UInt32 IMG_GNF = 1<<9;
}	

//////////////////////////////////////////////////////////////////////////
	
/// <summary>
/// This class is used to set initialization parameters for the Scaleform runtime. 
/// </summary>
// We Need Serializable and class attributes to make InitParms visible in the editor. The field offsets are needed in order to make the memory layout of the C# structure match that of C++. 
[StructLayout(LayoutKind.Explicit, Pack=4)]
[Serializable]
public class SFInitParams
{

    // static int sizeInt = Marshal.SizeOf(typeof(int));
    public SFInitParams()
    {
        SetFontCacheParams 	= true;
        SetFontPackParams 	= true;
        IsInitIME			= false;
			
        TheToleranceParams.Epsilon                  = 1e-5f;
        TheToleranceParams.CurveTolerance           = 1.0f;
        TheToleranceParams.CollinearityTolerance    = 1.0f;
        TheToleranceParams.IntersectionEpsilon      = 1e-3f;
        TheToleranceParams.FillLowerScale           = 0.707f;
        TheToleranceParams.FillUpperScale           = 1.414f;
        TheToleranceParams.FillAliasedLowerScale    = 0.5f;
        TheToleranceParams.FillAliasedUpperScale    = 2.0f;
        TheToleranceParams.StrokeLowerScale         = 0.99f;
        TheToleranceParams.StrokeUpperScale         = 1.01f;
        TheToleranceParams.HintedStrokeLowerScale   = 0.999f;
        TheToleranceParams.HintedStrokeUpperScale   = 1.001f;
        TheToleranceParams.Scale9LowerScale         = 0.995f;
        TheToleranceParams.Scale9UpperScale         = 1.005f;
        TheToleranceParams.EdgeAAScale              = 0.95f;
        TheToleranceParams.MorphTolerance           = 0.000f;
        TheToleranceParams.MinDet3D                 = 0.001f;
        TheToleranceParams.MinScale3D               = 0.05f;
		
		UseSystemFontProvider = true;
    }
    
	/// <summary>
	/// ActionScript VM Type. Typically only one VM is used, so only set the type that is needed. This will prevent code for the other VM from being loaded, saving memory. 
	/// </summary>
    public enum ASVersion:int
    {
        AS2 = 0,
        AS3,
        Both
    }
    
    [FieldOffset(0)]
    public ASVersion TheASVersion = SFInitParams.ASVersion.AS3;
    /// <summary>
    /// Initializes the video subsystem. Note that Scaleform video uses the CRI video codec which is only available on PC and Consoles, not on mobiles. So unless you have licensed Scaleform video and are working on a supported platform, best to set InitVideo = false
    /// </summary>
	[FieldOffset(4)]
    public bool InitVideo = false;
	/// <summary>
	/// Scaleform uses the FMOD sound library to render sound. FMOD is enabled by default and is provided with the integration. On PC, the FMOD libs are linked into the Unity Integration dll. On iOS, FMOD libs are linked in when the XCode project is built. On Mac and Android, FMOD dylibs/.so are linked in dynamically. Note that on Android, a special set up step is required which is described in the Android readme. 
	/// </summary>
    [FieldOffset(5)]
    public bool InitSound = true;

	/// <summary>
	/// The dynamic shader compilation flag tells the Scaleform renderer to compile shaders as they are needed, instead of all at once. This can be helpful in reducing the start-up time of your application. The trade-off is that there might be a slight hiccup as a particular shader is compiled for the first time. Once a shader has been compiled, the compiled binary is stored on the disk and recompilation is typically not required if the same application is run again.
	/// </summary>
	[FieldOffset(6)]
	public bool UseDynamicShaderCompilation = true;

	/// <summary>
	/// Specifies the sound system used to play embedded video sounds
	/// </summary>
    public enum VideoSoundSystem : int
    {
        SystemSound = 0,
        FMod,
        WWise,
        Default
    }
    
    [FieldOffset(8)]
    public VideoSoundSystem TheVideoSoundSystem = VideoSoundSystem.SystemSound;
    
	/// <summary>
	/// Currently IME (input method editor) is not implemented in the Unity Integration, so set this to false
	/// </summary>
    public enum InitIME:int
    {
        Yes = 0,
        No
    }
    [FieldOffset(12)]
    public bool IsInitIME = false;
    
	/// <summary>
	/// Used to enable AMP profiling. AMP is automatically disabled in a shipping build.
	/// </summary>
    public enum EnableAmpProfiling : int
    {
        Yes = 0,
        No
    }
    [FieldOffset(16)]
    public EnableAmpProfiling AmpProfiling;
	/// <summary>
	/// Used to enable progressive loading, which allows for loading a swf on a background thread. This can prevent application from becoming unresponsive while the flash file is being loaded. 
	/// </summary>
    public enum EnableProgressiveLoading : int
    {
        Yes = 0,
        No
    }
    [FieldOffset(20)]
    public EnableProgressiveLoading ProgLoading = EnableProgressiveLoading.No;
    
	/// <summary>
	/// Advanced Setting. Used to set the size and number of textures used in the dynamic font cache. Scaleform renderer rasterizes glyphs encountered while rendering text and stores the rasterized glyphs on a font cache. If there are more glyphs on a screen that can fit in the font cache, the remainder are rendered as a triangle mesh. A larger font cache leads to fewer updates however consumes more texture memory. The default parameters provided work well for most cases.  
	/// </summary>
    [StructLayout(LayoutKind.Explicit, Pack=4)]
    [Serializable]
    public class FontCacheConfig
    {
		[FieldOffset(0)]
        public int TextureHeight    = 1024;
		[FieldOffset(4)]
        public int TextureWidth     = 1024;
		[FieldOffset(8)]
        public int MaxNumTextures   = 1;
		[FieldOffset(12)]
        public int MaxSlotHeight    = 48;
    }
    
    [FieldOffset(24)]
    public FontCacheConfig TheFontCacheConfig = new FontCacheConfig();
    [FieldOffset(40)]
    public bool SetFontCacheParams = true;
	/// <summary>
	/// This enables dynamic shader compilation. Setting this flag to true can reduce startup times causing Scaleform to compile shaders as they are encountered instead of compiling all shaders at startup. Once a shader is compiled, the binary is stored on the disc and can usually be loaded directly the next time the same application is run again. Therefore, the shader compilation step is only performed the first time the application is run. 
	/// </summary>
    [FieldOffset(41)]
    public bool IsEnableDynamicCache = true;

    [StructLayout(LayoutKind.Sequential)]
    [Serializable]
    public class FontPackParams
    {
        public int NominalSize  = 32;
        public int PadPixels    = 2;
        public int TextureWidth = 512;
        public int TextureHeight= 512;
    }
    
    [FieldOffset(44)]
    public FontPackParams TheFontPackParams = new FontPackParams();
    [FieldOffset(60)]
    public bool SetFontPackParams = true;
    [FieldOffset(64)]
    public int    GlyphCountLimit = 1000;
	/// <summary>
	/// Initial volume for sound
	/// </summary>
    [FieldOffset(68)]
    public float SoundVolume = 10;
	/// <summary>
	/// If true, the sound renderer is initialized to mute
	/// </summary>
    [FieldOffset(72)]
    public bool IsMute = false;
	/// <summary>
	/// Specifies the image formats for which loaders are created
	/// </summary>
    [FieldOffset(76)]
	public UInt32 TheImageFormat = ImageFormat.IMG_Default;
		
	[StructLayout(LayoutKind.Sequential)]
	[Serializable]
	public struct ToleranceParams
	{
        public float Epsilon;
        public float CurveTolerance;
        public float CollinearityTolerance;
        public float IntersectionEpsilon;
        public float FillLowerScale;
        public float FillUpperScale;
        public float FillAliasedLowerScale;
        public float FillAliasedUpperScale;
        public float StrokeLowerScale;
        public float StrokeUpperScale;
        public float HintedStrokeLowerScale;
        public float HintedStrokeUpperScale;
        public float Scale9LowerScale;
        public float Scale9UpperScale;
        public float EdgeAAScale;
        public float MorphTolerance;
        public float MinDet3D;
        public float MinScale3D;
	}
		
	[FieldOffset(80)]		
	public ToleranceParams TheToleranceParams;		
		
    /// <summary>
    /// If true, uses the system-specific font provider in the case fonts cannot be located.
    /// </summary>
    [FieldOffset(152)]
    public bool UseSystemFontProvider = true;

	/// <summary>
	/// Used to check if the data structure was marshalled correctly across managed-unmanaged code. 
	/// </summary>
	[FieldOffset(156)]
	public UInt32 Sentinal = SFSentinal.Sentinal;
	
    
    public void Print()
    {
        Trace.Log("ASVersion:");
        switch(TheASVersion)
        {
            case ASVersion.AS2:
                Trace.Log("AS2");
                break;
            case ASVersion.AS3:
                Trace.Log("AS3");
                break;
            case ASVersion.Both:
                Trace.Log("Both");
                break;
            default:
                Trace.Log("Unknown");
                break;
        }
        
        // Console.Write("SetFontPackParams = " + SetFontPackParams);
        // Console.Write(TheFontCacheConfig.TextureHeight); 
        
        // Complete the rest..
    }
}


// Need struct definition in order to prevent AOT compilation error on iOS
[StructLayout(LayoutKind.Explicit, Pack = 4)]
[Serializable]
public struct SFInitParams2
{
    public SFInitParams2(int dummy)
    {
        TheASVersion			= ASVersion.AS3;
        InitVideo				= false;
        InitSound				= true;
        TheVideoSoundSystem     = VideoSoundSystem.SystemSound;
        IsInitIME               = false;
        AmpProfiling			= EnableAmpProfiling.Yes;
        IsSetFontCacheParams    = true;
        IsEnableDynamicCache    = true;
        SetFontPackParams		= true;
        IsInitIME               = false;
        ProgLoading				= EnableProgressiveLoading.No;
        
		UseDynamicShaderCompilation = true;

        TheFontCacheConfig.TextureHeight	= 1024;
        TheFontCacheConfig.TextureWidth		= 1024;
        TheFontCacheConfig.MaxNumTextures	= 1;
        TheFontCacheConfig.MaxSlotHeight	= 48;
        
        TheFontPackParams.NominalSize		= 32;
        TheFontPackParams.PadPixels			= 2;
        TheFontPackParams.TextureWidth		= 512;
        TheFontPackParams.TextureHeight		= 512;
        
        SetFontPackParams   = true;
        GlyphCountLimit     = 1000;
        SoundVolume         = 10;
        IsMute              = false;
		TheImageFormat		= ImageFormat.IMG_Default;
			
        TheToleranceParams.Epsilon                  = 1e-5f;
        TheToleranceParams.CurveTolerance           = 1.0f;
        TheToleranceParams.CollinearityTolerance    = 1.0f;
        TheToleranceParams.IntersectionEpsilon      = 1e-3f;
        TheToleranceParams.FillLowerScale           = 0.707f;
        TheToleranceParams.FillUpperScale           = 1.414f;
        TheToleranceParams.FillAliasedLowerScale    = 0.5f;
        TheToleranceParams.FillAliasedUpperScale    = 2.0f;
        TheToleranceParams.StrokeLowerScale         = 0.99f;
        TheToleranceParams.StrokeUpperScale         = 1.01f;
        TheToleranceParams.HintedStrokeLowerScale   = 0.999f;
        TheToleranceParams.HintedStrokeUpperScale   = 1.001f;
        TheToleranceParams.Scale9LowerScale         = 0.995f;
        TheToleranceParams.Scale9UpperScale         = 1.005f;
        TheToleranceParams.EdgeAAScale              = 0.95f;
        TheToleranceParams.MorphTolerance           = 0.000f;
        TheToleranceParams.MinDet3D                 = 0.001f;
        TheToleranceParams.MinScale3D               = 0.05f;
			
		UseSystemFontProvider 	= true;
		Sentinal            	= SFSentinal.Sentinal;
    }
    
    public SFInitParams2(SFInitParams initParams)
    {
        TheASVersion						= (ASVersion)initParams.TheASVersion;
        InitVideo							= initParams.InitVideo;
        InitSound							= initParams.InitSound;
        TheVideoSoundSystem					= (VideoSoundSystem) initParams.TheVideoSoundSystem;
        IsInitIME							= initParams.IsInitIME;
        AmpProfiling						= (EnableAmpProfiling) initParams.AmpProfiling;
        IsSetFontCacheParams				= initParams.SetFontCacheParams;
        IsEnableDynamicCache				= initParams.IsEnableDynamicCache;
        SetFontPackParams					= initParams.SetFontPackParams;
        IsInitIME							= initParams.IsInitIME;
		UseDynamicShaderCompilation			= initParams.UseDynamicShaderCompilation;
        ProgLoading							= (EnableProgressiveLoading) initParams.ProgLoading;
        
        TheFontCacheConfig.TextureHeight    = initParams.TheFontCacheConfig.TextureHeight;
        TheFontCacheConfig.TextureWidth		= initParams.TheFontCacheConfig.TextureWidth;
        TheFontCacheConfig.MaxNumTextures	= initParams.TheFontCacheConfig.MaxNumTextures;
        TheFontCacheConfig.MaxSlotHeight	= initParams.TheFontCacheConfig.MaxSlotHeight;

        TheFontPackParams.NominalSize		= initParams.TheFontPackParams.NominalSize;
        TheFontPackParams.PadPixels			= initParams.TheFontPackParams.PadPixels;
        TheFontPackParams.TextureWidth		= initParams.TheFontPackParams.TextureWidth;
        TheFontPackParams.TextureHeight		= initParams.TheFontPackParams.TextureHeight;

        SetFontPackParams   = initParams.SetFontCacheParams;
        GlyphCountLimit     = initParams.GlyphCountLimit;
        SoundVolume         = initParams.SoundVolume;
        IsMute              = initParams.IsMute;
		TheImageFormat		= initParams.TheImageFormat;
			
        TheToleranceParams.Epsilon                  = initParams.TheToleranceParams.Epsilon;
        TheToleranceParams.CurveTolerance           = initParams.TheToleranceParams.CurveTolerance;
        TheToleranceParams.CollinearityTolerance    = initParams.TheToleranceParams.CollinearityTolerance;
        TheToleranceParams.IntersectionEpsilon      = initParams.TheToleranceParams.IntersectionEpsilon;
        TheToleranceParams.FillLowerScale           = initParams.TheToleranceParams.FillLowerScale;
        TheToleranceParams.FillUpperScale           = initParams.TheToleranceParams.FillUpperScale;
        TheToleranceParams.FillAliasedLowerScale    = initParams.TheToleranceParams.FillAliasedLowerScale;
        TheToleranceParams.FillAliasedUpperScale    = initParams.TheToleranceParams.FillAliasedUpperScale;
        TheToleranceParams.StrokeLowerScale         = initParams.TheToleranceParams.StrokeLowerScale;
        TheToleranceParams.StrokeUpperScale         = initParams.TheToleranceParams.StrokeUpperScale;
        TheToleranceParams.HintedStrokeLowerScale   = initParams.TheToleranceParams.HintedStrokeLowerScale;
        TheToleranceParams.HintedStrokeUpperScale   = initParams.TheToleranceParams.HintedStrokeUpperScale;
        TheToleranceParams.Scale9LowerScale         = initParams.TheToleranceParams.Scale9LowerScale;
        TheToleranceParams.Scale9UpperScale         = initParams.TheToleranceParams.Scale9UpperScale;
        TheToleranceParams.EdgeAAScale              = initParams.TheToleranceParams.EdgeAAScale;
        TheToleranceParams.MorphTolerance           = initParams.TheToleranceParams.MorphTolerance;
        TheToleranceParams.MinDet3D                 = initParams.TheToleranceParams.MinDet3D;
        TheToleranceParams.MinScale3D               = initParams.TheToleranceParams.MinScale3D;

		UseSystemFontProvider = initParams.UseSystemFontProvider;
		Sentinal              = SFSentinal.Sentinal;
    }
    
    public enum ASVersion : int
    {
        AS2 = 0,
        AS3,
        Both
    }
    
    [FieldOffset(0)]
    public ASVersion TheASVersion;
    [FieldOffset(4)]
    public bool InitVideo;
    [FieldOffset(5)]
    public bool InitSound;
	[FieldOffset(6)]
	public bool UseDynamicShaderCompilation;

    public enum VideoSoundSystem : int
    {
        SystemSound = 0,
        FMod,
        WWise,
        Default
    }
    [FieldOffset(8)]
    public VideoSoundSystem TheVideoSoundSystem;

    public enum InitIME : int
    {
        Yes = 0,
        No
    }
    [FieldOffset(12)]
    public bool IsInitIME;

    public enum EnableAmpProfiling : int
    {
        Yes = 0,
        No
    }
    
    [FieldOffset(16)]
    public EnableAmpProfiling AmpProfiling; 
    public enum EnableProgressiveLoading : int
    {
        Yes = 0,
        No
    }
    [FieldOffset(20)]
    public EnableProgressiveLoading ProgLoading;

	[StructLayout(LayoutKind.Explicit, Pack = 4)]
    [Serializable]
    public struct FontCacheConfig
    {
		[FieldOffset(0)]
        public int TextureHeight;
		[FieldOffset(4)]
        public int TextureWidth;
		[FieldOffset(8)]
        public int MaxNumTextures;
		[FieldOffset(12)]
        public int MaxSlotHeight;
    }
    
    [FieldOffset(24)]
    public FontCacheConfig TheFontCacheConfig;
    [FieldOffset(40)]
    public bool IsSetFontCacheParams;
    [FieldOffset(41)]
    public bool IsEnableDynamicCache;

    [StructLayout(LayoutKind.Sequential)]
    [Serializable]
    public struct FontPackParams
    {
        public int NominalSize;
        public int PadPixels;
        public int TextureWidth;
        public int TextureHeight;
    }
    
    [FieldOffset(44)]
    public FontPackParams TheFontPackParams;
    [FieldOffset(60)]
    public bool SetFontPackParams;
    [FieldOffset(64)]
    public int GlyphCountLimit;
    [FieldOffset(68)]
    public float SoundVolume;
    [FieldOffset(72)]
    public bool IsMute;
    [FieldOffset(76)]
	public UInt32 TheImageFormat;	
		
		
	[StructLayout(LayoutKind.Sequential)]
	[Serializable]
	public struct ToleranceParams
	{
        public float Epsilon;
        public float CurveTolerance;
        public float CollinearityTolerance;
        public float IntersectionEpsilon;
        public float FillLowerScale;
        public float FillUpperScale;
        public float FillAliasedLowerScale;
        public float FillAliasedUpperScale;
        public float StrokeLowerScale;
        public float StrokeUpperScale;
        public float HintedStrokeLowerScale;
        public float HintedStrokeUpperScale;
        public float Scale9LowerScale;
        public float Scale9UpperScale;
        public float EdgeAAScale;
        public float MorphTolerance;
        public float MinDet3D;
        public float MinScale3D;		
	}		
		
	[FieldOffset(80)]		
	public ToleranceParams TheToleranceParams;
		
	[FieldOffset(152)]
	public bool UseSystemFontProvider;	
		
	[FieldOffset(156)]
	public int Sentinal;
		
    public void Print()
    {
        Trace.Log("ASVersion:");
        switch (TheASVersion)
        {
            case ASVersion.AS2:
                Trace.Log("AS2");
                break;
            case ASVersion.AS3:
                Trace.Log("AS3");
                break;
            case ASVersion.Both:
                Trace.Log("Both");
                break;
            default:
                Trace.Log("Unknown");
                break;
        }

        // Console.Write("SetFontPackParams = " + SetFontPackParams);
        // Console.Write(TheFontCacheConfig.TextureHeight);

        // Complete the rest..
    }
}
}