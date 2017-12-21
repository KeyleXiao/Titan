using UnityEngine;
using System.Collections;
using System;
using System.Collections.Generic;
using DG.Tweening;
using UnityEngine.Events;
using UnityEngine.UI;

namespace USpeedUI.UWidgets.UAnimator
{
    //[Serializable]
    //public class DOTStruct<T>
    //{
    //    public T BeginValue;
    //    public T EndValue;
        
    //    public DOTStruct(T _BeginValue, T _EndValue)
    //    {
    //        BeginValue = _BeginValue;
    //        EndValue = _EndValue;
    //    }
    //}

    [Serializable]
    public struct DOTWrapper_Float 
    {
        [SerializeField]
        public float BeginValue;
        [SerializeField]
        public float EndValue;
    }
    [Serializable]
    public struct DOTWrapper_Vector3 
    {
        [SerializeField]
        public Vector3 BeginValue;
        [SerializeField]
        public Vector3 EndValue;

        public Quaternion ToQuaternion(Vector3 _vec3Value)
        {
            Quaternion newRotation = Quaternion.identity;
            newRotation.eulerAngles = _vec3Value;
            return newRotation;
        }
    }

    [Serializable]
    public struct DOTWrapper_Vector2 
    {
        [SerializeField]
        public Vector2 BeginValue;
        [SerializeField]
        public Vector2 EndValue;
    }


    //UViewAnim_Element
    //    UViewAnim_LocalMove
    //    UViewAnim_PunchPosition
    //    UViewAnim_ShakePosition
    //    UViewAnim_LocalRotate
    //    UViewAnim_LocalScale
    //    UViewAnim_Fade
    [Serializable]
    public class UViewAnimElement
    {
        /// <summary>
        /// Tween 添加到Sequence方式
        /// </summary>
        public enum ETweenAttachType
        {
            ETA_Append,
            ETA_Insert,
            ETA_Join,
            ETA_Prepend,
        }
        public ETweenAttachType TweenAttachType;
        /// <summary>
        /// 插入类型，插入时间点
        /// </summary>
        public float TweenAttachType_Insert_Pos;

        /// <summary>
        /// Tween类型
        /// </summary>
        public enum ETweenType
        {
            ETT_AnchorPosition,
            ETT_LocalRotate,
            ETT_LocalScale,
            ETT_Fade,

            ETT_PunchPosition,
            ETT_ShakePosition,
            ETT_LocalPosition,




            ETT_Custom
        }
        public ETweenType TweenType;

        public Ease EaseValue = Ease.InOutQuad;

        public float DurationValue;
        public float DelayValue;

        public bool IsCustomTarget;
        public RectTransform CustomTarget;

        /// ETT_LocalPosition
        /// ETT_PunchPosition
        public DOTWrapper_Vector2 Vector2TypeValue;

        ///ETT_LocalRotate
        ///ETT_LocalScale
        public DOTWrapper_Vector3 Vector3TypeValue;

        //ETT_Fade
        public DOTWrapper_Float FloatTypeValue;

        public Vector2 PunchPositionValue;
        public float PunchPositionElasticity = 1;

        //ETT_ShakePosition
        public float ShakeStrength = 100;
        public float ShakeRandomness = 90;

        //ETT_PunchPosition
        //ETT_ShakePosition
        public int VibratoValue = 10;

        //ETT_Custom
        public int CustomID;
        public UViewCustomAnimCollection  CustomCollection;

    }

    //UViewAnimState
    //    UViewAnimState_Enter
    //    UViewAnimState_Execute
    //    UViewAnimState_Leave
    [Serializable]
    public class UViewAnimState
    {
        public float DelayValue;

        public Sequence CurrentSequence;

        public List<UViewAnimElement> ElementList = new List<UViewAnimElement>();

        protected RectTransform m_nTarget;
        public RectTransform Target { get { return m_nTarget; } }


        //protected CanvasGroup m_nCanvasGroup;
        //public CanvasGroup CanvasGroup
        //{
        //    get
        //    {
        //        if (m_nCanvasGroup == null && m_nTarget != null)
        //        {
        //            m_nCanvasGroup = m_nTarget.GetComponent<CanvasGroup>();
        //            if (m_nCanvasGroup == null)
        //                m_nCanvasGroup = m_nTarget.gameObject.AddComponent<CanvasGroup>();
        //        }
        //        return m_nCanvasGroup;
        //    }
        //}

        protected CanvasGroup GetTargetCanvasGroup(Transform _tfTarget)
        {
            if (_tfTarget == null)
                return null;

            CanvasGroup canvasGroup = _tfTarget.GetComponent<CanvasGroup>();
            if (canvasGroup == null)
                canvasGroup = _tfTarget.gameObject.AddComponent<CanvasGroup>();

            return canvasGroup;
        }

        protected void InitAnimElement()
        {
            for (int i = 0; i < ElementList.Count; ++i)
            {
                SetAnimElementInit(ElementList[i]);
            }
        }

        public void InitState(RectTransform _target, bool _bInitAnimElement = false)
        {
            m_nTarget = _target;

            if (_bInitAnimElement)
                InitAnimElement();

            InitSequence();
        }

