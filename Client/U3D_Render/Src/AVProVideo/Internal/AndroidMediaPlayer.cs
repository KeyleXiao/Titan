﻿//#define DLL_METHODS

//#if UNITY_ANDROID
//#if UNITY_5
//    #if !UNITY_5_0 && !UNITY_5_1
//        #define AVPROVIDEO_ISSUEPLUGINEVENT_UNITY52
//    #endif
//#endif

//using UnityEngine;
//using System;
//using System.Runtime.InteropServices;

////-----------------------------------------------------------------------------
//// Copyright 2015-2016 RenderHeads Ltd.  All rights reserverd.
////-----------------------------------------------------------------------------

//namespace RenderHeads.Media.AVProVideo
//{
//    // TODO: seal this class
//    public class AndroidMediaPlayer : BaseMediaPlayer
//    {
//        private static AndroidJavaObject	s_ActivityContext	= null;
//        private static bool					s_bInitialised		= false;

//        private static string				s_Version = "Plug-in not yet initialised";

//#if AVPROVIDEO_ISSUEPLUGINEVENT_UNITY52
//        private static System.IntPtr _nativeFunction_RenderEvent;
//#endif

//        protected AndroidJavaObject			m_Video;
//        private Texture2D					m_Texture;
//        private int                         m_TextureHandle;
//        private bool						m_UseFastOesPath;

//        private float						m_DurationMs		= 0.0f;
//        private int							m_Width				= 0;
//        private int							m_Height			= 0;

//        protected int 						m_iPlayerIndex		= -1;


//        public static void InitialisePlatform()
//        {
//            // Get the activity context
//            if( s_ActivityContext == null )
//            {
//                AndroidJavaClass activityClass = new AndroidJavaClass("com.unity3d.player.UnityPlayer");
//                if (activityClass != null)
//                {
//                    s_ActivityContext = activityClass.GetStatic<AndroidJavaObject>("currentActivity");
//                }
//            }

//            if( !s_bInitialised )
//            {
//                s_bInitialised = true;

//                AndroidJavaObject videoClass = new AndroidJavaObject("com.RenderHeads.AVProVideo.AVProMobileVideo");
//                if( videoClass != null )
//                {
//                    s_Version = videoClass.CallStatic<string>("GetPluginVersion");

//#if AVPROVIDEO_ISSUEPLUGINEVENT_UNITY52
//                    _nativeFunction_RenderEvent = Native.GetRenderEventFunc();
//#else
//                    // Calling this native function cause the .SO library to become loaded
//                    // This is important for Unity < 5.2.0 where GL.IssuePluginEvent works differently
//                    Native.GetRenderEventFunc();
//#endif
//                }
//            }
//        }

//        private static void IssuePluginEvent(Native.AVPPluginEvent type, int param)
//        {
//            // Build eventId from the type and param.
//            int eventId = 0x5d5ac000 | ((int)type << 8);

//            switch (type)
//            {
//                case Native.AVPPluginEvent.PlayerSetup:
//                case Native.AVPPluginEvent.PlayerUpdate:
//                case Native.AVPPluginEvent.PlayerDestroy:
//                    {
//                        eventId |= param & 0xff;
//                    }
//                    break;
//            }

//#if AVPROVIDEO_ISSUEPLUGINEVENT_UNITY52
//            GL.IssuePluginEvent(_nativeFunction_RenderEvent, eventId);
//#else
//            GL.IssuePluginEvent(eventId);
//#endif
//        }

//        public AndroidMediaPlayer(bool useFastOesPath)
//        {
//            // Create a java-size video class up front
//            m_Video = new AndroidJavaObject("com.RenderHeads.AVProVideo.AVProMobileVideo");

//            if (m_Video != null)
//            {
//                // Initialise
//                m_Video.Call("Initialise", s_ActivityContext);

//                m_iPlayerIndex = m_Video.Call<int>("GetPlayerIndex");

//                //Debug.Log( "AVPro: useFastOesPath: " + useFastOesPath );
//                SetOptions(useFastOesPath);

//                // Initialise renderer, on the render thread
//                AndroidMediaPlayer.IssuePluginEvent( Native.AVPPluginEvent.PlayerSetup, m_iPlayerIndex );
//            }
//        }

