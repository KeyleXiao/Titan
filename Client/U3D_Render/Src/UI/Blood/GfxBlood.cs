using UnityEngine;
using System.Collections;
using Scaleform;
using War;
/// <summary>
/// Blood类
/// </summary>
public class GfxBlood : UIComponent
{

    #region 外部成员

    public int ID
    {
        get
        {
            return m_ID;
        }
    }

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

    public MovieNode movieNode
    {
        get
        {
            return m_MovieNode;
        }
    }

    public ENTITY_TYPE bloodType = ENTITY_TYPE.TYPE_MONSTER;
    #endregion

    #region 内部成员
    private int m_ID;

    private int m_MaxValue;

    private int m_CurValue;
    private int m_TempValue;

    private string m_Name = "未知";

    private GameObject m_entry;

    private Vector3 lastpixelPos;
    private bool isHero;
    private CreatureProperty m_EntryProperty = null;

    private float fPer = 0.0f;

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

    private int camType = 0;

    private int nColor = 0;


    /// <summary>
    /// 是否拥有初始位置，刚开始的时候，位置不知道为什么，超级大。
    /// </summary>
    private bool HaveInitPos = false;

    private int m_level = 1;

    /// <summary>
    /// 血量数字信息，当前值/最大值
    /// </summary>
    private string m_BloodInfo = "100/100";

    /// <summary>
    /// 是否需要更新临时血量，当临时血量大于0的情况下，更新了血量，或者临时血量改变
    /// </summary>
    private bool bNeedToUpdateTempValue = false;

    /// <summary>
    /// 是否需要更新血量数字信息，当前血量或最大血量改变，都需要更新血量数字信息
    /// </summary>
    private bool bNeedToUpdateBloodInfo = false;

    #endregion

    #region 数据更新
    private bool bNeedToUpdateMaxValue = false;
    private bool bNeedToUpdateCurValue = false;
    private bool bNeedToUpdateCampType = false;
    private bool bNeedToUpdateLevel = false;
    private bool bNeedToUpdateName = false;
    #endregion

    #region 其他

    private MovieNode m_MovieNode;
    private BillBoard m_BillBorad = null;
    private static Material BloodMaterial = null;

    /// <summary>
    /// 距离缩放纠正，用来纠正距离远了之后，血条的高度
    /// </summary>
    private const float DistanceCorreFactor = 0.23f;

    #endregion

    public GfxBlood(Value Ins):base(Ins)
    {
    }

    public void Init(int id, GameObject host, int cam,ENTITY_TYPE bt)
    {
        lastpixelPos = Vector3.zero;
        m_entry = host;
        m_ID = id;
        m_TempValue = 0;
        m_level = 1;
        isHero = id == EntityFactory.MainHeroID;
        if (host)
        {
            m_EntryProperty = host.GetComponent<CreatureProperty>();
        }
        bloodType = bt;
        SetCamType(cam);

        setPrafabPer();

        nColor = GameLogicAPI.getCanseckillHpColor(getMonsterSchemeId());

    }

    public void CreateBillBorad()
    {
        if (null == BloodMaterial)
        {
            BloodMaterial = new Material(Shader.Find("Effect/BillBorad"));
            BloodMaterial.SetTexture("_MainTex", GfxBloodMovie.BloodRenderTexture);
            BloodMaterial.name = "BloodMaterial";
            BloodMaterial.hideFlags = HideFlags.HideAndDontSave;

        }

        if (m_BillBorad == null)
        {
            m_BillBorad = BillBoard.CreateBillBorad((int)m_MovieNode.vSize.x, (int)m_MovieNode.vSize.y, BloodMaterial, "Blood -- temp");
        }
    }

    public override void OnUpdate()
    {
        //先更新Tranform
        UpdateTransform();
        //更新显示信息
        UpdateDisplayInfo();
        //更新数据
        UpdateDate();
    }

    //private void UpdateTransform()
    //{
    //    if (!m_bEnable || !m_entry || !m_EntryProperty)
    //        return;

    //    Vector3 pos;

    //    m_CreatureHeight = m_EntryProperty.CreatureHeightInMeters * m_entry.transform.localScale.y;

    //    m_RealYOffset = m_EntryProperty.BloodYOffset * GfxUtil.ScreenScale.y;
    //    //主角永远处于屏幕中央的位置，所以只需要更新一次即可
    //    if (isHero)
    //    {
    //        //SetVisible(false);

