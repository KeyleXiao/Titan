using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;




// 显示层实体数值更新属性
public struct entity_view_update_Numproperty
{
    public int nEntityID;	        // 显示层的实体ID
    public int nPropID;			    // 属性ID
    public int nValue;				// 属性数值
    public int nBeforeValue;        // 更新前数据
    public int nOtherData;			// 状态数据
    public int nPBUID;			// PBUID
}

/// <summary>
///  数值属性实时更新消息同步处理
///  使用消息事件实现
/// </summary>
public class EntityNumPropEventArgs : System.EventArgs
{
    public entity_view_update_Numproperty data;   // 显示层实体数值更新属性
    public U3D_Render.EntityView objev = null;    // 实体对象              
}

/// <summary>
/// 主英雄被创建事件
/// </summary>
public class CreateMainHeroEntityEventArgs:System.EventArgs
{
    /// <summary>
    /// 主英雄ID，非实体ID
    /// </summary>
    public int MainHeroID;
    /// <summary>
    /// 实体ID
    /// </summary>
    public int MainHeroUID;
	/// <summary>
	/// 比赛类型ID
	/// </summary>
	public uint nMatchTypeID;
}

