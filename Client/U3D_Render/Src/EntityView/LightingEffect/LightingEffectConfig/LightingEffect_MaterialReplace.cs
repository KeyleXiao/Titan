//using UnityEngine;
//using System.Collections;
//using System.Collections.Generic;
//using System.Runtime.InteropServices;

//using GameLogic;

//[System.Serializable]

//public class LightingEffect_MaterialReplace : LightingEffect
//{
//    [Desc("开始时刻，单位毫秒")]
//    public int nStartTick;

//    [Desc("结束时刻，单位毫秒")]
//    public int nEndTick;

//    [Desc("替换材质索引")]
//    public int replaceMaterialIndex = 0;

//    [Desc("替换材质")]
//    public Material effectPrefb;
//    public string AssetGUID = string.Empty;

//    [Desc("优先级（数字越大优先度越高")]
//    public int replacePriority = 10;

//    public override LightingEffectConfigType GetEffectType()
//    {
//        return LightingEffectConfigType.MatReplace;
//    }

//    public override string GetAssetGUID()
//    {
//        return AssetGUID;
//    }

//}
