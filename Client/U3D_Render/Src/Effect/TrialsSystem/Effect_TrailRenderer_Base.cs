using System.Collections.Generic;
using System.Collections;
using System.Linq;
using UnityEngine;
using System;

namespace Effect
{
    namespace TrailsSystem
    {
        public abstract class Effect_TrailRenderer_Base : MonoBehaviour
        {
            public PCTrailRendererData TrailData;
            public bool Emit = false;

            public Effect_FadeInOutMaterialColor FadeInOutMaterialColor = null;
            public Effect_FadeInOutMaterialFloat FadeInOutMaterialFloat = null;
            public Effect_UVTextureAnimator UVTextureAnimator = null;
            public Effect_UVScroll UVScroll = null;
            public Effect_MaterialGradientColor GradientColor = null;

            protected bool _emit;
            protected bool _noDecay;

            private PCTrail _activeTrail;
            private List<PCTrail> _fadingTrails;
            protected Transform _t;


            private Dictionary<Material, List<PCTrail>> _matToTrailList;
            private List<Mesh> _toClean;

            /// <summary>
            /// 网格缓存
            /// </summary>
            private Queue<Mesh> combineMeshCacheList = new Queue<Mesh>();
            public void Awake()
            {
                OnAwake();
            }

            public void Start()
            {
                OnStart();
            }

            public void LateUpdate()
            {

                AfterUpdate();
            }

            //private void Update()
            //{
            //}
            public void OnDestroy()
            {

                OnClear();
            }

            protected virtual void OnAwake()
            {
                
                _matToTrailList = new Dictionary<Material, List<PCTrail>>();
                _toClean = new List<Mesh>();

                _fadingTrails = new List<PCTrail>();
                _t = transform;
                _emit = Emit;

                if (_emit)
                {
                    _activeTrail = new PCTrail(GetMaxNumberOfPoints());
                    _activeTrail.IsActiveTrail = true;
                    OnStartEmit();
                }

                CheckMaterialControl();
            }

            protected virtual void OnStart()
            {

            }
            
            protected virtual void AfterUpdate()
            {
                if (!TrailData.InstanceMaterial)
                {
                    Emit = false;
                    return;
                }
                CacluateTrialData();
                if(_fadingTrails.Count <= 0 && !Emit)
                {
                    return;
                }
                foreach (KeyValuePair<Material, List<PCTrail>> keyValuePair in _matToTrailList)
                {
                    CombineInstance[] combineInstances = new CombineInstance[keyValuePair.Value.Count];

                    for (int i = 0; i < keyValuePair.Value.Count; i++)
                    {
                        combineInstances[i] = new CombineInstance
                        {
                            mesh = keyValuePair.Value[i].mesh,
                            subMeshIndex = 0,
                            transform = Matrix4x4.identity
                        };
                    }

                    if (combineInstances.Length > 0)
                    {
                        if (combineInstances[0].mesh != null)
                        {
                            Mesh combinedMesh;
                            if (combineMeshCacheList.Count > 0)
                            {
                                combinedMesh = combineMeshCacheList.Dequeue();
                                combinedMesh.Clear(false);
                            }
                            else
                            {
                                combinedMesh = new Mesh();
                                //combinedMesh.hideFlags = HideFlags.HideAndDontSave;
                            }

                            combinedMesh.CombineMeshes(combineInstances, true, false);
                            //_toClean.Add(combinedMesh);
                            combineMeshCacheList.Enqueue(combinedMesh);
                            DrawMesh(combinedMesh, keyValuePair.Key);
                        }
                        keyValuePair.Value.Clear();
                    }
                    combineInstances = null;
                }
            }
            
