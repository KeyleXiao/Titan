using System;
using System.Runtime.InteropServices;
using U3D_Render;
using UnityEngine.UI;
using UnityEngine;
using System.Collections.Generic;
public class LoginVideo : MonoBehaviour
{
    public string LoginPath = "texture/ui/dynamic/nonatlases/video/loginvideo.ui";
    public string LoginBaseName = "LoginVideo_";
    public int StartIndex = 0;
    public int SpriteCount = 50;
    public int fps = 30;

    //public Vector2 size = new Vector2(1920, 1080);
    private VideoAnimationSprite m_video = null;
    private bool bInit = false;

    public  void Start()
    {
        Init();


    }

    public void OnDestroy()
    {

        Clear();
    }

    public void Clear()
    {
        if(!bInit)
        {
            return;
        }
        bInit = false;
        m_video.Clear();
        m_video = null;
    }

    public void Play()
    {
        if (!bInit)
        {
            return;
        }
        if (m_video)
        {
            m_video.Play();
        }
    }

    public void Stop()
    {
        if (!bInit)
        {
            return;
        }
        if (m_video)
        {
            m_video.Stop();
        }
    }

    public void Init()
    {
        if(bInit)
        {
            return;
        }
        bInit = true;
        m_video = GetComponent<VideoAnimationSprite>();
        if(!m_video)
        {
            m_video = gameObject.AddComponent<VideoAnimationSprite>();
        }

        m_video.Clear();

        m_video.Init(LoginPath, LoginBaseName, SpriteCount, StartIndex, fps);
        m_video.isLoop = true;
       
        //m_video.ChangeSize(size);
    }
}
