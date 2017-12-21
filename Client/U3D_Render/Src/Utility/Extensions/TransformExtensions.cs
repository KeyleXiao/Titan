using UnityEngine;
using System;
using System.Collections;

public static class TransformExtensions
{
	public static void SetPosition(this Transform trans, Vector3 pos)
    {
        if(GameUtil.IsInvalidPos(pos))
        {
            Debug.LogWarning(trans.name + "设置无效的位置,有NAN值!-" + pos);
            return;
        }
        trans.position = pos;
	}

    public static void SetLocalPosition(this Transform trans, Vector3 pos)
    {
        if (GameUtil.IsInvalidPos(pos))
        {
            Debug.LogWarning(trans.name + "设置无效的位置,有NAN值!!-" + pos);
            return;
        }
        trans.localPosition = pos;
    }
}
