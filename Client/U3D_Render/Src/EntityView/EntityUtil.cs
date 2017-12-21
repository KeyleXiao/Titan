using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using GameLogic;



public class EntityUtil
{
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

        return EntityFactory.MainHeroView.Property.GetNumProp(ENTITY_PROPERTY.PROPERTY_VOCATION);
    }
}