            protected virtual void OnClear()
            {

                if (_toClean != null && _toClean.Count > 0)
                {
                    foreach (Mesh mesh in _toClean)
                    {
                        GameUtil.DestroyMesh(mesh);
                    }
                }
                _toClean = null;

                if (combineMeshCacheList != null && combineMeshCacheList.Count > 0)
                {
                    foreach (Mesh mesh in combineMeshCacheList)
                    {
                        GameUtil.DestroyMesh(mesh);
                    }
                }

                _matToTrailList.Clear();
                _matToTrailList = null;

                if (_activeTrail != null)
                {
                    _activeTrail.ClearMesh();
                    _activeTrail.Dispose();
                    _activeTrail = null;
                }

                if (_fadingTrails != null)
                {
                    foreach (PCTrail fadingTrail in _fadingTrails)
                    {
                        if (fadingTrail != null)
                        {
                            fadingTrail.ClearMesh();
                            fadingTrail.Dispose();
                        }
                            
                    }

                    _fadingTrails.Clear();
                }
                TrailData.ClearInstanceMaterial();
                TrailData = null;
            }
            protected virtual void CacluateTrialData()
            {

                if (_toClean != null && _toClean.Count > 0)
                {
                    foreach (Mesh mesh in _toClean)
                    {
                        GameUtil.DestroyMesh(mesh);
                    }
                }

                if (_matToTrailList.ContainsKey(TrailData.InstanceMaterial) == false)
                {
                    _matToTrailList.Add(TrailData.InstanceMaterial, new List<PCTrail>());
                }


                if (_activeTrail != null)
                {
                    UpdatePoints(_activeTrail, Time.deltaTime);
                    UpdateTrail(_activeTrail, Time.deltaTime);
                    GenerateMesh(_activeTrail);
                    _matToTrailList[TrailData.InstanceMaterial].Add(_activeTrail);
                }

                for (int i = _fadingTrails.Count - 1; i >= 0; i--)
                {
                    if (_fadingTrails[i] == null || _fadingTrails[i].Points.Any(a => a.TimeActive() < TrailData.Lifetime) == false)
                    {
                        if (_fadingTrails[i] != null)
                        {
                            _fadingTrails[i].ClearMesh();
                            _fadingTrails[i].Dispose();
                        }
                            

                        _fadingTrails.RemoveAt(i);
                        continue;
                    }

                    UpdatePoints(_fadingTrails[i], Time.deltaTime);
                    UpdateTrail(_fadingTrails[i], Time.deltaTime);
                    GenerateMesh(_fadingTrails[i]);
                    _matToTrailList[TrailData.InstanceMaterial].Add(_fadingTrails[i]);
                }

                CheckEmitChange();
            }



            protected virtual void OnStopEmit()
            {
                TrailData.ClearInstanceMaterial();
                StopMaterialControl();
            }

            protected virtual void OnStartEmit()
            {
                PlayMaterialControl();
            }

            protected virtual void OnTranslate(Vector3 t)
            {
            }


            protected abstract int GetMaxNumberOfPoints();

            public virtual void Reset()
            {
                if (TrailData == null)
                    TrailData = new PCTrailRendererData();

                TrailData.Lifetime = 1;

                TrailData.UsingSimpleColor = false;
                TrailData.UsingSimpleSize = false;

                TrailData.ColorOverLife = new Gradient();
                TrailData.SimpleColorOverLifeStart = Color.white;
                TrailData.SimpleColorOverLifeEnd = new Color(1, 1, 1, 0);

                TrailData.SizeOverLife = new AnimationCurve(new Keyframe(0, 1), new Keyframe(1, 0));
                TrailData.SimpleSizeOverLifeStart = 1;
                TrailData.SimpleSizeOverLifeEnd = 0;
            }

            protected virtual void InitialiseNewPoint(PCTrailPoint newPoint)
            {

            }

            protected virtual void UpdateTrail(PCTrail trail, float deltaTime)
            {

            }

