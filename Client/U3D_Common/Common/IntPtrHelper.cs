using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;


/// <summary>
/// 封装托管代码与非托管代码的互相调用
/// 主要通过IntPtr来读写结构和字符串
/// </summary>
public class IntPtrHelper
{
    protected IntPtr mPtr;

    public IntPtrHelper()
    {
        mPtr = IntPtr.Zero;
    }

    ~IntPtrHelper()
    {
        Close();
    }

    void Close()
    {
        if (mPtr != IntPtr.Zero)
        {
            Marshal.FreeHGlobal(mPtr);
            mPtr = IntPtr.Zero;
        }
    }

    /// <summary>
    /// 一个结构对象转换成IntPtr
    /// </summary>
    /// <typeparam name="T">类型</typeparam>
    /// <param name="data">数据</param>
    /// <returns></returns>
    public IntPtr toPtr<T>(ref T data)
    {
        Close();
        mPtr = Marshal.AllocHGlobal(Marshal.SizeOf(data));
        Marshal.StructureToPtr(data, mPtr,false);
        return mPtr;
    }

    // 从一个IntPtr中读取结构
    public static T toData<T>(IntPtr ptr)
    {
        T data = (T)Marshal.PtrToStructure(ptr, typeof(T));
        return data;
    }

    // 把一个字符串生成IntPtr
    public IntPtr Str2Ptr(string str)
    {
        Close();
        mPtr = Marshal.StringToHGlobalAnsi(str);            // 到底是这个要freeHglobal还是下面要，搞不清楚
        return mPtr;
    }

    // 从一个IntPtr中读取字符串
    public static string Ptr2Str(IntPtr ptr)
    {
        return Marshal.PtrToStringAnsi(ptr);
    }
}