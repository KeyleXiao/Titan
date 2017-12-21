/*******************************************************************
** 文件名:	EntitySkinControl.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-06-25
** 版  本:	1.0
** 描  述:	实体皮肤控制
********************************************************************/

using UnityEngine;
using System;
using System.Collections.Generic;
using UnityEngine.Events;

namespace U3D_Render.SkinCtrl
{
    /// <summary>
    /// 皮肤切换结果
    /// </summary>
    public enum SkinChangedReslut
    {
        Succes = 0,
        Faild = 1,
        SameSkin = 2,
    }

    /// <summary>
    /// 皮肤状态数据
    /// </summary>
    public class SkinStateData
    {
        public float Alpha = 1.0f;
        public Vector3 Scale = Vector3.one;
        public bool Active = true;
        public bool Vislble = true;
    }

    public class ChangeSkinData
    {
        public int nPart = 0;
    }

    /// <summary>
    /// 实体皮肤控制
    /// </summary>
    public partial class EntitySkinControl: MonoBehaviour
    {
        #region << Field Area >>
        protected bool bInit = false;

        public const int InvalidSkinID = -10000;

        /// <summary>
        /// 当前皮肤ID
        /// </summary>
        public int currentSkinId = InvalidSkinID;

        /// <summary>
        /// 当前已经加载的皮肤
        /// </summary>
        protected SkinInstance m_currentLoadedSkinInstance = null;

        public SkinInstance CurrentSkinInstance
        {
            get 
            {
                if(m_currentLoadedSkinInstance != null &&  m_currentLoadedSkinInstance.isValid)
                {
                    if(currentSkinId == m_currentLoadedSkinInstance.nSkinID)
                    {
                        return m_currentLoadedSkinInstance;
                    }
                }
                return null;
            }
        }

        public float SkinAlpha
        {
            get
            {
                if (m_currStateData == null)
                {
                    return m_currStateData.Alpha;
                }
                return 1.0f;
            }
        }

        public SkinStateData currStateData
        {
            get
            {
                return m_currStateData;
            }
        }
        protected SkinStateData m_currStateData = null;

        protected bool NotReleaseFromCache = false;
        #endregion


        #region << Method Area >>

        protected virtual void SceneLoadBegain(string sceneName)
        {
            Clear();
        }

        protected virtual void SceneLoadFinish(string sceneName)
        {
            NotReleaseFromCache = !GameLogicAPI.isMainMap(SceneManager.Instance.m_curMapID);
        }

        protected virtual void Start()
        {
            SceneManager.RegisterSceneLoadStartEvent(SceneLoadBegain);
            SceneManager.RegisterSceneLoadFinishEvent(SceneLoadFinish);
            NotReleaseFromCache = !GameLogicAPI.isMainMap(SceneManager.Instance.m_curMapID);
        }

        protected virtual void OnDestroy()
        {
            SceneManager.UnRegisterSceneLoadStartEvent(SceneLoadBegain);
            SceneManager.UnRegisterSceneLoadFinishEvent(SceneLoadFinish);
        }
        
        protected virtual void LateUpdate()
        {

        }

        public virtual void Init()
        {
            currentSkinId = InvalidSkinID;
            if (null == m_currStateData)
            {
                m_currStateData = new SkinStateData();
            }

            m_currStateData.Active = true;
            m_currStateData.Alpha = 1.0f;
            m_currStateData.Scale = Vector3.one;
            m_currStateData.Vislble = true;

            if(null == onSkinCull)
            {
                onSkinCull = new UnityEvent();
            }

            if(null == onSkinNotCull)
            {
                onSkinNotCull = new UnityEvent();
            }

            if (null == onSkinChanged)
            {
                onSkinChanged = new SkinChangedEvent();
            }
            
        }


        /// <summary>
        /// 释放对象
        /// </summary>
        public virtual void Clear()
        {
            bInit = false;
            CollectAllSkin();
            ClearCull();
        }

        /// <summary>
        /// 回收皮肤
        /// </summary>
        public virtual void CollectAllSkin()
        {

        }
        /// <summary>
        /// 设置实体缩放
        /// </summary>
        /// <param name="sca"></param>
        public virtual void SetScale(Vector3 sca)
        {
            Debug.LogError("必须在子类重写这个函数");
        }

        /// <summary>
        /// 获取实体缩放
        /// </summary>
        /// <returns></returns>
        public virtual Vector3 GetScale()
        {
            Debug.LogError("必须在子类重写这个函数");
            return Vector3.one;
        }
        
        /// <summary>
        /// 修改皮肤
        /// </summary>
        /// <param name="nSkinID">皮肤ID</param>
        /// <param name="nPart">哪个部位</param>
        public virtual SkinChangedReslut ChangeSkin(int nSkinID, int nPart,AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
		//public virtual SkinChangedReslut ChangeSkin(int nSkinID, int nPart)
        {
            return SkinChangedReslut.Faild;
        }

        /// <summary>
        /// 获取当前模型对象
        /// </summary>
        public virtual GameObject GetCurrentModelObject()
        {
            Debug.LogError("必须在子类重写这个函数");
            return null;

        }

        public virtual void setAlpha(float alpha)
        {
            Debug.LogError("必须在子类重写这个函数");
        }

        /// <summary>
        /// 设置皮肤的可见性
        /// </summary>
        /// <param name="bVisible">可见性</param>
        /// <param name="ApplyAll">是否引用所有</param>
        public virtual void SetVisible(bool bVisible,bool ApplyAll = false)
        {
            Debug.LogError("必须在子类重写这个函数！");
        }

        public virtual void ApplyStateData(SkinStateData data)
        {
            Debug.LogError("必须在子类重写这个函数！");
        }
       #endregion
    }

}
