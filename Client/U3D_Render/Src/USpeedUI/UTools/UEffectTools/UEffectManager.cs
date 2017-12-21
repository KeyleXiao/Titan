/*******************************************************************
** 文件名:	UEffectManager.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-6-28
** 版  本:	1.0
** 描  述:	目前有两种效果类型
            1、预制体效果(UEffectManager_EffectPrefabType)
            2、全屏效果(UEffectManager_EffectImageType)
            
            需要新增 预制体效果 时看UEffectManager_EffectPrefabType.cs顶部说明；
            需要新增 全屏效果   时跟客户端联系		
       
********************************************************************/
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.Events;
using UnityEngine.UI;

namespace USpeedUI.UEffect
{
    #region 一些用到的数据
    /// <summary>
    /// 创建效果时参数列表
    /// </summary>
    [Serializable]
    public class UEffectParamBase
    {
        /// <summary>   
        /// 挂接延迟    
        /// <para>目前UET_EffectImage类型的效果没有延迟功能</para>
        /// </summary>
        public float? AttachDelay;
        /// <summary>   挂接对象    </summary>
        public Transform AttachParent;

        /// <summary>
        /// 添加效果参数列表
        /// </summary>
        /// <param name="AttachParent">挂接对象</param>
        /// <param name="_fAttachDelay">挂接延迟</param>
        public UEffectParamBase(Transform _tfAttachParent, float? _fAttachDelay = null)
        {
            AttachDelay = _fAttachDelay;
            AttachParent = _tfAttachParent;
        }
    }

    /// <summary>
    /// 预制体效果参数类型。改参数类型只用于同步加载资源。
    /// </summary>
    [Serializable]
    public class UEffectPrefabParam : UEffectParamBase
    {
        /// <summary>   类型    </summary>
        public UEffectPrefabType AttachEffectType;
        /// <summary>   挂接后显示坐标(相对挂接对象)    </summary>
        public Vector3? AttachPosition;
        /// <summary>   挂节后旋转角度(相对挂接对象)    </summary>
        public float? AttachRotation;
        /// <summary>   是否播放完后自动移除    </summary>
        public bool AutoDestroy;

        //////////////////非构建属性

        /// <summary>   创建成功的时候返回预制体对应的实例引用    </summary>
        public GameObject EffectGameObject;

        /// <summary>
        /// 资源结点
        /// </summary>
        public ResNode EffectRes = null;
        /// <summary>
        /// 用于删除时，释放实例的同时是否要释放资源
        /// </summary>
        public bool IsDestroyRes;

        /// <summary>
        /// 添加效果参数列表
        /// </summary>
        /// <param name="_eType">类型</param>
        /// <param name="_tfAttachParent">挂接对象</param>
        /// <param name="_vAttachPosition">挂接后显示坐标(相对挂接对象)</param>
        /// <param name="_vAttachRotation">挂节后旋转角度(相对挂接对象)</param>
        public UEffectPrefabParam(UEffectPrefabType _eType , Transform _tfAttachParent , Vector3? _vAttachPosition = null, float? _fAttachRotation = null,bool _bAutoDestroy = false, bool _bIsDestroyRes = false)
            :base(_tfAttachParent)
        {
            AttachEffectType = _eType;
            AttachParent = _tfAttachParent;
            AttachPosition = _vAttachPosition;
            AttachRotation = _fAttachRotation;
            AutoDestroy = _bAutoDestroy;
            IsDestroyRes = _bIsDestroyRes;
        }
    }

    /// <summary>
    /// 预制体效果参数类型。改参数类型只用于同步加载资源。
    /// </summary>
    [Serializable]
    public class UEffectPrefabAsyncParam : UEffectPrefabParam
    {    /// <summary>   资源加载回调，主要用于资源未加载，加载结束后第一次实例化执行用户操作    </summary>
        public Action<UEffectPrefabAsyncParam> onResLoadFinished = null;
        /// <summary>   资源加载回调时用户需要的数据    </summary>
        public object ResLoadUserObject = null;


        public UEffectPrefabAsyncParam(UEffectPrefabType _eType, Transform _tfAttachParent, Vector3? _vAttachPosition = null, float? _fAttachRotation = null, bool _bAutoDestroy = false, Action<UEffectPrefabAsyncParam> _onResLoadFinished = null, object _resLoadUserObject = null, bool _bIsDestroyRes = false) :base(_eType,_tfAttachParent,_vAttachPosition,_fAttachRotation,_bAutoDestroy, _bIsDestroyRes)
        {
            onResLoadFinished = _onResLoadFinished;
            ResLoadUserObject = _resLoadUserObject;
        }
    }

