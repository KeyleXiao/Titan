/*******************************************************************
** 文件名:	OptimitzedControlPlane.cs  
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	谭强均
** 日  期:	2016-2-24
** 版  本:	1.0
** 描  述:	OptimitzedPlane 优化控制面板

********************************************************************/
using UnityEngine;
using System;
using Effect;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using GameLogic;
/// <summary>
/// 优化面板
/// </summary>
public class OptimitzedControlPlane : MonoBehaviour
{
    public static OptimitzedControlPlane Instance = null;

    private GameObject selectedEffectCacheRoot = null;
    private List<UnityEngine.Object> selectedEffectsAssets = new List<UnityEngine.Object>();
    private List<UnityEngine.Object> selectedEffectsInstance = new List<UnityEngine.Object>();

    public void Awake()
    {
        if(!Instance)
        {
            Instance = this;
        }
        else
        {
            Debug.LogError("多个OptimitzedControlPlane实例，这个实例只能有一个！现有的实例为:" + Instance.name + ",多余的实例为:" + name);
        }
    }

    public void Start()
    {
        OptimitzedSelectedEffect();
    }
    public void OnDestroy()
    {
        ClearSelectedEffect();
    }


    //通过按键进入游戏
    //int state = 0;
    public void Update()
    {

        #region 通过按键进入游戏
        //通过按键进游戏
        //if(Input.GetKeyUp(KeyCode.N))
        //{
        //    switch (state)
        //    {
        //        case 0://denglu
        //            {
        //                // 现在是IP地址
        //                GameLogic.gameview_login data = new GameLogic.gameview_login();
        //                //data.svrip = "172.16.0.169";
        //                data.svrip = "127.0.0.1";
        //                data.account = "s124";
        //                data.password = "g";


        //                IntPtrHelper helper = new IntPtrHelper();
        //                IntPtr ptr = helper.toPtr<GameLogic.gameview_login>(ref data);
        //                int len = Marshal.SizeOf(data);

        //                ViewEventHelper.Instance.SendCommand(GameLogic.GameLogicDef.GVIEWCMD_USER_LOGIN, 1);
        //                state++;
        //                Debug.LogError("登陆");
        //            }

        //            break;
        //        case 1://打开战场大厅
        //            {
        //                ViewEventHelper.Instance.SendCommand(GameLogic.GameLogicDef.GVIEWCMD_MATCH_ENTER_HALL);
        //                state++;
        //                Debug.LogError("打开战场大厅");
        //            }

        //            break;
        //        case 2://打开创建房间
        //            {
        //                ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_OPEN_CREATE_ROOM);
        //                state++;
        //                Debug.LogError("打开创建房间");
        //            }
        //            break;
        //        case 3://创建房间
        //            {
        //                cmd_match_create_room data = new cmd_match_create_room();
        //                data.szName = "";
        //                ViewEventHelper.Instance.SendCommand<cmd_match_create_room>(GameLogicDef.GVIEWCMD_MATCH_CREATE_ROOM, ref data);
        //                state++;
        //                Debug.LogError("创建房间");
        //            }

        //            break;

        //        case 4://开始游戏
        //            {
        //                ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_START_GAME);
        //                state++;
        //                Debug.LogError("开始游戏");
        //            }
        //            break;
        //        case 5://选英雄
        //            {
        //                //选英雄
        //                ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ROOM_HERO_CHANGE, 10);
        //                state++;
        //                Debug.LogError("选英雄");
        //            }
        //            break;
                                
        //        case 6://确认
        //            {
        //                ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_ROOM_CONFIRM_HERO);
        //                state++;
        //                Debug.LogError("确认英雄");
        //            }
        //            break;
        //        case 7://倒计时结束
        //            {
        //                ViewEventHelper.Instance.SendCommand(GameLogicDef.GVIEWCMD_MATCH_READY_TO_FIGHT);
        //                state++;
        //                Debug.LogError("倒计时结束");
        //            }
                   
        //            break;
        //    }
        //}
        #endregion


     LightingEffectManager.UpdateCreateList();
    }

    #region 优化控制
    /// <summary>
    /// 是否开启普通飘字
    /// </summary>
    [System.NonSerialized]
    public bool EnabledNormalFloatFont = true;


    /// <summary>
    /// 是否开启光效
    /// </summary>
    [System.NonSerialized]
    public bool EnabledLightingEffect = true;

    [System.NonSerialized]
    public bool EnabledImageEffects = true;

    [System.NonSerialized]
    public bool EanableUI = true;
    #endregion



    /// <summary>
    /// 优化选择特效
    /// </summary>
    private void OptimitzedSelectedEffect()
    {
        if (!selectedEffectCacheRoot)
        {
            selectedEffectCacheRoot = new GameObject();
            selectedEffectCacheRoot.name = "selectedEffectCacheRoot";
            DontDestroyOnLoad(selectedEffectCacheRoot);
        }
        string SelectedEffectPatch = "Effect/selectEffect";
        selectedEffectsAssets.AddRange(Resources.LoadAll(SelectedEffectPatch));
        foreach (UnityEngine.Object o in selectedEffectsAssets)
        {
            if (o.GetType() == typeof(GameObject))
            {
                GameObject obj = ResNode.InstantiateRes(o);
                obj.transform.SetParent(selectedEffectCacheRoot.transform);
                selectedEffectsInstance.Add(obj);
                obj.SetActive(false);
            }

        }
    }


    private void ClearSelectedEffect()
    {
        foreach (UnityEngine.Object o in selectedEffectsAssets)
        {
            if (o.GetType() != typeof(GameObject))
            {
                Resources.UnloadAsset(o);
            }
        }

        foreach (UnityEngine.Object o in selectedEffectsInstance)
        {
            UnityEngine.Object temp = o;
            ResNode.DestroyRes(ref temp);
        }

        if (!selectedEffectCacheRoot)
        {
            ResNode.DestroyRes(ref selectedEffectCacheRoot);
            selectedEffectCacheRoot = null;
        }

        selectedEffectsAssets.Clear();
        selectedEffectsInstance.Clear();
    }

}