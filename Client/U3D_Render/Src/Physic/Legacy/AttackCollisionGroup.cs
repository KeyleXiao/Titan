using UnityEngine;
using System.Collections;

public class AttackCollisionGroup : CollisionGroup
{
    private AttackedCollisionGroup[] attackedGroups;
    [HideInInspector]
    public bool bEnableCollision=false;
    [HideInInspector]
    public int eventHandlerCount = 0;
    [HideInInspector]
    public int nFeedBackID;
    [HideInInspector]
    public int nAttackEntityID;

    public void Start()
    {
        attackedGroups = UnityEngine.Object.FindObjectsOfType(typeof(AttackedCollisionGroup)) as AttackedCollisionGroup[];
    }

    public void Update()
    {
        if (true == bEnableCollision)
        {
            collisionTest();
        }
    }

    public void OnDestroy()
    {
        if (eventHandlerCount > 0)
        {
            Trace.Log("ResNode.DestroyRes!");
            CollisionArgs args = new CollisionArgs();
            args.type = CollisionArgType.CollisionDestroy;
            collisionEvent(this, args);
        }
    }

    //Åö×²ÌåÏà¹Ø
    public event CollisionEventHandler collisionEvent;

    private void collisionTest()
    {
        if (eventHandlerCount > 0)
        {
            foreach (AttackedCollisionGroup attackedGroup in attackedGroups)
            {
                if (attackedGroup.nEntityID != nAttackEntityID)
                {
                    //Trace.Log("collisionTest!");
                    Vector3 collisionPos = Vector3.zero;
                    if (true == CollisionDetection.collisionDetection(this, attackedGroup, ref collisionPos))
                    {
                        Trace.Log("Hit!");
                        CollisionArgs args = new CollisionArgs();
                        args.type = CollisionArgType.CollisionDetected;
                        args.nAttackEntityID = nAttackEntityID;
                        args.nAttackedEntityID = attackedGroup.nEntityID;
                        args.nFeedBackID = nFeedBackID;
                        args.collisionPos = collisionPos;
                        collisionEvent(this, args);
                        bEnableCollision = false;
                        break;
                    }
                }
            }

        }

    }
}