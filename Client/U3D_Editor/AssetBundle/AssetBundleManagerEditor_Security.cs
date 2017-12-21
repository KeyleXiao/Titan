
/*******************************************************************
** 文件名:	AssetBundleManagerEditor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源系统
** 应  用:  	ab资源管理器编辑器

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using System.IO;
using System;
using LZ4ps;
using System.Text;
using System.Linq;
using System.Runtime.InteropServices;

public partial class AssetBundleManagerEditor : EditorWindow
{
    public class AssetFileInfo
    {
        public int fileLengthInBytes = 0;
        public float fileLengthInMB = 0.0f;
        public string md5 = string.Empty;
    }
    const uint CRYPT_TYPE_EMPTY		=0u;
    const uint CRYPT_TYPE_MAP			=2;	/// 字节映射加密类型，对加密后进行的压缩损害小
    const uint CRYPT_TYPE_XOR			=4;	/// 复杂异或加密类型，对加密后进行的压缩损害大
    const uint CRYPT_TYPE_HEA_FIR	= 8;	/// 第一段头加密
    const uint CRYPT_TYPE_HEA_SEC = 16; /// 第二段头加密

    /// <summary>
    ///  脚本加密key，暂时写死
    /// </summary>
    static uint ScriptClassfiyKey = 0x3ACBC542;
    /// <summary>
    /// 资源配置加密Key，暂时写死
    /// </summary>
    static uint AssetsConfigClassfiyKey = 0x7BD9E638;

    const int firstHeaderCount = 1000;
    const int SecondHeaderCount = 3000;

    /// <summary>
    /// dll的PE头数据
    /// </summary>
    const int headerByteCount = firstHeaderCount + SecondHeaderCount;



    /// <summary>
    /// 资源包直接使用偏移
    /// </summary>
    static Byte[] AssetFileClassfiyKeyBytes = new Byte[] { 85,110,105,116,121,70,83,6,53,46,120,46,120,53,46,52,46,50,112,52,8,106,42,64,91,67,30,1,81,1,8,105,63,17,64,14,8,1,22,67,65,66,45,54,49,51,
                                                            48,49,48,49,50,50,54,51,49,50,50,49,49,52,49,49,50,49,50,57,56,49,48,54,48,49,53,51,49,52,50,49,48,49,53,49,48,56,49,52,48,63,4,63,105,63,16,
                                                            63,46,52,46,50,112,52,15,16,63,46,52,46,50,112,52,5,1,3,28,47,63,19,63,114,4,231,128,155,229,137,144,53,230,181,190,63,25,1,2,106,3,226,130,172,
                                                            55,226,130,172,239,163,181,239,163,181,239,163,181,239,163,181,226,130,172,1,1,72,3,226,130,172,63,226,130,172,239,163,181,239,163,181,239,163,181,
                                                            239,163,181,1,1,226,130,172,1,2,1,49,226,130,172,49,226,130,172,239,163,181,239,163,181,239,163,181,239,163,181,2,1,64,1,3,232,191,137,27,3,226,130,
                                                            172,4,3,1,1,3,81,226,130,172,106,226,130,172,1,4,1,1,1,232,191,137,4,5,16,1,1,232,191,137,8,4,6,16,1,1,232,191,137,17,4,7,16,1,1,232,191,137,37,4,8,
                                                            1,1,1,232,191,137,53,4,9,16,1,1,76,226,130,172,64,76,226,130,172,77,1,11,232,191,137,63,15,1,2,232,191,137,63,16,1,2,238,148,133,63,17,1,2,232,191,137,
                                                            63,18,1,1,232,191,137,63,19,1,1,232,191,137,63,20,1,1,1,126,3,226,130,172,63,239,163,181,239,163,181,239,163,181,21,1,64,1,2,232,191,137,27,3,226,130,
                                                            172,4,22,1,1,2,63,226,130,172,106,226,130,172,1,23,1,1,1,63,239,163,181,239,163,181,239,163,181,239,163,181,24,
                                                            226,130,172,1,2,63,226,130,172,63,226,130,172,27,3,226,130,172,4,26,1,2,72,3,226,130,172,20,1,49,226,130,172,239,163,181,239,163,181,239,163,181,239,163,
                                                            181,28,1,64,1,4,232,191,137,27,3,226,130,172,4,29,1,1,4,81,226,130,172,106,226,130,172,15,16,63,46,52,
                                                            46,50,112,52,5,1,3,28,47,63,19,63,114,4,231,128,155,229,137,144,53,230,181,190,63,25,1,2,106,3,226,130,172,55,226,130,172,239,163,181,239,163,181,239,163,
                                                            181,239,163,181,226,130,172,1,1,72,3,226,130,172,63,226,130,172,239,163,181,239,163,181,239,163,181,239,163,181,1,1,226,130,172,1,2,1,49,226,130,172,49,226,
                                                            130,172,239,163,181,239,163,181,239,163,181,239,163,181,2,1,64,1,3,232,191,137,27,3,226,130,172,4,3,1,1,3,81,226,130,172,106,226,130,172,1,4,1,1,1,232,191,137,
                                                            4,5,16,1,1,232,191,137,8,4,6,16,1,1,232,191,137,17,4,7,16,1,1,232,191,137,37,4,8,1,1,1,232,191,137,53,4,9,16,1,1,76,226,130,172,64,76,226,130,172,77,1,11,232,
                                                            191,137,63,15,1,2,232,191,137,63,16,1,2,238,148,133,63,17,1,2,232,191,137,63,18,1,1,232,191,137,63,19,1,1,232,191,137,63,20,1,1,1,126,3,226,
                                                            130,172,63,239,163,181,239,163,181,239,163,181,21,1,64,1,2,232,191,137,27,3,226,130,172,4,22,1,1,2,63,226,130,172,106,226,130,172,1,23,1,1,1,63,239,163,181,239,
                                                            163,181,239,163,181,239,163,181,24,226,130,172,1,2,63,226,130,172,63,226,130,172,27,3,226,130,172,4,26,1,2,72,3,226,130,172,20,1,49,226,130,172,239,163,181,239,
                                                            163,181,239,163,181,239,163,181,28,1,64,1,4,232,191,137,27,3,226,130,172,4,29,1,1,4,81,226,130,172,106,226,130,172,1,109,95,70,105,108,101,73,68,109,95,80,97,116,
                                                            104,73,68,109,95,76,111,99,97,108,82,111,116,97,116,105,111,110,120,121,122,119,109,95,76,111,99,97,108,80,111,115,105,116,105,111,110,109,95,76,111,99,97,108,83,
                                                            99,97,108,101,109,95,67,104,105,108,100,114,101,110,109,95,70,97,116,104,101,114,43,116,229,153,131,78,119,230,144,156,230,
                                                            163,175,233,171,169,229,147,133,
                                                            85,110,105,116,121,70,83,6,53,46,120,46,120,53,46,52,46,50,112,52,8,106,42,64,91,67,30,1,81,1,8,105,63,17,64,14,8,1,22,67,65,66,45,49,48,49,53,51,55,49,49,51,50,57,
                                                            54,57,51,49,53,55,54,49,52,49,48,52,50,49,52,49,53,49,48,49,48,49,48,50,49,49,49,50,54,48,53,51,49,49,49,63,4,63,105,63,16,63,46,52,46,50,112,52,15,16,63,46,52,46,
                                                            50,112,52,5,1,3,28,47,63,19,63,114,4,231,128,155,229,137,144,53,230,181,190,63,25,1,2,106,3,226,130,172,55,226,130,172,239,163,181,239,163,181,239,163,
                                                            181,239,163,181,226,130,172,1,1,72,3,226,130,172,63,226,130,172,239,163,181,239,163,181,239,163,181,239,163,181,1,1,226,130,172,1,2,1,49,226,130,172,49,226,130,172,
                                                            239,163,181,239,163,181,239,163,181,239,163,181,2,1,64,1,3,232,191,137,27,3,226,130,172,4,3,1,1,3,81,226,130,172,106,226,130,172,1,4,1,1,1,232,191,137,4,5,16,1,1,232,
                                                            191,137,8,4,6,16,
                                                            1,1,232,191,137,17,4,7,16,1,1,232,191,137,37,4,8,1,1,1,232,191,137,53,4,9,16,1,1,76,226,130,172,64,76,226,130,172,77,1,11,232,191,137,63,15,1,2,232,191,137,63,16,1,
                                                            2,238,148,133,63,17,1,2,232,191,137,63,18,1,1,232,191,137,63,19,1,1,232,191,137,63,20,1,1,1,126,3,226,130,172,63,239,163,181,239,163,181,239,163,181,21,1,64,1,2,232,191,137,27,3,226,130,172,4,22,1,
                                                            1,2,63,226,130,172,106,226,130,172,1,23,1,1,1,63,239,163,181,239,163,181,239,163,181,239,163,181,24,226,130,172,1,2,63,226,130,172,63,226,130,172,27,3,226,130,172,4,26,
                                                            1,2,72,3,226,130,172,20,1,49,226,130,
                                                            172,239,163,181,239,163,181,239,163,181,239,163,181,28,1,64,1,4,232,191,137,27,3,226,130,172,4,29,1,1,4,81,226,130,172,106,226,130,172,15,16,63,46,52,46,50,112,52,
                                                            5,1,3,28,47,63,19,63,114,4,231,128,155,229,137,144,53,
                                                            230,181,190,63,25,1,2,106,3,226,130,172,55,226,130,172,239,163,181,239,163,181,239,163,181,239,163,181,226,130,172,1,1,72,3,226,130,172,63,226,130,172,239,163,181,
                                                            239,163,181,239,163,181,239,163,181,1,1,226,130,172,1,2,1,49,226,130,172,49,226,
                                                            130,172,239,163,181,239,163,181,239,163,181,239,163,181,2,1,64,1,3,232,191,137,27,3,226,130,172,4,3,1,1,3,81,226,130,172,106,226,130,172,1,4,1,1,1,232,191,137,4,
                                                            5,16,1,1,232,191,137,8,4,6,16,1,1,232,191,137,17,4,7,16,1,1,232,191,137,37,4,8,1,1,1,232,191,137,53,4,9,16,1,1,76,226,130,172,64,76,226,130,172,77,1,11,232,191,137,
                                                            63,15,1,2,232,191,137,63,16,1,2,238,148,133,63,17,1,2,232,191,137,63,18,1,1,232,191,137,63,
                                                            19,1,1,232,191,137,63,20,1,1,1,126,3,226,130,172,63,239,163,181,239,163,181,239,163,181,21,1,64,1,2,232,191,137,27,3,226,130,172,4,22,1,1,2,63,226,130,172,106,226,
                                                            130,172,1,23,1,1,1,63,239,163,181,239,163,181,239,163,181,239,163,181,24,226,130,172,1,2,63,
                                                            226,130,172,63,226,130,172,27,3,226,130,172,4,26,1,2,72,3,226,130,172,20,1,49,226,130,172,239,163,181,239,163,181,239,163,181,239,163,181,28,1,64,1,4,232,191,137,27,3,
                                                            226,130,172,4,29,1,1,4,81,226,130,172,106,226,130,172,109,95,70,105,108,101,73,68,109,95,80,97,116,104,73,68,110,73,68,101,86,111,99,97,116,105,111,110,115,116,114,68,101,115,99,98,71,108,83,99,
                                                            97,108,101,98,73,103,110,111,114,101,83,105,110,103,108,101,69,102,102,101,99,116,79,112,116,105,109,116,105,122,101,100,69,110,97,98,108,101,68,105,115,116,97,110,99,101,79,112,116,105,109,116,105,122,
                                                            101,100,67,108,111,115,101,87,104,101,110,68,101,97,
                                                            85,110,105,116,121,70,83,6,53,46,120,46,120,53,46,52,46,50,112,52,8,106,42,64,91,67,30,1,81,1,8,105,63,17,64,14,8,1,22,67,65,66,45,49,51,57,49,52,50,50,54,49,49,54,
                                                            57,56,49,49,51,49,49,51,49,51,49,53,49,49,50,50,52,49,50,49,49,49,52,53,49,57,56,49,50,54,53,51,63,4,63,105,63,16,63,46,52,46,50,112,52,15,16,63,46,52,46,50,112,52,
                                                            5,1,3,28,47,63,19,63,114,4,231,128,155,229,137,144,53,230,181,190,63,25,1,2,106,3,226,130,172,55,226,130,172,239,163,181,239,163,181,239,163,181,239,163,181,226,130,172,1,
                                                            1,72,3,226,130,172,63,226,130,172,239,163,181,239,163,181,239,163,181,239,163,181,1,1,226,130,172,1,2,1,49,226,130,172,49,226,130,172,239,163,181,239,163,181,239,163
                                                            ,181,239,163,181,2,1,64,1,3,232,191,137,27,3,226,130,172,4,3,1,1,3,81,226,130,172,106,226,130,172,1,4,1,1,1,232,191,137,4,5,16,1,1,232,191,137,8,4,6,16,1,1,232,191,
                                                            137,17,4,7,16,1,1,232,191,137,37,4,8,1,1,1,232,191,137,53,4,9,16,1,1,76,226,130,172,64,76,226,130,172,77,1,11,232,191,137,63,15,1,2,232,191,137,63,16,1,2,238,148,
                                                            133,63,17,1,2,232,191,137,63,18,1,1,232,191,137,63,19,1,1,232,191,137,63,20,1,1,1,126,3,226,130,172,63,239,163,181,239,163,181,239,163,181,21,1,64,1,2,232,191,137,
                                                            27,3,226,130,172,4,22,
                                                            1,1,2,63,226,130,172,106,226,130,172,1,23,1,1,1,63,239,163,181,239,163,181,239,163,181,239,163,181,24,226,130,172,1,2,63,226,130,172,63,226,130,172,27,3,226,130,172,
                                                            4,26,1,2,72,3,226,130,172,20,1,49,226,130,172,239,163,181,239,163,181,239,163,181,239,163,
                                                            181,28,1,64,1,4,232,191,137,27,3,226,130,172,4,29,1,1,4,81,226,130,172,106,226,130,172,15,16,63,46,52,46,50,112,52,5,1,3,28,47,63,19,63,114,4,231,128,155,229,137,
                                                            144,53,230,181,190,63,25,1,2,106,3,226,130,172,55,226,130,172,239,163,181,239,163,181,239,163,181,239,163,181,226,130,172,1,1,72,3,226,130,172,63,226,130,172,239,
                                                            163,181,239,163,181,239,163,181,239,163,181,1,1,226,130,172,1,2,1,49,226,130,172,49,226,130,172,239,163,181,239,163,181,239,163,181,239,163,181,2,1,64,1,3,232,191,
                                                            137,27,3,226,130,172,4,3,1,1,3,81,226,130,172,106,226,130,172,1,4,1,1,1,232,191,137,4,5,16,1,1,232,191,137,8,4,6,16,1,1,232,191,137,17,4,7,16,1,1,232,191,137,37,4,
                                                            8,1,1,1,232,191,137,53,4,9,16,1,1,76,226,130,172,64,76,226,130,172,77,1,11,232,191,137,63,15,1,2,232,191,137,63,16,1,
                                                            2,238,148,133,63,17,1,2,232,191,137,63,18,1,1,232,191,137,63,19,1,1,232,191,137,63,20,1,1,1,126,3,226,130,172,63,239,163,181,239,163,181,239,163,181,21,1,64,1,2,
                                                            232,191,137,27,3,226,130,172,4,22,1,1,2,63,226,130,172,106,226,130,172,1,23,1,1,1,63,239,163,181,239,163,181,239,163,181,239,163,181,24,226,130,172,1,2,63,226,130,
                                                            172,63,226,130,172,27,3,226,130,172,4,26,1,2,72,3,226,130,172,20,1,49,226,130,172,239,163,181,239,163,181,239,163,181,239,163,181,28,1,64,1,4,232,191,137,27,3,226,
                                                            130,172,4,29,1,1,4,81,226,130,172,106,226,130,172,65,117,100,105,111,67,108,105,112,109,95,76,111,97,100,84,121,112,101,109,95,67,104,97,110,110,101,83,97,109,112,
                                                            108,101,109,95,76,101,110,103,116,104,109,95,73,115,84,114,97,99,107,101,114,70,111,114,109,97,116,109,95,83,117,98,115,111,117,110,100,73,110,100,101,120,109,95,
                                                            80,114,101,108,111,97,100,65,117,100,105,111,68,97,116,97,109,95,76,111,97,100,73,110,66,97,99,107,103,114,111,117,110,100,109,95,};

    /// <summary>
    /// 绝对路径
    /// </summary>
    /// <param name="bundlieFile"></param>
    /// <returns></returns>
    public static string GetAssetBundleFildMD5(string bundlieFile)
    {
        string reslut = string.Empty;

        ////弃用，有时候hash变了，但是文件没有变，因为dll每次编译，都会产生不同的结果
        //if (!File.Exists(bundlieFile))
        //{
        //    return reslut;
        //}

        //string realPath = bundlieFile + ".manifest";
        //if (!File.Exists(realPath))
        //{
        //    return reslut;
        //}


        //System.IO.StreamReader MD5FileReader = new System.IO.StreamReader(realPath, System.Text.UTF8Encoding.UTF8);
        //string MD5FileText = MD5FileReader.ReadLine();
        //while (!string.IsNullOrEmpty(MD5FileText))
        //{
        //    MD5FileText = MD5FileText.ToLower();
        //    //只要找到第一个就退出，因为有两个hash，第一个是真正的资源hash,不能包括hashes，因为有一个总的标签
        //    if (MD5FileText.Contains("hash") && !MD5FileText.Contains("hashes") && !MD5FileText.Contains("assetfilehash"))
        //    {
        //        string[] metaArrays = MD5FileText.Split(':');
        //        //第二个是md5
        //        reslut = metaArrays[1];

        //        //去掉空格
        //        reslut = reslut.Trim();
        //        break;
        //    }
        //    MD5FileText = MD5FileReader.ReadLine();
        //}
        //MD5FileReader.Close();
        //MD5FileReader = null;
        return reslut;
    }

    public static AssetFileInfo GetFileInfo(string fileName)
    {
        //是文件夹
        if (!File.Exists(fileName) || System.IO.Directory.Exists(fileName))
        {
            return null;
        }


        FileInfo info = new FileInfo(fileName);
        int filesizeInBytes = (int)info.Length;
        float filesizeInMB = (float)filesizeInBytes / 1024.0f / 1024.0f;

        string fileMD5 = GetAssetBundleFildMD5(fileName);
        if (string.IsNullOrEmpty(fileMD5))
        {
            System.Security.Cryptography.MD5 md5 = new System.Security.Cryptography.MD5CryptoServiceProvider();
            FileStream stream = info.OpenRead();
            byte[] retVal = md5.ComputeHash(stream);

            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < retVal.Length; i++)
            {
                sb.Append(retVal[i].ToString("x2"));
            }
            md5.Clear();
            md5 = null;
            stream.Flush();
            stream.Close();
            info = null;
            fileMD5 = sb.ToString();
        }


        AssetFileInfo reslut = new AssetFileInfo();
        reslut.fileLengthInBytes = filesizeInBytes;
        reslut.fileLengthInMB = filesizeInMB;
        reslut.md5 = fileMD5;
        return reslut;

    }

    /// <summary>
    /// 资源配置文件
    /// </summary>
    /// <param name="source"></param>
    /// <returns></returns>
    private static Byte[] EnCodeAssetsConfigFile(Byte[] source)
    {
        if (null == source)
        {
            return source;
        }


        for (int i = 0; i < source.Length; i++)
        {
            source[i] ^= 3;
        }

        IntPtr memory = Marshal.AllocHGlobal(source.Length);
        if (memory == IntPtr.Zero)
        {
            Debug.LogError("分配加密内存失败!-IntPtr.Zero");
            return source;
        }

        Marshal.Copy(source, 0, memory, source.Length);

        if (EncryptData(memory, (uint)source.Length, AssetsConfigClassfiyKey, CRYPT_TYPE_MAP) == 0)
        {
            Debug.LogError("分配加密内存失败!-EncryptData");
            return source;
        }
        Marshal.Copy(memory, source, 0, source.Length);

        Marshal.FreeHGlobal(memory);
        return source;
    }

    private static Byte[] DeCodeAssetsConfigFile(Byte[] source)
    {
        if (null == source)
        {
            return source;
        }
        IntPtr memory = Marshal.AllocHGlobal(source.Length);
        if (memory == IntPtr.Zero)
        {
            Debug.LogError("分配加密内存失败!-IntPtr.Zero");
            return source;
        }

        Marshal.Copy(source, 0, memory, source.Length);
        if (DecryptData(memory, (uint)source.Length, AssetsConfigClassfiyKey, CRYPT_TYPE_MAP) == 0)
        {
            Debug.LogError("分配加密内存失败!-DecryptData");
            return source;
        }

        Marshal.Copy(memory, source, 0, source.Length);

        Marshal.FreeHGlobal(memory);

        for (int i = 0; i < source.Length; i++)
        {
            source[i] ^= 3;
        }
        return source;
    }

    /// <summary>
    /// 加密脚本
    /// </summary>
    /// <param name="source"></param>
    /// <returns></returns>
    private static Byte[] EnCodeScript(Byte[] source)
    {
        if (null == source)
        {
            return source;
        }

        source = EnCodeScript_XOR(source);
        source = EnCodeScript_MAP(source);
        source = EnCodeScript_HeaderSecond(source);
        source = EnCodeScript_HeaderFirst(source);

        return source;
    }


    private static Byte[] EnCodeScript_HeaderFirst(Byte[] source)
    {
        if (null == source)
        {
            return source;
        }
        //长度相等，不做加密
        if (source.Length <= headerByteCount)
        {
            Debug.LogWarning("source.Length <= headerByteCount,不做加密");
            return source;
        }
        int length = firstHeaderCount ;
        int startIndex = 0;
        IntPtr memory = Marshal.AllocHGlobal(length);
        if (memory == IntPtr.Zero)
        {
            Debug.LogError("分配加密内存失败!-IntPtr.Zero");
            return source;
        }

        Marshal.Copy(source, startIndex, memory, length);
        if (EncryptData(memory, (uint)length, ScriptClassfiyKey, CRYPT_TYPE_HEA_FIR) == 0)
        {
            Debug.LogError("分配加密内存失败!-EncryptData");
            return source;
        }

        Marshal.Copy(memory, source, startIndex, length);

        Marshal.FreeHGlobal(memory);
        return source;
    }

    private static Byte[] EnCodeScript_HeaderSecond(Byte[] source)
    {
        if (null == source)
        {
            return source;
        }
        //长度相等，不做加密
        if (source.Length <= headerByteCount)
        {
            Debug.LogWarning("source.Length <= headerByteCount,不做加密");
            return source;
        }
        int length = SecondHeaderCount;
        int startIndex = firstHeaderCount - 1;
        IntPtr memory = Marshal.AllocHGlobal(length);
        if (memory == IntPtr.Zero)
        {
            Debug.LogError("分配加密内存失败!-IntPtr.Zero");
            return source;
        }

        Marshal.Copy(source, startIndex, memory, length);
        if (EncryptData(memory, (uint)length, ScriptClassfiyKey, CRYPT_TYPE_HEA_SEC) == 0)
        {
            Debug.LogError("分配加密内存失败!-EncryptData");
            return source;
        }

        Marshal.Copy(memory, source, startIndex, length);

        Marshal.FreeHGlobal(memory);
        return source;
    }

    private static Byte[] EnCodeScript_MAP(Byte[] source)
    {
        if (null == source)
        {
            return source;
        }
        //长度相等，不做加密
        if (source.Length <= headerByteCount)
        {
            Debug.LogWarning("source.Length <= headerByteCount,不做加密");
            return source;
        }
        int length = source.Length - headerByteCount;
        int startIndex = headerByteCount - 1;
        IntPtr memory = Marshal.AllocHGlobal(length);
        if (memory == IntPtr.Zero)
        {
            Debug.LogError("分配加密内存失败!-IntPtr.Zero");
            return source;
        }

        Marshal.Copy(source, startIndex, memory, length);
        if (EncryptData(memory, (uint)length, ScriptClassfiyKey, CRYPT_TYPE_MAP) == 0)
        {
            Debug.LogError("分配加密内存失败!-EncryptData");
            return source;
        }

        Marshal.Copy(memory, source, startIndex, length);

        Marshal.FreeHGlobal(memory);
        return source;
    }

    private static Byte[] EnCodeScript_XOR(Byte[] source)
    {
        if (null == source)
        {
            return source;
        }
        //长度相等，不做加密
        if (source.Length <= headerByteCount)
        {
            Debug.LogWarning("source.Length <= headerByteCount,不做加密");
            return source;
        }
        int length = source.Length - headerByteCount;
        int startIndex = headerByteCount - 1;
        IntPtr memory = Marshal.AllocHGlobal(length);
        if (memory == IntPtr.Zero)
        {
            Debug.LogError("分配加密内存失败!-IntPtr.Zero");
            return source;
        }
        Marshal.Copy(source, startIndex, memory, length);
        if (EncryptData(memory, (uint)length, ScriptClassfiyKey, CRYPT_TYPE_XOR) == 0)
        {
            Debug.LogError("分配加密内存失败!-EncryptData");
            return source;
        }

        Marshal.Copy(memory, source, startIndex, length);

        Marshal.FreeHGlobal(memory);
        return source;
    }

    private static Byte[] DeCodeScript(Byte[] source)
    {
        if (null == source)
        {
            return source;
        }
        //长度相等，不做加密
        if (source.Length <= headerByteCount)
        {
            Debug.LogWarning("source.Length <= headerByteCount,不做加密");
            return source;
        }
        source = DeCodeScript_HeaderFirst(source);
        source = DeCodeScript_HeaderSecond(source);
        source = DeCodeScript_MAP(source);
        source = DeCodeScript_XOR(source);
        return source;
    }

    private static Byte[] DeCodeScript_HeaderFirst(Byte[] source)
    {
        if (null == source)
        {
            return source;
        }
        //长度相等，不做加密
        if (source.Length <= headerByteCount)
        {
            Debug.LogWarning("source.Length <= headerByteCount,不做加密");
            return source;
        }
        int length = source.Length;
        int startIndex = 0;
        IntPtr memory = Marshal.AllocHGlobal(length);
        if (memory == IntPtr.Zero)
        {
            Debug.LogError("分配加密内存失败!-IntPtr.Zero");
            return source;
        }

        Marshal.Copy(source, startIndex, memory, source.Length);

        if (DecryptData(memory, (uint)length, ScriptClassfiyKey, CRYPT_TYPE_HEA_FIR) == 0)
        {
            Debug.LogError("分配加密内存失败!-DecryptData");
            return source;
        }

        Marshal.Copy(memory, source, startIndex, length);

        Marshal.FreeHGlobal(memory);
        return source;
    }
    private static Byte[] DeCodeScript_HeaderSecond(Byte[] source)
    {
        if (null == source)
        {
            return source;
        }
        //长度相等，不做加密
        if (source.Length <= headerByteCount)
        {
            Debug.LogWarning("source.Length <= headerByteCount,不做加密");
            return source;
        }
        int length = SecondHeaderCount;
        int startIndex = firstHeaderCount - 1;
        IntPtr memory = Marshal.AllocHGlobal(length);
        if (memory == IntPtr.Zero)
        {
            Debug.LogError("分配加密内存失败!-IntPtr.Zero");
            return source;
        }

        Marshal.Copy(source, startIndex, memory, source.Length);

        if (DecryptData(memory, (uint)length, ScriptClassfiyKey, CRYPT_TYPE_HEA_SEC) == 0)
        {
            Debug.LogError("分配加密内存失败!-DecryptData");
            return source;
        }

        Marshal.Copy(memory, source, startIndex, length);

        Marshal.FreeHGlobal(memory);
        return source;
    }

    private static Byte[] DeCodeScript_MAP(Byte[] source)
    {
        if (null == source)
        {
            return source;
        }
        //长度相等，不做加密
        if (source.Length <= headerByteCount)
        {
            Debug.LogWarning("source.Length <= headerByteCount,不做加密");
            return source;
        }
        int length = source.Length - headerByteCount;
        int startIndex = headerByteCount - 1;
        IntPtr memory = Marshal.AllocHGlobal(length);
        if (memory == IntPtr.Zero)
        {
            Debug.LogError("分配加密内存失败!-IntPtr.Zero");
            return source;
        }

        Marshal.Copy(source, startIndex, memory, length);

        if (DecryptData(memory, (uint)length, ScriptClassfiyKey, CRYPT_TYPE_MAP) == 0)
        {
            Debug.LogError("分配加密内存失败!-DecryptData");
            return source;
        }

        Marshal.Copy(memory, source, startIndex, length);

        Marshal.FreeHGlobal(memory);
        return source;
    }

    private static Byte[] DeCodeScript_XOR(Byte[] source)
    {
        if (null == source)
        {
            return source;
        }
        //长度相等，不做加密
        if(source.Length <= headerByteCount)
        {
            Debug.LogWarning("source.Length <= headerByteCount,不做加密");
            return source;
        }
        int length = source.Length - headerByteCount;
        int startIndex = headerByteCount - 1;
        IntPtr memory = Marshal.AllocHGlobal(length);
        if (memory == IntPtr.Zero)
        {
            Debug.LogError("分配加密内存失败!-IntPtr.Zero");
            return source;
        }

        Marshal.Copy(source, startIndex, memory, length);

        if (DecryptData(memory, (uint)length, ScriptClassfiyKey, CRYPT_TYPE_XOR) == 0)
        {
            Debug.LogError("分配加密内存失败!-DecryptData");
            return source;
        }

        Marshal.Copy(memory, source, startIndex, length);

        Marshal.FreeHGlobal(memory);
        return source;

    }

    /// <summary>
    /// 加密资源文件
    /// </summary>
    /// <param name="source"></param>
    /// <returns></returns>
    private static Byte[] EnCodeAssetFile(Byte[] source)
    {
        if (null == source)
        {
            return source;
        }
        List<Byte> arraList = new List<byte>();
        arraList.AddRange(AssetFileClassfiyKeyBytes);
        arraList.AddRange(source);
        return arraList.ToArray();
    }

    private static Byte[] DeCodeAssetFile(Byte[] source)
    {
        if (null == source)
        {
            return source;
        }
        List<Byte> arraList = new List<byte>();
        arraList.AddRange(source);
        arraList.RemoveRange(0, AssetFileClassfiyKeyBytes.Length);
        return arraList.ToArray();
    }

    /// <summary>
    /// 加密资源文件帮助,绝对路径
    ///  将会对source文件加密后放到dst目录
    /// </summary>
    /// <param name="SourcePath"></param>
    /// <param name="DestPath"></param>
    private static void EnCodeAssetFileHelper(string SourcePath, string DestPath)
    {
        if (!File.Exists(SourcePath))
        {
            return;
        }

        string folderPath = System.IO.Path.GetDirectoryName(DestPath);
        if (!System.IO.Directory.Exists(folderPath))
        {
            System.IO.Directory.CreateDirectory(folderPath);
        }

        Byte[] data = File.ReadAllBytes(SourcePath);
        File.WriteAllBytes(DestPath, EnCodeAssetFile(data));
    }

    /// <summary>
    /// 加密资源配置文件等,绝对路径
    /// 将会对source文件加密后放到dst目录
    /// </summary>
    /// <param name="SourcePath"></param>
    /// <param name="DestPath"></param>
    private static void EnCodeAssetConfigFileHelper(string SourcePath, string DestPath)
    {
        if (!File.Exists(SourcePath))
        {
            return;
        }

        string folderPath = System.IO.Path.GetDirectoryName(DestPath);
        if (!System.IO.Directory.Exists(folderPath))
        {
            System.IO.Directory.CreateDirectory(folderPath);
        }

        Byte[] data = File.ReadAllBytes(SourcePath);
        if(data.Length <= 0)
        {
            Debug.LogError("加密失败，文件大小为0:" + SourcePath);
            return;
        }
        File.WriteAllBytes(DestPath, EnCodeAssetsConfigFile(data));
    }
    /// <summary>
    /// 加密代码,绝对路径
    /// 将会对source文件加密后放到dst目录
    /// </summary>
    /// <param name="SourcePath"></param>
    /// <param name="DestPath"></param>
    private static void EnCodeScriptHelper(string SourcePath, string DestPath)
    {
        if (!File.Exists(SourcePath))
        {
            return;
        }

        string folderPath = System.IO.Path.GetDirectoryName(DestPath);
        if (!System.IO.Directory.Exists(folderPath))
        {
            System.IO.Directory.CreateDirectory(folderPath);
        }

        Byte[] data = File.ReadAllBytes(SourcePath);
        //长度相等，不做加密
        if (data.Length <= headerByteCount)
        {
            Debug.Log("source.Length <= headerByteCount,不做加密:" + SourcePath);
        }
        File.WriteAllBytes(DestPath, EnCodeScript(data));
    }


    private static int EncryptData(IntPtr address, uint len, uint key, uint mask)
    {
        if (Is64bitEditor)
        {
            return EncryptData64(address, len, key, mask);
        }
        return EncryptData32(address, len, key, mask);
    }

    private static int DecryptData(IntPtr address, uint len, uint key, uint mask)
    {
        if (Is64bitEditor)
        {
            return DecryptData64(address, len, key, mask);
        }
        return DecryptData32(address, len, key, mask);
    }


    private static void GenerateAssetPackageKey()
    {
        string strKeyOne = "UnityFS5.x.x5.4.2p4j*@[CQi?@";
        //string strKeyTwo = "CAB-b9286b07835d25d41a79a698b0e33d26";
        string strKeyThree = "??i??.4.2p4";
        string strKeyFour = "?.4.2p4/??r瀛剐5浾?j€7€€H€?€€1€1€@迉€Q€j€迉迉迉迉%迉5	L€@L€M迉?迉??迉?迉?迉?~€?@迉€?€j€?€?€?€€H€1€@迉€Q€j€";

        string[] FinalPartList = new string[] {"m_Widthm_Heightm_CompleteImageSizem_TextureFormatm_MipCountm_IsReadablem_Re",
                                               "?m_Legacym_Compressedm_UseHighQualityCuQuaterniKeyframetimevaluexyzwinSlopeoutSlopem_PreInfinity",
                                               "@AnimatorControllerm_rConArrayOffsetPtrLayerConstantm_StateMachineIndeonSetIndexHumanPoseMaskm_BodyMaskword" ,
                                               "aterialPPtr<Shader>m_ShadehaderKeywordsm_LightmapFlagsm_CustomRenderQueuestringTagMapUnityProperty",
                                               "m_FileIDm_PathIDm_LocalRotationxyzwm_LocalPositionm_LocalScalem_Childrenm_Father+t噃Nw搜棯髩哅",
                                               "E MaterialPPtr<Shader>m_Shaderm_FileIDm_PathIDm_ShaderKeQueuestringTagMapUnityPropertySheesm_TexEnvsnameUnityTexEnv",
                                               "m_FileIDm_PathIDnIDeVocationstrDescbGlScalebIgnoreSingleEffectOptimtizedEnableDistanceOptimtizedCloseWhenDea",
                                               "AudioClipm_LoadTypem_ChanneSamplem_Lengthm_IsTrackerFormatm_SubsoundIndexm_PreloadAudioDatam_LoadInBackgroundm_",
                                                "m_FileIDm_PathIDIncludeChildemPPtr<$ParticleSysderer>_TrailRen>m_MeshRendm_enderer>m_LightsPPtr<$Light>bBak",
                        };
        List<byte> byaa = new List<byte>();
        
        string str = string.Empty;
        for (int i = 0; i < 3; i++)
        {
            foreach (Byte b in AssetBundleManager.StringToByte(strKeyOne))
            {
                str += b;
                str += ",";
                if(str.Length %256==0)
                {
                    str += "\n";
                }
            }

            string guidRandom = string.Empty;
            for (int j = 0; j < 32;j++ )
            {
                guidRandom += (Byte)UnityEngine.Random.Range(0, 16);
            }

            guidRandom = "CAB-" + guidRandom;

            foreach (Byte b in AssetBundleManager.StringToByte(guidRandom))
            {
                str += b;
                str += ",";
                if (str.Length % 256 == 0)
                {
                    str += "\n";
                }
            }

            foreach (Byte b in AssetBundleManager.StringToByte(strKeyThree))
            {
                str += b;
                str += ",";
                if (str.Length % 256 == 0)
                {
                    str += "\n";
                }
            }
            foreach (Byte b in AssetBundleManager.StringToByte(strKeyFour))
            {
                str += b;
                str += ",";
                if (str.Length % 256 == 0)
                {
                    str += "\n";
                }
            }

            foreach (Byte b in AssetBundleManager.StringToByte(strKeyFour))
            {
                str += b;
                str += ",";
                if (str.Length % 256 == 0)
                {
                    str += "\n";
                }
            }

            string finalPart = FinalPartList[UnityEngine.Random.Range(0, FinalPartList.Length - 1)];
            foreach (Byte b in AssetBundleManager.StringToByte(finalPart))
            {
                str += b;
                str += ",";
                if (str.Length % 256 == 0)
                {
                    str += "\n";
                }
            }
            str += "\n";
        }
        Debug.LogError(str);
    }

    [DllImport("mono_editor.dll", EntryPoint = "encrypt")]
    private extern static int EncryptData32(IntPtr address, uint len, uint key, uint mask);

    [DllImport("mono_editor64.dll", EntryPoint = "encrypt")]
    private extern static int EncryptData64(IntPtr address, uint len, uint key, uint mask);

    [DllImport("mono_editor.dll", EntryPoint = "decrypt")]
    private extern static int DecryptData32(IntPtr address, uint len, uint key, uint mask);

    [DllImport("mono_editor64.dll", EntryPoint = "decrypt")]
    private extern static int DecryptData64(IntPtr address, uint len, uint key, uint mask);
}