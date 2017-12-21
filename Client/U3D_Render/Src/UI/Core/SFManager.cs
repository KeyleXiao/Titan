/**********************************************************************

Filename    :   SFManager.cs
Content     :   SFManager implementation
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
	enum SFUnityErrorCodes: int
	{
		Failure = 0,
		Success,
		IncompatibleRenderer,
		NullManager
	};
	/// <summary>
	///  A structure to modify display properties of movie clips.
	/// </summary>
	public class DisplayInfo
	{
		float X; //The x coordinate
		float Y; // The y coordinate
		float Z; // The z coordinate
		float Rotation; //Rotation of movie clip
		float XScale; //Horizontal scale
		float YScale; //Vertical scale
		float ZScale; // Scale of the movie along Z axis
		float Alpha; // Alpha transparency
		bool Visible; //Visibility of movie clip
		float XRotation; //Rotation around X axis
		float YRotation; //Rotation around Y axis
	}
	/// <summary>
	/// Represents the scaling type used for the SWF file stage.
	/// </summary>
	public enum ScaleModeType
	{
		SM_NoScale, //The size of SWF content is fixed. 
		SM_ShowAll, //Makes the entire SWF content visible in the viewport without distortion while maintaining the original aspect ratio. 
		SM_ExactFit, //Makes the entire SWF content visible in the viewport without trying to preserve the original aspect ratio. 
		SM_NoBorder //Scales the SWF content to fill whole viewport area, without distortion but possibly with some cropping while maintaining original aspect ratio. 
	};

	/// <summary>
	/// A container of settings used for movie creation
	/// </summary>
	// Note: Must use struct and not class, otherwise iOS will throw AOT compilation errors
	[StructLayout(LayoutKind.Explicit, Pack = 4)]
	public struct SFMovieCreationParams
	{
		// Allows a rendertexture to be passed in
		public SFMovieCreationParams(String name, int depth, int ox, int oy, int width, int height,
			IntPtr pdata, int length, bool initFirstFrame, RenderTexture texture, Color32 bgColor,
			bool useBackgroundColor = false, ScaleModeType scaleModeType = ScaleModeType.SM_ShowAll, bool bAutoManageVP = true) :
			this(name, depth, ox, oy, width, height, pdata, length, initFirstFrame, bgColor, useBackgroundColor, scaleModeType, bAutoManageVP)
		{
			IsRenderToTexture = (texture != null);
			TexWidth = ((texture != null) ? (UInt32)texture.width : 0);
			TexHeight = ((texture != null) ? (UInt32)texture.height : 0);

#if !(UNITY_3_5)
			if (texture)
			{
#if UNITY_IPHONE
            TextureId = (uint)texture.GetNativeTextureID();
#else
				IntPtr texPtr = texture.GetNativeTexturePtr();
				TextureId = (uint)(texPtr);
				if (TextureId == 0)
				{
					UnityEngine.Debug.Log("TextureID of the RenderTexture is 0, render-to-texture will not work. Please check the settings for the rendertexture");
				}
#endif //UNITY_IPHONE
			}
			else
			{
				TextureId = 0;
			}
#else
        TextureId = ((texture != null) ? (UInt32)texture.GetNativeTextureID() : 0);
#if UNITY_STANDALONE_WIN
        if (SystemInfo.graphicsDeviceVersion[0] == 'D')
        {
            // SystemInfo.graphicsDeviceVersion starts with either "Direct3D" or "OpenGL".
            // We need to disable RTT on D3D+Windows because GetNativeTextureID() returns
            // a garbage value in D3D mode instead of zero, even though
            // GetNativeTextureID() is only supported in OpenGL mode.
            TextureId = 0;
            IsRenderToTexture = false;
            TexWidth = 0;
            TexHeight = 0;
        }
#endif //UNITY_STANDALONE_WIN
#endif //!(UNITY_3_5)

			Pad0 = Pad1 = Pad2 = false;
		}

		public SFMovieCreationParams(String name, int depth, int ox, int oy, int width, int height,
			IntPtr pdata, int length, bool initFirstFrame, Color32 bgColor, bool useBackgroundColor = false,
			ScaleModeType scaleModeType = ScaleModeType.SM_ShowAll, bool bautoManageVP = true)
		{
			MovieName = name;
			OX = ox;
			OY = oy;
			Width = width;
			Height = height;
			Depth = depth;
			IsInitFirstFrame = initFirstFrame;
			IsAutoManageViewport = bautoManageVP;
			pData = pdata;
			Length = length;
			TheScaleModeType = scaleModeType;
			IsUseBackgroundColor = useBackgroundColor;
			IsRenderToTexture = false;
			TextureId = 0;
			TexWidth = 0;
			TexHeight = 0;
			Red = bgColor.r;
			Green = bgColor.g;
			Blue = bgColor.b;
			Alpha = bgColor.a;
			IsMemoryFile = (pdata != IntPtr.Zero);
			Pad0 = Pad1 = Pad2 = false;
			Sentinal = SFSentinal.Sentinal;
		}

		[FieldOffset(0)]
		public String MovieName;
		[FieldOffset(4)]
		public int OX;
		[FieldOffset(8)]
		public int OY;
		[FieldOffset(12)]
		public int Width;
		[FieldOffset(16)]
		public int Height;
		// The song and dance below is necessary because OSX's compiler does not align Start to 8 bytes on the C++ side of things
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(20)]
#else
		[FieldOffset(20)]
#endif
		public IntPtr pData;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(24)]
#else
		[FieldOffset(24)]
#endif
		public int Length;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(28)]
#else
		[FieldOffset(28)]
#endif
		public ScaleModeType TheScaleModeType;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(32)]
#else
		[FieldOffset(32)]
#endif
		// Determines if the movie is advanced right after creation.
		public bool IsInitFirstFrame;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(33)]
#else
		[FieldOffset(33)]
#endif
		public bool IsAutoManageViewport;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(34)]
#else
		[FieldOffset(34)]
#endif
		public bool IsUseBackgroundColor;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(35)]
#else
		[FieldOffset(35)]
#endif
		public bool IsRenderToTexture;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(36)]
#else
		[FieldOffset(36)]
#endif
		public UInt32 TexWidth;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(40)]
#else
		[FieldOffset(40)]
#endif
		public UInt32 TexHeight;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(44)]
#else
		[FieldOffset(44)]
#endif
		public Byte Red;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(45)]
#else
		[FieldOffset(45)]
#endif
		public Byte Green;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(46)]
#else
		[FieldOffset(46)]
#endif
		public Byte Blue;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(47)]
#else
		[FieldOffset(47)]
#endif
		public Byte Alpha;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(48)]
#else
		[FieldOffset(48)]
#endif
		public UInt32 TextureId;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(52)]
#else
		[FieldOffset(52)]
#endif
		public int Depth;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(56)]
#else
		[FieldOffset(56)]
#endif
		public bool IsMemoryFile;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(57)]
#else
		[FieldOffset(57)]
#endif
		public bool Pad0;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(58)]
#else
		[FieldOffset(58)]
#endif
		public bool Pad1;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(59)]
#else
		[FieldOffset(59)]
#endif
		public bool Pad2;
#if UNITY_STANDALONE_OSX || UNITY_IPHONE
    [FieldOffset(60)]
#else
		[FieldOffset(60)]
#endif
		public UInt32 Sentinal; // Used to make sure the structure was passed through to C++ without any marshaling/alignment issues
	}

	/// <summary>
	/// Purpose of this class is to manage Movie creation/destruction etc.
	/// </summary>
	public partial class SFManager : MonoBehaviour
	{
		private event SF_ExternalInterfaceDelegate EIEvent
		{
			add
			{
				EIDelegate += value;
			}
			remove
			{
				EIDelegate -= value;
			}
		}

		// Maintains a list of all SFMovies created in the game
		private List<Movie>				SFMovieList;
		//private List<long>				MarkForReleaseIDs;
		private static Queue<IntPtr>		MarkForReleaseValues;
		private List<SFLifecycleEvent>	LifecycleEventsList;
		private	bool					SFInitialized;

		private SF_ExternalInterfaceDelegate EIDelegate;
		private SF_DisplayInfoDelegate	DisplayInfoDelegate;
		private SF_AllocateDelegate		AllocDelegate;
		private SF_LogDelegate			LogDelegate;

		private IntPtr					pValues_PreAllocated;
		private IntPtr					pValueQueue;
		private IntPtr					pCommandQueue;
		private IntPtr					pASOutput;
		private Int32					NumPreAllocatedValues;
		private IntPtr					pCommandOffset;
		private IntPtr					pValueOffset;
		private IntPtr					pASOutputOffset;
		private Int32					MaxLogBufferMessageSize; // corresponds to the buffer size in GFx.
		private int						ScreenWidth;
		private int						ScreenHeight;

        public bool IsDisplaySFTrace = true;
        //上一次与逻辑层同步的Tick
        public double LastSyncTickCount = 0;
        public double DeltaSyncTickCount = 0;

        // Delegate Declarations
        public delegate void SF_ExternalInterfaceDelegate(long MovieID, String command, IntPtr pValue, int numArgs, int valueSize);
		public delegate void SF_LogDelegate(String message);
		public delegate IntPtr SF_AllocateDelegate(int numVal);
		public delegate IntPtr SF_DisplayInfoDelegate(IntPtr ptr);


		/// <summary>
		/// Initializes the Scaleform runtime according to the initialization parameters. See ref doc for SFInitParams for InitParams details
		/// </summary>
		/// <param name="initParams"></param>
		public void Init(SFInitParams initParams)
		{
			SFInitParams2 initParams2 = new SFInitParams2(initParams);
			int initParamsSize = Marshal.SizeOf(typeof(SFInitParams2));
			int sfValueSize = Marshal.SizeOf(typeof(Value));

			// initParams2.Print();
			IntPtr pdata = Marshal.AllocCoTaskMem(initParamsSize);
			Marshal.StructureToPtr(initParams2, pdata, false);
			String version = Application.unityVersion;
			SFInitialized = false;
#if UNITY_WP8
		if (sf_init(pdata, initParamsSize, version) == SFUnityErrorCodes.Failure)
#else
			if (SF_Init(pdata, initParamsSize, version) == (int)SFUnityErrorCodes.Failure)
#endif
			{
				UnityEngine.Debug.Log("Error in Scaleform Manager Initialization. There could be a problem with marshaling structure members");
				return;
			}
#if UNITY_WP8
		sf_loadFontConfig(GetScaleformContentPath());
#else
			SF_LoadFontConfig(GetScaleformContentPath());
#endif
			AllocateSharedData();
#if UNITY_WP8
		sf_setSharedData(pCommandOffset, pCommandQueue, 0);
        sf_setSharedData(pValueOffset, pValueQueue, 1);
        sf_setSharedData(pASOutputOffset, pASOutput, 2);
#else
			SF_SetSharedData(pCommandOffset, pCommandQueue, 0);
			SF_SetSharedData(pValueOffset, pValueQueue, 1);
			SF_SetSharedData(pASOutputOffset, pASOutput, 2);
#endif
			Marshal.DestroyStructure(pdata, typeof(SFInitParams));

			// SFMovieList = new SortedList<int, Movie>();
			SFMovieList = new List<Movie>();

			//MarkForReleaseIDs = new List<long>();
			MarkForReleaseValues = new Queue<IntPtr>();
			LifecycleEventsList = new List<SFLifecycleEvent>();
			pValues_PreAllocated = Marshal.AllocCoTaskMem(sfValueSize * NumPreAllocatedValues);

			SFKey.CreateKeyDictionary();
			SFInitialized = true;
		}
		

		private void OnDestroy()
		{
            Trace.Log("SFManager::OnDestroy");
            if (pValues_PreAllocated != IntPtr.Zero)
            {
                Marshal.FreeCoTaskMem(pValues_PreAllocated);
            }
            if (pValueQueue != IntPtr.Zero)
            {
                Marshal.FreeCoTaskMem(pValueQueue);
            }
            if (pCommandQueue != IntPtr.Zero)
            {
                Marshal.FreeCoTaskMem(pCommandQueue);
            }
            if (pCommandOffset != IntPtr.Zero)
            {
                Marshal.FreeCoTaskMem(pCommandOffset);
            }
            if (pValueOffset != IntPtr.Zero)
            {
                Marshal.FreeCoTaskMem(pValueOffset);
            }
			// Notify all movies that the Manager has been destroyed.
			for (int i = 0; i < SFMovieList.Count; i++)
			{
                if (SFMovieList[i] != null)
                {
                    SFMovieList[i].OnSFManagerDied();
                }
			}

            // Clear out ReleaseList
            //MarkForReleaseIDs.Clear();

            // todo.
            lock (MarkForReleaseValues)
            {
                int count = MarkForReleaseValues.Count;
                for (int i = 0; i < count; i++)
                {
                    IntPtr pInternalData = MarkForReleaseValues.Dequeue();
                    if (pInternalData != IntPtr.Zero)
                    {
#if UNITY_WP8
				Value.sf_decrementValRefCount(pInternalData);
#else
                        Value.SF_DecrementValRefCount(pInternalData);
#endif
                    }
                }
                MarkForReleaseValues.Clear();
            }

            SFMovieList.Clear();
            GC.Collect();
#if UNITY_ANDROID || UNITY_IPHONE
       
        SF_DestroyManager();
        SF_Destroy();
#endif
            SFInitialized = false;
	
		}

		private void AllocateSharedData()
		{
			int SFCommandSize = Marshal.SizeOf(typeof(SFCommand));
			int sfValueSize = Marshal.SizeOf(typeof(Value));
			int SFCharSize = Marshal.SizeOf(typeof(char));
			NumPreAllocatedValues = 300;//分配太少会崩溃，as->c#的value数量
			MaxLogBufferMessageSize = 2048;
			pValueQueue = Marshal.AllocCoTaskMem(sfValueSize * NumPreAllocatedValues * 10);
			pCommandQueue = Marshal.AllocCoTaskMem(SFCommandSize * NumPreAllocatedValues);
			pASOutput = Marshal.AllocCoTaskMem(10 * MaxLogBufferMessageSize * SFCharSize);
			pCommandOffset = Marshal.AllocCoTaskMem(Marshal.SizeOf(typeof(System.Int32)));
			pValueOffset = Marshal.AllocCoTaskMem(Marshal.SizeOf(typeof(System.Int32)));
			pASOutputOffset = Marshal.AllocCoTaskMem(Marshal.SizeOf(typeof(System.Int32)));

			Marshal.WriteInt32(pCommandOffset, 0);
			Marshal.WriteInt32(pValueOffset, 0);
			Marshal.WriteInt32(pASOutputOffset, 0);

			for (int i = 0; i < (10 * MaxLogBufferMessageSize); i++)
			{
				Marshal.WriteByte(pASOutput, i, 0);
			}
		}

		private Int32 RandomNumber()
		{
			System.Random random = new System.Random();
			return random.Next();
		}

		/// <summary>
		/// Uses the Unity logging mechanism to write message to the Unity log
		/// </summary>
		/// <param name="message">Message string</param>
		public void OnLogMessage(String message)
		{
			LogMessage(message);
		}

		private void LogMessage(String message)
		{
			Trace.Log(message);
		}

		private void HandleASTraces()
		{
			int numTraces = Marshal.ReadInt32(pASOutputOffset);
			if (numTraces == 0)
			{
				return;
			}

			IntPtr ptr = new IntPtr(pASOutput.ToInt32());
			String str = Marshal.PtrToStringAnsi(ptr);
            if(IsDisplaySFTrace)
            Trace.Log(str); // Write trace() to Debug Log.

			Marshal.WriteInt32(pASOutputOffset, 0);

			// Clear the buffer
			for (int i = 0; i < 10 * MaxLogBufferMessageSize; i++)
			{
				Marshal.WriteByte(pASOutput, i, 0);
			}
		}

		private void GetCommandData(IntPtr pqueue, ref SFCommand command)
		{
			// Marshal.PtrToStructure(pqueue, command); // Works on Windows, unsupported on iOS due to aot compilation.
			IntPtr ptr = new IntPtr(pqueue.ToInt32()); // Workaround for iOS.
			command.MovieID = Marshal.ReadInt64(ptr);
			ptr = new IntPtr(ptr.ToInt32() + Marshal.SizeOf(typeof(long)));
			command.ArgCount = Marshal.ReadInt32(ptr);
			ptr = new IntPtr(ptr.ToInt32() + Marshal.SizeOf(typeof(int)));
			IntPtr pmethodName = Marshal.ReadIntPtr(ptr);
			command.MethodName = Marshal.PtrToStringAnsi(pmethodName);
		}

		/// <summary>
		/// Used to create the platform specific path string that can be passed to the Scaleform runtime to load a swf file from the disk
		/// </summary>
		/// <returns> Platform specific path string</returns>
		public static String GetScaleformContentPath()
		{
#if (UNITY_STANDALONE_WIN || UNITY_EDITOR || UNITY_METRO || UNITY_WP8)
			return Application.dataPath + "/StreamingAssets/";
#elif UNITY_IPHONE
            return (Application.dataPath + "/Raw/");
#elif UNITY_STANDALONE_OSX
            return Application.dataPath + "/Data/StreamingAssets/";
#elif UNITY_ANDROID
            return "";
#endif
		}

		private IntPtr CreateUnmanagedDataCopy(IntPtr psrc, int size)
		{
			if (size == 0) { return new IntPtr(); }
			IntPtr pdestBuffer = Marshal.AllocCoTaskMem(size);
			byte[] pdata = new byte[size];
			Marshal.Copy(psrc, pdata, 0, size);
			Marshal.Copy(pdata, 0, pdestBuffer, size);
			return pdestBuffer;
		}


		/// <summary>
		/// Used to process external interface callbacks from flash. When external interface callbacks arrive, they are put on a queue. This queue is processed here. Each external interface callback is directed to a method defined in the corresponding movie subclass that matches the signature. This is the standard method used for communicating from Flash to C#. The more direct way to implement this is to use delegates which allows for calling C# methods directly from C++, however delegates don't work in an iOS environment. 
		/// This method is called every frame during the SFCamera::Update(). 
		/// </summary>
		public void ProcessCommands()
		{
			// Deal with AS Traces:
			HandleASTraces();

			int numCommands = Marshal.ReadInt32(pCommandOffset);
			int numValues = Marshal.ReadInt32(pValueOffset);

			if (numCommands == 0) return;

			int commandSize = Marshal.SizeOf(typeof(SFCommand));
			int sfValueSize = Marshal.SizeOf(typeof(Value));

			// Make a copy of the ValueQueue. The copy needs to be made to take care of nested external interface calls.
			// For example if user calls a C# function using externalinterface (EI) which issues an invoke to an AS function which in turn
			// calls another C# function using ExternalInterface. Processing an EI callback overwrites the pCommandQueue and ValueQueue.
			// Therefore, we should extract the commands and copy the valuequeue before we start processing the current round of EI callbacks 
			IntPtr pValueQueueCopy = CreateUnmanagedDataCopy(pValueQueue, sfValueSize * numValues);

			SFCommand[] sfCommands = new SFCommand[numCommands];
			IntPtr pqueue = new IntPtr(pCommandQueue.ToInt32());

			for (int i = 0; i < numCommands; i++)
			{
				pqueue = new IntPtr(pCommandQueue.ToInt32() + commandSize * i);
				sfCommands[i] = new SFCommand();
				GetCommandData(pqueue, ref sfCommands[i]);

			}
			// We have copied all the data we need, safe to reset at this point
#if UNITY_WP8
		sf_clearCommandBuffer(numCommands);
#else
			SF_ClearCommandBuffer(numCommands);
#endif
			Marshal.WriteInt32(pCommandOffset, 0);
			Marshal.WriteInt32(pValueOffset, 0);

			IntPtr pargs = new IntPtr(pValueQueueCopy.ToInt32());

			int cumNumArgs = 0;
			for (int i = 0; i < numCommands; i++)
			{
				SFCommand command = sfCommands[i];
				int numArgs = command.ArgCount;
				long MovieID = command.MovieID;

				String methodName = command.MethodName;
				OnExternalInterface(MovieID, methodName, pargs, numArgs, sfValueSize);

				cumNumArgs += numArgs;
				pargs = new IntPtr(pValueQueueCopy.ToInt32() + sfValueSize * cumNumArgs);
			}

			Marshal.FreeCoTaskMem(pValueQueueCopy);
		}

		private IntPtr AllocateImpl(int numVal)
		{
			if (numVal < NumPreAllocatedValues)
			{
				return pValues_PreAllocated;
			}
			else
			{
				int sfValueSize = Marshal.SizeOf(typeof(Value));
				// Allocate space on the COM heap. Should also be able to use AllocHGlobal in order to
				// allocate space on process heap.
				IntPtr ptr = Marshal.AllocCoTaskMem(sfValueSize * numVal);
				return ptr;
			}
		}

		private IntPtr AllocateDisplayInfo(IntPtr sz)
		{
			int dInfoSize = Marshal.SizeOf(typeof(SFDisplayInfo));
			int floatSize = Marshal.SizeOf(typeof(float));
			int doubleSize = Marshal.SizeOf(typeof(double));
			int intPtrSize = Marshal.SizeOf(typeof(IntPtr));

			// Allocate space on the COM heap. Should also be able to use AllocHGlobal in order to
			// allocate space on process heap.
			IntPtr dInfoPtr = Marshal.AllocCoTaskMem(dInfoSize);
			IntPtr vmptr = Marshal.AllocCoTaskMem(4 * 3 * floatSize);
			IntPtr projPtr = Marshal.AllocCoTaskMem(4 * 4 * floatSize);

			IntPtr pdata1 = new IntPtr(dInfoPtr.ToInt32() + doubleSize * 11);
			Marshal.WriteIntPtr(pdata1, vmptr);

			IntPtr pdata2 = new IntPtr(pdata1.ToInt32() + intPtrSize);
			Marshal.WriteIntPtr(pdata2, projPtr);

			// Return size of managed DisplayInfo struct to C++ so that it can be compared with
			// native dinfo size in order for marshalling to work correctly.
			Marshal.WriteInt32(sz, dInfoSize);
			return dInfoPtr;
		}

		/// <summary>
		/// Used to unserialize an unmanaged memory buffer and initialize a Value. For internal use only
		/// </summary>
		/// <param name="pqueue"></param>
		/// <returns>The new value</returns>
		public static Value GetValueData(IntPtr pqueue)
		{
			Value pvalue = new Value();

			// Marshal.PtrToStructure(pqueue, pvalue); // The easy way to do this (supported on Windows).
			IntPtr ptr = new IntPtr(pqueue.ToInt32()); // Workaround for Mono on iOS.
			pvalue.SetInternalData(Marshal.ReadIntPtr(ptr));

			ptr = new IntPtr(ptr.ToInt32() + Marshal.SizeOf(typeof(IntPtr)));
			pvalue.SetValueType((Value.ValueType)Marshal.ReadInt32(ptr));

			ptr = new IntPtr(ptr.ToInt32() + Marshal.SizeOf(typeof(Int32)));
			pvalue.SetMovieId(Marshal.ReadInt64(ptr));

			return pvalue;
		}

		private void OnExternalInterface(long MovieID, String command, IntPtr ptr, int numArgs, int valueSize)
		{
			int sfValueSize = Marshal.SizeOf(typeof(Value));
			int count = 0;

			// Array of types passed to ExternalInterface
			Type[] typeArray;
			System.Object[] args;

			// Note that we can't preallocate typeArray and args since we have to pass them to the
			// GetMethod function below and there is no way to pass the size of the array, so the
			// array can't contain any null values.
			typeArray = new Type[numArgs];
			args = new System.Object[numArgs];

			for (int i = 0; i < numArgs; i++)
			{
				// Can't add an integer offset to IntPtr as you would with C/C++ pointer
				IntPtr data = new IntPtr(ptr.ToInt32() + sfValueSize * i);

				// This Value makes a copy of the data and will be garbage collected.
				Value val = GetValueData(data);
				// Value val = (Value)Marshal.PtrToStructure(data, typeof(Value)); // Unsupported on iOS.

				if (val.IsString())
				{
					String str = val; // Implicit conversion used here
					typeArray.SetValue(typeof(String), count);
					args.SetValue(str, count);
					count++;
				}
				else if (val.IsNumber())
				{
					double num = val;
					//Console.Write(num);
					typeArray.SetValue(typeof(double), count);
					args.SetValue(num, count);
					count++;
				}
				else if (val.IsBoolean())
				{
					Boolean boolVal = val;
					typeArray.SetValue(typeof(Boolean), count);
					args.SetValue(boolVal, count);
					count++;
				}
				else if (val.IsUInt())
				{
					UInt32 uintVal = val;
					typeArray.SetValue(typeof(UInt32), count);
					args.SetValue(uintVal, count);
					count++;
				}
				else if (val.IsInt())
				{
					Int32 intVal = val;
					typeArray.SetValue(typeof(Int32), count);
					args.SetValue(intVal, count);
					count++;
				}
				else if (val.IsObject())
				{
					Value newval = val.GetObject();
					typeArray.SetValue(typeof(Value), count);
					args.SetValue(newval, count);
					count++;
				}
			}

			// Preallocated memory is destroyed in the destructor
			/* This code can be uncommented if delegates are being used for EI implementation.
			if (ptr != pValues_PreAllocated)
			{
				UnityEngine.Debug.Log("Destroying Value Array");
				Marshal.FreeCoTaskMem(ptr);
			}
			*/

			// At this point, count must be = numArgs, since we should be able to determine the type of each Value. If not,
			// there is some problem.
			if (count != numArgs)
			{
				LogMessage("Invalid Type passed in ExternalInterface!");
				return;
			}

			for (int i = 0; i < SFMovieList.Count; i++) // Loop through List with for
			{
				Movie movie = SFMovieList[i];
				long mId = movie.GetID();
				if (MovieID != mId) continue;

				Type SFMovieClass = movie.GetType();
				//UnityEngine.Debug.Log("ExternalInterface arrived");
				// Command passed as an argument is the method we want to call
				MethodInfo methodInfo;
				if (typeArray == null)
				{
#if !NETFX_CORE
					methodInfo = SFMovieClass.GetMethod(command);
#else
                methodInfo = SFMovieClass.GetRuntimeMethod(command, typeArray);         
#endif
				}
				else
				{
#if !NETFX_CORE
					methodInfo = SFMovieClass.GetMethod(command, typeArray);
#else
				methodInfo = SFMovieClass.GetRuntimeMethod(command, typeArray);
#endif
				}

				if (methodInfo != null)
				{
					// LogMessage("Method Implementing " + command + " found, Invoking method");
					methodInfo.Invoke(movie, args); // Call the method
					return;
				}
				else
				{
					UnityEngine.Debug.Log("Handler for command: " + command + " not found!");
				}
			}
		}

		/// <summary>
		/// Used to enable IME, currently not implemented
		/// </summary>
		public void EnableIME()
		{
		}

		/// <summary>
		/// Used to inform the Scaleform runtime about viewport changes (for example when the game window in the editor is resized)
		/// </summary>
		/// <param name="ox">OriginX</param>
		/// <param name="oy">OriginY</param>
		/// <param name="width">Width</param>
		/// <param name="height">Height</param>
		public void SetNewViewport(int ox, int oy, int width, int height)
		{
#if UNITY_WP8
		sf_setNewViewport(ox, oy, width, height);
#else
			SF_SetNewViewport(ox, oy, width, height);
#endif
		}

		/// <summary>
		/// Returns true if Scaleform has been initialized
		/// </summary>
		/// <returns>True if Scaleform runtime has been initialized</returns>
		public bool IsSFInitialized()
		{
			return SFInitialized;
		}

		/// <summary>
		/// For internal use only
		/// </summary>
		/// <param name="ev"></param>
		public void AddToLifecycleEventList(SFLifecycleEvent ev)
		{
			LifecycleEventsList.Add(ev);
		}

        ///// <summary>
        ///// For internal use only. When a movie destructor is called, it puts itself on a release queue. This is done because the movie destructor is called from the Garbage collector thread while the destruction of the actual C++ movie must occur on the main thread.
        ///// </summary>
        ///// <param name="movieId"></param>
        //public void AddToReleaseList(long movieId)
        //{
        //    MarkForReleaseIDs.Add(movieId);
        //}

		/// <summary>
		/// For internal use only. When a Value destructor is called, The value is put on a release queue. This is done because the Value destructor is called from the Garbage collector thread while the destruction of the underlying C++ Value must occur on the main thread.
		/// </summary>
		/// <param name="valIntPtr"></param>
		static public void AddValueToReleaseList(IntPtr valIntPtr)
		{
            lock (MarkForReleaseValues)
            {
                MarkForReleaseValues.Enqueue(valIntPtr);
            }
		}

		public class MovieComparer : IComparer<Movie>
		{
			public int Compare(Movie mv1, Movie mv2)
			{
				if (mv1.Depth == mv2.Depth) return 0;
				if (mv1.Depth < mv2.Depth) return 1;
				return -1;
			}
		}

		/// <summary>
		/// Adds a movie to a movie list managed by the SFManager. The list is sorted in depth order which enforces a rendering order on the movies. Movie with a lower depth is rendered before (meaning "under") a movie with a higher depth. Movies are added to this list automatically after they are created, the user will normally not have to call this function.
		/// </summary>
		/// <param name="movie">Movie</param>
        public void AddMovie(Movie movie)
		{
			if (SFMovieList != null)
			{
				SFMovieList.Add(movie);
				SFMovieList.Sort(new MovieComparer());
			}
		}

