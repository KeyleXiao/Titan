/*******************************************************************
** 文件名:	SceneResInfoManager_QuadTree.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/7/20
** 版  本:	1.0
** 描  述:	场景资源信息 管理器,四叉树部分
** 应  用:   场景资源信息，用于做场景资源管理

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

public partial class SceneResInfoManager : MonoBehaviour
{
    public class QuadTreeNode
    {
        public List<QuadTreeLeaves> m_LeavesList = new List<QuadTreeLeaves>();
        public Rect Bounds;

        //子节点数目
        public int ChildCount = 0;

        public QuadTreeNode m_Parent = null;

        //四个子节点
        public QuadTreeNode[] ChildNode = new QuadTreeNode[4];

        /// <summary>
        /// 这个树节点是否被优化了
        /// </summary>
        public bool bOpmitizedRendering = true;
        
        /// <summary>
        /// 这个树节点是否在执行淡入操作
        /// </summary>
        public bool isDistanceFadingIn = false;

        /// <summary>
        /// 这个树节点是否在执行淡出操作
        /// </summary>
        public bool isDistanceFadingOut = false;


        public List<Material> FadeMaterialList = null;
    }

    public class QuadTreeLeaves
    {
        public QuadTreeNode m_Parent = null;
        public SceneResInfo m_ResInfo = null;
        public SceneLightInfo m_LightInfo = null;
        public SceneColliderInfo m_ColliderInfo = null;
        public SceneEffectInfo m_EffectInfo = null;
    }

    /// <summary>
    /// 计算活动结点的临时变量
    /// </summary>
    private HashSet<QuadTreeNode> tempGenerateQuadTree = new HashSet<QuadTreeNode>();

    #region Core

    /// <summary>
    /// 构建四叉树
    /// </summary>
    /// <param name="rootNode"></param>
    /// <param name="CellWidth">最小单位的size</param>
    /// <param name="CellHeight">最小单位的size</param>
    void BuildQuadTree(QuadTreeNode rootNode,float CellWidth,float CellHeight)
    {
        float subWidth = rootNode.Bounds.width  * 0.5f ;
        float subHeight = rootNode.Bounds.height * 0.5f ;

        //是否已经不能再分了
        bool isCanSplit = subWidth >= CellWidth && subHeight >= CellHeight;

        //if (CurrentTreeLevel > CurrentMaxLevel)
        //{
        //    isCanSplit = false;
        //}
        if(!isCanSplit)
        {
            //不可分割，作为最后的结点
            rootNode.ChildCount = 0;
            return;
        }
        else
        {
            //左上
            QuadTreeNode LeftTopNode = new QuadTreeNode();
            LeftTopNode.Bounds = new Rect(rootNode.Bounds.xMin, rootNode.Bounds.yMin, subWidth, subHeight);
            LeftTopNode.m_Parent = rootNode;

            //右上
            QuadTreeNode RightTopNode = new QuadTreeNode();
            RightTopNode.Bounds = new Rect(rootNode.Bounds.xMin + subWidth, rootNode.Bounds.yMin, subWidth, subHeight);
            RightTopNode.m_Parent = rootNode;

            //左下
            QuadTreeNode LeftDownpNode = new QuadTreeNode();
            LeftDownpNode.Bounds = new Rect(rootNode.Bounds.xMin, rootNode.Bounds.yMin + subHeight, subWidth, subHeight);
            LeftDownpNode.m_Parent = rootNode;

            //右下
            QuadTreeNode RightDownpNode = new QuadTreeNode();
            RightDownpNode.Bounds = new Rect(rootNode.Bounds.xMin + subWidth, rootNode.Bounds.yMin + subHeight, subWidth, subHeight);
            RightDownpNode.m_Parent = rootNode;

            rootNode.ChildNode[0] = LeftTopNode;
            rootNode.ChildNode[1] = RightTopNode;
            rootNode.ChildNode[2] = LeftDownpNode;
            rootNode.ChildNode[3] = RightDownpNode;
            rootNode.ChildCount = 4;



            //递归
            foreach(QuadTreeNode p in rootNode.ChildNode)
            {
                BuildQuadTree(p, CellWidth, CellHeight);
            }
        }

    }

    /// <summary>
    /// 根据位置取得对应的结点
    /// </summary>
    /// <param name="rootNode"></param>
    /// <param name="pos"></param>
    /// <returns></returns>
    QuadTreeNode GetQuadTreeParent(QuadTreeNode rootNode, Vector2 pos)
    {
        QuadTreeNode p = null;
        GetQuadTreeParentInteral(rootNode, pos, ref p);
        return p;
    }


    /// <summary>
    /// 取节点内部实现
    /// </summary>
    /// <param name="rootNode"></param>
    /// <param name="pos"></param>
    /// <param name="result"></param>
    void GetQuadTreeParentInteral(QuadTreeNode rootNode, Vector2 pos, ref QuadTreeNode result)
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
            foreach (QuadTreeNode p in rootNode.ChildNode)
            {
                GetQuadTreeParentInteral(p, pos, ref result);
            }

        }

        //没有找到结果，返回null
        return;
    }


        
    /// <summary>
    /// 取得活动的节点，取出上下左右8个活动的区域
    /// </summary>
    /// <param name="rootNode">四叉树跟结点</param>
    /// <param name="cameraFov">摄像机fov</param>
    /// <param name="pos2D">位置</param>
    /// <param name="cameraDir2D">摄像机方向</param>
    /// <param name="cellSize">格子大小</param>
    /// <param name="result">活动的叶子节点</param>
    /// <param name="ActiveParent">活动的父节点</param>
    /// <param name="unUsedLeaves">相比上一次检测，从活动变成未活动的结点</param>
    void GenerateActiveQuadParent(QuadTreeNode rootNode,float cameraFov, Vector2 pos2D,Vector2 cameraDir2D, int cellSize,
                                  ref List<QuadTreeLeaves> result, ref List<QuadTreeNode> ActiveParent,
                                  ref List<QuadTreeLeaves> unUsedLeaves,ref List<QuadTreeNode> unUsedParent)
    {
        if(null == result || null == rootNode)
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
        QuadTreeNode self = GetQuadTreeParent(rootNode, pos2D);
        if (null != self)
        {
            result.AddRange(self.m_LeavesList);
            ActiveParent.Add(self);
            if(tempGenerateQuadTree.Contains(self))
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
        QuadTreeNode LeftUpNode = GetQuadTreeParent(rootNode, leftUp);
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
        QuadTreeNode LeftDownNode = GetQuadTreeParent(rootNode, leftDown);
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
        QuadTreeNode LeftMidleNode = GetQuadTreeParent(rootNode, LeftMidle);
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
        QuadTreeNode RightUpNode = GetQuadTreeParent(rootNode, RightUp);
        if (null != RightUpNode)
        {
            Vector2 p2Dmin = RightUpNode.Bounds.min;
            Vector2 p2Dmax = RightUpNode.Bounds.max;
            p2Dmin = (p2Dmin - pos2D).normalized;
            p2Dmax = (p2Dmax - pos2D).normalized;

            //在背面的，就不鸟了
            if ( !isBackForPos(p2Dmin, cameraDir2D, cameraFov) || !isBackForPos(p2Dmax, cameraDir2D, cameraFov))
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
        QuadTreeNode RightMidleNode = GetQuadTreeParent(rootNode, RightMidle);
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
        QuadTreeNode RightDownNode = GetQuadTreeParent(rootNode, RightDown);
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
        QuadTreeNode UpMidleNode = GetQuadTreeParent(rootNode, UpMidle);
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
        QuadTreeNode DownMidleNode = GetQuadTreeParent(rootNode, DownMidle);
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

        if(tempGenerateQuadTree.Count > 0)
        {
            unUsedParent.AddRange(tempGenerateQuadTree);
            foreach (QuadTreeNode n in unUsedParent)
            {
                unUsedLeaves.AddRange(n.m_LeavesList);
            }
        }
    }

    void ClearQuadTree(QuadTreeNode rootNode)
    {
        if(null == rootNode)
        {
            return;
        }
        //已经到子节点了
        if (rootNode.ChildCount <= 0)
        {
            foreach(QuadTreeLeaves leaf in rootNode.m_LeavesList)
            {
                leaf.m_LightInfo = null;
                leaf.m_ResInfo = null;
            }
            rootNode.m_LeavesList.Clear();
        }

        //继续遍历剩下的结点
        foreach (QuadTreeNode p in rootNode.ChildNode)
        {
            ClearQuadTree(p);
        }

    }

    bool isBackForPos(Vector2 dir1, Vector2 dir2, float cameraFov)
    {
        //判断FOV不准确，直接使用小于0吧
        ////稍微调大一些角度
        //float rad = Mathf.PI / 180.0f * (cameraFov + 30);
        //float cameraViewHalfAngle = Mathf.Cos(rad);
        
        //float angle = Vector2.Dot(dir1, dir2);

        ////在摄像机的fov外面
        //return angle < 0;


        float angle = Vector2.Dot(dir1, dir2);
        //在摄像机的后面
        return angle < 0;
    }
    #endregion


}
