/*******************************************************************
** 文件名: MiscManager.h
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 6/26/2017
** 版  本: 1.0
** 描  述: 

********************************************************************/
#pragma once

#include "IMiscManager.h"


class CMiscManager: public IMiscManager
{
public:
	/////////////////////////////////////////IMiscManager/////////////////////////////////////////
    /** 释放
    */
    virtual void        release() override;
    
    /** 获取当前的地图ID
    */
    virtual int         getMapID(void) override;

    /** 加载场景
    */
    virtual bool        loadScene(int nCmdID, int nMapID, float fBornX=0.0f, float fBornY=0.0f, float fBornZ=0.0f) override;

    /** 预加载皮肤列表
    */
    virtual bool        preloadSkin(const int * nArraySkinID, size_t nCount) override;

    /** 设定客户端是否为续玩
    */
    virtual void        SetContinuePlay(bool isContinue) override;

    /** 获取客户端是否为续玩
    */
    virtual bool        IsContinuePlay() override;

public:
    CMiscManager();

private:
    bool        m_isContinuePlay;

};