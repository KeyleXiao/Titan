#if UNITY_5 && !UNITY_5_0 && !UNITY_5_1 && !UNITY_5_2 && !UNITY_5_3_0
#define AVPRO_UNITY_PLATFORM_TVOS
#endif
#if UNITY_5 && !UNITY_5_0 && !UNITY_5_1 && !UNITY_5_2_0
#define AVPRO_UNITY_IOS_ALLOWHTTPDOWNLOAD
#endif
#if !UNITY_5
#define AVPRO_UNITY_METRO
#endif
#if UNITY_5 &&  !UNITY_5_0 && !UNITY_5_1 && !UNITY_5_2 && !UNITY_5_3_0
#define AVPRO_UNITY_WP8_DEPRECATED
#endif
using UnityEngine;
using UnityEditor;
using System.Collections.Generic;

//-----------------------------------------------------------------------------
// Copyright 2015-2016 RenderHeads Ltd.  All rights reserverd.
//-----------------------------------------------------------------------------

namespace RenderHeads.Media.AVProVideo.Editor
{
    [CanEditMultipleObjects]
    [CustomEditor(typeof(MediaPlayer))]
    public class MediaPlayerEditor : UnityEditor.Editor
    {
        private SerializedProperty _propLocation;
        private SerializedProperty _propPath;
        private SerializedProperty _propAutoOpen;
        private SerializedProperty _propAutoStart;
        private SerializedProperty _propLoop;
        private SerializedProperty _propRate;
        private SerializedProperty _propVolume;
        private SerializedProperty _propMuted;
        private SerializedProperty _propPersistent;
        private SerializedProperty _propDebugGui;
        private SerializedProperty _propEvents;
        private SerializedProperty _propFilter;
        private SerializedProperty _propWrap;
        private SerializedProperty _propAniso;
        private SerializedProperty _propStereoPacking;
        private SerializedProperty _propAlphaPacking;
        private SerializedProperty _propDisplayStereoTint;

        private static bool _isTrialVersion = false;
        private static Texture2D _icon;
        private static int _platformIndex = -1;
        private static bool _expandPlatformOverrides = false;
        private static bool _expandMediaProperties = false;
        private static bool _expandGlobalSettings = false;
        private static bool _expandEvents = false;
        private static bool _expandPreview = false;
        private static bool _expandAbout = false;
        private static List<string> _recentFiles = new List<string>(16);

        private const string SettingsPrefix = "AVProVideo-MediaPlayerEditor-";
        private const int MaxRecentFiles = 16;

        private const string LinkPluginWebsite = "http://renderheads.com/product/avpro-video/";
        private const string LinkForumPage = "http://forum.unity3d.com/threads/released-avpro-video-complete-video-playback-solution.385611/";
        private const string LinkAssetStorePage = "https://www.assetstore.unity3d.com/#!/content/56355";
        private const string LinkEmailSupport = "mailto:unitysupport@renderheads.com";
        private const string LinkUserManual = "http://downloads.renderheads.com/docs/UnityAVProVideo.pdf";
        private const string LinkScriptingClassReference = "http://www.renderheads.com/content/docs/AVProVideoClassReference/";

        private const string SupportMessage = "If you are reporting a bug, please include any relevant files and details so that we may remedy the problem as fast as possible.\n\n" +
            "Essential details:\n" +
            "+ Error message\n" +
            "      + The exact error message\n" +
            "      + The console/output log if possible\n" +
            "+ Hardware\n" +
            "      + Phone / tablet / device type and OS version\n" +
            "+ Development environment\n" +
            "      + Unity version\n" +
            "      + Development OS version\n" +
            "      + AVPro Video plugin version\n" +
            " + Video details\n" +
            "      + Resolution\n" +
            "      + Codec\n" +
            "      + Frame Rate\n" +
            "      + Better still, include a link to the video file\n";

        private static bool _showAlpha = false;
        private static string[] _platformNames;

        [MenuItem("GameObject/AVPro Video/Media Player", false, 0)]
        public static void CreateMediaPlayerEditor()
        {
            GameObject go = new GameObject("MediaPlayer");
            go.AddComponent<MediaPlayer>();
            Selection.activeGameObject = go;
        }

#if UNITY_5
        [MenuItem("GameObject/AVPro Video/Media Player with Unity Audio", false, 0)]
        public static void CreateMediaPlayerWidthUnityAudioEditor()
        {
            GameObject go = new GameObject("MediaPlayer");
            go.AddComponent<MediaPlayer>();
            go.AddComponent<AudioSource>();
            AudioOutput ao = go.AddComponent<AudioOutput>();
            // Move the AudioOutput component above the AudioSource so that it acts as the audio generator
            UnityEditorInternal.ComponentUtility.MoveComponentUp(ao);
            Selection.activeGameObject = go;
        }
#endif

        private static void LoadSettings()
        {
            _expandPlatformOverrides = EditorPrefs.GetBool(SettingsPrefix + "ExpandPlatformOverrides", false);
            _expandMediaProperties = EditorPrefs.GetBool(SettingsPrefix + "ExpandMediaProperties", false);
            _expandGlobalSettings = EditorPrefs.GetBool(SettingsPrefix + "ExpandGlobalSettings", false);
            _expandEvents = EditorPrefs.GetBool(SettingsPrefix + "ExpandEvents", false);
            _expandPreview = EditorPrefs.GetBool(SettingsPrefix + "ExpandPreview", false);
            _platformIndex = EditorPrefs.GetInt(SettingsPrefix + "PlatformIndex", -1);
            _showAlpha = EditorPrefs.GetBool(SettingsPrefix + "ShowAlphaChannel", false);

            string recentFilesString = EditorPrefs.GetString(SettingsPrefix + "RecentFiles", string.Empty);
            _recentFiles = new List<string>(recentFilesString.Split(new string[] { ";" }, System.StringSplitOptions.RemoveEmptyEntries));
        }

        private static void SaveSettings()
        {
            EditorPrefs.SetBool(SettingsPrefix + "ExpandPlatformOverrides", _expandPlatformOverrides);
            EditorPrefs.SetBool(SettingsPrefix + "ExpandMediaProperties", _expandMediaProperties);
            EditorPrefs.SetBool(SettingsPrefix + "ExpandGlobalSettings", _expandGlobalSettings);
            EditorPrefs.SetBool(SettingsPrefix + "ExpandEvents", _expandEvents);
            EditorPrefs.SetBool(SettingsPrefix + "ExpandPreview", _expandPreview);
            EditorPrefs.SetInt(SettingsPrefix + "PlatformIndex", _platformIndex);
            EditorPrefs.SetBool(SettingsPrefix + "ShowAlphaChannel", _showAlpha);

            string recentFilesString = string.Empty;
            if (_recentFiles.Count > 0)
            {
                recentFilesString = string.Join(";", _recentFiles.ToArray());
            }
            EditorPrefs.SetString(SettingsPrefix + "RecentFiles", recentFilesString);
        }

        private bool IsTrialVersion()
        {
            bool result = false;

            string version = string.Empty;
#if UNITY_EDITOR_WIN
            version = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(Native.GetPluginVersion());
#elif UNITY_EDITOR_OSX
            version = Native.AVPGetVersion();
#endif
            result = version.Contains("t");

            return result;
        }

        void OnEnable()
        {
            LoadSettings();

            _isTrialVersion = IsTrialVersion();
            _platformNames = Helper.GetPlatformNames();

            _propLocation = serializedObject.FindProperty("m_VideoLocation");
            _propPath = serializedObject.FindProperty("m_VideoPath");
            _propAutoOpen = serializedObject.FindProperty("m_AutoOpen");
            _propAutoStart = serializedObject.FindProperty("m_AutoStart");
            _propLoop = serializedObject.FindProperty("m_Loop");
            _propRate = serializedObject.FindProperty("m_PlaybackRate");
            _propVolume = serializedObject.FindProperty("m_Volume");
            _propMuted = serializedObject.FindProperty("m_Muted");
            _propPersistent = serializedObject.FindProperty("m_Persistent");
            _propDebugGui = serializedObject.FindProperty("m_DebugGui");
            _propEvents = serializedObject.FindProperty("m_events");
            _propFilter = serializedObject.FindProperty("m_FilterMode");
            _propWrap = serializedObject.FindProperty("m_WrapMode");
            _propAniso = serializedObject.FindProperty("m_AnisoLevel");
            _propStereoPacking = serializedObject.FindProperty("m_StereoPacking");
            _propAlphaPacking = serializedObject.FindProperty("m_AlphaPacking");
            _propDisplayStereoTint = serializedObject.FindProperty("m_DisplayDebugStereoColorTint");
        }

        void OnDisable()
        {
            SaveSettings();
        }