//        private void ReleaseMoviesMarkedForRelease()
//        {
//            if (MarkForReleaseIDs.Count == 0)
//            {
//                return;
//            }
//            for (int i = 0; i < MarkForReleaseIDs.Count; i++)
//            {
//#if UNITY_WP8
//            sf_destroyMovie(MarkForReleaseIDs[i]);
//#else
//                SF_DestroyMovie(MarkForReleaseIDs[i]);
//#endif
//            }
//            MarkForReleaseIDs.Clear();
//        }

		static private void ReleaseValuesMarkedForRelease()
		{
            lock (MarkForReleaseValues)
            {
                if (MarkForReleaseValues.Count == 0)
                {
                    return;
                }
                int number = Mathf.Min(MarkForReleaseValues.Count, 500);
                if (MarkForReleaseValues.Count > 50000)//如果积压太多就只能释放了
                {
                    number = MarkForReleaseValues.Count;
                }
                for (int i = 0; i < number; i++)
                {
                    // UnityEngine.Debug.Log("Finalizing Object");
                    IntPtr pInternalData = MarkForReleaseValues.Dequeue();
                    if (pInternalData != IntPtr.Zero)
                    {
                        // UnityEngine.Debug.Log("Releasing Internal Data");
#if UNITY_WP8
				Value.sf_decrementValRefCount(pInternalData);
#else
                        Value.SF_DecrementValRefCount(pInternalData);
#endif
                    }
                }
            }
		}

		/// <summary>
		/// Removes the movie from the SFMovieList. This ensures the manager is not holding references to the movie anymore. If there are no more references, the destructor for the movie will be called during the next garbage collection cycle, enabling the C++ movie to be destroyed. 
		/// </summary>
		/// <param name="movie">Movie</param>
		public void RemoveMovie(Movie movie)
		{
			SFMovieList.Remove(movie);
		}

        //public void ForceDestroyMovies()
        //{
        //    for (int i = 0; i < SFMovieList.Count; i++)
        //    {
        //        SF_DestroyMovie(SFMovieList[i].MovieID);
        //    }
        //}


		public void DoCleanup()
		{
			for (int i = 0; i < SFMovieList.Count; i++)
			{
                GameObject.Destroy(SFMovieList[i].gameObject);
			}
			SFMovieList.Clear();
		}
		/*
		public T CreateMovie <T> (SFMovieCreationParams params) where T : Movie
		{
			Activator.CreateInstance (typeof (T), argslist);
		}
		*/

		/// <summary>
		/// A Movie is created by this function. You need to specify the creation parameters (see doc for SFMovieCreationParams for details) and the MovieClassType. The MovieClassType is the type of a Movie subclass which implements the external interface callbacks for this movie. If the movie is not expected to handle any external interface callbacks, the MovieClassType can simple by typeof(Movie)
		/// </summary>
		/// <param name="creationParams"> Creation Params. See SFMovieCreationParams for details</param>
		/// <param name="MovieClassType"> Movie subclass </param>
		/// <returns></returns>
		public Movie CreateMovie(SFMovieCreationParams creationParams, Type MovieClassType)
		{
			Type[] argTypes = new Type[] { typeof(SFManager), typeof(SFMovieCreationParams) };
			object[] argVals = new object[] { this, creationParams };
			ConstructorInfo cInfo = null;
#if !NETFX_CORE
			cInfo = MovieClassType.GetConstructor(argTypes);
#else
        cInfo = (ConstructorInfo)MovieClassType.GetTypeInfo().DeclaredConstructors;
#endif
			UnityEngine.Debug.Log(MovieClassType);
			UnityEngine.Debug.Log(cInfo);

			if (cInfo == null)
			{
				return null;
			}
			Movie movie = (Movie)cInfo.Invoke(argVals);
			return movie;
		}

		////////////////// Layering System ////////////////////////

		/// <summary>
		/// Used to resort the movielist. Should be called every time the depth of a movie is modified. This is done automatically by SFMovie::SetDepth(), and the user typcially will not need to call this function.  
		/// </summary>
		public void ReorderMovies()
		{
			// Resort the movies
			if (SFMovieList != null)
			{
				SFMovieList.Sort(new MovieComparer());
			}
		}

		/// <summary>
		/// Returns a list of all movies with depth = SFMovieList[0]
		/// </summary>
		/// <returns></returns>
		public List<Movie> GetTopMovie()
		{
			List<Movie> topMovies = new List<Movie>();
			if (SFMovieList != null && SFMovieList.Count != 0)
			{
				topMovies.Add(SFMovieList[0]);
				int cur = 1;
				while (cur < SFMovieList.Count && (SFMovieList[cur].Depth == SFMovieList[0].Depth))
				{
					topMovies.Add(SFMovieList[cur]);
					cur++;
				}
				return topMovies;
			}

			return null;
		}


		/// <summary>
		/// Returns the next movie in depth order. Returns mv if it is already at bottom of the list.
		/// </summary>
		/// <param name="mv">The next movie in depth order</param>
		/// <example>Sample code:
		/// <code>
		/// Movie nextMovie = SFMgr.GetNextMovie(topMovie);
		///	Movie currMovie = topMovie;
		///	while (nextMovie != currMovie)
		///	{
		///		currMovie = nextMovie;
		///		UnityEngine.Debug.Log(nextMovie.GetDepth());
		///		nextMovie = SFMgr.GetNextMovie(nextMovie);
		///	}
		/// </code>
		/// </example>
		/// <returns></returns>
		public Movie GetNextMovie(Movie mv)
		{

			if (SFMovieList != null && SFMovieList.Count != 0)
			{
				int numMovies = SFMovieList.Count;
				// Find index of mv
				int idx = SFMovieList.FindIndex(delegate(Movie _mv) { return _mv == mv; });
				if (idx == numMovies - 1) // Already at the bottom
					return mv;
				return SFMovieList[idx + 1];
			}
			return null;
		}

		/// <summary>
		/// Returns a list of all movies with depth = SFMovieList[numMovies-1]
		/// </summary>
		/// <returns></returns>
		public List<Movie> GetBottomMovie()
		{
			List<Movie> bottomMovies = new List<Movie>();
			if (SFMovieList != null && SFMovieList.Count != 0)
			{
				int numMovies = SFMovieList.Count;
				bottomMovies.Add(SFMovieList[numMovies - 1]);
				int cur = numMovies - 2;
				while (cur >= 0 && (SFMovieList[cur].Depth == SFMovieList[numMovies - 1].Depth))
				{
					bottomMovies.Add(SFMovieList[cur]);
					cur--;
				}
				return bottomMovies;
			}
			return null;
		}
		/// <summary>
		/// Returns the number of movies in the list.
		/// </summary>
		/// <returns>Number of movies.</returns>
		public int GetNumMovies()
		{
			if (SFMovieList != null)
			{
				return SFMovieList.Count;
			}
			return -1;
		}

		/// //////////////////////// End Layering System //////////////////

		private List<Movie> GetMovies()
		{
			return SFMovieList;
		}

		/// <summary>
		/// The Update function calls the Update functions for the Movies. You can override Update in your Movie subclasses to implement application specific logic
		/// </summary>
		public void Update()
		{
			if (!IsSFInitialized())
			{
				return;
			}
#if UNITY_WP8
		sf_processMarkedForDeleteMovies();
#else
			SF_ProcessMarkedForDeleteMovies();
#endif
			//ReleaseMoviesMarkedForRelease();
			ReleaseValuesMarkedForRelease();
            //Movie 改为继承自MonoBehaviour，自己会调用Update，这里暂时不需要
            //for (int i = 0; i < SFMovieList.Count; i++)
            //{
            //	SFMovieList[i].Update();
            //}
        }

        /// <summary>
        /// Perform a hit test against Movies in the scene. This returns the top-most Movie instance hit or null, to indicate one could not be found.
        /// </summary>
        /// <param name="x">Mouse x coordinate in Unity screen space</param>
        /// <param name="y">Mouse y coordinate in Unity screen space</param>
        /// <example>Sample code:
        /// <code>
        /// Movie movie = GetSFManager().DoHitTest( InputManager.MousePosition.x,  InputManager.MousePosition.y);
        ///	if (movie != null)
        ///	{
        ///		// ...
        ///	}
        /// </code>
        /// </example>
        /// <returns>
        /// The top most movie instance under the mouse coordinates or null if one cannot be found.
        /// </returns>
        public Movie DoHitTest(float x, float y)
		{
			if (!IsSFInitialized())
			{
				return null;
			}

			for (int i = 0; i < SFMovieList.Count; i++)
			{
				if (SFMovieList[i].DoHitTest(x, y, HitTestType.HitTest_ShapesNoInvisible))
				{
                    SFMovieEx hitTestMV = (SFMovieList[i] as SFMovieEx);
                    if (Event.current.type == EventType.MouseDown && hitTestMV.IsFloat && hitTestMV != PopupMovieManager.Instance.GetTopMovie())
                    {
                        SFMovieEx popUpTopMv = PopupMovieManager.Instance.GetTopMovie();

                        PopupMovieManager.Instance.InsertMovie(popUpTopMv, hitTestMV);

                    }
                    return SFMovieList[i];
				}
			}

			return null;
		}

        /// <summary>
        /// Perform a hit test against Movies in the scene. This returns the top-most Movie instance hit or null, to indicate one could not be found.
        /// </summary>
        /// <param name="x">Mouse x coordinate in scaleform screen space</param>
        /// <param name="y">Mouse y coordinate in scaleform screen space</param>
        /// <example>Sample code:
        /// <code>
        /// Movie movie = GetSFManager().DoHitTest( InputManager.MousePosition.x,  InputManager.MousePosition.y);
        ///	if (movie != null)
        ///	{
        ///		// ...
        ///	}
        /// </code>
        /// </example>
        /// <returns>
        /// The top most movie instance under the mouse coordinates or null if one cannot be found.
        /// </returns>
        public Movie DoHitTestEx(float x, float y,HitTestType _hitTestType)
        {
            if (!IsSFInitialized())
            {
                return null;
            }

            for (int i = 0; i < SFMovieList.Count; i++)
            {
                if (SFMovieList[i].DoHitTestEx(x, y, _hitTestType))
                {
                    return SFMovieList[i];
                }
            }

            return null;
        }

		/// <summary>
		/// Used to replace a named Flash texture with a Unity texture
		/// </summary>
		/// <param name="movieId">MovieId</param>
		/// <param name="textureName">texture name. This is the class name assigned to the symbol in the ActionScript linkage property box</param>
		/// <param name="texture">Unity texture</param>
		/// <returns>True if the operation succeeded</returns>
		public bool ReplaceTexture(long movieId, String textureName, Texture texture)
		{
			int texId;
			int rt_with = texture.width;
			int rt_height = texture.height;

#if !(UNITY_3_5)
#if UNITY_IPHONE
        texId = texture.GetNativeTextureID();
#else
			IntPtr texPtr = texture.GetNativeTexturePtr();
			texId = (int)(texPtr);
#endif
#else
        texId = texture.GetNativeTextureID();
#endif
#if UNITY_WP8
        return sf_replaceTexture(movieId, textureName, texId, rt_with, rt_height);
#else
			return SF_ReplaceTexture(movieId, textureName, texId, rt_with, rt_height);
#endif
		}

		/// <summary>
		/// This function is used to apply the fontconfig index specified by the langName in order to support localization. See our font configuration doc for more details.
		/// </summary>
		/// <param name="langName">Language Name</param>
		public void ApplyLanguage(String langName)
		{
#if UNITY_WP8
        sf_applyLanguage(langName);
#else
			SF_ApplyLanguage(langName);
#endif
		}

		/// <summary>
		/// Advances the movies. This is called automatically by the SFCamera during the Update call of the game object to which the Camera is attached. 
		/// </summary>
		/// <param name="deltaTime">Time elapsed since the last time Advance was called. Used to ensure that the movies are advanced according to their framerate</param>
		public void Advance(float deltaTime)
		{
			if (!IsSFInitialized())
			{
				return;
			}

			// Check if viewport coordinates have changed
			int newScreenWidth = Screen.width;
			int newScreenHeight = Screen.height;
			int ox = 0;
			int oy = 0;
#if UNITY_IPHONE
        oy = 0;
#else
			oy = 24; // Note that while using D3D renderer, the tool bar (that contains "Maximize on Play" text) is part of
			// the viewport, while using GL renderer, it's not. So there should be a further condition here depending on
			// what type of renderer is being used, however I couldn't find a macro for achieving that.
#endif

			if (ScreenWidth != newScreenWidth || ScreenHeight != newScreenHeight)
			{
				//UnityEngine.Debug.Log("ScreenWidth = " + newScreenWidth + "ScreenHeight = " + newScreenHeight);
                Trace.Log("ScreenWidth = " + newScreenWidth + "ScreenHeight = " + newScreenHeight);
				ScreenHeight = newScreenHeight;
				ScreenWidth = newScreenWidth;
#if UNITY_WP8
            sf_setNewViewport(ox, oy, ScreenWidth, ScreenHeight);
#else
				SF_SetNewViewport(ox, oy, ScreenWidth, ScreenHeight);
#endif
			}


            for (int i = 0; i < SFMovieList.Count; i++)
			{
				SFMovieList[i].Advance(deltaTime);
			}
			if (LifecycleEventsList != null)
			{
				for (int i = 0; i < LifecycleEventsList.Count; i++) // Loop through List with for
				{
					LifecycleEventsList[i].Execute();
				}
			}
			//ReleaseMoviesMarkedForRelease();
			ReleaseValuesMarkedForRelease();
		}

		private void Display()
		{
			if (!IsSFInitialized())
			{
				return;
			}
#if UNITY_WP8
        sf_display();
#else
			SF_Display();
#endif
			// This indicates to Unity that render states might have changed and it can't assume anything about previous render states
			GL.InvalidateState();
		}

		/// <summary>
		/// Used to install delegates which can be used to directly call C# methods from C++. However delegates don't work on iOS, so they are currently not used
		/// </summary>
		public void InstallDelegates()
		{
			EIDelegate = this.OnExternalInterface;
#if UNITY_WP8
        sf_setExternalInterfaceDelegate(EIDelegate);
#else
			SF_SetExternalInterfaceDelegate(EIDelegate);
#endif
			AllocDelegate = new SF_AllocateDelegate(this.AllocateImpl);
#if UNITY_WP8
        sf_setAllocateValues(AllocDelegate);
#else
			SF_SetAllocateValues(AllocDelegate);
#endif
			LogDelegate = new SF_LogDelegate(this.OnLogMessage);
#if UNITY_WP8
        sf_setLogDelegate(LogDelegate);
#else
			SF_SetLogDelegate(LogDelegate);
#endif
			DisplayInfoDelegate = new SF_DisplayInfoDelegate(this.AllocateDisplayInfo);
#if UNITY_WP8
        sf_setDisplayInfoDelegate(DisplayInfoDelegate);
#else
			SF_SetDisplayInfoDelegate(DisplayInfoDelegate);
#endif
		}

		/// <summary>
		/// Passes current mouse coordinates to different movies which in turn pass them to the runtime
		/// </summary>
		/// <param name="x"></param>
		/// <param name="y"></param>
		/// <returns></returns>
		public bool HandleMouseMoveEvent(float x, float y)
		{
			if (!IsSFInitialized())
			{
				return false;
			}

			int icase = 3;
#if UNITY_EDITOR
			y = y + 24; // Need to offset by height of title bar
#endif
			bool handled = false;
			for (int i = 0; i < SFMovieList.Count; i++)
			{
				// Movies can set breturnOnHandled to true if they don't want other movies to process the event further
				bool breturnOnHandled = false;
				handled = SFMovieList[i].HandleMouseEvent(x, y, icase, 0, 0, ref breturnOnHandled);
				if (breturnOnHandled && handled)
				{
					return handled;
				}
			}

			return handled;
		}


		/// <summary>
		/// Handles mouse move/up/down event. The events are passed to the movies which in turn pass them to the Scaleform runtime
		/// </summary>
		/// <param name="ev">Unity event</param>
		/// <returns>true if the event was handled by Scaleform. If you want the event to only be processed by the first movie that handles it, set the breturnOnHandled flag to true</returns>
		public bool HandleMouseDownAndUpEvent(UnityEngine.Event ev)
		{
			if (!IsSFInitialized())
			{
				return false;
			}

			int icase = 0;
			Vector2 mousePos = ev.mousePosition;
			switch (ev.type)
			{
				case EventType.MouseDown:
					icase = 1;
					break;
				case EventType.MouseUp:
					icase = 2;
					break;
				case EventType.MouseMove:
					icase = 3;
					break;
			}

            long curHitTestMovieID = -1;
            if (SFMovieList.Count > 0)
            {
                if (icase == 1 || icase == 2)
                {
                    Movie mv = DoHitTest(InputManager.MousePosition.x, InputManager.MousePosition.y);
                    curHitTestMovieID = mv == null ? 0 : mv.GetID();
                }
            }

#if UNITY_EDITOR
			mousePos[1] = mousePos[1] + 24; // Need to offset by height of title bar
#endif
			bool handled = false;

			for (int i = 0; i < SFMovieList.Count; i++)
			{
				// Movies can set breturnOnHandled to true if they don't want other movies to process the event further
				bool breturnOnHandled = false;
                if (curHitTestMovieID == SFMovieList[i].GetID())
                {
                    handled = SFMovieList[i].HandleMouseEvent(mousePos[0], mousePos[1], icase, ev.button, 0, ref breturnOnHandled);
                }

				if (breturnOnHandled && handled)
				{
					return handled;
				}
			}

			return handled;
		}
		
		/// <summary>
		/// Handles mouse scroll wheel event.
		/// </summary>
		/// <param name="ev">Unity event</param>
		/// <returns>true if the event was handled by Scaleform. If you want the event to only be processed by the first movie that handles it, set the breturnOnHandled flag to true</returns>		
		public bool HandleMouseWheelEvent(UnityEngine.Event ev)
		{
			bool handled = false;
			
			if (!IsSFInitialized())
			{
				return handled;
			}
			
			int icase = 4;
			
			Vector2 mousePosition = ev.mousePosition;
#if UNITY_EDITOR
			mousePosition.y = mousePosition.y + 24; // Need to offset by height of title bar
#endif
						
			for (int i = 0; i < SFMovieList.Count; i++)
			{
				// Movies can set breturnOnHandled to true if they don't want other movies to process the event further
				bool breturnOnHandled = false;
				
				// In order to match what the Scaleform runtime expects,
				// we need to invert the wheel delta.				
				handled = SFMovieList[i].HandleMouseEvent(mousePosition.x, mousePosition.y, icase, 0, -ev.delta.y, ref breturnOnHandled);
				if (breturnOnHandled && handled)
				{
					return handled;
				}
			}			
			


			return handled;
		}

		/// <summary>
		/// Handles touch events. The events are passed to the movies which in turn pass them to the Scaleform runtime
		/// </summary>
		/// <param name="ev">Unity event</param>
		/// <returns>true if the event was handled by Scaleform. If you want the event to only be processed by the first movie that handles it, set the breturnOnHandled flag to true</returns>
		public bool HandleTouchEvent(UnityEngine.Touch touch)
		{
			if (!IsSFInitialized())
			{
				return false;
			}

			int icase = 0;
			switch (touch.phase)
			{
				case TouchPhase.Began:
					icase = 1;
					break;
				case TouchPhase.Moved:
					icase = 2;
					break;
				case TouchPhase.Ended:
					icase = 3;
					break;
			}
			bool handled = false;

			for (int i = 0; i < SFMovieList.Count; i++)
			{
				// Movies can set breturnOnHandled to true if they don't want other movies to process the event further
				bool breturnOnHandled = false;
				handled = SFMovieList[i].HandleTouchEvent(touch.fingerId, touch.position[0], touch.position[1], icase, ref breturnOnHandled);
				if (breturnOnHandled && handled)
				{
					return handled;
				}
			}

			return handled;
		}

		/// <summary>
		/// Handles key down events
		/// </summary>
		/// <param name="code">Key Code</param>
		/// <param name="mod">Key Modifiers</param>
		/// <param name="keyboardIndex">Keyboard Index</param>
		/// <returns></returns>
		public bool HandleKeyDownEvent(SFKey.Code code, SFKeyModifiers.Modifiers mod = 0, int keyboardIndex = 0)
		{
			if (!IsSFInitialized())
			{
				return false;
			}

			bool handled = false;

			for (int i = 0; i < SFMovieList.Count; i++)
			{
				// Movies can set breturnOnHandled to true if they don't want other movies to process the event further
				bool breturnOnHandled = false;
				handled = SFMovieList[i].HandleKeyEvent(code, mod, 1, ref breturnOnHandled, keyboardIndex);

				if (breturnOnHandled && handled)
				{
					return handled;
				}
			}

			return handled;
		}


		/// <summary>
		/// Handles key up events
		/// </summary>
		/// <param name="code">Key Code</param>
		/// <param name="mod">Key Modifiers</param>
		/// <param name="keyboardIndex">Keyboard Index</param>
		/// <returns></returns>
		public bool HandleKeyUpEvent(SFKey.Code code, SFKeyModifiers.Modifiers mod = 0, int keyboardIndex = 0)
		{
			if (!IsSFInitialized())
			{
				return false;
			}

			bool handled = false;

			for (int i = 0; i < SFMovieList.Count; i++)
			{
				bool breturnOnHandled = false;
				handled = SFMovieList[i].HandleKeyEvent(code, mod, 0, ref breturnOnHandled, keyboardIndex);
				if (breturnOnHandled && handled)
				{
					return handled;
				}
			}

			return handled;
		}

		/// <summary>
		/// Handles key down events
		/// </summary>
		/// <param name="ev">Unity key event</param>
		/// <returns></returns>
		public bool HandleKeyDownEvent(UnityEngine.Event ev)
		{
			if (!IsSFInitialized())
			{
				return false;
			}

			SFKey.Code cd = SFKey.GetSFKeyCode(ev.keyCode);
			SFKeyModifiers.Modifiers modifiers = SFKey.GetSFModifiers(ev.modifiers);
			return HandleKeyDownEvent(cd, modifiers);
		}

		/// <summary>
		/// Handles key up events
		/// </summary>
		/// <param name="ev">Unity key event</param>
		/// <returns></returns>
		public bool HandleKeyUpEvent(UnityEngine.Event ev)
		{
			if (!IsSFInitialized())
			{
				return false;
			}

			SFKey.Code cd = SFKey.GetSFKeyCode(ev.keyCode);
			SFKeyModifiers.Modifiers modifiers = SFKey.GetSFModifiers(ev.modifiers);

			return HandleKeyUpEvent(cd, modifiers);
		}


		public bool HandleCharEvent(UnityEngine.Event ev)
		{
			if (!IsSFInitialized())
			{
				return false;
			}

			bool handled = false;
			UInt32 wchar = ev.character;

			for (int i = 0; i < SFMovieList.Count; i++)
			{
				bool breturnOnHandled = false;
				handled = SFMovieList[i].HandleCharEvent(wchar, ref breturnOnHandled);
				if (breturnOnHandled && handled)
				{
					return handled;
				}
			}
			return handled;
		}

		private void PrintAddress(System.Object o)
		{
			GCHandle h = GCHandle.Alloc(o, GCHandleType.Pinned);
			IntPtr addr = h.AddrOfPinnedObject();
			UnityEngine.Debug.Log(addr.ToString("x"));
			h.Free();
		}

        public void SyncTickCountToAS()
        {
            for (int i = 0; i < SFMovieList.Count; i++)
            {
                SFMovieList[i].SyncTickCountToAS(DeltaSyncTickCount);
            }
        }
	}

} // namespace Scaleform;
