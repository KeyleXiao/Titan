using UnityEngine;
using System.Collections;
using System.Collections.Generic;


public class CollisionGroup : MonoBehaviour 
{
    public CollisionShape cullingShape;    //快速剔除用的碰撞体
    public CollisionShape[] collisionShapes;      //一组受击碰撞体

    public void OnDrawGizmos()
    {
        if (cullingShape != null)
        {
            cullingShape.OnDrawGizmos();
        }
        foreach (CollisionShape shape in collisionShapes)
        {
            if (shape != null)
            {
                shape.OnDrawGizmos();
            }
        }
        
    }
}
