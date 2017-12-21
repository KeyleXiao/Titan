/*******************************************************************
** 文件名:	UPopupMenuHandle.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	林建聪
** 日  期:	2016/6/22
** 版  本:	1.0
** 描  述:	弹出菜单事件与消息控件
** 应  用:  鼠标响应弹出菜单，前提是已经添加了菜单元素容器。
            1、获取UPopupMenuHandle脚本
            2、初始化HandlePopupAction 属性
                参考形式：
                HandlePopupAction = new UPopupAction()
                {
                    {"菜单元素名字", new UListComponentItemDataPopup(_actionSelected:选择回调)},
                    {"菜单元素名字", new UListComponentItemDataPopup(_actionSelected:选择回调,_nGroupID:1)},
                    {"菜单元素名字", new UListComponentItemDataPopup(_actionSelected:选择回调,_nGroupID:1)},
                    {"菜单元素名字", new UListComponentItemDataPopup(_actionSelected:选择回调,_nGroupID:2)},
                    {"菜单元素名字", new UListComponentItemDataPopup(_actionSelected:选择回调,_nGroupID:2)},
                    {"菜单元素名字", new UListComponentItemDataPopup(_actionSelected:选择回调,_nGroupID:3)},
                    {"菜单元素名字", new UListComponentItemDataPopup(_actionSelected:选择回调,_nGroupID:3)},
                };
            3、目前暂不支持3D模型处理弹出菜单，因为跟按键系统/游戏一些设置冲突。

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.EventSystems;


namespace USpeedUI.UWidgets
{
    public class UPopupMenuHandle : UCustomUIBehaviour, IPointerClickHandler//,IPointerDownHandler
    {
        /// <summary>
        /// 触发的鼠标按键
        /// </summary>
        [Tooltip("鼠标左中右三个按键触发弹出事件")]
        public PointerEventData.InputButton CallPopupMenuButton = PointerEventData.InputButton.Right;
        /// <summary>
        /// 弹出菜单元素容器
        /// </summary>
        public UPopupAction HandlePopupAction = null;
        /// <summary>
        /// 弹出菜单相对Handle的显示位置
        /// </summary>
        [Tooltip("弹出菜单相对Handle的位置")]
        public UIWidgets.TipPosition HandleTipPosition = UIWidgets.TipPosition.MouseBottomRightCorner;

        public void OnPointerClick(PointerEventData eventData)
        {
            if(eventData.button == CallPopupMenuButton)
            {
                if (HandlePopupAction == null || HandlePopupAction.Count == 0)
                {
                    Trace.LogWarning("要使用弹出菜单，你要先初始化元素容器.");
                    return;
                }
                    
                //发消息给PopupMenu View
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_POPUPMENU, new UIPopupMenuData(HandlePopupAction,GetTransform, HandleTipPosition));         
            }
        }

        public void OnPointerDown(PointerEventData eventData)
        {
            if (eventData.button == CallPopupMenuButton)
            {
                if (HandlePopupAction == null || HandlePopupAction.Count == 0)
                {
                    Trace.LogWarning("要使用弹出菜单，你要先初始化元素容器.");
                    return;
                }

                //发消息给PopupMenu View
                UISystem.Instance.SendWndMessage(WndMsgID.WND_MSG_COMMON_POPUPMENU, new UIPopupMenuData(HandlePopupAction, GetTransform, HandleTipPosition));
            }
        }
    }

    public class PopupTest : MonoBehaviour
    {
        UPopupAction HandlePopupAction;
        UPopupAction HandlePopupAction2;
        public UPopupMenuHandle handle;
        public Sprite TestSprite01;
        public Sprite TestSprite02;
        public Sprite TestSprite03;

        public bool useAction1;
        public bool useAction2;

        void Start()
        {
            handle = GetComponent<UPopupMenuHandle>();
            handle.HandlePopupAction = HandlePopupAction;
        }

        public void Update()
        {
            if(useAction1)
            {
                SetData();
                handle.HandlePopupAction = HandlePopupAction;
                useAction1 = false;
            }
            if(useAction2)
            {
                SetData();
                handle.HandlePopupAction = HandlePopupAction2;
                useAction2 = false;
            }
        }

        private void SetData()
        {
            HandlePopupAction = null;
               HandlePopupAction = new UPopupAction()
                {
                    {"菜单元素名字1", new UListComponentItemDataPopup(_actionSelected:Select,_nDepth:0,_sprItemPopup:TestSprite01,
                     _listChildItems:new List<UListComponentItemDataPopupBase>() { { new UListComponentItemDataPopup(_actionSelected: Select,_strItemTypeName:"子项测试1",_nDepth:1)},
                                                                                   { new UListComponentItemDataPopup(_actionSelected: Select,_strItemTypeName:"子项测试2",_nDepth:1)},
                                                                                   { new UListComponentItemDataPopup(_actionSelected: Select,_strItemTypeName:"子项测试3",_nDepth:1)} })},
                    {"菜单元素名字2", new UListComponentItemDataPopup(_actionSelected:Select)},
                    {"菜单元素名字4", new UListComponentItemDataPopup(_actionSelected:Select)},
                    {"菜单元素名字5", new UListComponentItemDataPopup(_actionSelected:Select,_nDepth:0,
                     _listChildItems:new List<UListComponentItemDataPopupBase>() { { new UListComponentItemDataPopup(_actionSelected: Select,_strItemTypeName:"子项测试3",_nDepth:1,_sprItemPopup:TestSprite02)},
                                                                                   { new UListComponentItemDataPopup(_actionSelected: Select,_strItemTypeName:"子项测试4",_nDepth:1,
                                                                                   _listChildItems:new List<UListComponentItemDataPopupBase>()
                                                                                   {
                                                                                       new UListComponentItemDataPopup(_actionSelected: Select,_strItemTypeName:"子项测试4-1",_nDepth:2),
                                                                                       new UListComponentItemDataPopup(_actionSelected: Select,_strItemTypeName:"子项测试4-2",_nDepth:2),
                                                                                   })} })},
                    {"菜单元素名字6", new UListComponentItemDataPopup(_actionSelected:Select)},
                    {"菜单元素名字7", new UListComponentItemDataPopup(_actionSelected:Select,_sprItemPopup:TestSprite03,_bInteractable:false)},
                };

            HandlePopupAction2 = null;
            HandlePopupAction2 = new UPopupAction()
                {
                    {"菜单元素名字1", new UListComponentItemDataPopup(_actionSelected:Select,_nDepth:0)},
                    {"菜单元素名字2", new UListComponentItemDataPopup(_actionSelected:Select,
                     _listChildItems:new List<UListComponentItemDataPopupBase>() { { new UListComponentItemDataPopup(_actionSelected: Select,_strItemTypeName:"子项测试1",_nDepth:1)},
                                                                                   { new UListComponentItemDataPopup(_actionSelected: Select,_strItemTypeName:"子项测试2",_nDepth:1)} })},
                    {"菜单元素名字4", new UListComponentItemDataPopup(_actionSelected:Select)},
                    {"菜单元素名字5", new UListComponentItemDataPopup(_actionSelected:Select,_nDepth:0,
                                                                                   _listChildItems:new List<UListComponentItemDataPopupBase>()
                                                                                   {
                                                                                       new UListComponentItemDataPopup(_actionSelected: Select,_strItemTypeName:"子项测试4-1",_nDepth:2),
                                                                                       new UListComponentItemDataPopup(_actionSelected: Select,_strItemTypeName:"子项测试4-2",_nDepth:2),
                                                                                   })},
                    {"菜单元素名字6", new UListComponentItemDataPopup(_actionSelected:Select)},
                    {"菜单元素名字7", new UListComponentItemDataPopup(_actionSelected:Select,_sprItemPopup:TestSprite03,_bInteractable:false,
                     _listChildItems:new List<UListComponentItemDataPopupBase>() { { new UListComponentItemDataPopup(_actionSelected: Select,_strItemTypeName:"子项测试3",_nDepth:1,_sprItemPopup:TestSprite02)},
                                                                                   { new UListComponentItemDataPopup(_actionSelected: Select,_strItemTypeName:"子项测试4",_nDepth:1)} })},
                };
        }


        public void Select()
        {
            Debug.Log("PopupTest Select;");
        }
    }
}