        private static bool IsPathWithin(string fullPath, string targetPath)
        {
            return fullPath.StartsWith(targetPath);
        }

        private static string GetPathRelativeTo(string root, string fullPath)
        {
            string result = fullPath.Remove(0, root.Length);
            if (result.StartsWith(System.IO.Path.DirectorySeparatorChar.ToString()) || result.StartsWith(System.IO.Path.AltDirectorySeparatorChar.ToString()))
            {
                result = result.Remove(0, 1);
            }
            return result;
        }

        public override bool RequiresConstantRepaint()
        {
            MediaPlayer media = (this.target) as MediaPlayer;
            return (_expandPreview && media != null && media.Control != null && media.isActiveAndEnabled);
        }

        public override void OnInspectorGUI()
        {
            MediaPlayer media = (this.target) as MediaPlayer;

            serializedObject.Update();

            if (_propLocation == null)
                return;

            GUILayout.Space(6f);

            // Describe the watermark for trial version
            if (_isTrialVersion && Application.isPlaying)
            {
                string message = string.Empty;
#if UNITY_EDITOR_WIN
                message = "The watermark is the horizontal bar that moves vertically and the small 'AVPRO TRIAL' text.";
                if (media.Info.GetPlayerDescription().Contains("MF-MediaEngine-Hardware"))
                {
                    message = "The watermark is the RenderHeads logo that moves around the image.";
                }
#elif UNITY_EDITOR_OSX
                message = "The RenderHeads logo is the watermark.";
#endif

                GUI.backgroundColor = Color.yellow;
                EditorGUILayout.BeginVertical(GUI.skin.box);
                GUI.color = Color.yellow;
                GUILayout.Label("AVPRO VIDEO - TRIAL WATERMARK", EditorStyles.boldLabel);
                GUI.color = Color.white;
                GUILayout.Label(message, EditorStyles.wordWrappedLabel);
                EditorGUILayout.EndVertical();
                GUI.backgroundColor = Color.white;
                GUI.color = Color.white;
            }

            // Warning about linear colour space with GPU decoding
            /*if (Application.isPlaying && media.Control != null)
            {
                if (QualitySettings.activeColorSpace == ColorSpace.Linear && media.Info.GetPlayerDescription().Contains("MF-MediaEngine-Hardware"))
                {
                    GUI.backgroundColor = Color.magenta;
                    EditorGUILayout.BeginVertical("box");
                    GUILayout.Label("NOTE", EditorStyles.boldLabel);
                    GUILayout.Label("You're using the GPU video decoder with linear color-space set in Unity.  This can cause videos to become washed out due to our GPU decoder path not supporting sRGB textures.\n\nThis can be fixed easily by:\n1) Switching back to Gamma colour space in Player Settings\n2) Disabling hardware decoding\n3) Adding 'col.rgb = pow(col.rgb, 2.2);' to any shader rendering the video texture.\n\nIf you're using the InsideSphere shader, make sure to tick 'Apply Gamma' on the material.", EditorStyles.wordWrappedLabel);
                    EditorGUILayout.EndVertical();
                    GUI.backgroundColor = Color.white;
                }
            }*/

            /////////////////// FILE PATH

            EditorGUILayout.BeginVertical("box");

            OnInspectorGUI_CopyableFilename(media.m_VideoPath);

            EditorGUILayout.LabelField("Source Path", EditorStyles.boldLabel);

            EditorGUILayout.PropertyField(_propLocation, GUIContent.none);

            {
                string oldPath = _propPath.stringValue;
                string newPath = EditorGUILayout.TextField(string.Empty, _propPath.stringValue);
                if (newPath != oldPath)
                {
                    // Check for invalid characters
                    if (0 > newPath.IndexOfAny(System.IO.Path.GetInvalidPathChars()))
                    {
                        _propPath.stringValue = newPath.Replace("\\", "/");
                        EditorUtility.SetDirty(target);
                    }
                }
            }

            //if (!Application.isPlaying)
            {
                GUILayout.BeginHorizontal();
                OnInspectorGUI_RecentButton(_propPath, _propLocation);
                OnInspectorGUI_StreamingAssetsButton(_propPath, _propLocation);
                GUI.color = Color.green;
                if (GUILayout.Button("BROWSE"))
                {
                    string startFolder = GetStartFolder(_propPath.stringValue, (MediaPlayer.FileLocation)_propLocation.enumValueIndex);
                    string videoPath = media.m_VideoPath;
                    string fullPath = string.Empty;
                    MediaPlayer.FileLocation fileLocation = media.m_VideoLocation;
                    if (Browse(startFolder, ref videoPath, ref fileLocation, ref fullPath))
                    {
                        _propPath.stringValue = videoPath.Replace("\\", "/");
                        _propLocation.enumValueIndex = (int)fileLocation;
                        EditorUtility.SetDirty(target);

                        AddToRecentFiles(fullPath);
                    }
                }
                GUI.color = Color.white;
                GUILayout.EndHorizontal();

                ShowFileWarningMessages(_propPath.stringValue, (MediaPlayer.FileLocation)_propLocation.enumValueIndex, media.m_AutoOpen, Platform.Unknown);
                GUI.color = Color.white;
            }

            if (Application.isPlaying)
            {
                if (GUILayout.Button("Load"))
                {
                    media.OpenVideoFromFile(media.m_VideoLocation, media.m_VideoPath, media.m_AutoStart);
                }
            }



            EditorGUILayout.EndVertical();

            /////////////////// STARTUP FIELDS

            EditorGUILayout.BeginVertical("box");
            GUILayout.Label("Startup", EditorStyles.boldLabel);
            EditorGUILayout.PropertyField(_propAutoOpen);
            EditorGUILayout.PropertyField(_propAutoStart);
            EditorGUILayout.EndVertical();

            /////////////////// PLAYBACK FIELDS

            EditorGUILayout.BeginVertical("box");
            GUILayout.Label("Playback", EditorStyles.boldLabel);

            if (!Application.isPlaying)
            {
                EditorGUILayout.PropertyField(_propLoop);
                EditorGUILayout.PropertyField(_propRate);
                EditorGUILayout.PropertyField(_propVolume);
                EditorGUILayout.PropertyField(_propMuted);
            }
            else
            {
                if (media.Control != null)
                {
                    media.m_Loop = media.Control.IsLooping();
                    bool newLooping = EditorGUILayout.Toggle("Loop", media.m_Loop);
                    if (newLooping != media.m_Loop)
                    {
                        media.Control.SetLooping(newLooping);
                    }

                    media.m_PlaybackRate = media.Control.GetPlaybackRate();
                    float newPlaybackRate = EditorGUILayout.Slider("Rate", media.m_PlaybackRate, -4f, 4f);
                    if (newPlaybackRate != media.m_PlaybackRate)
                    {
                        media.Control.SetPlaybackRate(newPlaybackRate);
                    }

                    media.m_Volume = media.Control.GetVolume();
                    float newVolume = EditorGUILayout.Slider("Volume", media.m_Volume, 0f, 1f);
                    if (newVolume != media.m_Volume)
                    {
                        media.Control.SetVolume(newVolume);
                    }

                    media.m_Muted = media.Control.IsMuted();
                    bool newMuted = EditorGUILayout.Toggle("Muted", media.m_Muted);
                    if (newMuted != media.m_Muted)
                    {
                        media.Control.MuteAudio(newMuted);
                    }
                    /*
                                        int selectedTrackIndex = media.Control.GetCurrentAudioTrack();
                                        int numTracks = media.Info.GetAudioTrackCount();
                                        if (numTracks > 0)
                                        {
                                            string[] trackNames = new string[numTracks];
                                            for (int i = 0; i < numTracks; i++)
                                            {
                                                trackNames[i] = (i+1).ToString();
                                            }
                                            int newSelectedTrackIndex = EditorGUILayout.Popup("Audio Track", selectedTrackIndex, trackNames);
                                            if (newSelectedTrackIndex != selectedTrackIndex)
                                            {
                                                media.Control.SetAudioTrack(newSelectedTrackIndex);
                                            }
                                        }*/
                }
            }

            EditorGUILayout.EndVertical();


            EditorGUILayout.BeginVertical("box");
            GUILayout.Label("Other", EditorStyles.boldLabel);
            EditorGUILayout.PropertyField(_propPersistent, new GUIContent("Persistent", "Use DontDestroyOnLoad so this object isn't destroyed between level loads"));
            EditorGUILayout.PropertyField(_propDebugGui);
            EditorGUILayout.EndVertical();


            /////////////////// MEDIA PROPERTIES

            if (!Application.isPlaying)
            {
                OnInspectorGUI_MediaProperties();
                OnInspectorGUI_GlobalSettings();
            }

            //////////////////// PREVIEW

            OnInspectorGUI_Preview();

            /////////////////// EVENTS

            OnInspectorGUI_Events();

            /////////////////// PLATFORM OVERRIDES

            //if (!Application.isPlaying)
            {
                OnInspectorGUI_PlatformOverrides();
            }

            if (serializedObject.ApplyModifiedProperties())
            {
                EditorUtility.SetDirty(target);
            }

            if (!Application.isPlaying)
            {
                OnInspectorGUI_About();
            }
        }

