/*******************************************************************
** 文件名:	BillBoard.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/27
** 版  本:	1.0
** 描  述:	公告板类 
** 应  用:  	公告板

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;


/// <summary>
/// 公告板,相对于主相机
/// </summary>
public sealed class BillBoard : MonoBehaviour
{

    #region API
    /// <summary>
    /// 创建一个公告板
    /// </summary>
    /// <param name="pixelWidth">像素宽</param>
    /// <param name="pixelHeight">像素高</param>
    /// <param name="mat">材质（包含纹理）,同种类型的BillBoard(如血条)，尽量使用同一个材质</param>
    /// <param name="BillBoardName">名字</param>
    /// <returns></returns>
    public static BillBoard CreateBillBorad(int pixelWidth, int pixelHeight, Material mat, string BillBoardName = "billBoard")
    {

        GameObject go = new GameObject();
        go.name = BillBoardName;
        BillBoard bb = go.AddComponent<BillBoard>();

        bb.BuildBillBoard(pixelWidth, pixelHeight, mat);

        return bb;
    }

    #endregion

    #region 外部成员

    public int pixelWidth
    {
        get
        {
            return m_pixelWidth;
        }
    }

    public int pixelHeight
    {
        get
        {
            return m_pixelHeight;
        }
    }

    public float WidthInMeter
    {
        get
        {
            return m_WidthInMeter;
        }
    }

    public float HeightInMeter
    {
        get
        {
            return m_HeightInMeter;
        }
    }

    /// <summary>
    /// billBoard像素到米的缩放值，是绝对值
    /// </summary>
    public float PixelToMeterScale
    {
        get
        {
            return Mathf.Abs(m_PixelToMeterScale);
        }
    }

    /// <summary>
    /// billBoard到相机的Z距离
    /// </summary>
    public float ZDistance
    {
        get
        {
            return m_zDistance;
        }
    }

    #endregion

    #region 内部成员

    /// <summary>
    /// 标准缩放，这个值会根据Movie和RenderTexture的大小而改变
    /// </summary>
    [HideInInspector]
    public Vector3 StandradScale = Vector3.one;

    /// <summary>
    /// 缩放纠正，这个是外部缩放使用的
    /// </summary>
    [HideInInspector]
    public Vector3 CorrScale = Vector3.one;

    private Transform m_Transform;
    private MeshFilter m_Filter;
    private MeshRenderer m_Render;
    private Mesh m_Mesh;


    private int m_pixelWidth;
    private int m_pixelHeight;
    private bool bBuild = false;

    private Material m_mat = null;
    private Texture m_tex = null;
    private Quaternion direction = new Quaternion();

    private float m_StandradPixelToMeterScale = 1.0f;
    private float m_PixelToMeterScale = 1.0f;
    private float m_zDistance = 1.0f;
    private float m_WidthInMeter = 0.0f;
    private float m_HeightInMeter = 0.0f;

    private static float unScaleflags_fov = 0.0f;
    private static float unScaleflags_srceen = 0.0f;

    
    #endregion

    #region 缓存值

    private int lastScreenHeight = 0;
    private float lastFieldOfView = 0f;


    private Vector2 OldtexStart = Vector2.one * -1;
    private Vector2 OldtexEnd = Vector2.one * -1;
    #endregion



    void LateUpdate()
    {
        //不产生近大远小的效果
        m_zDistance = Vector3.Dot(m_Transform.position - Initialize.mainCam.transform.position, Initialize.mainCam.transform.forward);
        
        if (!lastFieldOfView.Equals(Initialize.mainCam.fieldOfView) || lastScreenHeight != Screen.height)
        {
            unScaleflags_srceen = 2f / Screen.height;
            unScaleflags_fov = Mathf.Tan(Initialize.mainCam.fieldOfView * 0.5f * Mathf.Deg2Rad);
            lastScreenHeight = Screen.height;
            lastFieldOfView = Initialize.mainCam.fieldOfView;
        }
        m_StandradPixelToMeterScale = unScaleflags_srceen * 5.0f * unScaleflags_fov;

        m_PixelToMeterScale = unScaleflags_srceen * m_zDistance * unScaleflags_fov;

        Vector3 scale;
        scale.x = m_pixelWidth * m_PixelToMeterScale * StandradScale.x * CorrScale.x ;
        scale.y = m_pixelHeight * m_PixelToMeterScale * StandradScale.y * CorrScale.y ;
        scale.z = StandradScale.z;

        m_WidthInMeter = Mathf.Abs(scale.x);
        m_HeightInMeter = Mathf.Abs(scale.y);

        m_Transform.localScale = scale;

        m_Transform.rotation = Initialize.mainCam.transform.rotation * direction;
    }

    public void Clear()
    {
        GameUtil.DestroyMesh(m_Mesh);
        Destroy(m_Filter);
        
        Destroy(m_Render);
        Destroy(m_Transform.gameObject);
    }

    /// <summary>
    /// 构建一个公告板
    /// </summary>
    /// <param name="pixelWidth"></param>
    /// <param name="pixelHeight"></param>
    /// <param name="tex"></param>
    public void BuildBillBoard(int pixelWidth, int pixelHeight, Material mat)
    {
        if (bBuild)
        {
            return;
        }

        m_Transform = transform;

        direction.x = m_Transform.localRotation.x;
        direction.y = m_Transform.localRotation.y;
        direction.z = m_Transform.localRotation.z;
        direction.w = m_Transform.localRotation.w;


        m_Filter = m_Transform.gameObject.AddComponent<MeshFilter>();
        m_Render = m_Transform.gameObject.AddComponent<MeshRenderer>();
        m_Mesh = m_Filter.mesh;
        m_Mesh.MarkDynamic();
        
        m_pixelWidth = pixelWidth;
        m_pixelHeight = pixelHeight;

        //float w = pixelWidth * GfxUtil.PixelToMeterUnit;
        //float h = pixelHeight * GfxUtil.PixelToMeterUnit;


        float w = 1.0f;
        float h = 1.0f;

        Vector3[] vertices = new Vector3[4];

        vertices[0] = new Vector3(-w * 0.5f, -h * 0.5f, 0);
        vertices[1] = new Vector3(w * 0.5f, -h * 0.5f, 0);
        vertices[2] = new Vector3(-w * 0.5f, h * 0.5f, 0);
        vertices[3] = new Vector3(w * 0.5f, h * 0.5f, 0);

        m_Mesh.vertices = vertices;

        int[] tri = new int[6];

        tri[0] = 0;
        tri[1] = 2;
        tri[2] = 1;

        tri[3] = 2;
        tri[4] = 3;
        tri[5] = 1;

        m_Mesh.triangles = tri;

        Vector3[] normals = new Vector3[4];

        normals[0] = -Vector3.forward;
        normals[1] = -Vector3.forward;
        normals[2] = -Vector3.forward;
        normals[3] = -Vector3.forward;

        m_Mesh.normals = normals;

        Vector2[] uv = new Vector2[4];

        uv[0] = new Vector2(0, 0);
        uv[1] = new Vector2(1, 0);
        uv[2] = new Vector2(0, 1);
        uv[3] = new Vector2(1, 1);

        m_Mesh.uv = uv;

        m_Filter.sharedMesh = m_Mesh;

        m_mat = mat;
        m_Render.sharedMaterial = mat;
        normals = null;
        uv = null;
        vertices = null;
        tri = null;
        bBuild = true;
    }


    /// <summary>
    /// 设置大小，像素为单位
    /// </summary>
    /// <param name="width"></param>
    /// <param name="height"></param>
    public void SetSize(int width, int height)
    {
        if (m_pixelWidth == width &&
            m_pixelHeight == height)
        {
            return;
        }
        m_pixelWidth = width;
        m_pixelHeight = height;
    }

    /// <summary>
    /// 设置名字
    /// </summary>
    /// <param name="name"></param>
    public void SetName(string name)
    {
        this.name = name;
    }

    /// <summary>
    /// 设置billBoard的纹理
    /// </summary>
    /// <param name="tex"></param>
    public void SetTexture(Texture tex)
    {
        if (m_tex == tex)
        {
            return;
        }
        m_tex = tex;
        m_mat.SetTexture("_MainTex", tex);

    }


    public void SetPixelSize(int w,int h)
    {
        m_pixelWidth = w;
        m_pixelHeight = h;
    }

    public void SetTextureOffset(Vector2 start, Vector2 end)
    {
        if(OldtexStart == start &&
            OldtexEnd == end)
        {
            return;
        }
        OldtexStart = start;
        OldtexEnd = end;
        Mesh mesh = m_Filter.mesh;
        Vector2[] uv = mesh.uv;
        uv[0] = new Vector2(start.x, start.y);
        uv[1] = new Vector2(end.x, start.y);
        uv[2] = new Vector2(start.x, end.y);
        uv[3] = new Vector2(end.x, end.y);
        mesh.uv = uv;
    }

    public void SetScale(Vector2 sc)
    {
        CorrScale.x = sc.x;
        CorrScale.y = sc.y;
    }
}