    //        //对于主角，如果Yoffset和高都没改变，跳过
    //        if (GfxUtil.EqualsFloat(m_RealYOffset, m_OldRealYOffset) && GfxUtil.EqualsFloat(m_CreatureHeight, m_OldCreatureHeight) && HaveInitPos)
    //        {
    //            return;
    //        }



    //        pos = m_entry.transform.position;
    //        pos.y += m_CreatureHeight;
    //        m_OldCreatureHeight = m_CreatureHeight;


    //        pos = Initialize.mainCam.WorldToScreenPoint(pos);

    //        //刚开始的时候，位置超级大，不知道为什么
    //        if (!HaveInitPos && pos.magnitude < 1000000)
    //        {
    //            HaveInitPos = true;
    //        }

    //        pos.y += m_RealYOffset;
    //        m_OldRealYOffset = m_RealYOffset;
    //        //主角永远处于屏幕中央的位置，所以当差距达到1以上才移动
    //        if (Vector3.Distance(lastpixelPos, pos) > 1.0f && HaveInitPos)
    //        {
    //            SetPositionFromWorldSpace(pos);
    //        }
    //        lastpixelPos = pos;

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
        off.y += m_EntryProperty.BloodYOffset * m_BillBorad.PixelToMeterScale;

        //if (isHero)
        //{
        //    off = m_BillBorad.transform.localRotation * off;
        //}
        m_BillBorad.transform.localPosition = off;

        //if (isHero)
        //{
        //    SetVisible(false);
        //    return;
        //}
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

    private void UpdateDisplayInfo()
    {
        if (bNeedToUpdateDisplayInfo)
        {
            m_Instance.SetDisplayInfo(m_DisplayInfo);
            bNeedToUpdateDisplayInfo = false;
        }
    }

    private void UpdateDate()
    {
        if (bNeedToUpdateMaxValue)
        {
            m_Instance.Invoke("SetMaxValue", m_MaxValue);
            bNeedToUpdateMaxValue = false;
        }

        if (bNeedToUpdateCurValue)
        {
            m_Instance.Invoke("SetCurrValue", m_CurValue);
            bNeedToUpdateCurValue = false;
        }

        if (bNeedToUpdateCampType)
        {
            m_Instance.Invoke("SetCamp", camType);
            bNeedToUpdateCampType = false;
        }

        if (bNeedToUpdateTempValue)
        {
            m_Instance.Invoke("SetTempValue", m_TempValue);
            bNeedToUpdateTempValue = false;
        }

        if (bNeedToUpdateLevel)
        {
            m_Instance.Invoke("SetLevel", m_level);
            bNeedToUpdateLevel = false;
        }

        if (bNeedToUpdateName)
        {
            m_Instance.Invoke("SetName", m_Name);
            bNeedToUpdateName = false;
        }

        if (bNeedToUpdateBloodInfo)
        {
            m_Instance.Invoke("SetBloodInfo", m_BloodInfo);
            bNeedToUpdateBloodInfo = false;
        }
    }



    #region 数据更新 C#--->AS

    public void SetMaxValue(int v)
    {
        if (m_MaxValue == v)
            return;
        m_MaxValue = v;

        if (m_Instance == null && !m_Instance.IsValid())
        {
            Trace.LogError("GfxBlood SetMaxValue failed m_Instance == null");
            return;
        }

        if (m_MaxValue > 0)
        {
          
            bNeedToUpdateMaxValue = true;
            //if (isHero)
            //{
            //    SetVisible(true);
            //}
        }
        //else
        //{

        //    SetVisible(false);
        //}

    }

    public void SetCurValue(int v)
    {
        if (m_CurValue == v /*|| m_TempValue > 0*/)
            return;
        if (v < 0)
            v = 0;
        m_CurValue = v;

        //判断如果是敌方，并且血量达到秒杀值，则设置阵营为敌方秒杀类型
        if (camType == (int)GFxCampManager.GFxCampTpye.CT_Enemy && getMonsterSubType() == (int)EWarMonsterExec.EWME_Soldier)
        {
            if ((float)m_CurValue <= (float)m_MaxValue * (this.fPer) && nColor != 0 )
            {
                m_Instance.Invoke("SetImageColor", nColor);
            }
        }

        if (m_Instance == null && !m_Instance.IsValid())
        {
            Trace.LogError("GfxBlood SetMaxValue failed m_Instance == null");
            return;
        }
        else
        {
            if (m_TempValue > 0)
            {
                bNeedToUpdateTempValue = true;
            }
            bNeedToUpdateCurValue = true;
         
        }

    }