            protected void AddPoint(PCTrailPoint newPoint, Vector3 pos)
            {
                if (_activeTrail == null)
                    return;

                newPoint.Position = pos;
                newPoint.PointNumber = _activeTrail.Points.Count == 0 ? 0 : _activeTrail.Points[_activeTrail.Points.Count - 1].PointNumber + 1;
                InitialiseNewPoint(newPoint);

                newPoint.SetDistanceFromStart(_activeTrail.Points.Count == 0
                                                  ? 0
                                                  : _activeTrail.Points[_activeTrail.Points.Count - 1].GetDistanceFromStart() + Vector3.Distance(_activeTrail.Points[_activeTrail.Points.Count - 1].Position, pos));

                if (TrailData.UseForwardOverride)
                {
                    newPoint.Forward = TrailData.ForwardOverrideRelative
                                           ? _t.TransformDirection(TrailData.ForwardOverride.normalized)
                                           : TrailData.ForwardOverride.normalized;
                }

                _activeTrail.Points.Add(newPoint);
            }

            private void GenerateMesh(PCTrail trail)
            {
                trail.mesh.Clear(false);

                Vector3 camForward =Initialize.mainCam!= null ? Initialize.mainCam.transform.forward : Vector3.forward;

                if (TrailData.UseForwardOverride)
                {
                    camForward = TrailData.ForwardOverride.normalized;
                }

                trail.activePointCount = NumberOfActivePoints(trail);

                if (trail.activePointCount < 2)
                    return;


                int vertIndex = 0;
                for (int i = 0; i < trail.Points.Count; i++)
                {
                    PCTrailPoint p = trail.Points[i];
                    float timeAlong = p.TimeActive() / TrailData.Lifetime;

                    if (p.TimeActive() > TrailData.Lifetime)
                    {
                        continue;
                    }

                    if (TrailData.UseForwardOverride && TrailData.ForwardOverrideRelative)
                        camForward = p.Forward;

                    Vector3 cross = Vector3.zero;

                    if (i < trail.Points.Count - 1)
                    {
                        cross =
                            Vector3.Cross((trail.Points[i + 1].Position - p.Position).normalized, camForward).
                                normalized;
                    }
                    else
                    {
                        cross =
                            Vector3.Cross((p.Position - trail.Points[i - 1].Position).normalized, camForward).
                                normalized;
                    }


                    //yuck! lets move these into their own functions some time
                    Color c = TrailData.StretchColorToFit ?
                        (TrailData.UsingSimpleColor ? Color.Lerp(TrailData.SimpleColorOverLifeStart, TrailData.SimpleColorOverLifeEnd, 1 - ((float)vertIndex / (float)trail.activePointCount / 2f)) : TrailData.ColorOverLife.Evaluate(1 - ((float)vertIndex / (float)trail.activePointCount / 2f))) :
                        (TrailData.UsingSimpleColor ? Color.Lerp(TrailData.SimpleColorOverLifeStart, TrailData.SimpleColorOverLifeEnd, timeAlong) : TrailData.ColorOverLife.Evaluate(timeAlong));

                    float s = TrailData.StretchSizeToFit ?
                        (TrailData.UsingSimpleSize ? Mathf.Lerp(TrailData.SimpleSizeOverLifeStart, TrailData.SimpleSizeOverLifeEnd, 1 - ((float)vertIndex / (float)trail.activePointCount / 2f)) : TrailData.SizeOverLife.Evaluate(1 - ((float)vertIndex / (float)trail.activePointCount / 2f))) :
                        (TrailData.UsingSimpleSize ? Mathf.Lerp(TrailData.SimpleSizeOverLifeStart, TrailData.SimpleSizeOverLifeEnd, timeAlong) : TrailData.SizeOverLife.Evaluate(timeAlong));


                    trail.verticies[vertIndex] = p.Position + cross * s;

                    if (TrailData.MaterialTileLength <= 0)
                    {
                        trail.uvs[vertIndex] = new Vector2((float)vertIndex / (float)trail.activePointCount / 2f, 0);
                    }
                    else
                    {
                        trail.uvs[vertIndex] = new Vector2(p.GetDistanceFromStart() / TrailData.MaterialTileLength, 0);
                    }

                    trail.normals[vertIndex] = camForward;
                    trail.colors[vertIndex] = c;
                    vertIndex++;
                    trail.verticies[vertIndex] = p.Position - cross * s;

                    if (TrailData.MaterialTileLength <= 0)
                    {
                        trail.uvs[vertIndex] = new Vector2((float)vertIndex / (float)trail.activePointCount / 2f, 1);
                    }
                    else
                    {
                        trail.uvs[vertIndex] = new Vector2(p.GetDistanceFromStart() / TrailData.MaterialTileLength, 1);
                    }

                    trail.normals[vertIndex] = camForward;
                    trail.colors[vertIndex] = c;

                    vertIndex++;
                }

                Vector2 finalPosition = trail.verticies[vertIndex - 1];
                for (int i = vertIndex; i < trail.verticies.Length; i++)
                {
                    trail.verticies[i] = finalPosition;
                }

                int indIndex = 0;
                for (int pointIndex = 0; pointIndex < 2 * (trail.activePointCount - 1); pointIndex++)
                {
                    if (pointIndex % 2 == 0)
                    {
                        trail.indicies[indIndex] = pointIndex;
                        indIndex++;
                        trail.indicies[indIndex] = pointIndex + 1;
                        indIndex++;
                        trail.indicies[indIndex] = pointIndex + 2;
                    }
                    else
                    {
                        trail.indicies[indIndex] = pointIndex + 2;
                        indIndex++;
                        trail.indicies[indIndex] = pointIndex + 1;
                        indIndex++;
                        trail.indicies[indIndex] = pointIndex;
                    }

                    indIndex++;
                }

                int finalIndex = trail.indicies[indIndex - 1];
                for (int i = indIndex; i < trail.indicies.Length; i++)
                {
                    trail.indicies[i] = finalIndex;
                }

                trail.mesh.vertices = trail.verticies;
                trail.mesh.SetIndices(trail.indicies, MeshTopology.Triangles, 0);
                trail.mesh.uv = trail.uvs;
                trail.mesh.normals = trail.normals;
                trail.mesh.colors = trail.colors;

            }

