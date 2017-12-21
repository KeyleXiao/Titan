using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System;

//��ײ��⺯��
public class CollisionDetection
{
    static public bool collisionDetection(CollisionGroup attack, CollisionGroup attacked, ref Vector3 collisionPos)
    {
        if (attack.cullingShape.collisionType != CollisionTypeEnum.Attack && attack.cullingShape.collisionType != CollisionTypeEnum.Attacked)
        {
            Trace.Log("CollisionGroup.collisionType Error!");
            return false;
        }        

        //��Χ������޳�
        if (false == collisionDetection(attack.cullingShape, attacked.cullingShape, ref collisionPos))
        {
            return false;
        }

        foreach (CollisionShape attackShape in attack.collisionShapes)
        {
            foreach (CollisionShape attachedShape in attacked.collisionShapes)
            {
                if (true == collisionDetection(attackShape, attachedShape, ref collisionPos))
                {
                    return true;
                }
            }
        }
        return false;
    }


    static public bool collisionDetection(CollisionShape attack, CollisionShape attacked, ref Vector3 collisionPos)
    {
        if (attack == null || attacked == null)
        {
            return false;
        }
        if (attack.collisionShape == CollisionShapeEnum.Sphere && attacked.collisionShape == CollisionShapeEnum.Sphere)
        {
            Vector3 attackCenter = attack.startBone.position + attack.startBone.rotation * attack.startOffSet;
            Vector3 attackedCenter = attacked.startBone.position + attacked.startBone.rotation * attacked.startOffSet;
            float dis=Vector3.Distance(attackCenter, attackedCenter);
            if (dis < attack.radius + attacked.radius)
            {
                collisionPos = (attackedCenter + attackCenter) / 2.0f;
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (attack.collisionShape == CollisionShapeEnum.Sphere && attacked.collisionShape == CollisionShapeEnum.Capsule)
        {
            Vector3 center1 = attack.startBone.position + attack.startBone.rotation * attack.startOffSet;
            Vector3 segBegin = attacked.startBone.position + attacked.startBone.rotation * attacked.startOffSet;
            Vector3 segEnd = attacked.endBone.position + attacked.endBone.rotation * attacked.endOffSet;

            Vector3 porjectPos = Vector3.zero;
            float dis = DisPoint2Segment(center1, segBegin, segEnd,ref porjectPos);
            if (dis < attack.radius + attacked.radius)
            {
                Vector3 dir = center1 - porjectPos;
                if (dir.sqrMagnitude < 0.001f)
                {
                    collisionPos = (center1 + porjectPos) / 2.0f;                    
                }
                else
                {
                    dir.Normalize();
                    collisionPos = porjectPos + dir * attacked.radius;                    
                }
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (attack.collisionShape == CollisionShapeEnum.Capsule && attacked.collisionShape == CollisionShapeEnum.Sphere)
        {
            Vector3 center2 = attacked.startBone.position + attacked.startBone.rotation * attacked.startOffSet;
            Vector3 segBegin = attack.startBone.position + attack.startBone.rotation * attack.startOffSet;
            Vector3 segEnd = attack.endBone.position + attack.endBone.rotation * attack.endOffSet;

            Vector3 porjectPos = Vector3.zero;
            float dis = DisPoint2Segment(center2, segBegin, segEnd, ref porjectPos);
            if (dis < attack.radius + attacked.radius)
            {
                Vector3 dir = center2 - porjectPos;
                if (dir.sqrMagnitude < 0.001f)
                {
                    collisionPos = (center2 + porjectPos) / 2.0f;
                }
                else
                {
                    dir.Normalize();
                    collisionPos = porjectPos + dir * attack.radius;
                }
                return true;
            }
            else
            {
                return false;
            }
        }
        else if (attack.collisionShape == CollisionShapeEnum.Capsule && attacked.collisionShape == CollisionShapeEnum.Capsule)
        {            
            Vector3 segBegin1 = attack.startBone.position + attack.startBone.rotation * attack.startOffSet;
            Vector3 segEnd1 = attack.endBone.position + attack.endBone.rotation * attack.endOffSet;
            Vector3 segBegin2 = attacked.startBone.position + attacked.startBone.rotation * attacked.startOffSet;
            Vector3 segEnd2 = attacked.endBone.position + attacked.endBone.rotation * attacked.endOffSet;
            Vector3 projectPos1=Vector3.zero;
            Vector3 projectPos2 = Vector3.zero;
            float dis = SegmentDistance(segBegin1, segEnd1, segBegin2, segEnd2, ref projectPos1, ref projectPos2);
            if (dis < attack.radius + attacked.radius)
            {
                collisionPos = (projectPos1 + projectPos2) / 2.0f;
                return true;
            }
        }
        return false;
    }


    static private float SegmentDistance(Vector3 startPoint0, Vector3 endPoint0, Vector3 startPoint1, Vector3 endPoint1, ref Vector3 distanceStartPoint, ref Vector3 distanceEndPoint)
    {
        //���������߶μ����̾���
        //�ο��㷨 http://wenku.baidu.com/link?url=O1P3cEXAqiX1utg0_f2JfVtNV6AoBelwNYQOBF51f7NX_k_slJvda-8U7mYl-CLX60ijj7zhi7-ipvnZUkAIcW3Q6RL3oXEgdTBDyzE7Iqy

        //���߶�AB(startPoint0,endPoint0)������Ϊs��أ����߶�CD(startPoint1,endPoint1)������Ϊt���
        float x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4;
        x1 = startPoint0.x; y1 = startPoint0.y; z1 = startPoint0.z;
        x2 = endPoint0.x; y2 = endPoint0.y; z2 = endPoint0.z;
        x3 = startPoint1.x; y3 = startPoint1.y; z3 = startPoint1.z;
        x4 = endPoint1.x; y4 = endPoint1.y; z4 = endPoint1.z;

        //�Ծ�����ƫ��������ƫ����Ϊ0�õ�����
        //a0s-b0t=c0
        //a1s-b1t=c1
        float a0, b0, c0, a1, b1, c1;
        a0 = (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1);
        b0 = (x2 - x1) * (x4 - x3) + (y2 - y1) * (y4 - y3) + (z2 - z1) * (z4 - z3);
        c0 = (x1 - x2) * (x1 - x3) + (y1 - y2) * (y1 - y3) + (z1 - z2) * (z1 - z3);
        a1 = -((x2 - x1) * (x4 - x3) + (y2 - y1) * (y4 - y3) + (z2 - z1) * (z4 - z3));
        b1 = (x4 - x3) * (x4 - x3) + (y4 - y3) * (y4 - y3) + (z4 - z3) * (z4 - z3);
        c1 = (x1 - x3) * (x4 - x3) + (y1 - y3) * (y4 - y3) + (z1 - z3) * (z4 - z3);

        float ab = a0 * b1 - a1 * b0;
        if (Mathf.Abs(ab) > 0.0001f) //�����н�
        {
            float s = (c0 * b1 - b0 * c1) / (a0 * b1 - b0 * a1);
            float t = (c0 * a1 - a0 * c1) / (b0 * a1 - a0 * b1);

            if ((0.0f < s) && (s < 1.0f) && (0.0f < t) && (t < 1.0f)) //��������������㶼���߶���
            {
                Vector3 p = Vector3.zero;
                p.x = x1 + s * (x2 - x1);
                p.y = y1 + s * (y2 - y1);
                p.z = z1 + s * (z2 - z1);

                Vector3 q = Vector3.zero;
                q.x = x3 + t * (x4 - x3);
                q.y = y3 + t * (y4 - y3);
                q.z = z3 + t * (z4 - z3);

                distanceStartPoint = p;
                distanceEndPoint = q;
                return (Vector3.Distance(p, q));
            }
        }

        //��̾��벻�������߶�֮�䣬�������ĸ�������жϵ㵽ֱ�ߵľ���

        float minDistance = Vector3.Distance(startPoint0, startPoint1);
        distanceStartPoint = startPoint0; distanceEndPoint = startPoint1;

        float testDis;
        Vector3 testPorjectPos = Vector3.zero;

        testDis = DisPoint2Segment(startPoint0, startPoint1, endPoint1, ref testPorjectPos);
        if (testDis < minDistance)
        {
            minDistance = testDis;
            distanceStartPoint = startPoint0;
            distanceEndPoint = testPorjectPos;
        }
        testDis = DisPoint2Segment(endPoint0, startPoint1, endPoint1, ref testPorjectPos);
        if (testDis < minDistance)
        {
            minDistance = testDis;
            distanceStartPoint = endPoint0;
            distanceEndPoint = testPorjectPos;
        }
        testDis = DisPoint2Segment(startPoint1, startPoint0, endPoint0, ref testPorjectPos);
        if (testDis < minDistance)
        {
            minDistance = testDis;
            distanceStartPoint = startPoint1;
            distanceEndPoint = testPorjectPos;
        }
        testDis = DisPoint2Segment(endPoint1, startPoint0, endPoint0, ref testPorjectPos);
        if (testDis < minDistance)
        {
            minDistance = testDis;
            distanceStartPoint = endPoint1;
            distanceEndPoint = testPorjectPos;
        }


        return minDistance;
    }


    static public float DisPoint2Segment(Vector3 pointPos, Vector3 segStartPos, Vector3 segEndPos, ref Vector3 projectPos)
    {
        //��ͶӰ����㵽�߶�֮��ľ���
        //�ο�http://blog.sina.com.cn/s/blog_5d5c80840101bnhw.html������ά��չΪ��ά

        Vector3 AB = segEndPos - segStartPos;
        Vector3 AC = pointPos - segStartPos;

        float dot = Vector3.Dot(AB, AC);
        if (dot <= 0)
        {
            //AC��AB�ķ�������̾���ΪAC
            projectPos = segStartPos;
            return AC.magnitude;
        }

        float d2 = AB.sqrMagnitude;
        if (dot >= d2)
        {
            //ͶӰ��AB������̾���ΪBC
            projectPos = segEndPos;
            return Vector3.Distance(pointPos, segEndPos);
        }

        float r = dot / d2;
        projectPos = segStartPos + AB * r;
        return Vector3.Distance(pointPos, projectPos);
    }
}