    public void SetCamType(int cam)
    {
        if (camType == cam)
            return;

        camType = cam;
        bNeedToUpdateCampType = true;
    }

    public void SetTemp(int temp)
    {
        if (m_TempValue == temp)
        {
            return;

        }
        if (m_Instance == null && !m_Instance.IsValid())
        {
            Trace.LogError("GfxBlood SetTemp failed m_Instance == null");
            return;
        }
        m_TempValue = temp;
        bNeedToUpdateTempValue = true;
    }

    /// <summary>
    /// 设置血条框上的等级
    /// </summary>
    /// <param name="leve">等级，最大99</param>
    public void SetLevel(int leve)
    {
        if (m_level == leve)
        {
            return;
        }
        m_level = Mathf.Clamp(leve, 1, 99);
        bNeedToUpdateLevel = true;

    }

    public void SetName(string name)
    {
        if (m_Name.Equals(name))
        {
            return;
        }
        m_Name = name;
        bNeedToUpdateName = true;
    }

    /// <summary>
    /// 设置血量数字信息
    /// </summary>
    /// <param name="bloodInfo">血量数字值</param>
    public void SetBloodInfo(string bloodInfo)
    {
        if (m_BloodInfo.Equals(bloodInfo))
        {
            return;
        }
        m_BloodInfo = bloodInfo;
        bNeedToUpdateBloodInfo = true;
    }
    #endregion


    #region 显示信息

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

            if (m_BillBorad)
            {
                m_BillBorad.gameObject.SetActive(FinalVisible);
            }
        }
    }

    /// <summary>
    /// 设置血量数值显示状态
    /// </summary>
    /// <param name="bVisible"></param>
    public void SetBloodInfoVisible(bool bVisible)
    {
        m_Instance.Invoke("SetBloodInfoVisible", bVisible);
    }

    /// <summary>
    /// 设置怪物血条的颜色
    /// </summary>
    /// <param name="bIsChange"></param>
    public void SetBloodColor(bool bIsChange)
    {
        if (!Value.IsValueValid(m_Instance))
        {
            Trace.LogError("setPlayerData failed vEndTable = " + m_Instance);
            return;
        }

        if (bIsChange)
        {
            m_Instance.Invoke("SetImageColor", nColor);
        }
        else
        {
            m_Instance.Invoke("SetImageFirstColor");
        }
    }

    /// <summary>
    /// 设置主角的血条可见性
    /// </summary>
    /// <param name="bPlayerVisible">是否可见</param>
    public void SetPlayerVisible(bool bPlayerVisible)
    {
        if (m_bVisible != bPlayerVisible)
        {
            m_bVisible = bPlayerVisible;
            UpdateVisible();
        }
    }
    #endregion


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

        m_BillBorad.SetPixelSize((int)m_MovieNode.vSize.x, (int)m_MovieNode.vSize.y);
        GfxBloodMovie.TransformBillBorad(m_BillBorad);
        Vector2 texStart = GfxBloodMovie.TransformTextureOffsetFromPixelPos(m_MovieNode.start);
        Vector2 texEnd = GfxBloodMovie.TransformTextureOffsetFromPixelPos(m_MovieNode.end);

        m_BillBorad.SetTextureOffset(texStart, texEnd);

        ChangeParent(m_entry.transform);
    }

    public void ChangeParent(Transform paren)
    {
        if(m_BillBorad)
        {
            m_BillBorad.transform.parent = paren;
            //m_BillBorad.name = "Blood--" + paren.name;
        }
        
    }

    public override void Clear()
    {
        base.Clear();
        if (m_BillBorad)
        {
            m_BillBorad.Clear();
            m_BillBorad = null;
        }
    }

    public int getMonsterSubType()
    {
        U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(this.m_ID);
        if (ev == null)
        {
            return 0;
        }
        
        return ev.Flag;
    }

    public void setPrafabPer()
    {
        U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(this.m_ID);
        if (ev == null)
        {
            return;
        }
        
        EntityPrefab ep = ev.gameObject.GetComponent<EntityPrefab>();
        if (ep != null)
        {
            this.fPer = ep.per;
        }
    }

    protected int getMonsterSchemeId()
    {
        U3D_Render.EntityView ev = EntityFactory.getEntityViewByID(this.m_ID);
        if (ev == null)
        {
            return 0;
        }
        return ev.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
    }

}
