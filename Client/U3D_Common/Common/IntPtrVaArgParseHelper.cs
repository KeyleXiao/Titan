/*******************************************************************
** 文件名:	IntPtrVaArgParseHelper.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-11-3
** 版  本:	1.0
** 描  述:	IntPtr扩展和可变参数处理

********************************************************************/
using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;

/// <summary>
/// IntPtr扩展
/// </summary>
public static class IntPtrExtensions
{
    public static IntPtr Decrement(this IntPtr pointer, Int32 value)
    {
        return Increment(pointer, -value);
    }
    public static IntPtr Decrement(this IntPtr pointer, Int64 value)
    {
        return Increment(pointer, -value);
    }
    public static IntPtr Decrement(this IntPtr pointer, IntPtr value)
    {
        switch (IntPtr.Size)
        {
            case sizeof(Int32):
                return (new IntPtr(pointer.ToInt32() - value.ToInt32()));

            default:
                return (new IntPtr(pointer.ToInt64() - value.ToInt64()));
        }
    }

    public static IntPtr Increment(this IntPtr pointer, Int32 value)
    {
        unchecked
        {
            switch (IntPtr.Size)
            {
                case sizeof(Int32):
                    return (new IntPtr(pointer.ToInt32() + value));

                default:
                    return (new IntPtr(pointer.ToInt64() + value));
            }
        }
    }
    public static IntPtr Increment(this IntPtr pointer, Int64 value)
    {
        unchecked
        {
            switch (IntPtr.Size)
            {
                case sizeof(Int32):
                    return (new IntPtr((Int32)(pointer.ToInt32() + value)));

                default:
                    return (new IntPtr(pointer.ToInt64() + value));
            }
        }
    }
    public static IntPtr Increment(this IntPtr pointer, IntPtr value)
    {
        unchecked
        {
            switch (IntPtr.Size)
            {
                case sizeof(int):
                    return new IntPtr(pointer.ToInt32() + value.ToInt32());
                default:
                    return new IntPtr(pointer.ToInt64() + value.ToInt64());
            }
        }
    }
    
    public static Boolean Equals(this IntPtr pointer, Int32 value)
    {
        return (pointer.ToInt32() == value);
    }
    public static Boolean Equals(this IntPtr pointer, Int64 value)
    {
        return (pointer.ToInt64() == value);
    }
    public static Boolean Equals(this IntPtr left, IntPtr ptr2)
    {
        return (left == ptr2);
    }


    public static IntPtr And(this IntPtr pointer, IntPtr value)
    {
        switch (IntPtr.Size)
        {
            case sizeof(Int32):
                return (new IntPtr(pointer.ToInt32() & value.ToInt32()));

            default:
                return (new IntPtr(pointer.ToInt64() & value.ToInt64()));
        }
    }
    public static IntPtr Not(this IntPtr pointer)
    {
        switch (IntPtr.Size)
        {
            case sizeof(Int32):
                return (new IntPtr(~pointer.ToInt32()));

            default:
                return (new IntPtr(~pointer.ToInt64()));
        }
    }
    public static IntPtr Or(this IntPtr pointer, IntPtr value)
    {
        switch (IntPtr.Size)
        {
            case sizeof(Int32):
                return (new IntPtr(pointer.ToInt32() | value.ToInt32()));

            default:
                return (new IntPtr(pointer.ToInt64() | value.ToInt64()));
        }
    }
    public static IntPtr Xor(this IntPtr pointer, IntPtr value)
    {
        switch (IntPtr.Size)
        {
            case sizeof(Int32):
                return (new IntPtr(pointer.ToInt32() ^ value.ToInt32()));

            default:
                return (new IntPtr(pointer.ToInt64() ^ value.ToInt64()));
        }
    }
}

/// <summary>
/// 可变参数解析
/// </summary>
public class IntPtrVaArgParseHelper
{
    private IntPtr m_ptr;       // 非托管内存地址
    private int m_nPtrLen;      // 非托管内存长度
    private int m_nOffset;      // 当前所指偏移


    /// <summary>
    /// 构造函数
    /// </summary>
    /// <param name="ptr">非托管内存地址</param>
    /// <param name="nPtrLen">非托管内存长度</param>
    public IntPtrVaArgParseHelper(IntPtr ptr, int nPtrLen)
    {
        m_ptr = ptr;
        m_nPtrLen = nPtrLen;
        m_nOffset = 0;
    }

    public IntPtr Current
    {
        get
        {
            if (m_ptr == null)
                return IntPtr.Zero;
            if (m_nPtrLen <= m_nOffset)
                return IntPtr.Zero;

            return IntPtrExtensions.Increment(m_ptr, m_nOffset);
        }
    }

