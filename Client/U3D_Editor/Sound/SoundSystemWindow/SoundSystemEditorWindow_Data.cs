/*******************************************************************
** 文件名:	SoundSystemEditorWindow_Data.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	声音系统
** 应  用:  	声音编辑器数据相关部分

**************************** 修改记录 ******************************
** 修改人: 昔文博
** 日  期: 2016/7/11
** 描  述: 增加声音类型区分，增加音量控制和静音设置
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
        public class EditorSoundConfigData
        {
            public SoundConfigData data;
            public string HostTreeViewName;
            public string ParentTreeViewName;
        }

        private static Dictionary<int, EditorSoundConfigData> CurrentSoundConfigTable = new Dictionary<int, EditorSoundConfigData>();

        private static EditorSoundConfigData currentSelectedConfig;

        private static void ExportConfig()
        {
            string patch = GetConfigPatch();
            string haderLine1 = "音效ID,声音类型,混音组,混音组索引,旁路效果,路由混响,立刻播放,优先级,高音,立体声,全局混响,是否3D音效,多普勒,穿透,衰减类型,最小距离,最大距离,播放次数,持续时间,延迟播放,每次播放之间的间隔,资源包名,资源名,资源GUID,资源尺寸X,资源尺寸Y,依赖资源GUID";
            string haderLine2 = "int,int,int,int,int,int,int,int,float,float,float,int,float,int,int,float,float,int,float,float,float,string,string,sting,float,float,string";

            StreamWriter writer = new StreamWriter(patch,false, new System.Text.UTF8Encoding(true));
            writer.WriteLine(haderLine1);
            writer.WriteLine(haderLine2);

            foreach (EditorSoundConfigData cof in CurrentSoundConfigTable.Values)
            {
                if (string.IsNullOrEmpty(cof.data.AssetBundleName))
                {
                    Debug.LogWarning("配置ID:" + cof.data.nID + ",没有绑定资源，已忽略!");
                    continue;
                }
                string s = "";
                s += cof.data.nID;
                s += ",";
                s += (int)cof.data.soundType;
                s += ",";
                s += (int)cof.data.mixerGroupType;
                s += ",";
                s += cof.data.nMixerGroupIndex;
                s += ",";
                s += cof.data.bBypassEffect ? "1" : "0";
                s += ",";
                s += cof.data.bBypassReverbZone ? "1" : "0";
                s += ",";
                s += cof.data.bPlayOnAwake ? "1" : "0";
                s += ",";
                s += cof.data.nPriority;
                s += ",";
                s += cof.data.fPitch;
                s += ",";
                s += cof.data.fStereoPan;
                s += ",";
                s += cof.data.fReverZoneMix;
                s += ",";
                s += cof.data.b3DSound ? "1" : "0";
                s += ",";
                s += cof.data.fDopplerLv;
                s += ",";
                s += cof.data.nSpread;
                s += ",";
                s += (int)cof.data.audioRoffType;
                s += ",";
                s += cof.data.fMinDistance;
                s += ",";
                s += cof.data.fMaxDistance;
                s += ",";
                s += cof.data.nPlayTimes;
                s += ",";
                s += cof.data.fDuartion;
                s += ",";
                s += cof.data.fDelayPlayTime;
                s += ",";
                s += cof.data.fIntervalBetweenPlayTimes;
                s += ",";
                s += cof.data.AssetBundleName;
                s += ",";
                s += cof.data.AssetName;
                s += ",";
                s += cof.data.AssetGUID;
                s += ",";
                s += cof.data.AssetSize_X.ToString("F3");
                s += ",";
                s += cof.data.AssetSize_Y.ToString("F3");
                s += ",";
                s += cof.data.szDependAssetGUID;
                writer.WriteLine(s);
            }

            writer.Flush();
            writer.Close();
        }

        private static void CreateNewConfig()
        {
            EditorSoundConfigData newdata = new EditorSoundConfigData();
            newdata.data = new SoundConfigData();
            newdata.data.nID = SoundIDAllocator.Instance.AllocId();
            newdata.data.soundType = EMSoundType.Skill;
            newdata.data.mixerGroupType = AudioMixerGroupType.Actor;
            newdata.data.nMixerGroupIndex = 0;
            newdata.data.bBypassEffect = false;
            newdata.data.bBypassReverbZone = false;
            newdata.data.bPlayOnAwake = true;
            newdata.data.nPriority = 128;
            newdata.data.fPitch = 1;
            newdata.data.fStereoPan = 0;
            newdata.data.fReverZoneMix = 1;
            newdata.data.b3DSound = true;
            newdata.data.fDopplerLv = 1;
            newdata.data.nSpread = 0;
            newdata.data.audioRoffType = AudioRolloffMode.Logarithmic;
            newdata.data.fMinDistance = 1;
            newdata.data.fMaxDistance = 15;
            newdata.data.nPlayTimes = 1;
            newdata.data.fDuartion = 0;
            newdata.data.fDelayPlayTime = 0;
            newdata.data.fIntervalBetweenPlayTimes = 0;
            newdata.data.AssetBundleName = string.Empty;
            newdata.data.AssetName = string.Empty;
            newdata.data.AssetGUID = string.Empty;
            newdata.data.szDependAssetGUID = string.Empty;
            newdata.HostTreeViewName = DefaultRoot;
            SoundIDAllocator.Instance.AcceptAlloc();

            CurrentSoundConfigTable.Add(newdata.data.nID, newdata);
            currentSelectedConfig = newdata;

            ConfigRootTreeView[NewConfigRoot].configList.Add(newdata);
            GenerateHostTreeView_Config(currentSelectedConfig);
            CloseAllTreeView();
            OpenTreeView(currentSelectedConfig.HostTreeViewName);
        }

        private static void DeleteCurrentConfig()
        {
            CurrentSoundConfigTable.Remove(currentSelectedConfig.data.nID);
            DeleteTreeView_Config(currentSelectedConfig);
            SoundIDAllocator.Instance.PushUnUsedId(currentSelectedConfig.data.nID);
            currentSelectedConfig = null;
        }

        private static void LoadSoundConfig()
        {
            string patch = GetConfigPatch();

            //if (!File.Exists(patch))
            //{
            //    return;
            //}
            //string stSchemeAllText = System.IO.File.ReadAllText(patch);

            ////去掉\n,因为ScpReader以"\r"分割
            //stSchemeAllText = stSchemeAllText.Replace("\n", "");
            ////再干掉最后的"\r"
            //stSchemeAllText = stSchemeAllText.Remove(stSchemeAllText.Length - 1, 1);
            //ScpReader SoundReader = new ScpReader(stSchemeAllText, "SoundConfig", 2);

            ScpReader SoundReader = new ScpReader(patch, true, 2);

            // 遍歷整個表并存儲起來
            for (int i = 0; i < SoundReader.GetRecordCount(); ++i)
            {
                EditorSoundConfigData cfgData = new EditorSoundConfigData();
                cfgData.data = new SoundConfigData();
                cfgData.data.nID = SoundReader.GetInt(i, (int)SoundConfigCol.COL_ID, 0);
                cfgData.data.soundType = (EMSoundType)SoundReader.GetInt(i, (int)SoundConfigCol.COL_Type, 2);
                cfgData.data.mixerGroupType = (AudioMixerGroupType)SoundReader.GetInt(i, (int)SoundConfigCol.COL_MixerGroupType, 1);
                cfgData.data.nMixerGroupIndex = SoundReader.GetInt(i, (int)SoundConfigCol.COL_MixerGroupIndex, 0);
                cfgData.data.bBypassEffect = SoundReader.GetInt(i, (int)SoundConfigCol.COL_BypassEffect, 0) > 0;
                cfgData.data.bBypassReverbZone = SoundReader.GetInt(i, (int)SoundConfigCol.COL_BypassReverbZone, 0) > 0;
                cfgData.data.bPlayOnAwake = SoundReader.GetInt(i, (int)SoundConfigCol.COL_PlayOnAwake, 1) > 0;
                cfgData.data.nPriority = Mathf.Clamp(SoundReader.GetInt(i, (int)SoundConfigCol.COL_Priority, 128), 0, 256);
                cfgData.data.fPitch = Mathf.Clamp(SoundReader.GetFloat(i, (int)SoundConfigCol.COL_Pitch, 1), -3.0f, 3.0f);
                cfgData.data.fStereoPan = Mathf.Clamp(SoundReader.GetFloat(i, (int)SoundConfigCol.COL_StereoPan, 0), -1.0f, 1.0f);
                cfgData.data.fReverZoneMix = Mathf.Clamp(SoundReader.GetFloat(i, (int)SoundConfigCol.COL_ReverZoneMix, 1), 0, 1.1f);
                cfgData.data.b3DSound = SoundReader.GetInt(i, (int)SoundConfigCol.COL_3DSound, 1) > 0;             
                cfgData.data.fDopplerLv = Mathf.Clamp(SoundReader.GetFloat(i, (int)SoundConfigCol.COL_DopplerLv, 1), 0, 5.0f);
                cfgData.data.nSpread = Mathf.Clamp(SoundReader.GetInt(i, (int)SoundConfigCol.COL_Spread, 0), 0, 360); ;
                cfgData.data.audioRoffType = (AudioRolloffMode)SoundReader.GetInt(i, (int)SoundConfigCol.COL_RolloffMode, 0);
                cfgData.data.fMinDistance = SoundReader.GetFloat(i, (int)SoundConfigCol.COL_MinDistance, 1);
                cfgData.data.fMaxDistance = SoundReader.GetFloat(i, (int)SoundConfigCol.COL_MaxDistance, 15);
                cfgData.data.nPlayTimes = SoundReader.GetInt(i, (int)SoundConfigCol.COL_PlayTimes, 1);
                cfgData.data.fDuartion = SoundReader.GetFloat(i, (int)SoundConfigCol.COL_Duartion, 0);
                cfgData.data.fDelayPlayTime = SoundReader.GetFloat(i, (int)SoundConfigCol.COL_DelayTime, 0);
                cfgData.data.fIntervalBetweenPlayTimes = SoundReader.GetFloat(i, (int)SoundConfigCol.COL_IntervalBetweenPlayTimes, 0);
                cfgData.data.AssetBundleName = SoundReader.GetString(i, (int)SoundConfigCol.COL_AssetBundleName, "");
                cfgData.data.AssetName = SoundReader.GetString(i, (int)SoundConfigCol.COL_AssetName, "");
                cfgData.data.AssetGUID = SoundReader.GetString(i, (int)SoundConfigCol.COL_AssetGUID, "");
                cfgData.data.AssetSize_X = SoundReader.GetFloat(i, (int)SoundConfigCol.COL_AssetSize_X, -1);
                cfgData.data.AssetSize_Y = SoundReader.GetFloat(i, (int)SoundConfigCol.COL_AssetSize_Y, -1);
                cfgData.data.szDependAssetGUID = SoundReader.GetString(i, (int)SoundConfigCol.COL_DependAssetGUID, "");
                cfgData.HostTreeViewName = string.Empty;
                CurrentSoundConfigTable.Add(cfgData.data.nID, cfgData);
                SoundIDAllocator.Instance.PushUsedId(cfgData.data.nID);
            }
            SoundIDAllocator.Instance.CalculateUnUsedID();
            SoundReader.Dispose();
            SoundReader = null;

            currentSelectedConfig = null;
            GenrateConfigTreeView();
        }

        private static string GetConfigPatch()
        {
            //int i = Application.dataPath.LastIndexOf("/");
            //int l = Application.dataPath.Length;

            //string dataPatch = Application.dataPath.Substring(0, i);
            //dataPatch = dataPatch + "/Scp/" + SoundManager.ConfigFileName;
            string dataPatch = ViewConfigManager.Editor_DataScpPath + (SoundManager.ConfigFileName);

            return dataPatch;
        }

        private static void UnLoadSoundConfig()
        {
            SoundIDAllocator.Instance.ClearIDAllocator();
            CurrentSoundConfigTable.Clear();
            ConfigRootTreeView.Clear();
            ConfigSecondRootTreeView.Clear();
        }

        private static void InsertSoureToConfig()
        {
            if (currentSelectedClipInfo != null && currentSelectedConfig != null)
            {
                currentSelectedConfig.data.AssetBundleName = currentSelectedClipInfo.AssetBundleName;
                currentSelectedConfig.data.AssetName = currentSelectedClipInfo.AssetName;
                currentSelectedConfig.data.AssetGUID = currentSelectedClipInfo.AssetGUID;
                currentSelectedConfig.data.AssetSize_X = currentSelectedClipInfo.AudioLength;
                currentSelectedConfig.data.AssetSize_Y = currentSelectedClipInfo.AudioLength;
                GenerateHostTreeView_Config(currentSelectedConfig);
                CloseAllTreeView();
                OpenTreeView(currentSelectedConfig.HostTreeViewName);
            }
        }

        private static Dictionary<int, SoundConfigData> EditorConfigToRunTimeConfig()
        {
            Dictionary<int, SoundConfigData> runtimeConf = new Dictionary<int, SoundConfigData>();

            foreach(EditorSoundConfigData cof in CurrentSoundConfigTable.Values)
            {
                runtimeConf.Add(cof.data.nID, cof.data);
            }
            return runtimeConf;
        }

        #region 树状图

        private const string DefaultRoot = "defaultsound";//默认路径是defaultsound
        private const string NewConfigRoot = "newconfig";

        /// <summary>
        /// 配置跟目录名
        /// </summary>
        private static Dictionary<string, TreeViewNode> ConfigRootTreeView = new Dictionary<string, TreeViewNode>();

        /// <summary>
        /// 配置第二根目录
        /// </summary>
        private static Dictionary<string, TreeViewNode> ConfigSecondRootTreeView = new Dictionary<string, TreeViewNode>();



        static void GenrateConfigTreeView()
        {
            ConfigRootTreeView.Clear();
            ConfigSecondRootTreeView.Clear();

            //在Sounds文件夹下的，直接放在default里
            TreeViewNode no = new TreeViewNode();
            no.TreeViewName = "Default";
            ConfigRootTreeView.Add(DefaultRoot, no);

            no = new TreeViewNode();
            no.TreeViewName = NewConfigRoot;
            ConfigRootTreeView.Add(NewConfigRoot, no);


            //遍历所有的配置，根据assetbundlename来分类
            foreach (EditorSoundConfigData s in CurrentSoundConfigTable.Values)
            {
                GenerateHostTreeView_Config(s);
            }
        }

        static void GenerateHostTreeView_Config(EditorSoundConfigData s)
        {
            //如果assetbundlename是空的，直接扔到NewConfig里去
            if (string.IsNullOrEmpty(s.data.AssetBundleName))
            {
                s.ParentTreeViewName = string.Empty;
                s.HostTreeViewName = NewConfigRoot;
                ConfigRootTreeView[NewConfigRoot].configList.Add(s);
                return;
            }

            
            //用于判断的节点不能有后缀
            string treepatch = s.data.AssetBundleName.Replace(".soundpkg", "");
            //去掉sound头
            treepatch = treepatch.Replace("sound/", "");

            string[] splitString = treepatch.Split('/');

            string root = splitString[0];

            //如果根节点表里有这个节点
            if (ConfigRootTreeView.ContainsKey(root))
            {
                s.ParentTreeViewName = string.Empty;
                //再去检查第二根节点
                CheckSecondRootView_Config(splitString, root, s);
            }
            else
            {
                //新建跟节点
                TreeViewNode no = new TreeViewNode();
                no.ParentTreeViewName = string.Empty;
                no.TreeViewName = root;
                ConfigRootTreeView.Add(root, no);

                //再去检查第二根节点
                CheckSecondRootView_Config(splitString, root, s);
            }
        }

        static void DeleteTreeView_Config(EditorSoundConfigData s)
        {
            //是在根节点
            if (ConfigRootTreeView.ContainsKey(s.HostTreeViewName))
            {
                ConfigRootTreeView[s.HostTreeViewName].configList.Remove(s);
            }
            else
            {
                if (ConfigSecondRootTreeView.ContainsKey(s.HostTreeViewName))
                {
                    ConfigSecondRootTreeView[s.HostTreeViewName].configList.Remove(s);
                }
            }
        }

        static void CheckSecondRootView_Config(string[] splitString, string root, EditorSoundConfigData data)
        {
            //如果长度是2，表明，只在这个跟节点里而已，直接添加进去就行
            if (splitString.Length == 1)
            {
                data.ParentTreeViewName = string.Empty;
                data.HostTreeViewName = root;
                ConfigRootTreeView[root].configList.Add(data);
            }
            else//如果不是，则看看次级节点
            {
                //第二根节点
                string secondRoot = splitString[1];

                //第二根节点已经存在,直接添加进去就行
                if (ConfigSecondRootTreeView.ContainsKey(secondRoot))
                {
                    data.HostTreeViewName = secondRoot;
                    data.ParentTreeViewName = root;
                    ConfigSecondRootTreeView[secondRoot].configList.Add(data);
                }
                else //不存在，新建一个,再添加进去
                {
                    ConfigRootTreeView[root].ChildViewList.Add(secondRoot);
                    TreeViewNode nod = new TreeViewNode();
                    nod.TreeViewName = secondRoot;
                    nod.ParentTreeViewName = root;
                    nod.configList.Add(data);

                    data.HostTreeViewName = secondRoot;
                    data.ParentTreeViewName = root;
                    ConfigSecondRootTreeView.Add(secondRoot, nod);
                }
            }
        }

        static void CloseAllTreeView()
        {
            foreach (TreeViewNode nod in ConfigRootTreeView.Values)
            {
                nod.isOpen = false;
            }

            foreach (TreeViewNode nod in ConfigSecondRootTreeView.Values)
            {
                nod.isOpen = false;
            }
        }

        static void OpenTreeView(string TreeViewName)
        {
            //如果在根节点
            if (ConfigRootTreeView.ContainsKey(TreeViewName))
            {
                ConfigRootTreeView[TreeViewName].isOpen = true;
            }

            //在子节点
            if (ConfigSecondRootTreeView.ContainsKey(TreeViewName))
            {
                //先打开父节点
                ConfigRootTreeView[ConfigSecondRootTreeView[TreeViewName].ParentTreeViewName].isOpen = true;
                
                //再打开自己
                ConfigSecondRootTreeView[TreeViewName].isOpen = true;
            }
        }
        #endregion
    }
}