        struct RecentFileData
        {
            public RecentFileData(string path, SerializedProperty propPath, SerializedProperty propLocation, Object target)
            {
                this.path = path;
                this.propPath = propPath;
                this.propLocation = propLocation;
                this.target = target;
            }

            public string path;
            public SerializedProperty propPath;
            public SerializedProperty propLocation;
            public Object target;
        }

        private static void AddToRecentFiles(string path)
        {
            if (!_recentFiles.Contains(path))
            {
                _recentFiles.Insert(0, path);
                if (_recentFiles.Count > MaxRecentFiles)
                {
                    // Remove the oldest item from the list
                    _recentFiles.RemoveAt(_recentFiles.Count - 1);
                }
            }
            else
            {
                // If it already contains the item, then move it to the top
                _recentFiles.Remove(path);
                _recentFiles.Insert(0, path);
            }
        }

        void RecentMenuCallback_Select(object obj)
        {
            RecentFileData data = (RecentFileData)obj;

            string videoPath = string.Empty;
            MediaPlayer.FileLocation fileLocation = MediaPlayer.FileLocation.RelativeToStreamingAssetsFolder;
            GetRelativeLocationFromPath(data.path, ref videoPath, ref fileLocation);

            // Move it to the top of the list
            AddToRecentFiles(data.path);

            data.propPath.stringValue = videoPath.Replace("\\", "/");
            data.propLocation.enumValueIndex = (int)fileLocation;

            serializedObject.ApplyModifiedProperties();
            EditorUtility.SetDirty(data.target);
        }

        private void RecentMenuCallback_Clear()
        {
            _recentFiles.Clear();
        }

        private void RecentMenuCallback_ClearMissing()
        {
            if (_recentFiles != null && _recentFiles.Count > 0)
            {
                List<string> newList = new List<string>(_recentFiles.Count);
                for (int i = 0; i < _recentFiles.Count; i++)
                {
                    string path = _recentFiles[i];
                    if (System.IO.File.Exists(path))
                    {
                        newList.Add(path);
                    }
                }
                _recentFiles = newList;
            }
        }

        private void RecentMenuCallback_Add()
        {
            // TODO: implement me
        }

        private void OnInspectorGUI_CopyableFilename(string path)
        {
            // Display the file name so it's easy to read and copy to the clipboard
            if (!string.IsNullOrEmpty(path) && 0 > path.IndexOfAny(System.IO.Path.GetInvalidPathChars()))
            {
                // Some GUI hacks here because SelectableLabel wants to be double height and it doesn't want to be centered because it's an EditorGUILayout function...
                GUI.backgroundColor = new Color(0.8f, 0.8f, 0.8f, 0.1f);
                if (EditorGUIUtility.isProSkin)
                {
                    GUI.backgroundColor = Color.black;
                    GUI.color = Color.cyan;
                }
                GUILayout.BeginHorizontal("box");
                GUILayout.FlexibleSpace();
                string text = System.IO.Path.GetFileName(path);
                Vector2 textSize = EditorStyles.boldLabel.CalcSize(new GUIContent(text));

                EditorGUILayout.SelectableLabel(text, EditorStyles.boldLabel, GUILayout.Height(textSize.y), GUILayout.Width(textSize.x));
                GUILayout.FlexibleSpace();
                GUILayout.EndHorizontal();
                GUI.color = Color.white;
                GUI.backgroundColor = Color.white;
            }
        }

        private void OnInspectorGUI_RecentButton(SerializedProperty propPath, SerializedProperty propLocation)
        {
            GUI.color = Color.white;

            if (GUILayout.Button("RECENT", GUILayout.Width(60f)))
            {
                GenericMenu toolsMenu = new GenericMenu();
                toolsMenu.AddDisabledItem(new GUIContent("Recent Files:"));

                // TODO: allow current path to be added.  Perhaps add it automatically when file is loaded?
                /*if (!string.IsNullOrEmpty(propPath.stringValue))
                {
                    string path = propPath.stringValue.Replace("/", ">").Replace("\\", ">");
                    toolsMenu.AddItem(new GUIContent("Add Current: " + path), false, RecentMenuCallback_Add);
                }*/
                toolsMenu.AddSeparator("");

                int missingCount = 0;
                for (int i = 0; i < _recentFiles.Count; i++)
                {
                    string path = _recentFiles[i];
                    string itemName = path.Replace("/", ">").Replace("\\", ">");
                    if (System.IO.File.Exists(path))
                    {
                        toolsMenu.AddItem(new GUIContent(itemName), false, RecentMenuCallback_Select, new RecentFileData(path, propPath, propLocation, target));
                    }
                    else
                    {
                        toolsMenu.AddDisabledItem(new GUIContent(itemName));
                        missingCount++;
                    }
                }
                if (_recentFiles.Count > 0)
                {
                    toolsMenu.AddSeparator("");
                    toolsMenu.AddItem(new GUIContent("Clear"), false, RecentMenuCallback_Clear);
                    if (missingCount > 0)
                    {
                        toolsMenu.AddItem(new GUIContent("Clear Missing (" + missingCount + ")"), false, RecentMenuCallback_ClearMissing);
                    }
                }

                toolsMenu.ShowAsContext();
            }
        }

        private void OnInspectorGUI_StreamingAssetsButton(SerializedProperty propPath, SerializedProperty propLocation)
        {
            GUI.color = Color.white;

            if (GUILayout.Button("SA", GUILayout.Width(32f)))
            {
                GenericMenu toolsMenu = new GenericMenu();
                toolsMenu.AddDisabledItem(new GUIContent("StreamingAssets Files:"));
                toolsMenu.AddSeparator("");

                if (System.IO.Directory.Exists(Application.streamingAssetsPath))
                {
                    List<string> files = new List<string>();

                    string[] allFiles = System.IO.Directory.GetFiles(Application.streamingAssetsPath, "*", System.IO.SearchOption.AllDirectories);
                    if (allFiles != null && allFiles.Length > 0)
                    {
                        // Filter by type
                        for (int i = 0; i < allFiles.Length; i++)
                        {
                            bool remove = false;
                            if (allFiles[i].EndsWith(".meta", System.StringComparison.InvariantCultureIgnoreCase))
                            {
                                remove = true;
                            }
                            if (!remove)
                            {
                                files.Add(allFiles[i]);
                            }
                        }
                    }

                    if (files.Count > 0)
                    {
                        for (int i = 0; i < files.Count; i++)
                        {
                            string path = files[i];
                            if (System.IO.File.Exists(path))
                            {
                                string itemName = path.Replace(Application.streamingAssetsPath, "");
                                if (itemName.StartsWith("/") || itemName.StartsWith("\\"))
                                {
                                    itemName = itemName.Remove(0, 1);
                                }
                                itemName = itemName.Replace("\\", "/");

                                toolsMenu.AddItem(new GUIContent(itemName), false, RecentMenuCallback_Select, new RecentFileData(path, propPath, propLocation, target));
                            }
                        }
                    }
                    else
                    {
                        toolsMenu.AddDisabledItem(new GUIContent("StreamingAssets folder contains no files"));
                    }
                }
                else
                {
                    toolsMenu.AddDisabledItem(new GUIContent("StreamingAssets folder doesn't exist"));
                }

                toolsMenu.ShowAsContext();
            }
        }

        private static void ShowNoticeBox(MessageType messageType, string message)
        {
            //GUI.backgroundColor = Color.yellow;
            //EditorGUILayout.HelpBox(message, messageType);

            switch (messageType)
            {
                case MessageType.Error:
                    GUI.color = Color.red;
                    message = "Error: " + message;
                    break;
                case MessageType.Warning:
                    GUI.color = Color.yellow;
                    message = "Warning: " + message;
                    break;
            }

            //GUI.color = Color.yellow;
            GUILayout.TextArea(message);
            GUI.color = Color.white;
        }

