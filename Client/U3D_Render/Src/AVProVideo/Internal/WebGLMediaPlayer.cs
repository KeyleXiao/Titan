﻿//#if UNITY_WEBGL
//using UnityEngine;
//using System.Runtime.InteropServices;
//using System;

////-----------------------------------------------------------------------------
//// Copyright 2015-2016 RenderHeads Ltd.  All rights reserverd.
////-----------------------------------------------------------------------------

//namespace RenderHeads.Media.AVProVideo
//{
//    public sealed class WebGLMediaPlayer : BaseMediaPlayer
//    {
//        //private enum AVPPlayerStatus
//        //{
//        //    Unknown,
//        //    ReadyToPlay,
//        //    Playing,
//        //    Finished,
//        //    Seeking,
//        //    Failed
//        //}

//        [DllImport("__Internal")]
//        private static extern bool AVPPlayerInsertVideoElement(string path, int[] idValues);

//        [DllImport("__Internal")]
//        private static extern int AVPPlayerWidth(int player);

//        [DllImport("__Internal")]
//        private static extern int AVPPlayerHeight(int player);

//        [DllImport("__Internal")]
//        private static extern int AVPPlayerAudioTrackCount(int player);

//        [DllImport("__Internal")]
//        private static extern bool AVPPlayerSetAudioTrack(int player, int index);

//        [DllImport("__Internal")]
//        private static extern bool AVPPlayerOpenFile(int player, string path);

//        [DllImport("__Internal")]
//        private static extern void AVPPlayerClose(int player);

//        [DllImport("__Internal")]
//        private static extern bool AVPPlayerReady(int player);

//        [DllImport("__Internal")]
//        private static extern void AVPPlayerSetLooping(int player, bool loop);

//        [DllImport("__Internal")]
//        private static extern bool AVPPlayerIsLooping(int player);

//        [DllImport("__Internal")]
//        private static extern bool AVPPlayerIsSeeking(int player);

//        [DllImport("__Internal")]
//        private static extern bool AVPPlayerIsPlaying(int player); 

//        [DllImport("__Internal")]
//        private static extern bool AVPPlayerIsPaused(int player);

//        [DllImport("__Internal")]
//        private static extern bool AVPPlayerIsFinished(int player);

//        [DllImport("__Internal")]
//        private static extern bool AVPPlayerIsBuffering(int player);

//        [DllImport("__Internal")]
//        private static extern void AVPPlayerPlay(int player);

//        [DllImport("__Internal")]
//        private static extern void AVPPlayerPause(int player);

//        [DllImport("__Internal")]
//        private static extern void AVPPlayerSeekToTime(int player, float timeMS, bool fast);

//        [DllImport("__Internal")]
//        private static extern float AVPPlayerGetCurrentTime(int player);

//        [DllImport("__Internal")]
//        private static extern float AVPPlayerGetPlaybackRate(int player);

//        [DllImport("__Internal")]
//        private static extern void AVPPlayerSetPlaybackRate(int player, float rate);

//        [DllImport("__Internal")]
//        private static extern void AVPPlayerSetMuted(int player, bool muted);

//        [DllImport("__Internal")]
//        private static extern float AVPPlayerGetDuration(int player);

//        [DllImport("__Internal")]
//        private static extern bool AVPPlayerIsMuted(int player);

//        [DllImport("__Internal")]
//        private static extern float AVPPlayerGetVolume(int player);

//        [DllImport("__Internal")]
//        private static extern void  AVPPlayerSetVolume(int player, float volume);

//        [DllImport("__Internal")]
//        private static extern bool AVPPlayerHasVideo(int player);

//        [DllImport("__Internal")]
//        private static extern bool AVPPlayerHasAudio(int player);

//        // Need jslib
//        [DllImport("__Internal")]
//        private static extern void AVPPlayerFetchVideoTexture(int player, IntPtr texture);

//        [DllImport("__Internal")]
//        private static extern int AVPPlayerGetDecodedFrameCount(int player);

//        [DllImport("__Internal")]
//        private static extern bool AVPPlayerHasMetadata(int player);

//        [DllImport("__Internal")]
//        private static extern int AVPPlayerUpdatePlayerIndex(int id);        

