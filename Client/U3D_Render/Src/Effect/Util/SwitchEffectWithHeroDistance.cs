/*******************************************************************
** 文件名:	SwitchEffectWithHeroDistance.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/6/9
** 版  本:	1.0
** 描  述:	根据英雄距离切换Effect
** 应  用:  	根据英雄距离切换Effect

**************************** 修改记录 ******************************
** 修改人:  昔文博
** 日  期:  2016/1/27
** 描  述:  添加Effect初始化操作
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

namespace Effect
{
    /// <summary>
    /// 根据英雄距离切换Effect
    /// </summary>
    public class SwitchEffectWithHeroDistance : MonoBehaviour
    {
        [System.Serializable]
        public class SwitchEffectNode
        {
            [System.NonSerialized]
            public EffectNode effect;
            [System.NonSerialized]
            public ResNode res;

            public string guid;
            public float ShowDistance;


        }

        public bool HideInEmeny = false;
        public bool HideInFriend = false;

        public List<SwitchEffectNode> SwitchList = new List<SwitchEffectNode>();

        public float MaxShowDistahce = 0.0f;

        private WaitForSeconds sections = new WaitForSeconds(0.05f);

        private bool bNeedToUpdate = true;
        private bool bInit = false;

        private float currentDistance = 0.0f;

        private int OldDistanceLevel = -1;

        private BaseStateMachine m_pFSM = null;

        U3D_Render.EntityView m_entityView = null;
        Coroutine cheCor = null;
        // Use this for initialization
        public IEnumerator Start()
        {
            if (SwitchList.Count <= 0)
            {
                bNeedToUpdate = false;
            }

            InitAll();

            if (!SceneManager.isLoadingScene)
            {
                DisableAll();
                m_pFSM = GetComponentInParent<BaseStateMachine>();
                if (!m_pFSM)
                {
                    enabled = false;
                    yield break;
                }
                m_entityView = EntityFactory.getEntityViewByID(m_pFSM.entityID);
            }
            else
            {
                //bNeedToUpdate = false;
                GameUtil.StartCoroutineHelper(this, PreWarmEffect());
            }
            bInit = true;
            yield return sections;
        }

        public void ResetLayer()
        {
            if (null != SwitchList && SwitchList.Count > 0)
            {
                for (int i = 0; i < SwitchList.Count; i++)
                {
                    SwitchEffectNode n = SwitchList[i];
                    if (n.effect)
                    {
                        GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerLightingEffectOnly), n.effect.gameObject, true);
                    }
                }
            }
        }

        public void DisableAll()
        {
            if (!bInit)
                return;

            for (int i = 0; i < SwitchList.Count; i++)
            {
                SwitchEffectNode n = SwitchList[i];
                if (n.effect)
                {
                    n.effect.StopEffect();
                }
            }
            OldDistanceLevel = -1;
        }

        public void Clear()
        {
            for (int i = 0; i < SwitchList.Count; i++)
            {
                SwitchEffectNode n = SwitchList[i];
                AssetBundleManager.DeleteAssets(ref n.res, true);
            }
        }

        private void InitAll()
        {
            for (int i = 0; i < SwitchList.Count; i++)
            {
                SwitchEffectNode n = SwitchList[i];
                n.res = AssetBundleManager.GetAssets(AssetType.Asset_Prefab, n.guid, true);
                if (null != n.res && n.res.IsValid())
                {
                    AssetBundleManager.SaveMemory(n.res);
                    n.effect = n.res.InstanceMainRes<Effect.EffectNode>();
                    SwitchList[i] = n;
                    if (n.effect)
                    {
                        n.effect.transform.SetParent(transform);
                        n.effect.transform.localPosition = Vector3.zero;

                        n.effect.isStaticEffect = false;
                        n.effect.PlayOnAwake = false;
                        n.effect.Init();
                    }
                    else
                    {
                        Debug.LogWarning("SwitchEffectWithHeroDistance有非特效结点，请[--策划--]检查皮肤预制体:" + transform.parent.name);
                    }
                }
                else
                {
                    Debug.LogWarning("SwitchEffectWithHeroDistance有特效加载失败，原因可能是预制体为空或者预制体加载失败，请[--策划--]检查皮肤预制体:" + transform.parent.name);
                }

            }
        }

        IEnumerator PreWarmEffect()
        {
            yield return new WaitForSeconds(1.0f);
            bNeedToUpdate = true;
            DisableAll();
        }

        IEnumerator CheckDistance()
        {
            while (true)
            {
                if (bInit && bNeedToUpdate)
                {
                    UpdateEffect();
                }

                yield return sections;
            }
        }


        public void SetEntitiyView(U3D_Render.EntityView ev)
        {
            if (null != cheCor)
            {
                StopCoroutine(cheCor);
            }

            cheCor = GameUtil.StartCoroutineHelper(this, CheckDistance());
            m_entityView = ev;
        }

        public void UpdateEffect()
        {
            if (EntityFactory.MainHero && m_entityView != null)
            {
                currentDistance = Vector3.SqrMagnitude(transform.position - EntityFactory.MainHero.transform.position);

                if (HideInEmeny && HideInFriend)
                {
                    DisableAll();
                }
                else if (HideInEmeny && !HideInFriend)
                {
                    if (null != m_entityView)
                    {
                        if (m_entityView.CampFlag == SPELL.CampFlag.CampFlag_Friend ||
                            m_entityView.CampFlag == SPELL.CampFlag.CampFlag_Self)
                        {
                            CacluLevel();
                        }
                        else
                        {
                            DisableAll();
                        }
                    }
                }
                else if (HideInFriend && !HideInEmeny)
                {
                    if (null != m_entityView)
                    {
                        if (m_entityView.CampFlag != SPELL.CampFlag.CampFlag_Friend &&
                            m_entityView.CampFlag != SPELL.CampFlag.CampFlag_Self
                            )
                        {
                            CacluLevel();
                        }
                        else
                        {
                            DisableAll();
                        }
                    }
                }
                else
                {
                    CacluLevel();
                }
            }
        }

        private void CacluLevel()
        {
            if (currentDistance < MaxShowDistahce * MaxShowDistahce)
            {
                bool bShowMin = true;
                for (int i = 0; i < SwitchList.Count; i++)
                {
                    float dis = SwitchList[i].ShowDistance * SwitchList[i].ShowDistance;
                    if (currentDistance > dis)
                    {
                        if (OldDistanceLevel != i)
                        {
                            if (OldDistanceLevel >= 0)
                            {
                                if (SwitchList[OldDistanceLevel].effect)
                                {
                                    SwitchList[OldDistanceLevel].effect.StopEffect();
                                }
                            }
                            if (SwitchList[i].effect)
                            {
                                SwitchList[i].effect.PlayEffect(false, -1, Vector3.zero, Vector3.zero);
                            }
                            OldDistanceLevel = i;
                        }
                        bShowMin = false;
                        break;
                    }
                }

                if (bShowMin)
                {
                    for (int i = 0; i < SwitchList.Count - 1; i++)
                    {
                        if (SwitchList[i].effect)
                        {
                            SwitchList[i].effect.StopEffect();
                        }

                    }
                    if (OldDistanceLevel != SwitchList.Count - 1)
                    {
                        OldDistanceLevel = SwitchList.Count - 1;
                        if (SwitchList[OldDistanceLevel].effect)
                        {
                            SwitchList[OldDistanceLevel].effect.PlayEffect(false, -1, Vector3.zero, Vector3.zero);
                        }

                    }

                }
            }
            else
            {
                DisableAll();
                OldDistanceLevel = -1;
            }
        }
    }
}

