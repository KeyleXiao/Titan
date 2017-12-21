/*******************************************************************
** 文件名:	FloatFontManager.cs
** 版  权:	(C) 深圳冰川网络技术有限公司 2015 - Speed
** 创建人:	谭强均
** 日  期:	2015/3/23
** 版  本:	1.0
** 描  述:	飘字管理器
** 应  用:  用于管理飘字
 *
**************************** 修改记录 ******************************
** 修改人: 昔文博
** 日  期: 2016/4/7
** 描  述: 暴击飘字动画时间延长
 *
********************************************************************/
using UnityEngine;
using System.Collections;
using Scaleform;
using System.Collections.Generic;

public enum FloatFontType
{
    /// <summary>
    /// 对敌普通伤害
    /// </summary>
    TYPE_ENEMY_DAMGE = 0,

    /// <summary>
    /// 治疗
    /// </summary>
    TYPE_CURE,

    /// <summary>
    /// 对敌暴击伤害 
    /// </summary>
    TYPE_ENEMY_FATAL,

    /// <summary>
    /// 自身受到伤害
    /// </summary>
    TYPE_SELF_DMAGE,

    /// <summary>
    /// 天赋飘字
    /// </summary>
    TYPE_TLANT_POINT,

    /// <summary>
    /// 升级飘字
    /// </summary>
    TYPE_LEVELUP,

}

public enum FloatTextType
{
    /// <summary>
    /// 以系统文字显示
    /// </summary>
    TYPE_SYSTEM_TEXT = 1,
    /// <summary>
    /// 以美术文字显示
    /// </summary>
    TYPE_ART_TEXT
}


/// <summary>
/// 飘字管理器
/// </summary>
public class FloatFontManager
{

    #region API
    /// <summary>
    /// 创建一个飘字，成功返回实例，失败返回null
    /// </summary>
    /// <param name="entry">实体</param>
    /// <param name="text">飘字的内容，目前只能是数字，像暴击、躲闪之类的尚未实现</param>
    /// <param name="type">飘字的类型</param>
    /// <param name="followEntity">跟随实体</param>
    /// <returns></returns>
    public static FloatFontNode CreateFloatFont(GameObject entry, string text, FloatFontType type = FloatFontType.TYPE_ENEMY_DAMGE,bool followEntity = false)
    {
        FloatFontNode tempNode = null;
        if (!bInit)
        {
            Trace.LogError("FloatFontManager必须初始化！");
            return tempNode;
        }

        if (SFGFxMovieManager.Instance == null)
        {
            Trace.LogError("SFGFxMovieManager没有初始化，请不要在Awake中创建！");
            return tempNode;
        }
        //有可能FloatFontMovie还没有创建完成.
        if (Movie.GetSingleton <FloatFontMovie>() == null)
        {
            //Trace.LogError("FloatFontMovie尚未创建完成，请等待创建完成后再调用，请不要在Awake或者Start中创建");
            return tempNode;
        }
        //有可能FloatFontMovie还没有创建完成.
        if (FloatFontMovie.Instance == null)
        {
            //Trace.LogError("FloatFontMovie尚未创建完成，请等待创建完成后再调用，请不要在Awake或者Start中创建");
            return tempNode;
        }

        if (!NeedToCreate(entry))
        {
            return null;
        }

        if (UnActiviteList.Count > 0)
        {
            tempNode = UnActiviteList.First.Value;
            UnActiviteList.RemoveFirst();
            EnableFloatNode(tempNode);

            tempNode.ChangeHostEntity(entry);
            tempNode.ChangeType(type);
            tempNode.SetText(text);
            tempNode.UpdatePos();
        }
        else
        {
            tempNode = CreateNewNode(entry);
            if (null == tempNode)
            {
                return null;
            }
            
            tempNode.ChangeType(type);
            tempNode.SetText(text);
            tempNode.UpdatePos();
            tempNode.SetEnable(true);
           
            ActiviteList.AddLast(tempNode);
        }
        tempNode.bFollowEntity = followEntity;
        return tempNode;
    }

    public static void Init()
    {
        if (!bInit)
        {
            EntityFactory.SetEntityNumPropHandler(ResponseEntityNumPropEvent);

            //各个飘字类型的持续时间，与flahs里的持续时间对应
            FloatFontLifeTime.Add(FloatFontType.TYPE_CURE,0.5f);
            FloatFontLifeTime.Add(FloatFontType.TYPE_ENEMY_DAMGE, 0.5f);
            FloatFontLifeTime.Add(FloatFontType.TYPE_ENEMY_FATAL, 0.5f);
            FloatFontLifeTime.Add(FloatFontType.TYPE_SELF_DMAGE, 0.5f);
            FloatFontLifeTime.Add(FloatFontType.TYPE_TLANT_POINT, 0.5f);
            FloatFontLifeTime.Add(FloatFontType.TYPE_LEVELUP, 0.5f);
            bInit = true;
        }
    }

    public static void Clear()
    {
        if(!bInit)
        {
            return;
        }
        bInit = false;
        EntityFactory.RemoveEntityNumPropHandler(ResponseEntityNumPropEvent);
        FloatFontLifeTime.Clear();
        DestroyALL();

    }