//        private int _playerIndex = -1;
//        private int _playerID = -1;
//        private Texture2D _texture = null;
//        private int _width = 0;
//        private int _height = 0;
//        private int _audioTrackCount = 0;
//        private int _audioTrackIndex = 0;

//        private int _lastFrameCount = 0;
//        private float _displayRateTimer = 0f;
//        private float _displayRate = 0f;

//        public static void InitialisePlatform()
//        {
//        }

//        public override string GetVersion()
//        {
//            return "1.4.9";
//        }

//        public override bool OpenVideoFromFile(string path, long offset)
//        {
//            bool result = false;

//            if (path.StartsWith("http://") || path.StartsWith("https://") || path.StartsWith("file://"))
//            {
//                int[] idValues = new int[2];
//                idValues[0] = -1;
//                AVPPlayerInsertVideoElement(path, idValues);
//                {
//                    int playerIndex = idValues[0];
//                    _playerID = idValues[1];

//                    if (playerIndex > -1)
//                    {
//                        _playerIndex = playerIndex;
//                        result = true;
//                    }				
//                }
//            }

//            return result;
//        }

//        public override void CloseVideo()
//        {
//            if (_playerIndex != -1)
//            {
//                Pause();

//                _width = 0;
//                _height = 0;
//                _audioTrackCount = 0;
//                _audioTrackIndex = 0;

//                AVPPlayerClose(_playerIndex);

//                if (_texture != null)
//                {
//                    // Have to update with zero to release Metal textures!
//                    //_texture.UpdateExternalTexture(0);
//                    Texture2D.Destroy(_texture);
//                    _texture = null;
//                }

//                _playerIndex = -1;
//                _playerID = -1;
//            }
//        }

//        public override bool IsLooping()
//        {
//            //Debug.Assert(_player != -1, "no player IsLooping");
//            bool result = false;

//            if (_playerIndex != -1)
//            {
//                result = AVPPlayerIsLooping(_playerIndex);
//            }

//            return result;
//        }

//        public override void SetLooping(bool looping)
//        {
//            Debug.Assert(_playerIndex != -1, "no player SetLooping");

//            AVPPlayerSetLooping(_playerIndex, looping);
//        }

//        public override bool HasAudio()
//        {
//            //Debug.Assert(_player != -1, "no player HasAudio");
//            bool result = false;

//            if (_playerIndex != -1)
//            {
//                result = AVPPlayerHasAudio(_playerIndex);
//            }

//            return result;
//        }

//        public override bool HasVideo()
//        {
//            //Debug.Assert(_player != -1, "no player HasVideo");
//            bool result = false;

//            if (_playerIndex != -1)
//            {
//                result = AVPPlayerHasVideo(_playerIndex);
//            }

//            return result;
//        }

//        public override bool HasMetaData()
//        {
//            //Debug.Assert(_player != -1, "no player HasMetaData");
//            bool result = false;

//            if (_playerIndex != -1)
//            {
//                result = AVPPlayerHasMetadata(_playerIndex);
//            }

//            return result;
//        }

//        public override bool CanPlay()
//        {
//            //Debug.Assert(_player != -1, "no player CanPlay");
//            bool result = false;

//            if (_playerIndex != -1)
//            {
//                result = AVPPlayerReady(_playerIndex);
//            }

//            return result;
//        }

//        public override void Play()
//        {
//            Debug.Assert(_playerIndex != -1, "no player Play");

//            AVPPlayerPlay(_playerIndex);
//        }

//        public override void Pause()
//        {
//            Debug.Assert(_playerIndex != -1, "no player Pause");

//            AVPPlayerPause(_playerIndex);
//        }

//        public override void Stop()
//        {
//            Debug.Assert(_playerIndex != -1, "no player Stop");

//            AVPPlayerPause(_playerIndex);
//        }

//        public override void Rewind()
//        {
//            Debug.Assert(_playerIndex != -1, "no player Rewind");

//            AVPPlayerSeekToTime(_playerIndex, 0.0f, true);
//        }

//        public override void Seek(float ms)
//        {
//            Debug.Assert(_playerIndex != -1, "no player Seek");

//            AVPPlayerSeekToTime(_playerIndex, ms * 0.001f, false);
//        }

