using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using Scaleform;
/// <summary>
/// Lable类
/// </summary>
public sealed class GfxLable : UIComponent  {


    public int ID
    {
        get
        {
            return m_ID;
        }
    }

    public string text
    {
        get
        {
            return m_Text;
        }
    }

    /// <summary>
    /// 处于不可用状态下的颜色
    /// </summary>
    private Color m_DiableColor = Color.gray;

    private Color m_Color;

    private string m_Text;

    private int m_ID;

    Value m_TextField = new Value();

    bool bNeedToUpdateColor = false;
    bool bNeedToUpdateString = false;
    bool bNeedToUpdateEnable = false;

    public GfxLable(Value Ins, string text, Color col,int id):base(Ins)
    {

        m_ID = id;
        Ins.GetMember("textField", ref m_TextField);
        SetText(text);
        SetColor(col);
    }

    /// <summary>
    /// 设置颜色。如果Lable处于Enable=false下，函数调用会失败。
    /// </summary>
    /// <param name="Col">颜色</param>
    public void SetColor(Color Col)
    {
        if (!m_bEnable)
            return;
        if (Col != m_Color)
        {
            m_Color = Col;
            bNeedToUpdateColor = true;
        }
            
    }


    public void SetText(string text)
    {
        if (m_Text != text)
        {
            m_Text = text;
            bNeedToUpdateString = true;
        }
    }


    public override void Clear()
    {
        base.Clear();
        m_TextField = null;
    }


    public override void SetEnable(bool bEnable)
    {
        
        if (m_bEnable != bEnable)
        {
            m_bEnable = bEnable;
            bNeedToUpdateEnable = true;
        }

    }

    private void UpdateData()
    {
        if (bNeedToUpdateEnable)
        {
            if (!m_bEnable)
                m_TextField.SetMember("textColor", GfxUtil.ColorToUINT(m_DiableColor));
            else
                m_TextField.SetMember("textColor", GfxUtil.ColorToUINT(m_Color));

            bNeedToUpdateEnable = false;
        }


        if (bNeedToUpdateString)
        {
            m_TextField.SetMember("text", m_Text);
            bNeedToUpdateString = false;
        }

        if (bNeedToUpdateColor)
        {
            m_TextField.SetMember("textColor", GfxUtil.ColorToUINT(m_Color));
            bNeedToUpdateColor = false;
        }

    }

    private void UpdateDisplayInfo()
    {
        if (bNeedToUpdateDisplayInfo)
        {
            m_Instance.SetDisplayInfo(m_DisplayInfo);
        }
    }


    public override void OnUpdate()
    {
        UpdateDisplayInfo();
        UpdateData();
    }
}
