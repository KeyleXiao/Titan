/*******************************************************************
** 文件名:	AssetBundleManager_Prefab.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	李有红
** 日  期:	2016/10/20
** 版  本:	1.0
** 描  述:	喇叭消息
** 应  用:  
 *****************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using UnityEngine.UI;
using UnityEngine;
using U3D_Render.Common;
using DataCenter;
using DG.Tweening;

namespace USpeedUI.Loudspeaker
{
    public class LoudspeakerWnd : UIBaseWnd<LoudspeakerWndView>
    {
        public override WndID GetID()
        {
            return WndID.WND_ID_LOUDSPEAKER;
        }

        public override WndLayerDef GetLayer()
        {
            return WndLayerDef.WND_LAYER_FIXED;
        }

        public override string GetPrefabPath()
        {
            return "Loudspeaker/LoudspeakerView";
        }

        public override bool Init()
        {
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.RegisterWndMessage(WndMsgID.WND_MSG_LOUDSPEAKER_ADD, this);
            return base.Init();
        }

        public override void Destroy()
        {
            base.Destroy();
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE, this);
            UISystem.Instance.UnregisterWndMessage(WndMsgID.WND_MSG_LOUDSPEAKER_ADD, this);
        }

        public override void OnMessage(WndMsgID msgID, UIMsgData data)
        {
            switch (msgID)
            {
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_ENTER:
                    {
                        LoadView();
                    }
                    break;
                case WndMsgID.WND_MSG_COMMOM_STATICGAMESTATE_LEAVE:
                    {
                        UnloadView();
                    }
                    break;
                case WndMsgID.WND_MSG_LOUDSPEAKER_ADD:
                    {
                        if (m_wndView != null)
                        {
                            m_wndView.onAddMessage();
                        }
                    }
                    break;
				case WndMsgID.WND_MSG_COMMON_LOGINSTATE_ENTER:
					{
						LogicDataCenter.loudspeakerManager.ResetData();
					}
					break;
                default:
                    break;
            }
        }
    }

    #region 若干帧更新
    /// <summary>
    /// 固定帧数更新
    /// </summary>
    public class UFixedFrameUpdateHelper
    {
        // 固定帧数更新一次文字，默认为一帧
        private int m_UpdateRate = 1;
        public int UpdateRate { set { m_UpdateRate = value; } }

        // 帧计数
        private int m_nFrameCount = 0;

        public delegate void UpdatePerFrameDelegate();

        UpdatePerFrameDelegate m_UpdateDelegate;

        public UFixedFrameUpdateHelper(UpdatePerFrameDelegate update)
        {
            m_UpdateDelegate = update;
        }

        // 每帧更新
        public void Update()
        {
            // 若干帧更新一次
            m_nFrameCount++;
            if (m_nFrameCount >= m_UpdateRate)
            {
                m_nFrameCount = 0;

                m_UpdateDelegate();
            }
        }
    }
    #endregion

    public class LoudspeakerWndView : UIBaseWndView
    {
        // 公告内容
        public LoudspeakerContentItem[] m_LoudspeakerItemList;

        // 是否可以播放下一个喇叭
        private bool m_bRunNext = false;

        public override void Start()
        {
            // 设置滚动结束回调函数
            foreach (var item in m_LoudspeakerItemList)
            {
                item.onRunComplete(onEndScroll);
                item.onRunPreComplete(onPreEndScroll);
            }

            base.Start();
        }

        public override void Update()
        {
            // 没有喇叭滚动 且 没有喇叭排队
            if (!IsRunning() && !LogicDataCenter.loudspeakerManager.HasLoudspeakerMessage())
            {
                SetVisible(false);
            }

            // 可以播放下一个喇叭
            if (m_bRunNext)
            {
                detectLoudspeakerMessage();
                m_bRunNext = false;
            }

            base.Update();
        }

        /// <summary>
        /// 检测是否有消息
        /// </summary>
        public void detectLoudspeakerMessage()
        {
            // 获取一个喇叭滚动条
            LoudspeakerContentItem noticeItem = getFreeLoudspeakerItem();
            if (noticeItem == null)
            {
                return;
            }

            // 获取一个公告
            if (!LogicDataCenter.loudspeakerManager.HasLoudspeakerMessage())
            {
                return;
            }
            LoudspeakerManager.SLoudspeakerInfo info = LogicDataCenter.loudspeakerManager.GetLoudspeakerMessage();
            LoudspeakerContentItemData data = new LoudspeakerContentItemData();
            data.info = info;

            // 设置数据
            noticeItem.SetData(data);

            // 开始滚动
            noticeItem.Run();
        }

        /// <summary>
        /// 喇叭跑马灯结束
        /// </summary>
        public void onEndScroll()
        {
            if (!IsRunning())
            {
                m_bRunNext = true;
            }
        }

        /// <summary>
        /// 喇叭跑马灯伪结束
        /// </summary>
        public void onPreEndScroll()
        {
            m_bRunNext = true;
        }

        /// <summary>
        /// 获取空闲的喇叭滚动条
        /// </summary>
        /// <returns>空闲的喇叭滚动条</returns>
        public LoudspeakerContentItem getFreeLoudspeakerItem()
        {
            foreach (var item in m_LoudspeakerItemList)
            {
                if (!item.IsRunning)
                {
                    return item;
                }
            }

            return null;
        }

        /// <summary>
        /// 是否在有喇叭在进行滚动
        /// </summary>
        /// <returns></returns>
        public bool IsRunning()
        {
            foreach (var item in m_LoudspeakerItemList)
            {
                if (item.IsRunning)
                {
                    return true;
                }
            }

            return false;
        }

        /// <summary>
        /// 有新喇叭消息增加
        /// </summary>
        public void onAddMessage()
        {
            if (!IsRunning())
            {
                SetVisible(true);
                m_bRunNext = true;
            }
        }
    }
}
