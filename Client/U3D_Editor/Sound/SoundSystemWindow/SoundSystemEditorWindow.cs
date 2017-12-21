/*******************************************************************
** 文件名:	SoundSystemEditorWindow.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	声音系统
** 应  用:  	声音编辑器main部分

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

namespace SoundSystem
{
    public partial class SoundSystemEditorWindow : EditorWindow
    {
        public class TreeViewNode
        {
            /// <summary>
            /// 所属的audio
            /// </summary>
            public HashSet<AudioClipInfo> AudioClipList = new HashSet<AudioClipInfo>();

            /// <summary>
            /// 所属的配置
            /// </summary>
            public HashSet<EditorSoundConfigData> configList = new HashSet<EditorSoundConfigData>();

            public string TreeViewName = string.Empty;
            public string ParentTreeViewName = string.Empty;
            public HashSet<string> ChildViewList = new HashSet<string>();
            public bool isOpen = false;

        }


        //private static void BuildAssetBundle()
        //{
        //    string fullPatch = Application.streamingAssetsPath + "/" + ResourceManager.AssetBundlePatch;
        //    if (!System.IO.Directory.Exists(fullPatch))
        //    {
        //        System.IO.Directory.CreateDirectory(fullPatch);
        //    }
        //    BuildPipeline.BuildAssetBundles(fullPatch, BuildAssetBundleOptions.None, BuildTarget.StandaloneWindows);
        //    AssetDatabase.Refresh();
        //}

        public static SoundSystemEditorWindow EditorInstance = null;

        [MenuItem("Tools/SoundEditor")]
        static void OpenSoundSystemEditor()
        {
            EditorInstance = EditorWindow.GetWindow<SoundSystemEditorWindow>(false, "SoundEditor", true);
            EditorInstance.Show();
        }

        public void OnEnable()
        {
            LoadAudioClipSource();
            LoadSoundConfig();
        }

        public void OnDisable()
        {
            StopSound();
            UnLoadAudioClipSource();
            UnLoadSoundConfig();
        }

        public void OnGUI()
        {
            DrawAudioResourceList();
            DrawConfigList();

            DrawConfigInfo();
            DrawAudioClipInfo();

            DrawImportanButton();

            DrawIOButton();
        }

        public static void ReCaclSound()
        {
            EditorInstance = EditorWindow.GetWindow<SoundSystemEditorWindow>(false, "SoundEditor", true);
            EditorInstance.Show();

            AdjustAll();
            ExportConfig();
        }

        /// <summary>
        /// 匹配配置里的音效和资源
        /// </summary>
        /// <param name="PointedGUIDList">制定匹配GUID，没有指定则匹配全部</param>
        private static void AdjustSourceAndConfig(List<string> PointedGUIDList = null)
        {
            if (null != PointedGUIDList && PointedGUIDList.Count > 0)
            {
                AdjustFromList(PointedGUIDList);
            }
            else
            {
                AdjustAll();
            }

            //匹配完了之后刷新TreeView
            GenrateConfigTreeView();
            GenrateResTreeView();

            currentSelectedClipInfo = null;
            currentSelectedConfig = null;
        }

        private static void AdjustAll()
        {
            //遍历配置去匹配资源
            foreach (EditorSoundConfigData cof in CurrentSoundConfigTable.Values)
            {

                bool bAdjust = false;
                foreach (AudioClipInfo clipInfo in AudioSourceList)
                {
                    string s = clipInfo.AssetGUID;
                    
                    //配置里的资源是存在的
                    if (cof.data.AssetGUID.Equals(s))
                    {
                        cof.data.AssetBundleName = clipInfo.AssetBundleName;
                        cof.data.AssetName = clipInfo.AssetName;
                        cof.data.AssetGUID = clipInfo.AssetGUID;
                        cof.data.AssetSize_X = clipInfo.AudioLength;
                        cof.data.AssetSize_Y = clipInfo.AudioLength;
                        bAdjust = true;
                        break;
                    }
                    
                }

                if (!bAdjust)
                {
                    cof.data.AssetBundleName = string.Empty;
                    cof.data.AssetName = string.Empty;
                    cof.data.AssetGUID = string.Empty;

                    SoundManager.SoundSystemLog("配置:" + cof.data.nID + "无法找到匹配的资源!");
                }
            }
        }

        private static void AdjustFromList(List<string> PointedGUIDList)
        {
            //移动了资源，更新编辑器里对应的资源配置
            if (null != PointedGUIDList && PointedGUIDList.Count > 0)
            {
                foreach (EditorSoundConfigData cof in CurrentSoundConfigTable.Values)
                {
                    bool bAdjust = false;
                    foreach (string s in PointedGUIDList)
                    {
                        //找出一样的GUID
                        if (cof.data.AssetGUID.Equals(s))
                        {
                            //移动的资源还在Sound文件夹里面
                            if (AudioSourceTable.ContainsKey(s))
                            {
                                AudioClipInfo inf = AudioSourceTable[s];
                                cof.data.AssetBundleName = inf.AssetBundleName;
                                cof.data.AssetName = inf.AssetName;
                                bAdjust = true;

                                //更新树状图
                                GenerateHostTreeView_Config(cof);
                                break;
                            }
                        }
                    }

                    if (!bAdjust)
                    {
                        cof.data.AssetBundleName = string.Empty;
                        cof.data.AssetName = string.Empty;
                        cof.data.AssetGUID = string.Empty;

                        SoundManager.SoundSystemLog("配置:" + cof.data.nID + "无法找到匹配的资源!");
                    }
                }
            }
        }
    }
}