//        public void SetOptions(bool useFastOesPath)
//        {
//            m_UseFastOesPath = useFastOesPath;
//            if (m_Video != null)
//            {
//                m_Video.Call("SetPlayerOptions", m_UseFastOesPath);
//            }
//        }

//        public override string GetVersion()
//        {
//            return s_Version;
//        }

//        public override bool OpenVideoFromFile(string path, long offset)
//        {
//            bool bReturn = false;


//            if( m_Video != null )
//            {
//#if UNITY_5
//                Debug.Assert(m_Width == 0 && m_Height == 0 && m_DurationMs == 0.0f);
//#endif

//                bReturn = m_Video.Call<bool>("OpenVideoFromFile", path, offset);
//            }

//            return bReturn;
//        }

//        public override void CloseVideo()
//        {
//            if (m_Texture != null)
//            {
//                Texture2D.Destroy(m_Texture);
//                m_Texture = null;
//            }
//            m_TextureHandle = 0;

//            m_DurationMs = 0.0f;
//            m_Width = 0;
//            m_Height = 0;

//            _lastError = ErrorCode.None;

//            m_Video.Call("CloseVideo");
//        }

//        public override void SetLooping( bool bLooping )
//        {
//            if( m_Video != null )
//            {
//                m_Video.Call("SetLooping", bLooping);
//            }
//        }

//        public override bool IsLooping()
//        {
//            bool result = false;
//            if( m_Video != null )
//            {
//                result = m_Video.Call<bool>("IsLooping");
//            }
//            return result;
//        }

//        public override bool HasVideo()
//        {
//            bool result = false;
//            if( m_Video != null )
//            {
//                result = m_Video.Call<bool>("HasVideo");
//            }
//            return result;
//        }

//        public override bool HasAudio()
//        {
//            bool result = false;
//            if( m_Video != null )
//            {
//                result = m_Video.Call<bool>("HasAudio");
//            }
//            return result;
//        }

//        public override bool HasMetaData()
//        {
//            bool result = false;
//            if( m_DurationMs > 0.0f )
//            {
//                result = true;

//                if( HasVideo() )
//                {
//                    result = ( m_Width > 0 && m_Height > 0 );
//                }
//            }
//            return result;
//        }

//        public override bool CanPlay()
//        {
//            bool result = false;
//#if DLL_METHODS
//            result = Native._CanPlay( m_iPlayerIndex );
//#else
//            if (m_Video != null)
//            {
//                result = m_Video.Call<bool>("CanPlay");
//            }
//#endif
//            return result;
//        }

//        public override void Play()
//        {
//            if (m_Video != null)
//            {
//                m_Video.Call("Play");
//            }
//        }

//        public override void Pause()
//        {
//            if (m_Video != null)
//            {
//                m_Video.Call("Pause");
//            }
//        }

//        public override void Stop()
//        {
//            if (m_Video != null)
//            {
//                // On Android we never need to actually Stop the playback, pausing is fine
//                m_Video.Call("Pause");
//            }
//        }

//        public override void Rewind()
//        {
//            Seek( 0.0f );
//        }

//        public override void Seek(float timeMs)
//        {
//            if (m_Video != null)
//            {
//                m_Video.Call("Seek", Mathf.FloorToInt(timeMs));
//            }
//        }

//        public override void SeekFast(float timeMs)
//        {
//            Seek( timeMs );
//        }

//        public override float GetCurrentTimeMs()
//        {
//            float result = 0.0f;
//            if (m_Video != null)
//            {
//                result = (float)m_Video.Call<long>("GetCurrentTimeMs");
//            }
//            return result;
//        }

//        public override void SetPlaybackRate(float rate)
//        {
//            if (m_Video != null)
//            {
//                m_Video.Call("SetPlaybackRate", rate);
//            }
//        }

//        public override float GetPlaybackRate()
//        {
//            float result = 0.0f;
//            if (m_Video != null)
//            {
//                result = m_Video.Call<float>("GetPlaybackRate");
//            }
//            return result;
//        }

//        public override float GetDurationMs()
//        {
//            return m_DurationMs;
//        }

//        public override int GetVideoWidth()
//        {
//            return m_Width;
//        }
			
