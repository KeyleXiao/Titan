using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.RegularExpressions;
using System;

using GameLogic;
using U3D_Render;
using DataCenter;


public struct EntityPosAngleInfo
{
    public int id;
    public Vector3 pos;
    public Vector3 dir;
}

/// <summary>
/// 实体通信类，沟通实体c++和显示层
/// </summary>
public class EntityBatchCommunicator
{
    private static Dictionary<int, EntityPosAngleInfo> infos = new Dictionary<int, EntityPosAngleInfo>();
    public static Dictionary<int, EntityPosAngleInfo> EntityInfo { get { return infos; } }

    public static void SendEntityPro(int id, Vector3 pos, Vector3 dir)
    {
        EntityPosAngleInfo info;
        info.id = id;
        info.pos = pos;
        info.dir = dir;
        infos[id] = info;
    }

    public static void Update()
    {
        SendDataToMinimap();
        infos.Clear();
        return;
    }

    private static void SendDataToLogic()
    {

    }

    private static void SendDataToMinimap()
    {
        LogicDataCenter.warMinimapDataManager.BatchUpdateEntityPos();
    }
}
