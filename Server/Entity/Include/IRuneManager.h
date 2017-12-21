/*******************************************************************
** 文件名: IRuneManager.h 
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 8/16/2017
** 版  本: 1.0
** 描  述: 符文系统管理接口

********************************************************************/
#ifndef __I_RUNEMANAGER_H__
#define __I_RUNEMANAGER_H__

struct EntityBelongContext;

struct IRuneManager
{
    // 添加拥有符文数据
    virtual bool addRuneStoreInfo(int nRuneId,int nCount = 1) = 0;

    // 判断下是否能添加符文
    virtual bool canAddRuneStoreInfo(int nRuneId,RUNE_BUY_ERROR_TYPE& eErrorType,int nCount = 1) = 0;

    // 获取当前符文数据
    virtual bool getOtherRuneData(obuf & outData) = 0;

    // 获取其他玩家当前符文数据
    //virtual bool getOtherCrownPageData(obuf & outData) = 0;

    // 获取当前符文页
    virtual int getCurRunePage() = 0;
};

#endif // __I_RUNEMANAGER_H__