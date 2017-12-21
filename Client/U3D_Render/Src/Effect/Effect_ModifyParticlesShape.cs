/*******************************************************************
** 文件名:	Effect_ModifyParticlesShape.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2014 - Speed
** 创建人:	谭强均
** 日  期:	2014/11/21
** 版  本:	1.0
** 描  述:	特效功能脚本
** 应  用:  	修改粒子系统的发射Shape

**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/

using UnityEngine;
using System.Collections;

namespace Effect
{
    public class Effect_ModifyParticlesShape : MonoBehaviour
    {
        public ModifyParticlesShapePart modifyPart = ModifyParticlesShapePart.Body;

        private ParticleSystem systemInstance;
        static System.Type MeshRendererType = typeof(MeshRenderer);
        static System.Type SkinnedMeshRendererType = typeof(SkinnedMeshRenderer);
        int lastID = int.MinValue;

        void Start()
        {
            systemInstance = GetComponent<ParticleSystem>();
        }
        public void Play(int entityID)
        {
            if(entityID == lastID)
            {
                return;
            }
            U3D_Render.EntityView view = EntityFactory.getEntityViewByID(entityID);
            if(null == view || !systemInstance)
            {
                return;
            }
            if (null == view.SkinControl.CurrentSkinInstance || !view.SkinControl.CurrentSkinInstance.entitySkinConfig)
            {
                return;

            }
            lastID = entityID;
            switch(modifyPart)
            {
                case ModifyParticlesShapePart.Body:
                    Modify(view.SkinControl.CurrentSkinInstance.entitySkinConfig.BodyRender);
                    break;
                case ModifyParticlesShapePart.LeftWeapon:
                    Modify(view.SkinControl.CurrentSkinInstance.entitySkinConfig.LeftWeaponRender);
                    break;
                case ModifyParticlesShapePart.RightWeapon:
                    Modify(view.SkinControl.CurrentSkinInstance.entitySkinConfig.RightWeaponRender);
                    break;
            }
        }

        public void OnDestroy()
        {
            systemInstance = null;
        }

        private void Modify(Renderer r)
        {
            if(null == r)
            {
                return;
            }
        
            ParticleSystem.ShapeModule shapeModle = systemInstance.shape;
            if(r.GetType() == MeshRendererType)
            {
                shapeModle.shapeType = ParticleSystemShapeType.MeshRenderer;
                shapeModle.meshRenderer = r as MeshRenderer;

            }
            else if(r.GetType() == SkinnedMeshRendererType)
            {
                shapeModle.shapeType = ParticleSystemShapeType.SkinnedMeshRenderer;
                shapeModle.skinnedMeshRenderer = r as SkinnedMeshRenderer;
            }
        }
    }
}


