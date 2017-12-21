using System;
using System.Runtime.InteropServices;
using U3D_Render;
using UnityEngine.UI;
using UnityEngine;
using System.Collections.Generic;
[RequireComponent(typeof(Image))]
public class VideoAnimationSprite : MonoBehaviour, IVideoControl
{
    [System.NonSerialized]
    public bool isLoop = false;

    public float LoadingProcess
    {
        get
        {
            return m_LoadingProgress;
        }
    }

    [System.NonSerialized]
    private float m_LoadingProgress = 0.0f;



    private string AssetBundleName = string.Empty;
    private string AssetBaseName = string.Empty;
    private int StartIndex = 0;
    private int SpriteCount = 0;

    private int fps = 30;

    private bool isPlaying = false;

    private List<Sprite> m_SpriteList = new List<Sprite>();
    private Image m_VideoRender = null;
    private List<ResNode> currSpriteResNode = new List<ResNode>();
    private HashSet<int> currLoadingSprite = new HashSet<int>();

    private int preLoadSpriteCount = 1;
    private int realyPreLoadCount = 0;

    private float fpsIntevel = 0.0f;
    private float fpsTimmer = 0.0f;
    private int currentFrame = 0;

    public void Init(string AssetBundleName, string AssetBaseName, int SpriteCount, int StartIndex = 0, int fps = 30)
    {
        m_VideoRender = GetComponent<Image>();

        this.AssetBundleName = AssetBundleName;
        this.AssetBaseName = AssetBaseName;
        this.SpriteCount = SpriteCount;
        this.StartIndex = StartIndex;
        this.fps = fps;

        ResetPlayStatus();
        PreLoadCacheSprite();
        SwitchSprite(0);
    }

    public void ChangeSize(Vector2 size)
    {
        if(m_VideoRender)
        {
            m_VideoRender.rectTransform.sizeDelta = size;
        }
    }

    /// <summary>
    /// 如果当前帧没有加载完，则会强制加载
    /// </summary>
    /// <param name="frame"></param>
    public void GoToAndStop(int frame)
    {
        if (!IsValidSpiteSettings())
        {
            return;
        }

        Stop();
        LoadSprite(frame);
        SwitchSprite(frame);
    }

    /// <summary>
    /// 如果当前帧没有加载完，则会强制加载
    /// </summary>
    public void GoToAndPlay(int frame)
    {
        if (!IsValidSpiteSettings())
        {
            return;
        }

        Stop();
        LoadSprite(frame);
        SwitchSprite(frame);
        Play();
    }

    public void Play()
    {
        if(!IsValidSpiteSettings())
        {
            return;
        }
        isPlaying = true;
    }

    public void Stop()
    {
        if (!IsValidSpiteSettings())
        {
            return;
        }
        ResetPlayStatus();
    }

    public void Pause()
    {
        if (!IsValidSpiteSettings())
        {
            return;
        }
        isPlaying = false;
    }

    public void ResLoadCallBack(ResConfigData cof, ResNode res,System.Object UserDataObj)
    {
        if (!IsValidSpiteSettings())
        {
            return;
        }

        if (null != res)
        {
            int index = (int)UserDataObj;
            if(index < currSpriteResNode.Count)
            {
                currSpriteResNode[index] = res;
                m_SpriteList[index] = (res.ConvertRes<Sprite>());

                currLoadingSprite.Remove(index);
                int temp = Mathf.Max(0, SpriteCount - realyPreLoadCount);
                if (temp <= 0)
                {
                    m_LoadingProgress = 1.01f;
                }
                else
                {
                    float fTemp1 = currLoadingSprite.Count;
                    m_LoadingProgress =( 1 - (fTemp1 / temp)) + 0.01f;
                }
            }            
        }
    }

    private int LimitFrame(int frame)
    {
        if (frame < 0)
        {
            frame = 0;
        }

        if (frame > SpriteCount)
        {
            frame = SpriteCount - 1;
        }
        return frame;
    }

