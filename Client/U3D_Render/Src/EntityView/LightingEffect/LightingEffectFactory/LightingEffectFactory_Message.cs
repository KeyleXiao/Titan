using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using Effect;


public partial class LightingEffectFactory : MonoBehaviour
{
    /// <summary>
    /// 接受需要缓存的光效信息
    /// </summary>
    /// <param name="infoList"></param>
    public void RecvPreLoadLinghtEffectInfo(IEnumerable<Pre_Load_Light_Info> infoList)
    {
        foreach(Pre_Load_Light_Info info in infoList)
        {
            PushPreWarmEffect(info);
        }
    }
}
