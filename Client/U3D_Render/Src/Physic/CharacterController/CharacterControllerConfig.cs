//Unity的ChatacterController已经解决，不需要使用这个了
//using UnityEngine;
//using System.Collections;



//public class CharacterControllerConfig : MonoBehaviour
//{

//    /// <summary>
//    /// 坡度
//    /// </summary>
//    public float SlopeLimit = 45;

//    /// <summary>
//    /// 台阶高度，可以走这么高的台阶
//    /// </summary>
//    public float StepOffset = 0.0f;

//    /// <summary>
//    /// 最小移动距离
//    /// </summary>
//    public float MinMoveDistance = 0.01f;

//    /// <summary>
//    /// 皮肤厚度决定了两个碰撞器可以互相渗入的深度。较大的皮肤厚值度会导致颤抖。小的皮肤厚度值会导致角色被卡住。一个合理的设定是使该值等于半径（Radius）的10%。
//    /// </summary>
//    public float SkinWidth = 0.05f;

//    public float height = 2.0f;
//    public float Radius = 0.5f;
//    public Vector3 Center = Vector3.zero;

//    private Transform _mTrs = null;

//    public float HeightInWorldSpace
//    {
//        get
//        {
//            if(!_mTrs)
//            {
//                _mTrs = transform;
//            }
//            return height * _mTrs.localScale.y;
//        }
//    }

//    public float RadiusInWorldSpace
//    {
//        get
//        {
//            if (!_mTrs)
//            {
//                _mTrs = transform;
//            }

//            return Radius * Mathf.Max(_mTrs.localScale.x,_mTrs.localScale.z);
//        }
//    }

//    public Vector3 CenterInWorldSpace
//    {
//        get
//        {
//            if (!_mTrs)
//            {
//                _mTrs = transform;
//            }
//            return _mTrs.localToWorldMatrix.MultiplyPoint(Center);
//        }
//    }
//}
