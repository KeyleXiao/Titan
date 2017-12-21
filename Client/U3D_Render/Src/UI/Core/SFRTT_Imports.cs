/**********************************************************************

Filename    :	SFRTT_Imports.cs
Content     :	
Created     :   
Authors     :   Ankur Mohan

Copyright   :   Copyright 2013 Autodesk, Inc. All Rights reserved.

Use of this software is subject to the terms of the Autodesk license
agreement provided at the time of installation or download, or which
otherwise accompanies this software in either electronic or hard copy form.
 
***********************************************************************/

using UnityEngine;
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
public partial class SFRTT : MonoBehaviour
{
#if (UNITY_STANDALONE || UNITY_EDITOR || UNITY_METRO) && !UNITY_WP8
	[DllImport("libgfxunity3d")]
	public static extern bool SF_WasDeviceReset();

	[DllImport("libgfxunity3d")]
    public static extern void SF_ClearDeviceReset();

#elif UNITY_IPHONE
	[DllImport("__Internal")]
	protected static extern bool SF_WasDeviceReset();

	[DllImport("__Internal")]
	protected static extern void SF_ClearDeviceReset();

#elif UNITY_ANDROID
	[DllImport("gfxunity3d")]
	protected static extern bool SF_WasDeviceReset();

	[DllImport("gfxunity3d")]
	protected static extern void SF_ClearDeviceReset();

#elif UNITY_WP8
	public delegate bool SF_WasDeviceReset();
	public static SF_WasDeviceReset sf_wasDeviceReset;
	public void SetSF_WasDeviceReset(SF_WasDeviceReset func){sf_wasDeviceReset = func;}

	public delegate void SF_ClearDeviceReset();
	public static SF_ClearDeviceReset sf_clearDeviceReset;
	public void SetSF_ClearDeviceReset(SF_ClearDeviceReset func){sf_clearDeviceReset = func;}

#endif
}

} // namespace Scaleform

