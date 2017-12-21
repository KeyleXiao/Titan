using UnityEngine;
using UnityEditor;
using System.Collections.Generic;
using USpeedUI.UWidgets.UAnimator;

[CustomEditor(typeof(UViewAnim))]
public class UViewAnimEditor : Editor
{
    protected struct AnimStateEditorData
    {
        public bool IsStateExpend;
        public Vector2 StateScrollPos;
        public List<bool> IsElememtExpend;

        public AnimStateEditorData(bool _isStateExpend)
        {
            IsStateExpend = _isStateExpend;
            StateScrollPos = Vector2.zero;
            IsElememtExpend = new List<bool>();
        }
    }
    public UViewAnim _src;

    private bool[] expendType = new bool[3] { false,false,false };
    private Vector2 scrollPos = Vector2.zero;

    private AnimStateEditorData[] m_nStateData = new AnimStateEditorData[3] { new AnimStateEditorData(false),new AnimStateEditorData(false),new AnimStateEditorData(false) };

    UViewAnim.EViewAnimState CurrentAnimState;
    void OnEnable()
    {
        _src = target as UViewAnim;
    }

    public override void OnInspectorGUI()
    {
        //base.OnInspectorGUI();
        //GUILayout.BeginVertical();
        {
          //  GUILayout.BeginScrollView(scrollPos);
            {
                SetAnimStateEditor(_src.EnterState, 0, "EnterState");
                SetAnimStateEditor(_src.ExecuteState, 1, "ExecuteState");
                SetAnimStateEditor(_src.LeaveState, 2, "ExitState");
            }
           // GUILayout.EndScrollView();
        }
        //GUILayout.EndVertical();

        CurrentAnimState = (UViewAnim.EViewAnimState)EditorGUILayout.EnumPopup("Current Anim State", CurrentAnimState);
        _src.DesignModePosition = EditorGUILayout.Vector2Field("Design Mode Position", _src.DesignModePosition);
        _src.DesignModeSizeDelta = EditorGUILayout.Vector2Field("Design Mode SizeDelta", _src.DesignModeSizeDelta);

        GUILayout.BeginVertical();
        GUILayout.BeginHorizontal();

        if (GUILayout.Button("InitValue"))
            _src.InitRectTransformValue();
        if (GUILayout.Button("DesignMode"))
            _src.DesignMode();
        GUILayout.EndHorizontal();


        if (Application.isPlaying)
        {
            GUILayout.BeginHorizontal();

            if (GUILayout.Button("Play"))
                _src.PlayForward(CurrentAnimState);

            GUILayout.EndHorizontal();
        }

        //serializedObject.Update();
        //serializedObject.ApplyModifiedProperties();

        GUILayout.EndVertical();



    }

    protected void SetAnimStateEditor(UViewAnimState _state,int _nIndex,string _strLabel)
    {
        m_nStateData[_nIndex].IsStateExpend = EditorGUILayout.Foldout(m_nStateData[_nIndex].IsStateExpend, _strLabel);
        if (m_nStateData[_nIndex].IsStateExpend)
        {
            CloseOther(_nIndex);
            if (m_nStateData[_nIndex].IsElememtExpend == null)
            {
                m_nStateData[_nIndex].IsElememtExpend = new List<bool>();
            }
            if(m_nStateData[_nIndex].IsElememtExpend.Count != _state.ElementList.Count)
            {
                for (int i = 0; i < _state.ElementList.Count; ++i)
                {
                    m_nStateData[_nIndex].IsElememtExpend.Add(false);
                }
            }
                

            m_nStateData[_nIndex].StateScrollPos = GUILayout.BeginScrollView(m_nStateData[_nIndex].StateScrollPos);
            GUILayout.BeginVertical("box");
            _state.DelayValue = EditorGUILayout.FloatField("延时启动时间:", _state.DelayValue);
            if (GUILayout.Button("添加动作"))
            {
                _state.ElementList.Add(new UViewAnimElement());
                m_nStateData[_nIndex].IsElememtExpend.Add(false);
            }

            GUILayout.BeginVertical("box");
            for (int i = _state.ElementList.Count - 1 ; i>=0 ; i--)
            {
                GUILayout.BeginHorizontal();

                m_nStateData[_nIndex].IsElememtExpend[i] = EditorGUILayout.Foldout(m_nStateData[_nIndex].IsElememtExpend[i],"元素"+i);
                if (GUILayout.Button("-",GUILayout.Width(100)))
                {
                    _state.ElementList.RemoveAt(i);
                    m_nStateData[_nIndex].IsElememtExpend.RemoveAt(i);
                    continue;
                }
                GUILayout.EndHorizontal();
                if(m_nStateData[_nIndex].IsElememtExpend[i])
                    SetAnimStateElementEditor(_state,_state.ElementList[i]);
            }
            GUILayout.EndVertical();

            GUILayout.EndVertical();
            GUILayout.EndScrollView();
        }

    }