//        public override int GetVideoHeight()
//        {
//            return m_Height;
//        }

//        public override float GetVideoFrameRate()
//        {
//            float result = 0.0f;
//            if( m_Video != null )
//            {
//                result = m_Video.Call<float>("GetSourceVideoFrameRate");
//            }
//            return result;
//        }

//        public override float GetBufferingProgress()
//        {
//            float result = 0.0f;
//            if( m_Video != null )
//            {
//                result = m_Video.Call<float>("GetBufferingProgressPercent") * 0.01f;
//            }
//            return result;
//        }

//        public override float GetVideoDisplayRate()
//        {
//            float result = 0.0f;
//#if DLL_METHODS
//            result = Native._GetVideoDisplayRate( m_iPlayerIndex );
//#else
//            if (m_Video != null)
//            {
//                result = m_Video.Call<float>("GetDisplayRate");
//            }
//#endif
//            return result;
//        }

//        public override bool IsSeeking()
//        {
//            bool result = false;
//            if (m_Video != null)
//            {
//                result = m_Video.Call<bool>("IsSeeking");
//            }
//            return result;
//        }

//        public override bool IsPlaying()
//        {
//            bool result = false;
//            if (m_Video != null)
//            {
//                result = m_Video.Call<bool>("IsPlaying");
//            }
//            return result;
//        }

//        public override bool IsPaused()
//        {
//            bool result = false;
//            if (m_Video != null)
//            {
//                result = m_Video.Call<bool>("IsPaused");
//            }
//            return result;
//        }

//        public override bool IsFinished()
//        {
//            bool result = false;
//            if (m_Video != null)
//            {
//                result = m_Video.Call<bool>("IsFinished");
//            }
//            return result;
//        }

//        public override bool IsBuffering()
//        {
//            bool result = false;
//            if (m_Video != null)
//            {
//                result = m_Video.Call<bool>("IsBuffering");
//            }
//            return result;
//        }

//        public override Texture GetTexture( int index )
//        {
//            Texture result = null;
//            if (GetTextureFrameCount() > 0)
//            {
//                result = m_Texture;
//            }
//            return result;
//        }

//        public override int GetTextureFrameCount()
//        {
//            int result = 0;
//#if DLL_METHODS
//            result = Native._GetFrameCount( m_iPlayerIndex );
//#else
//            if (m_Video != null)
//            {
//                result = m_Video.Call<int>("GetFrameCount");
//            }
//#endif
//            return result;
//        }

//        public override bool RequiresVerticalFlip()
//        {
//            return false;
//        }

//        public override void MuteAudio(bool bMuted)
//        {
//            if (m_Video != null)
//            {
//                m_Video.Call("MuteAudio", bMuted);
//            }
//        }

//        public override bool IsMuted()
//        {
//            bool result = false;
//            if( m_Video != null )
//            {
//                result = m_Video.Call<bool>("IsMuted");
//            }
//            return result;
//        }

//        public override void SetVolume(float volume)
//        {
//            if (m_Video != null)
//            {
//                m_Video.Call("SetVolume", volume);
//            }
//        }

//        public override float GetVolume()
//        {
//            float result = 0.0f;
//            if( m_Video != null )
//            {
//                result = m_Video.Call<float>("GetVolume");
//            }
//            return result;
//        }

//        public override int GetAudioTrackCount()
//        {
//            int result = 0;
//            if( m_Video != null )
//            {
//                result = m_Video.Call<int>("GetNumberAudioTracks");
//            }
//            return result;
//        }

//        public override int GetCurrentAudioTrack()
//        {
//            int result = 0;
//            if( m_Video != null )
//            {
//                result = m_Video.Call<int>("GetCurrentAudioTrackIndex");
//            }
//            return result;
//        }

//        public override void SetAudioTrack( int index )
//        {
//            if( m_Video != null )
//            {
//                m_Video.Call("SetAudioTrack", index);
//            }
//        }

//        public override string GetCurrentAudioTrackId()
//        {
//            string id = "";
//            if( m_Video != null )
//            {
//                id = m_Video.Call<string>("GetCurrentAudioTrackId");
//            }
//            return id;
//        }

