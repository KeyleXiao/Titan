using UnityEngine;
using System.Collections;


/// <summary>
/// Behavior删除脚本
/// </summary>
public class EffectBehaviorDestroyTimer : MonoBehaviour
{
    [System.NonSerialized]
    public uint destroyTick = 0;
    [System.NonSerialized]
    public LightingEffectBehavior hostEffect = null;



    //public void Terminal()
    //{
    //    if (null != hostEffect)
    //    {
    //        hostEffect.Close();
    //        LightingEffectFactory.Instance.CacheEffectBehavior(hostEffect);
    //        hostEffect = null;
    //    }
    //    LightingEffectFactory.Instance.CacheBehaviorDestroyTimer(this);
    //}


    //public void Update()
    //{
    //    uint currentTick = GameLogicAPI.getTickCount();
    //    if (destroyTick > 0 && destroyTick<currentTick)
    //    {
    //        Terminal();
    //    }

    //}

}