//        public override void SeekFast(float ms)
//        {
//            Debug.Assert(_playerIndex != -1, "no player SeekFast");

//            AVPPlayerSeekToTime(_playerIndex, ms * 0.001f, true);
//        }

//        public override float GetCurrentTimeMs()
//        {
//            //Debug.Assert(_player != -1, "no player GetCurrentTimeMs");
//            float result = 0.0f;

//            if (_playerIndex != -1)
//            {
//                result = (AVPPlayerGetCurrentTime(_playerIndex) * 1000.0f);
//                //Debug.Log("CurrentTime C#: " + result);
//            }

//            return result;
//        }

//        public override void SetPlaybackRate(float rate)
//        {
//            Debug.Assert(_playerIndex != -1, "no player SetPlaybackRate");

//            // No HTML implementations allow negative rate yet
//            rate = Mathf.Max(0f, rate);

//            AVPPlayerSetPlaybackRate(_playerIndex, rate);
//        }

//        public override float GetPlaybackRate()
//        {
//            //Debug.Assert(_player != -1, "no player GetPlaybackRate");
//            float result = 0.0f;

//            if (_playerIndex != -1)
//            {
//                result = AVPPlayerGetPlaybackRate(_playerIndex);
//            }

//            return result;
//        }

//        public override float GetDurationMs()
//        {
//            //Debug.Assert(_player != -1, "no player GetDurationMs");
//            float result = 0.0f;

//            if (_playerIndex != -1)
//            {
//                result = (AVPPlayerGetDuration(_playerIndex) * 1000.0f);
//            }

//            return result;
//        }

//        public override int GetVideoWidth()
//        {
//            if (_width == 0)
//            {
//                _width = AVPPlayerWidth(_playerIndex);
//            }
//            return _width;
//        }

//        public override int GetVideoHeight()
//        {
//            if (_height == 0)
//            {
//                _height = AVPPlayerHeight(_playerIndex);
//            }
//            return _height;
//        }

//        public override float GetVideoFrameRate()
//        {
//            // There is no way in HTML5 yet to get the frame rate of the video
//            return 0f;
//        }

//        public override float GetVideoDisplayRate()
//        {
//            return _displayRate;
//        }

//        public override bool IsSeeking()
//        {
//            //Debug.Assert(_player != -1, "no player IsSeeking");
//            bool result = false;

//            if (_playerIndex != -1)
//            {
//                result = AVPPlayerIsSeeking(_playerIndex);
//            }

//            return result;
//        }

//        public override bool IsPlaying()
//        {
//            //Debug.Assert(_player != -1, "no player IsPlaying");
//            bool result = false;

//            if (_playerIndex != -1)
//            {
//                result = AVPPlayerIsPlaying(_playerIndex);
//            }

//            return result;
//        }

//        public override bool IsPaused()
//        {
//            //Debug.Assert(_player != -1, "no player IsPaused");
//            bool result = false;

//            if (_playerIndex != -1)
//            {
//                result = AVPPlayerIsPaused(_playerIndex);
//            }

//            return result;
//        }

//        public override bool IsFinished()
//        {
//            //Debug.Assert(_player != -1, "no player IsFinished");
//            bool result = false;

//            if (_playerIndex != -1)
//            {
//                result = AVPPlayerIsFinished(_playerIndex);
//            }

//            return result;
//        }

//        public override bool IsBuffering()
//        {
//            //Debug.Assert(_player != -1, "no player IsBuffering");
//            bool result = false;

//            if (_playerIndex != -1)
//            {
//                result = AVPPlayerIsBuffering(_playerIndex);
//            }

//            return result;
//        }

//        public override Texture GetTexture( int index )
//        {
//            return _texture;
//        }

//        public override int GetTextureFrameCount()
//        {
//            //Debug.Assert(_player != -1, "no player GetTextureFrameCount");
//            int result = 0;

//            if (_playerIndex != -1)
//            {
//                result = AVPPlayerGetDecodedFrameCount(_playerIndex);
//            }

//            return result;
//        }

//        public override bool RequiresVerticalFlip()
//        {
//            return true;
//        }

//        public override bool IsMuted()
//        {
//            //Debug.Assert(_player != -1, "no player IsMuted");
//            bool result = false;