//        public override int GetCurrentAudioTrackBitrate()
//        {
//            int result = 0;
//            if( m_Video != null )
//            {
//                result = m_Video.Call<int>("GetCurrentAudioTrackIndex");
//            }
//            return result;		}

//        public override int GetVideoTrackCount()
//        {
//            int result = 0;
//            if( m_Video != null )
//            {
//                result = m_Video.Call<int>("GetNumberVideoTracks");
//            }
//            return result;
//        }

//        public override int GetCurrentVideoTrack()
//        {
//            int result = 0;
//            if( m_Video != null )
//            {
//                result = m_Video.Call<int>("GetCurrentVideoTrackIndex");
//            }
//            return result;
//        }

//        public override void SetVideoTrack( int index )
//        {
//            if( m_Video != null )
//            {
//                m_Video.Call("SetVideoTrack", index);
//            }
//        }

//        public override string GetCurrentVideoTrackId()
//        {
//            string id = "";
//            if( m_Video != null )
//            {
//                id = m_Video.Call<string>("GetCurrentVideoTrackId");
//            }
//            return id;
//        }

//        public override int GetCurrentVideoTrackBitrate()
//        {
//            int bitrate = 0;
//            if( m_Video != null )
//            {
//                bitrate = m_Video.Call<int>("GetCurrentVideoTrackBitrate");
//            }
//            return bitrate;
//        }

//        public override long GetTextureTimeStamp()
//        {
//            long timeStamp = long.MinValue;
//            if (m_Video != null)
//            {
//                timeStamp = m_Video.Call<long>("GetTextureTimeStamp");
//            }
//            return timeStamp;
//        }

//        public override void Render()
//        {
//            if (m_Video != null)
//            {
//                //GL.InvalidateState();
//                AndroidMediaPlayer.IssuePluginEvent( Native.AVPPluginEvent.PlayerUpdate, m_iPlayerIndex );
//                //GL.InvalidateState();

//                // Check if we can create the texture
//                // Scan for a change in resolution
//                int newWidth = -1;
//                int newHeight = -1;
//                if (m_Texture != null)
//                {
//#if DLL_METHODS
//                    newWidth = Native._GetWidth( m_iPlayerIndex );
//                    newHeight = Native._GetHeight( m_iPlayerIndex );
//#else
//                    newWidth = m_Video.Call<int>("GetWidth");
//                    newHeight = m_Video.Call<int>("GetHeight");
//#endif
//                    if (newWidth != m_Width || newHeight != m_Height)
//                    {
//                        m_Texture = null;
//                        m_TextureHandle = 0;
//                    }
//                }
//#if DLL_METHODS
//                int textureHandle = Native._GetTextureHandle( m_iPlayerIndex );
//#else
//                int textureHandle = m_Video.Call<int>("GetTextureHandle");
//#endif
//                if (textureHandle > 0 && textureHandle != m_TextureHandle )
//                {
//                    // Already got? (from above)
//                    if( newWidth == -1 || newHeight == -1 )
//                    {
//#if DLL_METHODS
//                        newWidth = Native._GetWidth( m_iPlayerIndex );
//                        newHeight = Native._GetHeight( m_iPlayerIndex );
//#else
//                        newWidth = m_Video.Call<int>("GetWidth");
//                        newHeight = m_Video.Call<int>("GetHeight");
//#endif
//                    }

//                    if (Mathf.Max(newWidth, newHeight) > SystemInfo.maxTextureSize)
//                    {
//                        m_Width = newWidth;
//                        m_Height = newHeight;
//                        m_TextureHandle = textureHandle;
//                        Debug.LogError("[AVProVideo] Video dimensions larger than maxTextureSize");
//                    }
//                    else if( newWidth > 0 && newHeight > 0 )
//                    {
//                        m_Width = newWidth;
//                        m_Height = newHeight;
//                        m_TextureHandle = textureHandle;

//                        _playerDescription = "MediaPlayer";
//                        Helper.LogInfo("Using playback path: " + _playerDescription + " (" + m_Width + "x" + m_Height + "@" + GetVideoFrameRate().ToString("F2") + ")");

