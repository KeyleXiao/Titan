using UnityEngine;
using System.Collections;
using Scaleform;

/// <summary>
/// Movie结点，主要是指MC在Movie上的位置，用来算UV
/// </summary>
public struct MovieNode
{
    /// <summary>
    /// 是否有效
    /// </summary>
    public bool isValid;

    /// <summary>
    /// 在Movie上的起始，像素
    /// </summary>
    public Vector2 start;

    /// <summary>
    /// 在Movie上的结束，像素
    /// </summary>
    public Vector2 end;


    /// <summary>
    /// 大小，像素
    /// </summary>
    public Vector2 vSize;

    public int RowIndex;

    public int ColIndex;
}

public class UIComponent
{

    public Value Instance
    {
        get
        {
            if (!Value.IsValueValid(m_Instance))
                return null;
            return m_Instance;
        }
    }

    protected Value m_Instance = null;

    protected bool m_bEnable = true;

    /// <summary>
    /// 是否可见
    /// </summary>
    protected bool m_bVisible = true;

    /// <summary>
    /// 是否在摄像机范围内
    /// </summary>
    protected bool m_isOnCameraView = true;


    /// <summary>
    /// 是否超出可见距离
    /// </summary>
    protected bool m_isOutOfVisbleDistance = false;

    /// <summary>
    /// 最终是否可见，目前由m_isOnView和m_bVisible决定
    /// </summary>
    protected bool FinalVisible = true;

    protected Vector2 m_Scale = Vector2.one;

    protected Vector2 m_PixPos = Vector2.zero * -2.0f;

    protected float m_Roation = 0.0f;

    protected SFDisplayInfo m_DisplayInfo = null;

    /// <summary>
    /// 是否需要更新DisplayInfo
    /// </summary>
    protected bool bNeedToUpdateDisplayInfo = false;

    public UIComponent(Value Ins)
    {
        m_bEnable = true;
        m_bVisible = true;
        m_isOnCameraView = true;
        FinalVisible = true;
        m_Scale = Vector2.one;
        m_PixPos = Vector2.zero;
        m_Roation = 0.0f;
        m_Instance = Ins;
        m_DisplayInfo = m_Instance.GetDisplayInfo();
    }


    /// <summary>
    /// 缩放，1为原始大小
    /// </summary>
    /// <param name="scaleX">X方向</param>
    /// <param name="scaleY">Y方向</param>
    public virtual void SetScale(float scaleX, float scaleY)
    {
        if (Mathf.Abs(m_Scale.x - scaleX) > 0.0001f ||
            Mathf.Abs(m_Scale.y - scaleY) > 0.0001f)
        {

            m_Scale.x = scaleX;
            m_Scale.y = scaleY;
            m_DisplayInfo.XScale = scaleX * 100;
            m_DisplayInfo.YScale = scaleY * 100;
            bNeedToUpdateDisplayInfo = true;
        }
    }


    public virtual void SetRoation(float deg)
    {
        if (Mathf.Abs(m_Roation - deg) > 0.0001f)
        {
            m_DisplayInfo.Rotation = deg;
            m_Roation = deg;
            bNeedToUpdateDisplayInfo = true;
        }
    }


    public virtual void SetVisible(bool bVisible)
    {
        if (m_bVisible != bVisible)
        {
            m_bVisible = bVisible;
            UpdateVisible();
        }
    }

    public virtual void UpdateVisible()
    {
        bool bVisible = m_bVisible && m_isOnCameraView && !m_isOutOfVisbleDistance;
        if (FinalVisible != bVisible)
        {
            FinalVisible = bVisible;
            m_DisplayInfo.Visible = FinalVisible;
            bNeedToUpdateDisplayInfo = true;
        }

    }

    public virtual void SetEnable(bool bEnable)
    {
        if (m_bEnable != bEnable)
        {
            m_Instance.SetMember("enabled", bEnable);
            m_bEnable = bEnable;
            bNeedToUpdateDisplayInfo = true;
        }
    }


    /// <summary>
    /// 设置位置，这个是直接设置位置，不是从世界坐标转换过来的。(Unity屏幕坐标)
    /// </summary>
    /// <param name="pixelPos">Unity屏幕坐标</param>
    public virtual void SetPositionDirectly_UnityPoint(Vector2 pixelPos)
    {
        //是否在视野中
        m_isOnCameraView = GfxUtil.IsOnView(pixelPos, false);
        m_PixPos = pixelPos;
        if (m_isOnCameraView)
        {
            Vector2 pos = GfxUtil.ScreenPointToStagePoint(pixelPos);
            if (Mathf.Abs(m_PixPos.x - pos.x) > 0.01f ||
                Mathf.Abs(m_PixPos.y - pos.y) > 0.01f)
            {
                m_DisplayInfo.X = pos.x;
                m_DisplayInfo.Y = pos.y;
                bNeedToUpdateDisplayInfo = true;
            }
        }
        
        OnPositionChange();
    }


    /// <summary>
    /// 设置位置，这个是直接设置位置，不是从世界坐标转换过来的。(舞台坐标)
    /// </summary>
    /// <param name="pixelPos">flash屏幕坐标</param>
    public virtual void SetPositionDirectly_StagePoint(Vector2 pixelPos)
    {
        m_DisplayInfo.X = pixelPos.x;
        m_DisplayInfo.Y = pixelPos.y;
        bNeedToUpdateDisplayInfo = true;
    }

    /// <summary>
    /// 设置位置。这个函数的位置是从世界坐标转换过来的屏幕坐标，Z值是有效值
    /// </summary>
    /// <param name="WorldPos">屏幕坐标</param>
    public virtual void SetPositionFromWorldSpace(Vector3 ScreenPos)
    {
        //是否在视野中
        m_isOnCameraView  = GfxUtil.IsOnView(ScreenPos, true);
        m_PixPos = ScreenPos;
        if (m_isOnCameraView)
        {
            Vector2 pos = GfxUtil.ScreenPointToStagePoint(ScreenPos);
            
            if (((Mathf.Abs(m_PixPos.x - pos.x) > 0.01f || Mathf.Abs(m_PixPos.y - pos.y) > 0.01f))
                && ((Mathf.Abs((float)m_DisplayInfo.X - pos.x) > 1.0f || Mathf.Abs((float)m_DisplayInfo.Y - pos.y) > 1.0f)))
            {
                m_DisplayInfo.X = pos.x;
                m_DisplayInfo.Y = pos.y;
                bNeedToUpdateDisplayInfo = true;
            }
        }
        OnPositionChange();
    }

    /// <summary>
    /// 当改变位置
    /// </summary>
    protected virtual void OnPositionChange()
    {
         UpdateVisible();
    }

    public virtual void Clear()
    {
        m_Instance = null;
    }

    public virtual bool GetEnable()
    {
        return m_bEnable;
    }

    public virtual void OnUpdate()
    {
        if (bNeedToUpdateDisplayInfo)
        {
            m_Instance.SetDisplayInfo(m_DisplayInfo);
            bNeedToUpdateDisplayInfo = false;
        }
    }
}
