using UnityEngine;
using System.Collections;
using System.Collections.Generic;


public class CollisionGroup : MonoBehaviour 
{
    public CollisionShape cullingShape;    //�����޳��õ���ײ��
    public CollisionShape[] collisionShapes;      //һ���ܻ���ײ��

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
