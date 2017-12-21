/*******************************************************************
** 文件名:	UpdateBehaviourOnTimmerManager.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	梁成
** 日  期:	2016-2-2
** 版  本:	1.0
** 描  述:	控制MonoBehaviour的更新行为

 * 1、管理OnTimmer类型下的脚本轮流执行，到了指定时间后面的就不执行了
 * 2、为UpdateBehaviourOnTimer的实例分配index，并且记录UpdateBehaviourOnTimer实例自我定义的layer
 * 3、
********************************************************************/
using UnityEngine;
using System;
using System.Collections;
using System.Collections.Generic;


public class UpdateBehaviourOnTimerManager : MonoBehaviour
{
    public class UpdateBehaviourOnTimerLayer
    {
        public int layerIndex;
        public uint componentCount;
        public uint roundStartTick; //本轮更新的开始时间
    }

    public static UpdateBehaviourOnTimerManager Instance = null;

    private Dictionary<int,UpdateBehaviourOnTimerLayer> layers;

    public void Awake()
    {
        Instance = this;
        layers=new Dictionary<int,UpdateBehaviourOnTimerLayer>();     
    }

    private uint frameStartTick;
    public void Update()
    {
        frameStartTick = GameLogicAPI.getTickCount();
    }

    public uint getFrameStartTick()
    {
        return frameStartTick;
    }

    public uint applyIndexOnLayer(int layer)
    {
        uint index = 0;
        if (!layers.ContainsKey(layer))
        {
            layers[layer] = new UpdateBehaviourOnTimerLayer();
            layers[layer].layerIndex = layer;
            layers[layer].componentCount = 0;
        }
        index = layers[layer].componentCount;
        layers[layer].componentCount++;
        return index;
    }

    public void componentDestory(int layer, uint index)
    {
        if (layers[layer].componentCount == index + 1 && layers[layer].componentCount > 0)
        {
            layers[layer].componentCount--;
        }
    }

    public UpdateBehaviourOnTimerLayer getLayerInfo(int layer)
    {
        return layers[layer];
    }

    public void setRoundStartTick(int layer)
    {
        layers[layer].roundStartTick = GameLogicAPI.getTickCount();
    }
}