//            if (_playerIndex != -1)
//            {
//                result = AVPPlayerIsMuted(_playerIndex);
//            }

//            return result;
//        }

//        public override void MuteAudio(bool bMute)
//        {
//            Debug.Assert(_playerIndex != -1, "no player MuteAudio");

//            AVPPlayerSetMuted(_playerIndex, bMute);
//        }

//        public override void SetVolume(float volume)
//        {
//            Debug.Assert(_playerIndex != -1, "no player SetVolume");

//            AVPPlayerSetVolume(_playerIndex, volume);
//        }

//        public override float GetVolume()
//        {
//            //Debug.Assert(_player != -1, "no player GetVolume");
//            float result = 0.0f;

//            if (_playerIndex != -1)
//            {
//                result = AVPPlayerGetVolume(_playerIndex);
//            }

//            return result;
//        }

//        public override void Render()
//        {
            
//        }

//        public override void Update()
//        {
//            if(_playerID > -1) // CheckPlayer's index and update it
//            {
//                _playerIndex = AVPPlayerUpdatePlayerIndex(_playerID);
//            }

//            if(_playerIndex > -1)
//            {
//                if (_texture == null)
//                {
//                    _texture = new Texture2D(128, 128, TextureFormat.ARGB32, false);
//                    _texture.wrapMode = TextureWrapMode.Clamp;
//                    ApplyTextureProperties(_texture);
//                }

//                if (AVPPlayerReady(_playerIndex))
//                {
//                    _width = AVPPlayerWidth(_playerIndex);
//                    _height = AVPPlayerHeight(_playerIndex);
//                    if (AVPPlayerHasAudio(_playerIndex))
//                    {
//                        _audioTrackCount = Mathf.Max(1, AVPPlayerAudioTrackCount(_playerIndex));
//                    }

//                    if (_texture.width != _width || _texture.height != _height)
//                    {
//                        _texture.Resize(_width, _height, TextureFormat.ARGB32, false);
//                        _texture.Apply();
//                    }

//                    AVPPlayerFetchVideoTexture(_playerIndex, _texture.GetNativeTexturePtr());

//                    UpdateDisplayFrameRate();
//                }
//            } 
//        }

//        private void UpdateDisplayFrameRate()
//        {
//            _displayRateTimer += Time.deltaTime;
//            if (_displayRateTimer >= 0.5f)
//            {
//                int frameCount = AVPPlayerGetDecodedFrameCount(_playerIndex);
//                int frames = (frameCount - _lastFrameCount);
//                if (frames > 0)
//                {
//                    _displayRate = (float)frames / _displayRateTimer;
//                }
//                else
//                {
//                    _displayRate = 0f;
//                }
//                _displayRateTimer = 0f;
//                _lastFrameCount = frameCount;
//            }
//        }

//        public override void Dispose()
//        {
//            CloseVideo();
//        }

//        public override int GetAudioTrackCount()
//        {
//            return _audioTrackCount;
//        }

//        public override int GetCurrentAudioTrack()
//        {
//            return _audioTrackIndex;
//        }

//        public override void SetAudioTrack(int index)
//        {
//            if (_playerIndex > -1)
//            {
//                if (index >= 0 && index < _audioTrackCount)
//                {
//                    if (index != _audioTrackIndex)
//                    {
//                        AVPPlayerSetAudioTrack(_playerIndex, index);
//                        _audioTrackIndex = index;
//                    }
//                }
//            }
//        }

//        public override string GetCurrentAudioTrackId()
//        {
//            // TODO
//            return "";
//        }

//        public override int GetCurrentAudioTrackBitrate()
//        {
//            // TODO
//            return 0;
//        }

//        public override int GetVideoTrackCount()
//        {
//            // TODO
//            return 0;
//        }

//        public override int GetCurrentVideoTrack()
//        {
//            // TODO
//            return 0;
//        }

//        public override void SetVideoTrack( int index )
//        {
//            // TODO
//        }

//        public override string GetCurrentVideoTrackId()
//        {
//            // TODO
//            return "";
//        }

//        public override int GetCurrentVideoTrackBitrate()
//        {
//            // TODO
//            return 0;
//        }

//        public override float GetBufferingProgress()
//        {
//            // TODO
//            return 0f;
//        }
//    }
//}
//#endif