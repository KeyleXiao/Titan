/*******************************************************************
** 文件名:	AssetBundleImporter.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	ab资源管理
** 应  用:  	ab资源导入

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using UnityEditor;
using System.Collections.Generic;
using SoundSystem;
using System.IO;

/// <summary>
/// 资源导入导入
/// </summary>
public partial class AssetBundleImporter : AssetPostprocessor
{
    //static string AudioInfoBegin = "[AUD-";
    //static string AudioInfoEnd = "-AUD]";

    //private void OnPostprocessAudio(AudioClip clip)
    //{
    //    string AudioInfo;
    //    float length;
    //    string AudioInfoOld;
    //    AudioInfo = ConvertAudioInfo(clip.length);

    //    if (GetAudioInfo(assetImporter, out length))
    //    {
    //        AudioInfoOld = ConvertAudioInfo(length);
    //        if (!AudioInfoOld.Equals(AudioInfo))
    //        {
    //            assetImporter.userData = assetImporter.userData.Replace(AudioInfoOld, string.Empty);
    //        }
    //    }

    //    if (!assetImporter.userData.Contains(AudioInfo))
    //    {
    //        assetImporter.userData += AudioInfo;
    //    }
    //}

    //private static string ConvertAudioInfo(float length)
    //{
    //    return AudioInfoBegin + length.ToString("F3") + AudioInfoEnd;
    //}

    //private static bool GetAudioInfo(AssetImporter assetImporter, out float length)
    //{
    //    length = 0;
    //    string filePatch = AssetBundleManagerEditor.ConverRelativePatchToObslotePatch(assetImporter.assetPath);
    //    filePatch += ".meta";

    //    if (!File.Exists(filePatch))
    //    {
    //        return false;
    //    }

    //    string userData = assetImporter.userData;
    //    //if (!userData.Contains(TextureInfoBegin))
    //    //{
    //    //string text = System.IO.File.ReadAllText(filePatch);
    //    //List<string> alltext = new List<string>();
    //    //alltext.AddRange(text.Split('\n'));
    //    //string strAbVar = "  userData: ";
    //    //for (int i = alltext.Count - 1; i >= 0; i--)
    //    //{
    //    //    string s = alltext[i];
    //    //    if (s.Contains(strAbVar))
    //    //    {
    //    //        userData = s.Replace(strAbVar, string.Empty);
    //    //    }
    //    //}
    //    //}

    //    if (!userData.Contains(AudioInfoBegin))
    //    {
    //        return false;
    //    }
    //    int begin = userData.IndexOf(AudioInfoBegin);
    //    int end = userData.IndexOf(AudioInfoEnd);

    //    string reslut = userData.Substring(begin, end - begin);
    //    reslut = reslut.Replace(AudioInfoBegin, string.Empty);
    //    reslut = reslut.Replace(AudioInfoEnd, string.Empty);
    //    return float.TryParse(reslut, out length); ;
    //}

    public void OnPostprocessAudio(AudioClip clip)
    {

        AudioImporter imp = assetImporter as AudioImporter;
        string asPatch = imp.assetPath;
        AudioImporterSampleSettings ass = imp.defaultSampleSettings;
        ass.quality = 0.8f;

        float length = clip.length;
        if(length < 2.0f)
        {
            ass.compressionFormat = AudioCompressionFormat.PCM;
            ass.loadType = AudioClipLoadType.DecompressOnLoad;
        }
        else if( length < 5.0f)
        {
            ass.compressionFormat = AudioCompressionFormat.ADPCM;
            ass.loadType = AudioClipLoadType.DecompressOnLoad;
        }
        else
        {
            ass.compressionFormat = AudioCompressionFormat.Vorbis;
            ass.loadType = AudioClipLoadType.Streaming;
        }
        imp.defaultSampleSettings = ass;
    }


    public static AssetBundleManagerEditor.AssetBundleEditorInfo MakeSoundAssetBundleInfoFromReference(string patch)
    {
        AssetBundleManagerEditor.AssetBundleEditorInfo info = AssetBundleManagerEditor.GetAssetBundleEditorInfo(patch);

        if (null != info)
        {
            return info;
        }

        info = MakeSoundAssetBundleData(patch);
        return info;
    }

    private static AssetBundleManagerEditor.AssetBundleEditorInfo MakeSoundAssetBundleData(string path)
    {
        AssetBundleManagerEditor.AssetBundleEditorInfo info = null;
        AssetImporter importer = AssetImporter.GetAtPath(path);
        if (!importer)
        {
            return info;
        }
        AudioImporter assetImporter = importer as AudioImporter;
        if (!assetImporter)
        {
            return info;
        }

        string asPatch = assetImporter.assetPath;

        //不是在这个路径下的，不处理
        if (!isAudioAssets(asPatch))
        {
            return info;
        }

        //移除头部
        asPatch = asPatch.Replace(soundAssetPatch, "");

        string[] strs = asPatch.Split('/');

        //没有放到其他文件夹下，则使用默认的音效包名
        if (strs.Length <= 1)
        {
            asPatch = "DefaultSound";
        }
        else
        {
            //移除名字
            asPatch = asPatch.Replace(strs[strs.Length - 1], "");

            //再移除最后的斜杠
            asPatch = asPatch.Remove(asPatch.Length - 1);
        }

        string assetName = Path.GetFileNameWithoutExtension(assetImporter.assetPath);

        string guid = AssetBundleManagerEditor.GetAssetGUIDFromMeta(assetImporter.assetPath);

        if (guid.Equals(string.Empty))
        {
            return info;
        }

        string abName = "Sound/" + asPatch;
        string variant =  AssetBundleManager.soundVariant;
        SaveAssetBundleData(assetImporter, abName, variant);

        info = new AssetBundleManagerEditor.AssetBundleEditorInfo();
        info.AssetBundleName = abName;
        info.AssetBundleVarian = variant;
        info.guid = guid;
        info.assetName = assetName;
        return info;
    }

    private static bool isAudioAssets(string s)
    {
        AudioImporter ap = AssetImporter.GetAtPath(s) as AudioImporter;
        if(!ap)
        {
            return false;
        }
        string asPatch = s;
        //不是在这个路径下的，不处理
        if (!asPatch.Contains(soundAssetPatch))
        {
            return false;
        }

        return true;
    }

}
