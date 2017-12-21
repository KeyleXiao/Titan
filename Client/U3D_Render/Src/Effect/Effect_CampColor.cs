/*******************************************************************
** 文件名:	Effect_CampColor.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2015/08/04
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	增加材质球

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using ImageEffects;
namespace Effect
{


    /// <summary>
    /// 阵营颜色
    /// </summary>
    public class Effect_CampColor : MonoBehaviour, ISaveSerializedData
    {

        public string ColorName = "_TintColor";

        public Color EnemyColor = new Color(0.97f,0.16f,0.05f,1.0f);
        public Color FriendColor = new Color(0.05f,0.47f,0.95f,1.0f);

        /// <summary>
        /// 添加的材质球实例
        /// </summary>
        private List<Material> matInstances = new List<Material>();
        private Renderer[] m_Renders;
        private Projector[] m_Projectors;

        private bool bPlay = false;
        private bool bInit = false;

        public System.Object SaveSerializedData()
        {
            m_Projectors = GetComponents<Projector>();
            List<Material> matlist = new List<Material>();
            foreach (Projector pro in m_Projectors)
            {
                matlist.Add(pro.material);
            }
            m_Projectors = null;
            return matlist;
        }

        public void SetSerializedData(System.Object data)
        {
            if (null == data)
            {
                return;
            }
            List<Material> matlist = data as List<Material>;
            if (null == data)
            {
                Debug.LogError("Effect_CampColor-- ConverSerializedData faild");
                return;
            }
            m_Projectors = GetComponents<Projector>();

            if (matlist.Count != m_Projectors.Length)
            {
                Debug.LogError("Effect_CampColor-- orginalMaterialMap not match");
                return;
            }
            for (int i = 0; i < m_Projectors.Length; i++)
            {
                m_Projectors[i].material = matlist[i];
            }

            m_Projectors = null;
        }

        public void OnDestroy()
        {
            matInstances.Clear();
            matInstances = null;
            m_Renders = null;
            m_Projectors = null;
        }

        public void Init()
        {
            if(bInit)
            {
                return;
            }
            bInit = true;
            m_Renders = GetComponents<Renderer>();
            m_Projectors = GetComponents<Projector>();
            GenrateMaterialInstance();
            
        }

        public void Play(bool isEmeny)
        {
            if (!bInit)
            {
                return;
            }

            bPlay = true;
            if(isEmeny)
            {
                Play_Enemy();
            }
            else
            {
                Play_Friend();
            }
        }

        public void Stop()
        {
            if (!bInit)
            {
                return;
            }

            StopAllCoroutines();
            bPlay = false;
        }


        /// <summary>
        /// 计算材质球实例，如果有多个render，会返回多个材质球实例
        /// </summary>
        /// <returns></returns>
        private void GenrateMaterialInstance()
        {
            if (!bInit)
            {
                return ;
            }
            if (matInstances.Count <= 0)
            {
                //在Render上增加一个材质球
                foreach (Renderer r in m_Renders)
                {
                    if (!r)
                    {
                        continue;
                    }
                    matInstances.Add(r.material);
                }

                if (m_Projectors.Length > 0)
                {
                    foreach (Projector r in m_Projectors)
                    {
                        if (!r.material)
                        {
                            continue;
                        }
                        if (!r.material.name.EndsWith("(Instance)"))
                        {
                            r.material = new Material(r.material) { name = r.material.name + " (Instance)" };
                        }
                        matInstances.Add(r.material);
                    }

                }
            }
        }

        private void Play_Enemy()
        {
            if(matInstances == null || matInstances.Count < 0)
            {
                return;
            }
            foreach (Material mat in matInstances)
            {
                mat.SetColor(ColorName, EnemyColor);
            }

        }

        private void Play_Friend()
        {
            if (matInstances == null || matInstances.Count < 0)
            {
                return;
            }
            foreach (Material mat in matInstances)
            {
                mat.SetColor(ColorName, FriendColor);
            }

        }

    }
}

