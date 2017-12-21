using System;
using UnityEngine;
using System.Collections;
using System.Collections.Generic;

// 复杂动画管理器
// 每个人物挂一个脚本
// 负责管理此人身上的光效

public class AnimationManager : MonoBehaviour
{
    private List<AnimationEffect>  mAnimationList; //记录当前挂在人物上的光效
    private Animator animator;
    public bool showLog = false;

    private LayerMask collisionLayers;

    public void Awake()
    {
        mAnimationList = new List<AnimationEffect>();        
        animator = GetComponent<Animator>();
        collisionLayers = -1;

        collisionLayers = collisionLayers.value & (~(1 << LayerMask.NameToLayer(Config.LayerHero)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerNeutrality)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerOcclusCulling)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastEnemy)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerScreenRaycastFriend)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerEnemy)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerPlayerFriend)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterEnemy)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerMonsterFriend)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerWard_Enemy)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerWard_Friend)));
        collisionLayers = collisionLayers & (~(1 << LayerMask.NameToLayer(Config.LayerLightingEffectOnly)));
    }

    // 播放一个复杂动作
    public AnimationEffect AddAnimation(int nID, AnimationParam param)
    {
        AnimationEffect newAnim = AnimationEffectFactory.Instance.CreateAnimation(nID);
        if (newAnim != null)
        {
            newAnim.param = param;
            newAnim.showLog = showLog;

            if (newAnim.Begin())
            {
                mAnimationList.Add(newAnim);
            }
        }
        return newAnim;
    }

    // 中断攻击动作
    public bool BreakAnimation()
    {
        foreach (AnimationEffect anim in mAnimationList)
        {
            anim.needClose = true;
        }
        return true;
    }


    public void Update()
    {
        if (mAnimationList == null)
        {
            return;
        }

        float currentHeight = -1.0f;

        //遍历列表，进行更新或关闭操作
        for (int i = 0; i < mAnimationList.Count; )
        {
            AnimationEffect anim = mAnimationList[i];

            bool bRemove = false;
            if (anim.needClose == true) //动作需要关闭并移除
            {
                anim.Close();
                bRemove = true;
            }
            else
            {
                if (anim.rigidHeight)
                {
                    if (currentHeight < 0.0f)
                    {                        
                        Vector3  pos=transform.position;
                        RaycastHit hitInfo;
                        Ray PointToGroundRay = new Ray(pos, new Vector3(0, -1, 0));
                        if (Physics.Raycast(PointToGroundRay, out hitInfo, 100, collisionLayers))
                        {
                            currentHeight = pos.y - hitInfo.point.y;
                        }
                        else
                        {
                            currentHeight = 0.0f;
                        }
                    }

                    anim.currentHeight = currentHeight;
                }
                if (anim.Update() == false) //否则，对动作进行更新，更新失败的关闭并移除
                {
                    anim.Close();
                    bRemove = true;
                }
            }

            if (bRemove)
            {
                mAnimationList.Remove(anim);
            }
            else
            {
                i++;
            }
        }
    }
}
