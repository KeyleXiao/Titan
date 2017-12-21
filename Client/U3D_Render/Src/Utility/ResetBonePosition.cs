using UnityEngine;
using System.Collections;

public class ResetBonePosition : MonoBehaviour
{

    public Animator hostAnimator = null;
    private static string ShowStateName = "zhanshi1";
    private static string StandStateName = "zhanshi2";
    private bool NeedToSet = false;
	// Update is called once per frame
	public void LateUpdate ()
    {
        if (!hostAnimator)
        {
            return;
        }
        if(!hostAnimator.enabled)
        {
            return;
        }
        if(!hostAnimator.runtimeAnimatorController)
        {
            return;
        }
        if(!hostAnimator.avatar)
        {
            return;
        }
        AnimatorStateInfo info_Show = hostAnimator.GetCurrentAnimatorStateInfo(0);

        if (info_Show.IsName(ShowStateName))
        {
            if (info_Show.normalizedTime >= 0.99f)
            {
                NeedToSet = true;
            }
            else
            {
                NeedToSet = false;
            }

        }
        else
        {
            AnimatorStateInfo info_Stand = hostAnimator.GetCurrentAnimatorStateInfo(0);
            if (info_Stand.IsName(StandStateName))
            {
                NeedToSet = true;
            }
        }


        if(NeedToSet)
        {
            transform.localPosition = Vector3.zero;
        }

	}
}
