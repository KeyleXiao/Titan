using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace U3D_Render.SkinCtrl
{
    public partial class PersonSkinControl : EntitySkinControl
    {
        private void CheckMainHeroOcclusion()
        {
            if (currentSkinId == InvalidSkinID)
            {
                return;
            }

            if (!isActiveAndEnabled)
            {
                return;
            }

            if (null == CurrentSkinInstance || null == CurrentSkinInstance.entitySkinConfig)
            {
                return;
            }

            if (!CurrentSkinInstance.entitySkinConfig.OptimizedNearMainHero)
            {
                return;
            }

            if (null == m_machine || null == m_machine.creature || m_machine.isHero)
            {
                return;
            }

            U3D_Render.EntityView mainEv = EntityFactory.getMainPlayer();
            if (null == mainEv || null == mainEv.gameObject)
                return;

            float dragonOptimitzedDistance = 7.0f;  //策划控制
            //检测距离，如果距离太近，不降低透明度
            float dis = CaclDistance(mainEv.gameObject.transform.position, m_machine.creature.position);
            if (dis < dragonOptimitzedDistance)
            {
                OptimizeAlpha(true);
            }
            else
                OptimizeAlpha(false);
        }

        private void OptimizeAlpha(bool bNearToMainView)
        {
            if (CurrentSkinInstance != null)
            {
                if (bNearToMainView)
                {
                    CurrentSkinInstance.SetAlpha(0.5f);
                }
                else
                {
                    CurrentSkinInstance.SetAlpha(m_currStateData.Alpha);
                }
            }
        }

        private float CaclDistance(Vector3 pos1, Vector3 pos2)
        {
            pos1.y = 0.0f;
            pos2.y = 0.0f;
            float dis = Vector3.Distance(pos1, pos2);
            return dis;
        }
    }
}
