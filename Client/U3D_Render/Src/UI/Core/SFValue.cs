/**********************************************************************

Filename    :	SFValue.cs
Content     :	Wrapper for Value
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
using System.Diagnostics;
using System.Reflection;
using System.Runtime.InteropServices;

namespace Scaleform
{
    public struct ASObjectEx
    {
        public object[] values;
        public string[] names;
        public Type[] types;
    }

    /// <summary>
    /// SFCommand is used to process commands in the GFx runtime.
    /// </summary>
	[StructLayout(LayoutKind.Sequential)]
	public class SFCommand : System.Object
	{
		public long MovieID;
		public int ArgCount;
		public String MethodName;
	}

	public class SFDllName
	{
		public static string DllName = "libgfxunity3d";
	}


	/// <summary>
	/// The Value class provides a wrapper for the native Scaleform Value which can represent a number, string, ActionScript VM object, ActionScript array, functions etc. It provides a set of methods such as Get/SetMember to directly interact with Scaleform Values.
	/// </summary>
	public partial class Value : System.Object
	{
		/// <summary>
		/// Enums for different types that can be represented by a Value. These enums directly correspond to the enums on the C++ side.
		/// </summary>
		public enum ValueType : int
		{
			// ** Type identifiers
			// Basic types
			VT_Undefined = 0x00,
			VT_Null = 0x01,
			VT_Boolean = 0x02,
			VT_Int = 0x03,
			VT_UInt = 0x04,
			VT_Number = 0x05,
			VT_String = 0x06,

			// StringW can be passed as an argument type, but it will only be returned 
			// if VT_ConvertStringW was specified, as it is not a native type.
			VT_StringW = 0x07,  // wchar_t* string

			// ActionScript VM objects
			VT_Object = 0x08,
			VT_Array = 0x09,
			// Special type for stage instances (MovieClips, Buttons, TextFields)
			VT_DisplayObject = 0x0a,

		}

		enum ValueTypeControl : int
		{
			// ** Special Bit Flags
			// Explicit coercion to type requested 
			VTC_ConvertBit = 0x80,
			// Flag that denotes managed resources
			VTC_ManagedBit = 0x40,
			// Set if value was referencing managed value and the owner Movie/VM
			// was destroyed.
			VTC_OrphanedBit = 0x20,

			// ** Type mask
			VTC_TypeMask = VTC_ConvertBit | 0x0F,
		};

		private IntPtr pInternalData;
		private ValueType Type;
		private long MovieId;

		/// <summary>
		/// Basic Constructor to create a "Null" value.
		/// </summary>
		public Value()
		{
			MovieId = 0;
			pInternalData = IntPtr.Zero;
		}

		/// <summary>
		/// Returns the type of the value. A Value can be Boolean, Int, UInt, Number, String, Object, Array, DisplayObject, Undefined and Null. These types correspond to the C++ value types. You can check for a certain type by using the IsType (for example IsInt()) methods.
		/// </summary>
		/// <returns>Type of the Value.</returns>
		new public ValueType GetType() { return (ValueType)(((int)(Type)) & ((int)(ValueTypeControl.VTC_TypeMask))); }

		/// <summary>
		/// Checks if the value is undefined.
		/// </summary>
		/// <returns> Returns true if the value is undefined.</returns>
		public Boolean IsUndefined() { return GetType() == ValueType.VT_Undefined; }
		/// <summary>
		/// Checks if the value is null.
		/// </summary>
		/// <returns> Returns true if the value is null.</returns>
		public Boolean IsNull() { return GetType() == ValueType.VT_Null; }
		/// <summary>
		/// Checks if the value is a Boolean.
		/// </summary>
		/// <returns> Returns true if the value is a boolean.</returns>
		public Boolean IsBoolean() { return GetType() == ValueType.VT_Boolean; }
		/// <summary>
		/// Checks if the value is an Int.
		/// </summary>
		/// <returns> Returns true if the value is an Int.</returns>
		public Boolean IsInt() { return GetType() == ValueType.VT_Int; }
		/// <summary>
		/// Checks if the value is a UInt.
		/// </summary>
		/// <returns> Returns true if the value is a UInt.</returns>
		public Boolean IsUInt() { return GetType() == ValueType.VT_UInt; }
		/// <summary>
		/// Checks if the value is a Number.
		/// </summary>
		/// <returns> Returns true if the value is a Number.</returns>
		public Boolean IsNumber() { return GetType() == ValueType.VT_Number; }
		/// <summary>
		/// Checks if the value is numeric (Int, UInt, Number).
		/// </summary>
		/// <returns> Returns true if the value is numeric (Int, UInt, Number).</returns>
		public Boolean IsNumeric() { return IsInt() || IsUInt() || IsNumber(); }
		/// <summary>
		/// Checks if the value is a String.
		/// </summary>
		/// <returns> Returns true if the value is a String.</returns>
		public Boolean IsString() { return GetType() == ValueType.VT_String; }
		/// <summary>
		/// Checks if the value is a wide string.
		/// </summary>
		/// <returns> Returns true if the value is a wide string.</returns>
		public Boolean IsStringW() { return GetType() == ValueType.VT_StringW; }
		/// <summary>
		/// Checks if the value is an ActionScript object, array or display object.
		/// </summary>
		/// <returns> Returns true if the value is an ActionScript object, array or display object.</returns>
		public Boolean IsObject()
		{
			return (GetType() == ValueType.VT_Object || GetType() == ValueType.VT_Array ||
							GetType() == ValueType.VT_DisplayObject);
		}
		/// <summary>
		/// Checks if the value is an Array.
		/// </summary>
		/// <returns> Returns true if the value is an Array.</returns>
		public Boolean IsArray() { return GetType() == ValueType.VT_Array; }
		/// <summary>
		/// Checks if the value is a DisplayObject.
		/// </summary>
		/// <returns> Returns true if the value is a DisplayObject.</returns>
		public Boolean IsDisplayObject() { return GetType() == ValueType.VT_DisplayObject; }

		/// <summary>
		/// Checks if two values point to the same underlying Scaleform object. 
		/// </summary>
		/// <returns> Returns true the values are equal i.e., they point to the same underlying Scaleform object.</returns>
		public bool Equals(Value obj)
		{
			/*
				Can't do this: 
				if (obj.MovieId == MovieId && obj.pInternalData == pInternalData && obj.type == type) return true;
				return false;
				Need to compare the underlying GFx Data, which can only be done in C++.
			 */
