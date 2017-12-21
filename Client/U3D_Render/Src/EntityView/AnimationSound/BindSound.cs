using UnityEngine;
using System.Collections;

//音效调用脚本，用于绑定动作音效
public delegate Sound BindSoundEventHandler(GameObject go, int soundID, int entityID = -1, bool bAsyncLoad = true);
/// <summary>
/// 从A状态过度到B状态，然后再用代码回到A状态，其中A状态的clip是有事件，B状态是空clip。这样会无限的去触发A状态的clip事件回调。
/// Bug无法解决，先记下来
/// </summary>
public class BindSound : MonoBehaviour
{
    public event BindSoundEventHandler bindSoundEvent;

    public void ClearEvent()
    {
        bindSoundEvent = null;
    }

    public void BindAnimSound(int soundID)
    {
        if (bindSoundEvent != null)
        {
            bindSoundEvent(this.gameObject, soundID);
        }
    }

}
