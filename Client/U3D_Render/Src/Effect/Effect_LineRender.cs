/*******************************************************************
** 文件名:	Effect_LineRender.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/10/10
** 版  本:	1.0
** 描  述:	条带脚本
** 应  用:  	条带脚本激光条带脚本

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace Effect
{
    /// <summary>
    /// 条带脚本
    /// </summary>
    [RequireComponent(typeof(LineRenderer))]
    public sealed class Effect_LineRender : MonoBehaviour
    {
        [System.Serializable]
        public struct ColorVertex
        {
            public float pert;
            public Color color;
        }



        public EffectRunMode RunMode = EffectRunMode.OutSiedePointed;
        public ValueMode valueMode = ValueMode.Constant;
        public LineRenderTargetType targetType = LineRenderTargetType.OutSiedePointed;

        public bool UseThickNess = true;
        public string []ThickNessTexture = new string[]{"_MainTex"};

        [Range(2,150)]
        public int VertexCount = 2;
        public bool useMultiColor=false;
        public Vector2 lineWidth=new Vector2(1,1);

        public AnimationCurve VertexCurve = new AnimationCurve(new Keyframe(0, 0), new Keyframe(1, 0));
        public float CurveScale = 1.0f;
        public bool VertexCurve_X = true;
        public bool VertexCurve_Y = true;
        public bool VertexCurve_Z = true;

        /// <summary>
        /// 参考距离，用于实际距离，来调整条带的密度
        /// </summary>
        public float RefDistance = 5.0f;

        /// <summary>
        /// 游戏模式中的Src
        /// </summary>
        public Transform GolbalSrcObj = null;

        /// <summary>
        /// 游戏模式中的Dst
        /// </summary>
        public Transform GolbalDstObj = null;

        public Transform LocalSrcObj = null;
        public Transform LocalDstObj = null;

        public Effect_FadeInOutMaterialColor FadeInOutMaterialColor = null;
        public Effect_FadeInOutMaterialFloat FadeInOutMaterialFloat = null;
        public Effect_UVTextureAnimator UVTextureAnimator = null;
        public Effect_UVScroll UVScroll = null;

        private bool bPlay = false;
        private bool bInit = false;
        private LineRenderer m_LineRender = null;
        private List<LineRenderer> m_subLineRenders=null;
        public ColorVertex[] subVectorColor = null;


        private float lastRealDistance = -9999.0f;
        private int lastVertexCount = 0;

        public void OnDestroy()
        {
            ResNode.DestroyRes(ref m_LineRender);
            m_LineRender = null;

            FadeInOutMaterialColor = null;
            FadeInOutMaterialFloat = null;
            UVTextureAnimator = null;
            UVScroll = null;
            LocalSrcObj = null;
            LocalDstObj = null;
            GolbalDstObj = null;
            GolbalSrcObj = null;
            StopMaterialControl();
            
        }

        bool ApplyGlobal()
        {
            if (GolbalSrcObj && GolbalDstObj)
            {
                Vector3 src = GolbalSrcObj.transform.position;
                Vector3 dst = GolbalDstObj.transform.position;

                UpdateVertex(src, dst);
                ModifyLineThickNess(Vector3.Distance(src ,dst));
                return true;
            }

            return false;
            
        }

        bool ApplyLocal() 
        {
            if(!GolbalDstObj || !GolbalSrcObj ||
                !LocalDstObj || !LocalSrcObj)
            {
                return false;
            }

            float dis = -1.0f;
            Vector3 dir = Vector3.zero;

            dir = GolbalDstObj.transform.position - GolbalSrcObj.transform.position;
            dir.Normalize();

            float ReferenceDistance = Vector3.Distance(LocalDstObj.transform.position, LocalSrcObj.transform.position);
            float GolbalDistance = Vector3.Distance(GolbalDstObj.transform.position, GolbalSrcObj.transform.position);
            dis = ReferenceDistance / GolbalDistance * ReferenceDistance;

            Vector3 src = GolbalSrcObj.transform.position;
            Vector3 dst = GolbalSrcObj.transform.position + dir * dis;

            UpdateVertex(src, dst);


            ModifyLineThickNess(Vector3.Distance(src, dst));    

            return true;
        }

        void createSubRenderer(bool force=false)
        {
            if (m_LineRender == null)
            {
                m_LineRender = GetComponent<LineRenderer>();
            }
            if (m_LineRender)
            {
                m_LineRender.enabled = true;
            }

            int childIndex = 0;
            while (childIndex < transform.childCount)
            {
                Transform child = transform.GetChild(childIndex);
                if (child.GetComponent<LineRenderer>() == null)
                {
                    childIndex++;
                }
                else
                {
                    child.parent = null;
                    DestroyImmediate(child.gameObject);
                }
            }

            if (valueMode == ValueMode.Constant && VertexCount > 2 && useMultiColor)
            {
                Vector3 startPoint = vecStartPoint;
                Vector3 endPoint = vecEndPoint;
                GameObject[] newObjects = new GameObject[VertexCount - 1];
                if (m_subLineRenders == null)
                {
                    m_subLineRenders = new List<LineRenderer>();
                }
                m_subLineRenders.Clear();
                for (int i = 0; i < VertexCount - 1; i++)
                {
                    newObjects[i] = ResNode.InstantiateRes(transform.gameObject);
                    foreach (Component comp in newObjects[i].GetComponents<Component>())
                    {
                        if (!(comp is LineRenderer || comp is Transform))
                        {
                            DestroyImmediate(comp);
                        }
                    }

                    int lchildIndex = 0;
                    while (lchildIndex < newObjects[i].transform.childCount)
                    {
                        Transform child = newObjects[i].transform.GetChild(lchildIndex);
                        if (child.GetComponent<Effect_UVScroll>() == null)
                        {
                            lchildIndex++;
                        }
                        else
                        {
                            child.parent = null;
                            DestroyImmediate(child.gameObject);
                        }
                    }

                    LineRenderer newLine = newObjects[i].GetComponent<LineRenderer>();

                    if (newLine)
                    {
                        m_subLineRenders.Add(newLine);
                        Vector3 left = Vector3.Lerp(startPoint, endPoint, subVectorColor[i].pert);
                        Vector3 right = Vector3.Lerp(startPoint, endPoint, subVectorColor[i+1].pert);
                        newLine.SetVertexCount(2);
                        newLine.SetPosition(0, left);
                        newLine.SetPosition(1, right);
                        if (VertexCount == subVectorColor.Length)
                        {
                            newLine.SetColors(subVectorColor[i].color, subVectorColor[i + 1].color);
                        }

                    }

                }
                for (int i = 0; i < VertexCount - 1; i++)
                {
                    newObjects[i].transform.parent = transform;
                }
            }

                m_LineRender.enabled = false;
        }

        public Vector3 vecStartPoint=Vector3.zero;
        public Vector3 vecEndPoint = Vector3.zero;


        public void UpdateVertex(Vector3 startPoint,Vector3 endPoint)
        {
            if(valueMode == ValueMode.Constant)
            {
                if (useMultiColor)
                {
                    float dis = (startPoint - vecStartPoint).sqrMagnitude + (endPoint - vecEndPoint).sqrMagnitude;
                    if (dis > 0.0001f)
                    {
                        vecStartPoint = startPoint;
                        vecEndPoint = endPoint;
                        m_LineRender.enabled = false;

                        if (m_subLineRenders == null)
                        {
                            m_subLineRenders = new List<LineRenderer>();
                            m_subLineRenders.Clear();
                            for (int i = 0; i < transform.childCount; i++)
                            {
                                Transform child = transform.GetChild(i);
                                LineRenderer newLine = child.GetComponent<LineRenderer>();
                                if (newLine != null)
                                {
                                    m_subLineRenders.Add(newLine);
                                }
                            }
                        }

                        VertexCount = m_subLineRenders.Count + 1;
                        for (int i = 0; i < m_subLineRenders.Count; i++)
                        {
                            LineRenderer newLine = m_subLineRenders[i];
                            Vector3 left = Vector3.Lerp(startPoint, endPoint, subVectorColor[i].pert);
                            Vector3 right = Vector3.Lerp(startPoint, endPoint, subVectorColor[i+1].pert);
                            newLine.SetPosition(0, left);
                            newLine.SetPosition(1, right);

                            float leftWidth = Mathf.Lerp(lineWidth.x, lineWidth.y, subVectorColor[i].pert);
                            float rightWidth = Mathf.Lerp(lineWidth.x, lineWidth.y, subVectorColor[i+1].pert);
                            newLine.SetWidth(leftWidth, rightWidth);
                        }

                    }
                }
                else
                {
                    VertexCount = 2;
                    if (lastVertexCount != VertexCount)
                    {
                        lastVertexCount = 2;
                        m_LineRender.SetVertexCount(2);
                    }
                    m_LineRender.SetPosition(0, startPoint);
                    m_LineRender.SetPosition(1, endPoint);
                }
            }
            else if (valueMode == ValueMode.Curve)
            {
                if (lastVertexCount != VertexCount)
                {
                    lastVertexCount = VertexCount;
                    m_LineRender.SetVertexCount(VertexCount);
                }

                if(VertexCount == 2)
                {
                    m_LineRender.SetPosition(0, startPoint);
                    m_LineRender.SetPosition(1, endPoint);
                    return;
                }
                int c = VertexCount - 1;
                for (int i = 0; i <= c; i++)
                {
                    float a = i;
                    float b = c;
                    Vector3 p = Vector3.Lerp(startPoint, endPoint, a / b);
                    float s = VertexCurve.Evaluate(a / b) * CurveScale;
                    if (VertexCurve_X)
                    {
                        p.x *= (1 + s);
                    }

                    if (VertexCurve_Y)
                    {
                        p.y *= (1 + s);
                    }

                    if (VertexCurve_Z)
                    {
                        p.z *= (1 + s);
                    }
                    m_LineRender.SetPosition(i, p);
                }
            }
 
        }


        /// <summary>
        /// 实际的距离
        /// </summary>
        /// <param name="RealDistance"></param>
        void ModifyLineThickNess(float RealDistance)
        {
            if(float.Equals(lastRealDistance,RealDistance) || !UseThickNess)
            {
                return;
            }
            lastRealDistance = RealDistance;
            float thickNess = RealDistance / RefDistance;
            thickNess = Mathf.Max(0.01f, thickNess);

            int length = m_LineRender.materials.Length;
            int strLength = ThickNessTexture.Length;
            for(int i = 0;i < length;i++)
            {
                Material mat = m_LineRender.materials[i];
                for(int j = 0;j < strLength;j++)
                {
                    string s = ThickNessTexture[j];
                    if(mat.HasProperty(s))
                    {
                        Vector2 v = mat.GetTextureScale(s);
                        v.x = RealDistance / RefDistance;
                        mat.SetTextureScale(s, v);
                    }
                    
                }
            }

            
            if (useMultiColor)
            {
                int lineCount = m_subLineRenders.Count;
                UVScroll.UseRendererOffset=true;
                float offsetCount = 0.0f;
                for (int linei=0;linei<lineCount;linei++)
                {
                    LineRenderer line = m_subLineRenders[linei];
                    float lineLength = subVectorColor[linei + 1].pert - subVectorColor[linei].pert;
                    float matScale = RealDistance / RefDistance * lineLength;

                    int sublength = line.materials.Length;
                    
                    for (int i = 0; i < sublength; i++)
                    {
                        Material mat = line.materials[i];
                        for (int j = 0; j < strLength; j++)
                        {
                            string s = ThickNessTexture[j];

                            if (mat.HasProperty(s))
                            {
                                Vector2 v = mat.GetTextureScale(s);
                                v.x = matScale;
                                mat.SetTextureScale(s, v);
                                Vector2 offset = Vector2.zero;
                                offset.x = offsetCount;                                
                                mat.SetTextureOffset(s, offset);
                            }                            
                        }                        
                    }
                    offsetCount += matScale;
                }
            }
        }


        public void Play()
        {
            if (!bInit)
            {
                return;
            }
            bPlay = true;

            if(RunMode == EffectRunMode.Self)
            {
                if(!ApplyLocal())
                {
                    return;
                }
            }
            else if(RunMode == EffectRunMode.OutSiedePointed)
            {
                if (!ApplyGlobal())
                {
                    return;
                }
            }
            else
            {
                return;
            }

            m_LineRender.enabled = true;
            foreach(Material m in m_LineRender.materials)
            {
                EffectUtily.DispatchRender(m_LineRender, m);
            }

            if (useMultiColor)
            {
                m_LineRender.enabled = false;
                foreach (LineRenderer line in m_subLineRenders)
                {
                    line.enabled = true;
                    //line.materials = m_LineRender.materials;
                    foreach (Material m in line.materials)
                    {
                        EffectUtily.DispatchRender(line, m);
                    }
                }
            }

            PlayMaterialControl();
            bPlay = true;
        }

        public void Stop()
        {
            if (!bInit)
            {
                return;
            }
            bPlay = false;
            lastRealDistance = 0.0f;
            //lastVertexCount = 0;
            foreach (Material m in m_LineRender.materials)
            {
                EffectUtily.GabageRender(m_LineRender, m);
            }
            m_LineRender.enabled = false;

            if (useMultiColor)
            {
                foreach (LineRenderer line in m_subLineRenders)
                {
                    foreach (Material m in line.materials)
                    {
                        EffectUtily.GabageRender(line, m);
                    }
                    line.enabled = false;
                }
            }
            StopMaterialControl();
        }

        public void Init()
        {
            if (bInit)
            {
                return;
            }
            CheckMaterialControl();
            m_LineRender = GetComponent<LineRenderer>();
            if(valueMode == ValueMode.Constant)
            {
                m_LineRender.SetVertexCount(2);
            }

            else if(valueMode == ValueMode.Curve)
            {
                m_LineRender.SetVertexCount(VertexCount);
                lastVertexCount = VertexCount;
            }

            m_LineRender.shadowCastingMode = UnityEngine.Rendering.ShadowCastingMode.Off;
            m_LineRender.receiveShadows = false;
            m_LineRender.lightProbeUsage = UnityEngine.Rendering.LightProbeUsage.Off;
            m_LineRender.enabled = false;

            createSubRenderer();
            bInit = true;
        }

        public void Update()
        {
            if (bPlay)
            {
                if (RunMode == EffectRunMode.Self)
                {
                    ApplyLocal();
                }
                else if (RunMode == EffectRunMode.OutSiedePointed)
                {
                    ApplyGlobal();
                }
                
            }
        }

        #region 材质控制器

        void StopMaterialControl()
        {
            //清空动画
            if (FadeInOutMaterialColor)
            {
                //如果运行模式不是外部指定的
                if (FadeInOutMaterialColor.RunMode == EffectRunMode.OutSiedePointed)
                {
                    FadeInOutMaterialColor.ClearMaterialInstance();
                    FadeInOutMaterialColor.Stop();
                }

            }

            if (FadeInOutMaterialFloat)
            {
                //如果运行模式不是外部指定的
                if (FadeInOutMaterialFloat.RunMode == EffectRunMode.OutSiedePointed)
                {
                    FadeInOutMaterialFloat.ClearMaterialInstance();
                    FadeInOutMaterialFloat.Stop();
                }

            }

            if (UVTextureAnimator)
            {
                //如果运行模式不是外部指定的
                if (UVTextureAnimator.RunMode == EffectRunMode.OutSiedePointed)
                {
                    UVTextureAnimator.ClearMaterialInstance();
                    UVTextureAnimator.Stop();
                }
            }

            if (UVScroll)
            {
                //如果运行模式不是外部指定的
                if (UVScroll.RunMode == EffectRunMode.OutSiedePointed)
                {
                    UVScroll.ClearMaterialInstance();
                    UVScroll.Stop();
                }
            }
        }

        void PlayMaterialControl()
        {

            List<Material> mats = new List<Material>();
            mats.Clear();
            if (useMultiColor)
            {                
                foreach (LineRenderer line in m_subLineRenders)
                {
                    mats.AddRange(line.materials);
                }
                UVScroll.SetMaterialInstance(mats);
            }
            else
            {
                mats.AddRange(m_LineRender.materials);
            }

            //把材质设置进动画里
            if (FadeInOutMaterialColor)
            {
                //如果运行模式不是外部指定的
                if (FadeInOutMaterialColor.RunMode == EffectRunMode.OutSiedePointed)
                {
                    FadeInOutMaterialColor.SetMaterialInstance(mats);
                    FadeInOutMaterialColor.Play();
                }

            }

            if (FadeInOutMaterialFloat)
            {
                //如果运行模式不是外部指定的
                if (FadeInOutMaterialFloat.RunMode == EffectRunMode.OutSiedePointed)
                {
                    FadeInOutMaterialFloat.SetMaterialInstance(mats);
                    FadeInOutMaterialFloat.Play();
                }

            }

            if (UVTextureAnimator)
            {
                //如果运行模式不是外部指定的
                if (UVTextureAnimator.RunMode == EffectRunMode.OutSiedePointed)
                {
                    UVTextureAnimator.SetMaterialInstance(mats);
                    UVTextureAnimator.Play();
                }
            }

            if (UVScroll)
            {
                //如果运行模式不是外部指定的
                if (UVScroll.RunMode == EffectRunMode.OutSiedePointed)
                {
                    UVScroll.SetMaterialInstance(mats);
                    UVScroll.Play();
                }
            }
        }

        void CheckMaterialControl()
        {
            if (UVTextureAnimator)
            {
                if (UVTextureAnimator.RunMode != EffectRunMode.OutSiedePointed)
                {
                    Debug.LogError("UVTextureAnimator成员的运行模式不是OutSiedePointed!" + gameObject.name);
                }
            }

            if (FadeInOutMaterialColor)
            {
                if (FadeInOutMaterialColor.RunMode != EffectRunMode.OutSiedePointed)
                {
                    Debug.LogError("FadeInOutMaterialColor成员的运行模式不是OutSiedePointed!" + gameObject.name);
                }
            }

            if (FadeInOutMaterialFloat)
            {
                if (FadeInOutMaterialFloat.RunMode != EffectRunMode.OutSiedePointed)
                {
                    Debug.LogError("FadeInOutMaterialFloat成员的运行模式不是OutSiedePointed!" + gameObject.name);
                }
            }

            if (UVScroll)
            {
                if (UVScroll.RunMode != EffectRunMode.OutSiedePointed)
                {
                    Debug.LogError("UVScroll成员的运行模式不是OutSiedePointed!" + gameObject.name);
                }
            }
        }

        #endregion
    }



}

