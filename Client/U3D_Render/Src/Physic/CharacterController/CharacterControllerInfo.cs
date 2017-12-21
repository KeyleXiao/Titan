using UnityEngine;
using System.Collections;



internal class CharacterControllerInfo
{
    
    public static CharacterControllerInfo getInfo(CharacterController controller)
    {
        if(!controller)
        {
            return null;
        }
        CharacterControllerInfo ins = new CharacterControllerInfo();
        ins.ChangeTarget(controller);
        return ins;
    }

    public static void Destroy(ref CharacterControllerInfo instance)
    {
        if(null == instance)
        {
            return;
        }

        instance.Clear();
        instance = null;
    }

    ~CharacterControllerInfo()
    {
        Clear();
    }

    private void ChangeTarget(CharacterController controller)
    {
        if(!controller)
        {
            return;
        }

        m_TargetController = controller;
        _mTrs = controller.transform;
        SlopeLimit = controller.slopeLimit;
        StepOffset = controller.stepOffset;
        SkinWidth = controller.skinWidth;
        height = controller.height;
        Radius = controller.radius;
        Center = controller.center;
    }

    private void Clear()
    {
        m_TargetController = null; ;
        _mTrs = null ;
    }

    /// <summary>
    /// �¶�
    /// </summary>
    public float SlopeLimit = 45;

    /// <summary>
    /// ̨�׸߶ȣ���������ô�ߵ�̨��
    /// </summary>
    public float StepOffset = 0.0f;

    /// <summary>
    /// Ƥ����Ⱦ�����������ײ�����Ի����������ȡ��ϴ��Ƥ����ֵ�Ȼᵼ�²�����С��Ƥ�����ֵ�ᵼ�½�ɫ����ס��һ��������趨��ʹ��ֵ���ڰ뾶��Radius����10%��
    /// </summary>
    public float SkinWidth = 0.05f;


    public float height = 2.0f;
    public float Radius = 0.5f;
    public Vector3 Center = Vector3.zero;

    private CharacterController m_TargetController = null;
    private Transform _mTrs = null;
    public float HalfHeightInWorldSpace
    {
        get
        {
            if (!_mTrs)
            {
                _mTrs = m_TargetController.transform;
            }
            float validHegiht = Mathf.Max(height, Radius);
            return validHegiht * _mTrs.localScale.y * 0.5f;
        }
    }

    public float HeightInWorldSpace
    {
        get
        {
            if (!_mTrs)
            {
                _mTrs = m_TargetController.transform;
            }
            float validHegiht = Mathf.Max(height, Radius);
            return validHegiht * _mTrs.localScale.y;
        }
    }

    public float RadiusInWorldSpace
    {
        get
        {
            if (!_mTrs)
            {
                _mTrs = m_TargetController.transform;
            }

            return Radius * Mathf.Max(_mTrs.localScale.x, _mTrs.localScale.z);
        }
    }

    public Vector3 CenterInWorldSpace
    {
        get
        {
            if (!_mTrs)
            {
                _mTrs = m_TargetController.transform;
            }
            return _mTrs.localToWorldMatrix.MultiplyPoint(Center);
        }
    }

    public Vector3 UpPointInWorldSpace
    {
        get
        {
            if (!_mTrs)
            {
                _mTrs = m_TargetController.transform;
            }
            Vector3 pt = CenterInWorldSpace;
            pt.y += HalfHeightInWorldSpace;
            return pt;
        }
    }

    public Vector3 DownPointInWorldSpace
    {
        get
        {
            if (!_mTrs)
            {
                _mTrs = m_TargetController.transform;
            }
            Vector3 pt = CenterInWorldSpace;
            pt.y -= HalfHeightInWorldSpace;

            return pt;
        }
    }

}
