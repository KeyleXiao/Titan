/**********************************************************************

Filename    :	SFDisplayMatrix.cs
Content     :	Definition of DisplayMatrix Wrapper
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
/// <summary>
/// Maps to the DisplayMatrix class in Scaleform. Used to set the DisplayMatrix for a display object.
/// </summary>
    
[StructLayout(LayoutKind.Sequential)]
public class  SFDisplayMatrix: System.Object
{
    public IntPtr DataPtr;
    
    public SFDisplayMatrix()
    {
        int numRows = 2;
        int numCols = 4;
        int floatSize = Marshal.SizeOf(typeof(float));
        DataPtr = Marshal.AllocCoTaskMem(floatSize * numRows * numCols);
    }

    ~SFDisplayMatrix()
    {
        Marshal.FreeCoTaskMem(DataPtr);
    }

    public void Print()
    {
        float m00 = GetDisplayMatrixElem(0,0); float m01 = GetDisplayMatrixElem(0,1);
        float m02 = GetDisplayMatrixElem(0,2); float m03 = GetDisplayMatrixElem(0,3);
        float m10 = GetDisplayMatrixElem(1,0); float m11 = GetDisplayMatrixElem(1,1);
        float m12 = GetDisplayMatrixElem(1,2); float m13 = GetDisplayMatrixElem(1,3);
        
        UnityEngine.Debug.Log(m00 + " " + m01 + " " + m02 + " " + m03);
        UnityEngine.Debug.Log(m10 + " " + m11 + " " + m12 + " " + m13);
    }
    
    // Accessors for view matrix and projection matrices
    public float GetDisplayMatrixElem(int row, int col)
    {
        int numRows     = 2;
        int numCols     = 4;
        if (row < 0 || row >= numRows || col < 0 || col >= numCols) return -1;
        
        float[] retVal  = new float[1];
        int floatSize   = Marshal.SizeOf(typeof(float));
        IntPtr psrc     = new IntPtr(DataPtr.ToInt32() + floatSize * (row * numCols + col));
        Marshal.Copy(psrc, retVal, 0, 1);
        return retVal[0];
    }

    // Accessors for view matrix and projection matrices
    public bool SetDisplayMatrixElem(float val, int row, int col)
    {
        int numRows = 2;
        int numCols = 4;
        if (row < 0 || row >= numRows || col < 0 || col >= numCols) return false;

        float[] arr = new float[1]{val};
        int floatSize = Marshal.SizeOf(typeof(float));
        IntPtr psrc = new IntPtr(DataPtr.ToInt32() + floatSize * (row * numCols + col));
        Marshal.Copy(arr, 0, psrc, 1);
        return true;
    }
}

} // namespace Scaleform;



 