/*******************************************************************
** 文件名:	FloatFontNode.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/3/23
** 版  本:	1.0
** 描  述:	飘字结点
** 应  用:   飘字功能具体结点
 *
**************************** 修改记录 ******************************
** 修改人: 昔文博
** 日  期: 2016/4/7
** 描  述: 暴击飘字增加左右交替偏移效果
 *
********************************************************************/
using UnityEngine;
using System.Collections;
using Scaleform;
using UnityEngine.UI;



/// <summary>
/// 飘字功能结点
/// </summary>
public sealed class FloatFontNode : UIComponent
{
    #region 逻辑数据

    public bool bFollowEntity = false;

    private string m_text = null;

    private FloatFontType m_type =  FloatFontType.TYPE_ENEMY_DAMGE;

    private float liveTime = 0.0f;
    private int m_Value = -1;
    private bool bNeedToUpdateValue = false;
    private GameObject m_Entity = null;
    private CreatureProperty m_Property = null;
    //下一次飘字是否向左偏移
    private static bool bLeftOffset = false;
    #endregion



    public FloatFontNode(GameObject entity,Value Ins)
        : base(Ins)
    {
        liveTime = 0.0f;
        m_Entity = entity;
        m_Property = entity.GetComponent<CreatureProperty>();
    }

    public void ChangeHostEntity(GameObject entity)
    {
        m_Entity = entity;
        m_Property = entity.GetComponent<CreatureProperty>();
    }

    public void ChangeType(FloatFontType type)
    {
        if (m_type == type)
        {
            return;
        }
        m_type = type;
        bNeedToUpdateValue = true;
    }

    public void SetText(string text)
    {
        if ( string.IsNullOrEmpty(text))
        {
            return;
        }
        m_text = text;
        int val = 0;
        bool flags = int.TryParse(text, out val);
        if (flags)
        {
            m_Value = val;
            bNeedToUpdateValue = true;
        }
        
    }

    public void OnEnabled()
    {
        liveTime = 0.0f;
    }

    public void OnDisabled()
    {
        liveTime = 0.0f;
    }
   
    public override void OnUpdate()
    {
        if (bNeedToUpdateValue)
        {
            m_Instance.Invoke("UpdateValue", m_Value, (int)m_type);
            bNeedToUpdateValue = false;
        }

        if (bNeedToUpdateDisplayInfo)
        {
            m_Instance.SetDisplayInfo(m_DisplayInfo);
            bNeedToUpdateDisplayInfo = false;
        }

        if (!m_bEnable )
        {
            return;
        }
        if (liveTime > FloatFontManager.FloatFontLifeTime[m_type])
        {
            FloatFontManager.DisableFloatNode(this);
            return;
        }
        liveTime += Time.deltaTime;
        if (bFollowEntity)
        {
            UpdatePos();
        }
        
    }

    /// <summary>
    /// 更新位置,如果有CreatureProperty，则会算上高度
    /// </summary>
    public void UpdatePos()
    {
        if (m_Entity)
        {
            Vector3 pos = m_Entity.transform.position;
            
            float creatureHeight = 0.0f;
            if (m_Property)
            {
                creatureHeight = m_Property.CreatureHeightInMeters * m_Property.transform.localScale.y;
            }

            SetPositionFromWorldSpace(GenratePos(m_type, pos, creatureHeight));
        }
    }


    /// <summary>
    /// 计算位置
    /// </summary>
    /// <param name="t">飘字类型</param>
    /// <param name="WorldPos">世界坐标</param>
    /// <param name="CreatureHeightInMeters">实体高度</param>
    /// <returns></returns>
    static Vector3 GenratePos(FloatFontType t,Vector3 WorldPos,float CreatureHeightInMeters)
    {
        Vector3 pixelPos = Vector3.zero;
        //高度超过3m，取3m
        if (CreatureHeightInMeters > 4)
        {
            CreatureHeightInMeters = 4;
        }
        
        switch (t)
        {
            case FloatFontType.TYPE_CURE:
                WorldPos.y += CreatureHeightInMeters;
                pixelPos = Initialize.mainCam.WorldToScreenPoint(WorldPos);
                break;
            case FloatFontType.TYPE_ENEMY_DAMGE:
                WorldPos.y += CreatureHeightInMeters;
                pixelPos = Initialize.mainCam.WorldToScreenPoint(WorldPos);
                break;
            case FloatFontType.TYPE_ENEMY_FATAL:
                WorldPos.y += CreatureHeightInMeters;
                pixelPos = Initialize.mainCam.WorldToScreenPoint(WorldPos);
                //策划要求，暴击飘字实现偏左和偏右逐次交替切换效果
                bLeftOffset = !bLeftOffset;
                if (bLeftOffset)
                {
                    pixelPos.x += -100 * GfxUtil.ScreenScale.x;
                }
                else
                {
                    pixelPos.x += 100 * GfxUtil.ScreenScale.x;
                }
                break;
            case FloatFontType.TYPE_SELF_DMAGE:
                //CreatureHeightInMeters *= 0.5f;//自己的伤害在腰间，也就是高度取一半
                WorldPos.y += CreatureHeightInMeters;
                pixelPos = Initialize.mainCam.WorldToScreenPoint(WorldPos);
                break;
            case FloatFontType.TYPE_TLANT_POINT:
                WorldPos.y += CreatureHeightInMeters;
                pixelPos = Initialize.mainCam.WorldToScreenPoint(WorldPos);
               // pixelPos.x += 50.0f * GfxUtil.ScreenScale.x;
                break;

            case FloatFontType.TYPE_LEVELUP:
                WorldPos.y += CreatureHeightInMeters;
                pixelPos = Initialize.mainCam.WorldToScreenPoint(WorldPos);
                break;
        }
        return pixelPos;
    }


    public override void SetEnable(bool bEnable)
    {
        if (m_bEnable != bEnable)
        {
            m_bEnable = bEnable;
            UpdateVisible();
        }
    }


    public override void UpdateVisible()
    {
        bool bVisible = m_bVisible && m_isOnCameraView && !m_isOutOfVisbleDistance && m_bEnable;
        if (FinalVisible != bVisible)
        {
            FinalVisible = bVisible;
            m_DisplayInfo.Visible = FinalVisible;
            bNeedToUpdateDisplayInfo = true;
        }
    }
}
