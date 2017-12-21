/*******************************************************************
** 文件名: MiscManager.cpp
** 版  权: (C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人: 李界华
** 日  期: 6/26/2017
** 版  本: 1.0
** 描  述: 

********************************************************************/

#include "stdafx.h"
#include "Vector3.h"
#include "IClientGlobal.h"
#include "SchemeMapInfo.h"
#include "ISchemeCenter.h"
#include "ActorClient.h"
#include "NetServiceProvider.h"

#include "MiscManager.h"


/////////////////////////////////////////IMiscManager/////////////////////////////////////////

CMiscManager::CMiscManager()
    : m_isContinuePlay(false)
{

}

/** 释放
*/
void CMiscManager::release()
{
    m_isContinuePlay = false;
         
    delete this;
}

/** 获取当前的地图ID
*/
int CMiscManager::getMapID(void)
{
    return ActorClient::getInstance().getMapID();
}

/** 加载场景
*/
bool CMiscManager::loadScene(int nCmdID, int nMapID, float fBornX/*=0.0f*/, float fBornY/*=0.0f*/, float fBornZ/*=0.0f*/)
{
    SMapSchemeInfo* pSchemeMap = gClientGlobal->getSchemeCenter()->getSchemeMapInfo()->GetMapSchemeInfo(nMapID);
    if(pSchemeMap == NULL)
    {
        ErrorLn("load scene fail! map="<< nMapID);
        return false;
    }

    game_LoadSceneInfo info;

    info.nCmdID     = nCmdID;
    info.nMapID     = nMapID;
    info.nSoundID   = pSchemeMap->lBackMusicID;
    info.fBornPosX  = fBornX;
    info.fBornPosY  = fBornY;
    info.fBornPosZ  = fBornZ;
    info.nXAxisLen  = pSchemeMap->nXAxisLen;     // x轴长度
    info.nZAxisLen  = pSchemeMap->nZAxisLen;     // z轴长度
    info.fOffx      = pSchemeMap->vOffset.x;    //
    info.fOffy      = pSchemeMap->vOffset.y;    //
    info.fOffz      = pSchemeMap->vOffset.z;    //
    sstrcpyn(info.strSceneName, pSchemeMap->szMapFile, sizeof(info.strSceneName));

    gClientGlobal->getRenderView()->sendControllerCommand(nCmdID, 0, "", &info, sizeof(info));

    return true;
}

/** 预加载皮肤列表
*/
bool CMiscManager::preloadSkin(const int * nArraySkinID, size_t nCount)
{
    if(nArraySkinID == NULL || nCount == 0)
    {
        return false;
    }

    obuf obSkinData;

    cmd_count cmdCount;
    cmdCount.nCount = nCount;
    obSkinData.push_back(&cmdCount, sizeof(cmd_count));

    for (int i=0; i<nCount; ++i)
    {
        const int & nSkinID = nArraySkinID[i];
        obSkinData.push_back(&nSkinID, sizeof(nSkinID));
    }

    gClientGlobal->getRenderView()->sendControllerCommand(GVIEWCMD_PRELOAD_SKIN_LIST, 0, NULL, obSkinData.data(), obSkinData.size());

    return true;
}

void CMiscManager::SetContinuePlay(bool isContinue)
{
    m_isContinuePlay = isContinue;
}

bool CMiscManager::IsContinuePlay()
{
    return m_isContinuePlay;
}