#if UNITY_WP8
		return sf_equals(this, obj);
#else
			return SF_Equals(this, obj);
#endif
		}
		/// <summary>
		///  Creates a Value from a String.
		/// </summary>
		/// <param name="sval"> String.</param>
		/// <param name="movieID">movieID. MovieID is typically corresponds to a movie created using CreateMovie or sent by ActionScript during an ExternalInterface callback or </param>
		public Value(String sval, long movieID)
		{
#if UNITY_WP8
		pInternalData   = sf_allocateString(sval, movieID);
#else
			pInternalData = SF_AllocateString(sval, movieID);
#endif
			Type = ValueType.VT_String;
			MovieId = movieID;
		}
		/// <summary>
		///  Creates a Value from a Boolean.
		/// </summary>
		/// <param name="sval"> Boolean</param>
		/// <param name="movieID">movieID. MovieID is typically corresponds to a movie created using CreateMovie or sent by ActionScript during an ExternalInterface callback or </param>
		public Value(Boolean bval, long movieID)
		{
#if UNITY_WP8
		pInternalData   = sf_allocateBoolean(bval, movieID);
#else
			pInternalData = SF_AllocateBoolean(bval, movieID);
#endif
			Type = ValueType.VT_Boolean;
			MovieId = movieID;
		}
		/// <summary>
		///  Creates a Value from a Double.
		/// </summary>
		/// <param name="sval"> Double.</param>
		/// <param name="movieID">movieID. MovieID is typically corresponds to a movie created using CreateMovie or sent by ActionScript during an ExternalInterface callback or </param>
		public Value(Double nval, long movieID)
		{
#if UNITY_WP8
		pInternalData   = sf_allocateDouble(nval, movieID);
#else
			pInternalData = SF_AllocateDouble(nval, movieID);
#endif
			Type = ValueType.VT_Number;
			MovieId = movieID;
		}

		/// <summary>
		/// Copy Constructor.
		/// </summary>
		/// <param name="val"></param>
		public Value(Value val)
		{
			if (val == null)
			{
				return;
			}

			pInternalData = IntPtr.Zero;
			if (val.pInternalData != IntPtr.Zero)
			{
#if UNITY_WP8
			pInternalData    = sf_createNewValue(val.pInternalData, val.MovieId);
#else
				pInternalData = SF_CreateNewValue(val.pInternalData, val.MovieId);
#endif
			}

			Type = val.Type;
			MovieId = val.MovieId;
		}

		/// <summary>
		/// Returns true if a value is "valid" i.e., the internal data pointer is not null and the value is not undefined. It's good to check for validity before calling Direct Access API functions such as Get/SetMember on a Value.
		/// </summary>
		/// <returns> True if a Value is valid.</returns>

		public bool IsValid()
		{
			if (pInternalData == IntPtr.Zero) return false;
			if (IsUndefined()) return false;
			return true;
		}

		// Implicit casts..
		/// <summary>
		/// Converts a Value to a String.
		/// </summary>
		/// <param name="instance"></param>
		/// <returns>String.</returns>
		public static implicit operator String(Value instance)
		{
			//implicit cast logic
			return instance.GetString();
		}

		/// <summary>
		/// Converts a Value to an Int.
		/// </summary>
		/// <param name="instance"></param>
		/// <returns>Int</returns>
		public static implicit operator Int32(Value instance)
		{
			//implicit cast logic
			return instance.GetInt();
		}

		/// <summary>
		/// Converts a Value to an UInt.
		/// </summary>
		/// <param name="instance"></param>
		/// <returns>UInt</returns>
		public static implicit operator UInt32(Value instance)
		{
			//implicit cast logic
			return instance.GetUInt();
		}

		/// <summary>
		/// converts a Value to Double
		/// </summary>
		/// <param name="instance"></param>
		/// <returns>Double</returns>
		public static implicit operator Double(Value instance)
		{
			//implicit cast logic
			return instance.GetNumber();
		}

		/// <summary>
		/// Converts a Value to Boolean.
		/// </summary>
		/// <param name="instance"></param>
		/// <returns>Boolean</returns>
		public static implicit operator Boolean(Value instance)
		{
			//implicit cast logic
			return instance.GetBool();
		}

		/// <summary>
		/// Prints the Type and value. For example, if the Value represents the number 5, ToString will print "Number: 5.0"
		/// </summary>
		/// <returns></returns>
		public override string ToString()
		{
			if (IsBoolean()) return "Boolean: " + GetBool() + "\n";
			if (IsString()) return "String: " + GetString() + "\n";
			if (IsUInt()) return "UInt: " + GetUInt() + "\n";
			if (IsInt()) return "Int: " + GetInt() + "\n";
			if (IsNumber()) return "Number: " + GetNumber() + "\n";
			if (IsObject()) return "Object" + "\n";
			return "Unknown";
		}
		/// <summary>
		/// Sets the InternalData member, which represents a pointer to the underlying C++ value. 
		/// </summary>
		/// <param name="internalData"></param>
		public void SetInternalData(IntPtr internalData) { pInternalData = internalData; }

		/// <summary>
		/// Sets the MovieId which corresponds to the Movie that this Value belongs to.
		/// </summary>
		/// <param name="movieId">MovieId</param>
		public void SetMovieId(long movieId) { MovieId = movieId; }

		/// <summary>
		/// Sets the type of the Value.
		/// </summary>
		/// <param name="vtype"></param>
		public void SetValueType(ValueType vtype) { Type = vtype; }

		/// <summary>
		/// Returns an integer representation of the C++ value pointed to by this Value. Mainly used by other classes in the Integration, not meant to be used by application code.
		/// </summary>
		/// <returns></returns>
		public IntPtr GetInternalData() { return pInternalData; }

		/// <summary>
		/// Returns the MovieId.
		/// </summary>
		/// <returns>MovieId</returns>
		public long GetMovieId() { return MovieId; }

		/// <summary>
		/// Returns the ValueType. See ValueType for details.
		/// </summary>
		/// <returns>ValueType</returns>
		public ValueType GetValueType() { return Type; }

		private double GetNumber()
		{
			if (IsNumber())
			{
#if UNITY_WP8
			return sf_getNumber(this);
#else
				return SF_GetNumber(this);
#endif
			}
			UnityEngine.Debug.Log("Trying to call GetNumber on a Non-Number value");
			return 0;
		}

		private UInt32 GetUInt()
		{
			if (IsNumeric())
			{
#if UNITY_WP8
			return sf_getUInt(this);
#else
				return SF_GetUInt(this);
#endif
			}
			UnityEngine.Debug.Log("Trying to call GetUInt on a Non-Numeric value");
			return 0;
		}

		private Int32 GetInt()
		{
			if (IsNumeric())
			{
#if UNITY_WP8
			return sf_getInt(this);
#else
				return SF_GetInt(this);
#endif
			}
			UnityEngine.Debug.Log("Trying to call GetInt on a Non-Numeric value");
			return 0;
		}

		private Boolean GetBool()
		{
			if (IsBoolean())
			{
#if UNITY_WP8
			return sf_getBool(this);
#else
				return SF_GetBool(this);
#endif
			}
			UnityEngine.Debug.Log("Trying to call GetBoolean on a Non-Boolean value");
			return false;
		}

		private String GetString()
		{
			if (IsString())
			{
#if UNITY_WP8
			String str = Marshal.PtrToStringAnsi(sf_getString(this));
#else
				String str = Marshal.PtrToStringAnsi(SF_GetString(this));
#endif
				return str;
			}
			UnityEngine.Debug.Log("Trying to call GetString on a Non-String value");
			return String.Empty;
		}

		/// <summary>
		/// Sets the number for a Number value
		/// </summary>
		/// <param name="num">Number</param>
		public void SetNumber(double num)
		{
			if (IsNumber())
			{
#if UNITY_WP8
			sf_setNumber(this, num);
#else
				SF_SetNumber(this, num);
#endif
			}
			else
			{
				UnityEngine.Debug.Log("Trying to call SetNumber on a Non-Number value");
			}
		}

		/// <summary>
		/// Sets an UInt Value.
		/// </summary>
		/// <param name="uival">UInt</param>
		public void SetUInt(UInt32 uival)
		{
			if (IsNumeric())
			{
#if UNITY_WP8
			sf_setUInt(this, uival);
#else
				SF_SetUInt(this, uival);
#endif
			}
			else
			{
				UnityEngine.Debug.Log("Trying to call SetUInt on a Non-Numeric value");
			}

		}

		/// <summary>
		/// Sets an Int Value.
		/// </summary>
		/// <param name="ival">Int</param>
		public void SetInt(Int32 ival)
		{
			if (IsNumeric())
			{
#if UNITY_WP8
			sf_setInt(this, ival);
#else
				SF_SetInt(this, ival);
#endif
			}
			else
			{
				UnityEngine.Debug.Log("Trying to call SetInt on a Non-Numeric value");
			}
		}

		/// <summary>
		/// Sets a Boolean value
		/// </summary>
		/// <param name="bval">Boolean </param>
		public void SetBool(Boolean bval)
		{
			if (IsBoolean())
			{
#if UNITY_WP8
			sf_setBool(this, bval);
#else
				SF_SetBool(this, bval);
#endif
			}
			else
			{
				UnityEngine.Debug.Log("Trying to call SetBool on a Non-Boolean value");
			}
		}

		/// <summary>
		/// Sets a String Value
		/// </summary>
		/// <param name="str">String</param>
		public void SetString(String str)
		{
			if (IsString())
			{
#if UNITY_WP8
			sf_setString(this, Marshal.StringToCoTaskMemAnsi(str));
#else
				SF_SetString(this, Marshal.StringToCoTaskMemAnsi(str));
#endif
			}
			else
			{
				UnityEngine.Debug.Log("Trying to call SetString on a Non-String value");
			}

		}

		/// <summary>
		/// Gets the C++ object pointed to by the Value.
		/// </summary>
		/// <returns>C++ object wrapped in a Value.</returns>
		public Value GetObject()
		{
			int SFValueSize = Marshal.SizeOf(typeof(Value));
			IntPtr ptr2 = Marshal.AllocCoTaskMem(SFValueSize);
#if UNITY_WP8
		sf_getObject(this, ptr2);
#else
			SF_GetObject(this, ptr2);
#endif
			// This value will be garbage collected as well.
			Value val = SFManager.GetValueData(ptr2);
			Marshal.FreeCoTaskMem(ptr2);
			return val;
		}

		/// <summary>
		/// Sets the member elemName to the Value val. Used to directly change values of ActionScript objects. For example, <c>v.SetMember("x", 100.0)</c>
		/// </summary>
		/// <param name="elemName">Element Name</param>
		/// <param name="val">Value</param>
		/// <returns>True if the member corresponding to elemName was found and was of the correct type. False otherwise.</returns>
		public bool SetMember(String elemName, Value val)
		{
#if UNITY_WP8
        return sf_setMember(this, elemName, val);
#else
			return SF_SetMember(this, elemName, val);
#endif
		}

		/// <summary>
		/// See entry for SetMember(String, Value).
		/// </summary>
		/// <param name="elemName">Element Name.</param>
		/// <param name="val">Value.</param>
		/// <returns>True if the member corresponding to elemName was found and was of the correct type. False otherwise.</returns>
		public bool SetMember(String elemName, String str)
		{
#if UNITY_WP8
        return sf_setMember(this, elemName, new Value(str, this.MovieId));				
#else
			return SF_SetMember(this, elemName, new Value(str, this.MovieId));
#endif
		}

		/// <summary>
		/// See entry for SetMember(String, Value).
		/// </summary>
		/// <param name="elemName">Element Name</param>
		/// <param name="val">Value</param>
		/// <returns>True if the member corresponding to elemName was found and was of the correct type. False otherwise.</returns>		
		public bool SetMember(String elemName, Boolean bval)
		{
#if UNITY_WP8
        return sf_setMember(this, elemName, new Value(bval, this.MovieId));
#else
			return SF_SetMember(this, elemName, new Value(bval, this.MovieId));
#endif
		}

		/// <summary>
		/// See entry for SetMember(String, Value).
		/// </summary>
		/// <param name="elemName">Element Name</param>
		/// <param name="val">Value</param>
		/// <returns>True if the member corresponding to elemName was found and was of the correct type. False otherwise.</returns>		
		public bool SetMember(String elemName, int num)
		{
#if UNITY_WP8
        return sf_setMember(this, elemName, new Value(num, this.MovieId));
#else
			return SF_SetMember(this, elemName, new Value(num, this.MovieId));
#endif
		}

		/// <summary>
		/// See entry for SetMember(String, Value).
		/// </summary>
		/// <param name="elemName">Element Name.</param>
		/// <param name="val">Value</param>
		/// <returns>True if the member corresponding to elemName was found and was of the correct type. False otherwise.</returns>	
		public bool SetMember(String elemName, uint num)
		{
#if UNITY_WP8
        return sf_setMember(this, elemName, new Value(num, this.MovieId));
#else
			return SF_SetMember(this, elemName, new Value(num, this.MovieId));
#endif
		}

		/// <summary>
		/// See entry for SetMember(String, Value).
		/// </summary>
		/// <param name="elemName">Element Name.</param>
		/// <param name="val">Value</param>
		/// <returns>True if the member corresponding to elemName was found and was of the correct type. False otherwise.</returns>
		public bool SetMember(String elemName, double num)
		{
#if UNITY_WP8
        return sf_setMember(this, elemName, new Value(num, this.MovieId));
#else
			return SF_SetMember(this, elemName, new Value(num, this.MovieId));
#endif
		}

		/// <summary>
		/// Sets the parameter dest to the value corresponding to elemName.
		/// </summary>
		/// <param name="elemName">Element Name.</param>
		/// <param name="dest">Destination Value.</param>
		/// <returns>True if the member corresponding to elemName was found. False otherwise.</returns>	
		public bool GetMember(String elemName, ref Value dest)
		{
#if UNITY_WP8
        return sf_getMember(this, elemName, dest);
#else
			return SF_GetMember(this, elemName, dest);
#endif
		}

		/// <summary>
		/// Returns Value corresponding to elemName.
		/// </summary>
		/// <param name="elemName">Element Name.</param>
		/// <returns>Value corresponding to elemName or null if the elemName is not found.</returns>

		public Value GetMember(String elemName)
		{
			Value dest = new Value();
#if UNITY_WP8
        bool res = sf_getMember(this, elemName, dest);
#else
			bool res = SF_GetMember(this, elemName, dest);
#endif
			if (res)
			{
				return dest;
			}
			else
			{
				return null;
			}
		}

		/// <summary>
		/// Returns the Array Size.
		/// </summary>
		/// <returns>Array size, or -1 if this Value is not an array type.</returns>	 				
		public int GetArraySize()
		{
			if (!IsArray())
			{
				return -1;
			}
#if UNITY_WP8
        return sf_getArraySize(this);
#else
			return SF_GetArraySize(this);
#endif
		}

		/// <summary>
		/// Sets the Array Size. typically used with CreateArray. 
		/// For example: 
		/// <code>
		/// Value arr = CreateArray();
		/// arr.SetArraySize(5);</code>
		/// </summary>
		/// <param name="sz">Size of array.</param>
		/// <returns></returns>
		public bool SetArraySize(UInt32 sz)
		{
			if (!IsArray())
			{
				return false;
			}
#if UNITY_WP8
        return sf_setArraySize(this, sz);
#else
			return SF_SetArraySize(this, sz);
#endif
		}

		/// <summary>
		/// Returns the element at index idx.
		/// </summary>
		/// <param name="idx">The index of the element list.</param>
		/// <returns>The element at index idx, or null if the Value is not an array or if the element doesn't exist.</returns>
		public Value GetElement(UInt32 idx)
		{
			Value dest = new Value();
			if (!IsArray())
			{
				return null;
			}
#if UNITY_WP8
        bool res = sf_getElement(this, idx, dest);
#else
			bool res = SF_GetElement(this, idx, dest);
#endif
			if (res)
			{
				return dest;
			}
			return null;
		}

		/// <summary>
		/// Sets the element at index idx to object o. For example:
		/// <code>
		/// Value arr = CreateArray();
		/// arr.SetArraySize(5);
		/// arr.SetElement(2, "Hello");</code>
		/// </summary>
		/// <param name="idx"></param>
		/// <param name="o"></param>
		/// <returns></returns>
		public bool SetElement(UInt32 idx, object o)
		{
			if (!IsArray())
			{
				return false;
			}
			Value v = CreateValue(o);
#if UNITY_WP8
        return sf_setElement(this, idx, v);
#else
			return SF_SetElement(this, idx, v);
#endif
		}

		/// <summary>
		/// Pops and returns the last value from the array.
		/// </summary>
		/// <returns> The popped value or null if the value is not of type array or if the array is empty.</returns>
		public Value PopBack()
		{
			Value dest = new Value();
			if (!IsArray())
			{
				return null;
			}
#if UNITY_WP8
        bool res = sf_popBack(this, dest);
#else
			bool res = SF_PopBack(this, dest);
#endif
			if (res)
			{
				return dest;
			}
			return null;
		}

		/// <summary>
		/// Pushes val at the back of the array.
		/// </summary>
		/// <param name="val"></param>
		/// <returns>True if pushback succeeded, false otherwise (for example if the Value on which the method is called is not an array). </returns>
		public bool PushBack(Value val)
		{
			if (!IsArray())
			{
				return false;
			}
#if UNITY_WP8
        return sf_pushBack(this, val);
#else
			return SF_PushBack(this, val);
#endif
		}

		/// <summary>
		/// Pushes object o at the back of the array.
		/// </summary>
		/// <param name="val"></param>
		/// <returns>True if pushback succeeded, false otherwise (for example if the Value on which the method is called is not an array). </returns>
		public bool PushBack(object o)
		{
			if (!IsArray())
			{
				return false;
			}

			Value v = CreateValue(o);

#if UNITY_WP8
        return sf_pushBack(this, v);
#else
			return SF_PushBack(this, v);
#endif
		}

		/// <summary>
		/// Removed element at index idx.
		/// </summary>
		/// <param name="idx"></param>
		/// <returns>True if the remove operation succeeded, false otherwise (if the Value is not an array or if the index is larger than the num of elements in the array.</returns>
		public bool RemoveElement(UInt32 idx)
		{
			if (!IsArray())
			{
				return false;
			}
#if UNITY_WP8
        return sf_removeElement(this, idx);
#else
			return SF_RemoveElement(this, idx);
#endif
		}

		/// <summary>
		/// Clears the array.
		/// </summary>
		/// <returns>True if clear operation succeeded.</returns>
		public bool ClearElements()
		{
			if (!IsArray())
			{
				return false;
			}
#if UNITY_WP8
        return sf_clearElements(this);
#else
			return SF_ClearElements(this);
#endif
		}

		/// <summary>
		/// Returns the DisplayInfo structure associated with a display object. The target value must be a display object. See doc for DisplayInfo for more information.
		/// </summary>
		/// <returns> DisplayInfo structure</returns>
		public SFDisplayInfo GetDisplayInfo()
		{
			SFDisplayInfo dinfo = new SFDisplayInfo();
			Int32 size = Marshal.SizeOf(typeof(SFDisplayInfo));
#if UNITY_WP8
        bool retVal         = sf_getDisplayInfo(this, dinfo, size);
#else
			bool retVal = SF_GetDisplayInfo(this, dinfo, size);
#endif
			return retVal ? dinfo : null;
		}

		/// <summary>
		/// Set the DisplayInfo property of the target DisplayObject to the DisplayInfo provided. Target Value must be a DisplayObject.
		/// </summary>
		/// <param name="dinfo"></param>
		/// <returns>True if the operation succeeded</returns>
		public bool SetDisplayInfo(SFDisplayInfo dinfo)
		{
			if (!IsDisplayObject())
			{
				return false;
			}
			Int32 size = Marshal.SizeOf(typeof(SFDisplayInfo));
#if UNITY_WP8
        return sf_setDisplayInfo(this, dinfo, size);
#else
			return SF_SetDisplayInfo(this, dinfo, size);
#endif
		}

		/// <summary>
		/// Returns the DisplayMatrix associated with the target Value. Target Value must be a DisplayObject.
		/// </summary>
		/// <returns>DisplayMatrix</returns>
		public SFDisplayMatrix GetDisplayMatrix()
		{
			SFDisplayMatrix dmat = new SFDisplayMatrix();
			Int32 size = Marshal.SizeOf(typeof(SFDisplayMatrix));
#if UNITY_WP8
        bool retVal = sf_getDisplayMatrix(this, dmat, size);
#else
			bool retVal = SF_GetDisplayMatrix(this, dmat, size);
#endif
			return retVal ? dmat : null;
		}

		/// <summary>
		/// Sets the DisplayMatrix on the target Value. Target value must be a DisplayObject.
		/// </summary>
		/// <param name="dmat"></param>
		/// <returns>true if the SetDisplayMatrix operation succeeded.</returns>
		public bool SetDisplayMatrix(SFDisplayMatrix dmat)
		{
			if (!IsDisplayObject())
			{
				return false;
			}
			Int32 size = Marshal.SizeOf(typeof(SFDisplayMatrix));
#if UNITY_WP8
        return sf_setDisplayMatrix(this, dmat, size);
#else
			return SF_SetDisplayMatrix(this, dmat, size);
#endif
		}

		/// <summary>
		/// Sets the ColorTransform on the target Value. Target value must be a DisplayObject.
		/// </summary>
		/// <param name="dmat"></param>
		/// <returns>true if the SetColorTransform operation succeeded.</returns>
		public bool SetColorTransform(SFCxForm cxform)
		{
			if (!IsDisplayObject())
			{
				return false;
			}
#if UNITY_WP8
        return sf_setColorTransform(this,  cxform);
#else
			return SF_SetColorTransform(this, cxform);
#endif
		}

		/// <summary>
		/// Gets the ColorTransform associated with the target Value which must be a DisplayObject.
		/// </summary>
		/// <param name="cxform">ColorTransform structure.</param>
		/// <returns>True if the operation succeeded.</returns>
		public bool GetColorTransform(ref SFCxForm cxform)
		{
			if (!IsDisplayObject())
			{
				return false;
			}
#if UNITY_WP8
        return sf_getColorTransform(this, cxform);
#else
			return SF_GetColorTransform(this, cxform);
#endif
		}

		/// <summary>
		/// Sets the text (UTF8 format) on the target Value which must be DisplayObject. In fact, the DisplayObject should be a textfield, otherwise the SetText operation has no meaning. 
		/// </summary>
		/// <param name="str"></param>
		/// <returns>True if the SetText operation succeeded.</returns>
		public bool SetText(String str)
		{
			if (!IsDisplayObject())
			{
				return false;
			}
#if UNITY_WP8
        return sf_setText(this, str);
#else
			return SF_SetText(this, str);
#endif
		}

		/// <summary>
		/// Gets the text string from the target Value which must be a TextField. 
		/// </summary>
		/// <param name="txt">Text String (UTF8 format).</param>
		/// <returns>True of the GetText operation succeeded.</returns>
		public bool GetText(ref Value txt)
		{
			if (!IsDisplayObject())
			{
				return false;
			}
#if UNITY_WP8
        return sf_getText(this, txt);
#else
			return SF_GetText(this, txt);
#endif
		}

		/// <summary>
		/// Creates an empty movie clip which becomes a child of the current movie and gives it an instance name. The depth is useful for AS2 CreateEmptyMovieClip only and has no meaning for AS3.
		/// </summary>
		/// <param name="dest">Represents the newly created movieclip.</param>
		/// <param name="instanceName">The instance name for the movieclip.</param>
		/// <param name="depth">Depth, only valid for AS2. For AS3, the new movieclip is added to the top of the displaylist.</param>
		/// <returns>True if the movieclip was successfully created.</returns>
		public bool CreateEmptyMovieClip(ref Value dest, String instanceName, Int32 depth)
		{
#if UNITY_WP8
        return sf_createEmptyMovieClip(this, dest, instanceName, depth);
#else
			return SF_CreateEmptyMovieClip(this, dest, instanceName, depth);
#endif
		}

		/// <summary>
		/// Creates a movieclip from a symbol with name = symbolName. For AS3, the symbol should have been marked "Export for ActionScript" and a class name must have been created in the AS Linkage column. 
		/// </summary>
		/// <param name="dest">Value corresponding to the created movieclip. </param>
		/// <param name="symbolName">symbolName.</param>
		/// <param name="instanceName">instanceName.</param>
		/// <param name="depth">Only relevant for AS2.</param>
		/// <returns>True if the movieclip was created successfully.</returns>
		public bool AttachMovie(ref Value dest, String symbolName, String instanceName, Int32 depth)
		{
#if UNITY_WP8
        return sf_attachMovie(this, dest, symbolName, instanceName, depth);
#else
			return SF_AttachMovie(this, dest, symbolName, instanceName, depth);
#endif
		}

		/// <summary>
		/// Removes the movieclip passed as the argument. The syntax is slightly different for AS2/AS3. The ActionScript VM type can be obtained using Movie::GetAVMVersion().
		/// </summary>
		/// <param name="movieRemoved">Movie to be removed.</param>
		/// <returns>True if operation succeeded.</returns>
		public bool RemoveMovieAS3(Value movieRemoved)
		{
			if (movieRemoved == null) return false;
            if (!movieRemoved.IsValid()) return false;
			Value[] valArray = new Value[] { movieRemoved };
			if (Invoke("removeChild", valArray) == null)
				return false; // problem invoking removeChild
			return true; // all good
		}

		/// <summary>
		/// Removes the target movieclip. The syntax is slightly different for AS2/AS3. The ActionScript VM type can be obtained using Movie::GetAVMVersion().
		/// </summary>
		/// <returns>True if operation succeeded.</returns>
		public bool RemoveMovieAS2()
		{
			if (Invoke("removeMovie", null) == null)
				return false; // problem invoking removeChild
			return true; // all good
		}

		/// <summary>
		/// Advances the playhead to the frameName and plays that frame. Target must be a MovieClip.
		/// </summary>
		/// <param name="frameName">Frame name.</param>
		/// <returns>True if the operation succeeded.</returns>
		public bool GotoAndPlayFrame(String frameName)
		{
			if (!IsDisplayObject())
			{
				return false;
			}
#if UNITY_WP8
		return sf_gotoAndPlayFrame(this, frameName);
#else
			return SF_GotoAndPlayFrame(this, frameName);
#endif
		}

		/// <summary>
		/// Advances the playhead to frameName and stops the animation. Target must be a MovieClip.
		/// </summary>
		/// <param name="frameName">Frame Name.</param>
		/// <returns>True if the operation succeeded.</returns>
		public bool GotoAndStopFrame(String frameName)
		{
			if (!IsDisplayObject())
			{
				return false;
			}
#if UNITY_WP8
        return sf_gotoAndStopFrame(this, frameName);
#else
			return SF_GotoAndStopFrame(this, frameName);
#endif
		}

		/// <summary>
		/// Advances the playhead to frameNum and plays that frame. Target must be a MovieClip.
		/// </summary>
		/// <param name="frameNum">Frame Number.</param>
		/// <returns>True if the operation succeeded.</returns>
		public bool GotoAndPlay(Int32 frameNum)
		{
			if (!IsDisplayObject())
			{
				return false;
			}
#if UNITY_WP8
		return sf_gotoAndPlay(this, frameNum);
#else
			return SF_GotoAndPlay(this, frameNum);
#endif
		}

		/// <summary>
		/// Advances the playhead to frameNum and stops the animation. Target must be a MovieClip.
		/// </summary>
		/// <param name="frameNum">Frame Number.</param>
		/// <returns>True if the operation succeeded.</returns>
		public bool GotoAndStop(Int32 frameNum)
		{
			if (!IsDisplayObject())
			{
				return false;
			}
#if UNITY_WP8
        return sf_gotoAndStop(this, frameNum);
#else
			return SF_GotoAndStop(this, frameNum);
#endif
		}

		/// <summary>
		/// Creates a Value. Provides a convenient way to create a Value from a general argument type (int, boolean, string etc).
		/// </summary>
		/// <param name="o">The object to be wrapped into a Value.</param>
		/// <returns>Returns the created value. </returns>

		public Value CreateValue(object o)
		{
			if (o is Value)
				return o as Value;

			if (o is int)
			{
				return new Value((int)o, MovieId);
			}
			else if (o is float)
			{
				return new Value((float)o, MovieId);
			}
			else if (o is double)
			{
				return new Value((double)o, MovieId);
			}
			else if (o is String)
			{
				return new Value((String)o, MovieId);
			}
			else if (o is bool)
			{
				return new Value((bool)o, MovieId);
			}

			return new Value(0, MovieId);
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
					Marshal.WriteIntPtr(data, val.pInternalData);
					data = new IntPtr(data.ToInt32() + IntPtrSize);
					Marshal.WriteInt32(data, (int)val.Type);
					data = new IntPtr(data.ToInt32() + IntSize);
					Marshal.WriteInt64(data, (long)val.MovieId);
				}
			}
			return ptr;
		}

		/// <summary>
		/// Calls the ActionScript method called methodName on the target Value passing it the array of arguments provided. For example, <c>val.Invoke("MyAsFunc", "hello", 0, 2, value2); </c>
		/// </summary>
		/// <param name="methodName">The name of the ActionScript method to be invoked.</param>
		/// <param name="objs">Array of arguments. The function takes a variable number of arguments which are serialized internally.</param>
		/// <returns></returns>

		public Value Invoke(String methodName, params object[] objs)
		{

			int numElem = objs.Length;
			Value retVal = new Value();
			IntPtr ptr = Serialize(objs);
#if UNITY_WP8
        bool result = sf_invoke2(this, methodName, numElem, ptr, retVal);
#else
			bool result = SF_Invoke2(this, methodName, numElem, ptr, retVal);
#endif
			Marshal.FreeCoTaskMem(ptr);
			if (result)
				return retVal; // indicates that method was successfully invoked, but doesn't return anything
			else
				return null; // indicates there was a problem invoking the method. 

		}

		/// <summary>
		/// Helper function to convert an ActionScript object to a C# object. The target is the ActionScript object to be converted. The C# and AS class should have corresponding properties. Any properties which do not correspond are not copied during the conversion process. The function is able to handle nested structures as well.
		/// </summary>
		/// <param name="objectType">The type of the C# class used to instantiate the C# object.</param>
		/// <returns>The C# object if the operation succeeded, null otherwise. A non-null result doesn't imply that all members were copied successfully.</returns>
		public object ConvertFromASObject(Type objectType)
		{
			if (!IsObject())
				return null;

			// Create a new C# object of the desired type
			object result = Activator.CreateInstance(objectType);
			if (result == null)
			{
				return null;
			}

			// Convert AS object members based on C# type definition
#if !NETFX_CORE
			foreach (var propInfo in objectType.GetProperties())
#else
        foreach (var propInfo in objectType.GetTypeInfo().DeclaredProperties)
#endif
			{
				if (!propInfo.CanWrite)
				{
					continue;
				}
				UnityEngine.Debug.Log(propInfo.PropertyType);
				// Get the corresponding object member from ActionScript
				Value propertyValue = GetMember(propInfo.Name);
				if (propertyValue == null)
				{
					continue;
				}

				object csValue = null;
				if (propInfo.PropertyType == typeof(System.Int32))
				{
					csValue = propertyValue.GetInt();
				}
				else if (propInfo.PropertyType == typeof(String))
				{
					csValue = propertyValue.GetString();
				}
				else if (propInfo.PropertyType == typeof(System.UInt32))
				{
					csValue = propertyValue.GetUInt();
				}
				else if (propInfo.PropertyType == typeof(Double))
				{
					csValue = propertyValue.GetNumber();
				}
				else if (propInfo.PropertyType == typeof(System.Single)) // float
				{
					csValue = (float)propertyValue.GetNumber();
				}
				else if (propInfo.PropertyType == typeof(System.Boolean))
				{
					csValue = propertyValue.GetBool();
				}
				else
				{
#if !NETFX_CORE
					if (!propInfo.PropertyType.IsPrimitive)
#else
                if(!propInfo.GetType().GetTypeInfo().IsPrimitive)
#endif
					{
						Value objectVal = propertyValue.GetObject();
						csValue = objectVal.ConvertFromASObject(propInfo.PropertyType);
					}
					else
					{
						String info = String.Format("Trying to convert a not handled managed type{0}!", propInfo.PropertyType.Name);
						UnityEngine.Debug.Log(info);
					}
				}

				propInfo.SetValue(result, csValue, null);

			}
			return result;
		}

		/// <summary>
		/// Creates an ActionScript object from the given C# object. The C# and AS class should have corresponding properties. Any properties which do not correspond are not copied during the conversion process. The function is able to handle nested structures as well. There should be an ActionScript class with the same name as the C# class. The AS object is created in the root level of the movie. See the Direct Access API demo for examples
		/// </summary>
		/// <param name="obj">C# object to be converted.</param>
		/// <param name="mv">Movie. The ActionScript object will be created in the root of this movie.</param>
		/// <param name="fullyQualifiedClassName"> The fully qualified class name of the ActionScript class. As an example, this can be used to specify an ActionScript class in a package where a C# class is defined in global scope.</param>
		/// <returns>The ActionScript object if successful, null otherwise.</returns>
		/// <remarks> Sample usage: 
		/// <code>
		/// MyMovie myMovie = ...;
		/// TestClass tc = new TestClass();
		/// Value obj = Value.ConvertToAsObject(tc, myMovie); This converts the C# TestClass object to an ActionScript class bearing the same name - "TestClass".
		/// Value obj2 = Value.ConvertToAsObject(tc, myMovie, "com.example.App.TestClass"); This converts the C# TestClass object to an ActionScript class with a fully qualified class name of "com.example.App.TestClass".
		/// </code>
		/// </remarks>
		public static Value ConvertToASObject(object obj, Movie mv, string fullyQualifiedClassName = "")
		{
			// Step1: Get object type
			System.Type tp = obj.GetType();
			// UnityEngine.Debug.Log(String.Format ("Type of CS object to convert: {0} \n", tp.Name));
			// Step 2: Create AS object of the correct type using CreateObject (defined in SFMovie for now)
			// The ActionScript class must have a default constructor that accepts no arguments or a constructor with default arguments,		// otherwise we will get error# 1063

			String typeName = tp.Name;
			if (fullyQualifiedClassName.Length > 0)
			{
				typeName = fullyQualifiedClassName;
			}

			Value result = mv.CreateObject(typeName);
			if (null == result || !result.IsValid())
			{
				return null;
			}
			// Step 3: Iterate over the properties of obj and fill in the corresponding members of the ASObject, if 
			// the members exist
			// Also think about nested objects etc. 
#if !NETFX_CORE
			foreach (var propInfo in tp.GetProperties())
#else
        foreach (var propInfo in tp.GetTypeInfo().DeclaredProperties)
#endif
			{
				//UnityEngine.Debug.Log(propInfo.PropertyType);


				if (propInfo.PropertyType == typeof(System.Int32))
				{
					result.SetMember(propInfo.Name, (Int32)propInfo.GetValue(obj, null));
				}
				else if (propInfo.PropertyType == typeof(String))
				{
					result.SetMember(propInfo.Name, (String)propInfo.GetValue(obj, null));
				}
				else if (propInfo.PropertyType == typeof(System.UInt32))
				{
                    result.SetMember(propInfo.Name, (UInt32)propInfo.GetValue(obj, null));
				}
				else if (propInfo.PropertyType == typeof(Double))
				{
					result.SetMember(propInfo.Name, (Double)propInfo.GetValue(obj, null));
				}
				else if (propInfo.PropertyType == typeof(System.Single)) // float
				{
					result.SetMember(propInfo.Name, (Single)propInfo.GetValue(obj, null));
				}
				else if (propInfo.PropertyType == typeof(System.Boolean))
				{
					result.SetMember(propInfo.Name, (Boolean)propInfo.GetValue(obj, null));
				}
				else
				{
#if !NETFX_CORE
					if (!propInfo.PropertyType.IsPrimitive)
#else
                if(!propInfo.GetType().GetTypeInfo().IsPrimitive)
#endif
					{

                        Value propertyValue = result.GetMember(propInfo.Name);
                        if (propertyValue == null)
                        {
                            continue;
                        }

						propertyValue = ConvertToASObject(propInfo.GetValue(obj, null), mv);
						result.SetMember(propInfo.Name, propertyValue);
					}
					else
					{
						UnityEngine.Debug.Log(String.Format("Trying to convert a not handled managed type{0}!", propInfo.PropertyType.Name));
					}
				}
			}
			return result;
		}



        /// <summary>
        /// Creates an ActionScript object from the given C# object. The C# and AS class should have corresponding properties. Any properties which do not correspond are not copied during the conversion process. The function is able to handle nested structures as well. There should be an ActionScript class with the same name as the C# class. The AS object is created in the root level of the movie. See the Direct Access API demo for examples
        /// </summary>
        /// <param name="obj">C# object to be converted.</param>
        /// <param name="mv">Movie. The ActionScript object will be created in the root of this movie.</param>
        /// <param name="fullyQualifiedClassName"> The fully qualified class name of the ActionScript class. As an example, this can be used to specify an ActionScript class in a package where a C# class is defined in global scope.</param>
        /// <returns>The ActionScript object if successful, null otherwise.</returns>
        /// <remarks> Sample usage: 
        /// <code>
        /// MyMovie myMovie = ...;
        /// TestClass tc = new TestClass();
        /// Value obj = Value.ConvertToAsObject(tc, myMovie); This converts the C# TestClass object to an ActionScript class bearing the same name - "TestClass".
        /// Value obj2 = Value.ConvertToAsObject(tc, myMovie, "com.example.App.TestClass"); This converts the C# TestClass object to an ActionScript class with a fully qualified class name of "com.example.App.TestClass".
        /// </code>
        /// </remarks>
        public static Value ConvertToASObjectEx(ASObjectEx obj, Movie mv, string fullyQualifiedClassName)
        {
            string typeName = fullyQualifiedClassName;

            Value result = mv.CreateObject(typeName);
            if (null == result || !result.IsValid())
            {
                return null;
            }

            if (obj.values==null || obj.names==null || obj.types==null)
            {
                return null;
            }
            if (obj.values.Length != obj.names.Length || obj.values.Length != obj.types.Length)
            {
                return null;
            }

            // Step 3: Iterate over the properties of obj and fill in the corresponding members of the ASObject, if 
            // the members exist
            // Also think about nested objects etc. 
            for (int i = 0; i < obj.values.Length; i++)
            {
                if (obj.types[i] == typeof(System.Int32))
                {
                    result.SetMember(obj.names[i], (Int32)obj.values[i]);
                }
                else if (obj.types[i] == typeof(String))
                {
                    result.SetMember(obj.names[i], (String)obj.values[i]);
                }
                else if (obj.types[i] == typeof(System.UInt32))
                {
                    result.SetMember(obj.names[i], (UInt32)obj.values[i]);
                }
                else if (obj.types[i] == typeof(Double))
                {
                    result.SetMember(obj.names[i], (Double)obj.values[i]);
                }
                else if (obj.types[i] == typeof(System.Single)) // float
                {
                    result.SetMember(obj.names[i], (Single)obj.values[i]);
                }
                else if (obj.types[i] == typeof(System.Boolean))
                {
                    result.SetMember(obj.names[i], (Boolean)obj.values[i]);
                }
 
            }
            return result;
        }

        /// <summary>
        /// 统一检测SFValue值是否为空，是否可用
        /// </summary>
        /// <param name="v">要检测的Value值</param>
        /// <returns>返回true：可用；false：不可用</returns>
        public static bool IsValueValid(Value v)
        {
            if (v == null || !v.IsValid())
            {
                //Trace.LogWarning("Here is a Value question to pay attention to!\n" + System.Environment.StackTrace);
                return false;
            }
            return true;
        }
		/*		
		public Value Invoke(String methodName, Value[] valArray)
		{

			int valueSize   = Marshal.SizeOf(typeof(Value));
			int IntPtrSize  = Marshal.SizeOf(typeof(IntPtr));
			int IntSize     = Marshal.SizeOf(typeof(int));
			int numElem		= 0;
			IntPtr	ptr		= IntPtr.Zero;
			Value retVal	= new Value();
			if (valArray != null)
			{
				numElem = valArray.GetLength(0);
				ptr = Marshal.AllocCoTaskMem(valueSize * numElem);
				for (int i = 0; i < numElem; i++)
				{
					// Can't add an integer offset to IntPtr as you would with C/C++ pointer 
					IntPtr data = new IntPtr(ptr.ToInt32() + valueSize * i);
					Marshal.WriteIntPtr(data, valArray[i].pInternalData);
					data = new IntPtr(data.ToInt32() + IntPtrSize);
					Marshal.WriteInt32(data, (int)valArray[i].Type);
					data = new IntPtr(data.ToInt32() + IntSize);
					Marshal.WriteInt64(data, (long)valArray[i].MovieId);
				}
			}
		
        
			bool result = SF_Invoke2(this, methodName, numElem, ptr, retVal);
			Marshal.FreeCoTaskMem(ptr);
			if (result)
				return retVal; // indicates that method was successfully invoked, but doesn't return anything
			else
				return null; // indicates there was a problem invoking the method. 
    
		}*/

		~Value()
		{
			if (pInternalData != IntPtr.Zero)
			{
				SFManager.AddValueToReleaseList(pInternalData);
				//    SF_DecrementValRefCount(pInternalData);

			}
		}
	}


} // namespace Scaleform;

