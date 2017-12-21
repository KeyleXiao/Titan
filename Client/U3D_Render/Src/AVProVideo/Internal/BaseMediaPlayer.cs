#if UNITY_EDITOR || UNITY_STANDALONE_OSX || UNITY_STANDALONE_WIN
#define UNITY_PLATFORM_SUPPORTS_LINEAR
#endif

using UnityEngine;

//-----------------------------------------------------------------------------
// Copyright 2015-2016 RenderHeads Ltd.  All rights reserved.
//-----------------------------------------------------------------------------

namespace RenderHeads.Media.AVProVideo
{
    public abstract class BaseMediaPlayer : IMediaPlayer, IMediaControl, IMediaInfo, IMediaProducer, System.IDisposable
    {
        public abstract string GetVersion();

        public abstract bool OpenVideoFromFile(string path, long offset);
        public abstract void CloseVideo();

        public abstract void SetLooping(bool bLooping);
        public abstract bool IsLooping();

        public abstract bool HasMetaData();
        public abstract bool CanPlay();
        public abstract void Play();
        public abstract void Pause();
        public abstract void Stop();
        public abstract void Rewind();

        public abstract void Seek(float timeMs);
        public abstract void SeekFast(float timeMs);
        public abstract float GetCurrentTimeMs();

        public abstract float GetPlaybackRate();
        public abstract void SetPlaybackRate(float rate);

        public abstract float GetDurationMs();
        public abstract int GetVideoWidth();
        public abstract int GetVideoHeight();
        public abstract float GetVideoDisplayRate();
        public abstract bool HasAudio();
        public abstract bool HasVideo();

        public abstract bool IsSeeking();
        public abstract bool IsPlaying();
        public abstract bool IsPaused();
        public abstract bool IsFinished();
        public abstract bool IsBuffering();

        public abstract Texture GetTexture(int index = 0);
        public abstract int GetTextureFrameCount();
        public virtual long GetTextureTimeStamp() { return long.MinValue; }
        public abstract bool RequiresVerticalFlip();

        public abstract void MuteAudio(bool bMuted);
        public abstract bool IsMuted();
        public abstract void SetVolume(float volume);
        public abstract float GetVolume();

        public abstract int GetAudioTrackCount();
        public abstract int GetCurrentAudioTrack();
        public abstract void SetAudioTrack(int index);
        public abstract string GetCurrentAudioTrackId();
        public abstract int GetCurrentAudioTrackBitrate();

        public abstract int GetVideoTrackCount();
        public abstract int GetCurrentVideoTrack();
        public abstract void SetVideoTrack(int index);
        public abstract string GetCurrentVideoTrackId();
        public abstract int GetCurrentVideoTrackBitrate();

        public abstract float GetVideoFrameRate();

        public abstract float GetBufferingProgress();

        public abstract void Update();
        public abstract void Render();
        public abstract void Dispose();

        public ErrorCode GetLastError()
        {
            return _lastError;
        }

        public string GetPlayerDescription()
        {
            return _playerDescription;
        }

        public virtual bool PlayerSupportsLinearColorSpace()
        {
#if UNITY_PLATFORM_SUPPORTS_LINEAR
            return true;
#else
            return false;
#endif
        }

        public virtual int GetBufferedTimeRangeCount() { return 0; }
        public virtual bool GetBufferedTimeRange(int index, ref float startTimeMs, ref float endTimeMs) { return false; }

        protected string _playerDescription = string.Empty;
        protected ErrorCode _lastError = ErrorCode.None;
        protected FilterMode _defaultTextureFilterMode = FilterMode.Bilinear;
        protected TextureWrapMode _defaultTextureWrapMode = TextureWrapMode.Clamp;
        protected int _defaultTextureAnisoLevel = 1;

        public void SetTextureProperties(FilterMode filterMode = FilterMode.Bilinear, TextureWrapMode wrapMode = TextureWrapMode.Clamp, int anisoLevel = 0)
        {
            _defaultTextureFilterMode = filterMode;
            _defaultTextureWrapMode = wrapMode;
            _defaultTextureAnisoLevel = anisoLevel;
            ApplyTextureProperties(GetTexture());
        }

        protected virtual void ApplyTextureProperties(Texture texture)
        {
            if (texture != null)
            {
                texture.filterMode = _defaultTextureFilterMode;
                texture.wrapMode = _defaultTextureWrapMode;
                texture.anisoLevel = _defaultTextureAnisoLevel;
            }
        }

        public virtual void GrabAudio(float[] buffer, int floatCount, int channelCount)
        {

        }
    }
}