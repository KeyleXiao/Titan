using UnityEngine;
using System.Collections;
using UnityEngine.EventSystems;
using System.Collections.Generic;
using System;
using UnityEngine.Events;

public class UButtonGroup : UIBehaviour
{
    [SerializeField]
    protected bool m_AllowSwitchSameItem = false;
    /// <summary>
    /// 如果在Item在组里，在已经选择的状态下是否可以再次选择
    /// </summary>
    public bool AllowSwitchSameItem
    {
        get { return m_AllowSwitchSameItem; }
        set
        {
            if (m_AllowSwitchSameItem == value)
                return;
            m_AllowSwitchSameItem = value;
        }
    }
    [SerializeField]
    protected bool m_bSkipInactive = true;
    public bool SkipInactive
    {
        get { return m_bSkipInactive; }
        set
        {
            if (m_bSkipInactive == value)
                return;
            m_bSkipInactive = value;
        }
    }

    [SerializeField]
    protected bool m_GroupMustHadSelected = false;
    /// <summary>
    /// 组里必须有一个Item处于选中状态
    /// </summary>
    public bool GroupMustHadSelected
    {
        get { return m_GroupMustHadSelected; }
        set { m_GroupMustHadSelected = value; }
    }

    [SerializeField]
    protected bool m_bUseGroupIndex;
    /// <summary>
    /// 是否用GroupItem的序号进行排序
    /// </summary>
    public bool UseGroupIndex
    {
        get { return m_bUseGroupIndex; }
        set { m_bUseGroupIndex = value; }
    }

    private List<UButtonGroupItem> m_ButtonItems = new List<UButtonGroupItem>();

    /// <summary>
    /// 当前选择的Item
    /// </summary>
    [NonSerialized]
    public UButtonGroupItem SelectedItem = null;

    /// <summary>
    /// 第一个是旧的选中对象，第二个是新的选择对象
    /// </summary>
    [Serializable]
    public class ButtonGroupSelectedChangeEvent : UnityEvent<UButtonGroupItem, UButtonGroupItem>
    { }
    public ButtonGroupSelectedChangeEvent onButtonGroupSelectedChange = new ButtonGroupSelectedChangeEvent();

    /// <summary>
    /// 通知其余的Item取消选择
    /// </summary>
    /// <param name="item">当前选择的Item</param>
    public void NotifyItemOn(UButtonGroupItem item)
    {
        if (!ValidateItemIsInGroup(item))
            return;

        for (var i = 0; i < m_ButtonItems.Count; i++)
        {
            if (m_ButtonItems[i] == item)
            {
                onButtonGroupSelectedChange.Invoke(SelectedItem, item);
                SelectedItem = item;
                continue;
            }
           
            m_ButtonItems[i].Selected = false;
        }
    }

    /// <summary>
    /// 从组里移除指定的Item
    /// </summary>
    /// <param name="item"></param>
    public void UnregisterButton(UButtonGroupItem item)
    {
        if (SkipInactive && m_ButtonItems.Contains(item))
            m_ButtonItems.Remove(item);
    }

    /// <summary>
    /// 移除指定Index的Item
    /// </summary>
    /// <param name="_nIndex"></param>
    public void UnregisterButton(int _nIndex)
    {
        if(_nIndex < 0 || _nIndex >= m_ButtonItems.Count)
        {
            Trace.LogWarning(this.name + " ButtonGroup UnregisterButton failed.Index out of range...");
            return;
        }
        m_ButtonItems.Remove(GetItemAt(_nIndex));
    }

    /// <summary>
    /// 移除组里所有的Item
    /// </summary>
    public void UnregisterAllButton()
    {
        m_ButtonItems.Clear();
    }

    /// <summary>
    /// 添加Item到组里
    /// </summary>
    /// <param name="item"></param>
    public void RegisterButton(UButtonGroupItem item)
    {
        if (!m_ButtonItems.Contains(item))
        {
            m_ButtonItems.Add(item);

            if(UseGroupIndex)
                m_ButtonItems.Sort();
        }        
    }



    /// <summary>
    /// 检测是否指定的Item是否在当前组里
    /// </summary>
    /// <param name="item"></param>
    private bool ValidateItemIsInGroup(UButtonGroupItem item)
    {
        if (item == null || !m_ButtonItems.Contains(item))
            return false;//throw new ArgumentException(string.Format("UButtonGroupItem {0} is not part of UButtonGroup {1}", new object[] { item, this }));
        return true;
    }

    /// <summary>
    /// 检测组里是否有Item选择了
    /// </summary>
    /// <returns></returns>
    public bool AnyItemSelected()
    {
        if (GroupMustHadSelected)
            return m_ButtonItems.Find(x => x.Selected) != null;
        else
            return true;
    }

    /// <summary>
    /// 取消组里所有Item的选择
    /// </summary>
    public void SetAllItemDeSelected()
    {

        for (var i = 0; i < m_ButtonItems.Count; i++)
            m_ButtonItems[i].Selected = false;
        
    }

    /// <summary>
    /// 根据Index设置组里的Item选择状态
    /// </summary>
    /// <para>如果序号超出组长度，组内Item全部设为非选中状态</para>
    /// <param name="_nIndex">Item在组里的序号</param>
    /// <param name="_bSelect">指定的Item选择状态</param>
    /// <returns>true:设置成功，false设置失败</returns>
    public bool SetItemSelectedItem(int _nIndex,bool _bSelect = true)
    {
        if (_nIndex < 0 || _nIndex >= m_ButtonItems.Count)
        {
            m_ButtonItems.ForEach((x) => SetSelectedItem(x, false));
            Trace.Log(transform.name+" 按钮组设置的Index超出范围了");
            return false;
        }       
        else
            return SetSelectedItem(GetItemAt(_nIndex), _bSelect);
    }

    /// <summary>
    /// 设置组内指定Item的选择状态,非组内的不处理
    /// </summary>
    /// <param name="_item">指定Item</param>
    /// <param name="_bSelect">新的选择状态</param>
    /// <returns>true:设置成功，false设置失败</returns>
    public bool SetSelectedItem(UButtonGroupItem _item,bool _bSelect)
    {
        if(m_ButtonItems.Contains(_item))
        {
            _item.Selected = _bSelect;
            return true;
        }
        return false;
    }

    /// <summary>
    /// 获取组内指定序号的Item
    /// </summary>
    /// <param name="_nIndex">指定序号</param>
    /// <returns>指定的Item</returns>
    public UButtonGroupItem GetItemAt(int _nIndex)
    {
        if (_nIndex < 0 || _nIndex >= m_ButtonItems.Count)
            return null;
        return m_ButtonItems[_nIndex];
    }

    /// <summary>
    /// 当前组内选择的Item序号
    /// </summary>
    /// <returns></returns>
    public int SelectedIndex
    {
        get
        {
            return GetItemIndex(SelectedItem);
        }        
    }

    /// <summary>
    /// 该组的长度
    /// </summary>
    public int Length()
    {
        return m_ButtonItems.Count;
    }

    /// <summary>
    /// 根据Item获取在组里的序号
    /// </summary>
    /// <param name="_nItem"></param>
    /// <returns></returns>
    public int GetItemIndex(UButtonGroupItem _nItem)
    {
        if (!ValidateItemIsInGroup(_nItem))
            return -1;
        return m_ButtonItems.IndexOf(_nItem);
    }
}
