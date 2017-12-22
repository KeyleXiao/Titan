using UnityEngine;
using System.Collections;
using GameLogic;
using System.Runtime.InteropServices;
using System.IO;
using System;

/// <summary>
/// 游戏辅助类
/// </summary>
public class GameUtil 
{
    /// <summary>
	/// Fixs the instantiated object (in some cases object have wrong position, rotation and scale).
	/// </summary>
	/// <param name="source">Source.</param>
	/// <param name="instance">Instance.</param>
	static public void FixInstantiated(ResNode source, Component instance)
	{
		FixInstantiated(source, instance.gameObject);
	}

	/// <summary>
	/// Fix the instantiated object (in some cases object have wrong position, rotation and scale).
	/// </summary>
	/// <param name="source">Source.</param>
	/// <param name="instance">Instance.</param>
    static public void FixInstantiated(ResNode source, GameObject instance)
	{
        var rectTransform = instance.transform as RectTransform;
        rectTransform.localPosition = source.GetResPosition();
        rectTransform.localRotation = source.GetResRotation();
		rectTransform.localScale = source.GetResScale();
		rectTransform.anchoredPosition = source.GetResAnchoredPosition();
		rectTransform.sizeDelta = source.GetResSizeDelta();
	}

    public static void BreakPoint()
    {
        int a = 0;
        switch(a)
        {
            case 1:
                break;
        }
    }

    public static double TicksToSecond(double ticks)
    {
 
        return ticks * 0.0000001f;
    }

    public static double SecondToTicks(double ticks)
    {

        return ticks / 0.0000001f;
    }



    public static void EnabledAnimator(Animator an)
    {
        if (an)
        {
            if (!an.enabled)
            {
                an.enabled = true;
            }
        }
    }

    public static void DisabledAnimator(Animator an)
    {
        if (an)
        {
            if (an.enabled)
            {
                an.enabled = false;
            }
        }
    }

    
    public static void DestroyMesh(Mesh m)
    {
        if (! m)
        {
            return;
        }
        m.UploadMeshData(true);
        m.Clear();
        MonoBehaviour.DestroyImmediate(m, true);
        m = null;
        
    }

    public static Coroutine StartCoroutineHelper(MonoBehaviour host,IEnumerator routine)
    {
        if(!host)
        {
            return null;
        }

        if(!host.isActiveAndEnabled)
        {
            return null;
        }

        return host.StartCoroutine(routine);
    }

    public static void SetLayer(int layer, GameObject go, bool bIncludeChild = false)
    {
        if (bIncludeChild)
        {
            Transform[] trs = go.GetComponentsInChildren<Transform>();
            //先找出skinDataObj和ScreenCastLayerObj和ColliderShapeObj
            foreach (Transform t in trs)
            {
                t.gameObject.layer = layer;
            }
        }
        else
        {
            go.layer = layer;
        }

    }

    /// <summary>
    /// 是否是无效的位置
    /// </summary>
    /// <param name="pos"></param>
    /// <returns></returns>
    public static bool IsInvalidPos(Vector3 pos)
    {
        if (float.IsNaN(pos.x) || float.IsNaN(pos.y) || float.IsNaN(pos.z))
        {
            return true;
        }
        return false;
    }



    public static bool isLightMappingShader(string shaderName)
    {
        string s = shaderName.ToLower();
        if (s.Contains("lightingmap"))
        {
            return true;
        }
        return false;


    }
    public static bool isStandardShader(string shaderName)
    {
        string s = shaderName.ToLower();
        if (s.Contains("standard"))
        {
            return true;
        }
        return false;

    }

    public static float GetSqrDistanceWithOutY(Vector3 p1,Vector3 p2)
    {
        p1.y = 0;
        p2.y = 0;
        return (p1 - p2).sqrMagnitude;
    }

    public static bool FloatEquls(float a,float b)
    {
        float c = a - b;
        c = Mathf.Abs(c);
        return c < 0.000001f;
    }

    public static int MsgBox(IntPtr hWnd,string txt,string caption,int type)
    {
        if(Application.platform == RuntimePlatform.WindowsPlayer || Application.platform == RuntimePlatform.WindowsEditor)
        {
            return WindowsMessageBox(hWnd,txt,caption,type);
        }
        return 0;
    }

    [DllImport("user32.dll", CharSet = CharSet.Auto, EntryPoint = "MessageBox")]
    private static extern int WindowsMessageBox(IntPtr hWnd, string txt, string caption, int type);

	/// <summary>
	/// 删除子节点
	/// </summary>
	/// <param name="parent"></param>
	public static void DestroyChild(Transform parent)
	{
		if (parent == null)
			return;

		for (int i = parent.childCount - 1; i >= 0; --i)
		{
			GameObject go = parent.GetChild(i).gameObject;
			ResNode.DestroyRes(ref go);
		}
	}


    /// <summary>
    /// 获取广告信息,在编辑器下无效
    /// </summary>
    /// <param name="info"></param>
    public static AdvInfo GetAdvInfo()
    {
        AdvInfo info = new AdvInfo();

        //编辑器下的信息
        if (Application.isEditor)
        {
            info.radid = string.Empty;
            info.rsid = string.Empty;
            info.PartnerID = "0";

            return info;
        }

        string RadidKey = "radid:";
        string rsidKey = "rsid:";
        string PartnerKey = "Partner:";

        System.Collections.Generic.List<string> argList = new System.Collections.Generic.List<string>();
        argList.AddRange(System.Environment.GetCommandLineArgs());

        foreach (string arg in argList)
        {
            if (arg.Contains(RadidKey))
            {
                info.radid = arg.Replace(RadidKey, string.Empty);
            }
            if (arg.Contains(rsidKey))
            {
                info.rsid = arg.Replace(rsidKey, string.Empty);
            }
            if (arg.Contains(PartnerKey))
            {
                info.PartnerID = arg.Replace(PartnerKey, string.Empty);
            }
        }
        return info;
    }

}
