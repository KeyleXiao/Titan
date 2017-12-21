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
    //void MakeAnimationClip(ModelImporter modelImporter)
    //{
    //    string assetpath = assetImporter.assetPath;
    //    string ext = Path.GetExtension(assetpath).ToLower();
    //    string fullpath = Path.GetFullPath(assetpath);
    //    string dir = Path.GetDirectoryName(fullpath);
    //    string filename = Path.GetFileNameWithoutExtension(fullpath);
    //    string cfgpath = dir +"/"+ filename + ".csv";
    //    if (ext==".fbx" && System.IO.File.Exists(cfgpath))
    //    {
    //        string cfgs=System.IO.File.ReadAllText(cfgpath);
    //        if (cfgs == null || cfgs.Length <= 0) 
    //        {
    //            return;
    //        }

    //        modelImporter.animationCompression = ModelImporterAnimationCompression.Off;
    //        List<ModelImporterClipAnimation> clipanims = new List<ModelImporterClipAnimation>();

    //        ScpReaderEditor reader = new ScpReaderEditor(cfgs, filename + "cfg");
    //        int n=reader.GetRecordCount();
    //        for (int i = 0; i < n; i++)
    //        {
    //            ModelImporterClipAnimation clipanim = new ModelImporterClipAnimation();
    //            string command = reader.GetString(i, 0, "");
    //            if (command == "animcompression")
    //            {
    //                if (reader.GetInt(i, 1, 0) != 0)
    //                {
    //                    modelImporter.animationCompression = ModelImporterAnimationCompression.Optimal;
    //                }
    //                else
    //                {
    //                    modelImporter.animationCompression = ModelImporterAnimationCompression.Off;
    //                }
    //                //modelImporter.animationCompression = (ModelImporterAnimationCompression)reader.GetInt(i, 1, 0);
    //                modelImporter.animationRotationError = reader.GetFloat(i, 2, 0.1f);
    //                modelImporter.animationPositionError = reader.GetFloat(i, 3, 0.1f);
    //                modelImporter.animationScaleError = reader.GetFloat(i, 4, 0.1f);
    //            }
    //            else if (command == "clip")
    //            {
    //                clipanim.name = reader.GetString(i, 1, "");
    //                clipanim.name = clipanim.name.Trim();
    //                if (clipanim.name == null || clipanim.name.Length <= 0)
    //                {
    //                    continue;
    //                }
    //                clipanim.firstFrame = reader.GetInt(i, 2, -1);
    //                clipanim.lastFrame = reader.GetInt(i, 3, -1);
    //                if (clipanim.firstFrame >= clipanim.lastFrame)
    //                {
    //                    continue;
    //                }
    //                clipanim.loopTime = reader.GetInt(i, 4, 0)!=0;
    //                clipanim.loopPose = reader.GetInt(i, 5, 0) != 0;
    //                clipanim.cycleOffset = reader.GetFloat(i, 6, 0.0f);

    //                clipanim.lockRootRotation = reader.GetInt(i, 7, 0) != 0;
    //                clipanim.keepOriginalOrientation = reader.GetInt(i, 8, 0) == 0;
    //                clipanim.rotationOffset = reader.GetFloat(i, 9, 0);

    //                clipanim.lockRootHeightY = reader.GetInt(i, 10, 0) != 0;
    //                clipanim.keepOriginalPositionY = reader.GetInt(i, 11, 0) == 0;
    //                clipanim.heightOffset = reader.GetFloat(i, 12, 0);

    //                clipanim.lockRootPositionXZ = reader.GetInt(i, 13, 0) != 0;
    //                clipanim.keepOriginalPositionXZ = reader.GetInt(i, 14, 0) == 0;

    //                int wrapvalue=reader.GetInt(i, 15, 0);
    //                if (wrapvalue < 0 || wrapvalue>3)
    //                {
    //                    wrapvalue = 0;
    //                }
    //                wrapvalue = 2 ^ wrapvalue;
    //                clipanim.wrapMode = (WrapMode)wrapvalue;

    //                int BindLightingEffectID1 = reader.GetInt(i, 16, 0);
    //                float BindLightingEffectTime1 = reader.GetFloat(i, 17, 0.0f);
    //                int BindLightingEffectID2 = reader.GetInt(i, 18, 0);
    //                float BindLightingEffectTime2 = reader.GetFloat(i, 19, 0.0f);
    //                float startScaleTime = reader.GetFloat(i, 20, 0.0f);
    //                float startScaleLength = reader.GetFloat(i, 21, 0.0f);
    //                float endScaleTime = reader.GetFloat(i, 22, 0.0f);
    //                float endScaleLength = reader.GetFloat(i, 23, 0.0f);

    //                int eventCount = 0;
    //                if (BindLightingEffectID1 > 0)
    //                {
    //                    eventCount++;
    //                }

    //                if (BindLightingEffectID2 > 0)
    //                {
    //                    eventCount++;
    //                }

    //                if (startScaleTime > 0.0f)
    //                {
    //                    eventCount++;
    //                }

    //                if (endScaleTime > 0.0f)
    //                {
    //                    eventCount++;
    //                }

    //                if (eventCount > 0)
    //                {
    //                    AnimationEvent[] events = new AnimationEvent[eventCount];
    //                    eventCount = 0;
    //                    if (BindLightingEffectID1 > 0)
    //                    {
    //                        AnimationEvent newEvent1 = new AnimationEvent();
    //                        newEvent1.functionName = "BindLighting";
    //                        newEvent1.intParameter = BindLightingEffectID1;
    //                        newEvent1.time = BindLightingEffectTime1;
    //                        events[eventCount] = newEvent1;
    //                        eventCount++;
    //                    }
    //                    if (BindLightingEffectID2 > 0)
    //                    {
    //                        AnimationEvent newEvent2 = new AnimationEvent();
    //                        newEvent2.functionName = "BindLighting";
    //                        newEvent2.intParameter = BindLightingEffectID2;
    //                        newEvent2.time = BindLightingEffectTime2;
    //                        events[eventCount] = newEvent2;
    //                        eventCount++;
    //                    }

    //                    if (startScaleTime > 0.0f)
    //                    {
    //                        AnimationEvent newEvent3 = new AnimationEvent();
    //                        newEvent3.functionName = "beginScale";
    //                        newEvent3.floatParameter = startScaleLength;
    //                        newEvent3.time = startScaleTime;
    //                        events[eventCount] = newEvent3;
    //                        eventCount++;
    //                    }

    //                    if (endScaleTime > 0.0f)
    //                    {
    //                        AnimationEvent newEvent4 = new AnimationEvent();
    //                        newEvent4.functionName = "endScale";
    //                        newEvent4.floatParameter = endScaleLength;
    //                        newEvent4.time = endScaleTime;
    //                        events[eventCount] = newEvent4;
    //                        eventCount++;
    //                    }

    //                    clipanim.events = events;
    //                }

    //                clipanims.Add(clipanim);
    //            }
    //        }
    //        reader.Dispose();

    //        //光效等效果另起文档配置，策划负责维护
    //        string effectPath = dir + "/" + filename + "_Effect.csv";
    //        if (System.IO.File.Exists(effectPath))
    //        {
    //            string effectCfgs = System.IO.File.ReadAllText(effectPath);
    //            if (effectCfgs == null || effectCfgs.Length <= 0)
    //            {
    //                return;
    //            }

    //            ScpReaderEditor effectReader = new ScpReaderEditor(effectCfgs, effectCfgs + "cfg");
    //            int lineCount = effectReader.GetRecordCount();
    //            for (int effecti = 0; effecti < lineCount; effecti++)
    //            {
    //                string effectcommand = effectReader.GetString(effecti, 0, "");
    //                if (effectcommand != "clip")
    //                {
    //                    continue;
    //                }
    //                string effectclipname = effectReader.GetString(effecti, 1, "");
    //                if (effectclipname == null || effectclipname.Length <= 0)
    //                {
    //                    continue;
    //                }

    //                int clipIndex = -1;
    //                for (int si = 0; si < clipanims.Count;si++ )
    //                {
    //                    if (clipanims[si].name == effectclipname)
    //                    {
    //                        clipIndex = si;
    //                        break;
    //                    }
    //                }

    //                if (clipIndex<0)
    //                {
    //                    continue;
    //                }

    //                int BindLightingEffectID1 = effectReader.GetInt(effecti, 2, 0);
    //                float BindLightingEffectTime1 = effectReader.GetFloat(effecti, 3, 0.0f);
    //                int BindLightingEffectID2 = effectReader.GetInt(effecti, 4, 0);
    //                float BindLightingEffectTime2 = effectReader.GetFloat(effecti, 5, 0.0f);
    //                float startScaleTime = effectReader.GetFloat(effecti, 6, 0.0f);
    //                float startScaleLength = effectReader.GetFloat(effecti, 7, 0.0f);
    //                float endScaleTime = effectReader.GetFloat(effecti, 8, 0.0f);
    //                float endScaleLength = effectReader.GetFloat(effecti, 9, 0.0f);
    //                int animSoundID = effectReader.GetInt(effecti, 10, 0);
    //                float startAnimSoundTime = effectReader.GetFloat(effecti, 11, 0.0f);

    //                int eventCount = 0;
    //                if (BindLightingEffectID1 > 0)
    //                {
    //                    eventCount++;
    //                }

    //                if (BindLightingEffectID2 > 0)
    //                {
    //                    eventCount++;
    //                }

    //                if (startScaleTime > 0.0f)
    //                {
    //                    eventCount++;
    //                }

    //                if (endScaleTime > 0.0f)
    //                {
    //                    eventCount++;
    //                }

    //                if (animSoundID > 0)
    //                {
    //                    eventCount++;
    //                }

    //                if (eventCount > 0)
    //                {
    //                    AnimationEvent[] events = new AnimationEvent[eventCount];
    //                    eventCount = 0;
    //                    if (BindLightingEffectID1 > 0)
    //                    {
    //                        AnimationEvent newEvent1 = new AnimationEvent();
    //                        newEvent1.functionName = "BindLighting";
    //                        newEvent1.intParameter = BindLightingEffectID1;
    //                        newEvent1.time = BindLightingEffectTime1;
    //                        events[eventCount] = newEvent1;
    //                        eventCount++;
    //                    }
    //                    if (BindLightingEffectID2 > 0)
    //                    {
    //                        AnimationEvent newEvent2 = new AnimationEvent();
    //                        newEvent2.functionName = "BindLighting";
    //                        newEvent2.intParameter = BindLightingEffectID2;
    //                        newEvent2.time = BindLightingEffectTime2;
    //                        events[eventCount] = newEvent2;
    //                        eventCount++;
    //                    }

    //                    if (startScaleTime > 0.0f)
    //                    {
    //                        AnimationEvent newEvent3 = new AnimationEvent();
    //                        newEvent3.functionName = "beginScale";
    //                        newEvent3.floatParameter = startScaleLength;
    //                        newEvent3.time = startScaleTime;
    //                        events[eventCount] = newEvent3;
    //                        eventCount++;
    //                    }

    //                    if (endScaleTime > 0.0f)
    //                    {
    //                        AnimationEvent newEvent4 = new AnimationEvent();
    //                        newEvent4.functionName = "endScale";
    //                        newEvent4.floatParameter = endScaleLength;
    //                        newEvent4.time = endScaleTime;
    //                        events[eventCount] = newEvent4;
    //                        eventCount++;
    //                    }

    //                    if (animSoundID > 0)
    //                    {
    //                        AnimationEvent newEvent5 = new AnimationEvent();
    //                        newEvent5.functionName = "BindAnimSound";
    //                        newEvent5.intParameter = animSoundID;
    //                        newEvent5.time = startAnimSoundTime;
    //                        events[eventCount] = newEvent5;
    //                        eventCount++;
    //                    }

    //                    clipanims[clipIndex].events = events;
    //                }
    //            }

    //            effectReader.Dispose();

    //        }

    //        modelImporter.clipAnimations = clipanims.ToArray();
    //    }
    //}

    void OnPreprocessModel()
    {
        ModelImporter modelImporter = (ModelImporter)assetImporter;

        modelImporter.globalScale = 1;
        string assetpath = assetImporter.assetPath;
        //MakeAnimationClip(modelImporter);
        modelImporter.meshCompression = ModelImporterMeshCompression.Low;
        if (assetpath.Contains(MonsterAssetPatch) ||
           assetpath.Contains(BossAssetPatch) ||
           assetpath.Contains(NpcAssetPatch) ||
           assetpath.Contains(wingAssetPatch) ||
           assetpath.Contains(effectAssetPatch))
        {
            modelImporter.optimizeGameObjects = true;
            ImportExposedBones(modelImporter);
        }
        else
        {
            modelImporter.optimizeGameObjects = false;
        }


        if (assetpath.Contains(XPjingtouAssetPatch))
        {
            modelImporter.animationType = ModelImporterAnimationType.Legacy;
        }

        if (assetpath.Contains(SceneMeshColliderPatch) ||
           assetpath.Contains(effectParticleModelAssetPatch))
        {
            modelImporter.isReadable = true;
        }
        else
        {
            modelImporter.isReadable = false;
        }

        bool isEffectModel = assetpath.Contains(effectAssetPatch);
        bool isMeshCollider = assetpath.Contains(SceneMeshColliderPatch);
        if (isEffectModel || isMeshCollider)
        {
            modelImporter.importMaterials = false;
        }
    }

    /// <summary>
    /// 导入其他骨骼
    /// </summary>
    /// <param name="modelImporter"></param>
    static void ImportExposedBones(ModelImporter modelImporter)
    {
        string assetpath = modelImporter.assetPath;
        string extName = Path.GetExtension(assetpath).ToLower();
        string fullpath = Path.GetFullPath(assetpath);
        string dir = Path.GetDirectoryName(fullpath);
        string filename = Path.GetFileNameWithoutExtension(fullpath);
        if (filename.EndsWith("_model"))
        {
            filename = filename.Replace("_model", "");
        }
        string ConfigPatch = dir + "/" + filename + ".csv";
        if (extName == ".fbx" && System.IO.File.Exists(ConfigPatch))
        {
            string ConfigString = System.IO.File.ReadAllText(ConfigPatch);
            if (ConfigString == null || ConfigString.Length <= 0)
            {
                return;
            }
            HashSet<string> exposedbonesList = new HashSet<string>();
            List<string> AllTransforms = new List<string>();
            AllTransforms.AddRange(modelImporter.transformPaths);
            ScpReaderEditor reader = new ScpReaderEditor(ConfigString, "BonesExposedConfig");
            int n = reader.GetRecordCount();
            for (int i = 0; i < n; i++)
            {
                string s = reader.GetString(i, 0, string.Empty);
                if (s.Equals(string.Empty))
                {
                    continue;
                }
                if (!s.Equals("exposedbones"))
                {
                    continue;
                }

                string bonesName = reader.GetString(i, 1, string.Empty);
                if (bonesName.Equals(string.Empty))
                {
                    continue;
                }
                bool bFound = false;
                if (!exposedbonesList.Contains(bonesName))
                {
                    foreach (string sb in AllTransforms)
                    {
                        if (sb.Contains(bonesName))
                        {
                            exposedbonesList.Add(bonesName);
                            bFound = true;
                            break;
                        }
                    }
                    if (!bFound)
                    {
                        Debug.LogWarning("找不到骨骼:" + bonesName + ",文件:" + assetpath + ",请在配置中删除");
                    }

                }

            }
            string woudName = "Wound";
            if (!exposedbonesList.Contains(woudName))
            {
                foreach (string sb in AllTransforms)
                {
                    if (sb.Contains(woudName))
                    {
                        exposedbonesList.Add(woudName);
                        break;
                    }
                }
            }
            string[] ssss = new string[exposedbonesList.Count];
            exposedbonesList.CopyTo(ssss);
            modelImporter.extraExposedTransformPaths = ssss;
            exposedbonesList.Clear();
            exposedbonesList = null;
        }
    }

    public static void CheckFBXInImportTime(string path)
    {
        CheckFBXInImportTime(AssetDatabase.LoadAssetAtPath<GameObject>(path));
    }

    public static void CheckFBXInImportTime(GameObject model,string assPath = null)
    {
        string assetpath = AssetDatabase.GetAssetPath(model);
        if(!string.IsNullOrEmpty(assPath))
        {
            assetpath = assPath;
        }
        MeshFilter[] mfs = model.GetComponentsInChildren<MeshFilter>();
        bool isEffectModel = assetpath.Contains(effectAssetPatch);
        bool isEffectParticleSystemModel = assetpath.Contains(effectParticleModelAssetPatch);
        bool isMeshCollider = assetpath.Contains(SceneMeshColliderPatch);

        if (isEffectParticleSystemModel)
        {
            foreach (MeshFilter mf in mfs)
            {
                if (mf.sharedMesh.triangles.Length / 3 > 500)
                {
                    Debug.LogError("资源:" + assetpath + "面超过500,当前面:" + mf.sharedMesh.triangles.Length / 3);
                }

            }
        }
    }

    void OnPostprocessModel (GameObject model)
    {
        //if(null == writer)
        //{
        //    writer = new StreamWriter("E:/不合格的资源.csv", false, new System.Text.UTF8Encoding(true));

        //}

        ModelImporter modelImporter = (ModelImporter)assetImporter;
        string assetpath = assetImporter.assetPath;

        if (assetpath.Contains(SceneAssetPatch) ||
            assetpath.Contains(effectAssetPatch))
        {
            if (modelImporter.defaultClipAnimations == null || modelImporter.defaultClipAnimations.Length <= 0)
            {
                if (modelImporter.animationType != ModelImporterAnimationType.None)
                {
                    modelImporter.animationType = ModelImporterAnimationType.None;
                    modelImporter.importAnimation = false;
                    AssetDatabase.ImportAsset(assetpath);
                    return;
                }

            }

        }

        MeshFilter []mfs = model.GetComponentsInChildren<MeshFilter>();
        bool isEffectModel = assetpath.Contains(effectAssetPatch);
        bool isEffectParticleSystemModel = assetpath.Contains(effectParticleModelAssetPatch);
        bool isMeshCollider = assetpath.Contains(SceneMeshColliderPatch);


        //删掉UV1，2，3，4和顶点色，节省内存
        List<Vector2> nullUV = null;
        List<Color> nullColor = null;
        //int length = 0;
        if(mfs.Length > 0)
        {
            foreach(MeshFilter mf in mfs)
            {
                if (isMeshCollider)
                {
                    mf.sharedMesh.SetUVs(1, nullUV);
                    mf.sharedMesh.SetUVs(2, nullUV);
                    mf.sharedMesh.SetUVs(3, nullUV);
                    mf.sharedMesh.SetColors(nullColor);
                    continue;
                }
                if (!modelImporter.generateSecondaryUV)
                {
                    //length = mf.sharedMesh.uv2.Length;
                    //if (length > 0)
                    //{
                    //    Debug.LogWarning("资源:" + modelImporter.assetPath + "有多余的UV2，请删除！");
                    //}
                    mf.sharedMesh.SetUVs(1, nullUV);
                }
                //length = mf.sharedMesh.uv3.Length;
                //if (length > 0)
                //{
                //    Debug.LogWarning("资源:" + modelImporter.assetPath + "有多余的UV3，请删除！");
                //}
                //length = mf.sharedMesh.uv4.Length;
                //if (length > 0)
                //{
                //    Debug.LogWarning("资源:" + modelImporter.assetPath + "有多余的UV4，请删除！");
                //}

                //if(mf.sharedMesh.triangles.Length / 3 > 3000)
                //{
                //    Debug.LogWarning("资源:" + modelImporter.assetPath + "面超过3000,当前面:" + mf.sharedMesh.triangles.Length / 3);
                //}

                //特效模型可以有顶点色
                if (!isEffectModel)
                {
                    //length = mf.sharedMesh.colors.Length;
                    //if (length > 0)
                    //{
                    //    Debug.LogWarning("资源:" + modelImporter.assetPath + "有多余的顶点色信息，请删除！");
                    //    //writer.WriteLine("资源:" + modelImporter.assetPath + "有多余的UV4，请删除！");
                    //}
                    mf.sharedMesh.SetColors(nullColor);
                }
                mf.sharedMesh.SetUVs(2, nullUV);
                mf.sharedMesh.SetUVs(3, nullUV);
             
            }
        }

        SkinnedMeshRenderer []smrs = model.GetComponentsInChildren<SkinnedMeshRenderer>();
        if(smrs.Length > 0)
        {
            foreach (SkinnedMeshRenderer smr in smrs)
            {
                //length = smr.sharedMesh.uv2.Length;
                //if (length > 0)
                //{
                //    Debug.LogWarning("资源:" + modelImporter.assetPath + "有多余的UV2，请删除！");
                //}

                //length = smr.sharedMesh.uv3.Length;
                //if (length > 0)
                //{
                //    Debug.LogWarning("资源:" + modelImporter.assetPath + "有多余的UV3，请删除！");
                //}
                //length = smr.sharedMesh.uv4.Length;
                //if (length > 0)
                //{
                //    Debug.LogWarning("资源:" + modelImporter.assetPath + "有多余的UV4，请删除！");
                //}
                //length = smr.sharedMesh.colors.Length;
                //if (length > 0)
                //{
                //    Debug.LogWarning("资源:" + modelImporter.assetPath + "有多余的顶点色信息，请删除！");
                //    //writer.WriteLine("资源:" + modelImporter.assetPath + "有多余的顶点色信息，请删除！");
                //}
                smr.sharedMesh.SetUVs(1, nullUV);
                smr.sharedMesh.SetUVs(2, nullUV);
                smr.sharedMesh.SetUVs(3, nullUV);
                smr.sharedMesh.SetColors(nullColor);
            }
        }
        CheckFBXInImportTime(model, assetpath);
        //if(null != writer)
        //{
        //    writer.Flush();
        //}
    }

    private static bool IsFBXAsset(string patch)
    {
        ModelImporter ap = AssetImporter.GetAtPath(patch) as ModelImporter; 
        if (!ap)
        {
            return false;
        }
        string asPatch = patch;
        if(!asPatch.Contains(ArtitsPatch))
        {
            return false;
        }
        return true;
    }

    private static AssetBundleManagerEditor.AssetBundleEditorInfo BuildFBXAssetBundleInfo(AssetImporter assetImporter,string referenceAssetbundleName)
    {
        string AbName = string.Empty;
        string AbVirtant = string.Empty;
        if (!assetImporter)
        {
            return null;
        }

        if (!CheckAssetDependices(assetImporter.assetPath))
        {
            return null;
        }

        string asPatch = assetImporter.assetPath;

        //移除头部
        asPatch = asPatch.Replace(ArtitsPatch, "");

        string[] strs = asPatch.Split('/');

        //没有放到其他文件夹下，则使用默认的包
        if (strs.Length <= 1)
        {
            asPatch = "Default";
        }
        else
        {
            //移除名字
            asPatch = asPatch.Replace(strs[strs.Length - 1], "");

            //再移除最后的斜杠
            asPatch = asPatch.Remove(asPatch.Length - 1);
        }


        //最后就是包名
        AbName = "Model/" + asPatch;
        AbVirtant = AssetBundleManager.FBXVariant;

        if (AbName.Equals(string.Empty) || AbVirtant.Equals(string.Empty))
        {
            return null;
        }

        string assetName = Path.GetFileNameWithoutExtension(assetImporter.assetPath);
        string assetGUID = AssetBundleManagerEditor.GetAssetGUIDFromMeta(assetImporter.assetPath);

        if (assetName.Equals(string.Empty) || assetGUID.Equals(string.Empty))
        {
            return null;
        }
        if (!referenceAssetbundleName.Equals(string.Empty))
        {
            AbName = referenceAssetbundleName;
        }
        AssetBundleManagerEditor.AssetBundleEditorInfo info = new AssetBundleManagerEditor.AssetBundleEditorInfo();
        info.AssetBundleName = AbName;
        info.AssetBundleVarian = AbVirtant;
        info.guid = assetGUID;
        info.assetName = assetName;
        return info;
    }


        /// <summary>
    /// 构建ab，原因是被需要的东西引用了
    /// </summary>
    /// <param name="path"></param>
    /// <param name="referenceAssetbundleName">参考名，如果参考不为空，则使用参考的</param>
    /// <param name="referenceVariant">参考名，如果参考不为空，则使用参考的<</param>
    /// <returns></returns>
    public static ResConfigData MakeFBXAssetBundleFromReference(string path,string referenceAssetbundleName)
    {
        AssetBundleManagerEditor.AssetBundleEditorInfo conf = null;
        if (path.Equals(string.Empty))
        {
            return null;
        }

        AssetImporter assetImporter = AssetImporter.GetAtPath(path);
        if (!assetImporter)
        {
            return null;
        }

        if (!IsCanGeneratePrefabUnionRes(path))
        {
            referenceAssetbundleName = string.Empty;
        }

        //然后刷一下名字
        conf = BuildFBXAssetBundleInfo(assetImporter, referenceAssetbundleName);
        //如果还是取不到，就说明不是FBX了。
        if (conf == null)
        {
            return null;
        }
        assetImporter.userData += FromReferencebKey;
        //没有在打包的时候，不需要写入ab包，因为编辑器不走ab包
        if(AssetBundleManagerEditor.isBuildingPackage)
        {
            PushPrefabAssetImport(assetImporter, conf);
           // SaveAssetBundleData(assetImporter, conf);
        }
      
        return conf.ConvertToResConfigData();
    }


    static void MakeFBXAssetBundle(ModelImporter assetImporter)
    {
        if (!assetImporter)
        {
            return;
        }
        string asPatch = assetImporter.assetPath;

        if (!IsFBXAsset(asPatch))
        {
            return;
        }
        /*
        * 只有Artist目录下的FBX才会被打包
        * 一般情况下,FBX的包是通过预制体来生成包的，但是也有一种例外
        * 就是在通用资源(Artist/CommonArtist)下的fbx，这些是必须要打包的。
        */

        bool bFromPrefab = assetImporter.userData.Contains(FromReferencebKey);
        assetImporter.userData = assetImporter.userData.Replace(FromReferencebKey, "");

        if (bFromPrefab)
        {
            SaveAssetBundleUserData(assetImporter, assetImporter.userData);
            return;
        }

        //不是来自于prefab的，再commonartist目录下才可以生成AB名
        if (!IsCommonArtistAssets(asPatch))
        {
            return;
        }


        AssetBundleManagerEditor.AssetBundleEditorInfo info = BuildFBXAssetBundleInfo(assetImporter,string.Empty);
        if (info == null)
        {
            return;
        }
        SaveAssetBundleData(assetImporter, info);
    }
}