//                        // NOTE: From Unity 5.4.x when using OES textures, an error "OPENGL NATIVE PLUG-IN ERROR: GL_INVALID_OPERATION: Operation illegal in current state" will be logged.
//                        // We assume this is because we're passing in TextureFormat.RGBA32 which isn't the true texture format.  This error should be safe to ignore.
//                        m_Texture = Texture2D.CreateExternalTexture(m_Width, m_Height, TextureFormat.RGBA32, false, false, new System.IntPtr(textureHandle));
//                        if (m_Texture != null)
//                        {
//                            ApplyTextureProperties(m_Texture);
//                        }
//                        Helper.LogInfo("Texture ID: " + textureHandle);
//                    }
//                }

//#if UNITY_5_4_OR_NEWER
//                // In Unity 5.4.2 and above the vidoe texture turns black when changing the TextureQuality in the Quality Settings
//                // The code below gets around this issue.  A bug report has been sent to Unity.  So far we have tested and replicated the
//                // "bug" in Windows only, but a user has reported it in Android too
//                if (m_Texture != null && textureHandle > 0 && m_Texture.GetNativeTexturePtr() == System.IntPtr.Zero)
//                {
//                    //Debug.Log("RECREATING");
//                    m_Texture.UpdateExternalTexture(new System.IntPtr(textureHandle));
//                }
//#endif

//                if( m_DurationMs == 0.0f )
//                {
//#if DLL_METHODS
//                    m_DurationMs = (float)( Native._GetDuration( m_iPlayerIndex ) );
//#else
//                    m_DurationMs = (float)(m_Video.Call<long>("GetDurationMs"));
//#endif
////					if( m_DurationMs > 0.0f ) { Helper.LogInfo("Duration: " + m_DurationMs); }
//                }
//            }
//        }

//        protected override void ApplyTextureProperties(Texture texture)
//        {
//            // NOTE: According to OES_EGL_image_external: For external textures, the default min filter is GL_LINEAR and the default S and T wrap modes are GL_CLAMP_TO_EDGE
//            // See https://www.khronos.org/registry/gles/extensions/OES/OES_EGL_image_external_essl3.txt
//            if (!m_UseFastOesPath)
//            {
//                base.ApplyTextureProperties(texture);
//            }
//        }

//        public override void Update()
//        {
//            if (m_Video != null)
//            {
////				_lastError = (ErrorCode)( m_Video.Call<int>("GetLastErrorCode") );
//                _lastError = (ErrorCode)( Native._GetLastErrorCode( m_iPlayerIndex) );
//            }
//        }
		
//        public override void Dispose()
//        {
//            //Debug.LogError("DISPOSE");

//            // Deinitialise player (replaces call directly as GL textures are involved)
//            AndroidMediaPlayer.IssuePluginEvent( Native.AVPPluginEvent.PlayerDestroy, m_iPlayerIndex );

//            if (m_Video != null)
//            {
//                m_Video.Call("SetDeinitialiseFlagged");

//                m_Video.Dispose();
//                m_Video = null;
//            }

//            if (m_Texture != null)
//            {
//                Texture2D.Destroy(m_Texture);
//                m_Texture = null;
//            }
//        }
	
	
	
//        private struct Native
//        {
//            [DllImport ("AVProLocal")]
//            public static extern IntPtr GetRenderEventFunc();

//            [DllImport ("AVProLocal")]
//            public static extern int _GetWidth( int iPlayerIndex );

//            [DllImport ("AVProLocal")]
//            public static extern int _GetHeight( int iPlayerIndex );
			
//            [DllImport ("AVProLocal")]
//            public static extern int _GetTextureHandle( int iPlayerIndex );

//            [DllImport ("AVProLocal")]
//            public static extern long _GetDuration( int iPlayerIndex );

//            [DllImport ("AVProLocal")]
//            public static extern int _GetLastErrorCode( int iPlayerIndex );

//            [DllImport ("AVProLocal")]
//            public static extern int _GetFrameCount( int iPlayerIndex );
		
//            [DllImport ("AVProLocal")]
//            public static extern float _GetVideoDisplayRate( int iPlayerIndex );

//            [DllImport ("AVProLocal")]
//            public static extern bool _CanPlay( int iPlayerIndex );
			
//            public enum AVPPluginEvent
//            {
//                Nop,
//                PlayerSetup,
//                PlayerUpdate,
//                PlayerDestroy,
//            }
//        }
//    }
//}
//#endif