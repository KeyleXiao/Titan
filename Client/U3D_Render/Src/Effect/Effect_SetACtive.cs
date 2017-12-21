using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

namespace Effect
{
    public struct ElaryStopNode
    {
        public List<UnityEngine.Object> ElaryStopObj;
        public List<Type> ElaryStopObjType;
    }

    // 在光效内添加，当光效到达时一部分先消失，另外一部分继续播放
    public class Effect_SetACtive : MonoBehaviour
    {
        public Transform[] DisableTransform;

        private bool bInit = false;

        private Dictionary<Transform, ElaryStopNode> ElaryStopMap = new Dictionary<Transform, ElaryStopNode>();
        private HashSet<Type> SupportTypes = new HashSet<Type>();


        private static Type ParticleSystem_Type = typeof(ParticleSystem);
        private static Type Animator_Type = typeof(Animator);
        private static Type MeshRenderer_Type = typeof(MeshRenderer);
        private static Type Effect_SmokePlume_Type = typeof(TrailsSystem.Effect_SmokePlume);
        private static Type Effect_SmokeTrail_Type = typeof(TrailsSystem.Effect_SmokeTrail);
        private static Type Effect_Trail_Type = typeof(TrailsSystem.Effect_Trail);
        private static Type Effect_WeaponTrail_Type = typeof(Effect_WeaponTrail);
        private static Type Effect_WeaponTrail_Simplest_Type = typeof(Effect_WeaponTrail_Simplest);

        private void Init()
        {
            if (bInit)
            {
                return;
            }

            //添加进去，不添加就不会起作用
            SupportTypes.Add(ParticleSystem_Type);
            SupportTypes.Add(Animator_Type);
            SupportTypes.Add(MeshRenderer_Type);
            //SupportTypes.Add(Effect_SmokePlume_Type);
            //SupportTypes.Add(Effect_SmokeTrail_Type);
            //SupportTypes.Add(Effect_Trail_Type);
            //SupportTypes.Add(Effect_WeaponTrail_Type);
            //SupportTypes.Add(Effect_WeaponTrail_Simplest_Type);

            foreach (Transform trans in DisableTransform)
            {
                if(!trans)
                {
                    continue;
                }
                List<ElaryStopNode> list = new List<ElaryStopNode>();
                ElaryStopNode elaryNode = default(ElaryStopNode);

                elaryNode.ElaryStopObj = new List<UnityEngine.Object>();
                elaryNode.ElaryStopObjType = new List<Type>();

                bool bParentRoot = true;
                foreach (Type t in SupportTypes)
                {
                    Component c = trans.GetComponent(t);
                    if(c)//如果都没有这些组件，那么，直接设置Active！
                    {
                        elaryNode.ElaryStopObj.Add(c);
                        elaryNode.ElaryStopObjType.Add(t);
                        bParentRoot = false;
                        
                    }
                }

                if (!bParentRoot)
                {
                    ElaryStopMap.Add(trans, elaryNode);
                    RenewElaryStopNode(elaryNode);
                }
                else
                {
                    trans.gameObject.SetActive(true);
                }

            }
            bInit = true;
        }

        public void disableActiveForDelayDestroy()
        {
            if (!bInit)
            {
                Init();
            }
            foreach (Transform trans in DisableTransform)
            {
                if(!trans)
                {
                    continue;
                }
                //找得到
                if (ElaryStopMap.ContainsKey(trans))
                {
                    DisabledElaryStopNode(ElaryStopMap[trans]);
                }
                else//找不到直接设active
                {
                    trans.gameObject.SetActive(false);
                }
               
            }
        }


        public void renewActiveForDelayDestroy()
        {
            if (!bInit)
            {
                Init();
            }
            foreach (Transform trans in DisableTransform)
            {
                if(!trans)
                {
                    continue;
                }
                //找得到
                if (ElaryStopMap.ContainsKey(trans))
                {
                    RenewElaryStopNode(ElaryStopMap[trans]);
                }
                else//找不到直接设active
                {
                    trans.gameObject.SetActive(true);
                }
            }
        }


        private void Clear()
        {
            ElaryStopMap.Clear();
            SupportTypes.Clear();
            bInit = false;
        }


        private void DisabledElaryStopNode(ElaryStopNode nod)
        {
            int count = nod.ElaryStopObj.Count;
            for (int i = 0; i < count; i++)
            {
                SetObjEnabled(nod.ElaryStopObj[i], nod.ElaryStopObjType[i], false);
            }
        }


        private void RenewElaryStopNode(ElaryStopNode nod)
        {
            int count = nod.ElaryStopObj.Count;
            for (int i = 0; i < count; i++)
            {
                SetObjEnabled(nod.ElaryStopObj[i], nod.ElaryStopObjType[i], true);
            }
        }


        private bool GetObjEnabled(UnityEngine.Object c, Type t)
        {
            return Execute(c,t,false);
        }


        private void SetObjEnabled(UnityEngine.Object c, Type t, bool en)
        {
            Execute(c, t, true, en);
        }


        /// <summary>
        /// 这个函数，我也不知道怎么命名，求大神优化,主要是根据第三个参数来取得，设置enabled的
        /// </summary>
        /// <param name="c"></param>
        /// <param name="t"></param>
        /// <param name="isSetEnabled">为true时，请无视返回值</param>
        /// <param name="enabled"></param>
        /// <returns></returns>
        private bool Execute(UnityEngine.Object c, Type t, bool isSetEnabled, bool enabled = false)
        {
            if (!SupportTypes.Contains(t) || !c)
            {
                return false;
            }

            if (ParticleSystem_Type == t) //直接继承Component的
            {
                ParticleSystem pe = c as ParticleSystem;
                if (isSetEnabled)
                {
                    ParticleSystem.EmissionModule emm = pe.emission;
                    emm.enabled = false;
                    return false;
                }
                else
                {
                    return pe.emission.enabled;
                }
            }
            else if (MeshRenderer_Type == t)//直接继承Component的
            {
                MeshRenderer pe = c as MeshRenderer;
                if (isSetEnabled)
                {
                    pe.enabled = enabled;
                    return false;
                }
                else
                {
                    return pe.enabled;
                }
            }
            else //直接继承Behaviour的
            {
                Behaviour pe = c as Behaviour;
                if (isSetEnabled)
                {
                    pe.enabled = enabled;
                    return false;
                }
                else
                {
                    return pe.enabled;
                }
            }
        }


        public void OnDestroy()
        {
            Clear();
        }
    }
}

