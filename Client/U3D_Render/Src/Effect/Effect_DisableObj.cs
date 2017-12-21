/*******************************************************************
** 文件名:	Effect_DisableObj.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	禁用物体

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace Effect
{
    /// <summary>
    /// 禁用物体
    /// </summary>
    public class Effect_DisableObj : MonoBehaviour
    {
        public float DisableTime = 0.0f;

        public List<GameObject> DisableList = new List<GameObject>();
        public List<TrailsSystem.Effect_TrailRenderer_Base> TrailRendererDisableList = new List<TrailsSystem.Effect_TrailRenderer_Base>();
        public List<Effect_WeaponTrailBase> WeaponTrailRendererDisableList = new List<Effect_WeaponTrailBase>();
        private bool bInit = false;
        Coroutine waitplay = null;
        public void Start()
        {
            Init();
        }

        public void OnDestroy()
        {
            if(DisableList.Count > 0)
            {
                DisableList.Clear();
            }

            if (TrailRendererDisableList.Count > 0)
            {
                TrailRendererDisableList.Clear();
            }

            if (WeaponTrailRendererDisableList.Count > 0)
            {
                WeaponTrailRendererDisableList.Clear();
            }
        }

        public void Init()
        {
            if(bInit)
            {
                return;
            }
            bInit = true;

        }

        public void Play()
        {
            if(!gameObject.activeInHierarchy)
            {
                return;
            }
           if(DisableTime < 0.03f)
           {
               Disabled();
               return;
           }

           waitplay = GameUtil.StartCoroutineHelper(this,WaitForTimeUp());
        }
        

        IEnumerator WaitForTimeUp()
        {
            yield return new WaitForSeconds(DisableTime);
            Disabled();
        }

        
        public void Stop()
        {
            ResetEnable();
            if(waitplay != null)
            {
                StopCoroutine(waitplay);
                waitplay = null;
            }
            
        }

        void ResetEnable()
        {
            if(!bInit)
            {
                return;
            }

            if (DisableList.Count > 0)
            {
                int cout = DisableList.Count;
                for (int i = 0; i < cout;i++ )
                {
                    DisableList[i].SetActive(true);
                }
            }
        }

        void Disabled()
        {
            if (!bInit)
            {
                return;
            }

            if(DisableList.Count > 0)
            {
                 foreach(GameObject go in DisableList)
                 {
                     go.SetActive(false);
                 }
            }

            if (TrailRendererDisableList.Count > 0)
            {
                foreach (TrailsSystem.Effect_TrailRenderer_Base go in TrailRendererDisableList)
                {
                    go.ClearSystem(false);
                }
            }

            if (WeaponTrailRendererDisableList.Count > 0)
            {
                foreach (Effect_WeaponTrailBase go in WeaponTrailRendererDisableList)
                {
                    go.Stop();
                }
            }

        }
    }
}


