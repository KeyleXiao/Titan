using System;
using System.Collections.Generic;
using System.Text;
using UnityEngine;
using Scaleform;
/// <summary>
/// 头顶名称
/// </summary>
public sealed class TopName : UIComponent
{

    #region 外部属性

    public GameObject Entry
    {
        get
        {
            return m_entry;
        }
        set
        {
            m_entry = value;
        }
    }

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

    public MovieNode movieNode
    {
        get
        {
            return m_MovieNode;
        }
    }


    #endregion 

    #region 内部数据成员

    private GameObject m_entry;
    private int m_ID = 0;
    private bool isHero = false;
    private Vector3 lastpixelPos;
    private CreatureProperty m_EntryProperty = null;
    private Color m_Color = Color.black;
    private string m_Text = "";
    #endregion

    #region 显示信息

    /// <summary>
    /// 计算好的Y偏移
    /// </summary>
    private float m_RealYOffset = 0;
    /// <summary>
    /// 上一次计算好的Y便宜
    /// </summary>
    private float m_OldRealYOffset = -1;

    /// <summary>
    /// 高度
    /// </summary>
    private float m_CreatureHeight = 0.0f;
    /// <summary>
    /// 旧的高度
    /// </summary>
    private float m_OldCreatureHeight = -1.0f;

    /// <summary>
    /// 是否拥有初始位置，刚开始的时候，位置不知道为什么，超级大。
    /// </summary>
    private bool HaveInitPos = false;


    bool bNeedToUpdateColor = false;
    bool bNeedToUpdateString = false;
    #endregion

    #region 其他属性

    private MovieNode m_MovieNode;
    private BillBoard m_BillBorad = null;
    private static Material TopNameMaterial = null;

    /// <summary>
    /// 距离缩放纠正，用来纠正距离远了之后，血条的高度
    /// </summary>
    private const float DistanceCorreFactor = 0.27f;

    #endregion


    #region scaleForm数据

    Value m_TextField = new Value();

    #endregion

    public TopName(Value SFInstance)
        : base(SFInstance)
    {


        //SetScale(2.0f, 2.0f);
    }


    public void Init( GameObject entry, int id,string text,Color col)
    {
        lastpixelPos = Vector3.zero;
        m_entry = entry;
        m_ID = id;
        isHero = id == EntityFactory.MainHeroID;
        if (entry)
        {
            m_EntryProperty = m_entry.GetComponent<CreatureProperty>();
        }
        if(m_Instance != null)
            m_Instance.GetMember("textField", ref m_TextField);
        SetText(text);
        SetColor(col);
    }


    public override void OnUpdate()
    {
        UpdateData();
        UpdateTransform();
        UpdateDisplayInfo();
    }


    private void UpdateDisplayInfo()
    {
        if (bNeedToUpdateDisplayInfo)
        {
            m_Instance.SetDisplayInfo(m_DisplayInfo);
            bNeedToUpdateDisplayInfo = false;
        }
    }

