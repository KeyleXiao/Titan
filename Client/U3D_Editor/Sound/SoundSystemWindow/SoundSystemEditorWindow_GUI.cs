/*******************************************************************
** 文件名:	SoundSystemEditorWindow_GUI.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/26
** 版  本:	1.0
** 描  述:	声音系统
** 应  用:  	声音编辑器GUI相关部分

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
        Rect AudioResourceListRect = new Rect();//资源链表区域大小
        Rect ConfigViewListRect = new Rect();//配置链表区域大小
        Rect AudioConfigInfoRect = new Rect();//配资信息区域大小
        Rect AudioResInfoRect = new Rect();//资源信息区域大小
        Rect InsertResRect = new Rect();//插入资源区域大小
        Rect IORect = new Rect();//IO区域大小

        private Vector2 AudlipSourceScrollViewPos = Vector2.zero;
        private Vector2 ConfigScrollViewPos = Vector2.zero;

        private Vector2 AudlipSourceInfoScrollViewPos = Vector2.zero;
        private Vector2 ConfigInofScrollViewPos = Vector2.zero;

        private Vector2 IOButtonScrollViewPos = Vector2.zero;

        private List<AudioClipInfo> UnValidResInfo = new List<AudioClipInfo>();
        private List<EditorSoundConfigData> unValidConfigInfo = new List<EditorSoundConfigData>();


        #region 资源绘制  
        void DrawResTreeView()
        {
            UnValidResInfo.Clear();
            //绘制根节点
            foreach (TreeViewNode treeView in ResRootTreeView.Values)
            {
                treeView.isOpen = EditorGUILayout.Foldout(treeView.isOpen, treeView.TreeViewName);
                if (treeView.isOpen)//跟节点可见
                {
                    #region 子节点绘制
                    //先画子节点，这个跟节点有子节点
                    if (treeView.ChildViewList.Count > 0)
                    {
                        //遍历绘制子节点
                        foreach (string s in treeView.ChildViewList)
                        {
                            TreeViewNode secondtreeView = ResSecondRootTreeView[s];
                            GUILayout.BeginHorizontal();
                            GUILayout.Space(18);
                            secondtreeView.isOpen = EditorGUILayout.Foldout(secondtreeView.isOpen, secondtreeView.TreeViewName);
                            GUILayout.EndHorizontal();

                            if (secondtreeView.isOpen)
                            {
                                #region 绘制

                                //绘制子节点里的配置
                                foreach (AudioClipInfo clip in secondtreeView.AudioClipList)
                                {
                                    //是同一个树节点才绘制
                                    if (clip.HostTreeViewName.Equals(secondtreeView.TreeViewName))
                                    {
                                        GUILayout.BeginHorizontal();
                                        GUILayout.Space(36);

                                        if (currentSelectedClipInfo!= null &&
                                            currentSelectedClipInfo.AssetGUID.Equals(clip.AssetGUID))
                                        {
                                            Color old = GUI.color;
                                            GUI.color = Color.green;
                                            if (GUILayout.Button("音效资源:" + clip.AssetNameWithExe))
                                            {
                                                currentSelectedClipInfo = clip;
                                            }
                                            GUI.color = old;
                                        }
                                        else
                                        {
                                            if (GUILayout.Button("音效资源:" + clip.AssetNameWithExe))
                                            {
                                                currentSelectedClipInfo = clip;
                                            }
                                        }
                                        GUILayout.EndHorizontal();
                                    }
                                    else
                                    {
                                        UnValidResInfo.Add(clip);
                                    }

                                }

                                #endregion

                                //移除无用的节点
                                if (UnValidResInfo.Count > 0)
                                {
                                    foreach (AudioClipInfo clip in UnValidResInfo)
                                    {
                                        secondtreeView.AudioClipList.Remove(clip);
                                    }

                                    UnValidResInfo.Clear();
                                }

                            }
                        }
                    }
                    #endregion

                    #region 父节点绘制

                    UnValidResInfo.Clear();
                    foreach (AudioClipInfo clip in treeView.AudioClipList)
                    {
                        if (clip.HostTreeViewName.Equals(treeView.TreeViewName))
                        {
                            GUILayout.BeginHorizontal();
                            GUILayout.Space(18);
                            if (currentSelectedClipInfo != null &&
                                currentSelectedClipInfo.AssetGUID.Equals(clip.AssetGUID))
                            {
                                Color old = GUI.color;
                                GUI.color = Color.green;
                                if (GUILayout.Button("音效资源:" + clip.AssetNameWithExe))
                                {
                                    currentSelectedClipInfo = clip;
                                }
                                GUI.color = old;
                            }
                            else
                            {
                                if (GUILayout.Button("音效资源:" + clip.AssetNameWithExe))
                                {
                                    currentSelectedClipInfo = clip;
                                }
                            }
                            GUILayout.EndHorizontal();
                        }
                        else
                        {
                            UnValidResInfo.Add(clip);
                        }
                    }

                    //移除无用的信息
                    if(UnValidResInfo.Count > 0)
                    {
                        foreach (AudioClipInfo clip in UnValidResInfo)
                        {
                            treeView.AudioClipList.Remove(clip);
                        }
                        UnValidResInfo.Clear();
                    }
                   
                    #endregion
                }
                  
            }

            UnValidResInfo.Clear();
        }

        /// <summary>
        /// 绘制资源链表
        /// </summary>
        void DrawAudioResourceList()
        {
            Rect pos = position;
            
            AudioResourceListRect.x = pos.width * 0.65f;
            AudioResourceListRect.y = 0;

            AudioResourceListRect.width = pos.width * 0.35f;
            AudioResourceListRect.height = pos.height * 0.9f;

            GUILayout.BeginArea(AudioResourceListRect);
            GUILayout.BeginVertical("box");

            AudlipSourceScrollViewPos = GUILayout.BeginScrollView(AudlipSourceScrollViewPos, GUILayout.Height(AudioResourceListRect.height * 0.9f));

            DrawResTreeView();
            //foreach (AudioClipInfo clip in AudioSourceList)
            //{
            //    if(GUILayout.Button("音效资源:" + clip.AssetNameWithExe))
            //    {
            //        currentSelectedClipInfo = clip;
            //    }
            //}

            GUILayout.EndScrollView();

            DrawAudioResourceButton();

            GUILayout.EndVertical();
            GUILayout.EndArea();
        }


        /// <summary>
        /// 绘制资源按钮
        /// </summary>
        private void DrawAudioResourceButton()
        {
            GUILayout.BeginHorizontal("box");

            if (bPlayingSound)
            {
                if (GUILayout.Button("停止"))
                {
                    StopSound();
                }
            }
            else
            {
                if(currentSelectedClipInfo!= null)
                {
                    if (GUILayout.Button("播放"))
                    {
                        PlaySound(currentSelectedClipInfo.ResObj);
                    }
                }

            }


            if (GUILayout.Button("刷新资源列表"))
            {
                RefrshAudioClipSource();
            }

            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal("box");

            
            if (GUILayout.Button("打开资源目录"))
            {
                System.Diagnostics.Process.Start(Application.dataPath + "/" + "Sounds");
            }
            GUI.enabled = false;
            if (GUILayout.Button("高科技!!"))
            {
                
                
            }
            GUI.enabled = true;
            GUILayout.EndHorizontal();
        }

        /// <summary>
        /// 绘制资源信息
        /// </summary>
        private void DrawAudioClipInfo()
        {
            Rect pos = position;

            AudioResInfoRect.x = ConfigViewListRect.width;
            AudioResInfoRect.y = AudioResourceListRect.height * 0.67f;

            AudioResInfoRect.width = pos.width - ConfigViewListRect.width - AudioResourceListRect.width;
            AudioResInfoRect.height = AudioResourceListRect.height * 0.27f;

            GUI.Box(AudioResInfoRect, "");
            GUILayout.BeginArea(AudioResInfoRect);
            AudlipSourceInfoScrollViewPos = GUILayout.BeginScrollView(AudlipSourceInfoScrollViewPos, GUILayout.Height(AudioResInfoRect.height));
            GUILayout.BeginVertical("box");

            if (currentSelectedClipInfo == null)
            {
                GUILayout.Label("无可用资源信息..");
            }
            else
            {


                GUILayout.Label("资源信息:");

                GUILayout.Space(5.0f);

                GUILayout.Label("资源时间:" + currentSelectedClipInfo.AudioLength.ToString());

                GUILayout.Label("所属资源包:" + currentSelectedClipInfo.AssetBundleName);
                GUILayout.Label("资源名:" + currentSelectedClipInfo.AssetName);
                GUILayout.Label("资源GUID:" + currentSelectedClipInfo.AssetGUID);

                //GUILayout.Label("绝对路径:" + currentSelectedClipInfo.AbsolutePatch);
                GUILayout.Label("相对路径:" + currentSelectedClipInfo.RelativePatch);

            }

            GUILayout.EndVertical();
            GUILayout.EndScrollView();
            GUILayout.EndArea();
        }

        #endregion

        
        #region 配置绘制

        void DrawConfigTreeView()
        {
            unValidConfigInfo.Clear();
           //绘制根节点
           foreach (TreeViewNode treeView in ConfigRootTreeView.Values)
           {
               treeView.isOpen = EditorGUILayout.Foldout(treeView.isOpen, treeView.TreeViewName);
               if (treeView.isOpen)//跟节点可见
               {
                   #region 绘制子节点
                   //先画子节点，这个跟节点有子节点
                   if (treeView.ChildViewList.Count > 0)
                   {
                       
                       //遍历绘制子节点
                       foreach (string s in treeView.ChildViewList)
                       {
                           TreeViewNode secondtreeView = ConfigSecondRootTreeView[s];
                           GUILayout.BeginHorizontal();
                           GUILayout.Space(18);
                           secondtreeView.isOpen = EditorGUILayout.Foldout(secondtreeView.isOpen, secondtreeView.TreeViewName);
                           GUILayout.EndHorizontal();

                           if (secondtreeView.isOpen)
                           {
                               //绘制子节点里的配置
                               foreach (EditorSoundConfigData cof in secondtreeView.configList)
                               {
                                   //同一个父节点的才绘制
                                   if (cof.HostTreeViewName.Equals(secondtreeView.TreeViewName))
                                   {
                                       GUILayout.BeginHorizontal();
                                       GUILayout.Space(36);
                                       if (currentSelectedConfig != null &&
                                           currentSelectedConfig.data.nID.Equals(cof.data.nID))
                                       {
                                           Color old = GUI.color;
                                           GUI.color = Color.green;
                                           if (GUILayout.Button("音效配置ID:" + cof.data.nID.ToString()))
                                           {
                                               currentSelectedConfig = cof;
                                           }
                                           GUI.color = old;
                                       }
                                       else
                                       {
                                           if (GUILayout.Button("音效配置ID:" + cof.data.nID.ToString()))
                                           {
                                               currentSelectedConfig = cof;
                                           }
                                       }

                                       GUILayout.EndHorizontal();
                                   }
                                   else
                                   {
                                       unValidConfigInfo.Add(cof);
                                   }
                               }

                               if (unValidConfigInfo.Count > 0)
                               {
                                   foreach (EditorSoundConfigData cof in unValidConfigInfo)
                                   {
                                       secondtreeView.configList.Remove(cof);
                                   }
                                   unValidConfigInfo.Clear();
                               }
                           }

                       }

                   }
                   #endregion

                   #region 绘制父节点

                   unValidConfigInfo.Clear();

                   foreach (EditorSoundConfigData cof in treeView.configList)
                   {
                       //同一个树节点绘制
                       if (cof.HostTreeViewName.Equals(treeView.TreeViewName))
                       {
                           GUILayout.BeginHorizontal();
                           GUILayout.Space(18);
                           if (currentSelectedConfig != null &&
                               currentSelectedConfig.data.nID.Equals(cof.data.nID))
                           {
                               Color old = GUI.color;
                               GUI.color = Color.green;
                               if (GUILayout.Button("音效配置ID:" + cof.data.nID.ToString()))
                               {
                                   currentSelectedConfig = cof;
                               }
                               GUI.color = old;
                           }
                           else
                           {
                               if (GUILayout.Button("音效配置ID:" + cof.data.nID.ToString()))
                               {
                                   currentSelectedConfig = cof;
                               }
                           }
                           GUILayout.EndHorizontal();
                       }
                       else
                       {
                           unValidConfigInfo.Add(cof);
                       }
                   }


                   if (unValidConfigInfo.Count > 0)
                   {
                       foreach (EditorSoundConfigData cof in unValidConfigInfo)
                       {
                           treeView.configList.Remove(cof);
                       }
                       unValidConfigInfo.Clear();
                   }

                   #endregion

               }
           }
       }

        /// <summary>
        /// 绘制配置链表
        /// </summary>
        void DrawConfigList()
        {
            Rect pos = position;
            
            ConfigViewListRect.x = 0;
            ConfigViewListRect.y = 0;

            ConfigViewListRect.width = pos.width * 0.35f;
            ConfigViewListRect.height = pos.height * 0.9f;

            GUILayout.BeginArea(ConfigViewListRect);
            GUILayout.BeginVertical("box");

            ConfigScrollViewPos = GUILayout.BeginScrollView(ConfigScrollViewPos, GUILayout.Height(ConfigViewListRect.height * 0.9f));

            DrawConfigTreeView();

            //foreach (SoundConfigData cof in CurrentSoundConfigTable.Values)
            //{
                
            //    if (GUILayout.Button("音效配置ID:" + cof.nID.ToString()))
            //    {
            //        currentSelectedConfig = cof;
                    
            //    }
                
            //}
            
            EditorGUILayout.EndFadeGroup();
            GUILayout.EndScrollView();

            DrawConfigButton();

            
            GUILayout.EndVertical();
            GUILayout.EndArea();
        }


        /// <summary>
        /// 绘制配置区域按钮
        /// </summary>
        private void DrawConfigButton()
        {
            GUILayout.BeginHorizontal("box");
            if (GUILayout.Button("新增配置"))
            {
                CreateNewConfig();
            }

            if (GUILayout.Button("删除当前配置"))
            {
                DeleteCurrentConfig();
            }
            GUILayout.EndHorizontal();

            GUILayout.BeginHorizontal("box");

            GUI.enabled = false;
            if (GUILayout.Button("高科技!!"))
            {

            }

            if (GUILayout.Button("高科技!!"))
            {

            }
            GUI.enabled = true;

            GUILayout.EndHorizontal();
        }


        /// <summary>
        /// 绘制配置信息
        /// </summary>
        private void DrawConfigInfo()
        {
            Rect pos = position;
            
            AudioConfigInfoRect.x = ConfigViewListRect.width;
            AudioConfigInfoRect.y = 0;

            AudioConfigInfoRect.width = pos.width - ConfigViewListRect.width - AudioResourceListRect.width;
            AudioConfigInfoRect.height = AudioResourceListRect.height * 0.67f;

            GUI.Box(AudioConfigInfoRect, "");
            GUILayout.BeginArea(AudioConfigInfoRect);
            ConfigInofScrollViewPos = GUILayout.BeginScrollView(ConfigInofScrollViewPos, GUILayout.Height(AudioConfigInfoRect.height));
            GUILayout.BeginVertical("box");

            if (currentSelectedConfig == null)
            {
                GUILayout.Label("无可用配置信息..");
            }
            else
            {
                

                GUILayout.Label("配置信息:");

                GUILayout.BeginHorizontal("box");
                currentSelectedConfig.data.nID = EditorGUILayout.IntField("ID:" , currentSelectedConfig.data.nID);
                GUILayout.EndHorizontal();

                currentSelectedConfig.data.soundType = (EMSoundType)EditorGUILayout.EnumPopup("声音类型:", currentSelectedConfig.data.soundType);
                currentSelectedConfig.data.mixerGroupType = (AudioMixerGroupType)EditorGUILayout.EnumPopup("混音组类型:", currentSelectedConfig.data.mixerGroupType);
                currentSelectedConfig.data.nMixerGroupIndex = EditorGUILayout.IntField("混音组序号:", currentSelectedConfig.data.nMixerGroupIndex);

                GUI.changed = false;

                currentSelectedConfig.data.bBypassEffect = EditorGUILayout.Toggle("Bypass Effects:", currentSelectedConfig.data.bBypassEffect);
                currentSelectedConfig.data.bBypassReverbZone = EditorGUILayout.Toggle("Bypass Reverb Zone:", currentSelectedConfig.data.bBypassReverbZone);
                currentSelectedConfig.data.bPlayOnAwake = EditorGUILayout.Toggle("Play On Awake:", currentSelectedConfig.data.bPlayOnAwake);
                currentSelectedConfig.data.nPriority = EditorGUILayout.IntField("Priority:", currentSelectedConfig.data.nPriority);
                currentSelectedConfig.data.fPitch = EditorGUILayout.FloatField("Pitch:", currentSelectedConfig.data.fPitch);
                currentSelectedConfig.data.fStereoPan = EditorGUILayout.FloatField("Stereo Pan:", currentSelectedConfig.data.fStereoPan);
                currentSelectedConfig.data.fReverZoneMix = EditorGUILayout.FloatField("Rever Zone Mix:", currentSelectedConfig.data.fReverZoneMix);
                currentSelectedConfig.data.b3DSound = EditorGUILayout.Toggle("是否3D音效:", currentSelectedConfig.data.b3DSound);

                if (currentSelectedConfig.data.b3DSound)
                {
                    currentSelectedConfig.data.fDopplerLv = EditorGUILayout.FloatField("Doppler Level:", currentSelectedConfig.data.fDopplerLv);
                    currentSelectedConfig.data.nSpread = EditorGUILayout.IntField("Spread:", currentSelectedConfig.data.nSpread);
                    currentSelectedConfig.data.audioRoffType = (AudioRolloffMode)EditorGUILayout.EnumPopup("Volume Rolloff:", currentSelectedConfig.data.audioRoffType);
                    currentSelectedConfig.data.fMinDistance = EditorGUILayout.FloatField("Min Distance:", currentSelectedConfig.data.fMinDistance);
                    currentSelectedConfig.data.fMaxDistance = EditorGUILayout.FloatField("Max Distance:", currentSelectedConfig.data.fMaxDistance);
                }

                currentSelectedConfig.data.nPlayTimes = EditorGUILayout.IntField("播放次数(≤0循环):", currentSelectedConfig.data.nPlayTimes);
                currentSelectedConfig.data.fDuartion = EditorGUILayout.FloatField("持续时间(≤0无限):", currentSelectedConfig.data.fDuartion);
                currentSelectedConfig.data.fDelayPlayTime = EditorGUILayout.FloatField("延迟时间(≤0.03无延迟):", currentSelectedConfig.data.fDelayPlayTime);   
                currentSelectedConfig.data.fIntervalBetweenPlayTimes = EditorGUILayout.FloatField("两次之间播放间隔(≤0无间隔):", currentSelectedConfig.data.fIntervalBetweenPlayTimes);

                if (GUI.changed)
                {
                   // CurrentSoundConfigTable[currentSelectedConfig.data.nID] = currentSelectedConfig;
                }

                GUILayout.BeginVertical("box");
                GUILayout.Label("依赖预制体:");
                currentSelectedConfig.data.dependAsset = (GameObject)EditorGUILayout.ObjectField(currentSelectedConfig.data.dependAsset, typeof(Object), true);
                GUILayout.EndVertical();

                AssetImporter asp = AssetImporter.GetAtPath(AssetDatabase.GetAssetPath(currentSelectedConfig.data.dependAsset));
                if (asp)
                {
                    string szGUID = AssetBundleManagerEditor.GetAssetGUIDFromMeta(asp.assetPath);
                    if (currentSelectedConfig.data.szDependAssetGUID == string.Empty || (currentSelectedConfig.data.szDependAssetGUID != string.Empty && szGUID != string.Empty))
                    {
                        currentSelectedConfig.data.szDependAssetGUID = szGUID;
                    }
                }

                GUI.enabled = false;
                currentSelectedConfig.data.szDependAssetGUID = EditorGUILayout.TextField("资源所在的资源包:", currentSelectedConfig.data.szDependAssetGUID);
                GUI.enabled = true;

                GUILayout.Space(5.0f);

                GUILayout.BeginVertical("box");
                GUILayout.Label("资源所在的资源包:" + currentSelectedConfig.data.AssetBundleName);
                GUILayout.Label("资源名:" + currentSelectedConfig.data.AssetName);
                GUILayout.Label("资源GUID:" + currentSelectedConfig.data.AssetGUID);
                GUILayout.EndVertical();
            }

            GUILayout.EndVertical();
            GUILayout.EndScrollView();
            GUILayout.EndArea();
        }

        #endregion

        /// <summary>
        /// 绘制重要的按钮，目前有插入资源到配置，更新配置到游戏,匹配资源
        /// </summary>
        private void DrawImportanButton()
        {
            Rect pos = position;

            InsertResRect.x = ConfigViewListRect.width;
            InsertResRect.y = AudioResourceListRect.height * 0.94f;

            InsertResRect.width = AudioResInfoRect.width;
            InsertResRect.height = AudioResourceListRect.height * 0.06f;

            GUILayout.BeginArea(InsertResRect);

            GUILayout.BeginHorizontal("box");
            Color oldColod = GUI.color;
            GUI.color = Color.red;
            if (GUILayout.Button("插入资源到配置"))
            {
                InsertSoureToConfig();
            }

            if (GUILayout.Button("刷新配置到游戏"))
            {
                if (Application.isPlaying)
                {
                    SoundManager.Clear();
                    SoundManager.InitFromSystemEditor(EditorConfigToRunTimeConfig());
                }

            }

            if (GUILayout.Button("匹配资源"))
            {
                AdjustSourceAndConfig();
            }
            GUILayout.EndHorizontal();



            GUI.color = oldColod;

            GUILayout.EndArea();
        }


        /// <summary>
        /// 绘制输入输出按钮
        /// </summary>
        private void DrawIOButton()
        {
            Rect pos = position;

            IORect.x = 0;
            IORect.y = ConfigViewListRect.height;

            IORect.width = pos.width;
            IORect.height = pos.height * 0.1f;

            GUILayout.BeginArea(IORect);
            GUILayout.Space(18);

            GUILayout.BeginHorizontal("box");

            if (GUILayout.Button("加载配置"))
            {
                UnLoadSoundConfig();
                LoadSoundConfig();
            }

            if(GUILayout.Button("导出配置"))
            {
                ExportConfig();
            }

            if (GUILayout.Button("帮助"))
            {
                SoundSystemEditorWindowHelpWindow.OpenSoundSystemEditorWindowHelpWindow();
            }

            GUI.enabled = false;

            if (GUILayout.Button("高科技"))
            {
                //if (!Application.isPlaying)
                //{
                //    //BuildAssetBundle();
                //}
                //else
                //{
                //    SoundManager.SoundSystemLog("不能在播放模式下构建资源包!");
                //}
            }
            GUI.enabled = true;

            GUILayout.EndHorizontal();

            GUILayout.EndArea();
        }
    }
}
