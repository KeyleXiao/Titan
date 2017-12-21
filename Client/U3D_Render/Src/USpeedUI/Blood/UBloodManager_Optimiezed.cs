using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;
using UnityEngine.UI;
using U3D_Render;

namespace USpeedUI.Blood
{
    public partial class UBloodManager : MonoBehaviour
    {
        private int[] CacheCountArry = new int[3] { 10, 30, 100 }; 
        private bool bPreWarmBlood = false;

        //预热血条
        public void PreWarmBlood()
        {
            if (bPreWarmBlood)
            {
                return;
            }

            bPreWarmBlood = true;

            for(int i = 0; i < (int)BloodStyleType.Boss; i++)
            {
                BloodStyleType eType = (BloodStyleType)i;

                if (!m_CacheBloodMap.ContainsKey(eType))
                {
                    m_CacheBloodMap.Add(eType, new Queue<UBlood>());
                }

                Queue<UBlood> BloodList = m_CacheBloodMap[eType];
                BloodList.Clear();

                for (int n = 0; n < CacheCountArry[i]; n++)
                {
                    BloodCanvasNode canvNode = new BloodCanvasNode();
                    if (!AllocBloodPos(eType, ref canvNode))
                    {
                        continue;
                    }
                    UBlood ub = InstanceUBlood(eType, canvNode);
                    if (null != ub)
                    {
                        ub.Init(null, 0, eType, canvNode);
                        ub.ChangeParent(CacheBloodRoot.transform);
                        ub.SetEnable(false);
                        BloodList.Enqueue(ub);
                    }
                }
            }

            if (EntityFactory.MainHero != null)
                bossBloodTarget = EntityFactory.MainHero.transform;
        }

        private UBlood GetBlood(BloodStyleType type, ref BloodCanvasNode canvNode)
        {
            UBlood ub = null;
            if (!bPreWarmBlood)
            {
                if (!AllocBloodPos(type, ref canvNode))
                {
                    return null;
                }
                ub = InstanceUBlood(type, canvNode);
                return ub;
            }
            if (!m_CacheBloodMap.ContainsKey(type))
            {
                if (!AllocBloodPos(type, ref canvNode))
                {
                    return null;
                }
                ub = InstanceUBlood(type, canvNode);
                return ub;
            }
            else
            {
                if (!m_CacheBloodMap.ContainsKey(type))
                {
                    if (!AllocBloodPos(type, ref canvNode))
                    {
                        return null;
                    }
                    ub = InstanceUBlood(type, canvNode);
                    return ub;
                }
                Queue<UBlood> m_CacheList = m_CacheBloodMap[type];
                if (m_CacheList.Count > 0)
                {
                    ub = m_CacheList.Dequeue();
                    canvNode = ub.CanvasNode;
                    ub.RefreshTexture();
                }
                else
                {
                    if (!AllocBloodPos(type, ref canvNode))
                    {
                        return null;
                    }
                    ub = InstanceUBlood(type, canvNode);
                }
            }
            return ub;
        }

        //清除缓存
        private void ClearCache()
        {
            if (null != m_CacheBloodMap)
            {
                foreach (Queue<UBlood> lis in m_CacheBloodMap.Values)
                {
                    if (null != lis)
                    {
                        foreach (UBlood ub in lis)
                        {
                            DestroyBlood(ub);
                        }
                        lis.Clear();
                    }
                }
            }
            //if (null != CacheBloodRoot)
            //{
            //    GameObject.DestroyImmediate(CacheBloodRoot);
            //}
            bPreWarmBlood = false;
        }

        //缓存血条
        private void CacheBlood(UBlood ub)
        {
            if (null == ub)
            {
                return;
            }

            if (!m_CacheBloodMap.ContainsKey(ub.BloodType))
            {
                DestroyBlood(ub);
                return;
            }

            if (ub.ISHero)
                return;

            if (m_BloodMap.ContainsKey(ub.ID))
            {
                ub.SetEnable(false);
                ub.ChangeBloodColor(false);
                ub.ChangeParent(CacheBloodRoot.transform);
                Queue<UBlood> m_list = m_CacheBloodMap[ub.BloodType];
                m_list.Enqueue(ub);
                m_BloodMap.Remove(ub.ID);
            }
            else
            {
                DestroyBlood(ub);
            }
        }

        private void InitCache()
        {
            if(!CacheBloodRoot)
            {
                CacheBloodRoot = new GameObject();
                CacheBloodRoot.name = "BloodCacheRoot";
                DontDestroyOnLoad(CacheBloodRoot);
            }
        }

        private GameObject CacheBloodRoot = null;
        private Dictionary<BloodStyleType, Queue<UBlood>> m_CacheBloodMap = new Dictionary<BloodStyleType, Queue<UBlood>>();
    }
}