    private void UpdateData()
    {
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

    //private void UpdateTransform()
    //{
    //    if (!m_bEnable || !m_entry || !m_EntryProperty)
    //    {
    //        return;
    //    }
    //    Vector3 pos;

    //    m_CreatureHeight = m_EntryProperty.CreatureHeightInMeters * m_entry.transform.localScale.y;
    //    m_RealYOffset = m_EntryProperty.TopNameYOffset * GfxUtil.ScreenScale.y;
    //    if (isHero)
    //    {
    //        SetVisible(false);

    //        ////对于主角，如果Yoffset和高都没改变，跳过
    //        //if (GfxUtil.EqualsFloat(m_RealYOffset, m_OldRealYOffset) && GfxUtil.EqualsFloat(m_CreatureHeight, m_OldCreatureHeight) && HaveInitPos)
    //        //{
    //        //    return;
    //        //}
    //        //pos = m_entry.transform.position;
    //        //pos.y += m_CreatureHeight;
    //        //m_OldCreatureHeight = m_CreatureHeight;

    //        //pos = Initialize.mainCam.WorldToScreenPoint(pos);
    //        ////刚开始的时候，位置超级大，不知道为什么
    //        //if (!HaveInitPos && pos.magnitude < 1000000)
    //        //{
    //        //    HaveInitPos = true;
    //        //}
    //        //pos.y += m_RealYOffset;
    //        //m_OldRealYOffset = m_RealYOffset;

    //        ////主角永远处于屏幕中央的位置，所以当差距达到1以上才移动
    //        //if (Vector3.Distance(lastpixelPos, pos) > 1.0f && HaveInitPos)
    //        //{
    //        //    SetPositionFromWorldSpace(pos);
    //        //}
    //        //lastpixelPos = pos;

    //        return;
    //    }

    //    //算出相机和人物的距离
    //    Vector3 tempDis = m_entry.transform.position - Initialize.mainCam.transform.position;
    //    float dis = tempDis.sqrMagnitude;

    //    float hideinDis = m_EntryProperty ? m_EntryProperty.TopInfoHideInDistance : 20.0f;
    //    float CameraDis = SoldierCamera.MainInstance ? SoldierCamera.MainInstance.CurDistance : 8.0f;
    //    if (hideinDis <= 0)
    //    {
    //        hideinDis = 0.0f;
    //    }

    //    hideinDis += CameraDis;
    //    if (dis > hideinDis * hideinDis)//距离超过hideinDis，隐藏
    //    {
    //        m_isOutOfVisbleDistance = true;
    //        UpdateVisible();
    //        return;
    //    }
    //    else
    //    {
    //        if (m_isOutOfVisbleDistance)
    //        {
    //            m_isOutOfVisbleDistance = false;
    //            UpdateVisible();
    //        }

    //    }

    //    pos = m_entry.transform.position;
    //    pos.y += m_CreatureHeight;
    //    m_OldCreatureHeight = m_CreatureHeight;

    //    pos = Initialize.mainCam.WorldToScreenPoint(pos);

    //    pos.y += m_RealYOffset;
    //    m_OldRealYOffset = m_RealYOffset;

    //    tempDis = pos - lastpixelPos;
    //    if (tempDis.sqrMagnitude > 0.0001f)
    //    {
    //        SetPositionFromWorldSpace(pos);
    //    }
    //    lastpixelPos = pos;
    //}


    private void UpdateTransform()
    {
        if (!m_bEnable || !m_entry || !m_EntryProperty || !m_BillBorad)
            return;


        Vector3 off = Vector3.zero;

        float RealY = m_EntryProperty.CreatureHeightInMeters;
        if (m_BillBorad.ZDistance > 5.0f)
        {
            RealY += m_BillBorad.HeightInMeter * 0.5f;
        }
        off.y = RealY;
        off.y += m_EntryProperty.TopNameYOffset * m_BillBorad.PixelToMeterScale;

        if (isHero)
        {
            off = m_BillBorad.transform.localRotation * off;
        }
        m_BillBorad.transform.localPosition = off;


        if (isHero)
        {
            off = m_BillBorad.transform.localRotation * off;
        }
        
        m_BillBorad.transform.localPosition = off;

        if (isHero)
        {
            SetVisible(false);
            return;
        }
        //算出相机和人物的距离
        Vector3 tempDis = m_entry.transform.position - Initialize.mainCam.transform.position;
        float dis = tempDis.sqrMagnitude;
        float hideinDis = m_EntryProperty ? m_EntryProperty.TopInfoHideInDistance : 20.0f;
        if (hideinDis <= 0)
        {
            hideinDis = 0.0f;
        }
        if (dis > hideinDis * hideinDis)//距离超过hideinDis，隐藏
        {
            m_isOutOfVisbleDistance = true;
            UpdateVisible();
            return;
        }
        else
        {
            if (m_isOutOfVisbleDistance)
            {
                m_isOutOfVisbleDistance = false;
                UpdateVisible();
            }
        }

    }


    #region 显示更新

    public override void  SetEnable(bool bEnable)
    {
        if (m_bEnable != bEnable)
        {
            m_bEnable = bEnable;
            UpdateVisible();
        }

    }


    public override void UpdateVisible()
    {
        bool bVis = m_bVisible && m_isOnCameraView && !m_isOutOfVisbleDistance && m_bEnable;
        if (bVis != FinalVisible)
        {
            FinalVisible = bVis;
            m_DisplayInfo.Visible = FinalVisible;
            bNeedToUpdateDisplayInfo = true;

            if (m_BillBorad)
            {
                m_BillBorad.gameObject.SetActive(FinalVisible);
            }
        }
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

    #endregion

    #region 其他

    public override void Clear()
    {
        base.Clear();
        m_TextField = null;
        if (m_BillBorad)
        {
            m_BillBorad.Clear();
            m_BillBorad = null;
        }
    }

    public override bool GetEnable()
    {
        return m_bEnable;

    }
    #endregion

    public void CreateBillBorad()
    {
        if (null == TopNameMaterial)
        {
            TopNameMaterial = new Material(Shader.Find("Effect/BillBorad"));
            TopNameMaterial.SetTexture("_MainTex", TopNameMovie.TopNameRenderTexture);
            TopNameMaterial.name = "TopNameMaterial";
            TopNameMaterial.hideFlags = HideFlags.HideAndDontSave;
        }

        if (m_BillBorad == null)
        {
            m_BillBorad = BillBoard.CreateBillBorad((int)m_MovieNode.vSize.x, (int)m_MovieNode.vSize.y, TopNameMaterial, "TopName -- temp" );
        }
    }

    public void ChangeMovieNode(MovieNode trs)
    {
        if(!m_BillBorad)
        {
            return;
        }

        m_MovieNode = trs;
        Vector2 pos = m_MovieNode.start;

        pos.x += m_MovieNode.vSize.x * 0.5f;
        pos.y += m_MovieNode.vSize.y * 0.5f;

        SetPositionDirectly_StagePoint(pos);

        TopNameMovie.TransformBillBorad(m_BillBorad);
        Vector2 texStart = TopNameMovie.TransformTextureOffsetFromPixelPos(m_MovieNode.start);
        Vector2 texEnd = TopNameMovie.TransformTextureOffsetFromPixelPos(m_MovieNode.end);

        m_BillBorad.SetTextureOffset(texStart, texEnd);
        m_BillBorad.transform.parent = m_entry.transform;
        m_BillBorad.transform.localPosition = new Vector3(0.0f, m_EntryProperty.CreatureHeightInMeters, 0.0f);
    }

    public void ChangeParent(Transform paren)
    {
        if (m_BillBorad)
        {
            m_BillBorad.transform.parent = paren;
            //m_BillBorad.name = "TopName--" + paren.name;
        }

    }
}