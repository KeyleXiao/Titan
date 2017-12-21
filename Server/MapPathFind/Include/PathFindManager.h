/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\MapPathFind\Src\PathFindManager.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/12/2015	16:27
** 版  本:	1.0
** 描  述:	
            寻路服务管理
			在ServerGlobal中参照m_pSceneManager加入
			在scene_services.xml中加入
			<Service name="PathFindService" dll_path="MapPathFind.dll" create_func="CreatePathFindService" thread_safe="0" stack_size="4096" time_slice="32" thread_id="-1" is_prefab="1">
			</Service>
********************************************************************/

#pragma once

#include "IPathFindService.h"
#include "IPathFindManager.h"


//#include <map>
class HeightFieldMgr;


class PathFindManager : public IPathFindManager
{
public:
	// IPathFindManager
	//////////////////////////////////////////////////////////////////////////
	virtual bool				Load();
    virtual void				Release();

    /************************************************************************/
    /* 以下接口供客户端使用 NOTE:仅限于单线程框架上使用                            */
    /************************************************************************/
    virtual IPathFindService*   createServiceInterface( int nMapID,int nSceneID,BYTE mode );
    virtual void                releaseServiceInterface( IPathFindService *& pInterface );

    /************************************************************************/
    /* 以下接口供服务端使用                                                    */
    /************************************************************************/
	virtual bool				GetPathFromMapID( int nMapID,int nSceneID,
                                                  Vector3 const& v3Start,Vector3 const& v3End, vector<Vector3>& vecRet,
                                                  int nFlag = 0x0, int nCamp = 0x0,
                                                  float fWidthMin = 0 );
    virtual bool                checkObstacle(int nMapID, int nSceneID, Vector3 const& v3Pos, const float *verts, const int nVerts,
                                              int nFlag = 0x0, int nType = 0x0);
	virtual bool				getHeight( int nMapID,int nSceneID,Vector3* v3Pos,float& fHeight );
};