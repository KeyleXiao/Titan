/**********************************************************************

Filename    :	SFValue_Imports.cs
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
public partial class Value: System.Object
{
#if (UNITY_STANDALONE || UNITY_EDITOR || UNITY_METRO) && !UNITY_WP8
	[DllImport("libgfxunity3d")]
	protected static extern IntPtr SF_AllocateBoolean(Boolean bval, long movieID);

	[DllImport("libgfxunity3d")]
	protected static extern IntPtr SF_AllocateDouble(Double nval, long movieID);

	[DllImport("libgfxunity3d")]
	protected static extern IntPtr SF_AllocateString(String sval, long movieID);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_AttachMovie(Value target, [Out] Value dest, String symbolName, String instanceName, Int32 depth);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_ClearElements(Value target);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_CreateEmptyMovieClip(Value target, [Out] Value dest, String instanceName, Int32 depth);

	[DllImport("libgfxunity3d")]
	protected static extern IntPtr SF_CreateNewValue(IntPtr src, long movieID);

	[DllImport("libgfxunity3d")]
	public static extern void SF_DecrementValRefCount(IntPtr val);

	[DllImport("libgfxunity3d")]
	protected static extern Boolean SF_Equals(Value val1, Value val2);

	[DllImport("libgfxunity3d")]
	protected static extern int SF_GetArraySize(Value target);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_GetBool(Value val);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_GetColorTransform(Value target, [Out] SFCxForm cxform);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_GetDisplayInfo(Value target, [Out] SFDisplayInfo dinfo, Int32 size);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_GetDisplayMatrix(Value target, [Out] SFDisplayMatrix dmat, Int32 size);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_GetElement(Value target, UInt32 idx, [Out] Value dest);

	[DllImport("libgfxunity3d")]
	protected static extern Int32 SF_GetInt(Value val);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_GetMember(Value target, String elemName, [Out] Value dest);

	[DllImport("libgfxunity3d")]
	protected static extern double SF_GetNumber(Value val);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_GetObject(Value val, IntPtr newval);

	[DllImport("libgfxunity3d")]
	protected static extern IntPtr SF_GetString(Value val);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_GetText(Value target, [Out] Value dst);

	[DllImport("libgfxunity3d")]
	protected static extern UInt32 SF_GetUInt(Value val);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_GotoAndPlay(Value target, Int32 frameNum);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_GotoAndPlayFrame(Value target, String frameName);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_GotoAndStop(Value target, Int32 frameNum);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_GotoAndStopFrame(Value target, String frameName);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_Invoke2(Value val, String methodName, int numVal, IntPtr iptr, [Out] Value retVal);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_PopBack(Value target, [Out] Value dest);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_PushBack(Value target, Value dest);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_RemoveElement(Value target, UInt32 idx);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_SetArraySize(Value target, UInt32 sz);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_SetBool(Value val, bool bval);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_SetColorTransform(Value target, SFCxForm cxform);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_SetDisplayInfo(Value target, SFDisplayInfo dinfo, Int32 size);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_SetDisplayMatrix(Value target, SFDisplayMatrix dmat, Int32 size);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_SetElement(Value target, UInt32 idx, Value val);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_SetInt(Value val, Int32 ival);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_SetMember(Value target, String elemName, Value val);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_SetNumber(Value val, double num);

	[DllImport("libgfxunity3d")]
	protected static extern bool SF_SetText(Value target, String str);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_SetString(Value val, IntPtr str);

	[DllImport("libgfxunity3d")]
	protected static extern void SF_SetUInt(Value val, UInt32 uival);

#elif UNITY_IPHONE
	[DllImport("__Internal")]
	protected static extern IntPtr SF_AllocateBoolean(Boolean bval, long movieID);

	[DllImport("__Internal")]
	protected static extern IntPtr SF_AllocateDouble(Double nval, long movieID);

	[DllImport("__Internal")]
	protected static extern IntPtr SF_AllocateString(String sval, long movieID);

	[DllImport("__Internal")]
	protected static extern bool SF_AttachMovie(Value target, [Out] Value dest, String symbolName, String instanceName, Int32 depth);

	[DllImport("__Internal")]
	protected static extern bool SF_ClearElements(Value target);

	[DllImport("__Internal")]
	protected static extern bool SF_CreateEmptyMovieClip(Value target, [Out] Value dest, String instanceName, Int32 depth);

	[DllImport("__Internal")]
	protected static extern IntPtr SF_CreateNewValue(IntPtr src, long movieID);

	[DllImport("__Internal")]
	public static extern void SF_DecrementValRefCount(IntPtr val);

	[DllImport("__Internal")]
	protected static extern Boolean SF_Equals(Value val1, Value val2);

	[DllImport("__Internal")]
	protected static extern int SF_GetArraySize(Value target);

	[DllImport("__Internal")]
	protected static extern bool SF_GetBool(Value val);

	[DllImport("__Internal")]
	protected static extern bool SF_GetColorTransform(Value target, [Out] SFCxForm cxform);

	[DllImport("__Internal")]
	protected static extern bool SF_GetDisplayInfo(Value target, [Out] SFDisplayInfo dinfo, Int32 size);

	[DllImport("__Internal")]
	protected static extern bool SF_GetDisplayMatrix(Value target, [Out] SFDisplayMatrix dmat, Int32 size);

	[DllImport("__Internal")]
	protected static extern bool SF_GetElement(Value target, UInt32 idx, [Out] Value dest);

	[DllImport("__Internal")]
	protected static extern Int32 SF_GetInt(Value val);

	[DllImport("__Internal")]
	protected static extern bool SF_GetMember(Value target, String elemName, [Out] Value dest);

	[DllImport("__Internal")]
	protected static extern double SF_GetNumber(Value val);

	[DllImport("__Internal")]
	protected static extern void SF_GetObject(Value val, IntPtr newval);

	[DllImport("__Internal")]
	protected static extern IntPtr SF_GetString(Value val);

	[DllImport("__Internal")]
	protected static extern bool SF_GetText(Value target, [Out] Value dst);

	[DllImport("__Internal")]
	protected static extern UInt32 SF_GetUInt(Value val);

	[DllImport("__Internal")]
	protected static extern bool SF_GotoAndPlay(Value target, Int32 frameNum);

	[DllImport("__Internal")]
	protected static extern bool SF_GotoAndPlayFrame(Value target, String frameName);

	[DllImport("__Internal")]
	protected static extern bool SF_GotoAndStop(Value target, Int32 frameNum);

	[DllImport("__Internal")]
	protected static extern bool SF_GotoAndStopFrame(Value target, String frameName);

	[DllImport("__Internal")]
	protected static extern bool SF_Invoke2(Value val, String methodName, int numVal, IntPtr iptr, [Out] Value retVal);

	[DllImport("__Internal")]
	protected static extern bool SF_PopBack(Value target, [Out] Value dest);

	[DllImport("__Internal")]
	protected static extern bool SF_PushBack(Value target, Value dest);

	[DllImport("__Internal")]
	protected static extern bool SF_RemoveElement(Value target, UInt32 idx);

	[DllImport("__Internal")]
	protected static extern bool SF_SetArraySize(Value target, UInt32 sz);

	[DllImport("__Internal")]
	protected static extern void SF_SetBool(Value val, bool bval);

	[DllImport("__Internal")]
	protected static extern bool SF_SetColorTransform(Value target, SFCxForm cxform);

	[DllImport("__Internal")]
	protected static extern bool SF_SetDisplayInfo(Value target, SFDisplayInfo dinfo, Int32 size);

	[DllImport("__Internal")]
	protected static extern bool SF_SetDisplayMatrix(Value target, SFDisplayMatrix dmat, Int32 size);

	[DllImport("__Internal")]
	protected static extern bool SF_SetElement(Value target, UInt32 idx, Value val);

	[DllImport("__Internal")]
	protected static extern void SF_SetInt(Value val, Int32 ival);

	[DllImport("__Internal")]
	protected static extern bool SF_SetMember(Value target, String elemName, Value val);

	[DllImport("__Internal")]
	protected static extern void SF_SetNumber(Value val, double num);

	[DllImport("__Internal")]
	protected static extern bool SF_SetText(Value target, String str);

	[DllImport("__Internal")]
	protected static extern void SF_SetString(Value val, IntPtr str);

	[DllImport("__Internal")]
	protected static extern void SF_SetUInt(Value val, UInt32 uival);

#elif UNITY_ANDROID
	[DllImport("gfxunity3d")]
	protected static extern IntPtr SF_AllocateBoolean(Boolean bval, long movieID);

	[DllImport("gfxunity3d")]
	protected static extern IntPtr SF_AllocateDouble(Double nval, long movieID);

	[DllImport("gfxunity3d")]
	protected static extern IntPtr SF_AllocateString(String sval, long movieID);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_AttachMovie(Value target, [Out] Value dest, String symbolName, String instanceName, Int32 depth);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_ClearElements(Value target);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_CreateEmptyMovieClip(Value target, [Out] Value dest, String instanceName, Int32 depth);

	[DllImport("gfxunity3d")]
	protected static extern IntPtr SF_CreateNewValue(IntPtr src, long movieID);

	[DllImport("gfxunity3d")]
	public static extern void SF_DecrementValRefCount(IntPtr val);

	[DllImport("gfxunity3d")]
	protected static extern Boolean SF_Equals(Value val1, Value val2);

	[DllImport("gfxunity3d")]
	protected static extern int SF_GetArraySize(Value target);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_GetBool(Value val);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_GetColorTransform(Value target, [Out] SFCxForm cxform);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_GetDisplayInfo(Value target, [Out] SFDisplayInfo dinfo, Int32 size);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_GetDisplayMatrix(Value target, [Out] SFDisplayMatrix dmat, Int32 size);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_GetElement(Value target, UInt32 idx, [Out] Value dest);

	[DllImport("gfxunity3d")]
	protected static extern Int32 SF_GetInt(Value val);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_GetMember(Value target, String elemName, [Out] Value dest);

	[DllImport("gfxunity3d")]
	protected static extern double SF_GetNumber(Value val);

	[DllImport("gfxunity3d")]
	protected static extern void SF_GetObject(Value val, IntPtr newval);

	[DllImport("gfxunity3d")]
	protected static extern IntPtr SF_GetString(Value val);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_GetText(Value target, [Out] Value dst);

	[DllImport("gfxunity3d")]
	protected static extern UInt32 SF_GetUInt(Value val);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_GotoAndPlay(Value target, Int32 frameNum);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_GotoAndPlayFrame(Value target, String frameName);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_GotoAndStop(Value target, Int32 frameNum);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_GotoAndStopFrame(Value target, String frameName);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_Invoke2(Value val, String methodName, int numVal, IntPtr iptr, [Out] Value retVal);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_PopBack(Value target, [Out] Value dest);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_PushBack(Value target, Value dest);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_RemoveElement(Value target, UInt32 idx);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_SetArraySize(Value target, UInt32 sz);

	[DllImport("gfxunity3d")]
	protected static extern void SF_SetBool(Value val, bool bval);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_SetColorTransform(Value target, SFCxForm cxform);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_SetDisplayInfo(Value target, SFDisplayInfo dinfo, Int32 size);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_SetDisplayMatrix(Value target, SFDisplayMatrix dmat, Int32 size);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_SetElement(Value target, UInt32 idx, Value val);

	[DllImport("gfxunity3d")]
	protected static extern void SF_SetInt(Value val, Int32 ival);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_SetMember(Value target, String elemName, Value val);

	[DllImport("gfxunity3d")]
	protected static extern void SF_SetNumber(Value val, double num);

	[DllImport("gfxunity3d")]
	protected static extern bool SF_SetText(Value target, String str);

	[DllImport("gfxunity3d")]
	protected static extern void SF_SetString(Value val, IntPtr str);

	[DllImport("gfxunity3d")]
	protected static extern void SF_SetUInt(Value val, UInt32 uival);

#elif UNITY_WP8
	public delegate IntPtr SF_AllocateBoolean(Boolean bval, long movieID);
	public static SF_AllocateBoolean sf_allocateBoolean;
	public void SetSF_AllocateBoolean(SF_AllocateBoolean func){sf_allocateBoolean = func;}

	public delegate IntPtr SF_AllocateDouble(Double nval, long movieID);
	public static SF_AllocateDouble sf_allocateDouble;
	public void SetSF_AllocateDouble(SF_AllocateDouble func){sf_allocateDouble = func;}

	public delegate IntPtr SF_AllocateString(String sval, long movieID);
	public static SF_AllocateString sf_allocateString;
	public void SetSF_AllocateString(SF_AllocateString func){sf_allocateString = func;}

	public delegate bool SF_AttachMovie(Value target, [Out] Value dest, String symbolName, String instanceName, Int32 depth);
	public static SF_AttachMovie sf_attachMovie;
	public void SetSF_AttachMovie(SF_AttachMovie func){sf_attachMovie = func;}

	public delegate bool SF_ClearElements(Value target);
	public static SF_ClearElements sf_clearElements;
	public void SetSF_ClearElements(SF_ClearElements func){sf_clearElements = func;}

	public delegate bool SF_CreateEmptyMovieClip(Value target, [Out] Value dest, String instanceName, Int32 depth);
	public static SF_CreateEmptyMovieClip sf_createEmptyMovieClip;
	public void SetSF_CreateEmptyMovieClip(SF_CreateEmptyMovieClip func){sf_createEmptyMovieClip = func;}

	public delegate IntPtr SF_CreateNewValue(IntPtr src, long movieID);
	public static SF_CreateNewValue sf_createNewValue;
	public void SetSF_CreateNewValue(SF_CreateNewValue func){sf_createNewValue = func;}

	public delegate void SF_DecrementValRefCount(IntPtr val);
	public static SF_DecrementValRefCount sf_decrementValRefCount;
	public void SetSF_DecrementValRefCount(SF_DecrementValRefCount func){sf_decrementValRefCount = func;}

	public delegate Boolean SF_Equals(Value val1, Value val2);
	public static SF_Equals sf_equals;
	public void SetSF_Equals(SF_Equals func){sf_equals = func;}

	public delegate int SF_GetArraySize(Value target);
	public static SF_GetArraySize sf_getArraySize;
	public void SetSF_GetArraySize(SF_GetArraySize func){sf_getArraySize = func;}

	public delegate bool SF_GetBool(Value val);
	public static SF_GetBool sf_getBool;
	public void SetSF_GetBool(SF_GetBool func){sf_getBool = func;}

	public delegate bool SF_GetColorTransform(Value target, [Out] SFCxForm cxform);
	public static SF_GetColorTransform sf_getColorTransform;
	public void SetSF_GetColorTransform(SF_GetColorTransform func){sf_getColorTransform = func;}

	public delegate bool SF_GetDisplayInfo(Value target, [Out] SFDisplayInfo dinfo, Int32 size);
	public static SF_GetDisplayInfo sf_getDisplayInfo;
	public void SetSF_GetDisplayInfo(SF_GetDisplayInfo func){sf_getDisplayInfo = func;}

	public delegate bool SF_GetDisplayMatrix(Value target, [Out] SFDisplayMatrix dmat, Int32 size);
	public static SF_GetDisplayMatrix sf_getDisplayMatrix;
	public void SetSF_GetDisplayMatrix(SF_GetDisplayMatrix func){sf_getDisplayMatrix = func;}

	public delegate bool SF_GetElement(Value target, UInt32 idx, [Out] Value dest);
	public static SF_GetElement sf_getElement;
	public void SetSF_GetElement(SF_GetElement func){sf_getElement = func;}

	public delegate Int32 SF_GetInt(Value val);
	public static SF_GetInt sf_getInt;
	public void SetSF_GetInt(SF_GetInt func){sf_getInt = func;}

	public delegate bool SF_GetMember(Value target, String elemName, [Out] Value dest);
	public static SF_GetMember sf_getMember;
	public void SetSF_GetMember(SF_GetMember func){sf_getMember = func;}

	public delegate double SF_GetNumber(Value val);
	public static SF_GetNumber sf_getNumber;
	public void SetSF_GetNumber(SF_GetNumber func){sf_getNumber = func;}

	public delegate void SF_GetObject(Value val, IntPtr newval);
	public static SF_GetObject sf_getObject;
	public void SetSF_GetObject(SF_GetObject func){sf_getObject = func;}

	public delegate IntPtr SF_GetString(Value val);
	public static SF_GetString sf_getString;
	public void SetSF_GetString(SF_GetString func){sf_getString = func;}

	public delegate bool SF_GetText(Value target, [Out] Value dst);
	public static SF_GetText sf_getText;
	public void SetSF_GetText(SF_GetText func){sf_getText = func;}

	public delegate UInt32 SF_GetUInt(Value val);
	public static SF_GetUInt sf_getUInt;
	public void SetSF_GetUInt(SF_GetUInt func){sf_getUInt = func;}

	public delegate bool SF_GotoAndPlay(Value target, Int32 frameNum);
	public static SF_GotoAndPlay sf_gotoAndPlay;
	public void SetSF_GotoAndPlay(SF_GotoAndPlay func){sf_gotoAndPlay = func;}

	public delegate bool SF_GotoAndPlayFrame(Value target, String frameName);
	public static SF_GotoAndPlayFrame sf_gotoAndPlayFrame;
	public void SetSF_GotoAndPlayFrame(SF_GotoAndPlayFrame func){sf_gotoAndPlayFrame = func;}

	public delegate bool SF_GotoAndStop(Value target, Int32 frameNum);
	public static SF_GotoAndStop sf_gotoAndStop;
	public void SetSF_GotoAndStop(SF_GotoAndStop func){sf_gotoAndStop = func;}

	public delegate bool SF_GotoAndStopFrame(Value target, String frameName);
	public static SF_GotoAndStopFrame sf_gotoAndStopFrame;
	public void SetSF_GotoAndStopFrame(SF_GotoAndStopFrame func){sf_gotoAndStopFrame = func;}

	public delegate bool SF_Invoke2(Value val, String methodName, int numVal, IntPtr iptr, [Out] Value retVal);
	public static SF_Invoke2 sf_invoke2;
	public void SetSF_Invoke2(SF_Invoke2 func){sf_invoke2 = func;}

	public delegate bool SF_PopBack(Value target, [Out] Value dest);
	public static SF_PopBack sf_popBack;
	public void SetSF_PopBack(SF_PopBack func){sf_popBack = func;}

	public delegate bool SF_PushBack(Value target, Value dest);
	public static SF_PushBack sf_pushBack;
	public void SetSF_PushBack(SF_PushBack func){sf_pushBack = func;}

	public delegate bool SF_RemoveElement(Value target, UInt32 idx);
	public static SF_RemoveElement sf_removeElement;
	public void SetSF_RemoveElement(SF_RemoveElement func){sf_removeElement = func;}

	public delegate bool SF_SetArraySize(Value target, UInt32 sz);
	public static SF_SetArraySize sf_setArraySize;
	public void SetSF_SetArraySize(SF_SetArraySize func){sf_setArraySize = func;}

	public delegate void SF_SetBool(Value val, bool bval);
	public static SF_SetBool sf_setBool;
	public void SetSF_SetBool(SF_SetBool func){sf_setBool = func;}

	public delegate bool SF_SetColorTransform(Value target, SFCxForm cxform);
	public static SF_SetColorTransform sf_setColorTransform;
	public void SetSF_SetColorTransform(SF_SetColorTransform func){sf_setColorTransform = func;}

	public delegate bool SF_SetDisplayInfo(Value target, SFDisplayInfo dinfo, Int32 size);
	public static SF_SetDisplayInfo sf_setDisplayInfo;
	public void SetSF_SetDisplayInfo(SF_SetDisplayInfo func){sf_setDisplayInfo = func;}

	public delegate bool SF_SetDisplayMatrix(Value target, SFDisplayMatrix dmat, Int32 size);
	public static SF_SetDisplayMatrix sf_setDisplayMatrix;
	public void SetSF_SetDisplayMatrix(SF_SetDisplayMatrix func){sf_setDisplayMatrix = func;}

	public delegate bool SF_SetElement(Value target, UInt32 idx, Value val);
	public static SF_SetElement sf_setElement;
	public void SetSF_SetElement(SF_SetElement func){sf_setElement = func;}

	public delegate void SF_SetInt(Value val, Int32 ival);
	public static SF_SetInt sf_setInt;
	public void SetSF_SetInt(SF_SetInt func){sf_setInt = func;}

	public delegate bool SF_SetMember(Value target, String elemName, Value val);
	public static SF_SetMember sf_setMember;
	public void SetSF_SetMember(SF_SetMember func){sf_setMember = func;}

	public delegate void SF_SetNumber(Value val, double num);
	public static SF_SetNumber sf_setNumber;
	public void SetSF_SetNumber(SF_SetNumber func){sf_setNumber = func;}

	public delegate bool SF_SetText(Value target, String str);
	public static SF_SetText sf_setText;
	public void SetSF_SetText(SF_SetText func){sf_setText = func;}

	public delegate void SF_SetString(Value val, IntPtr str);
	public static SF_SetString sf_setString;
	public void SetSF_SetString(SF_SetString func){sf_setString = func;}

	public delegate void SF_SetUInt(Value val, UInt32 uival);
	public static SF_SetUInt sf_setUInt;
	public void SetSF_SetUInt(SF_SetUInt func){sf_setUInt = func;}

#endif
}

}//namespace Scaleform
