using UnityEngine;
using System.Collections;
using System;
using System.Collections.Generic;
using System.Linq;


namespace Spline
{
    /// <summary>
    /// 样线顶点类型
    /// </summary>
    public enum ESplinePointType
    {
        Linear,
        Curve,
        Constant,
        CurveClamped,
        CurveCustomTangent
    }

    public enum ESplineCoordinateSpace
    {
        Local,
        World
    }

    [Serializable]
    public class FSplineCurves
    {
        /// <summary>
        /// 样线坐标数据
        /// </summary>
        [SerializeField]
        public FInterpCurveVector Position;

        /// <summary>
        /// 样线携带的旋转数据
        /// </summary>
        [SerializeField]
        public FInterpCurveQuat Rotation;

        /// <summary>
        /// 样线携带的缩放数据
        /// </summary>
        [SerializeField]
        public FInterpCurveVector Scale;

        /// <summary>
        /// 样线长度数据
        /// </summary>
        [SerializeField]
        public FInterpCurveFloat ReparamTable;

        public FSplineCurves(bool test = false)
        {
            Position = new FInterpCurveVector();
            Rotation = new FInterpCurveQuat();
            Scale = new FInterpCurveVector();
            ReparamTable = new FInterpCurveFloat();
        }

        //public bool Equals(FSplineCurves other)
        //{
        //    if (Position.Points.Count != other.Position.Points.Count)
        //        return false;
        //    if (Rotation.Points.Count != other.Rotation.Points.Count)
        //        return false;
        //    if (Scale.Points.Count != other.Scale.Points.Count)
        //        return false;
        //    if (ReparamTable.Points.Count != other.ReparamTable.Points.Count)
        //        return false;
        //    return true;
        //}

        //public override bool Equals(object obj)
        //{
        //    if (!(obj is FSplineCurves))
        //        return false;

        //    return Equals((FSplineCurves)obj);
        //}

        //public override int GetHashCode()
        //{
        //    return base.GetHashCode();
        //}

        //public static bool operator ==(FSplineCurves _curve1, FSplineCurves _curve2)
        //{
        //    return _curve1.Equals(_curve2);
        //}

        //public static bool operator !=(FSplineCurves _curve1, FSplineCurves _curve2)
        //{
        //    return !_curve1.Equals(_curve2);
        //}

        //    bool operator ==(const FSplineCurves& Other) const
        // 	{
        //		return Position == Other.Position && Rotation == Other.Rotation && Scale == Other.Scale;
        //	}

        //bool operator !=(const FSplineCurves& Other) const
        //	{
        //		return !(* this == Other);
        //}
    };

    [Serializable]
    public struct FSplinePoint
    {
        public float InputKey;

        public Vector3 Position;

        public Vector3 ArriveTangent;

        public Vector3 LeaveTangent;

        public Quaternion Rotation;

        public Vector3 Scale;

        public ESplinePointType Type;

        public FSplinePoint(
            float _fInputKey = 0f,
            Vector3 _vPos = new Vector3(),
            Vector3 _vArriveTangent = new Vector3(),
            Vector3 _vLeaveTangent = new Vector3(),
            Quaternion _qRotation = new Quaternion(),
            ESplinePointType _eType = ESplinePointType.Curve)
        {
            InputKey = _fInputKey;
            Position = _vPos;
            ArriveTangent = _vArriveTangent;
            LeaveTangent = _vLeaveTangent;
            Rotation = _qRotation;
            Scale = Vector3.one;
            Type = _eType;
        }
    };

    public class SplineComponent : MonoBehaviour
    {
        [SerializeField]
        protected FSplineCurves SplineCurves;

        /// <summary>
        /// 两关键点之间样线的分段数
        /// </summary>
        int ReparamStepsPerSegment = 10;

        /// <summary>
        /// 是否循环样线
        /// </summary>
        [SerializeField]
        protected bool bClosedLoop;

        /// <summary>
        /// 是否重写循环样线的终点偏移值
        /// </summary>
        bool bLoopPositionOverride;

        /// <summary>
        /// 循环样线的终点偏移值
        /// </summary>
        float LoopPosition;


        bool bStationaryEndpoints;

        public bool DrawDebug = true;

        Vector3 DefaultUpVector;

        [SerializeField]
        protected bool m_bInit = false;
        public bool IsInit
        {
            get { return m_bInit; }
        }
        public void Awake()
        {
            Init();
        }

