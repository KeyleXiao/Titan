using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

public static class ImageExtensions
{
    //追加自定义数据至图片
    public static bool AppendDataToJPG(string filePath, byte[] imgSourceData, byte[] customData)
    {
        if (filePath == "")
        {
            Trace.Warning("JPG filePath 为空！");
            return false;
        }

        byte[] newBytes = new byte[imgSourceData.Length + customData.Length];

        Array.Copy(imgSourceData, newBytes, imgSourceData.Length);
        Array.Copy(customData, 0, newBytes, imgSourceData.Length, customData.Length);

        File.WriteAllBytes(filePath, newBytes);
        return true;
    }

    public static byte[] GetAllByteData(string filePath)
    {
        if (filePath == "")
            return null;

        if (!File.Exists(filePath))
            return null;

        FileStream fileStream = new FileStream(filePath, FileMode.Open, FileAccess.Read);
        fileStream.Seek(0, SeekOrigin.Begin);

        if (fileStream.Length <= 0)
            return null;

        byte[] bytes = new byte[fileStream.Length];
        fileStream.Read(bytes, 0, (int)fileStream.Length);
        fileStream.Close();
        fileStream.Dispose();
        fileStream = null;

        return bytes;
    }

    //获取自定义数据
    public static T GetCustomData<T>(byte[] imgData)where T : struct
    {
        Type t = typeof(T);
        int nTypeSize = Marshal.SizeOf(t);

        if (imgData.Length < nTypeSize)
            return default(T);

        int index = imgData.Length - nTypeSize;
        byte[] bytArr = new byte[nTypeSize];

        for (int i = 0; i < bytArr.Length; i++)
        {
            bytArr[i] = imgData[index + i];
        }

        T customData = new T();

        customData = BytesToStruct<T>(bytArr);

        return customData;
    }

    public static byte[] StructToBytes(object structObj)
    {
        int size = Marshal.SizeOf(structObj);
        IntPtr buffer = Marshal.AllocHGlobal(size);
        try
        {
            Marshal.StructureToPtr(structObj, buffer, false);
            byte[] bytes = new byte[size];
            Marshal.Copy(buffer, bytes, 0, size);
            return bytes;
        }
        finally
        {
            Marshal.FreeHGlobal(buffer);
        }
    }

    public static T BytesToStruct<T>(byte[] bytes) where T : struct
    {
        Type t = typeof(T);
        int size = Marshal.SizeOf(t);
        IntPtr buffer = Marshal.AllocHGlobal(size);
        try
        {
            Marshal.Copy(bytes, 0, buffer, size);
            object obj = Marshal.PtrToStructure(buffer, t);
            if (obj != null)
                return (T)obj;
            else
                return default(T);
        }
        finally
        {
            Marshal.FreeHGlobal(buffer);
        }
    }
}

