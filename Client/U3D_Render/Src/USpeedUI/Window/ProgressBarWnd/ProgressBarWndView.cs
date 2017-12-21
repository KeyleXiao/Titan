/*******************************************************************
** 文件名:	ProgressBarWndView.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈言蒙
** 日  期:	2016-6-15
** 版  本:	1.0
** 描  述:  进度条统一管理
		
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
using USpeedUI.ProgressBarPart;
using DG.Tweening;
using USpeedUI.UEffect;

namespace USpeedUI
{
     public class SkillProgressBar:TimeCountDown
     {
         public Slider sliderBar;
         public GameObject moveObj;
        /// <summary>
        /// 效果挂接点
        /// </summary>
        public RectTransform ProgressBarEffectAttachPoint;
        protected UEffectParamBase ProgressBarEffect = null;

        protected override void CountDownRepeatCallBack()
         {
             base.CountDownRepeatCallBack();

             sliderBar.value = 1-(fCurTime / this.fOriginalTime);
         }

         protected override void CountDownCompleteCallBack()
         {
             base.CountDownCompleteCallBack();

             sliderBar.value = 0;
            this.gameObject.SetActive(false);
        }

        public void StopWarSkillProgress()
         {
             CountDownCompleteCallBack();
         }

         public void DoBarGameObjectMove(EBarObjMoveType eType, float fTime)
         {
             if (fTime <= 0)
             {
                 return;
             }

             if(moveObj == null)
             {
                 return;
             }

             this.gameObject.SetActive(true);
             float fTotalLen = moveObj.GetComponent<RectTransform>().sizeDelta.x;

             float fCurPosition = fTotalLen;
             float fCurMoveTime = fTime;
             fCurPosition = eType == EBarObjMoveType.EBOMT_LEFT ? fCurPosition * -1 : fCurPosition;
             fCurMoveTime = fCurMoveTime <= 0 ? 1 : fCurMoveTime;

            if(ProgressBarEffect == null)
            {
                ProgressBarEffect = new UEffectPrefabParam(UEffectPrefabType.UEPT_ProgressBarWndView_DuTiao, ProgressBarEffectAttachPoint);
                if (UEffectManager.Instance.CreateEffect(UEffectType.UET_EffectPrefab, ref ProgressBarEffect) == false)
                {
                    ProgressBarEffect = null;
                }
            }
                

             moveObj.transform.DOPause();
             moveObj.transform.DOLocalMoveX(0, fCurMoveTime);
         }

         public void DoBarGameObjectStop()
         {
             if (moveObj == null)
             {
                 return;
             }

             moveObj.GetComponent<RectTransform>().anchoredPosition = new Vector3(-moveObj.GetComponent<RectTransform>().sizeDelta.x, 0, 0);
             moveObj.transform.DOPause();
            if (ProgressBarEffect != null)
            {
                if (UEffectManager.Instance.DestroyEffect(UEffectType.UET_EffectPrefab, ref ProgressBarEffect))
                {
                    ProgressBarEffect = null;
                }
            }
            this.gameObject.SetActive(false);
         }
     }

    public class ProgressBarWndView : UIBaseWndView
    {
        ProgressBarWnd ProgressBarWnd;
        public SkillProgressBar skillMainProgressBar;
        public SkillProgressBar skillWingProgressBar;

        public override void Start()
        {
            if (m_wnd == null)//测试用
            { 
            }
            else
            {
                ProgressBarWnd = (ProgressBarWnd)m_wnd;
            }

            StopWarSkillProgress();
            StopWarWingProgress();
            return;
        }
		//临时测试
        public bool test = false;
        public int time = 6;
        public override void Update()
        {
            base.Update();
            if(test)
            {
                test = false;
                SetWarSkillProgress(time);
            }
        }

        public void SetWarSkillProgress(int nTime)
        {
            //StopWarSkillProgress();
            //skillMainProgressBar.gameObject.SetActive(true);
            //skillMainProgressBar.StartCountDown((float)nTime / 1000, 0.1f, 0.1f);
            StopWarSkillProgress();
            skillMainProgressBar.DoBarGameObjectMove(EBarObjMoveType.EBOMT_RIGHT,(float)nTime / 1000);
        }

        public void StopWarSkillProgress()
        {
            skillMainProgressBar.DoBarGameObjectStop();
        }

        public void SetWingProgress(int nTime)
        {
            //StopWarWingProgress();
            //skillWingProgressBar.gameObject.SetActive(true);
            //skillWingProgressBar.StartCountDown((float)nTime / 1000, 0.1f, 0.1f);
            StopWarWingProgress();
            skillWingProgressBar.DoBarGameObjectMove(EBarObjMoveType.EBOMT_RIGHT, (float)nTime / 1000);
        }

        public void StopWarWingProgress()
        {
            skillWingProgressBar.DoBarGameObjectStop();
        }
    }
}