        private static void ShowFileWarningMessages(string filePath, MediaPlayer.FileLocation fileLocation, bool isAutoOpen, Platform platform)
        {
            string finalPath = MediaPlayer.GetFilePath(filePath, fileLocation);

            if (string.IsNullOrEmpty(filePath))
            {
                if (isAutoOpen)
                {
                    ShowNoticeBox(MessageType.Error, "No file specified");
                }
                else
                {
                    ShowNoticeBox(MessageType.Warning, "No file specified");
                }
            }
            else
            {
                bool isPlatformAndroid = (platform == Platform.Android) || (platform == Platform.Unknown && BuildTargetGroup.Android == UnityEditor.EditorUserBuildSettings.selectedBuildTargetGroup);
                bool isPlatformIOS = (platform == Platform.iOS);
#if AVPRO_UNITY_IOS_ALLOWHTTPDOWNLOAD
                isPlatformIOS |= (platform == Platform.Unknown && BuildTargetGroup.iOS == UnityEditor.EditorUserBuildSettings.selectedBuildTargetGroup);
#endif
#if AVPRO_UNITY_PLATFORM_TVOS
                bool isPlatformTVOS = (platform == Platform.tvOS);

                isPlatformTVOS |= (platform == Platform.Unknown && BuildTargetGroup.tvOS == UnityEditor.EditorUserBuildSettings.selectedBuildTargetGroup);
#endif

                // Test file extensions
                {
                    bool isExtensionAVI = filePath.ToLower().EndsWith(".avi");
                    bool isExtensionMOV = filePath.ToLower().EndsWith(".mov");
                    bool isExtensionMKV = filePath.ToLower().EndsWith(".mkv");

                    if (isPlatformAndroid && isExtensionMOV)
                    {
                        ShowNoticeBox(MessageType.Warning, "MOV file detected. Android doesn't support MOV files, you should change the container file.");
                    }
                    if (isPlatformAndroid && isExtensionAVI)
                    {
                        ShowNoticeBox(MessageType.Warning, "AVI file detected. Android doesn't support AVI files, you should change the container file.");
                    }
                    if (isPlatformAndroid && isExtensionMKV)
                    {
                        ShowNoticeBox(MessageType.Warning, "MKV file detected. Android doesn't support MKV files, you should change the container file.");
                    }
                    if (isPlatformIOS && isExtensionAVI)
                    {
                        ShowNoticeBox(MessageType.Warning, "AVI file detected. iOS doesn't support AVI files, you should change the container file.");
                    }
                }

                if (finalPath.Contains("://"))
                {
                    if (filePath.Contains("youtube.com/watch"))
                    {
                        ShowNoticeBox(MessageType.Warning, "YouTube URL detected. YouTube website URL contains no media, a direct media file URL (eg MP4 or M3U8) is required.  See the documentation FAQ for YouTube support.");
                    }
                    if (fileLocation != MediaPlayer.FileLocation.AbsolutePathOrURL)
                    {
                        ShowNoticeBox(MessageType.Warning, "URL detected, change location type to URL?");
                    }
                    else
                    {
#if AVPRO_UNITY_IOS_ALLOWHTTPDOWNLOAD
                        // Display warning to iOS users if they're trying to use HTTP url without setting the permission

                        if (isPlatformIOS
#if AVPRO_UNITY_PLATFORM_TVOS
 || isPlatformTVOS
#endif
)
                        {
                            if (!PlayerSettings.iOS.allowHTTPDownload && filePath.StartsWith("http://"))
                            {
                                ShowNoticeBox(MessageType.Warning, "Starting with iOS 9 'allow HTTP downloads' must be enabled for HTTP connections (see Player Settings)");
                            }
                        }
#endif
                        // Display warning for Android users if they're trying to use a URL without setting permission
                        if (isPlatformAndroid && !PlayerSettings.Android.forceInternetPermission)
                        {
                            ShowNoticeBox(MessageType.Warning, "You may need to set 'Internet Access' to 'require' in your Player Settings for Android builds when using URLs");
                        }

                        // Display warning for UWP users if they're trying to use a URL without setting permission
                        if (platform == Platform.WindowsUWP || (platform == Platform.Unknown && (
#if !AVPRO_UNITY_WP8_DEPRECATED
                            BuildTargetGroup.WP8 == UnityEditor.EditorUserBuildSettings.selectedBuildTargetGroup ||
#endif
#if AVPRO_UNITY_METRO
                            BuildTargetGroup.Metro == UnityEditor.EditorUserBuildSettings.selectedBuildTargetGroup
#else
BuildTargetGroup.WSA == UnityEditor.EditorUserBuildSettings.selectedBuildTargetGroup
#endif
)))
                        {
#if AVPRO_UNITY_METRO
                            if (!PlayerSettings.Metro.GetCapability(PlayerSettings.MetroCapability.InternetClient))
#else
                            if (!PlayerSettings.WSA.GetCapability(PlayerSettings.WSACapability.InternetClient))
#endif
                            {
                                ShowNoticeBox(MessageType.Warning, "You may need to set 'InternetClient' capability in your Player Settings when using URLs");
                            }
                        }
                    }
                }
                else
                {
                    if (fileLocation != MediaPlayer.FileLocation.AbsolutePathOrURL && filePath.StartsWith("/"))
                    {
                        ShowNoticeBox(MessageType.Warning, "Absolute path detected, change location to Absolute path?");
                    }

                    // Display warning for Android users if they're trying to use absolute file path without permission
                    if (isPlatformAndroid && !PlayerSettings.Android.forceSDCardPermission)
                    {
                        ShowNoticeBox(MessageType.Warning, "You may need to access the local file system you may need to set 'Write Access' to 'External(SDCard)' in your Player Settings for Android");
                    }

                    if (platform == Platform.Unknown || platform == MediaPlayer.GetPlatform())
                    {
                        if (!System.IO.File.Exists(finalPath))
                        {
                            ShowNoticeBox(MessageType.Error, "File not found");
                        }
                        else
                        {
                            // Check the case
                            // This approach is very slow, so we only run it when the app isn't playing
                            if (!Application.isPlaying)
                            {
                                string comparePath = finalPath.Replace('\\', '/');
                                string folderPath = System.IO.Path.GetDirectoryName(comparePath);
                                if (!string.IsNullOrEmpty(folderPath))
                                {

                                    string[] files = System.IO.Directory.GetFiles(folderPath, "*", System.IO.SearchOption.TopDirectoryOnly);
                                    bool caseMatch = false;
                                    if (files != null && files.Length > 0)
                                    {
                                        //Debug.Log("final: " + comparePath);
                                        for (int i = 0; i < files.Length; i++)
                                        {
                                            //Debug.Log("comp: " + files[i].Replace('\\', '/'));
                                            if (files[i].Replace('\\', '/') == comparePath)
                                            {
                                                caseMatch = true;
                                                break;
                                            }
                                        }
                                    }
                                    if (!caseMatch)
                                    {
                                        ShowNoticeBox(MessageType.Warning, "File found but case doesn't match");
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (fileLocation == MediaPlayer.FileLocation.RelativeToStreamingAssetsFolder)
            {
                if (!System.IO.Directory.Exists(Application.streamingAssetsPath))
                {
                    GUILayout.BeginHorizontal();
                    GUI.color = Color.yellow;
                    GUILayout.TextArea("Warning: No StreamingAssets folder found");

                    if (GUILayout.Button("Create Folder"))
                    {
                        System.IO.Directory.CreateDirectory(Application.streamingAssetsPath);
                        AssetDatabase.Refresh();
                    }
                    GUILayout.EndHorizontal();
                }
                else
                {
                    bool checkAndroidFileSize = false;
#if UNITY_ANDROID
                    if (platform == Platform.Unknown)
                    {
                        checkAndroidFileSize = true;
                    }
#endif
                    if (platform == Platform.Android)
                    {
                        checkAndroidFileSize = true;
                    }

                    if (checkAndroidFileSize)
                    {
                        try
                        {
                            System.IO.FileInfo info = new System.IO.FileInfo(finalPath);
                            if (info != null && info.Length > (1024 * 1024 * 512))
                            {
                                ShowNoticeBox(MessageType.Warning, "Using this very large file inside StreamingAssets folder on Android isn't recommended.  Deployments will be slow and mapping the file from the StreamingAssets JAR may cause storage and memory issues.  We recommend loading from another folder on the device.");
                            }
                        }
                        catch (System.Exception)
                        {
                        }
                    }
                }
            }

            GUI.color = Color.white;
        }

        private void OnInspectorGUI_VideoPreview(MediaPlayer media, IMediaProducer textureSource)
        {
            Texture texture = null;
            if (textureSource != null)
            {
                texture = textureSource.GetTexture();
            }
            if (texture == null)
            {
                texture = EditorGUIUtility.whiteTexture;
            }

            float ratio = (float)texture.width / (float)texture.height;

            // Reserve rectangle for texture
            GUILayout.BeginHorizontal();
            GUILayout.FlexibleSpace();
            Rect textureRect;
            Rect alphaRect = new Rect(0f, 0f, 1f, 1f);
            if (texture != EditorGUIUtility.whiteTexture)
            {
                textureRect = GUILayoutUtility.GetRect(Screen.width / 2, Screen.width / 2, (Screen.width / 2) / ratio, (Screen.width / 2) / ratio);
                if (_showAlpha)
                {
                    alphaRect = GUILayoutUtility.GetRect(Screen.width / 2, Screen.width / 2, (Screen.width / 2) / ratio, (Screen.width / 2) / ratio);
                }
            }
            else
            {
                textureRect = GUILayoutUtility.GetRect(1920f / 40f, 1080f / 40f);
                if (_showAlpha)
                {
                    alphaRect = GUILayoutUtility.GetRect(1920f / 40f, 1080f / 40f);
                }
            }
            GUILayout.FlexibleSpace();
            GUILayout.EndHorizontal();

            // Dimensions
            string dimensionText = string.Format("{0}x{1}@{2}", 0, 0, 0.0f);
            if (texture != EditorGUIUtility.whiteTexture && media.Info != null)
            {
                dimensionText = string.Format("{0}x{1}@{2:F2}", texture.width, texture.height, media.Info.GetVideoFrameRate());
            }

            CentreLabel(dimensionText);

            string rateText = "0";
            string playerText = string.Empty;
            if (media.Info != null)
            {
                rateText = media.Info.GetVideoDisplayRate().ToString("F2");
                playerText = media.Info.GetPlayerDescription();
            }

            EditorGUILayout.LabelField("Display Rate", rateText);
            EditorGUILayout.LabelField("Using", playerText);
            _showAlpha = EditorGUILayout.Toggle("Show Alpha", _showAlpha);

            // Draw the texture
            Matrix4x4 prevMatrix = GUI.matrix;
            if (textureSource != null && textureSource.RequiresVerticalFlip())
            {
                GUIUtility.ScaleAroundPivot(new Vector2(1f, -1f), new Vector2(0, textureRect.y + (textureRect.height / 2)));
            }
            {
                if (!GUI.enabled)
                {
                    GUI.color = Color.grey;
                }
                EditorGUI.DrawTextureTransparent(textureRect, texture, ScaleMode.ScaleToFit);

                //GUI.DrawTexture(textureRect, texture, ScaleMode.ScaleToFit, false);
                GUI.color = Color.white;
            }
            if (_showAlpha)
            {
                EditorGUI.DrawTextureAlpha(alphaRect, texture, ScaleMode.ScaleToFit);
            }
            GUI.matrix = prevMatrix;

            // Select texture button
            GUILayout.BeginHorizontal(GUILayout.ExpandWidth(true));
            GUILayout.FlexibleSpace();
            if (GUILayout.Button("Select Texture", GUILayout.ExpandWidth(false)))
            {
                Selection.activeObject = texture;
            }
            if (GUILayout.Button("Save PNG", GUILayout.ExpandWidth(true)))
            {
                media.SaveFrameToPng();
            }
            GUILayout.FlexibleSpace();
            GUILayout.EndHorizontal();
        }

        private void OnInspectorGUI_PlayControls(IMediaControl control, IMediaInfo info)
        {
            GUILayout.Space(8.0f);

            // Slider
            EditorGUILayout.BeginHorizontal();
            bool isPlaying = false;
            if (control != null)
            {
                isPlaying = control.IsPlaying();
            }
            float currentTime = 0f;
            if (control != null)
            {
                currentTime = control.GetCurrentTimeMs();
            }

            float durationTime = 0f;
            if (info != null)
            {
                durationTime = info.GetDurationMs();
                if (float.IsNaN(durationTime))
                {
                    durationTime = 0f;
                }
            }
            string timeUsed = Helper.GetTimeString(currentTime / 1000f);
            GUILayout.Label(timeUsed, GUILayout.ExpandWidth(false));

            float newTime = GUILayout.HorizontalSlider(currentTime, 0f, durationTime, GUILayout.ExpandWidth(true));
            if (newTime != currentTime)
            {
                control.Seek(newTime);
            }

            string timeTotal = "Infinity";
            if (!float.IsInfinity(durationTime))
            {
                timeTotal = Helper.GetTimeString(durationTime / 1000f);
            }

            GUILayout.Label(timeTotal, GUILayout.ExpandWidth(false));

            EditorGUILayout.EndHorizontal();

            // Buttons
            EditorGUILayout.BeginHorizontal();
            if (GUILayout.Button("Rewind", GUILayout.ExpandWidth(false)))
            {
                control.Rewind();
            }

            if (!isPlaying)
            {
                GUI.color = Color.green;
                if (GUILayout.Button("Play", GUILayout.ExpandWidth(true)))
                {
                    control.Play();
                }
            }
            else
            {
                GUI.color = Color.yellow;
                if (GUILayout.Button("Pause", GUILayout.ExpandWidth(true)))
                {
                    control.Pause();
                }
            }
            GUI.color = Color.white;
            EditorGUILayout.EndHorizontal();
        }

        private struct Native
        {
#if UNITY_EDITOR_WIN
            [System.Runtime.InteropServices.DllImport("AVProVideo")]
            public static extern System.IntPtr GetPluginVersion();
#elif UNITY_EDITOR_OSX
            [System.Runtime.InteropServices.DllImport("AVProVideo")]
            public static extern string AVPGetVersion();
#endif
        }

        private void OnInspectorGUI_About()
        {
            //GUILayout.Space(8f);

            GUI.color = Color.white;
            GUI.backgroundColor = Color.clear;
            if (_expandAbout)
            {
                GUI.color = Color.white;
                GUI.backgroundColor = new Color(0.8f, 0.8f, 0.8f, 0.1f);
                if (EditorGUIUtility.isProSkin)
                {
                    GUI.backgroundColor = Color.black;
                }
            }
            GUILayout.BeginVertical("box");
            GUI.backgroundColor = Color.white;
            if (GUILayout.Button("About", EditorStyles.toolbarButton))
            {
                _expandAbout = !_expandAbout;
            }
            GUI.color = Color.white;

            if (_expandAbout)
            {
                string version = "Unknown";
#if UNITY_EDITOR_WIN
                version = System.Runtime.InteropServices.Marshal.PtrToStringAnsi(Native.GetPluginVersion());
#elif UNITY_EDITOR_OSX
                version = Native.AVPGetVersion();
#endif

                EditorGUILayout.BeginHorizontal();
                GUILayout.FlexibleSpace();
                if (_icon == null)
                {
                    _icon = Resources.Load<Texture2D>("AVProVideoIcon");
                }
                if (_icon != null)
                {
                    GUILayout.Label(new GUIContent(_icon));
                }
                GUILayout.FlexibleSpace();
                EditorGUILayout.EndHorizontal();

                GUI.color = Color.yellow;
                CentreLabel("AVPro Video by RenderHeads Ltd", EditorStyles.boldLabel);
                CentreLabel("version " + version + " (scripts v" + Helper.ScriptVersion + ")");
                GUI.color = Color.white;



                GUILayout.Space(32f);
                GUI.backgroundColor = Color.white;

                EditorGUILayout.LabelField("Links", EditorStyles.boldLabel);

                GUILayout.Space(8f);

                EditorGUILayout.LabelField("Documentation");
                if (GUILayout.Button("User Manual", GUILayout.ExpandWidth(false)))
                {
                    Application.OpenURL(LinkUserManual);
                }
                if (GUILayout.Button("Scripting Class Reference", GUILayout.ExpandWidth(false)))
                {
                    Application.OpenURL(LinkScriptingClassReference);
                }

                GUILayout.Space(16f);

                GUILayout.Label("Rate and Review (★★★★☆)", GUILayout.ExpandWidth(false));
                if (GUILayout.Button("Unity Asset Store Page", GUILayout.ExpandWidth(false)))
                {
                    Application.OpenURL(LinkAssetStorePage);
                }

                GUILayout.Space(16f);

                GUILayout.Label("Community");
                if (GUILayout.Button("Unity Forum Page", GUILayout.ExpandWidth(false)))
                {
                    Application.OpenURL(LinkForumPage);
                }

                GUILayout.Space(16f);

                GUILayout.Label("Homepage", GUILayout.ExpandWidth(false));
                if (GUILayout.Button("AVPro Video Website", GUILayout.ExpandWidth(false)))
                {
                    Application.OpenURL(LinkPluginWebsite);
                }

                GUILayout.Space(16f);

                GUILayout.Label("Bugs and Support");
                EditorGUILayout.BeginHorizontal();
                if (GUILayout.Button("Email unitysupport@renderheads.com", GUILayout.ExpandWidth(false)))
                {
                    Application.OpenURL(LinkEmailSupport);
                }
                EditorGUILayout.EndHorizontal();

                GUILayout.Space(32f);

                EditorGUILayout.LabelField("Credits", EditorStyles.boldLabel);
                GUILayout.Space(8f);

                CentreLabel("Programming", EditorStyles.boldLabel);
                CentreLabel("Andrew Griffiths");
                CentreLabel("Morris Butler");
                CentreLabel("Ste Butcher");
                CentreLabel("Muano Mainganye");
                CentreLabel("Shane Marks");
                CentreLabel("Sunrise Wang");
                GUILayout.Space(8f);
                CentreLabel("Graphics", EditorStyles.boldLabel);
                GUILayout.Space(8f);
                CentreLabel("Jeff Rusch");

                GUILayout.Space(32f);

                EditorGUILayout.LabelField("Bug Reporting Notes", EditorStyles.boldLabel);

                EditorGUILayout.SelectableLabel(SupportMessage, EditorStyles.wordWrappedLabel, GUILayout.Height(300f));
            }

            EditorGUILayout.EndVertical();
        }

        private void OnInspectorGUI_Events()
        {
            //GUILayout.Space(8f);
            GUI.color = Color.white;
            GUI.backgroundColor = Color.clear;
            if (_expandEvents)
            {
                GUI.color = Color.white;
                GUI.backgroundColor = new Color(0.8f, 0.8f, 0.8f, 0.1f);
                if (EditorGUIUtility.isProSkin)
                {
                    GUI.backgroundColor = Color.black;
                }
            }

            GUILayout.BeginVertical("box");
            GUI.backgroundColor = Color.white;

            if (GUILayout.Button("Events", EditorStyles.toolbarButton))
            {
                _expandEvents = !_expandEvents;
            }
            GUI.color = Color.white;

            if (_expandEvents)
            {
                EditorGUILayout.PropertyField(_propEvents);
            }

            GUILayout.EndVertical();
        }

        private void OnInspectorGUI_MediaProperties()
        {
            //GUILayout.Space(8f);
            GUI.color = Color.white;
            GUI.backgroundColor = Color.clear;
            if (_expandMediaProperties)
            {
                GUI.color = Color.white;
                GUI.backgroundColor = new Color(0.8f, 0.8f, 0.8f, 0.1f);
                if (EditorGUIUtility.isProSkin)
                {
                    GUI.backgroundColor = Color.black;
                }
            }

            GUILayout.BeginVertical("box");
            GUI.backgroundColor = Color.white;

            if (GUILayout.Button("Media Properties", EditorStyles.toolbarButton))
            {
                _expandMediaProperties = !_expandMediaProperties;
            }
            GUI.color = Color.white;

            if (_expandMediaProperties)
            {
                MediaPlayer media = (this.target) as MediaPlayer;

                EditorGUILayout.BeginVertical();
                GUILayout.Label("Texture", EditorStyles.boldLabel);

                EditorGUILayout.PropertyField(_propFilter, new GUIContent("Filter"));
                EditorGUILayout.PropertyField(_propWrap, new GUIContent("Wrap"));
                EditorGUILayout.PropertyField(_propAniso, new GUIContent("Aniso"));

                if (_propWrap.enumValueIndex != (int)media.m_WrapMode ||
                    _propFilter.enumValueIndex != (int)media.m_FilterMode ||
                    _propAniso.intValue != media.m_AnisoLevel)
                {
                    if (media.Control != null)
                    {
                        media.Control.SetTextureProperties((FilterMode)_propFilter.enumValueIndex, (TextureWrapMode)_propWrap.enumValueIndex, _propAniso.intValue);
                    }
                }

                EditorGUILayout.EndVertical();


                EditorGUILayout.BeginVertical();
                GUILayout.Label("Transparency", EditorStyles.boldLabel);
                EditorGUILayout.PropertyField(_propAlphaPacking, new GUIContent("Packing"));
                EditorGUILayout.EndVertical();

                EditorGUILayout.BeginVertical();
                GUILayout.Label("Stereo", EditorStyles.boldLabel);
                EditorGUILayout.PropertyField(_propStereoPacking, new GUIContent("Packing"));
                EditorGUILayout.PropertyField(_propDisplayStereoTint, new GUIContent("Debug Eye Tint", "Tints the left eye green and the right eye red so you can confirm stereo is working"));
                EditorGUILayout.EndVertical();
            }

            GUILayout.EndVertical();
        }

        private static void CentreLabel(string text, GUIStyle style = null)
        {
            GUILayout.BeginHorizontal();
            GUILayout.FlexibleSpace();
            if (style == null)
            {
                GUILayout.Label(text);
            }
            else
            {
                GUILayout.Label(text, style);
            }
            GUILayout.FlexibleSpace();
            GUILayout.EndHorizontal();
        }

        private void OnInspectorGUI_GlobalSettings()
        {
            GUI.color = Color.white;
            GUI.backgroundColor = Color.clear;
            if (_expandGlobalSettings)
            {
                GUI.color = Color.white;
                GUI.backgroundColor = new Color(0.8f, 0.8f, 0.8f, 0.1f);
                if (EditorGUIUtility.isProSkin)
                {
                    GUI.backgroundColor = Color.black;
                }
            }

            GUILayout.BeginVertical("box");
            GUI.backgroundColor = Color.white;

            if (GUILayout.Button("Global Settings", EditorStyles.toolbarButton))
            {
                _expandGlobalSettings = !_expandGlobalSettings;
            }
            GUI.color = Color.white;

            if (_expandGlobalSettings)
            {
                GUI.enabled = (!Application.isPlaying);
                EditorGUILayout.LabelField("Current Platform", EditorUserBuildSettings.selectedBuildTargetGroup.ToString());

                // TimeScale
                {
                    const string TimeScaleDefine = "AVPROVIDEO_BETA_SUPPORT_TIMESCALE";

                    string defines = PlayerSettings.GetScriptingDefineSymbolsForGroup(EditorUserBuildSettings.selectedBuildTargetGroup);
                    bool supportsTimeScale = defines.Contains(TimeScaleDefine);
                    bool supportsTimeScaleNew = EditorGUILayout.Toggle("TimeScale Support", supportsTimeScale);
                    if (supportsTimeScale != supportsTimeScaleNew)
                    {
                        if (supportsTimeScaleNew)
                        {
                            defines += ";" + TimeScaleDefine + ";";
                        }
                        else
                        {
                            defines = defines.Replace(TimeScaleDefine, "");
                        }

                        PlayerSettings.SetScriptingDefineSymbolsForGroup(EditorUserBuildSettings.selectedBuildTargetGroup, defines);
                    }

                    if (supportsTimeScaleNew)
                    {
                        ShowNoticeBox(MessageType.Warning, "This will affect performance if you change Time.timeScale or Time.captureFramerate.  This feature is useful for supporting video capture system that adjust time scale during capturing.");
                    }
                }

                // Disable Debug GUI
                {
                    const string DisableDebugGUIDefine = "AVPROVIDEO_DISABLE_DEBUG_GUI";

                    string defines = PlayerSettings.GetScriptingDefineSymbolsForGroup(EditorUserBuildSettings.selectedBuildTargetGroup);
                    bool disableDebugGUI = defines.Contains(DisableDebugGUIDefine);
                    EditorGUILayout.BeginHorizontal();
                    bool disableDebugGUINew = EditorGUILayout.Toggle("Disable Debug GUI", disableDebugGUI);
                    GUILayout.Label("(in builds only)");
                    EditorGUILayout.EndHorizontal();
                    if (disableDebugGUI != disableDebugGUINew)
                    {
                        if (disableDebugGUINew)
                        {
                            defines += ";" + DisableDebugGUIDefine + ";";
                        }
                        else
                        {
                            defines = defines.Replace(DisableDebugGUIDefine, "");
                        }

                        PlayerSettings.SetScriptingDefineSymbolsForGroup(EditorUserBuildSettings.selectedBuildTargetGroup, defines);
                    }
                    if (!disableDebugGUI)
                    {
                        GUI.color = Color.white;
                        GUILayout.TextArea("The Debug GUI can be disabled globally for builds to help reduce garbage generation each frame.");
                        GUI.color = Color.white;
                    }
                }

                // Disable Logging
                {
                    const string DisableLogging = "AVPROVIDEO_DISABLE_LOGGING";

                    string defines = PlayerSettings.GetScriptingDefineSymbolsForGroup(EditorUserBuildSettings.selectedBuildTargetGroup);
                    bool disableDebugGUI = defines.Contains(DisableLogging);
                    bool disableDebugGUINew = EditorGUILayout.Toggle("Disable Logging", disableDebugGUI);
                    if (disableDebugGUI != disableDebugGUINew)
                    {
                        if (disableDebugGUINew)
                        {
                            defines += ";" + DisableLogging + ";";
                        }
                        else
                        {
                            defines = defines.Replace(DisableLogging, "");
                        }

                        PlayerSettings.SetScriptingDefineSymbolsForGroup(EditorUserBuildSettings.selectedBuildTargetGroup, defines);
                    }
                }

                GUI.enabled = true;
            }

            GUILayout.EndVertical();
        }

        private void OnInspectorGUI_PlatformOverrides()
        {
            MediaPlayer media = (this.target) as MediaPlayer;

            //GUILayout.Space(8f);
            GUI.color = Color.white;
            GUI.backgroundColor = Color.clear;
            if (_expandPlatformOverrides)
            {
                GUI.color = Color.white;
                GUI.backgroundColor = new Color(0.8f, 0.8f, 0.8f, 0.1f);
                if (EditorGUIUtility.isProSkin)
                {
                    GUI.backgroundColor = Color.black;
                }
            }
            GUILayout.BeginVertical("box");
            GUI.backgroundColor = Color.white;

            if (GUILayout.Button("Platform Specific", EditorStyles.toolbarButton))
            {
                _expandPlatformOverrides = !_expandPlatformOverrides;
            }
            GUI.color = Color.white;

            if (_expandPlatformOverrides)
            {
                int rowCount = 0;
                int platformIndex = _platformIndex;
                for (int i = 0; i < _platformNames.Length; i++)
                {
                    if (i % 3 == 0)
                    {
                        GUILayout.BeginHorizontal();
                        rowCount++;
                    }
                    MediaPlayer.PlatformOptions options = media.GetPlatformOptions((Platform)i);

                    Color hilight = Color.yellow;

                    if (i == _platformIndex)
                    {
                        // Selected, unmodified
                        if (!options.IsModified())
                        {
                            GUI.contentColor = Color.white;
                        }
                        else
                        {
                            // Selected, modified
                            GUI.color = hilight;
                            GUI.contentColor = Color.white;
                        }
                    }
                    else if (options.IsModified())
                    {
                        // Unselected, modified
                        GUI.backgroundColor = Color.grey * hilight;
                        GUI.contentColor = hilight;
                    }
                    else
                    {
                        // Unselected, unmodified
                        if (EditorGUIUtility.isProSkin)
                        {
                            GUI.backgroundColor = Color.grey;
                            GUI.color = new Color(0.65f, 0.66f, 0.65f);// Color.grey;
                        }
                    }

                    if (i == _platformIndex)
                    {
                        if (!GUILayout.Toggle(true, _platformNames[i], GUI.skin.button))
                        {
                            platformIndex = -1;
                        }
                    }
                    else
                    {
                        if (GUILayout.Button(_platformNames[i]))
                        {
                            platformIndex = i;
                        }
                    }
                    if ((i + 1) % 3 == 0)
                    {
                        rowCount--;
                        GUILayout.EndHorizontal();
                    }
                    GUI.backgroundColor = Color.white;
                    GUI.contentColor = Color.white;
                    GUI.color = Color.white;
                }

                if (rowCount > 0)
                {
                    GUILayout.EndHorizontal();
                }

                //platformIndex = GUILayout.SelectionGrid(_platformIndex, Helper.GetPlatformNames(), 3);
                //int platformIndex = GUILayout.Toolbar(_platformIndex, Helper.GetPlatformNames());

                if (platformIndex != _platformIndex)
                {
                    _platformIndex = platformIndex;

                    // We do this to clear the focus, otherwise a focused text field will not change when the Toolbar index changes
                    EditorGUI.FocusTextInControl("ClearFocus");
                }

                OnInspectorGUI_PathOverrides();
                switch ((Platform)_platformIndex)
                {
                    case Platform.Windows:
                        OnInspectorGUI_Override_Windows();
                        break;
                    case Platform.MacOSX:
                        OnInspectorGUI_Override_MacOSX();
                        break;
                    case Platform.iOS:
                        OnInspectorGUI_Override_iOS();
                        break;
                    case Platform.tvOS:
                        OnInspectorGUI_Override_tvOS();
                        break;
                    case Platform.Android:
                        OnInspectorGUI_Override_Android();
                        break;
                    case Platform.WindowsPhone:
                        OnInspectorGUI_Override_WindowsPhone();
                        break;
                    case Platform.WindowsUWP:
                        OnInspectorGUI_Override_WindowsUWP();
                        break;
                    case Platform.WebGL:
                        OnInspectorGUI_Override_WebGL();
                        break;
                }
            }
            GUILayout.EndVertical();
        }

        void OnInspectorGUI_Preview()
        {
            MediaPlayer media = (this.target) as MediaPlayer;

            //GUILayout.Space(8f);
            GUI.color = Color.white;
            GUI.backgroundColor = Color.clear;

            if (_expandPreview)
            {
                GUI.color = Color.white;
                GUI.backgroundColor = new Color(0.8f, 0.8f, 0.8f, 0.1f);
                if (EditorGUIUtility.isProSkin)
                {
                    GUI.backgroundColor = Color.black;
                }
            }
            GUILayout.BeginVertical("box");
            GUI.backgroundColor = Color.white;

            if (GUILayout.Button("Preview", EditorStyles.toolbarButton))
            {
                _expandPreview = !_expandPreview;
            }
            GUI.color = Color.white;

            if (_expandPreview)
            {
                GUI.enabled = (media.TextureProducer != null && media.Info.HasVideo());
                OnInspectorGUI_VideoPreview(media, media.TextureProducer);
                GUI.enabled = true;

                GUI.enabled = (media.Control != null && media.Control.CanPlay() && media.isActiveAndEnabled && !EditorApplication.isPaused);
                OnInspectorGUI_PlayControls(media.Control, media.Info);
                GUI.enabled = true;
            }

            GUILayout.EndVertical();
        }

        private static string GetStartFolder(string path, MediaPlayer.FileLocation fileLocation)
        {
            // Try to resolve based on file path + file location
            string result = MediaPlayer.GetFilePath(path, fileLocation);
            if (!string.IsNullOrEmpty(result))
            {
                if (System.IO.File.Exists(result))
                {
                    result = System.IO.Path.GetDirectoryName(result);
                }
            }

            if (!System.IO.Directory.Exists(result))
            {
                // Just resolve on file location
                result = MediaPlayer.GetPath(fileLocation);
            }
            if (string.IsNullOrEmpty(result))
            {
                // Fallback
                result = Application.streamingAssetsPath;
            }
            return result;
        }

        private void GUI_OverridePath(int platformIndex)
        {
            string optionsVarName = MediaPlayer.GetPlatformOptionsVariable((Platform)platformIndex);

            SerializedProperty propLocation = serializedObject.FindProperty(optionsVarName + ".pathLocation");
            if (propLocation != null)
            {
                EditorGUILayout.PropertyField(propLocation, GUIContent.none);
            }
            SerializedProperty propPath = serializedObject.FindProperty(optionsVarName + ".path");
            if (propPath != null)
            {

                {
                    string oldPath = propPath.stringValue;
                    string newPath = EditorGUILayout.TextField(string.Empty, propPath.stringValue);
                    if (newPath != oldPath)
                    {
                        // Check for invalid characters
                        if (0 > newPath.IndexOfAny(System.IO.Path.GetInvalidPathChars()))
                        {
                            propPath.stringValue = newPath.Replace("\\", "/");
                            EditorUtility.SetDirty(target);
                        }
                    }
                }
            }

            GUILayout.BeginHorizontal();
            OnInspectorGUI_RecentButton(propPath, propLocation);
            OnInspectorGUI_StreamingAssetsButton(propPath, propLocation);
            GUI.color = Color.green;
            if (GUILayout.Button("BROWSE"))
            {
                string result = string.Empty;
                MediaPlayer.FileLocation fileLocation = MediaPlayer.FileLocation.RelativeToStreamingAssetsFolder;
                string startFolder = GetStartFolder(propPath.stringValue, (MediaPlayer.FileLocation)propLocation.enumValueIndex);
                string fullPath = string.Empty;
                if (Browse(startFolder, ref result, ref fileLocation, ref fullPath))
                {
                    propPath.stringValue = result.Replace("\\", "/");

                    propLocation.enumValueIndex = (int)fileLocation;
                    EditorUtility.SetDirty(target);			// TODO: not sure if we need this any more.  Was put here to help prefabs save values I think
                }
            }

            GUILayout.EndHorizontal();

            GUI.color = Color.white;

            // Display the file name so it's easy to read and copy to the clipboard
            OnInspectorGUI_CopyableFilename(propPath.stringValue);

            if (GUI.enabled)
            {
                ShowFileWarningMessages(propPath.stringValue, (MediaPlayer.FileLocation)propLocation.enumValueIndex, false, (Platform)platformIndex);
            }
        }

        private void OnInspectorGUI_PathOverrides()
        {
            //MediaPlayer media = (this.target) as MediaPlayer;

            //MediaPlayer.PlatformOptions options = media.GetPlatformOptions((Platform)_platformIndex);
            //if (options != null)
            {
                string optionsVarName = MediaPlayer.GetPlatformOptionsVariable((Platform)_platformIndex);
                SerializedProperty propOverridePath = serializedObject.FindProperty(optionsVarName + ".overridePath");
                if (propOverridePath != null)
                {
                    EditorGUILayout.PropertyField(propOverridePath, new GUIContent("Override Path"));

                    //if (propOverridePath.boolValue)
                    {
                        GUI.enabled = propOverridePath.boolValue;
                        GUI_OverridePath(_platformIndex);
                        GUI.enabled = true;
                    }
                }
            }
        }

        private void OnInspectorGUI_Override_Windows()
        {
            //MediaPlayer media = (this.target) as MediaPlayer;
            //MediaPlayer.OptionsWindows options = media._optionsWindows;

            GUILayout.Space(8f);

            string optionsVarName = MediaPlayer.GetPlatformOptionsVariable((Platform)_platformIndex);
            SerializedProperty propVideoApi = serializedObject.FindProperty(optionsVarName + ".videoApi");
            if (propVideoApi != null)
            {
                EditorGUILayout.PropertyField(propVideoApi, new GUIContent("Preferred Video API", "The preferred video API to use"));

                EditorGUILayout.BeginVertical("box");
                GUILayout.Label("Media Foundation Options", EditorStyles.boldLabel);
                SerializedProperty propUseHardwareDecoding = serializedObject.FindProperty(optionsVarName + ".useHardwareDecoding");
                if (propUseHardwareDecoding != null)
                {
                    EditorGUILayout.PropertyField(propUseHardwareDecoding, new GUIContent("Hardware Decoding"));
                }

                SerializedProperty propUseUnityAudio = serializedObject.FindProperty(optionsVarName + ".useUnityAudio");
                if (propUseUnityAudio != null)
                {
                    EditorGUILayout.PropertyField(propUseUnityAudio, new GUIContent("Use Unity Audio", "Allows the AudioOutput component to grab audio from the video and play it through Unity to the AudioListener."));
                }
                EditorGUILayout.EndVertical();

                EditorGUILayout.BeginVertical("box");
                GUILayout.Label("DirectShow Options", EditorStyles.boldLabel);
                SerializedProperty propForceAudioOutputDeviceName = serializedObject.FindProperty(optionsVarName + ".forceAudioOutputDeviceName");
                if (propForceAudioOutputDeviceName != null)
                {
                    EditorGUILayout.PropertyField(propForceAudioOutputDeviceName, new GUIContent("Force Audio Output Device Name", "Useful for VR when you need to output to the VR audio device"));
                }
                EditorGUILayout.EndVertical();
            }
        }

        private void OnInspectorGUI_Override_MacOSX()
        {
            //MediaPlayer media = (this.target) as MediaPlayer;
            //MediaPlayer.OptionsMacOSX options = media._optionsMacOSX;
        }

        private void OnInspectorGUI_Override_iOS()
        {
            //MediaPlayer media = (this.target) as MediaPlayer;
            //MediaPlayer.OptionsIOS options = media._optionsIOS;
        }

        private void OnInspectorGUI_Override_tvOS()
        {
            //MediaPlayer media = (this.target) as MediaPlayer;
            //MediaPlayer.OptionsTVOS options = media._optionsTVOS;
        }

        private void OnInspectorGUI_Override_Android()
        {
            //MediaPlayer media = (this.target) as MediaPlayer;
            //MediaPlayer.OptionsAndroid options = media._optionsAndroid;

            GUILayout.Space(8f);

            string optionsVarName = MediaPlayer.GetPlatformOptionsVariable((Platform)_platformIndex);

            SerializedProperty propFileOffset = serializedObject.FindProperty(optionsVarName + ".fileOffset");
            if (propFileOffset != null)
            {
                EditorGUILayout.PropertyField(propFileOffset);
            }

            SerializedProperty propUseFastOesPath = serializedObject.FindProperty(optionsVarName + ".useFastOesPath");
            if (propUseFastOesPath != null)
            {
                EditorGUILayout.PropertyField(propUseFastOesPath, new GUIContent("Use Fast OES Path", "Enables a faster rendering path using OES textures.  This requires that all rendering in Unity uses special GLSL shaders."));
            }

            /*
            SerializedProperty propFileOffsetLow = serializedObject.FindProperty(optionsVarName + ".fileOffsetLow");
            SerializedProperty propFileOffsetHigh = serializedObject.FindProperty(optionsVarName + ".fileOffsetHigh");
            if (propFileOffsetLow != null && propFileOffsetHigh != null)
            {
                propFileOffsetLow.intValue = ;

                EditorGUILayout.PropertyField(propFileOFfset);
            }*/
        }

        private void OnInspectorGUI_Override_WindowsPhone()
        {
            //MediaPlayer media = (this.target) as MediaPlayer;
            //MediaPlayer.OptionsWindowsPhone options = media._optionsWindowsPhone;

            GUILayout.Space(8f);

            string optionsVarName = MediaPlayer.GetPlatformOptionsVariable((Platform)_platformIndex);

            SerializedProperty propUseHardwareDecoding = serializedObject.FindProperty(optionsVarName + ".useHardwareDecoding");
            if (propUseHardwareDecoding != null)
            {
                EditorGUILayout.PropertyField(propUseHardwareDecoding, new GUIContent("Hardware Decoding"));
            }
        }

        private void OnInspectorGUI_Override_WindowsUWP()
        {
            //MediaPlayer media = (this.target) as MediaPlayer;
            //MediaPlayer.OptionsWindowsUWP options = media._optionsWindowsUWP;

            GUILayout.Space(8f);

            string optionsVarName = MediaPlayer.GetPlatformOptionsVariable((Platform)_platformIndex);

            SerializedProperty propUseHardwareDecoding = serializedObject.FindProperty(optionsVarName + ".useHardwareDecoding");
            if (propUseHardwareDecoding != null)
            {
                EditorGUILayout.PropertyField(propUseHardwareDecoding, new GUIContent("Hardware Decoding"));
            }
        }

        private void OnInspectorGUI_Override_WebGL()
        {
            //MediaPlayer media = (this.target) as MediaPlayer;
            //MediaPlayer.OptionsWebGL options = media._optionsWebGL;
        }

        private static bool Browse(string startPath, ref string filePath, ref MediaPlayer.FileLocation fileLocation, ref string fullPath)
        {
            bool result = false;
#if UNITY_EDITOR_OSX
            string extensions = "mp4,m4v,mov,avi";
            extensions += ",mp3,m4a,aac,ac3,au,aiff,wav";
#else
            string extensions = "Media Files;*.mp4;*.mov;*.m4v;*.avi;*.mkv;*.ts;*.webm;*.flv;*.vob;*.ogg;*.ogv;*.mpg;*.wmv;*.3gp";
            extensions += ";Audio Files;*wav;*.mp3;*.mp2;*.m4a;*.wma;*.aac;*.au;*.flac";
#endif
            string path = UnityEditor.EditorUtility.OpenFilePanel("Browse Video File", startPath, extensions);
            if (!string.IsNullOrEmpty(path) && !path.EndsWith(".meta"))
            {
                fullPath = path;
                GetRelativeLocationFromPath(path, ref filePath, ref fileLocation);
                result = true;
            }

            return result;
        }

        private static void GetRelativeLocationFromPath(string path, ref string filePath, ref MediaPlayer.FileLocation fileLocation)
        {
            string projectRoot = System.IO.Path.GetFullPath(System.IO.Path.Combine(Application.dataPath, ".."));
            projectRoot = projectRoot.Replace('\\', '/');

            if (path.StartsWith(projectRoot))
            {
                if (path.StartsWith(Application.streamingAssetsPath))
                {
                    // Must be StreamingAssets relative path
                    filePath = GetPathRelativeTo(Application.streamingAssetsPath, path);
                    fileLocation = MediaPlayer.FileLocation.RelativeToStreamingAssetsFolder;
                }
                else if (path.StartsWith(Application.dataPath))
                {
                    // Must be Assets relative path
                    filePath = GetPathRelativeTo(Application.dataPath, path);
                    fileLocation = MediaPlayer.FileLocation.RelativeToDataFolder;
                }
                else
                {
                    // Must be project relative path
                    filePath = GetPathRelativeTo(projectRoot, path);
                    fileLocation = MediaPlayer.FileLocation.RelativeToProjectFolder;
                }
            }
            else
            {
                // Must be persistant data
                if (path.StartsWith(Application.persistentDataPath))
                {
                    filePath = GetPathRelativeTo(Application.persistentDataPath, path);
                    fileLocation = MediaPlayer.FileLocation.RelativeToPeristentDataFolder;
                }

                // Must be absolute path
                filePath = path;
                fileLocation = MediaPlayer.FileLocation.AbsolutePathOrURL;
            }
        }
    }
}