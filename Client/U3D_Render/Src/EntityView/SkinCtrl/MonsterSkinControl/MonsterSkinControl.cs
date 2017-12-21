/*******************************************************************
** 文件名:	MonsterSkinControl.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-06-25
** 版  本:	1.0
** 描  述:	怪物实体皮肤控制
********************************************************************/

using UnityEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace U3D_Render.SkinCtrl
{
    /// <summary>
    /// 怪物皮肤控制
    /// </summary>
    public partial class MonsterSkinControl : EntitySkinControl
    {
        #region << Field Area >>

        MonsterStateMachine m_machine = null;

        #endregion


        #region << Method Area >>

        public override void Init()
        {
            if(bInit)
            {
                return;
            }
            base.Init();
            m_machine = GetComponent<MonsterStateMachine>();
            bInit = true;
   
        }

        protected override void LateUpdate()
        {
            UpdateLoadingSkin();
        }
        private void ClearMonsterSkinDataFromMachine(MonsterStateMachine machine)
        {
            m_machine.animator = null;
        }

        private void ApplyMonsterSkinDataToMachine(MonsterStateMachine machine, SkinInstance instance)
        {
            MonsterSkinConfig SkinConfig = instance.entitySkinConfig as MonsterSkinConfig;
            if (null == SkinConfig)
            {
                Debug.LogError("切换皮肤重大错误-ApplyMonsterSkinDataToMachine");
                return;
            }

            machine.animator = instance.MainAnimator;
            machine.RefershAnimatorDataWhenSkinChangedFinish();
            instance.ReferencedSkinCtrl = this;
        }

        public override GameObject GetCurrentModelObject()
        {
            if (CurrentSkinInstance == null)
            {
                return null;
            }
            return CurrentSkinInstance.SkinObj;
        }

        /// <summary>
        /// 修改皮肤
        /// </summary>
        /// <param name="nSkinID">皮肤ID</param>
        /// <param name="nPart">哪个部位</param>
        public override SkinChangedReslut ChangeSkin(int nSkinID, int nPart, AssetLoadPriority priority = AssetLoadPriority.Priority_Normal)
		//public override SkinChangedReslut ChangeSkin(int nSkinID, int nPart)
        {

            if (currentSkinId == nSkinID)
            {
                return SkinChangedReslut.SameSkin;
            }
            //if (nSkinID == InvalidSkinID)
            //{
            //    ClearMonsterSkinDataFromMachine(m_machine);
            //    SkinInstance.DisableSkinInstance(m_currentSkinInstance);
            //    SkinManager.DeleteSkinAsync(currentSkinId, OnSkinLoadFinish);
            //    RemoveCallBack(currentSkinId);
            //    m_currentSkinInstance = null;
            //    currentSkinId = nSkinID;


            //    return true;
            //}
            RemoveCallBack(currentSkinId);
            currentSkinId = nSkinID;

            if (SkinManager.GetSkinAsync(nSkinID, OnSkinLoadFinish, priority))
		   //if (SkinManager.GetSkinAsync(nSkinID, OnSkinLoadFinish))
            {
                PushToCallBack(nSkinID, nPart);
                return SkinChangedReslut.Succes;
            }
            return SkinChangedReslut.Faild;
        }


        public override void SetScale(Vector3 sca)
        {
            m_currStateData.Scale = sca;

            if (null == CurrentSkinInstance)
            {
                return;
            }
            //保存下原来的
             Vector3 tempScale = sca; 
                
            //先取出配置中的缩放
             float s = CurrentSkinInstance.ConfigScale;
            //再取出预制体中的缩放
             Vector3 prefabScale = CurrentSkinInstance.PrefabScale;
            
           
            //将要缩放的值先乘预制体的
            tempScale.x *= prefabScale.x;
            tempScale.y *= prefabScale.y;
            tempScale.z *= prefabScale.z;

            //再乘上配置中的缩放，得出最后的缩放
            tempScale = tempScale * s;
            GameObject curSkin = CurrentSkinInstance.SkinObj;
            //curSkin.transform.localScale = tempScale;

            m_machine.creaturePropety.SetScale(tempScale);
        }

        public override Vector3 GetScale()
        {
            return m_currStateData.Scale;
        }

        public override void ApplyStateData(SkinStateData data)
        {
            if(data.Active)
            {
                SkinManager.EnableSkinInstance(CurrentSkinInstance);

            }
            else
            {
                SkinManager.DisableSkinInstance(CurrentSkinInstance);

            }
            SetScale(data.Scale);
            SetVisible(data.Vislble, true);
            setAlpha(data.Alpha);
        }

        public override void setAlpha(float alpha)
        {
            m_currStateData.Alpha = alpha;
            if (CurrentSkinInstance != null)
            {
                if (alpha > 0.99f)
                {
                    CurrentSkinInstance.ResetAlpha();
                }
                else
                {
                    CurrentSkinInstance.SetAlpha(alpha);
                }
            }
        }


        public override void CollectAllSkin()
        {
            if (currentSkinId == InvalidSkinID)
            {
                return;
            }
            SetVisible(true, true);
            setAlpha(1.0f);

            CollectAsyncData();
            currentSkinId = InvalidSkinID;

        }

        public override void SetVisible(bool bVisible, bool ApplyAll = false)
        {
            m_currStateData.Vislble = bVisible;
            if (!SkinInstance.isNullOrEmpry(CurrentSkinInstance))
            {
                SetVisible_Impl(bVisible, CurrentSkinInstance);
            }

        }

        private void SetVisible_Impl(bool bVisible, SkinInstance si)
        {
            if (!SkinInstance.isNullOrEmpry(si))
            {
                si.SetVisible(bVisible);
                si.SetBindEffectEnabled(bVisible);
            }
        }
        #endregion
    }
}