    private bool IsValidSpiteSettings()
    {
        if (fps > 0 && SpriteCount > 0 && !AssetBundleName.Equals(string.Empty) && !AssetBaseName.Equals(string.Empty))
        {
            return true;
        }

        return false;
    }

    private void SwitchSprite(int frame)
    {
        if (!IsValidSpiteSettings())
        {
            return;
        }
        frame = LimitFrame(frame);
        m_VideoRender.sprite = m_SpriteList[frame];

    }

    private void PreLoadCacheSprite()
    {
        if (!IsValidSpiteSettings())
        {
            return;
        }

        Clear();

        for(int i = 0;i < SpriteCount;i++)
        {
            currSpriteResNode.Add(null);
            m_SpriteList.Add(USpriteManager.Instance.DefaultSprite);
        }
        realyPreLoadCount = Mathf.Min(preLoadSpriteCount, SpriteCount);

        for (int i = 0; i < realyPreLoadCount; i++)
        {
            LoadSprite(i);
        }

        m_LoadingProgress = 1.01f;
      
        //剩下的再慢慢异步加载，异步加载没有完成的时候，不能播动画
        for(int i = realyPreLoadCount;i < SpriteCount; i++)
        {
            m_LoadingProgress = 0.0f;
            LoadSpriteAsync(i);
        }
    }

    private void LoadSpriteAsync(int index)
    {
        if (!IsValidSpiteSettings())
        {
            return;
        }

        index = LimitFrame(index);
        if (currSpriteResNode[index] != null)
        {
            return;
        }
        string assetName = StringHelper.BuildString(AssetBaseName, StartIndex + index);
        ResNode res = AssetBundleManager.GetAssetsAsync(AssetType.Asset_Sprite, AssetBundleName, assetName,ResLoadCallBack,index);
        if (null != res)
        {
            currSpriteResNode[index] = res;
            m_SpriteList[index] = USpriteManager.Instance.DefaultSprite;
            currLoadingSprite.Add(index);
        }
        else
        {
            Debug.LogWarning(GetType().ToString() + "加载动画Sprite失败，索引:" + assetName);
        }
    }

    private void LoadSprite(int index)
    {
        if (!IsValidSpiteSettings())
        {
            return;
        }

        index = LimitFrame(index);
        if(currSpriteResNode[index] != null)
        {
            return;
        }
        string assetName = StringHelper.BuildString(AssetBaseName, StartIndex + index);
        ResNode res = AssetBundleManager.GetAssets(AssetType.Asset_Sprite, AssetBundleName, assetName);
        if (null != res)
        {
            currSpriteResNode[index] = res;
            m_SpriteList[index] = (res.ConvertRes<Sprite>());
        }
    }

    private void ResetPlayStatus()
    {
        if (!IsValidSpiteSettings())
        {
            return;
        }

        fpsIntevel = 1.0f / fps;
        fpsTimmer = 0.0f;
        currentFrame = 0;
        isPlaying = false;
    }

    public void Clear()
    {
        if (!IsValidSpiteSettings())
        {
            return;
        }

        foreach (ResNode res in currSpriteResNode)
        {
            ResNode temp = res;
            if(null == temp)
            {
                continue;
            }
            if(!temp.isAsync)
            {
                AssetBundleManager.DeleteAssets(ref temp, true);
            }
            else
            {
                AssetBundleManager.DeleteAssetsAsync(ref temp, ResLoadCallBack, true);
            }
        }
        currSpriteResNode.Clear();
        m_SpriteList.Clear();
    }

    public void OnDestroy()
    {
        Clear();
    }

    public void LateUpdate()
    {
        if(!isPlaying)
        {
            return;
        }

        if (LoadingProcess < 1.0f)
        {
            return;
        }

        fpsTimmer += Time.deltaTime;
        if(fpsTimmer > fpsIntevel)
        {
            if (currentFrame >= SpriteCount)
            {
                if(isLoop)
                {
                    currentFrame = 0;
                }
                else
                {
                    return;
                }
                
            }
            fpsTimmer = 0.0f;
            SwitchSprite(currentFrame);
            currentFrame++;
        }
    }
}
