using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace USpeedUI.UWidgets
{
    //公告板，血条、头顶名字用
    public sealed class UBillBoard : MonoBehaviour
    {
        public int PixelWidth{ get{return m_pixelWidth;} }
        public int PixelHeight{ get{return m_pixelHeight;} }
        public float WidthInMeter { get{return m_widthInMeter; } }
        public float HeightInMeter{ get{return m_heightInMeter;} }
        public float ZDistance { get { return m_zDistance; } }
        public float PixelToMeterScale{ get{return Mathf.Abs(m_pixelToMeterScale);} }

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

        private MeshFilter m_filter;
        private MeshRenderer m_render;
        private Mesh m_mesh;

        private int m_pixelWidth;
        private int m_pixelHeight;
        private bool m_bBuild = false;

        private Material m_mat = null;
        private Texture m_tex = null;

        private float m_zDistance = 1.0f;
        private float m_widthInMeter = 0.0f;
        private float m_heightInMeter = 0.0f;
        private float m_pixelToMeterScale = 1.0f;

        private static float m_unScaleflags_fov = 0.0f;
        private static float m_unScaleflags_srceen = 0.0f;

        private int m_lastScreenHeight = 0;
        private float m_lastFieldOfView = 0f;

        private Vector2 m_oldtexStart = Vector2.one * -1;
        private Vector2 m_oldtexEnd = Vector2.one * -1;
        private Quaternion m_direction = new Quaternion();

        public void OnDestroy()
        {
            Clear();
        }

        public void Clear()
        {
            GameUtil.DestroyMesh(m_mesh);
            ResNode.DestroyRes(ref m_filter);
            ResNode.DestroyRes(ref m_render);
            GameObject temp = gameObject;
            ResNode.DestroyRes(ref temp);

            m_mat = null;
            m_tex = null;
            m_mesh = null;
            m_filter = null;
            m_render = null;

            m_bBuild = false;
        }

        public void LateUpdate()
        {
            if (null == EntityFactory.MainHero || null == SoldierCamera.MainInstance() || null == SoldierCamera.MainInstance().CurrentCamera)
            {
                return;
            }

            Camera curCam = SoldierCamera.MainInstance().CurrentCamera;

            //消除近大远小的效果
            float dis = Vector3.Dot(transform.position - curCam.transform.position, curCam.transform.forward);
            if (Mathf.Abs(dis - m_zDistance) > 0.1f)
                m_zDistance = dis;

            if (!m_lastFieldOfView.Equals(curCam.fieldOfView) || m_lastScreenHeight != Screen.height)
            {
                m_unScaleflags_srceen = 2f / Screen.height;
                m_unScaleflags_fov = Mathf.Tan(curCam.fieldOfView * 0.5f * Mathf.Deg2Rad);
                m_lastScreenHeight = Screen.height;
                m_lastFieldOfView = curCam.fieldOfView;
            }

            float m_standradPixelToMeterScale = m_unScaleflags_srceen * 5.0f * m_unScaleflags_fov;
            m_pixelToMeterScale = m_unScaleflags_srceen * m_zDistance * m_unScaleflags_fov;

            Vector3 scale;
            scale.x = Mathf.Abs(m_pixelWidth * m_pixelToMeterScale * StandradScale.x * CorrScale.x);
            scale.y = Mathf.Abs(m_pixelHeight * m_pixelToMeterScale * StandradScale.y * CorrScale.y);
            scale.z = StandradScale.z;

            m_widthInMeter = Mathf.Abs(scale.x);
            m_heightInMeter = Mathf.Abs(scale.y);

            transform.localScale = scale;
            transform.rotation = curCam.transform.rotation * m_direction;
        }

        /// <summary>
        /// 创建一个公告板
        /// </summary>
        /// <param name="pixelWidth">像素宽</param>
        /// <param name="pixelHeight">像素高</param>
        /// <param name="mat">材质</param>
        /// <param name="BillBoardName">名字</param>
        /// <returns></returns>
        public static UBillBoard CreateBillBorad(int pixelWidth, int pixelHeight, Material mat, string BillBoardName = "billBoard")
        {
            GameObject go = new GameObject();
            go.name = BillBoardName;
            UBillBoard bb = go.AddComponent<UBillBoard>();

            bb.BuildBillBoard(pixelWidth, pixelHeight, mat);

            return bb;
        }

        /// <summary>
        /// 构建一个公告板
        /// </summary>
        /// <param name="pixelWidth"></param>
        /// <param name="pixelHeight"></param>
        /// <param name="tex"></param>
        public void BuildBillBoard(int pixelWidth, int pixelHeight, Material mat)
        {
            if (m_bBuild)
                return;

            m_filter = gameObject.AddComponent<MeshFilter>();
            m_render = gameObject.AddComponent<MeshRenderer>();
            m_mesh = m_filter.mesh;
            m_mesh.MarkDynamic();

            m_direction.x = transform.localRotation.x;
            m_direction.y = transform.localRotation.y;
            m_direction.z = transform.localRotation.z;
            m_direction.w = transform.localRotation.w;
            m_pixelWidth = pixelWidth;
            m_pixelHeight = pixelHeight;

            Vector3[] vertices = new Vector3[4];
            vertices[0] = new Vector3(-0.5f, -0.5f, 0);
            vertices[1] = new Vector3(0.5f, -0.5f, 0);
            vertices[2] = new Vector3(-0.5f, 0.5f, 0);
            vertices[3] = new Vector3(0.5f, 0.5f, 0);
            m_mesh.vertices = vertices;

            int[] tri = new int[6];
            tri[0] = 0;
            tri[1] = 2;
            tri[2] = 1;

            tri[3] = 2;
            tri[4] = 3;
            tri[5] = 1;
            m_mesh.triangles = tri;

            Vector3[] normals = new Vector3[4];
            normals[0] = -Vector3.forward;
            normals[1] = -Vector3.forward;
            normals[2] = -Vector3.forward;
            normals[3] = -Vector3.forward;
            m_mesh.normals = normals;

            Vector2[] uv = new Vector2[4];
            uv[0] = new Vector2(0, 0);
            uv[1] = new Vector2(1, 0);
            uv[2] = new Vector2(0, 1);
            uv[3] = new Vector2(1, 1);
            m_mesh.uv = uv;

            m_filter.sharedMesh = m_mesh;
            m_mat = mat;
            m_render.sharedMaterial = mat;
            m_bBuild = true;
        }

        /// <summary>
        /// 设置billBoard的纹理
        /// </summary>
        /// <param name="tex"></param>
        public void SetTexture(Texture tex)
        {
            if (m_tex == tex)
                return;

            m_tex = tex;
            m_mat.SetTexture("_MainTex", tex);

        }

        public void SetTextureOffset(Vector2 start, Vector2 end)
        {
            if (m_oldtexStart == start && m_oldtexEnd == end)
                return;

            m_oldtexStart = start;
            m_oldtexEnd = end;

            Mesh mesh = m_filter.mesh;
            Vector2[] uv = mesh.uv;
            uv[0] = new Vector2(start.x, start.y);
            uv[1] = new Vector2(end.x, start.y);
            uv[2] = new Vector2(start.x, end.y);
            uv[3] = new Vector2(end.x, end.y);
            mesh.uv = uv;
        }

        /// <summary>
        /// 设置大小，像素为单位
        /// </summary>
        /// <param name="width"></param>
        /// <param name="height"></param>
        public void SetPixelSize(int width, int height)
        {
            m_pixelWidth = width;
            m_pixelHeight = height;
        }

        public void SetScale(Vector2 sc)
        {
            CorrScale.x = sc.x;
            CorrScale.y = sc.y;
        }

        /// <summary>
        /// 设置名字
        /// </summary>
        /// <param name="name"></param>
        public void SetName(string name)
        {
            this.name = name;
        }
    }
}