    private int getRemainLength()
    {
        return m_nPtrLen - m_nOffset;
    }


    /// <summary>
    /// 获取一个byte值
    /// </summary>
    /// <returns></returns>
    public byte getByte()
    {
        int type_size = sizeof(byte);
        int nRemainLength = getRemainLength();
        if (nRemainLength < type_size)
            return 0;

        IntPtr currentPtr = Current;
        if (currentPtr == IntPtr.Zero)
            return 0;
        
        m_nOffset += type_size;
        return Marshal.ReadByte(currentPtr);
    }

    /// <summary>
    /// 获取一个short值
    /// </summary>
    /// <returns></returns>
    public int getShort()
    {
        int type_size = sizeof(short);
        int nRemainLength = getRemainLength();
        if (nRemainLength < type_size)
            return 0;

        IntPtr currentPtr = Current;
        if (currentPtr == IntPtr.Zero)
            return 0;

        m_nOffset += type_size;
        return Marshal.ReadInt16(currentPtr);
    }

    /// <summary>
    /// 获取一个int值
    /// </summary>
    /// <returns></returns>
    public int getInt()
    {
        int type_size = sizeof(int);
        int nRemainLength = getRemainLength();
        if (nRemainLength < type_size)
            return 0;

        IntPtr currentPtr = Current;
        if (currentPtr == IntPtr.Zero)
            return 0;

        m_nOffset += type_size;
        return Marshal.ReadInt32(currentPtr);
    }

    /// <summary>
    /// 获取一个Int64值
    /// </summary>
    /// <returns></returns>
    public Int64 getInt64()
    {
        int type_size = sizeof(Int64);
        int nRemainLength = getRemainLength();
        if (nRemainLength < type_size)
            return 0;

        IntPtr currentPtr = Current;
        if (currentPtr == IntPtr.Zero)
            return 0;

        m_nOffset += type_size;
        return Marshal.ReadInt64(currentPtr);
    }

    /// <summary>
    /// 获取一个float值
    /// </summary>
    /// <returns></returns>
    public float getFloat()
    {
        int type_size = sizeof(float);
        int nRemainLength = getRemainLength();
        if (nRemainLength < type_size)
            return 0.0f;

        IntPtr currentPtr = Current;
        if (currentPtr == IntPtr.Zero)
            return 0.0f;

        m_nOffset += type_size;

        return (float)Marshal.PtrToStructure(currentPtr, typeof(float));
    }


    /// <summary>
    /// 获取一个double值
    /// </summary>
    /// <returns></returns>
    public double getDouble()
    {
        int type_size = sizeof(double);
        int nRemainLength = getRemainLength();
        if (nRemainLength < type_size)
            return 0.0d;

        IntPtr currentPtr = Current;
        if (currentPtr == IntPtr.Zero)
            return 0.0d;

        m_nOffset += type_size;

        return (double)Marshal.PtrToStructure(currentPtr, typeof(double));
    }

    public string getString(int nStrLen)
    {
        int nRemainLength = getRemainLength();
        if (nRemainLength < nStrLen)
            return string.Empty;

        IntPtr currentPtr = Current;
        if (currentPtr == IntPtr.Zero)
            return string.Empty;

        // 读取字符串
        byte[] arr = new byte[nStrLen];
        Marshal.Copy(currentPtr, arr, 0, nStrLen);

        m_nOffset += nStrLen;

        return System.Text.Encoding.Default.GetString(arr);
    }

    public string getUTF8String(int nStrLen)
    {
        int nRemainLength = getRemainLength();
        if (nRemainLength < nStrLen)
            return string.Empty;

        IntPtr currentPtr = Current;
        if (currentPtr == IntPtr.Zero)
            return string.Empty;

        // 读取字符串
        byte[] arr = new byte[nStrLen];
        Marshal.Copy(currentPtr, arr, 0, nStrLen);

        m_nOffset += nStrLen;

        return System.Text.Encoding.UTF8.GetString(arr);
    }

    public T get<T> () where T : struct
    {
        Type t = typeof(T);
        int nTypeSize = Marshal.SizeOf(t);
        int nRemainLength = getRemainLength();
        if (nRemainLength < nTypeSize)
            return default(T);

        IntPtr currentPtr = Current;
        if (currentPtr == IntPtr.Zero)
            return default(T);

        object obj = Marshal.PtrToStructure(currentPtr, t);        
        if (obj != null)
            m_nOffset += nTypeSize;

        return (T)obj;
    }
}