        public void OnEnable()
        {
            Init();
        }
        public void Init()
        {
            if (m_bInit)
                return;
            m_bInit = true;

            bStationaryEndpoints = false;
            ReparamStepsPerSegment = 10;
            bClosedLoop = false;

            DefaultUpVector = Vector3.up;

            SplineCurves = new FSplineCurves(true);
            SplineCurves.Position.Points.Add(new FInterpCurveVectorPoint(0.0f, Vector3.zero, Vector3.zero, new Vector3(1, 0, 0), ESplinePointType.Curve));
            SplineCurves.Rotation.Points.Add(new FInterpCurveQuaternionPoint(0.0f, Quaternion.identity, Quaternion.identity, Quaternion.identity, ESplinePointType.Curve));
            SplineCurves.Scale.Points.Add(new FInterpCurveVectorPoint(0.0f, Vector3.one, Vector3.zero, Vector3.zero, ESplinePointType.Curve));

            SplineCurves.Position.Points.Add(new FInterpCurveVectorPoint(1.0f, new Vector3(10, 0, 0), new Vector3(1, 0, 0), Vector3.zero, ESplinePointType.Curve));
            SplineCurves.Rotation.Points.Add(new FInterpCurveQuaternionPoint(1.0f, Quaternion.identity, Quaternion.identity, Quaternion.identity, ESplinePointType.Curve));
            SplineCurves.Scale.Points.Add(new FInterpCurveVectorPoint(1.0f, Vector3.one, Vector3.zero, Vector3.zero, ESplinePointType.Curve));

            UpdateSpline();
        }

        void UpdateSpline()
        {
            int NumPoints = SplineCurves.Position.Points.Count;
            //check(SplineCurves.Rotation.Points.Num() == NumPoints && SplineCurves.Scale.Points.Num() == NumPoints);

            //#if DO_CHECK
            //	// Ensure input keys are strictly ascending
            //	for (int Index = 1; Index < NumPoints; Index++)
            //	{
            //		check(SplineCurves.Position.Points[Index - 1].InVal < SplineCurves.Position.Points[Index].InVal);
            //	}
            //#endif

            if (bClosedLoop)
            {
                float LastKey = SplineCurves.Position.Points.Count > 0 ? SplineCurves.Position.Points.Last().InVal : 0.0f;
                float LoopKey = bLoopPositionOverride ? LoopPosition : LastKey + 1.0f;
                SplineCurves.Position.SetLoopKey(LoopKey);
                SplineCurves.Rotation.SetLoopKey(LoopKey);
                SplineCurves.Scale.SetLoopKey(LoopKey);
            }
            else
            {
                SplineCurves.Position.ClearLoopKey();
                SplineCurves.Rotation.ClearLoopKey();
                SplineCurves.Scale.ClearLoopKey();
            }

            // 自动设置切线值
            SplineCurves.Position.AutoSetTangents(0.0f, bStationaryEndpoints);
            SplineCurves.Rotation.AutoSetTangents(0.0f, bStationaryEndpoints);
            SplineCurves.Scale.AutoSetTangents(0.0f, bStationaryEndpoints);

            // 根据是否循环样线得关键点数
            int NumSegments = bClosedLoop ? NumPoints : NumPoints - 1;

            SplineCurves.ReparamTable.Points.Clear();

            float AccumulatedLength = 0.0f;
            //计算当前样线长度
            //每两关键点之间分ReparamStepsPerSegment份来算长度
            for (int SegmentIndex = 0; SegmentIndex < NumSegments; ++SegmentIndex)
            {
                for (int Step = 0; Step < ReparamStepsPerSegment; ++Step)
                {
                    float Param = ((float)(Step) / ReparamStepsPerSegment);
                    float SegmentLength = (Step == 0) ? 0.0f : GetSegmentLength(SegmentIndex, Param);

                    SplineCurves.ReparamTable.Points.Add(new FInterpCurveFloatPoint(SegmentLength + AccumulatedLength, SegmentIndex + Param, 0.0f, 0.0f, ESplinePointType.Linear));
                }
                AccumulatedLength += GetSegmentLength(SegmentIndex, 1.0f);
            }

            SplineCurves.ReparamTable.Points.Add(new FInterpCurveFloatPoint(AccumulatedLength, (float)(NumSegments), 0.0f, 0.0f, ESplinePointType.Linear));
        }
        struct FLegendreGaussCoefficient
        {
            public float Abscissa;
            public float Weight;

            public FLegendreGaussCoefficient(float _abs, float _weight)
            {
                Abscissa = _abs;
                Weight = _weight;
            }
        };
        static FLegendreGaussCoefficient[] LegendreGaussCoefficients = new FLegendreGaussCoefficient[5]
        {
        new FLegendreGaussCoefficient( 0.0f, 0.5688889f ),
        new FLegendreGaussCoefficient(-0.5384693f, 0.47862867f ),
        new FLegendreGaussCoefficient(  0.5384693f, 0.47862867f ),
        new FLegendreGaussCoefficient(  -0.90617985f, 0.23692688f),
        new FLegendreGaussCoefficient(  0.90617985f, 0.23692688f )
        };