        protected void DesignAnimElement()
        {
            for (int i = 0; i < ElementList.Count; ++i)
            {
                SetAnimElementInit(ElementList[i], true);
            }
        }

        public void InitDesignState(RectTransform _target)
        {
            m_nTarget = _target;

            DesignAnimElement();
        }

        protected void InitSequence()
        {
            if (ElementList == null || ElementList.Count == 0)
                return;

            CurrentSequence = DOTween.Sequence();

            for (int i = 0; i < ElementList.Count; ++i)
            {
                SetElement(ElementList[i]);
            }
            CurrentSequence.SetDelay(DelayValue);
        }

        protected void SetElement(UViewAnimElement _newElement)
        {
            Tween newTween = SetTween(_newElement);

            if (newTween == null)
                return;

            newTween.SetAutoKill(true);
            newTween.SetDelay(_newElement.DelayValue);
            newTween.SetEase(_newElement.EaseValue);

            switch (_newElement.TweenAttachType)
            {
                case UViewAnimElement.ETweenAttachType.ETA_Append:
                    {
                        CurrentSequence.Append(newTween);
                    }
                    break;
                case UViewAnimElement.ETweenAttachType.ETA_Insert:
                    {
                        CurrentSequence.Insert(_newElement.TweenAttachType_Insert_Pos, newTween);
                    }
                    break;
                case UViewAnimElement.ETweenAttachType.ETA_Join:
                    {
                        CurrentSequence.Join(newTween);
                    }
                    break;
                case UViewAnimElement.ETweenAttachType.ETA_Prepend:
                    {
                        CurrentSequence.Prepend(newTween);
                    }
                    break;
                default:
                    Debug.LogWarning("这里会报错？那就出现重大大问题了。");
                    break;
            }
        }

        protected Tween SetTween(UViewAnimElement _newElement)
        {
            RectTransform tmpTarget = (_newElement.IsCustomTarget && _newElement.CustomTarget != null) ? _newElement.CustomTarget : Target != null ? Target : null;
            if (tmpTarget == null)
                return null;

            switch (_newElement.TweenType)
            {
                case UViewAnimElement.ETweenType.ETT_Fade:
                    {
                        CanvasGroup canvasGroup = GetTargetCanvasGroup(tmpTarget);
                        return canvasGroup == null ? null : canvasGroup.DOFade(_newElement.FloatTypeValue.EndValue, _newElement.DurationValue);
                    }
                case UViewAnimElement.ETweenType.ETT_AnchorPosition:
                    {
                        return tmpTarget.DOAnchorPos(_newElement.Vector2TypeValue.EndValue, _newElement.DurationValue);
                    }
                case UViewAnimElement.ETweenType.ETT_LocalRotate:
                    {
                        return tmpTarget.DORotate(_newElement.Vector3TypeValue.EndValue, _newElement.DurationValue);
                    }
                case UViewAnimElement.ETweenType.ETT_LocalScale:
                    {
                        return tmpTarget.DOScale(_newElement.Vector3TypeValue.EndValue, _newElement.DurationValue);
                    }
                case UViewAnimElement.ETweenType.ETT_PunchPosition:
                    {
                        return tmpTarget.DOPunchAnchorPos(_newElement.Vector2TypeValue.EndValue, _newElement.DurationValue, _newElement.VibratoValue, _newElement.PunchPositionElasticity);
                    }
                case UViewAnimElement.ETweenType.ETT_ShakePosition:
                    {
                        return tmpTarget.DOShakeAnchorPos(_newElement.DurationValue, _newElement.ShakeStrength, _newElement.VibratoValue, _newElement.ShakeRandomness);
                    }
                case UViewAnimElement.ETweenType.ETT_LocalPosition:
                    {
                        return tmpTarget.DOLocalMove(_newElement.Vector2TypeValue.EndValue, _newElement.DurationValue);
                    }
                case UViewAnimElement.ETweenType.ETT_Custom:
                    {
                        if (_newElement.CustomCollection != null)
                            return _newElement.CustomCollection.GetTweener(_newElement.CustomID, _newElement.DurationValue);
                        else
                            return null;
                    }
                default:
                    Debug.LogWarning("这里会报错？那就出现重大大问题了。");
                    return null;
            }
        }

