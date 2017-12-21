using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

[System.Serializable]
public enum CollisionShapeEnum
{
    Sphere=0,
    Capsule=1,
}

[System.Serializable]
public enum CollisionTypeEnum
{
    Attacked = 0,
    Attack = 1,
}

public delegate void CollisionEventHandler(AttackCollisionGroup sender, CollisionArgs arg);
public enum CollisionArgType
{
    CollisionDetected=0,
    CollisionDestroy=1,
}
public class CollisionArgs : EventArgs
{
    public CollisionArgType type;
    public Vector3 collisionPos;

    public int nFeedBackID;
    public int nAttackEntityID;
    public int nAttackedEntityID;

    public CollisionArgs()
    {
        type = CollisionArgType.CollisionDestroy;
        collisionPos = Vector3.zero;
        nFeedBackID = 0;
        nAttackEntityID = 0;
        nAttackedEntityID = 0;
    }
}

[System.Serializable]
public class CollisionShape
{
    public CollisionShapeEnum collisionShape=CollisionShapeEnum.Sphere;
    public CollisionTypeEnum collisionType=CollisionTypeEnum.Attack;
    public Transform startBone;
    public Transform endBone;
    public float radius = .5f;
    public Vector3 startOffSet=Vector3.zero;
    public Vector3 endOffSet=Vector3.zero;
    public bool enableCollision=true;


    public void OnDrawGizmos()
    {
        if (collisionType == CollisionTypeEnum.Attack)
        {
            if (collisionShape == CollisionShapeEnum.Sphere)
            {
                Gizmos.color = Color.magenta;
            }
            else
            {
                Gizmos.color = Color.cyan;

            }
        }
        else
        {
            if (collisionShape == CollisionShapeEnum.Sphere)
            {
                Gizmos.color = Color.yellow;
            }
            else
            {
                Gizmos.color = Color.blue;
            }
        }

        if (collisionShape == CollisionShapeEnum.Sphere)
        {
            if (startBone)
            {
                Vector3 center = startBone.position + startBone.rotation * startOffSet;
                if (radius > 0) Gizmos.DrawWireSphere(center, radius);
            }
        }
        else if (collisionShape == CollisionShapeEnum.Capsule)
        {
            if ((startBone) && (endBone))
            {
                Vector3 startPosition = startBone.position + startBone.rotation * startOffSet;
                Vector3 endPosition = endBone.position + endBone.rotation * endOffSet;
                if (radius > 0)
                {
                    Gizmos.DrawWireSphere(startPosition, radius);
                    Gizmos.DrawWireSphere(endPosition, radius);
                }

                Gizmos.DrawLine(startPosition, endPosition);
            }
        }
    }
}