    //数值属性回调函数
    public static void ResponseEntityNumPropEvent(EntityNumPropEventArgs e)
    {
        switch (e.data.nPropID)
        {
            case (int)ENTITY_PROPERTY.PROPERTY_TALENT_POINT:			// 天赋
                {
                    int deltaNum= e.data.nValue - e.data.nBeforeValue;
                    if (deltaNum > 0)
                    {
                        GameObject entity = EntityFactory.getEntityGameObjectByID(e.data.nEntityID);
                        FloatFontManager.CreateFloatFont(entity, deltaNum.ToString(),FloatFontType.TYPE_TLANT_POINT);
                    }
                }
                break;
            default:
                break;
        }

    }


    public static void Update()
    {
        if (!bInit)
        {
            return;
        }
        if (WillRemoveActiviteList.Count > 0)
        {
            //先移除上一帧需要移除的对象
            foreach (FloatFontNode ff in WillRemoveActiviteList)
            {
                ActiviteList.Remove(ff);
                //Destroy(ff);
            }
            WillRemoveActiviteList.Clear();
        }

        foreach (FloatFontNode ff in ActiviteList)
        {
            
            ff.OnUpdate();
        }
    }

    /// <summary>
    /// 禁用飘字结点
    /// </summary>
    /// <param name="ins"></param>
    public static void DisableFloatNode(FloatFontNode ins)
    {
        WillRemoveActiviteList.AddLast(ins);
        UnActiviteList.AddLast(ins);
        ins.SetEnable(false);
        ins.OnDisabled();
        
    }

    /// <summary>
    /// 启用飘字结点
    /// </summary>
    /// <param name="ins"></param>
    public static void EnableFloatNode(FloatFontNode ins)
    {
        ActiviteList.AddLast(ins);
        UnActiviteList.Remove(ins);
        ins.SetEnable(true);
        ins.OnEnabled();
    }

    public static void DestroyALL()
    {
        if (Movie.GetSingleton<FloatFontMovie>() == null)
        {
            return;
        }
        foreach (FloatFontNode ff in ActiviteList)
        {
            Destroy(ff);
        }
        foreach (FloatFontNode ff in UnActiviteList)
        {
            Destroy(ff);
        }
        WillRemoveActiviteList.Clear();
        ActiviteList.Clear();
        UnActiviteList.Clear();
    }

    /// <summary>
    /// 摧毁指定的组件
    /// </summary>
    /// <param name="instance">需要清除的Blood</param>
    public static void Destroy(FloatFontNode instance)
    {
        if (Movie.GetSingleton<FloatFontMovie>() == null)
        {
            return;
        }
        if (null == instance)
        {
            return;
        }
        if (FloatFontMovie.Instance != null)
        {
            FloatFontMovie.Instance.RemoveMovieAS3(instance.Instance);
        }
        instance.Clear();
        instance = null;
    }

    #endregion

    #region 属性

    private static bool bInit = false;

    private const string FloatFontAS3RefrenceName = "Com.Scaleform.FloatFont.FloatFont";

    /// <summary>
    /// 活动列表
    /// </summary>
    private static LinkedList<FloatFontNode> ActiviteList = new LinkedList<FloatFontNode>();

    /// <summary>
    /// 非活动列表
    /// </summary>
    private static LinkedList<FloatFontNode> UnActiviteList = new LinkedList<FloatFontNode>();

    /// <summary>
    /// 将要从活动列表移除的物体
    /// </summary>
    private static LinkedList<FloatFontNode> WillRemoveActiviteList = new LinkedList<FloatFontNode>();

    static int index = 0;

    /// <summary>
    /// 超过这个距离就不创建了。
    /// </summary>
    private const float HideInDistance = 30.0f;

    /// <summary>
    /// 各个飘字类型的生命周期
    /// </summary>
    public static Dictionary<FloatFontType, float> FloatFontLifeTime = new Dictionary<FloatFontType, float>();
    
    #endregion

    #region 私有方法

    /// <summary>
    /// 是否需要创建，主要是通过距离来判断
    /// </summary>
    /// <param name="entry"></param>
    /// <returns></returns>
    static bool NeedToCreate(GameObject entry)
    {
        //算出相机和人物的距离
        float dis = Vector3.Distance(entry.transform.position, Initialize.mainCam.transform.position);

        float CameraDis = SoldierCamera.MainInstance ? SoldierCamera.MainInstance.CurDistance : 8.0f;
        float hideinDis = HideInDistance + CameraDis;
        if (dis > hideinDis)//距离超过hideinDis，不创建
        {
            return false;
        }

        return true;
    }

    static FloatFontNode CreateNewNode(GameObject entry)
    {
        FloatFontNode temp = null;

        Value ins = new Value();
        if (!FloatFontMovie.Instance.AttachMovie(ref ins, FloatFontAS3RefrenceName, FloatFontAS3RefrenceName + index, -1))
        {
            ins = null;
            return temp;
        }
        index ++;
        temp = new FloatFontNode(entry,ins);
        return temp;
    }
    #endregion
}
