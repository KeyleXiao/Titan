//Unity��ChatacterController�Ѿ����������Ҫʹ�������
//using UnityEngine;
//using System.Collections;



//public class CharacterControllerConfig : MonoBehaviour
//{

//    /// <summary>
//    /// �¶�
//    /// </summary>
//    public float SlopeLimit = 45;

//    /// <summary>
//    /// ̨�׸߶ȣ���������ô�ߵ�̨��
//    /// </summary>
//    public float StepOffset = 0.0f;

//    /// <summary>
//    /// ��С�ƶ�����
//    /// </summary>
//    public float MinMoveDistance = 0.01f;

//    /// <summary>
//    /// Ƥ����Ⱦ�����������ײ�����Ի����������ȡ��ϴ��Ƥ����ֵ�Ȼᵼ�²�����С��Ƥ�����ֵ�ᵼ�½�ɫ����ס��һ��������趨��ʹ��ֵ���ڰ뾶��Radius����10%��
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