    [Serializable]
    public class UEffectImageParam : UEffectParamBase
    {
        /// <summary>全屏效果类型</summary>
        public Type AttachImageEffect;


        /// <summary>
        /// 添加效果类型参数列表
        /// </summary>
        /// <param name="_eAttachImageEffect">那个View用全屏效果</param>
        public UEffectImageParam(Type _eAttachImageEffect, Transform _tfAttachParent)
            :base(_tfAttachParent)
        {
            AttachImageEffect = _eAttachImageEffect;
        }
    }

    [Serializable]
    public class UEffectMaterialParam : UEffectParamBase
    {
        public UEffectMaterialType MaterialType;
        public bool IsNewMaterial;
        public Graphic GraphicsTarget;
        [SerializeField]
        public Material ReturnEffectMaterial;

        public UEffectMaterialParam(UEffectMaterialType _eMatType, Graphic _gGraphicsTarget = null,bool _bNewMat = false,Transform _tfAttachParent = null)
            :base(_tfAttachParent)
        {
            MaterialType = _eMatType;
            IsNewMaterial = _bNewMat;
            GraphicsTarget = _gGraphicsTarget;
        }
        public UEffectMaterialParam(Transform _tfAttachParent = null):base(_tfAttachParent) { }
    }

    [Serializable]
    public class UEffectMeshParam : UEffectParamBase
    {       
        public Vector2 RTTSize;

        //可返回可输入
        public List<UEffectMeshObject> MeshObjectList;
        public UEffectMeshCamera MeshCamrea;

        /// <summary>
        /// 返回MeshEffectNode对象
        /// </summary>
        public UEffectNode MeshRenderNode;
        /// <summary>
        /// 
        /// </summary>
        /// <param name="_vRTTSize">渲染图片大小</param>
        /// <param name="_listMeshObject">一堆要显示在UI的模型对象</param>
        /// <param name="_nMeshCamera">观察那对模型对象的相机</param>
        /// <param name="_tfAttachParent">要添加EffectMeshRenderNode脚本的对象</param>
        public UEffectMeshParam(Vector2 _vRTTSize,List<UEffectMeshObject> _listMeshObject,UEffectMeshCamera _nMeshCamera, Transform _tfAttachParent = null):base(_tfAttachParent)
        {
            RTTSize = _vRTTSize;
            MeshObjectList = _listMeshObject;
            MeshCamrea = _nMeshCamera;
        }
    }

    public enum UEffectType
    {
        UET_None,
        /// <summary> 预制体特效 </summary>
        UET_EffectPrefab,
        /// <summary> 全屏背景效果 </summary>
        UET_EffectImage,
        /// <summary> 材质效果 </summary>
        UET_EffectMaterial,
        /// <summary> 模型效果效果 </summary>
        UET_EffectMesh,
    }
    #endregion


    public partial class UEffectManager : Singleton<UEffectManager>, ITimerHanlder
    {
        protected enum EEffectTimer
        {
            /// <summary>延时定时器句柄</summary>
            EET_Delay,
        }

        //下面这个值注意修改
        public const int MaxTimerIDCount = 10000;

        /// <summary>
        /// 预加载一些效果，目前只支持预制体效果。
        /// </summary>
        /// <param name="_eType"></param>
        /// <param name="_nParam"></param>
        /// <returns></returns>
        public bool PreLoadEffect(UEffectType _eType,ref UEffectParamBase _nParam)
        {
            switch (_eType)
            {
                case UEffectType.UET_EffectPrefab:
                    {
                        if (_nParam is UEffectPrefabParam)
                            return LoadEffectPrefab((_nParam as UEffectPrefabParam)) != null;
                        else
                        {
                            Trace.LogWarning("创建失败，效果类型是 UET_EffectPrefab，但参数不是 UEffectPrefabParam 这个类型");
                            return false;
                        }
                    }
                    break;
                default:
                    Trace.LogWarning("目前只有预制体特效才会进行预加载");
                    return false;

            }
        }

