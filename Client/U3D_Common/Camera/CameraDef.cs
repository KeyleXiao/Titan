using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;


/// <summary>
/// 相机模式
/// </summary>
public enum CameraMode
{
    CameraAnim,                   //摄像机动画模式
    MapGodControl,               //上帝模式（小地图）
    DeadControl,                   //死亡模式
    EndWarControl,               //战场结束摄像机移动
    FollowTarget,                   //跟随模式
    SniperscopeControl,        //狙击模式 
    FPSMode,                        //FPS英雄模式
    FloatingControl,              //飞行模式
    RidingControl,                 //坐骑模式（龙女飞天）
    TopControl,                     //V视角模式 
    UserControl,                    //用户控制模式
    OBHeroLock,                  //OB模式-英雄锁定
    OBMoveFree,                   //OB模式-自由移动

#if ENABLE_YUNYING_MODE
    YUNYING_OBHeroTrace,                 //YUNYING_OB模式-英雄跟踪
    YUNYING_OBHeroLock,                  //YUNYING_OB模式-英雄锁定
    YUNYING_OBMoveFree,                   //YUNYING_OB模式-自由移动
#endif
    MaxCount
}