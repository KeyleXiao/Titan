/*******************************************************************
** 文件名:	AssetBundleManager_Prefab.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2016/10/25
** 版  本:	1.0
** 描  述:	喇叭消息
** 应  用:  
 *****************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using DataCenter;
using UIWidgets;
using UnityEngine;
using DG.Tweening;

namespace USpeedUI.Loudspeaker
{
    public delegate void LoudspeakerCallback();

    public class LoudspeakerContentItemData// : IItemHeight
    {
        public LoudspeakerManager.SLoudspeakerInfo info;

        //public float Height { get; set; }
    }

    public class LoudspeakerContentItem : MonoBehaviour
    {
        private struct LineObjCache
        {
            public float width;
            public float height;
            public RectTransform trans;
        }

        // 遮蔽框
        public GameObject m_ViewPortObj;
        // 滚动动画时间
        public float m_RunTime = 18f;

        // 实际宽度
        private float m_fCurWidth = 0f;
        public float Width { get { return m_fCurWidth; } }

        // 是否正在滚动
        private bool m_bRunning = false;
        public bool IsRunning { get { return m_bRunning; } set { m_bRunning = value; } }

        // 隔挡层宽度
        private float m_fViewPortWidth;

        // 滚动结束回调
        private LoudspeakerCallback m_onComplete;

        // 原始位置
        private Vector2 m_vOrginAnchoredPos = Vector2.zero;

        // 滚动伪结束位置，当动画到达该位置时候，通知上层播放下一个喇叭
        private float m_fPreCompleteX = 0f;
        private bool m_bPreCompleteFlag = false;

        // 滚动伪结束回调
        private LoudspeakerCallback m_onPreComplete;

        public void Start()
        {
            // 保存初始位置
            m_vOrginAnchoredPos = gameObject.GetComponent<RectTransform>().anchoredPosition;

            // 获取隔挡框的宽度
            m_fViewPortWidth = m_ViewPortObj.GetComponent<RectTransform>().rect.width;
        }

        public void SetData(LoudspeakerContentItemData data)
        {
            float offsetX = 0f;
            float offsetY = 0f;

            List<LineObjCache> curLineObject = new List<LineObjCache>();
            if (data.info.objList != null)
            {
                foreach (var obj in data.info.objList)
                {
                    CreateSubItem(obj, curLineObject);
                }
            }
            AlignLineObject(ref offsetX, ref offsetY, curLineObject);

            m_fCurWidth = offsetX;
        }

        private void CreateSubItem(LoudspeakerManager.SLoudspeakerObjectInfo info, List<LineObjCache> curLineObject)
        {
            if (curLineObject == null)
                return;

            ResNode itemPrefab = LogicDataCenter.loudspeakerManager.GetChatItemPrefabByObjType(info.type);
            if (itemPrefab == null)
                return;

            GameObject itemObj = itemPrefab.InstanceMainRes();
            if (itemObj == null)
                return;

            itemObj.transform.SetParent(this.transform, false);
            ILoudspeakerItem subItem = itemObj.GetComponent<ILoudspeakerItem>();
            if (subItem == null)
                return;

            if (subItem.SetItemData(info) == false)
                return;

            AddLineObject(subItem, curLineObject);
        }

        private void AddLineObject(ILoudspeakerItem item, List<LineObjCache> curLineObject)
        {
            LineObjCache objCache;
            objCache.width = item.GetItemWidth();
            objCache.height = item.GetItemHeight();
            objCache.trans = item.GetTransform();
            objCache.trans.sizeDelta = new Vector2(objCache.width, objCache.height);
            curLineObject.Add(objCache);
        }

        private void AlignLineObject(ref float curOffsetX, ref float curOffsetY, List<LineObjCache> curLineObject)
        {
            float maxHeight = 0f;
            for (int i = 0; i < curLineObject.Count; ++i)
            {
                maxHeight = Mathf.Max(maxHeight, curLineObject[i].height);
            }

            curOffsetX = 0f;
            const float yMargin = 2f;
            curOffsetY -= (yMargin + maxHeight);
            for (int i = 0; i < curLineObject.Count; ++i)
            {
                curLineObject[i].trans.anchoredPosition = new Vector2(curOffsetX, curOffsetY);
                curOffsetX += curLineObject[i].width;
            }

            curLineObject.Clear();
        }

        public void removeAllChild()
        {
            // 删除现有子对象
            for (int i = this.transform.childCount - 1; i >= 0; --i)
            {
                var temp = this.transform.GetChild(i).gameObject;
                ResNode.DestroyRes(ref temp);
            }
        }

        public void Run()
        {
            // 计算文字滚动结束的坐标
            float m_fRunCompletePosX = m_vOrginAnchoredPos.x - (Width + m_fViewPortWidth);
            
            // 计算滚动伪结束坐标
            m_fPreCompleteX = m_fRunCompletePosX + (m_fViewPortWidth / 4);
            m_bPreCompleteFlag = false;

            // 滚动开始
            m_bRunning = true;

            // 设置动画
            Tweener tw = transform.GetComponent<RectTransform>().DOAnchorPosX(m_fRunCompletePosX, m_RunTime);
            tw.SetEase(Ease.Linear);
            tw.OnComplete(() => {
                // 滚动结束
                gameObject.GetComponent<RectTransform>().anchoredPosition = m_vOrginAnchoredPos;
                removeAllChild();

                m_bRunning = false;

                m_onComplete();
            });
            tw.OnUpdate(onRunUpdate);
        }

        /// <summary>
        /// 设置滚动结束回调
        /// </summary>
        /// <param name="action"></param>
        public void onRunComplete(LoudspeakerCallback action)
        {
            this.m_onComplete = action;
        }

        /// <summary>
        /// 设置滚动伪结束回调
        /// </summary>
        /// <param name="action"></param>
        public void onRunPreComplete(LoudspeakerCallback action)
        {
            this.m_onPreComplete = action;
        }

        /// <summary>
        /// 滚动动画期间，每帧调用
        /// </summary>
        public void onRunUpdate()
        {
            float fCurPosX = transform.GetComponent<RectTransform>().anchoredPosition.x;
            if (!m_bPreCompleteFlag && fCurPosX < m_fPreCompleteX)
            {
                m_bPreCompleteFlag = true;
                m_onPreComplete();
            }
        }
    }
}
