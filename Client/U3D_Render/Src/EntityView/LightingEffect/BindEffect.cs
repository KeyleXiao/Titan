using UnityEngine;
using System.Collections;

// 光效调用脚本

public delegate void BindLightingEventHandler(int effectID);

/// <summary>
/// 从A状态过度到B状态，然后再用代码回到A状态，其中A状态的clip是有事件，B状态是空clip。这样会无限的去触发A状态的clip事件回调。
/// Bug无法解决，先记下来
/// </summary>
public class BindEffect : MonoBehaviour
{
    public event BindLightingEventHandler bindLightingEvent;

    public void ClearEvent()
    {
        bindLightingEvent = null;
    }

    public void BindLighting(int effectID)
    {
        if(null != bindLightingEvent)
        {
            bindLightingEvent(effectID);
        }

    }

}
