/*******************************************************************
** 文件名:	E:\work\w1\speed\Server\SchemeCenter\Src\SchemePathFindService.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	吴欣伟
** 日  期:	1/2/2016	18:36
** 版  本:	1.0
** 描  述:	
            动态障碍配置表
********************************************************************/

#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "EntityDef.h"
#include "SchemeDef.h"
#include "SchemeCache.h"

using namespace std;

class CSchemeDynamicObstacle : public IDynamicObstacleInfo, 
                               public ISchemeUpdateSink,
                               public CSchemeCache<CSchemeDynamicObstacle>
{
    enum EMDynamicObsSchemeColunm
    {
        OBS_COL_ID = 0,						    // 动态障碍怪ID

        OBS_COL_SHAPE,							// 动态障碍形状
        OBS_COL_LENGTH,                         // 如果是四边形则是长度，如果是六边形则是六边形的内接圆半径
        OBS_COL_WIDTH,                          // 如果是四边形则是宽度，如果是六边形则是六边形的内接圆半径,与上面的值一样 
        OBS_COL_ANGLE,                          // 动态障碍旋转角度
        OBS_COL_ENTITY_CAMP_FLAG,				// 动态障碍实体是否可穿越标志位
        OBS_COL_SPELL_CAMP_FLAG,				// 动态障碍技能阵营标志位
        OBS_COL_SPELL_FLAG,						// 动态障碍技能是否可穿越标志位
		OBS_COL_PRELOAD_FLAG,				    // 动态障碍是否预加载（允许在客户端创建前生成）

        OBS_COL_MAX,                            // 动态障碍最大列数
    };

	typedef map<unsigned int, DynamicObstacleScheme>	T_SchemeInfo;
public:
	///////////////////////////////IDynamicObstacleInfo///////////////////	
	virtual DynamicObstacleScheme * GetSchemeInfo(int nDynamicMonsterID);

	///////////////////////////////ISchemeUpdateSink///////////////////	
	virtual bool OnSchemeLoad(SCRIPT_READER reader, const char* szFileName);
	virtual bool OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	///////////////////////////////CSchemePathFindServiceInfo//////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void Close(void);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeDynamicObstacle(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeDynamicObstacle(void);

private:
	// 所有寻路服务配置属性	
	T_SchemeInfo m_mapSchemeInfo;
};
