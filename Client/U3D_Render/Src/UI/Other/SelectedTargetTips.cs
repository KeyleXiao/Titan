#define USING_SCALEFORM
using System;
using System.Collections.Generic;
using System.Text;
using UnityEngine;
using Scaleform;
#if !USING_SCALEFORM
public sealed class SelectedTargetTips : MonoBehaviour
{
    private const string m_TexturePatch = "Effect/selectEffect/SelectArrow";
    private static Texture m_TipsTexture = null;
    private static Material m_TipsMaterial = null;
    private static SelectedTargetTips m_SingleInstance = null;

    public static SelectedTargetTips CreateInstance()
    {
        //暂时只能创建一个
        if (m_SingleInstance != null)
        {
            return m_SingleInstance;
        }

        SelectedTargetTips TempInstance = null;
        if (!m_TipsTexture)
        {
            m_TipsTexture = Resources.Load(m_TexturePatch) as Texture;
        }

        if (!m_TipsTexture)
        {
            return TempInstance;
        }

        if (null == m_TipsMaterial)
        {
            m_TipsMaterial = new Material(Shader.Find("Effect/BillBorad"));
            m_TipsMaterial.SetTexture("_MainTex", m_TipsTexture);
        }

        BillBoard temp = BillBoard.CreateBillBorad(32, 64, m_TipsMaterial);
        TempInstance = temp.gameObject.AddComponent<SelectedTargetTips>();
        UnityEngine.Object.DontDestroyOnLoad(TempInstance);
        temp.gameObject.hideFlags = HideFlags.HideAndDontSave;
        m_SingleInstance = TempInstance;
        return TempInstance;
    }


    private BillBoard m_BillBoard = null;
    private int m_entityID = -1;
    private U3D_Render.EntityView m_entityView = null;
    
#region 漂浮
    private float minHeightOffset = 0.5f;
    private float maxHeightOffset = 0.8f;
    
    private float floatSpeed = 0.8f;
    private float SpeedAcclect = 0.0f;
    private bool bUp = true;

    private float CurrentHeightOffset = 0.5f;
    private float CurrentSpeedAcclect = 0.0f;
    private float CurrentSpeed = 0.5f;
#endregion

    void Start()
    {
        TargetFrame.OnSelectedTarget += OnSelectedTarget;
        TargetFrame.OnDisSelectedTarget += OnDisSelectedTarget;
        m_BillBoard = GetComponent<BillBoard>();
        transform.gameObject.SetActive(false);
    }

    void OnSelectedTarget(int entityID)
    {
        m_entityID = entityID;
        m_entityView = EntityFactory.getEntityViewByID(entityID);
        m_BillBoard.gameObject.SetActive(true);
        transform.gameObject.name = "SelectedTargetTips:" + entityID;
        CurrentHeightOffset = minHeightOffset;
        CurrentSpeedAcclect = SpeedAcclect;
        CurrentSpeed = floatSpeed;
        bUp = true;
    }

    void OnDisSelectedTarget(int entityID)
    {
        m_BillBoard.gameObject.SetActive(false);
        m_entityID = -1;
        m_entityView = null;
    }

    void Update()
    {
        if (m_entityID < 0 || null == m_entityView)
        {
            return;
        }
        if (bUp)
        {
            //CurrentSpeedAcclect = SpeedAcclect;
            CurrentSpeed = floatSpeed;
        }
        else
        {
            //CurrentSpeedAcclect = -SpeedAcclect;
            CurrentSpeed = -floatSpeed;
        }

        //floatSpeed += CurrentSpeedAcclect;
        CurrentHeightOffset += CurrentSpeed * Time.deltaTime;
        Vector3 pos = m_entityView.gameObject.transform.position;

        pos.y += m_entityView.Property.CreatureHeightInMeters;

        pos.y += CurrentHeightOffset;

        if (CurrentHeightOffset > maxHeightOffset)
        {
            bUp = false;
        }
        else if(CurrentHeightOffset < minHeightOffset)
        {
            bUp = true;
        }
        transform.position = pos;
    }
}

#else

public sealed class SelectedTargetTips : UIComponent
{
    public static SelectedTargetTips SingleInstance
    {
        get
        {
            return m_SingleInstance;
        }
    }

    private const string m_AS3RefernceName = "SelectedTips";
    private static SelectedTargetTips m_SingleInstance = null;

    public static void CreateInstance()
    {
        Start();
    }

    private static void CreateSingleInstanceImpl()
    {
        if (null != m_SingleInstance)
        {
            return;
        }
    }

    private static int m_entityID = -1;
    private static U3D_Render.EntityView m_entityView = null;

    #region 漂浮
    private static float minHeightOffset = 0.5f;
    private static float maxHeightOffset = 0.8f;

    private static float floatSpeed = 0.8f;
    private static float SpeedAcclect = 0.0f;
    private static bool bUp = true;

    private static float CurrentHeightOffset = 0.5f;
    private static float CurrentSpeedAcclect = 0.0f;
    private static float CurrentSpeed = 0.5f;
    #endregion

    static void Start()
    {
        TargetFrame.OnSelectedTarget += OnSelectedTarget;
        TargetFrame.OnDisSelectedTarget += OnDisSelectedTarget;
    }

    static void OnSelectedTarget(TargetFrame.SelectTargetParam param)
    {
        m_entityID = param.nTargetId;
        m_entityView = EntityFactory.getEntityViewByID(param.nTargetId);
        m_SingleInstance.SetVisible(true);
        CurrentHeightOffset = minHeightOffset;
        CurrentSpeedAcclect = SpeedAcclect;
        CurrentSpeed = floatSpeed;
        bUp = true;
    }

    static void OnDisSelectedTarget(int entityID)
    {
        m_SingleInstance.SetVisible(false);
        m_entityID = -1;
        m_entityView = null;
    }


    public static void Update()
    {
        CreateSingleInstanceImpl();
        if (m_SingleInstance == null)
        {
            return;
        }
        if (m_entityID > 0 && null != m_entityView)
        {

            if (!m_entityView.gameObject)
            {
                OnDisSelectedTarget(m_entityView.ID);
                return;
            }

            if (bUp)
            {
                //CurrentSpeedAcclect = SpeedAcclect;
                CurrentSpeed = floatSpeed;
            }
            else
            {
                //CurrentSpeedAcclect = -SpeedAcclect;
                CurrentSpeed = -floatSpeed;
            }

            //floatSpeed += CurrentSpeedAcclect;
            CurrentHeightOffset += CurrentSpeed * Time.deltaTime;
            Vector3 pos = m_entityView.gameObject.transform.position;

            pos.y += m_entityView.Property.CreatureHeightInMeters;

            pos.y += CurrentHeightOffset;

            if (CurrentHeightOffset > maxHeightOffset)
            {
                bUp = false;
            }
            else if (CurrentHeightOffset < minHeightOffset)
            {
                bUp = true;
            }
            pos = Initialize.mainCam.WorldToScreenPoint(pos);
            m_SingleInstance.SetPositionFromWorldSpace(pos);
        }

        m_SingleInstance.OnUpdate();
    }






    public SelectedTargetTips(Value vs)
        : base(vs)
    {

    }
}

#endif