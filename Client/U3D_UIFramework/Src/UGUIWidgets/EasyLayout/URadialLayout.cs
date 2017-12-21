/*******************************************************************
** 文件名:	URadialLayout.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-9-26
** 版  本:	1.0
** 描  述:	中心辐射型的布局工具
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

namespace UIWidgets
{
    public class URadialLayout : LayoutGroup
    {
        public float fDistance;
        [Range(0f, 360f)]
        public float MinAngle, MaxAngle, StartAngle;

        public override void SetLayoutHorizontal()
        {
        }

        public override void SetLayoutVertical()
        {
        }
        protected override void OnEnable() { base.OnEnable(); CalculateRadial(); }
        public override void CalculateLayoutInputVertical()
        {
            CalculateRadial();
        }
        public override void CalculateLayoutInputHorizontal()
        {
            CalculateRadial();
        }
#if UNITY_EDITOR
        protected override void OnValidate()
        {
            base.OnValidate();
            CalculateRadial();
        }
#endif
        public void CalculateRadial()
        {
            m_Tracker.Clear();
            if (transform.childCount == 0)
                return;
            float fOffsetAngle = ((MaxAngle - MinAngle)) / (transform.childCount-1);

            float fAngle = StartAngle;
            for (int i = 0; i < transform.childCount; i++)
            {
                RectTransform child = (RectTransform)transform.GetChild(i);
                if (child != null)
                {
                    //添加子项不允许修改的属性
                    m_Tracker.Add(this, child,
                    DrivenTransformProperties.Anchors |
                    DrivenTransformProperties.AnchoredPosition |
                    DrivenTransformProperties.Pivot);
                    Vector3 vPos = new Vector3(Mathf.Cos(fAngle * Mathf.Deg2Rad), Mathf.Sin(fAngle * Mathf.Deg2Rad), 0);
                    child.localPosition = vPos * fDistance;
                    //统一用中心对其
                    child.anchorMin = child.anchorMax = child.pivot = new Vector2(0.5f, 0.5f);
                    fAngle += fOffsetAngle;
                }
            }

        }
    }

    public class UCurvedLayout : LayoutGroup
    {
        protected override void OnEnable()
        {
            rectTrans = GetComponent<RectTransform>();
            base.OnEnable();
            OnRectTransformDimensionsChange();
            CalculateCurved();
        }

        protected override void Awake()
        {
            base.Awake();
            rectTrans = GetComponent<RectTransform>();
            OnRectTransformDimensionsChange();
        }

        public override void CalculateLayoutInputVertical()
        {
            CalculateCurved();
        }
        public override void CalculateLayoutInputHorizontal()
        {
            CalculateCurved();
        }

#if UNITY_EDITOR
        protected override void OnValidate()
        {
            
            base.OnValidate();
            CalculateCurved();
        }
#endif

        public override void SetLayoutHorizontal()
        {
        }

        public override void SetLayoutVertical()
        {
        }

        public AnimationCurve curveForChild = AnimationCurve.Linear(0, 0, 1, 0); //AnimationCurve.Linear(0, 0, 1, 10);
        public float curveMultiplier = 1;
        public float fDistance;
        private RectTransform rectTrans;

        public void CalculateCurved()
        {
            m_Tracker.Clear();
            var childCount = rectTrans.childCount;

            if (childCount == 0)
                return;

            var tmpDistance = fDistance;
            if (fDistance.Equals(0))
            {
                tmpDistance = rectTrans.rect.width / (childCount-1);
            }

            for (int i = 0; i < childCount; i++)
            {
                RectTransform child = (RectTransform)rectTrans.GetChild(i);
                float tmpX, tmpY;
                if (child != null)
                {
                    //添加子项不允许修改的属性
                    m_Tracker.Add(this, child,
                    DrivenTransformProperties.Anchors |
                    DrivenTransformProperties.AnchoredPosition |
                    DrivenTransformProperties.Pivot);

                    tmpX = (rectTrans.pivot.x )+ i * tmpDistance;
                    tmpY = ( rectTrans.pivot.y )+ curveForChild.Evaluate(i * tmpDistance) * curveMultiplier;

                    child.localPosition = new Vector2(tmpX,tmpY);
                    //统一用中心对其
                    child.anchorMin = child.anchorMax = child.pivot = new Vector2(0.5f, 0.5f);
                }

            }
        }

        protected override void OnRectTransformDimensionsChange()
        {
            var tmpRect = curveForChild[curveForChild.length - 1];
            tmpRect.time = rectTrans.rect.width;
            curveForChild.MoveKey(curveForChild.length - 1, tmpRect);

            CalculateCurved();
        }
    }

}
