/*******************************************************************
** 文件名:	Effect_Scale.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	缩放特效

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
    public class Effect_Scale : MonoBehaviour,ISaveSerializedData
    {


        /// <summary>
        /// 只对粒子系统有效，模型无效
        /// </summary>
        public bool IncludeChildren = false;
        public ScaleType scaleType = ScaleType.Model;
        public ParticleSystemScaleType particleSystemScaleType = ParticleSystemScaleType.LocalSpaceScale;

        public ParticleSystem[] allParticleSystem = null;

        private List<float> orginalScale = new List<float>();

        public void Bake()
        {
            if(scaleType == ScaleType.ParitcleSystem)
            {
                if(IncludeChildren)
                {
                    allParticleSystem = GetComponentsInChildren<ParticleSystem>();
                }
                else
                {
                    allParticleSystem = GetComponents<ParticleSystem>();
                }
            }
        }

        public System.Object SaveSerializedData()
        {
            List<float> orginalScaleData = new List<float>();
            if(scaleType == ScaleType.ParitcleSystem)
            {
                foreach(ParticleSystem ps in allParticleSystem)
                {
                    orginalScaleData.Add(ps.transform.localScale.x);
                }
            }
            else if(scaleType == ScaleType.Model)
            {
                orginalScaleData.Add(transform.localScale.x);
            }

            return orginalScaleData;
        }

        public void SetSerializedData(System.Object data)
        {
            List<float> orginalScaleData = data as List<float>;
            if(null == orginalScaleData)
            {
                Debug.LogError("Effect_Scale-- ConverSerializedData faild");
                return;
            }

            if(orginalScaleData.Count <= 0)
            {
                return;
            }
            if (scaleType == ScaleType.ParitcleSystem)
            {
                if (orginalScaleData.Count != allParticleSystem.Length)
                {
                    Debug.LogError("Effect_Scale-- allParticleSystem not match");
                    return;
                }

                for (int i = 0; i < orginalScaleData.Count; i++)
                {
                    allParticleSystem[i].transform.localScale = orginalScaleData[i] * Vector3.one;
                }

            }
            else if (scaleType == ScaleType.Model)
            {
                transform.localScale = orginalScaleData[0] * Vector3.one;
            }

        }

        public void Init()
        {
            if (scaleType == ScaleType.ParitcleSystem)
            {
                if (allParticleSystem.Length <= 0)
                {
                    return;
                }
                foreach (ParticleSystem ps in allParticleSystem)
                {
                    orginalScale.Add(ps.transform.localScale.x);
                    if(particleSystemScaleType == ParticleSystemScaleType.LocalSpaceScale)
                    {
                        ps.scalingMode = ParticleSystemScalingMode.Local;
                    }
                    else  if(particleSystemScaleType == ParticleSystemScaleType.WoldSpaceScale)
                    {
                        ps.scalingMode = ParticleSystemScalingMode.Hierarchy;
                    }
                    else if (particleSystemScaleType == ParticleSystemScaleType.ShapeScale)
                    {
                        ps.scalingMode = ParticleSystemScalingMode.Shape;
                    }
                }
            }
            else if (scaleType == ScaleType.Model)
            {
                orginalScale.Add(transform.localScale.x);
            }

        }

        public void ResetScale()
        {
            if (scaleType == ScaleType.ParitcleSystem)
            {
                if(allParticleSystem.Length <= 0)
                {
                    return;
                }
                for (int i = 0; i < orginalScale.Count; i++)
                {
                    allParticleSystem[i].transform.localScale = orginalScale[i] * Vector3.one;
                }

            }
            else if (scaleType == ScaleType.Model)
            {
                transform.localScale = orginalScale[0] * Vector3.one;
            }
        }

        public void ApplyScale(float scale)
        {
            if (scaleType == ScaleType.ParitcleSystem)
            {
                if (allParticleSystem.Length <= 0)
                {
                    return;
                }
                for (int i = 0; i < allParticleSystem.Length; i++)
                {
                    allParticleSystem[i].transform.localScale = scale * Vector3.one;
                }

            }
            else if (scaleType == ScaleType.Model)
            {
                transform.localScale = scale * Vector3.one;
            }
        }
    }
}


