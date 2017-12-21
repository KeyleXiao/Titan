/*******************************************************************
** 文件名:	SceneManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	陈涛 (Carl Chen)
** 日  期:	8/30/2014
** 版  本:	1.0
** 描  述:	
            场景管理器
********************************************************************/

#pragma once

#include "ISceneManager.h"
#include "AOI_Forest.h"
#include "fast_id_map.h"
#include <map>

class SceneManager : public ISceneManager
{
public:
    struct SceneInfo
    {
        int             nMapID;
        int             nSerialNo;
        SERVICE_PTR     pService;

        SceneInfo() : nSerialNo(0), nMapID(INVALID_MAPID)
        {
        }

        void clear()
        {
            nSerialNo = 0;
            nMapID = INVALID_MAPID;
            pService = SERVICE_PTR(0);
        }
    };

	//////////////ISceneManager/////////////////////////////////////////////////////////////
	virtual void onStop();

	virtual void release();

	virtual bool load();

	virtual int createScene(SCreateSceneContext context, void* pUserData = NULL, int len = 0);

	virtual bool closeScene( int sceneID );

    // 场景service真正退出 回调
    virtual void onSceneClosed( int sceneID );

	virtual SERVICE_PTR getScene( int sceneID );

	virtual int getSceneCount();

	virtual int getAllScenes( SERVICE_PTR * pReturnArray,int nArraySize );

    virtual int getAllSceneID( int* pReturnArray, int nArraySize );

	virtual void clearAllScenes();

    virtual int sceneIDToMapID( int sceneID );

    virtual int sceneIDToSerialNo( int sceneID );

    // 获取本场景服开启的主城镜像地图区间值
    virtual void getMirrorMapID(BYTE& nMin, BYTE &nMax);

	//////////////SceneManager/////////////////////////////////////////////////////////////
    SceneManager();

    ~SceneManager();
    // 根据地图ID获取全局草丛对象
    AOI_Forest * getGlobalForest( int mapID );

private:
    // 分配一个新的场景ID(只用于动态场景)，需要保证不同场景间的唯一性
    bool allocSceneID( int & sceneID );

    // 归还一个场景ID
    void revertSceneID( int sceneID );

private:
    // id生成器
    fast_id_map<int, int >  m_idAllocator;

    // 本场景服中主城平衡分流地图ID区间
    BYTE                    m_nMinMirrorMapID;
    BYTE                    m_nMaxMirrorMapID;

    // SceneID是唯一
    SceneInfo               m_scenesList[MAX_SCENE_COUNT];
    volatile	LONG        m_nSceneCount;

    typedef std::map< int, AOI_Forest* > T_FORESTS;
    T_FORESTS               m_forestMap;

    volatile LONG           m_mutex;

    volatile LONG           m_nCurSerialNo;
};
