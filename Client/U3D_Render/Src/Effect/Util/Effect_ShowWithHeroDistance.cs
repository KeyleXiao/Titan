///*******************************************************************
//** 文件名:	Effect_ShowWithHeroDistance.cs
//** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
//** 创建人:	谭强均
//** 日  期:	2015/6/9
//** 版  本:	1.0
//** 描  述:	特效功能脚本
//** 应  用:  	根据英雄距离显示/隐藏的特效

//**************************** 修改记录 ******************************
//** 修改人: 
//** 日  期: 
//** 描  述: 
//********************************************************************/
//using UnityEngine;
//using System.Collections;
//using System.Collections.Generic;

//namespace Effect
//{
//public class Effect_ShowWithHeroDistance : MonoBehaviour
//{
//    public float ShowDistahce = 0.0f;

//    EffectNode m_Node = null;
//    private WaitForSeconds sections = new WaitForSeconds(0.05f);
//    private float currentDistance = 0.0f;

//    /// <summary>
//    /// 1是显示，0是隐藏
//    /// </summary>
//    int currentStatus = 0;

//    int OldStatus = -1;

//    // Use this for initialization
//    public void Start()
//    {
//        m_Node = GetComponent<EffectNode>();
//        GameUtil.StartCoroutineHelper(this,CheckDistance());
//    }


//    IEnumerator CheckDistance()
//    {
//        while (true)
//        {
//            if (m_Node)
//            {
//                if (EntityFactory.MainHero)
//                {
//                    currentDistance = Vector3.SqrMagnitude(transform.position - EntityFactory.MainHero.transform.position);
//                    if (currentDistance > ShowDistahce * ShowDistahce)
//                    {
//                        currentStatus = 1;
//                        if (OldStatus != currentStatus)
//                        {
//                            OldStatus = currentStatus;
//                            m_Node.StopEffect();
//                        }

//                    }
//                    else
//                    {
//                        currentStatus = 0;
//                        if (OldStatus != currentStatus)
//                        {
//                            OldStatus = currentStatus;
//                            m_Node.PlayEffect(false,-1,Vector3.zero,Vector3.zero);
//                        }

//                    }
//                }
//            }

//            yield return sections;
//        }
//    }
//}
//}

