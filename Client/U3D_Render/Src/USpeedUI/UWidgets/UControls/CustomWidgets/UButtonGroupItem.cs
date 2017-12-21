using UnityEngine;
using System.Collections;
using UnityEngine.EventSystems;
using System;
using UnityEngine.UI;
using UnityEngine.Events;

public class UButtonGroupItem : Selectable, IPointerClickHandler,IComparable
{
    [SerializeField]
    protected bool m_Selected;
    /// <summary>
    /// 设置Item是否选择
    /// </summary>
    public bool Selected
    {
        get { return m_Selected; }
        set
        {
            if (m_Selected == value)
                return;
            Set(value, true);
        }
    }

    [SerializeField]
    protected bool m_Toggle = true;
    /// <summary>
    /// Item是否可以选择了之后，不自动变暗
    /// </summary>
    public bool Toggle
    {
        get { return m_Toggle; }
        set
        {
            if (m_Toggle == value)
                return;
            m_Toggle = value;
        }
    }

    [SerializeField]
    protected UButtonGroup m_Group;
    /// <summary>
    /// Item所在的控制组
    /// </summary>
    public UButtonGroup Group
    {
        get { return m_Group; }
        set
        {
            m_Group = value;
            SetButtonGroup(m_Group, true);
        }
    }

    /// <summary>
    /// Item在Group里的序号
    /// </summary>
    [SerializeField]
    public int GroupIndex;

    [Serializable]
    public class ButtonItemSelectedEvent : UnityEvent<bool>
    { }
    [Serializable]
    public class ButtonItemSelectedEventEx : UnityEvent<UButtonGroupItem,bool>
    { }

    /// <summary>
    /// 与onSelectedChanged 事件一致，只有选择状态
    /// </summary>
    public ButtonItemSelectedEvent onSelectedChanged = new ButtonItemSelectedEvent();
    /// <summary>
    /// 与onSelectedChanged 事件一致，只是返回多了一个自身引用
    /// </summary>
    public ButtonItemSelectedEventEx onSelectedChangedEx = new ButtonItemSelectedEventEx();
    /// <summary>
    /// 右键响应事件
    /// </summary>
    public ButtonItemSelectedEventEx onRightClick = new ButtonItemSelectedEventEx();
    /// <summary>
    /// 左键双击响应事件
    /// </summary>
    public ButtonItemSelectedEventEx onDoubleClick = new ButtonItemSelectedEventEx();

    protected override void OnEnable()
    {
        base.OnEnable();
        SetButtonGroup(m_Group, false);

    }

	protected override void OnDisable()
    {
        SetButtonGroup(null, false);
        base.OnDisable();
    }

    public void Clear()
    {
        SetButtonGroup(null, true);
        Set(false, false);
        onSelectedChanged.RemoveAllListeners();
        onSelectedChangedEx.RemoveAllListeners();
        onRightClick.RemoveAllListeners();
        onDoubleClick.RemoveAllListeners();
    }

    /// <summary>
    //  这里只是为了更新一下动画，没其他意思
    /// </summary>
    protected void RefreshState()
    {
        DoStateTransition(SelectionState.Normal, false);

    }

    protected override void DoStateTransition(SelectionState state, bool instant)
    {
        if(Toggle && Selected)
        {
            base.DoStateTransition(SelectionState.Pressed, instant);
            return;
        }
        else if(Toggle && !Selected)
        {
            base.DoStateTransition(SelectionState.Normal, instant);
            return;
        }
            

        base.DoStateTransition(state, instant);
    }

    private void InternalToggle()
    {
        if (!IsActive() || !IsInteractable())
            return;

        if (Group != null && !Group.AllowSwitchSameItem && !Selected)
            Selected = !Selected;
        else if ((Group != null && Group.AllowSwitchSameItem) || Group == null)
            Selected = !Selected;
    }

    public virtual void SetButtonGroup(UButtonGroup newGroup, bool setMemberValue)
    {
        UButtonGroup oldGroup = m_Group;

        if (m_Group != null)
            m_Group.UnregisterButton(this);

        if (setMemberValue)
            m_Group = newGroup;

        if (m_Group != null && IsActive())
            m_Group.RegisterButton(this);

        if (newGroup != null && newGroup != oldGroup && Selected)// && IsActive())
            m_Group.NotifyItemOn(this);
    }

    private void Set(bool value, bool sendCallback)
    {
        if (m_Selected == value)
            return;

        m_Selected = value;
        if (m_Group != null && Toggle)
        {
            if (m_Selected|| !m_Group.AnyItemSelected())
            {
                //一旦组里有Item选择了，组里必须要有一个处于选中状态
                m_Selected = true;
                m_Group.NotifyItemOn(this);
            }
            
        }
        RefreshState();
        if (sendCallback)
        {
            onSelectedChanged.Invoke(Selected);
            onSelectedChangedEx.Invoke(this, Selected);
        }
    }

    public virtual void OnPointerClick(PointerEventData eventData)
    {
        if (eventData.button == PointerEventData.InputButton.Left && eventData.clickCount == 1)
        {
            InternalToggle();
        }
        if (eventData.button == PointerEventData.InputButton.Left && eventData.clickCount == 2)
        {
            onDoubleClick.Invoke(this,Selected);
        }
        if (eventData.button == PointerEventData.InputButton.Right && eventData.clickCount == 1)
        {
            onRightClick.Invoke(this, Selected);
        }
    }

    public int CompareTo(object obj)
    {
        int res = 0;
        try
        {
            UButtonGroupItem sObj = (UButtonGroupItem)obj;
            if (this.GroupIndex > sObj.GroupIndex)
            {
                res = 1;
            }
            else if (this.GroupIndex < sObj.GroupIndex)
            {
                res = -1;
            }
        }
        catch (Exception ex)
        {
            throw new Exception("比较异常", ex.InnerException);
        }
        return res;
    }
}
