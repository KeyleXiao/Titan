/*******************************************************************
** 文件名:	SkinManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/4/3
** 版  本:	1.0
** 描  述:	皮肤管理器
** 应  用:   用于管理皮肤的，简单的说，就是模型了
**************************** 修改记录 ******************************
** 修改人:  昔文博
** 日  期:  2016/1/19
** 描  述:  增加缓存皮肤函数
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;


/// <summary>
/// 用于管理皮肤的，简单的说，就是模型了
/// </summary>
public partial class SkinManager 
{

    public class SkinQuadTreeNode
    {
        public Rect Bounds = default(Rect);

        public int ChildCount = 0;

        public SkinQuadTreeNode m_Parent = null;

        public SkinQuadTreeNode[] ChildNode = new SkinQuadTreeNode[4] { null, null, null, null };

        public List<U3D_Render.SkinCtrl.EntitySkinControl> m_LeavesList = new List<U3D_Render.SkinCtrl.EntitySkinControl>(10);

        public bool IsPointOnParent(Vector3 point)
        {
            Vector2 pos2D = Vector2.zero;
            pos2D.x = point.x;
            pos2D.y = point.z;

            return Bounds.Contains(pos2D);
        }

        public void RemoveLeaf(U3D_Render.SkinCtrl.EntitySkinControl instance)
        {
            if(!instance)
            {
                return;
            }
            m_LeavesList.Remove(instance);
        }
    }

    //private class SkinQuadTreeLeaf
    //{
    //    public SkinQuadTreeNode parent = null;
    //    public U3D_Render.SkinCtrl.EntitySkinControl skinControl = null;
    //}

    private static HashSet<SkinQuadTreeNode> tempGenerateQuadTree = new HashSet<SkinQuadTreeNode>();

    static void BuildQuadTree(SkinQuadTreeNode rootNode, float CellWidth, float CellHeight)
    {
        float subWidth = rootNode.Bounds.width * 0.5f;
        float subHeight = rootNode.Bounds.height * 0.5f;

        //是否已经不能再分了
        bool isCanSplit = subWidth >= CellWidth && subHeight >= CellHeight;

        //if (CurrentTreeLevel > CurrentMaxLevel)
        //{
        //    isCanSplit = false;
        //}
        if (!isCanSplit)
        {
            //不可分割，作为最后的结点
            rootNode.ChildCount = 0;
            return;
        }
        else
        {
            //左上
            SkinQuadTreeNode LeftTopNode = new SkinQuadTreeNode();
            LeftTopNode.Bounds = new Rect(rootNode.Bounds.xMin, rootNode.Bounds.yMin, subWidth, subHeight);
            LeftTopNode.m_Parent = rootNode;

            //右上
            SkinQuadTreeNode RightTopNode = new SkinQuadTreeNode();
            RightTopNode.Bounds = new Rect(rootNode.Bounds.xMin + subWidth, rootNode.Bounds.yMin, subWidth, subHeight);
            RightTopNode.m_Parent = rootNode;

            //左下
            SkinQuadTreeNode LeftDownpNode = new SkinQuadTreeNode();
            LeftDownpNode.Bounds = new Rect(rootNode.Bounds.xMin, rootNode.Bounds.yMin + subHeight, subWidth, subHeight);
            LeftDownpNode.m_Parent = rootNode;

            //右下
            SkinQuadTreeNode RightDownpNode = new SkinQuadTreeNode();
            RightDownpNode.Bounds = new Rect(rootNode.Bounds.xMin + subWidth, rootNode.Bounds.yMin + subHeight, subWidth, subHeight);
            RightDownpNode.m_Parent = rootNode;

            rootNode.ChildNode[0] = LeftTopNode;
            rootNode.ChildNode[1] = RightTopNode;
            rootNode.ChildNode[2] = LeftDownpNode;
            rootNode.ChildNode[3] = RightDownpNode;
            rootNode.ChildCount = 4;



            //递归
            foreach (SkinQuadTreeNode p in rootNode.ChildNode)
            {
                BuildQuadTree(p, CellWidth, CellHeight);
            }
        }

    }

    private static void InsetToTree(SkinQuadTreeNode root, U3D_Render.SkinCtrl.EntitySkinControl instance)
    {
        if (!instance)
        {
            return;
        }

        if(!instance.isActiveAndEnabled)
        {
            return;
        }

        Vector2 pos2D = Vector2.zero;
        pos2D.x = instance.transform.position.x;
        pos2D.y = instance.transform.position.z;
        SkinQuadTreeNode p = GetQuadTreeParent(root, pos2D);
        if (null != p)
        {
            //SkinQuadTreeLeaf leaf = new SkinQuadTreeLeaf();
            //leaf.parent = p;
           // p.m_LeavesList.Add(leaf);
           // leaf.skinControl = instance;
            p.m_LeavesList.Add(instance);
            instance.hostQuadTreeNode = p;
        }
    }

    static SkinQuadTreeNode GetQuadTreeParent(SkinQuadTreeNode rootNode, Vector2 pos)
    {
        SkinQuadTreeNode p = null;
        GetQuadTreeParentInteral(rootNode, pos, ref p);
        return p;
    }

    static void GetQuadTreeParentInteral(SkinQuadTreeNode rootNode, Vector2 pos, ref SkinQuadTreeNode result)
    {
        //已经找到了
        if (result != null)
        {
            return;
        }

        //已经到子节点了
        if (rootNode.ChildCount <= 0)
        {
            if (rootNode.Bounds.Contains(pos))
            {
                result = rootNode;
                return;
            }
        }

        //在这个结点里
        if (rootNode.Bounds.Contains(pos))
        {
            //继续遍历剩下的结点
            foreach (SkinQuadTreeNode p in rootNode.ChildNode)
            {
                GetQuadTreeParentInteral(p, pos, ref result);
            }

        }

        //没有找到结果，返回null
        return;
    }


    static void GenerateActiveQuadParent(SkinQuadTreeNode rootNode, float cameraFov, Vector2 pos2D, Vector2 cameraDir2D, int cellSize,
                          ref List<U3D_Render.SkinCtrl.EntitySkinControl> result, ref List<SkinQuadTreeNode> ActiveParent,
                          ref List<U3D_Render.SkinCtrl.EntitySkinControl> unUsedLeaves, ref List<SkinQuadTreeNode> unUsedParent)
    {
        if (null == result || null == rootNode)
        {
            return;
        }
        tempGenerateQuadTree.Clear();
        tempGenerateQuadTree.UnionWith(ActiveParent);

        unUsedParent.Clear();
        unUsedLeaves.Clear();

        result.Clear();
        ActiveParent.Clear();
        //本身的这个区域
        SkinQuadTreeNode self = GetQuadTreeParent(rootNode, pos2D);
        if (null != self)
        {
            result.AddRange(self.m_LeavesList);
            ActiveParent.Add(self);
            if (tempGenerateQuadTree.Contains(self))
            {
                tempGenerateQuadTree.Remove(self);
            }
        }
        else
        {
            return;
        }

        float offset = cellSize * 0.1f;

        //剩下的8宫格位置，采用8+1=9宫格来管理
        Vector2 min = self.Bounds.min;
        Vector2 max = self.Bounds.max;

        //左上
        Vector2 leftUp = min;
        leftUp.y = max.y;
        leftUp.x -= offset;
        leftUp.y += offset;
        SkinQuadTreeNode LeftUpNode = GetQuadTreeParent(rootNode, leftUp);
        if (null != LeftUpNode)
        {
            Vector2 p2Dmin = LeftUpNode.Bounds.min;
            Vector2 p2Dmax = LeftUpNode.Bounds.max;
            p2Dmin = (p2Dmin - pos2D).normalized;
            p2Dmax = (p2Dmax - pos2D).normalized;
            //在背面的，就不鸟了
            if (!isBackForPos(p2Dmin, cameraDir2D, cameraFov) || !isBackForPos(p2Dmax, cameraDir2D, cameraFov))
            {
                result.AddRange(LeftUpNode.m_LeavesList);
                ActiveParent.Add(LeftUpNode);
                if (tempGenerateQuadTree.Contains(LeftUpNode))
                {
                    tempGenerateQuadTree.Remove(LeftUpNode);
                }
            }
        }

        //左下
        Vector2 leftDown = min;
        leftDown.x -= offset;
        leftDown.y -= offset;
        SkinQuadTreeNode LeftDownNode = GetQuadTreeParent(rootNode, leftDown);
        if (null != LeftDownNode)
        {
            Vector2 p2Dmin = LeftDownNode.Bounds.min;
            Vector2 p2Dmax = LeftDownNode.Bounds.max;
            p2Dmin = (p2Dmin - pos2D).normalized;
            p2Dmax = (p2Dmax - pos2D).normalized;
            //在背面的，就不鸟了
            if (!isBackForPos(p2Dmin, cameraDir2D, cameraFov) || !isBackForPos(p2Dmax, cameraDir2D, cameraFov))
            {
                result.AddRange(LeftDownNode.m_LeavesList);
                ActiveParent.Add(LeftDownNode);
                if (tempGenerateQuadTree.Contains(LeftDownNode))
                {
                    tempGenerateQuadTree.Remove(LeftDownNode);
                }
            }
        }

        //左中
        Vector2 LeftMidle = min;
        LeftMidle.y += offset;
        LeftMidle.x -= offset;
        SkinQuadTreeNode LeftMidleNode = GetQuadTreeParent(rootNode, LeftMidle);
        if (null != LeftMidleNode)
        {
            Vector2 p2Dmin = LeftMidleNode.Bounds.min;
            Vector2 p2Dmax = LeftMidleNode.Bounds.max;
            p2Dmin = (p2Dmin - pos2D).normalized;
            p2Dmax = (p2Dmax - pos2D).normalized;
            //在背面的，就不鸟了
            if (!isBackForPos(p2Dmin, cameraDir2D, cameraFov) || !isBackForPos(p2Dmax, cameraDir2D, cameraFov))
            {
                result.AddRange(LeftMidleNode.m_LeavesList);
                ActiveParent.Add(LeftMidleNode);
                if (tempGenerateQuadTree.Contains(LeftMidleNode))
                {
                    tempGenerateQuadTree.Remove(LeftMidleNode);
                }
            }

        }

        //右上
        Vector2 RightUp = max;
        RightUp.x += offset;
        RightUp.y += offset;
        SkinQuadTreeNode RightUpNode = GetQuadTreeParent(rootNode, RightUp);
        if (null != RightUpNode)
        {
            Vector2 p2Dmin = RightUpNode.Bounds.min;
            Vector2 p2Dmax = RightUpNode.Bounds.max;
            p2Dmin = (p2Dmin - pos2D).normalized;
            p2Dmax = (p2Dmax - pos2D).normalized;

            //在背面的，就不鸟了
            if (!isBackForPos(p2Dmin, cameraDir2D, cameraFov) || !isBackForPos(p2Dmax, cameraDir2D, cameraFov))
            {
                result.AddRange(RightUpNode.m_LeavesList);
                ActiveParent.Add(RightUpNode);
                if (tempGenerateQuadTree.Contains(RightUpNode))
                {
                    tempGenerateQuadTree.Remove(RightUpNode);
                }
            }


        }

        //右中
        Vector2 RightMidle = max;
        RightMidle.x += offset;
        RightMidle.y -= offset;
        SkinQuadTreeNode RightMidleNode = GetQuadTreeParent(rootNode, RightMidle);
        if (null != RightMidleNode)
        {
            Vector2 p2Dmin = RightMidleNode.Bounds.min;
            Vector2 p2Dmax = RightMidleNode.Bounds.max;
            p2Dmin = (p2Dmin - pos2D).normalized;
            p2Dmax = (p2Dmax - pos2D).normalized;
            //在背面的，就不鸟了
            if (!isBackForPos(p2Dmin, cameraDir2D, cameraFov) || !isBackForPos(p2Dmax, cameraDir2D, cameraFov))
            {
                result.AddRange(RightMidleNode.m_LeavesList);
                ActiveParent.Add(RightMidleNode);
                if (tempGenerateQuadTree.Contains(RightMidleNode))
                {
                    tempGenerateQuadTree.Remove(RightMidleNode);
                }
            }


        }

        //右下
        Vector2 RightDown = max;
        RightDown.x = max.x;
        RightDown.y = min.y;
        RightDown.x += offset;
        RightDown.y -= offset;
        SkinQuadTreeNode RightDownNode = GetQuadTreeParent(rootNode, RightDown);
        if (null != RightDownNode)
        {
            Vector2 p2Dmin = RightDownNode.Bounds.min;
            Vector2 p2Dmax = RightDownNode.Bounds.max;
            p2Dmin = (p2Dmin - pos2D).normalized;
            p2Dmax = (p2Dmax - pos2D).normalized;
            //在背面的，就不鸟了
            if (!isBackForPos(p2Dmin, cameraDir2D, cameraFov) || !isBackForPos(p2Dmax, cameraDir2D, cameraFov))
            {
                result.AddRange(RightDownNode.m_LeavesList);
                ActiveParent.Add(RightDownNode);
                if (tempGenerateQuadTree.Contains(RightDownNode))
                {
                    tempGenerateQuadTree.Remove(RightDownNode);
                }
            }


        }

        //上中
        Vector2 UpMidle = max;
        UpMidle.x -= offset;
        UpMidle.y += offset;
        SkinQuadTreeNode UpMidleNode = GetQuadTreeParent(rootNode, UpMidle);
        if (null != UpMidleNode)
        {
            Vector2 p2Dmin = UpMidleNode.Bounds.min;
            Vector2 p2Dmax = UpMidleNode.Bounds.max;
            p2Dmin = (p2Dmin - pos2D).normalized;
            p2Dmax = (p2Dmax - pos2D).normalized;
            //在背面的，就不鸟了
            if (!isBackForPos(p2Dmin, cameraDir2D, cameraFov) || !isBackForPos(p2Dmax, cameraDir2D, cameraFov))
            {
                result.AddRange(UpMidleNode.m_LeavesList);
                ActiveParent.Add(UpMidleNode);
                if (tempGenerateQuadTree.Contains(UpMidleNode))
                {
                    tempGenerateQuadTree.Remove(UpMidleNode);
                }
            }


        }

        //下中
        Vector2 DownMidle = min;
        DownMidle.x += offset;
        DownMidle.y -= offset;
        SkinQuadTreeNode DownMidleNode = GetQuadTreeParent(rootNode, DownMidle);
        if (null != DownMidleNode)
        {
            Vector2 p2Dmin = DownMidleNode.Bounds.min;
            Vector2 p2Dmax = DownMidleNode.Bounds.max;
            p2Dmin = (p2Dmin - pos2D).normalized;
            p2Dmax = (p2Dmax - pos2D).normalized;

            //在背面的，就不鸟了
            if (!isBackForPos(p2Dmin, cameraDir2D, cameraFov) || !isBackForPos(p2Dmax, cameraDir2D, cameraFov))
            {
                result.AddRange(DownMidleNode.m_LeavesList);
                ActiveParent.Add(DownMidleNode);
                if (tempGenerateQuadTree.Contains(DownMidleNode))
                {
                    tempGenerateQuadTree.Remove(DownMidleNode);
                }
            }
        }

        if (tempGenerateQuadTree.Count > 0)
        {
            unUsedParent.AddRange(tempGenerateQuadTree);
            foreach (SkinQuadTreeNode n in unUsedParent)
            {
                unUsedLeaves.AddRange(n.m_LeavesList);
            }
        }
    }

    static bool isBackForPos(Vector2 dir1, Vector2 dir2, float cameraFov)
    {
        return false;
        //判断FOV不准确，直接使用小于0吧
        ////稍微调大一些角度
        //float rad = Mathf.PI / 180.0f * (cameraFov + 30);
        //float cameraViewHalfAngle = Mathf.Cos(rad);

        //float angle = Vector2.Dot(dir1, dir2);

        ////在摄像机的fov外面
        //return angle < 0;


        //float angle = Vector2.Dot(dir1, dir2);
        ////在摄像机的后面
        //return angle < 0;
    }

    static void ClearQuadTree(SkinQuadTreeNode rootNode)
    {
        if (null == rootNode)
        {
            return;
        }
        //已经到子节点了
        if (rootNode.ChildCount <= 0)
        {
            //foreach (SkinQuadTreeLeaf leaf in rootNode.m_LeavesList)
            //{
            //    leaf.skinControl = null;
            //}
            rootNode.m_LeavesList.Clear();
        }

        //继续遍历剩下的结点
        foreach (SkinQuadTreeNode p in rootNode.ChildNode)
        {
            ClearQuadTree(p);
        }

    }
}