    protected void SetAnimStateElementEditor(UViewAnimState _state,UViewAnimElement _element)
    {
        GUILayout.BeginVertical();
        {
                _element.TweenAttachType = (UViewAnimElement.ETweenAttachType)EditorGUILayout.EnumPopup("元素插入方式", _element.TweenAttachType);
                if (_element.TweenAttachType == UViewAnimElement.ETweenAttachType.ETA_Insert)
                    _element.TweenAttachType_Insert_Pos = EditorGUILayout.FloatField("插入时间点", _element.TweenAttachType_Insert_Pos);

                _element.TweenType = (UViewAnimElement.ETweenType)EditorGUILayout.EnumPopup("元素动画类型", _element.TweenType);
                _element.EaseValue = (DG.Tweening.Ease)EditorGUILayout.EnumPopup("缓动方式", _element.EaseValue);
                _element.DurationValue = EditorGUILayout.FloatField("持续时间", _element.DurationValue);
                _element.DelayValue = EditorGUILayout.FloatField("延迟时间", _element.DelayValue);

                SetAnimStateElementTweenEditor(_state,_element);
        }
        GUILayout.EndVertical();

    }

    protected void SetAnimStateElementTweenEditor(UViewAnimState _state,UViewAnimElement _element)
    {
        GUILayout.BeginVertical();

        GUILayout.BeginHorizontal();
        {
            _element.IsCustomTarget = EditorGUILayout.Toggle("指定对象？",_element.IsCustomTarget);
            if (_element.IsCustomTarget)
                _element.CustomTarget = (RectTransform)EditorGUILayout.ObjectField( _element.CustomTarget, typeof(RectTransform), true);
        }
        GUILayout.EndHorizontal();

        switch (_element.TweenType)
        {
            case UViewAnimElement.ETweenType.ETT_Fade:
                {
                    _element.FloatTypeValue.BeginValue = EditorGUILayout.FloatField("起始值",_element.FloatTypeValue.BeginValue);
                    _element.FloatTypeValue.EndValue = EditorGUILayout.FloatField("结束值", _element.FloatTypeValue.EndValue);
                }
                break;
            case UViewAnimElement.ETweenType.ETT_AnchorPosition:
                {
                    _element.Vector2TypeValue.BeginValue = EditorGUILayout.Vector2Field("起始值", _element.Vector2TypeValue.BeginValue);
                    _element.Vector2TypeValue.EndValue = EditorGUILayout.Vector2Field("结束值", _element.Vector2TypeValue.EndValue);

                }
                break;
            case UViewAnimElement.ETweenType.ETT_LocalRotate:
                {
                    _element.Vector3TypeValue.BeginValue = EditorGUILayout.Vector3Field("起始值", _element.Vector3TypeValue.BeginValue);
                    _element.Vector3TypeValue.EndValue = EditorGUILayout.Vector3Field("结束值", _element.Vector3TypeValue.EndValue);

                }
                break;
            case UViewAnimElement.ETweenType.ETT_LocalScale:
                {
                    _element.Vector3TypeValue.BeginValue = EditorGUILayout.Vector3Field("起始值", _element.Vector3TypeValue.BeginValue);
                    _element.Vector3TypeValue.EndValue = EditorGUILayout.Vector3Field("结束值", _element.Vector3TypeValue.EndValue);

                }
                break;
            case UViewAnimElement.ETweenType.ETT_PunchPosition:
                {
                    _element.Vector2TypeValue.BeginValue = EditorGUILayout.Vector2Field("起始值", _element.Vector2TypeValue.BeginValue);
                    _element.Vector2TypeValue.EndValue = EditorGUILayout.Vector2Field("结束值", _element.Vector2TypeValue.EndValue);


                    _element.VibratoValue = EditorGUILayout.IntField("冲撞频率", _element.VibratoValue);
                    _element.PunchPositionElasticity = EditorGUILayout.FloatField("伸缩系数", _element.PunchPositionElasticity); 

                }
                break;
            case UViewAnimElement.ETweenType.ETT_ShakePosition:
                {
                    _element.Vector3TypeValue.BeginValue = EditorGUILayout.Vector3Field("起始值", _element.Vector3TypeValue.BeginValue);
                    _element.Vector3TypeValue.EndValue = EditorGUILayout.Vector3Field("结束值", _element.Vector3TypeValue.EndValue);
                    _element.ShakeStrength = EditorGUILayout.FloatField("震动强度", _element.ShakeStrength);
                    _element.VibratoValue = EditorGUILayout.IntField("震动频率", _element.VibratoValue);
                    _element.ShakeRandomness = EditorGUILayout.FloatField("震动方向随机值", _element.ShakeRandomness);
                }
                break;
            case UViewAnimElement.ETweenType.ETT_LocalPosition:
                {
                    _element.Vector3TypeValue.BeginValue = EditorGUILayout.Vector3Field("起始值", _element.Vector3TypeValue.BeginValue);
                    _element.Vector3TypeValue.EndValue = EditorGUILayout.Vector3Field("结束值", _element.Vector3TypeValue.EndValue);

                }
                break;
            case UViewAnimElement.ETweenType.ETT_Custom:
                {
                    _element.CustomCollection = (UViewCustomAnimCollection )EditorGUILayout.ObjectField("收集器", _element.CustomCollection,typeof(UViewCustomAnimCollection ),true);
                    _element.CustomID = EditorGUILayout.IntField("ID", _element.CustomID);
                }
                break;
            default:
                Debug.LogWarning("这里会报错？那就出现重大大问题了。");
                break;
        }

        GUILayout.EndVertical();
    }

    protected void CloseOther(int _nIndex)
    {
        for (int i = 0; i < m_nStateData.Length; ++i)
        {
            if (i != _nIndex)
            {
                m_nStateData[i].IsStateExpend = false;
            }
        }
    }
}

