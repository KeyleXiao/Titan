/*******************************************************************
** 文件名:	USpeedUILayerManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-7-29
** 版  本:	1.0
** 描  述:	UI层级管理器
		    主要用于UILayer和UIElement注册、反注册操作和一些与UI层级有关的通用方法、定义
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;

namespace USpeedUI
{
    public class USpeedUILayerManager : Singleton<USpeedUILayerManager>
    {
        private Dictionary<USpeedUILayer, List<USpeedUILayerElement>> m_DicLayerElememt = new Dictionary<USpeedUILayer, List<USpeedUILayerElement>>();
        private Dictionary<WndLayerDef, USpeedUILayer> m_DicLayer = new Dictionary<WndLayerDef, USpeedUILayer>();
        private List<USpeedUILayerElement> m_ListRegistrationFailed = new List<USpeedUILayerElement>();

        protected const int LayerZAxisDistance = 5000;
        protected const int LayerOrderDelta = 5000;
        protected const int WndViewLayerOrderDelta = 1000;
        protected const int LayerElementSiblingIndexDelta = 100;

        //官方限制了SortingOrder范围，暂时么找到哪里有定义，先自己定义下        
        public const int MaxSortingOrder = 32767;
        public const int MinSortingOrder = -32767;


        public static void RegisterUILayer(WndLayerDef _nLayerDef, USpeedUILayer _nLayer)
        {
            
            if (_nLayerDef == WndLayerDef.WND_LAYER_MAX || _nLayerDef == WndLayerDef.WND_LAYER_NONE)
                return;

            if (instance.m_DicLayer.ContainsKey(_nLayerDef))
                return;

            instance.m_DicLayer.Add(_nLayerDef, _nLayer);

            //重新调整canvas的Sorting信息
            _nLayer.LayerCanvas.overrideSorting = true;

            //调整SortingLayer，目前都统一用一个Layer，layer多的话会影响渲染效率
            _nLayer.LayerCanvas.sortingLayerName = CalculateLayerName(_nLayerDef);
            //调整SortingOrder
            _nLayer.LayerCanvas.sortingOrder = CheckSortingOrderRange(CalculateLayerOrder(_nLayerDef));

            //计算Layer的Z轴坐标 layerDef越大，越靠近相机
            Vector3 tmpPos = _nLayer.GetTransform.localPosition;
            tmpPos.z = ((int)WndLayerDef.WND_LAYER_MAX - (int)_nLayerDef) * LayerZAxisDistance;
            _nLayer.GetTransform.localPosition = tmpPos;

            instance.RegisterFailedElementAgain();
        }
        public static void UnRegisterUILayer(WndLayerDef _nLayerDef, USpeedUILayer _nLayer)
        {
            if (_nLayerDef == WndLayerDef.WND_LAYER_MAX || _nLayerDef == WndLayerDef.WND_LAYER_NONE)
                return;

            if (instance.m_DicLayer.ContainsKey(_nLayerDef))
                instance.m_DicLayer.Remove(_nLayerDef);

        }
        public static USpeedUILayer GetUILayer(WndLayerDef _nLayerDef)
        {
            USpeedUILayer tmpLayer;
            if (instance.m_DicLayer.TryGetValue(_nLayerDef, out tmpLayer))
                return tmpLayer;

            Trace.LogWarning(_nLayerDef.ToString() + " 对应的UILayer目前没注册。");
            return null;
        }

        public static void RegisterElementForLayer(USpeedUILayer _nLayer, USpeedUILayerElement _nElement)
        {
            if (_nLayer == null)
            {
                if(!instance.m_ListRegistrationFailed.Contains(_nElement))
                    instance.m_ListRegistrationFailed.Add(_nElement);
                return;
            }                

            List<USpeedUILayerElement> layerElements;
            instance.m_DicLayerElememt.TryGetValue(_nLayer, out layerElements);

            if (layerElements != null)
            {
                if (!layerElements.Contains(_nElement))
                    layerElements.Add(_nElement);
            }
            else
            {
                layerElements = new List<USpeedUILayerElement>();
                layerElements.Add(_nElement);
                instance.m_DicLayerElememt.Add(_nLayer, layerElements);
            }

            //重新调整canvas的Sorting信息

            //调整SortingLayer，目前都统一用一个Layer，layer多的话会影响渲染效率
            _nElement.SortingLayerName = _nLayer.LayerCanvas.sortingLayerName;
            //调整SortingOrder
            _nElement.SortingLayerOrder = _nLayer.LayerCanvas.sortingOrder;

            if (instance.m_ListRegistrationFailed.Contains(_nElement) && _nLayer.LayerCanvas.sortingOrder != 0)
                instance.m_ListRegistrationFailed.Remove(_nElement);
        }
        public static void UnRegisterElementForLayer(USpeedUILayer _nLayer, USpeedUILayerElement _nElement)
        {
            if (_nLayer == null)
                return;

            List<USpeedUILayerElement> layerElements;
            if (instance.m_DicLayerElememt.TryGetValue(_nLayer, out layerElements))
            {
                layerElements.Remove(_nElement);
            }
        }

        public static void RegisterWndViewForLayer(IUIWndView _uView)
        {
            if (_uView.GetTransform() == null || _uView.GetUIWnd() == null)
                return;
            USpeedUILayerElement viewLayerElement = _uView.GetTransform().GetComponent<USpeedUILayerElement>() ?? _uView.GetTransform().gameObject.AddComponent<USpeedUILayerElement>();
            viewLayerElement.UILayerElementType = USpeedUILayerElement.EUILayerElementType.ELET_UI;
            viewLayerElement.IsUIRequirRaycaster = true;

            //重新调整canvas的Sorting信息
            //viewLayerElement = true;

            //调整SortingLayer，目前都统一用一个Layer，layer多的话会影响渲染效率
            viewLayerElement.SortingLayerName = CalculateLayerName(_uView.GetUIWnd().GetLayer());
            //调整SortingOrder
            viewLayerElement.SortingLayerOrder = CheckSortingOrderRange(CalculateWndViewLayerOrder(_uView));

            //计算Layer的Z轴坐标 layerDef越大，越靠近相机
            //Vector3 tmpPos = _nLayer.GetTransform.localPosition;
            //tmpPos.z = ((int)WndLayerDef.WND_LAYER_MAX - (int)_nLayerDef) * LayerZAxisDistance;
            //_nLayer.GetTransform.localPosition = tmpPos;

        }
        public static void UnRegisterWndViewForLayer(IUIWndView _uView) { }

        public static int CheckSortingOrderRange(int _nSortingOrder)
        {
            if(_nSortingOrder < MinSortingOrder || _nSortingOrder > MaxSortingOrder)
            {
                Trace.LogWarning("这里有个对象的SortingOrder超出[-32767,32767]范围，以强制改为最大/最小，注意修改下");
                _nSortingOrder = _nSortingOrder < MinSortingOrder ? MinSortingOrder : MaxSortingOrder;
            }
            return _nSortingOrder;
        }

        public static int CalculateLayerOrder(WndLayerDef _eLayer)
        {
            return 0;//(int)_eLayer * LayerOrderDelta;//值越大，越后渲染
        }

        public static int CalculateWndViewLayerOrder(IUIWndView _uView)
        {
            return _uView.GetTransform().GetSiblingIndex() * WndViewLayerOrderDelta + WndViewLayerOrderDelta;
        }

        /// <summary>
        /// USpeedUILayerElement（层级元素）计算公式
        /// 1、if( USpeedUILayerElement.gameobject has IUIWndView)
        ///     USpeedUILayerElement 在父节点下的对象层级顺序 * LayerElementSiblingIndexDelta + LayerElementSiblingIndexDelta;
        /// 2、else
        ///     USpeedUILayerElement.wndViewLayerOrder(element所在的View 层级) + USpeedUILayerElement.SortingLayerOrderOffset （element 层级偏移系数） 
        ///     + USpeedUILayerElement.SortingLayrOrderParentOffset（element 所有带 USpeedUILayerElement 的父对象层级偏移数） 
        ///     + USpeedUILayerElement.SortingLayerSiblingIndex （在父节点下的对象层级顺序 ） * LayerElementSiblingIndexDelta（增量系数）;
        /// </summary>
        /// <param name="_nElement"></param>
        /// <returns></returns>
        public static int CalculateLayerElementOrder(USpeedUILayerElement _nElement)
        {
            if (_nElement.UIWndView == null || _nElement.UIWndView.GetTransform() == null || _nElement.UIWndView.GetTransform().GetComponent<Canvas>() == null)
                return _nElement.SortingLayerOrderOffset;

            if(Transform.Equals(_nElement.UIWndView.GetTransform(), _nElement.GetTransform))
            {
                //当前element 是View
                return CalculateWndViewLayerOrder(_nElement.GetComponent<IUIWndView>());
                //return _nElement.SortingLayerSiblingIndex * LayerElementSiblingIndexDelta + LayerElementSiblingIndexDelta;
            }
            else
            {
                int viewLayer = _nElement.UIWndView.GetTransform().GetComponent<Canvas>().sortingOrder;
                return viewLayer + _nElement.SortingLayerOrderOffset + _nElement.SortingLayrOrderParentOffset +
                    _nElement.SortingLayerSiblingIndex * LayerElementSiblingIndexDelta;
            }
                
        }
        //由于SortingOrder有限为了方便，使用了4个SortingLayer区分
        public static string CalculateLayerName(WndLayerDef _nLayerDef = WndLayerDef.WND_LAYER_NONE)
        {
            string tmpSortingLayerName = string.Empty;

            switch (_nLayerDef)
            {
                case WndLayerDef.WND_LAYER_FIXED:
                    {
                        tmpSortingLayerName = Config.SortingLayerUI_FIXED;
                    }
                    break;
                case WndLayerDef.WND_LAYER_POPUP01:
                    {
                        tmpSortingLayerName = Config.SortingLayerUI_POPUP01;
                    }
                    break;
                case WndLayerDef.WND_LAYER_FILL:
                    {
                        tmpSortingLayerName = Config.SortingLayerUI_FILL;
                    }
                    break;
                case WndLayerDef.WND_LAYER_POPUP02:
                    {
                        tmpSortingLayerName = Config.SortingLayerUI_POPUP02;
                    }
                    break;
                case WndLayerDef.WND_LAYER_OVERHEAD:
                    {
                        tmpSortingLayerName = Config.SortingLayerUI_OVERHEAD;
                    }
                    break;
                case WndLayerDef.WND_LAYER_MAX:
                    {
                        tmpSortingLayerName = Config.SortingLayerUI;
                    }
                    break;

                case WndLayerDef.WND_LAYER_NONE:
                default:
                    return Config.LayerDefault;
            }
            
            //调整SortingLayer，目前都统一用一个Layer，layer多的话会影响渲染效率
            if (!SortingLayer.layers.Any(x => x.name == tmpSortingLayerName))
            {
                Trace.LogWarning("没有名称为:" + Config.SortingLayerUI + " 的SortingLayer,使用默认的");
                tmpSortingLayerName = SortingLayer.layers[0].name;
            }
            return tmpSortingLayerName;
        }
        
        private void RegisterFailedElementAgain()
        {
            if(m_ListRegistrationFailed.Count>0)
            {
                int oldLength = m_ListRegistrationFailed.Count;
                for(int i=0;i<m_ListRegistrationFailed.Count;)
                {
                    if(m_ListRegistrationFailed[i] == null)
                        break;

                    m_ListRegistrationFailed[i].RegisterUIElement();
                    if (m_ListRegistrationFailed.Count < oldLength)
                    {
                        i = 0;
                        oldLength = m_ListRegistrationFailed.Count;
                    }
                    else
                        i++;
                }
            }
        }


        protected int m_UIMeshCameraCount = 0;
        protected GameObject UIMeshCameraRoot;
        public static Camera GetUIMeshCamera()
        {
            if(instance.UIMeshCameraRoot == null)
            {
                instance.UIMeshCameraRoot = new GameObject("UIMeshCameraRoot");
                instance.UIMeshCameraRoot.transform.SetPosition(new Vector3(20000, 20000, 20000));
            }

            instance.m_UIMeshCameraCount++;
            var newCameraGO = new GameObject("UIMeshCamera_" + instance.m_UIMeshCameraCount);
            newCameraGO.transform.SetParent(instance.UIMeshCameraRoot.transform, false);

            var newCamera = newCameraGO.AddComponent<Camera>();
            newCamera.clearFlags = CameraClearFlags.SolidColor;
            newCamera.backgroundColor = new Color();
            newCamera.farClipPlane = 5;
            return newCamera;
        }
        public static void ReleaseUIMeshCamera(Camera _camera)
        {
            instance.m_UIMeshCameraCount--;
            var temp1 = _camera.gameObject;
            ResNode.DestroyRes(ref temp1);
        }
    }
}
