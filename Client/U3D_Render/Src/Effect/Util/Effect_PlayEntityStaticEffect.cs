/*******************************************************************
** 文件名:	Effect_PlayEntityStaticEffect.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2016/4/12
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	播放预制体上的特效

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
    public class Effect_PlayEntityStaticEffect : MonoBehaviour
    {
        [System.Serializable]
        public class StaticEffectNode
        {
            [System.NonSerialized]
            public EffectNode effect;
            [System.NonSerialized]
            public Transform effectTransform;

            public int PlayTimes;
            public Transform fllowBone;
            public Vector3 PosOffset;
            public Vector3 AngleOffset;
            public string guid;



            [System.NonSerialized]
            public ResNode res;
        }

        public List<StaticEffectNode> EffectList = new List<StaticEffectNode>();

        private bool bInit = false;
        private bool isPlaying = false;
        U3D_Render.EntityView LastEv = null;

        private Vector3 lastPos = Vector3.zero;
        private Quaternion lastRot = Quaternion.identity;
        private List<int> currPlayTimesList = new List<int>();
        private Transform m_Trans;

        public void Start()
        {
            m_Trans = transform;
        }

        public void Clear()
        {
            if (!bInit)
            {
                return;
            }
            Effect.EffectNode.UnResigterEffectQualityChange(OnEffectQualityChange);
            int cout = EffectList.Count;
            for (int i = 0; i < cout; i++)
            {
                ResNode temp = EffectList[i].res;
                AssetBundleManager.DeleteAssetsAsync(ref temp, ResLoadCallBack,true);
            }
            EffectList.Clear();
            bInit = false;
        }

        public void OnDestroy()
        {
            Clear();
        }

        public void ResetLayer()
        {
            if (null != EffectList && EffectList.Count > 0)
            {
                int cout = EffectList.Count;
                for (int i = 0; i < cout; i++)
                {
                    StaticEffectNode n = EffectList[i];
                    if (n.effect)
                    {
                        GameUtil.SetLayer(LayerMask.NameToLayer(Config.LayerLightingEffectOnly), n.effect.gameObject, true);
                    }
                }
            }
        }

        void ResLoadCallBack(ResConfigData cof, ResNode res, System.Object UserDataObj)
        {
            StaticEffectNode n = UserDataObj as StaticEffectNode;
            if(n == null)
            {
                Debug.LogError("Effect_PlayEntityStaticEffect -- 重大bug");
                return;
            }
            n.res = res;
            if (null != n.res && n.res.IsValid())
            {
                n.effect = n.res.InstanceMainRes<Effect.EffectNode>();
                if (n.effect)
                {
                    n.effect.isStaticEffect = false;
                    n.effect.PlayOnAwake = false;
                    n.effect.Init();
                    n.effect.transform.SetParent(LightingEffectFactory.Instance.StaticEffectRoot.transform);
                    n.effect.transform.SetPosition(transform.position);
                    n.effect.transform.rotation = transform.rotation;

                    lastPos = transform.position;
                    lastRot = transform.rotation;

                    n.effectTransform = n.effect.transform;
                    ResetLayer();

                    if(isPlaying)
                    {
                        Play(LastEv);
                    }

                }
                else
                {
                    Debug.LogWarning("Effect_PlayEntityStaticEffect有非特效结点，请[--策划--]检查皮肤预制体:" + transform.parent.name);
                }
            }
            else
            {
                Debug.LogWarning("Effect_PlayEntityStaticEffect有特效加载失败，原因可能是预制体为空或者预制体加载失败，请[--策划--]检查皮肤预制体:" + transform.parent.name);
            }
        }

        public void Init()
        {
            if (bInit)
            {
                return;
            }
            if (null != EffectList && EffectList.Count > 0)
            {
                isPlaying = false;
                Effect.EffectNode.ResigterEffectQualityChange(OnEffectQualityChange);
                int cout = EffectList.Count;
                for (int i = 0; i < cout; i++)
                {
                    currPlayTimesList.Add(0);
                    StaticEffectNode n = EffectList[i];
                    n.res = AssetBundleManager.GetAssetsAsync(AssetType.Asset_Prefab, ResLoadCallBack,n.guid, true, n);
                    if (null == n.res)
                    {
                        Debug.LogWarning("Effect_PlayEntityStaticEffect有特效加载失败，原因可能是预制体为空或者预制体加载失败，请[--策划--]检查皮肤预制体:" + transform.parent.name);
                    }
                }
            }
            bInit = true;
        }

        public void OnEffectQualityChange()
        {
            if (isPlaying)
            {
                if (null != EffectList && EffectList.Count > 0)
                {
                    int cout = EffectList.Count;
                    for (int i = 0; i < cout; i++)
                    {
                        currPlayTimesList[i]--;
                    }
                }
                Stop();
                Play(LastEv);
            }

        }

        public void Play(U3D_Render.EntityView ev )
        {
            if (!bInit)
            {
                return;
            }
            isPlaying = true;
            if(ev != null)
            {
                LastEv = ev;
            }
         
            if (null != EffectList && EffectList.Count > 0)
            {
                int cout = EffectList.Count;
                for (int i = 0; i < cout; i++)
                {
                    StaticEffectNode n = EffectList[i];
                    if(n.PlayTimes > 0)
                    {
                        currPlayTimesList[i]++;
                        if (currPlayTimesList[i] > n.PlayTimes)
                        {
                            continue;
                        }
                    }
                    
                    if (n.effect)
                    {
                        n.effect.PlayEffect(false,LastEv == null ? -1 : LastEv.ID, null, null);
                    }
                }
            }
        }

        public void Stop()
        {
            if (!bInit || !isPlaying)
            {
                return;
            }
            isPlaying = false;
            if (null != EffectList && EffectList.Count > 0)
            {
                int cout = EffectList.Count;
                for (int i = 0; i < cout; i++)
                {
                    StaticEffectNode n = EffectList[i];
                    if (n.effect)
                    {
                        n.effect.StopEffect();
                    }
                }
            }
        }

        public void LateUpdate()
        {
            if (isPlaying)
            {
                if(null != EffectList && EffectList.Count > 0)
                {
                    int cout = EffectList.Count;
                    for(int i = 0; i < cout;i++)
                    {
                        StaticEffectNode nod = EffectList[i];
                        if (nod.effect)
                        {
                            if (nod.fllowBone)
                            {
                                nod.effectTransform.SetPosition(nod.fllowBone.position + nod.PosOffset);
                                Vector3 ang = nod.fllowBone.rotation.eulerAngles;
                                ang += nod.AngleOffset;
                                nod.effectTransform.rotation = Quaternion.Euler(ang);
                            }
                            else
                            {
                                Vector3 disPos = lastPos - m_Trans.position;
                                if (disPos.sqrMagnitude > 0.0001f)
                                {
                                    nod.effectTransform.SetPosition(m_Trans.position);
                                    lastPos = transform.position;
                                }
                                nod.effectTransform.rotation = m_Trans.rotation;
                                
                            }
                        }

                    }
                }
            }
        }

    }
}