        /// <summary>
        /// 添加UI效果
        /// </summary>
        /// <param name="_eType">效果类型</param>
        /// <param name="_nParam">效果参数</param>
        /// <returns></returns>
        public bool CreateEffect(UEffectType _eType,ref UEffectParamBase _nParam)
        {
            switch (_eType)
            {
                case UEffectType.UET_EffectPrefab:
                    {
                        if (_nParam is UEffectPrefabParam)
                            return CreatePrefabEffect(_nParam as UEffectPrefabParam);
                        else
                        {
                            Trace.LogWarning("创建失败，效果类型是 UET_EffectPrefab，但参数不是 UEffectPrefabParam 这个类型");
                            return false;
                        }
                    }
                case UEffectType.UET_EffectImage:
                    {
                        if (_nParam is UEffectImageParam)
                            return CreateImageEffect(_nParam as UEffectImageParam);
                        else
                        {
                            Trace.LogWarning("创建失败，效果类型是 UET_EffectImage，但参数不是 UEffectImageParam 这个类型");
                            return false;
                        }
                    }
                case UEffectType.UET_EffectMaterial:
                    {
                        if (_nParam is UEffectMaterialParam)
                            return CreateMaterialEffect(_nParam as UEffectMaterialParam);
                        else
                        {
                            Trace.LogWarning("创建失败，效果类型是 UET_EffectMaterial，但参数不是 UEffectMaterialParam 这个类型");
                            return false;
                        }
                    }
                case UEffectType.UET_EffectMesh:
                    {
                        if (_nParam is UEffectMeshParam)
                            return CreateMeshEffect(_nParam as UEffectMeshParam);
                        else
                        {                             
                            Trace.LogWarning("创建失败，效果类型是 UET_EffectMesh，但参数不是 UEffectMeshParam 这个类型");
                            return false;
                        }
                    }
                default:
                    return false;
            }

        }

        /// <summary>
        /// 移除效果
        /// </summary>
        /// <param name="_eType">效果类型</param>
        /// <param name="_goEffect">效果所处对象</param>
        public bool DestroyEffect(UEffectType _eType, ref UEffectParamBase _nParam)
        {
            switch (_eType)
            {
                case UEffectType.UET_EffectPrefab:
                    {
                        if (_nParam is UEffectPrefabParam)
                            return DestroyPrefabEffect(_nParam as UEffectPrefabParam);
                        else
                        {
                            Trace.LogWarning("移除失败，效果类型是 UET_EffectPrefab，但参数不是 UEffectPrefabParam 这个类型");
                            return false;
                        }
                    }
                case UEffectType.UET_EffectImage:
                    {
                        if (_nParam is UEffectImageParam)
                            return DestroyImageEffect(_nParam as UEffectImageParam);
                        else
                        {
                            Trace.LogWarning("创建失败，效果类型是 UET_EffectImage，但参数不是 UEffectImageParam 这个类型");
                            return false;
                        }
                    }
                case UEffectType.UET_EffectMaterial:
                    {
                        if (_nParam is UEffectMaterialParam)
                            return DestroyMaterialEffect(_nParam as UEffectMaterialParam);
                        else
                        {
                            Trace.LogWarning("创建失败，效果类型是 UET_EffectMaterial，但参数不是 UEffectMaterialParam 这个类型");
                            return false;
                        }
                    }
                case UEffectType.UET_EffectMesh:
                    {
                        if (_nParam is UEffectMeshParam)
                            return DestroyMeshEffect(_nParam as UEffectMeshParam);
                        else
                        {
                            Trace.LogWarning("创建失败，效果类型是 UET_EffectMesh，但参数不是 UEffectMeshParam 这个类型");
                            return false;
                        }
                    }
                default:
                    return false;
            }
        }

        /// <summary>
        /// 释放所有资源与效果
        /// </summary>
        /// <returns></returns>
        public bool DestroyAll()
        { return true; }

        public void OnTimer(int nTimerID)
        {
            if (m_dicTimerIDObject.ContainsKey(nTimerID))
            {
                //延时类型
                if (nTimerID - (int)EEffectTimer.EET_Delay * MaxTimerIDCount < MaxTimerIDCount)
                {
                    m_dicTimerIDObject[nTimerID].SetActive(true);
                }

                //TimerManager.KillTimer(this, nTimerID);
                m_dicTimerIDObject.Remove(nTimerID);
            }
        }
    }
}
