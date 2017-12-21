using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace Spline
{

    #region InterpCurve

    /// <summary>
    /// 曲线关键点结构
    /// </summary>
    [Serializable]
    public class FInterpCurvePoint<T>//:IEquatable<FInterpCurvePoint<T>>
    {
        /// <summary>
        /// 曲线点的键
        /// </summary>
        [SerializeField]
        public float InVal;

        /// <summary>
        /// 曲线点的值
        /// </summary>
        [SerializeField]
        public T OutVal;

        /// <summary>
        /// 曲线点的入口切线值
        /// </summary>
        [SerializeField]
        public T ArriveTangent;

        /// <summary>
        /// 曲线点的出口切线值
        /// </summary>
        [SerializeField]
        public T LeaveTangent;

        /// <summary>
        /// 曲线点类型
        /// </summary>
        [SerializeField]
        public ESplinePointType InterpMode;

        public FInterpCurvePoint(float _fInVal, T _tOutVal, T _tArriveTangent, T _tLeaveTangent, ESplinePointType _eInterpMode)
        {
            InVal = _fInVal;
            OutVal = _tOutVal;
            ArriveTangent = _tArriveTangent;
            LeaveTangent = _tLeaveTangent;
            InterpMode = _eInterpMode;
        }

        public bool IsCurveKey()
        {
            return ((InterpMode == ESplinePointType.Curve) || (InterpMode == ESplinePointType.CurveClamped) || (InterpMode == ESplinePointType.CurveCustomTangent) || (InterpMode == ESplinePointType.Constant));
        }
    }
    //C# 不支持直接显示的泛型类序列化
    //所以把子类都统一弄个间接显示的泛型类
    [Serializable]
    public class FInterpCurveVectorPoint : FInterpCurvePoint<Vector3>
    {
        public FInterpCurveVectorPoint(float _fInVal, Vector3 _tOutVal, Vector3 _tArriveTangent, Vector3 _tLeaveTangent, ESplinePointType _eInterpMode)
            : base(_fInVal, _tOutVal, _tArriveTangent, _tLeaveTangent, _eInterpMode)
        {
        }

    }
    [Serializable]
    public class FInterpCurveQuaternionPoint : FInterpCurvePoint<Quaternion>
    {
        public FInterpCurveQuaternionPoint(float _fInVal, Quaternion _tOutVal, Quaternion _tArriveTangent, Quaternion _tLeaveTangent, ESplinePointType _eInterpMode)
            : base(_fInVal, _tOutVal, _tArriveTangent, _tLeaveTangent, _eInterpMode)
        {
        }

    }
    [Serializable]
    public class FInterpCurveFloatPoint : FInterpCurvePoint<float>
    {
        public FInterpCurveFloatPoint(float _fInVal, float _tOutVal, float _tArriveTangent, float _tLeaveTangent, ESplinePointType _eInterpMode)
            : base(_fInVal, _tOutVal, _tArriveTangent, _tLeaveTangent, _eInterpMode)
        {
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    //C# 不支持直接显示的泛型类序列化
    //所以把子类都统一弄个间接显示的泛型类

    /// <summary>
    /// 曲线数据
    /// </summary>
    [Serializable]
    public class FInterpCurve<T>//where T : UnityEngine.Object
    {
        /** 曲线是否循环 */
        public bool bIsLooped;

        /** 曲线循环点相对于结束点的偏移值(曲线结束点不一定是起点) */
        public float LoopKeyOffset;

        /// <summary>
        /// 二分法根据曲线点的键查找在Points的向下边界序号
        /// </summary>
        /// <param name="InValue">曲线点的键</param>
        /// <returns>在Points的下边界序号</returns>
        protected virtual int GetPointIndexForInputValue(float InValue)
        {
            throw new NotImplementedException();
        }

        public virtual T Eval(float InVal, T Default)
        {
            throw new NotImplementedException();
        }

        public virtual T EvalDerivative(float InVal, T Default)
        {
            throw new NotImplementedException();
        }

        public virtual void SetLoopKey(float InLoopKey)
        {
            throw new NotImplementedException();
        }

        public void ClearLoopKey()
        {
            bIsLooped = false;
        }

        protected void ComputeCurveTangent(float PrevTime, T PrevPoint,
                                float CurTime, T CurPoint,
                                float NextTime, T NextPoint,
                                float Tension,

                                bool bWantClamping,
                                ref T OutTangent)
        {
            // NOTE: Clamping not supported for non-float vector types (bWantClamping is ignored)

            AutoCalcTangent(PrevPoint, CurPoint, NextPoint, Tension, ref OutTangent);

            float PrevToNextTimeDiff = Mathf.Max(float.MinValue, NextTime - PrevTime);//(KINDA_SMALL_NUMBER, NextTime - PrevTime);

            OutTangent = Divide(OutTangent, PrevToNextTimeDiff);
        }

        public virtual void AutoSetTangents(float Tension, bool bStationaryEndpoints)
        {
            throw new NotImplementedException();
        }

        public virtual void Reset()
        {
            throw new NotImplementedException();
        }

        protected virtual T ForceInit()
        {
            throw new NotImplementedException();
        }
        protected virtual T CubicInterpDerivative(T P0, T T0, T P1, T T1, float A, float _fDiff = 1f)
        {
            //这步应该在外面处理，由于T类型不确定所以放里面处理
            //T0*=_fDiff;
            //T1*=_fDiff;

            //T a = 6.f * P0 + 3.f * T0 + 3.f * T1 - 6.f * P1;
            //T b = -6.f * P0 - 4.f * T0 - 2.f * T1 + 6.f * P1;
            //T c = T0;

            //const float A2 = A * A;
            //T result = (a * A2) + (b * A) + c;
            //这步应该在外面处理，由于T类型不确定所以放里面处理
            // result /= _fDiff;
            //return result;
            throw new NotImplementedException();
        }
        protected virtual T Divide(T _tValue1, float _fScale)
        {
            throw new NotImplementedException();
        }
        protected virtual T Minus(T _tValue1, T _tValue2)
        {
            throw new NotImplementedException();
        }

        protected virtual T Lerp(T _tValue1, T _tValue2, float _fAlpha)
        {
            throw new NotImplementedException();
        }

        protected virtual T CubicInterp(T P0, T T0, T P1, T T1, float A, float _fDiff = 1f)
        {
            //这步应该在外面处理，由于T类型不确定所以放里面处理
            // T0 = T0*_fDiff;
            //T1 = T1 * _fDiff;
            //  const float A2 = A * A;
            //  const float A3 = A2 * A;
            //return (T)(((2*A3)-(3*A2)+1) * P0) + ((A3-(2*A2)+A) * T0) + ((A3-A2) * T1) + (((-2*A3)+(3*A2)) * P1);

            throw new NotImplementedException();
        }

        protected virtual void AutoCalcTangent(T PrevP, T P, T NextP, float Tension, ref T OutTan)
        {
            //OutTan = (1.f - Tension) * ( (P - PrevP) + (NextP - P) );
            throw new NotImplementedException();
        }
    }
    [Serializable]
    public class FInterpCurveVector : FInterpCurve<Vector3>
    {
        /** 所有曲线点 */
        public List<FInterpCurveVectorPoint> Points = new List<FInterpCurveVectorPoint>();

        /// <summary>
        /// 二分法根据曲线点的键查找在Points的向下边界序号
        /// </summary>
        /// <param name="InValue">曲线点的键</param>
        /// <returns>在Points的下边界序号</returns>
        protected override int GetPointIndexForInputValue(float InValue)
        {
            int NumPoints = Points.Count;
            int LastPoint = NumPoints - 1;

            if (NumPoints <= 0)
                return -1;

            if (InValue < Points[0].InVal)
            {
                return -1;
            }

            if (InValue >= Points[LastPoint].InVal)
            {
                return LastPoint;
            }

            int MinIndex = 0;
            int MaxIndex = NumPoints;

            while (MaxIndex - MinIndex > 1)
            {
                int MidIndex = (MinIndex + MaxIndex) / 2;

                if (Points[MidIndex].InVal <= InValue)
                {
                    MinIndex = MidIndex;
                }
                else
                {
                    MaxIndex = MidIndex;
                }
            }

            return MinIndex;
        }

        public override Vector3 Eval(float InVal, Vector3 Default)
        {
            int NumPoints = Points.Count;
            int LastPoint = NumPoints - 1;

            // Curve 没有顶点的话返回默认值
            if (NumPoints == 0)
            {
                return Default;
            }

            int Index = GetPointIndexForInputValue(InVal);

            if (Index == -1)
            {
                return Points[0].OutVal;
            }

            // 如果是Points最尾的序号，则根据是否循环曲线来取
            if (Index == LastPoint)
            {
                if (!bIsLooped)
                {
                    return Points[LastPoint].OutVal;
                }
                else if (InVal >= Points[LastPoint].InVal + LoopKeyOffset)
                {
                    return Points[0].OutVal;
                }
            }

            // 曲线范围内插值计算
            //check(Index >= 0 && ((bIsLooped && Index < NumPoints) || (!bIsLooped && Index < LastPoint)))

            bool bLoopSegment = (bIsLooped && Index == LastPoint);
            int NextIndex = bLoopSegment ? 0 : (Index + 1);

            var PrevPoint = Points[Index];
            var NextPoint = Points[NextIndex];

            float Diff = bLoopSegment ? LoopKeyOffset : (NextPoint.InVal - PrevPoint.InVal);

            if (Diff > 0.0f && PrevPoint.InterpMode != ESplinePointType.Constant)
            {
                float Alpha = (InVal - PrevPoint.InVal) / Diff;
                //check(Alpha >= 0.0f && Alpha <= 1.0f);

                if (PrevPoint.InterpMode == ESplinePointType.Linear)
                {
                    return Lerp(PrevPoint.OutVal, NextPoint.OutVal, Alpha);
                }
                else
                {
                    return CubicInterp(PrevPoint.OutVal, PrevPoint.LeaveTangent, NextPoint.OutVal, NextPoint.ArriveTangent, Alpha, Diff);
                }
            }
            else
            {
                return Points[Index].OutVal;
            }
        }

        public override Vector3 EvalDerivative(float InVal, Vector3 Default)
        {
            int NumPoints = Points.Count;
            int LastPoint = NumPoints - 1;

            // Curve 没有顶点的话返回默认值
            if (NumPoints == 0)
            {
                return Default;
            }

            int Index = GetPointIndexForInputValue(InVal);

            if (Index == -1)
            {
                return Points[0].LeaveTangent;
            }

            // 如果是Points最尾的序号，则根据是否循环曲线来取
            if (Index == LastPoint)
            {
                if (!bIsLooped)
                {
                    return Points[LastPoint].ArriveTangent;
                }
                else if (InVal >= Points[LastPoint].InVal + LoopKeyOffset)
                {
                    return Points[0].ArriveTangent;
                }
            }

            // 曲线范围内插值计算
            // Somewhere within curve range - interpolate.
            //check(Index >= 0 && ((bIsLooped && Index < NumPoints) || (!bIsLooped && Index < LastPoint)));
            bool bLoopSegment = (bIsLooped && Index == LastPoint);
            int NextIndex = bLoopSegment ? 0 : (Index + 1);

            var PrevPoint = Points[Index];
            var NextPoint = Points[NextIndex];

            float Diff = bLoopSegment ? LoopKeyOffset : (NextPoint.InVal - PrevPoint.InVal);

            if (Diff > 0.0f && PrevPoint.InterpMode != ESplinePointType.Constant)
            {
                if (PrevPoint.InterpMode == ESplinePointType.Linear)
                {
                    return Divide(Minus(NextPoint.OutVal, PrevPoint.OutVal), Diff);
                }
                else
                {
                    float Alpha = (InVal - PrevPoint.InVal) / Diff;
                    //check(Alpha >= 0.0f && Alpha <= 1.0f);
                    return CubicInterpDerivative(PrevPoint.OutVal, PrevPoint.LeaveTangent, NextPoint.OutVal, NextPoint.ArriveTangent, Alpha, Diff);
                    //return FMath::CubicInterpDerivative(PrevPoint.OutVal, PrevPoint.LeaveTangent * Diff, NextPoint.OutVal, NextPoint.ArriveTangent * Diff, Alpha) / Diff;
                }
            }
            else
            {
                // Derivative of a constant is zero
                return ForceInit();//T(ForceInit);
            }
        }

        public override void SetLoopKey(float InLoopKey)
        {
            if (Points.Count == 0)
            {
                bIsLooped = false;
                return;
            }

            float LastInKey = Points.Last().InVal;
            if (InLoopKey > LastInKey)
            {
                // 计算循环偏移量
                bIsLooped = true;
                LoopKeyOffset = InLoopKey - LastInKey;
            }
            else
            {
                // 循环值少于最大值的话，不循环
                bIsLooped = false;
            }
        }

        public override void AutoSetTangents(float Tension, bool bStationaryEndpoints)
        {
            int NumPoints = Points.Count;
            int LastPoint = NumPoints - 1;

            // Iterate over all points in this InterpCurve
            for (int PointIndex = 0; PointIndex < NumPoints; PointIndex++)
            {
                int PrevIndex = (PointIndex == 0) ? (bIsLooped ? LastPoint : 0) : (PointIndex - 1);
                int NextIndex = (PointIndex == LastPoint) ? (bIsLooped ? 0 : LastPoint) : (PointIndex + 1);

                var ThisPoint = Points[PointIndex];
                var PrevPoint = Points[PrevIndex];
                var NextPoint = Points[NextIndex];

                if (ThisPoint.InterpMode == ESplinePointType.Curve || ThisPoint.InterpMode == ESplinePointType.CurveClamped)
                {
                    if (bStationaryEndpoints && (PointIndex == 0 || (PointIndex == LastPoint && !bIsLooped)))
                    {
                        // Start and end points get zero tangents if bStationaryEndpoints is true
                        ThisPoint.ArriveTangent = ForceInit();
                        ThisPoint.LeaveTangent = ForceInit();
                    }
                    else if (PrevPoint.IsCurveKey())
                    {
                        bool bWantClamping = (ThisPoint.InterpMode == ESplinePointType.CurveClamped);
                        Vector3 Tangent = ForceInit();

                        float PrevTime = (bIsLooped && PointIndex == 0) ? (ThisPoint.InVal - LoopKeyOffset) : PrevPoint.InVal;
                        float NextTime = (bIsLooped && PointIndex == LastPoint) ? (ThisPoint.InVal + LoopKeyOffset) : NextPoint.InVal;

                        ComputeCurveTangent(
                            PrevTime,           // Previous time
                            PrevPoint.OutVal,   // Previous point
                            ThisPoint.InVal,    // Current time
                            ThisPoint.OutVal,   // Current point
                            NextTime,           // Next time
                            NextPoint.OutVal,   // Next point
                            Tension,            // Tension
                            bWantClamping,      // Want clamping?
                            ref Tangent);           // Out

                        ThisPoint.ArriveTangent = Tangent;
                        ThisPoint.LeaveTangent = Tangent;
                    }
                    else
                    {
                        // Following on from a line or constant; set curve tangent equal to that so there are no discontinuities
                        ThisPoint.ArriveTangent = PrevPoint.ArriveTangent;
                        ThisPoint.LeaveTangent = PrevPoint.LeaveTangent;
                    }
                }
                else if (ThisPoint.InterpMode == ESplinePointType.Linear)
                {
                    Vector3 Tangent = Minus(NextPoint.OutVal, ThisPoint.OutVal);
                    ThisPoint.ArriveTangent = Tangent;
                    ThisPoint.LeaveTangent = Tangent;
                }
                else if (ThisPoint.InterpMode == ESplinePointType.Constant)
                {
                    ThisPoint.ArriveTangent = ForceInit();
                    ThisPoint.LeaveTangent = ForceInit();
                }
            }
        }

        public override void Reset()
        {
            Points.Clear();
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////

        protected override Vector3 Lerp(Vector3 _tValue1, Vector3 _tValue2, float _fAlpha)
        {
            return Vector3.Lerp(_tValue1, _tValue2, _fAlpha);
        }

        protected override Vector3 CubicInterp(Vector3 P0, Vector3 T0, Vector3 P1, Vector3 T1, float A, float _fDiff = 1)
        {
            //这步应该在外面处理，由于T类型不确定所以放里面处理
            T0 = T0 * _fDiff;
            T1 = T1 * _fDiff;
            float A2 = A * A;
            float A3 = A2 * A;
            return (Vector3)(((2 * A3) - (3 * A2) + 1) * P0) + ((A3 - (2 * A2) + A) * T0) + ((A3 - A2) * T1) + (((-2 * A3) + (3 * A2)) * P1);
        }

        protected override Vector3 Minus(Vector3 _tValue1, Vector3 _tValue2)
        {
            return new Vector3(_tValue1.x - _tValue2.x,
                                _tValue1.y - _tValue2.y,
                                _tValue1.z - _tValue2.z);
        }

        protected override void AutoCalcTangent(Vector3 PrevP, Vector3 P, Vector3 NextP, float Tension, ref Vector3 OutTan)
        {
            //OutTan = (1.f - Tension) * ( (P - PrevP) + (NextP - P) );
            OutTan = (1f - Tension) * ((P - PrevP) + (NextP - P));
        }

        protected override Vector3 ForceInit()
        {
            return Vector3.zero;
        }

        protected override Vector3 Divide(Vector3 _tValue1, float _fScale)
        {
            return new Vector3(_tValue1.x / _fScale,
                                _tValue1.y / _fScale,
                                _tValue1.z / _fScale);
        }

        protected override Vector3 CubicInterpDerivative(Vector3 P0, Vector3 T0, Vector3 P1, Vector3 T1, float A, float _fDiff = 1)
        {//具体公式看父类

            //这步应该在外面处理，由于父类是T类型不确定所以放里面处理
            T0 *= _fDiff;
            T1 *= _fDiff;
            Vector3 result = Vector3.zero;

            Vector3 a = 6f * P0 + 3f * T0 + 3f * T1 - 6f * P1;
            Vector3 b = -6f * P0 - 4f * T0 - 2f * T1 + 6f * P1;
            Vector3 c = T0;

            float A2 = A * A;
            result = (a * A2) + (b * A) + c;

            //这步应该在外面处理，由于父类是T类型不确定所以放里面处理
            result /= _fDiff;
            return result;
        }
    }
    [Serializable]
    public class FInterpCurveQuat : FInterpCurve<Quaternion>
    {
        /** 所有曲线点 */
        public List<FInterpCurveQuaternionPoint> Points = new List<FInterpCurveQuaternionPoint>();
        /// <summary>
        /// 二分法根据曲线点的键查找在Points的向下边界序号
        /// </summary>
        /// <param name="InValue">曲线点的键</param>
        /// <returns>在Points的下边界序号</returns>
        protected override int GetPointIndexForInputValue(float InValue)
        {
            int NumPoints = Points.Count;
            int LastPoint = NumPoints - 1;

            if (NumPoints <= 0)
                return -1;

            if (InValue < Points[0].InVal)
            {
                return -1;
            }

            if (InValue >= Points[LastPoint].InVal)
            {
                return LastPoint;
            }

            int MinIndex = 0;
            int MaxIndex = NumPoints;

            while (MaxIndex - MinIndex > 1)
            {
                int MidIndex = (MinIndex + MaxIndex) / 2;

                if (Points[MidIndex].InVal <= InValue)
                {
                    MinIndex = MidIndex;
                }
                else
                {
                    MaxIndex = MidIndex;
                }
            }

            return MinIndex;
        }

        public override Quaternion Eval(float InVal, Quaternion Default)
        {
            int NumPoints = Points.Count;
            int LastPoint = NumPoints - 1;

            // Curve 没有顶点的话返回默认值
            if (NumPoints == 0)
            {
                return Default;
            }

            int Index = GetPointIndexForInputValue(InVal);

            if (Index == -1)
            {
                return Points[0].OutVal;
            }

            // 如果是Points最尾的序号，则根据是否循环曲线来取
            if (Index == LastPoint)
            {
                if (!bIsLooped)
                {
                    return Points[LastPoint].OutVal;
                }
                else if (InVal >= Points[LastPoint].InVal + LoopKeyOffset)
                {
                    return Points[0].OutVal;
                }
            }

            // 曲线范围内插值计算
            //check(Index >= 0 && ((bIsLooped && Index < NumPoints) || (!bIsLooped && Index < LastPoint)))

            bool bLoopSegment = (bIsLooped && Index == LastPoint);
            int NextIndex = bLoopSegment ? 0 : (Index + 1);

            var PrevPoint = Points[Index];
            var NextPoint = Points[NextIndex];

            float Diff = bLoopSegment ? LoopKeyOffset : (NextPoint.InVal - PrevPoint.InVal);

            if (Diff > 0.0f && PrevPoint.InterpMode != ESplinePointType.Constant)
            {
                float Alpha = (InVal - PrevPoint.InVal) / Diff;
                //check(Alpha >= 0.0f && Alpha <= 1.0f);

                if (PrevPoint.InterpMode == ESplinePointType.Linear)
                {
                    return Lerp(PrevPoint.OutVal, NextPoint.OutVal, Alpha);
                }
                else
                {
                    return CubicInterp(PrevPoint.OutVal, PrevPoint.LeaveTangent, NextPoint.OutVal, NextPoint.ArriveTangent, Alpha, Diff);
                }
            }
            else
            {
                return Points[Index].OutVal;
            }
        }

        public override Quaternion EvalDerivative(float InVal, Quaternion Default)
        {
            int NumPoints = Points.Count;
            int LastPoint = NumPoints - 1;

            // Curve 没有顶点的话返回默认值
            if (NumPoints == 0)
            {
                return Default;
            }

            int Index = GetPointIndexForInputValue(InVal);

            if (Index == -1)
            {
                return Points[0].LeaveTangent;
            }

            // 如果是Points最尾的序号，则根据是否循环曲线来取
            if (Index == LastPoint)
            {
                if (!bIsLooped)
                {
                    return Points[LastPoint].ArriveTangent;
                }
                else if (InVal >= Points[LastPoint].InVal + LoopKeyOffset)
                {
                    return Points[0].ArriveTangent;
                }
            }

            // 曲线范围内插值计算
            // Somewhere within curve range - interpolate.
            //check(Index >= 0 && ((bIsLooped && Index < NumPoints) || (!bIsLooped && Index < LastPoint)));
            bool bLoopSegment = (bIsLooped && Index == LastPoint);
            int NextIndex = bLoopSegment ? 0 : (Index + 1);

            var PrevPoint = Points[Index];
            var NextPoint = Points[NextIndex];

            float Diff = bLoopSegment ? LoopKeyOffset : (NextPoint.InVal - PrevPoint.InVal);

            if (Diff > 0.0f && PrevPoint.InterpMode != ESplinePointType.Constant)
            {
                if (PrevPoint.InterpMode == ESplinePointType.Linear)
                {
                    return Divide(Minus(NextPoint.OutVal, PrevPoint.OutVal), Diff);
                }
                else
                {
                    float Alpha = (InVal - PrevPoint.InVal) / Diff;
                    //check(Alpha >= 0.0f && Alpha <= 1.0f);
                    return CubicInterpDerivative(PrevPoint.OutVal, PrevPoint.LeaveTangent, NextPoint.OutVal, NextPoint.ArriveTangent, Alpha, Diff);
                    //return FMath::CubicInterpDerivative(PrevPoint.OutVal, PrevPoint.LeaveTangent * Diff, NextPoint.OutVal, NextPoint.ArriveTangent * Diff, Alpha) / Diff;
                }
            }
            else
            {
                // Derivative of a constant is zero
                return ForceInit();//T(ForceInit);
            }
        }

        public override void SetLoopKey(float InLoopKey)
        {
            if (Points.Count == 0)
            {
                bIsLooped = false;
                return;
            }

            float LastInKey = Points.Last().InVal;
            if (InLoopKey > LastInKey)
            {
                // 计算循环偏移量
                bIsLooped = true;
                LoopKeyOffset = InLoopKey - LastInKey;
            }
            else
            {
                // 循环值少于最大值的话，不循环
                bIsLooped = false;
            }
        }

        public override void AutoSetTangents(float Tension, bool bStationaryEndpoints)
        {
            int NumPoints = Points.Count;
            int LastPoint = NumPoints - 1;

            // Iterate over all points in this InterpCurve
            for (int PointIndex = 0; PointIndex < NumPoints; PointIndex++)
            {
                int PrevIndex = (PointIndex == 0) ? (bIsLooped ? LastPoint : 0) : (PointIndex - 1);
                int NextIndex = (PointIndex == LastPoint) ? (bIsLooped ? 0 : LastPoint) : (PointIndex + 1);

                var ThisPoint = Points[PointIndex];
                var PrevPoint = Points[PrevIndex];
                var NextPoint = Points[NextIndex];

                if (ThisPoint.InterpMode == ESplinePointType.Curve || ThisPoint.InterpMode == ESplinePointType.CurveClamped)
                {
                    if (bStationaryEndpoints && (PointIndex == 0 || (PointIndex == LastPoint && !bIsLooped)))
                    {
                        // Start and end points get zero tangents if bStationaryEndpoints is true
                        ThisPoint.ArriveTangent = ForceInit();
                        ThisPoint.LeaveTangent = ForceInit();
                    }
                    else if (PrevPoint.IsCurveKey())
                    {
                        bool bWantClamping = (ThisPoint.InterpMode == ESplinePointType.CurveClamped);
                        Quaternion Tangent = ForceInit();

                        float PrevTime = (bIsLooped && PointIndex == 0) ? (ThisPoint.InVal - LoopKeyOffset) : PrevPoint.InVal;
                        float NextTime = (bIsLooped && PointIndex == LastPoint) ? (ThisPoint.InVal + LoopKeyOffset) : NextPoint.InVal;

                        ComputeCurveTangent(
                            PrevTime,           // Previous time
                            PrevPoint.OutVal,   // Previous point
                            ThisPoint.InVal,    // Current time
                            ThisPoint.OutVal,   // Current point
                            NextTime,           // Next time
                            NextPoint.OutVal,   // Next point
                            Tension,            // Tension
                            bWantClamping,      // Want clamping?
                            ref Tangent);           // Out

                        ThisPoint.ArriveTangent = Tangent;
                        ThisPoint.LeaveTangent = Tangent;
                    }
                    else
                    {
                        // Following on from a line or constant; set curve tangent equal to that so there are no discontinuities
                        ThisPoint.ArriveTangent = PrevPoint.ArriveTangent;
                        ThisPoint.LeaveTangent = PrevPoint.LeaveTangent;
                    }
                }
                else if (ThisPoint.InterpMode == ESplinePointType.Linear)
                {
                    Quaternion Tangent = Minus(NextPoint.OutVal, ThisPoint.OutVal);
                    ThisPoint.ArriveTangent = Tangent;
                    ThisPoint.LeaveTangent = Tangent;
                }
                else if (ThisPoint.InterpMode == ESplinePointType.Constant)
                {
                    ThisPoint.ArriveTangent = ForceInit();
                    ThisPoint.LeaveTangent = ForceInit();
                }
            }
        }

        public override void Reset()
        {
            Points.Clear();
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        protected override Quaternion Lerp(Quaternion _tValue1, Quaternion _tValue2, float _fAlpha)
        {
            return Quaternion.Lerp(_tValue1, _tValue2, _fAlpha);
        }

        protected override Quaternion CubicInterp(Quaternion P0, Quaternion T0, Quaternion P1, Quaternion T1, float A, float _fDiff = 1)
        {
            //这步应该在外面处理，由于T类型不确定所以放里面处理
            T0 = mul(T0, _fDiff);
            T1 = mul(T1, _fDiff);
            float A2 = A * A;
            float A3 = A2 * A;
            Quaternion m0 = mul(P0, ((2 * A3) - (3 * A2) + 1));
            Quaternion m1 = mul(T0, ((A3 - (2 * A2) + A)));
            Quaternion m2 = mul(T1, ((A3 - A2)));
            Quaternion m3 = mul(P1, (((-2 * A3) + (3 * A2))));
            return add(add(add(m0, m1), m2), m3);
        }

        protected Quaternion mul(Quaternion _Q1, float _fScale)
        {
            return new Quaternion(_Q1.x * _fScale, _Q1.y * _fScale, _Q1.z * _fScale, _Q1.w * _fScale);
        }

        protected Quaternion add(Quaternion _Q1, Quaternion _Q2)
        {
            return new Quaternion(_Q1.x + _Q2.x, _Q1.y + _Q2.y, _Q1.z + _Q2.z, _Q1.w + _Q2.w);
        }

        protected override Quaternion Minus(Quaternion _tValue1, Quaternion _tValue2)
        {
            return new Quaternion(_tValue1.x - _tValue2.x,
                                    _tValue1.y - _tValue2.y,
                                    _tValue1.z - _tValue2.z,
                                    _tValue1.w - _tValue2.w);
        }

        protected override void AutoCalcTangent(Quaternion PrevP, Quaternion P, Quaternion NextP, float Tension, ref Quaternion OutTan)
        {
            //OutTan = (1.f - Tension) * ( (P - PrevP) + (NextP - P) );
            OutTan = mul(add(Minus(P, PrevP), Minus(NextP, P)), (1f - Tension));
        }

        protected override Quaternion CubicInterpDerivative(Quaternion P0, Quaternion T0, Quaternion P1, Quaternion T1, float A, float _fDiff = 1)
        {
            //这步应该在外面处理，由于父类是T类型不确定所以放里面处理
            T0 = mul(T0, _fDiff);
            T1 = mul(T1, _fDiff);

            Quaternion a = add((add(add(mul(P0, 6f), mul(T0, 3f)), mul(T1, 3f))), mul(P1, -6f));

            Quaternion b = add((add(add(mul(P0, -6f), mul(T0, -4f)), mul(T1, -2f))), mul(P1, 6f));
            Quaternion c = T0;

            float A2 = A * A;

            Quaternion result = add(add(mul(a, A2), mul(b, A)), c);

            //这步应该在外面处理，由于父类是T类型不确定所以放里面处理
            result = Divide(result, _fDiff);

            return result;
        }

        protected override Quaternion Divide(Quaternion _tValue1, float _fScale)
        {
            float Recip = 1f / _fScale;
            return new Quaternion(_tValue1.x * Recip, _tValue1.y * Recip, _tValue1.z * Recip, _tValue1.w * Recip);
        }

        protected override Quaternion ForceInit()
        {
            return Quaternion.identity;
        }
    }
    [Serializable]
    public class FInterpCurveFloat : FInterpCurve<float>
    {
        /** 所有曲线点 */
        [SerializeField]
        public List<FInterpCurveFloatPoint> Points = new List<FInterpCurveFloatPoint>();



        /// <summary>
        /// 二分法根据曲线点的键查找在Points的向下边界序号
        /// </summary>
        /// <param name="InValue">曲线点的键</param>
        /// <returns>在Points的下边界序号</returns>
        protected override int GetPointIndexForInputValue(float InValue)
        {
            int NumPoints = Points.Count;
            int LastPoint = NumPoints - 1;

            if (NumPoints <= 0)
                return -1;

            if (InValue < Points[0].InVal)
            {
                return -1;
            }

            if (InValue >= Points[LastPoint].InVal)
            {
                return LastPoint;
            }

            int MinIndex = 0;
            int MaxIndex = NumPoints;

            while (MaxIndex - MinIndex > 1)
            {
                int MidIndex = (MinIndex + MaxIndex) / 2;

                if (Points[MidIndex].InVal <= InValue)
                {
                    MinIndex = MidIndex;
                }
                else
                {
                    MaxIndex = MidIndex;
                }
            }

            return MinIndex;
        }

        public override float Eval(float InVal, float Default)
        {
            int NumPoints = Points.Count;
            int LastPoint = NumPoints - 1;

            // Curve 没有顶点的话返回默认值
            if (NumPoints == 0)
            {
                return Default;
            }

            int Index = GetPointIndexForInputValue(InVal);

            if (Index == -1)
            {
                return Points[0].OutVal;
            }

            // 如果是Points最尾的序号，则根据是否循环曲线来取
            if (Index == LastPoint)
            {
                if (!bIsLooped)
                {
                    return Points[LastPoint].OutVal;
                }
                else if (InVal >= Points[LastPoint].InVal + LoopKeyOffset)
                {
                    return Points[0].OutVal;
                }
            }

            // 曲线范围内插值计算
            //check(Index >= 0 && ((bIsLooped && Index < NumPoints) || (!bIsLooped && Index < LastPoint)))

            bool bLoopSegment = (bIsLooped && Index == LastPoint);
            int NextIndex = bLoopSegment ? 0 : (Index + 1);

            var PrevPoint = Points[Index];
            var NextPoint = Points[NextIndex];

            float Diff = bLoopSegment ? LoopKeyOffset : (NextPoint.InVal - PrevPoint.InVal);

            if (Diff > 0.0f && PrevPoint.InterpMode != ESplinePointType.Constant)
            {
                float Alpha = (InVal - PrevPoint.InVal) / Diff;
                //check(Alpha >= 0.0f && Alpha <= 1.0f);

                if (PrevPoint.InterpMode == ESplinePointType.Linear)
                {
                    return Lerp(PrevPoint.OutVal, NextPoint.OutVal, Alpha);
                }
                else
                {
                    return CubicInterp(PrevPoint.OutVal, PrevPoint.LeaveTangent, NextPoint.OutVal, NextPoint.ArriveTangent, Alpha, Diff);
                }
            }
            else
            {
                return Points[Index].OutVal;
            }
        }

        public override float EvalDerivative(float InVal, float Default)
        {
            int NumPoints = Points.Count;
            int LastPoint = NumPoints - 1;

            // Curve 没有顶点的话返回默认值
            if (NumPoints == 0)
            {
                return Default;
            }

            int Index = GetPointIndexForInputValue(InVal);

            if (Index == -1)
            {
                return Points[0].LeaveTangent;
            }

            // 如果是Points最尾的序号，则根据是否循环曲线来取
            if (Index == LastPoint)
            {
                if (!bIsLooped)
                {
                    return Points[LastPoint].ArriveTangent;
                }
                else if (InVal >= Points[LastPoint].InVal + LoopKeyOffset)
                {
                    return Points[0].ArriveTangent;
                }
            }

            // 曲线范围内插值计算
            // Somewhere within curve range - interpolate.
            //check(Index >= 0 && ((bIsLooped && Index < NumPoints) || (!bIsLooped && Index < LastPoint)));
            bool bLoopSegment = (bIsLooped && Index == LastPoint);
            int NextIndex = bLoopSegment ? 0 : (Index + 1);

            var PrevPoint = Points[Index];
            var NextPoint = Points[NextIndex];

            float Diff = bLoopSegment ? LoopKeyOffset : (NextPoint.InVal - PrevPoint.InVal);

            if (Diff > 0.0f && PrevPoint.InterpMode != ESplinePointType.Constant)
            {
                if (PrevPoint.InterpMode == ESplinePointType.Linear)
                {
                    return Divide(Minus(NextPoint.OutVal, PrevPoint.OutVal), Diff);
                }
                else
                {
                    float Alpha = (InVal - PrevPoint.InVal) / Diff;
                    //check(Alpha >= 0.0f && Alpha <= 1.0f);
                    return CubicInterpDerivative(PrevPoint.OutVal, PrevPoint.LeaveTangent, NextPoint.OutVal, NextPoint.ArriveTangent, Alpha, Diff);
                    //return FMath::CubicInterpDerivative(PrevPoint.OutVal, PrevPoint.LeaveTangent * Diff, NextPoint.OutVal, NextPoint.ArriveTangent * Diff, Alpha) / Diff;
                }
            }
            else
            {
                // Derivative of a constant is zero
                return ForceInit();//T(ForceInit);
            }
        }

        public override void SetLoopKey(float InLoopKey)
        {
            if (Points.Count == 0)
            {
                bIsLooped = false;
                return;
            }

            float LastInKey = Points.Last().InVal;
            if (InLoopKey > LastInKey)
            {
                // 计算循环偏移量
                bIsLooped = true;
                LoopKeyOffset = InLoopKey - LastInKey;
            }
            else
            {
                // 循环值少于最大值的话，不循环
                bIsLooped = false;
            }
        }

        public override void AutoSetTangents(float Tension, bool bStationaryEndpoints)
        {
            int NumPoints = Points.Count;
            int LastPoint = NumPoints - 1;

            // Iterate over all points in this InterpCurve
            for (int PointIndex = 0; PointIndex < NumPoints; PointIndex++)
            {
                int PrevIndex = (PointIndex == 0) ? (bIsLooped ? LastPoint : 0) : (PointIndex - 1);
                int NextIndex = (PointIndex == LastPoint) ? (bIsLooped ? 0 : LastPoint) : (PointIndex + 1);

                var ThisPoint = Points[PointIndex];
                var PrevPoint = Points[PrevIndex];
                var NextPoint = Points[NextIndex];

                if (ThisPoint.InterpMode == ESplinePointType.Curve || ThisPoint.InterpMode == ESplinePointType.CurveClamped)
                {
                    if (bStationaryEndpoints && (PointIndex == 0 || (PointIndex == LastPoint && !bIsLooped)))
                    {
                        // Start and end points get zero tangents if bStationaryEndpoints is true
                        ThisPoint.ArriveTangent = ForceInit();
                        ThisPoint.LeaveTangent = ForceInit();
                    }
                    else if (PrevPoint.IsCurveKey())
                    {
                        bool bWantClamping = (ThisPoint.InterpMode == ESplinePointType.CurveClamped);
                        float Tangent = ForceInit();

                        float PrevTime = (bIsLooped && PointIndex == 0) ? (ThisPoint.InVal - LoopKeyOffset) : PrevPoint.InVal;
                        float NextTime = (bIsLooped && PointIndex == LastPoint) ? (ThisPoint.InVal + LoopKeyOffset) : NextPoint.InVal;

                        ComputeCurveTangent(
                            PrevTime,           // Previous time
                            PrevPoint.OutVal,   // Previous point
                            ThisPoint.InVal,    // Current time
                            ThisPoint.OutVal,   // Current point
                            NextTime,           // Next time
                            NextPoint.OutVal,   // Next point
                            Tension,            // Tension
                            bWantClamping,      // Want clamping?
                            ref Tangent);           // Out

                        ThisPoint.ArriveTangent = Tangent;
                        ThisPoint.LeaveTangent = Tangent;
                    }
                    else
                    {
                        // Following on from a line or constant; set curve tangent equal to that so there are no discontinuities
                        ThisPoint.ArriveTangent = PrevPoint.ArriveTangent;
                        ThisPoint.LeaveTangent = PrevPoint.LeaveTangent;
                    }
                }
                else if (ThisPoint.InterpMode == ESplinePointType.Linear)
                {
                    float Tangent = Minus(NextPoint.OutVal, ThisPoint.OutVal);
                    ThisPoint.ArriveTangent = Tangent;
                    ThisPoint.LeaveTangent = Tangent;
                }
                else if (ThisPoint.InterpMode == ESplinePointType.Constant)
                {
                    ThisPoint.ArriveTangent = ForceInit();
                    ThisPoint.LeaveTangent = ForceInit();
                }
            }
        }

        public override void Reset()
        {
            Points.Clear();
        }

        ///////////////////////////////////////////////////////////////////////////////////////////////////
        public FInterpCurveFloat() : base()
        { }
        protected override float Lerp(float _tValue1, float _tValue2, float _fAlpha)
        {
            return Mathf.Lerp(_tValue1, _tValue2, _fAlpha);
        }

        protected override float CubicInterp(float P0, float T0, float P1, float T1, float A, float _fDiff = 1)
        {
            //这步应该在外面处理，由于T类型不确定所以放里面处理
            T0 = T0 * _fDiff;
            T1 = T1 * _fDiff;
            float A2 = A * A;
            float A3 = A2 * A;
            return (float)(((2 * A3) - (3 * A2) + 1) * P0) + ((A3 - (2 * A2) + A) * T0) + ((A3 - A2) * T1) + (((-2 * A3) + (3 * A2)) * P1);

        }

        protected override float Minus(float _tValue1, float _tValue2)
        {
            return _tValue1 - _tValue2;
        }

        protected override float ForceInit()
        {
            return 0f;
        }
        protected override void AutoCalcTangent(float PrevP, float P, float NextP, float Tension, ref float OutTan)
        {
            OutTan = (1f - Tension) * ((P - PrevP) + (NextP - P));
        }

        protected override float CubicInterpDerivative(float P0, float T0, float P1, float T1, float A, float _fDiff = 1)
        {
            //这步应该在外面处理，由于父类是T类型不确定所以放里面处理
            T0 *= _fDiff;
            T1 *= _fDiff;

            float a = 6f * P0 + 3f * T0 + 3f * T1 - 6f * P1;
            float b = -6f * P0 - 4f * T0 - 2f * T1 + 6f * P1;
            float c = T0;

            float A2 = A * A;
            float result = (a * A2) + (b * A) + c;
            //这步应该在外面处理，由于父类是T类型不确定所以放里面处理
            result /= _fDiff;

            return result;
        }

        protected override float Divide(float _tValue1, float _fScale)
        {
            return _tValue1 / _fScale;
        }


    }
    #endregion


    public static class MathfExtensions
    {
        /// <summary>
        /// 向量转换为四元数
        /// </summary>
        public static Quaternion ToQuaternion(this Vector3 _v3Target)
        {
            if (float.IsNaN(_v3Target.x) || float.IsNaN(_v3Target.y) || float.IsNaN(_v3Target.z))
                return Quaternion.identity;

        Vector3 euler = Vector3.zero;

		//坐标系转换处理
        ////  R.Roll = 0;
        euler.x = 0;
        ////  R.Yaw = FMath::Atan2(Y,X) * (180.f / PI);
        euler.y = Mathf.Atan2(_v3Target.x, _v3Target.z) * (Mathf.Rad2Deg);
        ////  R.Pitch = FMath::Atan2(Z, FMath::Sqrt(X * X + Y * Y)) * (180.f / PI);
        euler.z = Mathf.Atan2(_v3Target.y, Mathf.Sqrt(_v3Target.z * _v3Target.z + _v3Target.x * _v3Target.x)) * (Mathf.Rad2Deg);


        euler.y -= 90;
        Quaternion quat = Quaternion.Euler(euler);
        return quat;
    }

        /// <summary>
        /// 避免向量长度小于 float.MinValue 时返回0向量
        /// </summary>
        public static Vector3 GetSafeNormal(this Vector3 _value)
        {
            float SquareSum = _value.x * _value.x + _value.y * _value.y + _value.z * _value.z;

            if (SquareSum == 1f)
            {
                return _value;
            }
            else if (SquareSum < float.MinValue)
            {
                return Vector3.zero;
            }
            float Scale = InvSqrt(SquareSum);//InvSqrt(SquareSum);
            return new Vector3(_value.x * Scale, _value.y * Scale, _value.z * Scale);
        }

        public static Vector3 Caret(Vector3 _value1, Vector3 _value2)
        {
            Vector3 result = Vector3.zero;

        // x = Y * V.Z - Z * V.Y,
        result.z = _value1.x * _value2.y - _value1.y * _value2.x;
        //result.x = _value1.y * _value2.z - _value1.z * _value2.y;

        // y = Z* V.X - X * V.Z,
        result.x = _value1.y * _value2.z - _value1.z * _value2.y;
        //result.y = _value1.z * _value2.x - _value1.x * _value2.z;

        //z = X* V.Y - Y * V.X
        result.y = _value1.z * _value2.x - _value1.x * _value2.z;
        //result.z = _value1.x * _value2.y - _value1.y * _value2.x;

            return result;
        }

        public static float Or(Vector3 _value1, Vector3 _value2)
        {
            //X*V.X + Y*V.Y + Z*V.Z;
            return _value1.x * _value2.x + _value1.y * _value2.y + _value1.z + _value2.z;
        }

        /// <summary>
        /// 根据两向量的X轴和Y轴取矩阵
        /// </summary>
        public static Matrix4x4 MakeFromXZ(Vector3 _vec3XAxis, Vector3 _vec3ZAxis)
        {
            Matrix4x4 matrix = new Matrix4x4();

            Vector3 NewX = _vec3XAxis.GetSafeNormal();//GetSafeNormal(_vec3XAxis);
            Vector3 Norm = _vec3ZAxis.GetSafeNormal();//GetSafeNormal(_vec3ZAxis);

        //// if they're almost same, we need to find arbitrary vector
        if (IsNearlyEqual(Mathf.Abs(Or(NewX, Norm)), 1))
        {
            // make sure we don't ever pick the same as NewX
            //Norm = (Mathf.Abs(NewX.z) < (1f - float.MinValue)) ? new Vector3(0, 0, 1f) : new Vector3(1f, 0, 0);
            Norm = (Mathf.Abs(NewX.y) < (1f - float.MinValue)) ? new Vector3(0, 1f, 0) : new Vector3(0, 0, 1f);
        }

        Vector3 NewY = Caret(Norm, NewX).GetSafeNormal();
        Vector3 NewZ = Caret(NewX, NewY);

        //matrix[0,0] = NewX.x; matrix[0,1] = NewX.y; matrix[0,2] = NewX.z; matrix[0,3] = 0.0f;
        //matrix[1,0] = NewY.x; matrix[1,1] = NewY.y; matrix[1,2] = NewY.z; matrix[1,3] = 0.0f;
        //matrix[2,0] = NewZ.x; matrix[2,1] = NewZ.y; matrix[2,2] = NewZ.z; matrix[2,3] = 0.0f;
        //matrix[3,0] = 0.0f  ; matrix[3,1] = 0.0f  ; matrix[3,2] = 0.0f  ; matrix[3,3] = 1.0f;

        matrix[0, 0] = NewX.z; matrix[0, 1] = NewX.x; matrix[0, 2] = NewX.y; matrix[0, 3] = 0.0f;
        matrix[1, 0] = NewY.z; matrix[1, 1] = NewY.x; matrix[1, 2] = NewY.y; matrix[1, 3] = 0.0f;
        matrix[2, 0] = NewZ.z; matrix[2, 1] = NewZ.x; matrix[2, 2] = NewZ.y; matrix[2, 3] = 0.0f;
        matrix[3, 0] = 0.0f  ; matrix[3, 1] = 0.0f  ; matrix[3, 2] = 0.0f  ; matrix[3, 3] = 1.0f;

            return matrix;
        }

        public static bool IsNearlyEquals(this Vector3 _v3This,Vector3 _v3Target, float Tolerance)
        {
            return Mathf.Abs(_v3This.x - _v3Target.x) <= Tolerance && Mathf.Abs(_v3This.y - _v3Target.y) <= Tolerance && Mathf.Abs(_v3This.z - _v3Target.z) <= Tolerance;
        }



        /// <summary>
        /// 根据四元数旋转向量
        /// </summary>
        public static Vector3 RotateVector(this Quaternion _qQuat, Vector3 _v3Target)
        {
            // V' = V + w(2(Q x V)) + (Q x (2(Q x V)))
            // T = 2(Q x V);
            // V' = V + w*(T) + (Q x T)
            Vector3 Result = Vector3.zero;

            Vector3 Q = new Vector3(_qQuat.x, _qQuat.y, _qQuat.z);
            Vector3 T = 2f * Vector3.Cross(Q, _v3Target);
            Result = _v3Target + (_qQuat.w * T) + Vector3.Cross(Q, T);

            return Result;
        }

        /// <summary>
        /// 矩阵转换为四元数
        /// </summary>
        public static Quaternion ToQuat(this Matrix4x4 _matrix)
        {
            Quaternion quat = Quaternion.identity;

            //const MeReal *const t = (MeReal *) tm;
            float s;

            // 检测矩阵对角线是否合理
            float tr = _matrix.m00 + _matrix.m11 + _matrix.m22;

            if (tr > 0.0f)
            {
                float InvS = InvSqrt(tr + 1f);
                quat.w = 0.5f * (1f / InvS);
                s = 0.5f * InvS;

                quat.x = (_matrix.m12 - _matrix.m21) * s;
                quat.y = (_matrix.m20 - _matrix.m02) * s;
                quat.z = (_matrix.m01 - _matrix.m10) * s;
            }
            else
            {
                // 对角线为负值
                int i = 0;

                if (_matrix.m11 > _matrix.m00)
                    i = 1;

                if (_matrix.m22 > _matrix[i, i])
                    i = 2;

                int[] nxt = new int[3] { 1, 2, 0 };
                int j = nxt[i];
                int k = nxt[j];

                s = _matrix[i, i] - _matrix[j, j] - _matrix[k, k] + 1.0f;

                float InvS = InvSqrt(s);

                float[] qt = new float[4];
                qt[i] = 0.5f * (1f / InvS);

                s = 0.5f * InvS;

                qt[3] = (_matrix[j, k] - _matrix[k, j]) * s;
                qt[j] = (_matrix[i, j] + _matrix[j, i]) * s;
                qt[k] = (_matrix[i, k] + _matrix[k, i]) * s;

                quat.x = qt[0];
                quat.y = qt[1];
                quat.z = qt[2];
                quat.w = qt[3];
            }

            return quat;
        }

        /// <summary>
        /// 1.0/sqrt(value)
        /// </summary>
        public static float InvSqrt(float x)
        {
            float xhalf = 0.5f * x;
            int i = (int)x;
            i = 0x5f3759df - (i >> 1);
            x = i;
            x = x * (1.5f - xhalf * x * x);
            return x;
        }

        public static bool IsNearlyEqual(float _value1, float _value2, float ErrorTolerance = float.MinValue)
        {
            return Mathf.Abs(_value1 - _value2) <= ErrorTolerance;
        }
    }

}