        float GetSegmentLength(int Index, float Param)
        {

            int NumPoints = SplineCurves.Position.Points.Count;
            int LastPoint = NumPoints - 1;


            //check(Index >= 0 && ((bClosedLoop && Index<NumPoints) || (!bClosedLoop && Index<LastPoint)));
            //check(Param >= 0.0f && Param <= 1.0f);

            // Evaluate the length of a Hermite spline segment.
            // This calculates the integral of |dP/dt| dt, where P(t) is the spline equation with components (x(t), y(t), z(t)).
            // This isn't solvable analytically, so we use a numerical method (Legendre-Gauss quadrature) which performs very well
            // with functions of this type, even with very few samples.  In this case, just 5 samples is sufficient to yield a
            // reasonable result.


            var StartPoint = SplineCurves.Position.Points[Index];
            var EndPoint = SplineCurves.Position.Points[Index == LastPoint ? 0 : Index + 1];

            Vector3 Scale3D = transform.localScale;

            var P0 = StartPoint.OutVal;
            var T0 = StartPoint.LeaveTangent;
            var P1 = EndPoint.OutVal;
            var T1 = EndPoint.ArriveTangent;

            // Special cases for linear or constant segments
            if (StartPoint.InterpMode == ESplinePointType.Linear)
            {
                Vector3 newVec = (P1 - P0);
                newVec.x = newVec.x * Scale3D.x;
                newVec.y = newVec.y * Scale3D.y;
                newVec.z = newVec.z * Scale3D.z;
                return (newVec.magnitude * Param);
            }
            else if (StartPoint.InterpMode == ESplinePointType.Constant)
            {
                return 0.0f;
            }

            // Cache the coefficients to be fed into the function to calculate the spline derivative at each sample point as they are constant.
            Vector3 Coeff1 = ((P0 - P1) * 2.0f + T0 + T1) * 3.0f;
            Vector3 Coeff2 = (P1 - P0) * 6.0f - T0 * 4.0f - T1 * 2.0f;
            Vector3 Coeff3 = T0;

            float HalfParam = Param * 0.5f;

            float Length = 0.0f;
            foreach (var LegendreGaussCoefficient in LegendreGaussCoefficients)
            {
                // 计算导数在每个Legendre Gauss Coefficient，并执行加权总和
                float Alpha = HalfParam * (1.0f + LegendreGaussCoefficient.Abscissa);

                Vector3 Derivative = ((Coeff1 * Alpha + Coeff2) * Alpha + Coeff3);

                Derivative.x = Derivative.x * Scale3D.x;
                Derivative.y = Derivative.y * Scale3D.y;
                Derivative.z = Derivative.z * Scale3D.z;

                Length += Derivative.magnitude * LegendreGaussCoefficient.Weight;
            }
            Length *= HalfParam;

            return Length;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////


        public FInterpCurveVector GetSplineCurvesPosition() { return SplineCurves.Position; }
        public FInterpCurveQuat GetSplineCurvesRotation() { return SplineCurves.Rotation; }
        public FInterpCurveVector GetSplineCurvesScale() { return SplineCurves.Scale; }
        public FInterpCurveFloat GetSplineCurvesReparamTable() { return SplineCurves.ReparamTable; }

        //////////////////////////////////////////////////////////////////////////////////////////////////////
        #region 沿样线 键值 的操作
        /// <summary>
        /// 在给定输入键值处沿样线获取位置
        /// </summary>
        /// <param name="InKey">键值</param>
        /// <param name="CoordinateSpace">返回世界坐标向量还是局部向量</param>
        public Vector3 GetLocationAtSplineInputKey(float InKey, ESplineCoordinateSpace CoordinateSpace)
        {
            Vector3 Location = SplineCurves.Position.Eval(InKey, Vector3.zero);

            if (CoordinateSpace == ESplineCoordinateSpace.World)
            {
                Location = transform.TransformPoint(Location);//ComponentToWorld.TransformPosition(Location);
            }

            return Location;
        }


        /// <summary>
        /// 在给定输入键值处沿样线获取切线角度
        /// </summary>
        /// <param name="InKey">键值</param>
        /// <param name="CoordinateSpace">返回世界坐标向量还是局部向量</param>
        public Quaternion GetTangentAtSplineInputKey(float InKey, ESplineCoordinateSpace CoordinateSpace)
        {
            Vector3 Tangent = SplineCurves.Position.EvalDerivative(InKey, Vector3.zero);

            if (CoordinateSpace == ESplineCoordinateSpace.World)
            {
                Tangent = transform.TransformVector(Tangent);//ComponentToWorld.TransformVector(Tangent);
            }

            return Tangent.ToQuaternion();//Vector3ToQuat(Tangent);
        }


        /// <summary>
        /// 在给定输入键值处沿样线获取旋转角度
        /// </summary>
        public Quaternion GetQuaternionAtSplineInputKey(float InKey, ESplineCoordinateSpace CoordinateSpace)
        {
            Quaternion Quat = SplineCurves.Rotation.Eval(InKey, Quaternion.identity);
            Vector3 Direction = SplineCurves.Position.EvalDerivative(InKey, Vector3.zero);//.GetSafeNormal();
            Vector3 UpVector = Quat.RotateVector(DefaultUpVector);//RotateVector(Quat, DefaultUpVector);

            Quaternion Rot = (MathfExtensions.MakeFromXZ(Direction, UpVector)).ToQuat();

            if (CoordinateSpace == ESplineCoordinateSpace.World)
            {
                Rot = transform.rotation * Rot;
            }

            return Rot;
        }

        /// <summary>
        /// 在给定输入键值处沿样线获取缩放值
        /// </summary>
        public Vector3 GetScaleAtSplineInputKey(float InKey)
        {
            Vector3 Scale = SplineCurves.Scale.Eval(InKey, Vector3.one);
            return Scale;
        }

        /// <summary>
        /// 在给定输入键值处沿样线获取 右方向向量值
        /// </summary>
        Vector3 GetRightVectorAtSplineInputKey(float InKey, ESplineCoordinateSpace CoordinateSpace)
        {
            Quaternion Quat = GetQuaternionAtSplineInputKey(InKey, ESplineCoordinateSpace.Local);
            Vector3 RightVector = Quat.RotateVector(Vector3.right);//RotateVector(Quat, Vector3.right);

            if (CoordinateSpace == ESplineCoordinateSpace.World)
            {
                RightVector = transform.TransformVector(RightVector);
            }

            return RightVector;
        }
        #endregion
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        #region 沿样线 长度 的操作
        /// <summary>
        /// 在给定的输入样线长度处沿样线取位置
        /// </summary>
        public Vector3 GetLocationAtDistanceAlongSpline(float Distance, ESplineCoordinateSpace CoordinateSpace)
        {
            float Param = SplineCurves.ReparamTable.Eval(Distance, 0.0f);
            return GetLocationAtSplineInputKey(Param, CoordinateSpace);
        }

        /// <summary>
        /// 在给定的输入样线长度处沿样线取切线旋转值
        /// </summary>
        public Quaternion GetTangentAtDistanceAlongSpline(float Distance, ESplineCoordinateSpace CoordinateSpace)
        {
            float Param = SplineCurves.ReparamTable.Eval(Distance, 0.0f);
            return GetTangentAtSplineInputKey(Param, CoordinateSpace);
        }

        /// <summary>
        /// 在给定的输入样线长度处沿样线取 缩放值
        /// </summary>
        public Vector3 GetScaleAtDistanceAlongSpline(float Distance)
        {
            float Param = SplineCurves.ReparamTable.Eval(Distance, 0.0f);
            return GetScaleAtSplineInputKey(Param);
        }
        #endregion
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        #region 样线 关键点 的操作
        /// <summary>
        /// 获取样线关键点数
        /// </summary>
        public int GetNumberOfSplinePoints()
        {
            int NumPoints = SplineCurves.Position.Points.Count;
            return NumPoints;
        }

        /// <summary>
        /// 向上取序号
        /// </summary>
        /// <param name="SplinePoints"></param>
        /// <param name="Value"></param>
        /// <returns></returns>
        int UpperBound(List<FInterpCurveVectorPoint> SplinePoints, float Value)
        {

            int Count = SplinePoints.Count;
            int First = 0;

            while (Count > 0)
            {
                int Middle = Count / 2;
                if (Value >= SplinePoints[First + Middle].InVal)
                {
                    First += Middle + 1;
                    Count -= Middle + 1;
                }
                else
                {
                    Count = Middle;
                }
            }

            return First;
        }

        /// <summary>
        /// 清除所有样线关键点
        /// </summary>
        /// <param name="bUpdateSpline"></param>
        public void ClearSplinePoints(bool bUpdateSpline = true)
        {
            //int Capacity = SplineCurves.Position.Points.Capacity ;
            SplineCurves.Position.Points.Clear();
            //SplineCurves.Position.Points.Capacity = Capacity;

            SplineCurves.Rotation.Points.Clear();
            //SplineCurves.Rotation.Points.Capacity = Capacity;

            SplineCurves.Scale.Points.Clear();
            //SplineCurves.Scale.Points.Capacity = Capacity;

            if (bUpdateSpline)
            {
                UpdateSpline();
            }
        }

        /// <summary>
        /// 添加样线关键点
        /// </summary>
        public void AddPoint(FSplinePoint InSplinePoint, bool bUpdateSpline = true)
        {
            int Index = UpperBound(SplineCurves.Position.Points, InSplinePoint.InputKey);


            SplineCurves.Position.Points.Insert(Index, new FInterpCurveVectorPoint
                (
                InSplinePoint.InputKey,
                InSplinePoint.Position,
                InSplinePoint.ArriveTangent,
                InSplinePoint.LeaveTangent,
                InSplinePoint.Type
                ));

            SplineCurves.Rotation.Points.Insert(Index, new FInterpCurveQuaternionPoint
        (
        InSplinePoint.InputKey,
        InSplinePoint.Rotation,
        Quaternion.identity,
        Quaternion.identity,
        ESplinePointType.Curve
        ));
            SplineCurves.Scale.Points.Insert(Index, new FInterpCurveVectorPoint
        (
        InSplinePoint.InputKey,
        InSplinePoint.Scale,
        Vector3.zero,
        Vector3.zero,
        ESplinePointType.Curve
        ));

            if (bLoopPositionOverride && LoopPosition <= SplineCurves.Position.Points.Last().InVal)
            {
                bLoopPositionOverride = false;
            }

            if (bUpdateSpline)
            {
                UpdateSpline();
            }
        }

        public void AddPoints(FSplinePoint[] InSplinePoints, bool bUpdateSpline)
        {
            ClearSplinePoints(false);
            foreach (var point in InSplinePoints)
            {
                AddPoint(point, false);
            }

            if (bUpdateSpline)
            {

                UpdateSpline();
            }
        }

        public void AddSplinePoint(Vector3 Position, ESplineCoordinateSpace CoordinateSpace, bool bUpdateSpline = true)
        {
            Vector3 TransformedPosition = (CoordinateSpace == ESplineCoordinateSpace.World) ? transform.InverseTransformPoint(Position) : Position;

            // Add the spline point at the end of the array, adding 1.0 to the current last input key.
            // This continues the former behavior in which spline points had to be separated by an interval of 1.0.
            float InKey = SplineCurves.Position.Points.Count > 0 ? SplineCurves.Position.Points.Last().InVal + 1.0f : 0.0f;

            SplineCurves.Position.Points.Add(new FInterpCurveVectorPoint(InKey, TransformedPosition, Vector3.zero, Vector3.zero, ESplinePointType.Curve));
            SplineCurves.Rotation.Points.Add(new FInterpCurveQuaternionPoint(InKey, Quaternion.identity, Quaternion.identity, Quaternion.identity, ESplinePointType.Curve));
            SplineCurves.Scale.Points.Add(new FInterpCurveVectorPoint(InKey, Vector3.one, Vector3.zero, Vector3.zero, ESplinePointType.Curve));

            if (bLoopPositionOverride)
            {
                LoopPosition += 1.0f;
            }

            if (bUpdateSpline)
            {
                UpdateSpline();
            }
        }

        public void AddSplinePointAtIndex(Vector3 Position, int Index, ESplineCoordinateSpace CoordinateSpace, bool bUpdateSpline = true)
        {
            Vector3 TransformedPosition = (CoordinateSpace == ESplineCoordinateSpace.World) ? transform.InverseTransformPoint(Position) : Position;

            int NumPoints = SplineCurves.Position.Points.Count;

            if (Index >= 0 && Index <= NumPoints)
            {
                float InKey = (Index == 0) ? 0.0f : SplineCurves.Position.Points[Index - 1].InVal + 1.0f;

                SplineCurves.Position.Points.Insert(Index, new FInterpCurveVectorPoint(InKey, TransformedPosition, Vector3.zero, Vector3.zero, ESplinePointType.Curve));
                SplineCurves.Rotation.Points.Insert(Index, new FInterpCurveQuaternionPoint(InKey, Quaternion.identity, Quaternion.identity, Quaternion.identity, ESplinePointType.Curve));
                SplineCurves.Scale.Points.Insert(Index, new FInterpCurveVectorPoint(InKey, Vector3.one, Vector3.zero, Vector3.zero, ESplinePointType.Curve));

                NumPoints++;

                // Adjust subsequent points' input keys to make room for the value just added
                for (int I = Index + 1; I < NumPoints; ++I)
                {
                    SplineCurves.Position.Points[I].InVal += 1.0f;
                    SplineCurves.Rotation.Points[I].InVal += 1.0f;
                    SplineCurves.Scale.Points[I].InVal += 1.0f;
                }

                if (bLoopPositionOverride)
                {
                    LoopPosition += 1.0f;
                }

            }
        }


        /// <summary>
        /// 根据样线关键点序号进行移除
        /// </summary>
        public void RemoveSplinePoint(int PointIndex, bool bUpdateSpline = true)
        {
            int NumPoints = SplineCurves.Position.Points.Count;

            if (PointIndex >= 0 && PointIndex < NumPoints)
            {
                SplineCurves.Position.Points.RemoveAt(PointIndex);
                SplineCurves.Rotation.Points.RemoveAt(PointIndex);
                SplineCurves.Scale.Points.RemoveAt(PointIndex);
                NumPoints--;

                // Adjust all following spline point input keys to close the gap left by the removed point
                while (PointIndex < NumPoints)
                {
                    SplineCurves.Position.Points[PointIndex].InVal -= 1.0f;
                    SplineCurves.Rotation.Points[PointIndex].InVal -= 1.0f;
                    SplineCurves.Scale.Points[PointIndex].InVal -= 1.0f;
                    PointIndex++;
                }

                if (bLoopPositionOverride)
                {
                    LoopPosition -= 1.0f;
                }
            }

            if (bUpdateSpline)
            {
                UpdateSpline();
            }
        }

        /// <summary>
        /// 设置关键点的坐标
        /// </summary>
        public void SetLocationAtSplinePoint(int PointIndex, Vector3 InLocation, ESplineCoordinateSpace CoordinateSpace, bool bUpdateSpline = true)
        {
            int NumPoints = SplineCurves.Position.Points.Count;

            if ((PointIndex >= 0) && (PointIndex < NumPoints))
            {
                Vector3 TransformedLocation = (CoordinateSpace == ESplineCoordinateSpace.World) ?
                    transform.InverseTransformPoint(InLocation) : InLocation;

                SplineCurves.Position.Points[PointIndex].OutVal = TransformedLocation;

                if (bUpdateSpline)
                {
                    UpdateSpline();
                }
            }
        }

        /// <summary>
        /// 设置关键点的前后切线值
        /// </summary>
        public void SetTangentsAtSplinePoint(int PointIndex, Vector3 InArriveTangent, Vector3 InLeaveTangent, ESplineCoordinateSpace CoordinateSpace, bool bUpdateSpline = true)
        {
            int NumPoints = SplineCurves.Position.Points.Count;

            if ((PointIndex >= 0) && (PointIndex < NumPoints))
            {
                Vector3 TransformedArriveTangent = (CoordinateSpace == ESplineCoordinateSpace.World) ?
                    transform.InverseTransformVector(InArriveTangent) : InArriveTangent;
                Vector3 TransformedLeaveTangent = (CoordinateSpace == ESplineCoordinateSpace.World) ?
        transform.InverseTransformVector(InLeaveTangent) : InLeaveTangent;

                SplineCurves.Position.Points[PointIndex].LeaveTangent = TransformedLeaveTangent;
                SplineCurves.Position.Points[PointIndex].ArriveTangent = TransformedArriveTangent;
                SplineCurves.Position.Points[PointIndex].InterpMode = ESplinePointType.CurveCustomTangent;

                if (bUpdateSpline)
                {
                    UpdateSpline();
                }
            }
        }

        /// <summary>
        /// 获取关键点的样线类型
        /// </summary>
        public ESplinePointType GetSplinePointType(int PointIndex)
        {
            if ((PointIndex >= 0) && (PointIndex < SplineCurves.Position.Points.Count))
            {
                return (SplineCurves.Position.Points[PointIndex].InterpMode);
            }

            return ESplinePointType.Constant;
        }

        /// <summary>
        /// 设置关键点的样线类型
        /// </summary>
        public void SetSplinePointType(int PointIndex, ESplinePointType Type, bool bUpdateSpline = true)
        {
            if ((PointIndex >= 0) && (PointIndex < SplineCurves.Position.Points.Count))
            {
                SplineCurves.Position.Points[PointIndex].InterpMode = (Type);
                if (bUpdateSpline)
                {
                    UpdateSpline();
                }
            }
        }
        /// <summary>
        /// 取样线关键点的 位置
        /// </summary>
        public Vector3 GetLocationAtSplinePoint(int PointIndex, ESplineCoordinateSpace CoordinateSpace)
        {

            int NumPoints = SplineCurves.Position.Points.Count;
            int ClampedIndex = (bClosedLoop && PointIndex >= NumPoints) ? 0 : Mathf.Clamp(PointIndex, 0, NumPoints - 1);
            Vector3 Location = SplineCurves.Position.Points[ClampedIndex].OutVal;
            return (CoordinateSpace == ESplineCoordinateSpace.World) ? transform.TransformPoint(Location) : Location;
        }

        /// <summary>
        /// 取样线关键点的 离开点切线值
        /// </summary>
        public Vector3 GetLeaveTangentAtSplinePoint(int PointIndex, ESplineCoordinateSpace CoordinateSpace)
        {
            int NumPoints = SplineCurves.Position.Points.Count;
            int ClampedIndex = (bClosedLoop && PointIndex >= NumPoints) ? 0 : Mathf.Clamp(PointIndex, 0, NumPoints - 1);
            Vector3 Direction = SplineCurves.Position.Points[ClampedIndex].LeaveTangent;
            return (CoordinateSpace == ESplineCoordinateSpace.World) ? transform.TransformVector(Direction) : Direction;
        }

        /// <summary>
        /// 取样线关键点的 进入点切线值
        /// </summary>
        public Vector3 GetArriveTangentAtSplinePoint(int PointIndex, ESplineCoordinateSpace CoordinateSpace)
        {
            int NumPoints = SplineCurves.Position.Points.Count;
            int ClampedIndex = (bClosedLoop && PointIndex >= NumPoints) ? 0 : Mathf.Clamp(PointIndex, 0, NumPoints - 1);
            Vector3 Direction = SplineCurves.Position.Points[ClampedIndex].ArriveTangent;
            return (CoordinateSpace == ESplineCoordinateSpace.World) ? transform.TransformVector(Direction) : Direction;
        }

        /// <summary>
        /// 设置样线关键点的 离开点切线值
        /// </summary>
        public void SetLeaveTangentAtSplilnePoint(int PointIndex, Vector3 LeaveTangent, ESplineCoordinateSpace CoordinateSpace, bool bUpdateSpline = true)
        {
            int NumPoints = SplineCurves.Position.Points.Count;

            if ((PointIndex >= 0) && (PointIndex < NumPoints))
            {
                Vector3 TransformedLeaveTangent = (CoordinateSpace == ESplineCoordinateSpace.World) ?
            transform.InverseTransformVector(LeaveTangent) : LeaveTangent;

                SplineCurves.Position.Points[PointIndex].LeaveTangent = TransformedLeaveTangent;
            }

            if (bUpdateSpline)
            {
                UpdateSpline();
            }
        }

        /// <summary>
        /// 设置样线关键点的 进入点切线值
        /// </summary>
        public void SetArriveTangentAtSplilnePoint(int PointIndex, Vector3 ArriveTangent, ESplineCoordinateSpace CoordinateSpace, bool bUpdateSpline = true)
        {
            int NumPoints = SplineCurves.Position.Points.Count;

            if ((PointIndex >= 0) && (PointIndex < NumPoints))
            {
                Vector3 TransformedArriveTangent = (CoordinateSpace == ESplineCoordinateSpace.World) ?
            transform.InverseTransformVector(ArriveTangent) : ArriveTangent;

                SplineCurves.Position.Points[PointIndex].ArriveTangent = TransformedArriveTangent;
            }

            if (bUpdateSpline)
            {
                UpdateSpline();
            }
        }

        /// <summary>
        /// 取样线关键点的缩放值
        /// </summary>
        public Vector3 GetScaleAtSplinePoint(int PointIndex)
        {
            int NumPoints = SplineCurves.Position.Points.Count;
            int ClampedIndex = (bClosedLoop && PointIndex >= NumPoints) ? 0 : Mathf.Clamp(PointIndex, 0, NumPoints - 1);
            Vector3 Scale = SplineCurves.Scale.Points[ClampedIndex].OutVal;
            return Scale;
        }

        /// <summary>
        /// 设置样线关键点的缩放值
        /// </summary>
        public void SetScaleAtSplinePoint(int PointIndex, Vector3 Scale)
        {
            int NumPoints = SplineCurves.Position.Points.Count;
            int ClampedIndex = (bClosedLoop && PointIndex >= NumPoints) ? 0 : Mathf.Clamp(PointIndex, 0, NumPoints - 1);

            if ((ClampedIndex >= 0) && (ClampedIndex < NumPoints))
            {
                SplineCurves.Scale.Points[ClampedIndex].OutVal = Scale;
            }

        }

        /// <summary>
        /// 设置样线关键点的旋转值
        /// </summary>
        public void SetQuaternionAtSplineInputKey(int PointIndex, Quaternion Quat, ESplineCoordinateSpace CoordinateSpace)
        {
            int NumPoints = SplineCurves.Position.Points.Count;
            int ClampedIndex = (bClosedLoop && PointIndex >= NumPoints) ? 0 : Mathf.Clamp(PointIndex, 0, NumPoints - 1);

            if ((ClampedIndex >= 0) && (ClampedIndex < NumPoints))
            {
                Quaternion Quatern = (CoordinateSpace == ESplineCoordinateSpace.World) ? transform.localRotation * Quat : Quat;

                SplineCurves.Rotation.Points[ClampedIndex].OutVal = Quatern;
            }
        }
        #endregion
        //////////////////////////////////////////////////////////////////////////////////////////////////////


        /// <summary>
        /// 获取样线距离总长度
        /// </summary>
        /// <returns></returns>
        public float GetSplineLength()
        {
            int NumPoints = SplineCurves.ReparamTable.Points.Count;

            if (NumPoints > 0)
            {
                return SplineCurves.ReparamTable.Points.Last().InVal;
            }

            return 0.0f;
        }
        /// <summary>
        /// 设置样线是否闭合
        /// </summary>
        public void SetClosedLoop(bool bInClosedLoop, bool bUpdateSpline)
        {
            bClosedLoop = bInClosedLoop;
            bLoopPositionOverride = false;
            if (bUpdateSpline)
            {
                UpdateSpline();
            }
        }

        public bool IsClosedLoop()
        {
            return bClosedLoop;
        }

        public void OnDrawGizmos()
        {
            if (!DrawDebug || SplineCurves == null || SplineCurves.Position == null || SplineCurves.Position.Points == null || SplineCurves.Position.Points.Count <= 0)
                return;

            float spheresSize = 1f;

            //Vector3 prePt = transform.position;
            Gizmos.color = Color.yellow;

            int NumPoints = SplineCurves.Position.Points.Count;
            int NumSegments = SplineCurves.Position.bIsLooped ? NumPoints : NumPoints - 1;

            Vector3 OldKeyPos = Vector3.zero;
            //Vector3 OldKeyRightVector = Vector3.zero;
            //Vector3 OldKeyScale = Vector3.zero;

            for (int KeyIdx = 0; KeyIdx < NumSegments + 1; KeyIdx++)
            {
                Vector3 NewKeyPos = GetLocationAtSplinePoint(KeyIdx, ESplineCoordinateSpace.World);//GetLocationAtSplineInputKey(SplineCurves.Position.Points[KeyIdx].InVal, ESplineCoordinateSpace.World);
                if (KeyIdx < NumPoints)
                {
                    Gizmos.color = Color.red;

                    //画关键点
                    Gizmos.DrawSphere(NewKeyPos, spheresSize);
                }

                if (KeyIdx > 0)
                {
                    Gizmos.color = Color.yellow;

                    Vector3 OldPos = OldKeyPos;
                    //Vector3 OldRightVector = OldKeyRightVector;
                    //Vector3 OldScale = OldKeyScale;

                    int NumSteps = 20;
                    for (int StepIdx = 1; StepIdx <= NumSteps; StepIdx++)
                    {
                        float Key = (KeyIdx - 1) + (StepIdx / (float)(NumSteps));
                        Vector3 NewPos = GetLocationAtSplineInputKey(Key, ESplineCoordinateSpace.World);
                        //Vector3 NewRightVector = GetRightVectorAtSplineInputKey(Key, ESplineCoordinateSpace.World);
                        //Vector3 NewScale = GetScaleAtSplineInputKey(Key);// * DefaultScale;

                        Gizmos.DrawLine(OldPos, NewPos);

                        OldPos = NewPos;
                        //OldRightVector = NewRightVector;
                        //OldScale = NewScale;
                    }
                }
                OldKeyPos = NewKeyPos;
                //OldKeyRightVector = NewKeyRightVector;
                //OldKeyScale = NewKeyScale;
            }

            //for (int i = 1; i < SplineCurves.ReparamTable.Points.Count; ++i)
            //{
            //    //画线
            //    Vector3 curPt = GetLocationAtDistanceAlongSpline(i * ReparamStepsPerSegment, ESplineCoordinateSpace.World);
            //    Gizmos.DrawLine(prePt, curPt);
            //    prePt = curPt;
            //}

            //foreach (var point in SplineCurves.Position.Points)
            //{

            //}
        }

        static void DrawGizmos(SplineComponent _spline)
        {

        }
    }
}