            private void DrawMesh(Mesh trailMesh, Material trailMaterial)
            {
                Graphics.DrawMesh(trailMesh, Matrix4x4.identity, trailMaterial, gameObject.layer);
                EffectUtily.DispatchMesh(trailMesh, Matrix4x4.identity, trailMaterial, gameObject.layer);
            }

            private void UpdatePoints(PCTrail line, float deltaTime)
            {
                for (int i = 0; i < line.Points.Count; i++)
                {
                    line.Points[i].Update(_noDecay ? 0 : deltaTime);
                }
            }
            private void CheckEmitChange()
            {
                if (_emit != Emit)
                {
                    _emit = Emit;
                    if (_emit)
                    {
                        _activeTrail = new PCTrail(GetMaxNumberOfPoints());
                        _activeTrail.IsActiveTrail = true;

                        OnStartEmit();
                    }
                    else
                    {
                        OnStopEmit();
                        if (null != _activeTrail)
                        {
                            _activeTrail.IsActiveTrail = false;
                            _fadingTrails.Add(_activeTrail);
                            _activeTrail = null;
                        }

                    }
                }
            }

            private int NumberOfActivePoints(PCTrail line)
            {
                int count = 0;
                for (int index = 0; index < line.Points.Count; index++)
                {
                    if (line.Points[index].TimeActive() < TrailData.Lifetime) count++;
                }
                return count;
            }

            public void LifeDecayEnabled(bool enabled)
            {
                _noDecay = !enabled;
            }

            /// <summary>
            /// Translates every point in the vector t
            /// </summary>
            public void Translate(Vector3 t)
            {
                if (_activeTrail != null)
                {
                    for (int i = 0; i < _activeTrail.Points.Count; i++)
                    {
                        _activeTrail.Points[i].Position += t;
                    }
                }

                if (_fadingTrails != null)
                {
                    foreach (PCTrail fadingTrail in _fadingTrails)
                    {
                        for (int i = 0; i < fadingTrail.Points.Count; i++)
                        {
                            fadingTrail.Points[i].Position += t;
                        }
                    }
                }

                OnTranslate(t);
            }

