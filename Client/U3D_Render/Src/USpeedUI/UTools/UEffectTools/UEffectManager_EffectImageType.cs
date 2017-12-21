/*******************************************************************
** 文件名:	UEffectManager_EffectImageType.cs 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	林建聪
** 日  期:	2016-6-28
** 版  本:	1.0
** 描  述:	全屏效果(UEffectManager_EffectImageType)
            
            一、需要新增全屏效果：
                1、找客户端给全屏效果的类名
                
            二、使用
                1、单独使用
                    UEffectParamBase param = new UEffectImageParam(typeof(_eAttachImageEffect:typeof(ImageEffects.BlurAndDownScreen),gameobject));
                    a、创建 UEffectManager.CreateEffect(UEffectType.UET_EffectImage,ref param);
                    b、移除 UEffectManager.DestroyEffect(UEffectType.UET_EffectImage,ref param);
                2、View使用
	                a、显示/隐藏View的时候使用默认效果ImageEffects.BlurAndDownScreen;  View.canUseImageEffect = true;
                    b、如果需要在显示/隐藏的时候用另一种效果 View.imageEffect = typeof(另一种效果类);
       
********************************************************************/
using ImageEffects;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace USpeedUI.UEffect
{
    public partial class UEffectManager
    {
        protected Dictionary<Type, Transform> m_EffectImageTypeList = new Dictionary<Type, Transform>();
        protected bool CreateImageEffect(UEffectImageParam _param)
        {
            if(_param.AttachImageEffect != null && !m_EffectImageTypeList.ContainsKey(_param.AttachImageEffect))
            {
                ImageEffectManager.API_EnableImageEffect(_param.AttachImageEffect);
                m_EffectImageTypeList.Add(_param.AttachImageEffect,_param.AttachParent);
                return true;
            }
            
            return false;
        }
        protected bool DestroyImageEffect(UEffectImageParam _param)
        {
            if (_param.AttachImageEffect != null && m_EffectImageTypeList.ContainsKey(_param.AttachImageEffect) && m_EffectImageTypeList[_param.AttachImageEffect] == _param.AttachParent)
            {
                ImageEffectManager.API_DisableImageEffect<BlurAndDownScreen>();
                m_EffectImageTypeList.Remove(_param.AttachImageEffect);
                return true;
            }
            return false;
        }
    }
}
