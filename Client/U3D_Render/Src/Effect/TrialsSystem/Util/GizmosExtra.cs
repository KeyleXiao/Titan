//using UnityEngine;
//namespace Effect
//{
//    namespace TrailsSystem
//    {
//        public static class GizmosExtra
//        {

//            public static void GizmosDrawCircle(Vector3 position, Vector3 up, float size, int divisions)
//            {
//                Vector3 offset = (Quaternion.Euler(90, 0, 0) * (up * size));

//                for (int i = 0; i < divisions; i++)
//                {

//                    Vector3 newOffset = Quaternion.AngleAxis(360f / divisions, up) * offset;
//                    Gizmos.DrawLine(position + offset, position + newOffset);
//                    offset = newOffset;
//                }
//            }

//            public static void GizmosDrawArrow(Vector3 from, Vector3 to, float arrowSize)
//            {
//                Gizmos.DrawLine(from, to);

//                Vector3 dir = to - from;
//                dir = dir.normalized * arrowSize;

//                Gizmos.DrawLine(to, to - Quaternion.Euler(0, 0, 45) * dir);
//                Gizmos.DrawLine(to, to - Quaternion.Euler(0, 0, -45) * dir);

//            }
//        }
//    }
//}