            /// <summary>
            /// Insert a trail into this trail renderer. 
            /// </summary>
            /// <param name="from">The start position of the trail.</param>
            /// <param name="to">The end position of the trail.</param>
            /// <param name="distanceBetweenPoints">Distance between each point on the trail</param>
            public void CreateTrail(Vector3 from, Vector3 to, float distanceBetweenPoints)
            {
                float distanceBetween = Vector3.Distance(from, to);

                Vector3 dirVector = to - from;
                dirVector = dirVector.normalized;

                float currentLength = 0;

                CircularBuffer<PCTrailPoint> newLine = new CircularBuffer<PCTrailPoint>(GetMaxNumberOfPoints());
                int pointNumber = 0;
                while (currentLength < distanceBetween)
                {
                    PCTrailPoint newPoint = new PCTrailPoint();
                    newPoint.PointNumber = pointNumber;
                    newPoint.Position = from + dirVector * currentLength;
                    newLine.Add(newPoint);
                    InitialiseNewPoint(newPoint);

                    pointNumber++;

                    if (distanceBetweenPoints <= 0)
                        break;
                    else
                        currentLength += distanceBetweenPoints;
                }

                PCTrailPoint lastPoint = new PCTrailPoint();
                lastPoint.PointNumber = pointNumber;
                lastPoint.Position = to;
                newLine.Add(lastPoint);
                InitialiseNewPoint(lastPoint);

                PCTrail newTrail = new PCTrail(GetMaxNumberOfPoints());
                newTrail.Points = newLine;

                _fadingTrails.Add(newTrail);
            }

            /// <summary>
            /// Clears all active trails from the system.
            /// </summary>
            /// <param name="emitState">Desired emit state after clearing</param>
            public void ClearSystem(bool emitState)
            {
                if (_activeTrail != null)
                {
                    _activeTrail.ClearMesh();
                    _activeTrail.Dispose();
                    _activeTrail = null;
                }

                if (_fadingTrails != null)
                {
                    foreach (PCTrail fadingTrail in _fadingTrails)
                    {
                        if (fadingTrail != null)
                        {
                            fadingTrail.ClearMesh();
                            fadingTrail.Dispose();
                        }
                           
                    }

                    _fadingTrails.Clear();
                }

                Emit = emitState;
                _emit = !emitState;

                CheckEmitChange();
            }

            /// <summary>
            /// Get the number of active seperate trail segments.
            /// </summary>
            public int NumSegments()
            {
                int num = 0;
                if (_activeTrail != null && NumberOfActivePoints(_activeTrail) != 0)
                    num++;

                num += _fadingTrails.Count;
                return num;
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

                if (GradientColor)
                {
                    //如果运行模式不是外部指定的
                    if (GradientColor.RunMode == EffectRunMode.OutSiedePointed)
                    {
                        GradientColor.ClearMaterialInstance();
                        GradientColor.Stop();
                    }
                }
            }