        protected void SetAnimElementInit(UViewAnimElement _newElement, bool _bInDesign = false)
        {
            RectTransform tmpTarget = (_newElement.IsCustomTarget && _newElement.CustomTarget != null) ? _newElement.CustomTarget : Target != null ? Target : null;
            if (tmpTarget == null)
                return ;

            switch (_newElement.TweenType)
            {
                case UViewAnimElement.ETweenType.ETT_Fade:
                    {
                        CanvasGroup canvasGroup = GetTargetCanvasGroup(tmpTarget);

                        canvasGroup.alpha = _bInDesign ? _newElement.FloatTypeValue.EndValue : _newElement.FloatTypeValue.BeginValue;
                    }
                    break;
                case UViewAnimElement.ETweenType.ETT_AnchorPosition:
                    {
                        tmpTarget.anchoredPosition = _bInDesign ? _newElement.Vector2TypeValue.EndValue : _newElement.Vector2TypeValue.BeginValue;
                    }
                    break;
                case UViewAnimElement.ETweenType.ETT_LocalRotate:
                    {
                        tmpTarget.localEulerAngles = _bInDesign ? _newElement.Vector3TypeValue.EndValue : _newElement.Vector3TypeValue.BeginValue;
                    }
                    break;
                case UViewAnimElement.ETweenType.ETT_LocalScale:
                    {
                        tmpTarget.localScale = _bInDesign ? _newElement.Vector3TypeValue.EndValue : _newElement.Vector3TypeValue.BeginValue;
                    }
                    break;
                case UViewAnimElement.ETweenType.ETT_LocalPosition:
                    {
                        tmpTarget.localPosition = _bInDesign ? _newElement.Vector3TypeValue.EndValue : _newElement.Vector3TypeValue.BeginValue;
                    }
                    break;
                case UViewAnimElement.ETweenType.ETT_Custom:
                    {
                        if (_newElement.CustomCollection != null)
                        {
                            if (_bInDesign)
                                _newElement.CustomCollection.DesignAnim(_newElement.CustomID);
                            else
                                _newElement.CustomCollection.InitAnim(_newElement.CustomID);
                        }
                    }
                    break;
                default:
                    break;
            }

        }

        public void AddElement(UViewAnimElement _newElement)
        {
            if (ElementList.Contains(_newElement))
                return;

            ElementList.Add(_newElement);
        }
    }

    //View
    //    enter
    //        UViewAnim_Element_1
    //        UViewAnim_Element_2
    //    execute
    //    leave
    public class UViewAnim : MonoBehaviour
    {
        public enum EViewAnimState
        {
            EVA_None,
            EVA_Enter,
            EVA_Execute,
            EVA_Leave,
        }

        public UViewAnimState EnterState;
        public UViewAnimState ExecuteState;
        public UViewAnimState LeaveState;

        //public EViewAnimState CurrentAnimState;

        protected RectTransform m_nRectTransform = null;
        public RectTransform rectTransform
        {
            get
            {
                if (m_nRectTransform == null)
                {
                    m_nRectTransform = GetComponent<RectTransform>();
                }
                return m_nRectTransform;
            }
        }

        public Vector2 DesignModePosition;
        public Vector2 DesignModeSizeDelta;
        //protected CanvasGroup m_nCanvasGroup;
        //public CanvasGroup CanvasGroup
        //{
        //    get
        //    {
        //        if (m_nCanvasGroup == null)
        //        {
        //            m_nCanvasGroup = GetComponent<CanvasGroup>();
        //            if (m_nCanvasGroup == null)
        //                m_nCanvasGroup = gameObject.AddComponent<CanvasGroup>();
        //        }
        //        return m_nCanvasGroup;
        //    }
        //}


        public void PlayForward(EViewAnimState _eAnimState = EViewAnimState.EVA_Enter)
        {
            //CurrentAnimState = _eAnimState;
            switch (_eAnimState)
            {
                case EViewAnimState.EVA_Enter:
                    {
                        if (EnterState != null)
                        {
                            EnterState.InitState(rectTransform, true);
                            EnterState.CurrentSequence.PlayForward();
                        }
                    }
                    break;
                case EViewAnimState.EVA_Execute:
                    {
                        if (ExecuteState != null)
                        {
                            ExecuteState.InitState(rectTransform);
                            ExecuteState.CurrentSequence.PlayForward();
                        }
                    }
                    break;
                case EViewAnimState.EVA_Leave:
                    {
                        if (LeaveState != null)
                        {
                            LeaveState.InitState(rectTransform);
                            LeaveState.CurrentSequence.PlayForward();
                        }
                    }
                    break;
                default:
                    break;
            }
        }

        public void InitRectTransformValue()
        {
            if (EnterState != null)
            {
                EnterState.InitState(rectTransform, true);
            }
            //switch (CurrentAnimState)
            //{
            //    case EViewAnimState.EVA_Enter:
            //        {
            //            if (EnterState != null)
            //            {
            //                EnterState.InitState(rectTransform);
            //            }
            //        }
            //        break;
            //    case EViewAnimState.EVA_Execute:
            //        {
            //            if (ExecuteState != null)
            //            {
            //                ExecuteState.InitState(rectTransform);
            //            }
            //        }
            //        break;
            //    case EViewAnimState.EVA_Leave:
            //        {
            //            if (LeaveState != null)
            //            {
            //                LeaveState.InitState(rectTransform);
            //            }
            //        }
            //        break;
            //    default:
            //        break;
            //}
        }

        public void DesignMode()
        {
            rectTransform.anchoredPosition = DesignModePosition;
            rectTransform.sizeDelta = DesignModeSizeDelta;
            rectTransform.localRotation = Quaternion.identity;
            rectTransform.localScale = Vector3.one;

            if (EnterState != null)
            {
                EnterState.InitDesignState(rectTransform);
            }
        }
    }
}