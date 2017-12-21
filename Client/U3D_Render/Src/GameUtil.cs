using UnityEngine;
using System.Collections;
using GameLogic;
using System.Runtime.InteropServices;
using System.IO;
using System;
using USpeedUI;

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

    public static string CopyString(string srcStr)
    {
        
        return string.Intern(srcStr);
    }

    static System.Text.StringBuilder BuildStringBuilder = new System.Text.StringBuilder(512);
    public static string BuildString(params object[] parms)
    {
        BuildStringBuilder.Length = 0;
        foreach(object o in parms)
        {
            BuildStringBuilder.Append(o);
        }
        return BuildStringBuilder.ToString();
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
    /// 设置光效的碰撞层
    /// </summary>
    /// <param name="layer"></param>
    /// <param name="nod"></param>
    public static void SetEffectColliderLayer(int layer,Effect.EffectNode nod)
    {
        if(!nod)
        {
            return;
        }

        if(nod.effectColliders == null || nod.effectColliders.Length <= 0)
        {
            return;
        }

        foreach(Collider col in nod.effectColliders)
        {
            col.gameObject.layer = layer;
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

    /// <summary>
    /// 是否在摄像机视野内
    /// </summary>
    /// <param name="pos"></param>
    /// <returns></returns>
    public static bool isOnCameraView(Vector3 pos)
    {
        if(!Initialize.mainCam)
        {
            return false;
        }

        Vector3 ViewPos = Initialize.mainCam.WorldToViewportPoint(pos);
        if (ViewPos.x < 0 || ViewPos.x > 1 ||
            ViewPos.y < 0 || ViewPos.y > 1 ||
                ViewPos.z < 0)
            return false;
        return true;
    }

	// 发送聊天消息
	// @param isDetectTextRepeat: 是否做相似度检查
	public static void SendChatMessage(EMChatChannelType channel, string msg, bool isDetectTextRepeat = false)
    {
        if (channel == EMChatChannelType.CHAT_CHANNEL_WORLD || channel == EMChatChannelType.CHAT_CHANNEL_ZONE)
        {
            isDetectTextRepeat = true;
        }

        gameview_send_chat_message msgInfo = new gameview_send_chat_message();
        msgInfo.channel = (int)channel;
        msgInfo.message = msg;
		msgInfo.isDetectTextRepeat = isDetectTextRepeat;
            
        ViewEventHelper.Instance.SendCommand<gameview_send_chat_message>(GameLogicDef.GVIEWCMD_SEND_CHAT_MESSAGE, ref msgInfo);
    }

	// 发送聊听消息
	// @param isDetectTextRepeat: 是否做相似度检查
	public static void SendChatMessage(EMChatChannelType channel, string msg, string target, uint pdbid, bool isDetectTextRepeat = false)
    {
        if (channel == EMChatChannelType.CHAT_CHANNEL_WORLD || channel == EMChatChannelType.CHAT_CHANNEL_ZONE)
        {
            isDetectTextRepeat = true;
        }

        gameview_send_chat_message msgInfo = new gameview_send_chat_message();
        msgInfo.channel = (int)channel;
        msgInfo.message = msg;
        msgInfo.targetName = target;
        msgInfo.targetPdbid = pdbid;
		msgInfo.isDetectTextRepeat = isDetectTextRepeat;

        ViewEventHelper.Instance.SendCommand<gameview_send_chat_message>(GameLogicDef.GVIEWCMD_SEND_CHAT_MESSAGE, ref msgInfo);
    }

    // 显示系统消息
    public static void ShowSystemMessage(EMChatTipID tipID, string param = "")
    {
        gameview_show_system_message msgInfo = new gameview_show_system_message();
        msgInfo.nIndexId = (int)tipID;
        msgInfo.param = param;

        ViewEventHelper.Instance.SendCommand<gameview_show_system_message>(GameLogicDef.GVIEWCMD_SHOW_SYSTEM_MESSAGE, ref msgInfo);
    }

    // 显示喇叭
    public static void ShowLoudspeaker(EMChatTipID tipID, string param = "")
    {
        gameview_show_loudspeaker msgInfo = new gameview_show_loudspeaker();
        msgInfo.nIndexId = (int)tipID;
        msgInfo.param = param;

        ViewEventHelper.Instance.SendCommand<gameview_show_loudspeaker>(GameLogicDef.GVIEWCMD_SHOW_LOUDSPEAKER, ref msgInfo);
    }

    public static bool StringToEnum(Type enumType,string str,out System.Object result)
    {
        result = null;
        try
        {
            if (str.Equals(string.Empty))
            {
                return false;
            }
            result = Enum.Parse(enumType, str, false);
        }
        catch (Exception e)
        {
            return false;
        }
        return true;
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
	/// 获取本地玩家的联盟ID
	/// </summary>
	/// <returns></returns>
	public static int getMainHeroClanID()
	{
		if (EntityFactory.MainHeroView == null)
			return 0;

		return EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_CLAN);
	}

	/// <summary>
	/// 获取本地玩家的战队ID
	/// </summary>
	/// <returns></returns>
	public static int getMainHeroKinID()
	{
		if (EntityFactory.MainHeroView == null)
			return 0;

		return EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_KIN);
	}
	
	/// <summary>
	/// 获取本地玩家的Pdbid
	/// </summary>
	/// <returns></returns>
	public static int getMainHeroPdbid()
	{
		if (EntityFactory.MainHeroView == null)
			return 0;

		return EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_ID);
	}

	/// <summary>
	/// 获取本地玩家的当前等级
	/// </summary>
	/// <returns></returns>
	public static int getMainHeroLv()
	{
		if (EntityFactory.MainHeroView == null)
			return 0;

		return EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_LEVEL);
	}

    /// <summary>
    /// 格式化MapID，从配置里读取数据时，对传进来的主城地图ID都转换成1
    /// </summary>
    /// <param name="nMapID"></param>
    public static void formatMapID(ref int nMapID)
    {
        if (GameLogicAPI.isMainMap(nMapID))
            nMapID = 1;
    }

    /// <summary>
    /// 获取本地玩家当前地图ID
    /// </summary>
    /// <returns></returns>
    public static int getMainHeroCurMapID()
    {
        if (EntityFactory.MainHeroView == null)
            return 0;

        return EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_SCENEID);
    }


    public static string getMainHeroName()
    {
        if (EntityFactory.MainHeroView == null)
            return "";

        return EntityFactory.MainHeroView.Property.CreatureName;
    }

    public static int getMainHeroHeroID()
    {
        if (EntityFactory.MainHeroView == null)
            return 0;

        return EntityFactory.MainHeroID;
    }

    static System.Text.StringBuilder FormatStringBuilder = new System.Text.StringBuilder(512);
    public static string FormatString(string format,params object[] args)
    {
        if(string.IsNullOrEmpty(format) || (null == args))
        {
            return string.Empty;
        }

        int len = format.Length;
        for(int i = 0;i < args.Length;i++)
        {
            if(args[i] is string)
            {
                len += ((string)args[i]).Length;
            }
            else if(args[i] is int)
            {
                len += 16;
            }
            else if (args[i] is double)
            {
                len += 16;
            }
            else if (args[i] is float)
            {
                len += 16;
            }
            else
            {
                len += 8;
            }
        }

        if (len > FormatStringBuilder.Capacity)
        {
            FormatStringBuilder.Capacity = len;
        }
        FormatStringBuilder.Length = 0;
        FormatStringBuilder.AppendFormat(format, args);
        return FormatStringBuilder.ToString();
    }
}