            void PlayMaterialControl()
            {
                //把材质设置进动画里
                if (FadeInOutMaterialColor)
                {
                    //如果运行模式不是外部指定的
                    if (FadeInOutMaterialColor.RunMode == EffectRunMode.OutSiedePointed)
                    {
                        FadeInOutMaterialColor.SetMaterialInstance(TrailData.InstanceMaterial);
                        FadeInOutMaterialColor.Play();
                    }

                }

                if (FadeInOutMaterialFloat)
                {
                    //如果运行模式不是外部指定的
                    if (FadeInOutMaterialFloat.RunMode == EffectRunMode.OutSiedePointed)
                    {
                        FadeInOutMaterialFloat.SetMaterialInstance(TrailData.InstanceMaterial);
                        FadeInOutMaterialFloat.Play();
                    }

                }

                if (UVTextureAnimator)
                {
                    //如果运行模式不是外部指定的
                    if (UVTextureAnimator.RunMode == EffectRunMode.OutSiedePointed)
                    {
                        UVTextureAnimator.SetMaterialInstance(TrailData.InstanceMaterial);
                        UVTextureAnimator.Play();
                    }
                }

                if (UVScroll)
                {
                    //如果运行模式不是外部指定的
                    if (UVScroll.RunMode == EffectRunMode.OutSiedePointed)
                    {
                        UVScroll.SetMaterialInstance(TrailData.InstanceMaterial);
                        UVScroll.Play();
                    }
                }

                if (GradientColor)
                {
                    //如果运行模式不是外部指定的
                    if (GradientColor.RunMode == EffectRunMode.OutSiedePointed)
                    {
                        GradientColor.SetMaterialInstance(TrailData.InstanceMaterial);
                        GradientColor.Play();
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

        public class PCTrail : System.IDisposable
        {
            public CircularBuffer<PCTrailPoint> Points;
            public Mesh mesh;

            public Vector3[] verticies;
            public Vector3[] normals;
            public Vector2[] uvs;
            public Color[] colors;
            public int[] indicies;
            public int activePointCount;

            public bool IsActiveTrail = false;

            public PCTrail(int numPoints)
            {
                mesh = new Mesh();
                //mesh.hideFlags = HideFlags.HideAndDontSave;
                mesh.MarkDynamic();

                verticies = new Vector3[2 * numPoints];
                normals = new Vector3[2 * numPoints];
                uvs = new Vector2[2 * numPoints];
                colors = new Color[2 * numPoints];
                indicies = new int[2 * (numPoints) * 3];

                Points = new CircularBuffer<PCTrailPoint>(numPoints);
            }

            #region Implementation of IDisposable

            public void ClearMesh()
            {
                GameUtil.DestroyMesh(mesh);
            }

            public void Dispose()
            {
                Points.Clear();
                Points = null;
            }

            #endregion
        }

        public class PCTrailPoint
        {
            public Vector3 Forward;
            public Vector3 Position;
            public int PointNumber;

            private float _timeActive = 0;
            private float _distance;

            public virtual void Update(float deltaTime)
            {
                _timeActive += deltaTime;
            }

            public float TimeActive()
            {
                return _timeActive;
            }

            public void SetTimeActive(float time)
            {
                _timeActive = time;
            }

            public void SetDistanceFromStart(float distance)
            {
                _distance = distance;
            }

            public float GetDistanceFromStart()
            {
                return _distance;
            }
        }

        [System.Serializable]
        public class PCTrailRendererData
        {
            public Material TrailMaterial;

            public Material InstanceMaterial
            {
                get
                {
                    if (!m_InstanceMaterial)
                    {
                        if (TrailMaterial)
                        {
                            m_InstanceMaterial = new Material(TrailMaterial);
                            m_InstanceMaterial.name = TrailMaterial.name + " (Instance)";
                        }
                    }
                    return m_InstanceMaterial;
                }
                set
                {
                    if (!value)
                    {
                        if (m_InstanceMaterial)
                        {
                            ResNode.DestroyRes(ref m_InstanceMaterial);
                            m_InstanceMaterial = null;
                        }

                    }
                }
            }

            private Material m_InstanceMaterial = null;

            public void ClearInstanceMaterial()
            {
                InstanceMaterial = null;
            }

            //~PCTrailRendererData()
            //{
               
            //}
            [Range(0.1f, 100.0f)]
            public float Lifetime = 1;
            public bool UsingSimpleSize = false;
            public float SimpleSizeOverLifeStart;
            public float SimpleSizeOverLifeEnd;
            public AnimationCurve SizeOverLife = new AnimationCurve();
            public bool UsingSimpleColor = false;
            public Color SimpleColorOverLifeStart;
            public Color SimpleColorOverLifeEnd;
            public Gradient ColorOverLife;
            public bool StretchSizeToFit;
            public bool StretchColorToFit;
            public float MaterialTileLength = 0;
            public bool UseForwardOverride;
            public Vector3 ForwardOverride;
            public bool ForwardOverrideRelative;
        }
    }
}



