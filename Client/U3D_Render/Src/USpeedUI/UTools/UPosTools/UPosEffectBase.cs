using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace USpeedUI.UEffect
{
    public enum EPosEffectType
    {
        PET_None,
        PET_Curved,
        PET_Radial,
        PET_Max
    }
    public interface IPosEffect
    {
        void PlayAnim();
        void CalculatePos();
        /// <summary>
        /// 这个只有RectTransform才有
        /// </summary>
        void RectTransformDimensionsChange();
        void AddActionTrans(Transform _tTrans);
        void RemoveActionTrans(Transform _tTrans);
        void RemoveAllActionTrans();
        void DestroyPosEffect();
        EPosEffectType GetPosEffectType();
    }
    abstract public class UPosEffectBase : IPosEffect
    {
        /// <summary>
        /// 效果作用Transform父节点
        /// </summary>
        public Transform EffectTrans;

        /// <summary>
        /// 效果作用的Transform列表
        /// </summary>
        protected List<Transform> m_ActionTransList;

        protected bool m_bActionTransIsChild = false;

        public UPosEffectBase (Transform _tEffectTrans, List<Transform>  _listActionTrans = null)
        {
            EffectTrans = _tEffectTrans;
            m_bActionTransIsChild = false;
            m_ActionTransList = _listActionTrans;
            if (_listActionTrans == null)
            {
                m_bActionTransIsChild = true;

                GetChildTrans();
            }
                
        }

        protected void GetChildTrans()
        {
            m_ActionTransList = new List<Transform>();
            var childCount = EffectTrans.childCount;
            for (int i = 0; i < childCount; i++)
            {
                m_ActionTransList.Add(EffectTrans.GetChild(i));
            }

        }
        virtual public void PlayAnim() { }
        virtual public void CalculatePos()
        {
            if (m_bActionTransIsChild)
                GetChildTrans();
            if (EffectTrans is RectTransform)
                CalculateRectTransformPos();
            else
                CalculateTransformPos();
        }

        /// <summary>
        /// 当Transform时 UI Transform时走这里
        /// </summary>
        virtual protected void CalculateRectTransformPos() { }

        /// <summary>
        /// 当Transform 非UI Transform时走这里
        /// </summary>
        virtual protected void CalculateTransformPos() { }

        virtual public void RectTransformDimensionsChange() { }

        public void AddActionTrans(Transform _tTrans)
        {
            if (m_ActionTransList == null)
                m_ActionTransList = new List<Transform>();

            if (!m_ActionTransList.Contains(_tTrans))
                m_ActionTransList.Add(_tTrans);
        }

        public void RemoveActionTrans(Transform _tTrans)
        {
            if (m_ActionTransList == null)
                return;
            if (m_ActionTransList.Contains(_tTrans))
                m_ActionTransList.Remove(_tTrans);
        }

        public void RemoveAllActionTrans()
        {
            if (m_ActionTransList == null)
                return;

            m_ActionTransList.Clear();
        }

        abstract public EPosEffectType GetPosEffectType();

        virtual public void DestroyPosEffect()
        {
            m_ActionTransList.Clear();
            EffectTrans = null;
        }
        
    }

    public class UPosEffect_Curved : UPosEffectBase
    {
        public UPosEffect_Curved(Transform _tEffectTrans, List<Transform> _listActionTrans = null) :base(_tEffectTrans, _listActionTrans)
        { }


        public AnimationCurve CurveForPosition = AnimationCurve.Linear(0, 0, 1, 0); //AnimationCurve.Linear(0, 0, 1, 10);
        /// <summary>
        /// 曲线的Value缩放
        /// </summary>
        public float CurveVerticalMultiplier = 1;
        /// <summary>
        /// 曲线Time缩放，为0时根据RectTransform的width和作用对象数量决定
        /// </summary>
        public float CurveHorizontalMultiplier = 0f;

        protected override void CalculateRectTransformPos()
        {
            base.CalculateRectTransformPos();
            if (m_ActionTransList == null || m_ActionTransList.Count == 0)
                return;

            var childCount = m_ActionTransList.Count;

            RectTransform rectTrans = EffectTrans as RectTransform;

            var tmpHorizontalMultiplier = CurveHorizontalMultiplier;
            if (CurveHorizontalMultiplier.Equals(0))
            {
                tmpHorizontalMultiplier = rectTrans.rect.width / (childCount - 1);
            }

            for (int i = 0; i < childCount; i++)
            {
                RectTransform child = (RectTransform)m_ActionTransList[i];
                float tmpX, tmpY;
                if (child != null)
                {
                    tmpX = ( rectTrans.pivot.x) + i * tmpHorizontalMultiplier;
                    tmpY = ( rectTrans.pivot.y) + CurveForPosition.Evaluate(i * tmpHorizontalMultiplier) * CurveVerticalMultiplier;
                    
                    child.localPosition = new Vector2(tmpX, tmpY);
                    //统一用中心对其
                    child.anchorMin = child.anchorMax = child.pivot = new Vector2(0.5f, 0.5f);
                }
            }
        }

        public override EPosEffectType GetPosEffectType()
        {
            return EPosEffectType.PET_Curved;
        }

        public override void RectTransformDimensionsChange()
        {
            var tmpRect = CurveForPosition[CurveForPosition.length - 1];
            tmpRect.time = (EffectTrans as RectTransform).rect.width;
            CurveForPosition.MoveKey(CurveForPosition.length - 1, tmpRect);

        }
    }

    public class UPosEffect_Radial : UPosEffectBase
    {
        public UPosEffect_Radial(Transform _tEffectTrans, List<Transform> _listActionTrans = null) :base(_tEffectTrans, _listActionTrans)
        { }

        /// <summary>
        /// 辐射半径
        /// </summary>
        public float RadialRadius = 0f;
        [Range(0f, 360f)]
        public float MinAngle = 0f;
        [Range(0f, 360f)]
        public float MaxAngle = 0f;
        [Range(0f, 360f)]
        public float StartAngle = 0f;

        protected override void CalculateRectTransformPos()
        {
            base.CalculateRectTransformPos();

            if (m_ActionTransList == null || m_ActionTransList.Count == 0)
                return;

            var childCount = m_ActionTransList.Count;

            RectTransform rectTrans = EffectTrans as RectTransform;


            float fOffsetAngle = ((MaxAngle - MinAngle)) / (childCount - 1);

            float fAngle = StartAngle;
            for (int i = 0; i < childCount; i++)
            {
                RectTransform child = (RectTransform)m_ActionTransList[i];

                if (child != null)
                {
                    Vector3 vPos = new Vector3(Mathf.Cos(fAngle * Mathf.Deg2Rad), Mathf.Sin(fAngle * Mathf.Deg2Rad), 0);
                    child.localPosition = vPos * RadialRadius;
                    //统一用中心对其
                    child.anchorMin = child.anchorMax = child.pivot = new Vector2(0.5f, 0.5f);
                    fAngle += fOffsetAngle;
                }
            }
        }

        public override EPosEffectType GetPosEffectType()
        {
            return EPosEffectType.PET_Radial;
        }
    }
}
