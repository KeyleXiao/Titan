/*******************************************************************
** 文件名:	TimeThresholdGoodsView.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-4-12
** 版  本:	1.0
** 描  述:	天赋页界面视图模块
		
********************************************************************/

using UnityEngine;
using UnityEngine.UI;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.EventSystems;
using UnityEngine.Events;
using System;
using GameLogic;
using DataCenter;
using UIWidgets;
using USpeedUI.UExtensions;
using USpeedUI.TimeThresholdGoodsPart;
using U3D_Render.USpeedUI.UWidgets.UControls.UListView;

namespace USpeedUI
{
    public class TimeThresholdGoodsView : UIBaseWndView
    {
        TimeThresholdGoodsWnd TimeThresholdWnd;
        public  UListTimeThresholdList TimeThresholdList;
        public override void Start()
        {
            if(m_wnd == null)
            {
                return;
            }

            TimeThresholdWnd = (TimeThresholdGoodsWnd)m_wnd;
            return;
        }

        public void ClearTypeThresholdGoodList(TimeThresholdGoodsType type)
        {
            if(TimeThresholdList == null)
            {
                return;
            }

            ObservableList<UListTimeThresholdData> ListData = TimeThresholdList.GetTypeListTimeThresholdData(type);
            if (ListData == null)
            {
                return;
            }

            ListData.Clear();
        }

        public void AddThresholdGoodListItem(TimeThresholdGoodsType type,UListTimeThresholdData data)
        {
            if (TimeThresholdList == null)
            {
                return;
            }

            ObservableList<UListTimeThresholdData> ListData = TimeThresholdList.GetTypeListTimeThresholdData(type);
            if (ListData == null)
            {
                return;
            }

            ListData.Add(data);
        }

        public bool SetThresholdGoodListDataSource()
        {
            TimeThresholdList.DataSource.Clear();
            // 按照时间排序
            foreach (TimeThresholdGoodsType nType in TimeThresholdList.ListTimeThresholdData.Keys)
            {
                foreach (UListTimeThresholdData info in TimeThresholdList.ListTimeThresholdData[nType])
                {
                    TimeThresholdList.DataSource.Add(info);
                }
            }

            TimeThresholdList.DataSource.Sort(new Comparison<UListTimeThresholdData>(new UListTimeThresholdDataComparison().Compare));
            return TimeThresholdList.DataSource.Count > 0 ? true : false;
        }
    }
}