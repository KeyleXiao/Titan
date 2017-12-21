/*******************************************************************
** 文件名:	SoundSystemEditorWindow_Resource.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	声音系统
** 应  用:  	声音编辑器资源相关部分

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
    public class AudioClipInfo
    {
        public AudioClip ResObj = null;
        public float AudioLength = -1.0f;
        public string AssetBundleName = string.Empty;
        public string AssetName = string.Empty;
        public string AssetNameWithExe = string.Empty;
        public string AssetGUID = string.Empty;
        public string RelativePatch = string.Empty;
        public string AbsolutePatch = string.Empty;

        public string HostTreeViewName = string.Empty;
        public string ParentTreeViewName = string.Empty;
    }

    public partial class SoundSystemEditorWindow : EditorWindow
    {
        /// <summary>
        /// AudioClip资源表
        /// </summary>
        private static HashSet<AudioClipInfo> AudioSourceList = new HashSet<AudioClipInfo>();


        /// <summary>
        /// AudioClip和他的GUID
        /// </summary>
        private static Dictionary<string, AudioClipInfo> AudioSourceTable = new Dictionary<string, AudioClipInfo>();


        /// <summary>
        /// 所有的Audio资源绝对路径
        /// </summary>
        private static List<string> AudioAssetAbsolutePathList = new List<string>();

        /// <summary>
        /// 所有的Audio资源相对路径
        /// </summary>
        private static List<string> AudioAssetRelativePathList = new List<string>();

        private static AudioClipInfo currentSelectedClipInfo;

        /// <summary>
        /// 计算所有的音频路径
        /// </summary>
        private static void GenerateAllAudioPatch()
        {
            AudioAssetAbsolutePathList.Clear();
            AudioAssetRelativePathList.Clear();
            List<string> folders = new List<string>();

            //取得Sounds目录下的所有文件
            folders.AddRange(System.IO.Directory.GetFiles(Application.dataPath + "/Sounds", "*.*", SearchOption.AllDirectories));
            foreach (string s in folders)
            {
                string s1 = s.Remove(0, s.Length - 5);
                //不是meta文件，不是BootCampSound文件夹
                if (!s1.Contains(".meta") && !s.Contains("BootCampSound"))
                {
                    s1 = s.Replace("\\", "/");
                    AudioAssetAbsolutePathList.Add(s1);

                    //相对路径
                    s1 = s.Replace(Application.dataPath, "Assets");
                    s1 = s1.Replace("\\", "/");
                    AudioAssetRelativePathList.Add(s1);
                }
            }
        }

        /// <summary>
        /// 加载所有的音频资源
        /// </summary>
        public static void LoadAudioClipSource()
        {
            GenerateAllAudioPatch();
            for (int i = 0; i < AudioAssetRelativePathList.Count; i++)
            {
                AudioClip cli = AssetDatabase.LoadAssetAtPath<AudioClip>(AudioAssetRelativePathList[i]);
                if (cli)
                {

                    string guid = AssetBundleManagerEditor.GetAssetGUIDFromMeta(AudioAssetAbsolutePathList[i]);
                    if (!string.IsNullOrEmpty(guid))
                    {
                        AudioClipInfo inf = new AudioClipInfo();
                        inf.AssetGUID = guid;
                        inf.AssetName = GetAssetNameFromPatchWithOutExe(AudioAssetRelativePathList[i]);
                        inf.AssetNameWithExe =  GetAssetNameFromPatch(AudioAssetRelativePathList[i]);
                        AssetImporter importer = AssetImporter.GetAtPath(AudioAssetRelativePathList[i]);

                        inf.AssetBundleName = importer.assetBundleName;
                        if (!string.IsNullOrEmpty(importer.assetBundleVariant))
                        {
                            inf.AssetBundleName += "." + importer.assetBundleVariant;
                        }
                        inf.AudioLength = cli.length;
                        inf.ResObj = cli;
                        inf.AbsolutePatch = AudioAssetAbsolutePathList[i];
                        inf.RelativePatch = AudioAssetRelativePathList[i];
                        inf.HostTreeViewName = string.Empty;
                        AudioSourceList.Add(inf);
                        AudioSourceTable.Add(guid, inf);
                    }

                }
            }

            GenrateResTreeView();
        }


        /// <summary>
        /// 卸载所有的音频资源
        /// </summary>
        public static void UnLoadAudioClipSource()
        {
            foreach (AudioClipInfo s in AudioSourceList)
            {
                Resources.UnloadAsset(s.ResObj);
            }
            ResRootTreeView.Clear();
            ResSecondRootTreeView.Clear();
            AudioSourceList.Clear();
            AudioAssetAbsolutePathList.Clear();
            AudioAssetRelativePathList.Clear();
            AudioSourceTable.Clear();
            currentSelectedClipInfo = default(AudioClipInfo);
        }

        /// <summary>
        /// 刷新资源
        /// </summary>
        /// <param name="movedAssetGUIDList">移动资源的GUID，如果移动了资源编辑器需要更新对应的配置里的资源</param>
        public static void RefrshAudioClipSource(List<string> movedAssetGUIDList = null)
        {
            UnLoadAudioClipSource();
            LoadAudioClipSource();

            currentSelectedClipInfo = default(AudioClipInfo);
            currentSelectedConfig = null;

            AdjustSourceAndConfig(movedAssetGUIDList);
            if (EditorInstance)
            {
                EditorInstance.Repaint();
            }
        }


        private static string GetAssetNameFromPatch(string patch)
        {
            string[] temp = patch.Split('/');
            string fullName = temp[temp.Length - 1];
            //移除后面的".wav,.mp3,.ogg"之类的
            //return fullName.Remove(fullName.Length - 4, 4);

            return fullName;
        }

        private static string GetAssetNameFromPatchWithOutExe(string patch)
        {
            string[] temp = patch.Split('/');
            string fullName = temp[temp.Length - 1];
            //移除后面的".wav,.mp3,.ogg"之类的
            return fullName.Remove(fullName.Length - 4, 4);
        }

        private static AudioSource m_AudioInstance = null;

        private static bool bPlayingSound = false;
        private static void PlaySound(AudioClip clip)
        {
            if (!m_AudioInstance)
            {
                GameObject go = new GameObject();
                go.hideFlags = HideFlags.HideAndDontSave;
                m_AudioInstance = go.AddComponent<AudioSource>();
            }

            m_AudioInstance.clip = clip;
            m_AudioInstance.Play();
            bPlayingSound = true;
        }

        private static void StopSound()
        {
            if (m_AudioInstance)
            {
                m_AudioInstance.Stop();
            }
            bPlayingSound = false;
        }

        #region 树状图

        /// <summary>
        /// 配置跟目录名
        /// </summary>
        private static Dictionary<string, TreeViewNode> ResRootTreeView = new Dictionary<string, TreeViewNode>();

        /// <summary>
        /// 配置第二根目录
        /// </summary>
        private static Dictionary<string, TreeViewNode> ResSecondRootTreeView = new Dictionary<string, TreeViewNode>();

        static void GenrateResTreeView()
        {
            ResRootTreeView.Clear();
            ResSecondRootTreeView.Clear();

            //在Sounds文件夹下的，直接放在default里
            TreeViewNode no = new TreeViewNode();
            no.TreeViewName = "Default";
            no.ParentTreeViewName = string.Empty;
            ResRootTreeView.Add(DefaultRoot, no);

            //遍历所有的资源，根据assetbundlename来分类
            foreach (AudioClipInfo s in AudioSourceList)
            {
                GenerateHostTreeView_Res(s);
            }
        }

        /// <summary>
        /// 计算属于哪个树节点
        /// </summary>
        /// <param name="assetBundleName"></param>
        /// <returns></returns>
        static void GenerateHostTreeView_Res(AudioClipInfo s)
        {
            //如果assetbundlename是空的,不处理
            if (string.IsNullOrEmpty(s.AssetBundleName))
            {
                return ;
            }

            
            //用于判断的节点不能有后缀
            string treepatch = s.AssetBundleName.Replace(".soundpkg", "");
            //去掉sound头
            treepatch = treepatch.Replace("sound/", "");

            string[] splitString = treepatch.Split('/');

            string root = splitString[0];

            //如果根节点表里有这个节点
            if (ResRootTreeView.ContainsKey(root))
            {
                s.ParentTreeViewName = string.Empty;
                //再去检查第二根节点
                CheckSecondRootView_Res(splitString, root, s);
            }
            else
            {
                //新建跟节点
                TreeViewNode no = new TreeViewNode();
                no.TreeViewName = root;
                no.ParentTreeViewName = string.Empty;
                ResRootTreeView.Add(root, no);

                //再去检查第二根节点
                CheckSecondRootView_Res(splitString, root, s);
            }
        }

        static void CheckSecondRootView_Res(string[] splitString, string root, AudioClipInfo data)
        {
            //如果长度是2，表明，只在这个跟节点里而已，直接添加进去就行
            if (splitString.Length == 1)
            {
                data.ParentTreeViewName = string.Empty;
                data.HostTreeViewName = root;
                ResRootTreeView[root].AudioClipList.Add(data);
            }
            else//如果不是，则看看次级节点
            {
                //第二根节点
                string secondRoot = splitString[1];

                
                //第二根节点已经存在,直接添加进去就行
                if (ResSecondRootTreeView.ContainsKey(secondRoot))
                {
                    data.HostTreeViewName = secondRoot;
                    data.ParentTreeViewName = root;

                    ResSecondRootTreeView[secondRoot].AudioClipList.Add(data);
                }
                else //不存在，新建一个,再添加进去
                {
                    TreeViewNode nod = null;
                    ResRootTreeView[root].ChildViewList.Add(secondRoot);
                    nod = new TreeViewNode();
                    nod.TreeViewName = secondRoot;
                    nod.ParentTreeViewName = root;

                    nod.AudioClipList.Add(data);
                    data.HostTreeViewName = secondRoot;
                    data.ParentTreeViewName = root;
                   
                    ResSecondRootTreeView.Add(secondRoot, nod);
                }
            }
        }

        #endregion
    }
}
