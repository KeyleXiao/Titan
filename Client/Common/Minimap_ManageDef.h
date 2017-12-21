#pragma once

#include "ManagedCode.h"

#pragma warning(disable:4482)

#pragma pack(push, 1)

namespace GameLogic
{
    // 小地图信息
    STRUCT SMinimapInfo
    {
        int     nMapID;                             // 地图ID
        float   fLeft;                              // 小地图左边框
        float   fRight;                             // 小地图右边框
        float   fTop;                               // 小地图上边框
        float   fBottom;                            // 小地图下边框
        sp_char_array(szDarkBg, 32)                 // 无视野背景
        sp_char_array(szLightBg, 32)                // 全视野背景
        sp_char_array(szBaseMap, 32)                // 地图底图
        sp_char_array(szDefaultRotation, 256)       // 旋转信息"camp;rotation;xOffset;yOffset;camp;rotation;xOffset;yOffset;camp;rotation;xOffset;yOffset;..."
        int  nOriRotation;                          // 不旋转时 相对原场景的旋转角度 
		sp_char_array(szAlertBtnPos, 32)			// 警告按钮位置"x;y"
		sp_char_array(szMarkBtnPos, 32)				// 标记按钮位置"x;y"

        STRUCT_INIT(SMinimapInfo)
    };

    // 小地图图标信息
    STRUCT SMinimapIcon
    {
        int nIcon;                                  // 图标ID 
        int nIsTower;                               // 是否是建筑物(0否 1是)
        sp_char_array(szFriendCampFg, 32);          // 友方前景图标  
        sp_char_array(szFriendCampBg, 32);          // 友方背景图标
        sp_char_array(szEnemyCampFg, 32);           // 敌方前景图标
        sp_char_array(szEnemyCampBg, 32);           // 敌方背景图标
        sp_char_array(szMidCampFg, 32);             // 中立方前景图标
        sp_char_array(szMidCampBg, 32);             // 中立方背景图标
        sp_char_array(szMainHeroFg, 32);            // 主角前景图标  
        sp_char_array(szMainHeroBg, 32);            // 主角背景图标
        int nShowType;                              // 显示类型(0,阵营显示;1,全局显示)
        STRUCT_INIT(SMinimapIcon)
    };

    // 小地图静态图标信息
    STRUCT SMinimapStaticIcon
    {
        int nIndex;                                 // 序号
        int nAutoCreate;                            // 是否自动创建
        sp_char_array(szName, 32)                   // 名称
        sp_char_array(szIconName, 32)               // 图标
        int nX;                                     // x坐标
        int nZ;                                     // z坐标
        sp_char_array(szTipStr, 256)                // 提示字符串     
        int nTipBgColor;                            // 提示背景颜色

        STRUCT_INIT(SMinimapStaticIcon)
    };
}

#pragma pack(pop)

using namespace GameLogic;