/*******************************************************************
** 文件名:	PathFindDef.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	2015-05-20
** 版  本:	1.0
** 描  述:	寻路模块公共定义以及动态障碍消息定义
********************************************************************/


#pragma once

#include "Vector3.h"
#include "GameDef.h"
// 输出DEBUG信息
#ifdef VERSEION_DEBUG
 #   define PATH_FIND_DEBUG
#endif
#ifdef PATH_FIND_DEBUG
#	define PATH_TRACELN(x)		TraceLn(x)
#	define PATH_ERRORLN(x)		ErrorLn(x)
#else
#	define PATH_TRACELN(x)
#	define PATH_ERRORLN(x)
#endif


#define PATHFIND_MSC_POLYREF_MAX			1000
#define PATHFIND_MSC_STRAIGHTPATH_MAX		1000
#define PathAgentNeighbourMaxCount			10

#define AREA_FLAG_NO_PASS				    0x0
#define AREA_FLAG_ALL_PASS					0xffff


// 状态
enum AgentState
{
    AgentState_None = 0,		// 0未使用
    AgentState_Stop,		    // 1停着
    AgentState_Walk,		    // 2走路中
};

const int   AREA_ET_MAX_BIT         = 4;
const int   AREA_SP_MAX_BIT         = 2;

const int CheckSingle = 1;
const int CheckMulti = 2;

enum Area_Flag
{
    AREA_NO_PASS            = 0,        /* 不能通过 */

    // 实体占1-4位
    AREA_ET_SELF_PASS       = 0x0001,      /* 实体（英雄和怪物）-仅我方可走 */
    AREA_ET_ENEMY_PASS      = 0x0002,      /* 实体（英雄和怪物）-仅敌方可走 */
    AREA_ET_NEUTRAL_PASS    = 0x0004,      /* 实体（英雄和怪物）-中立可走 */
    AREA_ET_RESERVE         = 0x0008,      /* 实体-保留 */

    // 技能占5-8位
    AREA_SP_SELF_PASS       = 0x0010,      /* 技能-仅我方可走 */
    AREA_SP_ENEMY_PASS      = 0x0020,      /* 技能-仅敌方可走 */
    AREA_SP_HAS_EFFECT      = 0x0040,      /* 技能-控制能对障碍起作用 */
    AREA_SP_RESERVE         = 0x0008,      /* 技能-保留 */

    AREA_CAN_PASS           = AREA_ET_SELF_PASS | AREA_ET_ENEMY_PASS | AREA_ET_NEUTRAL_PASS | AREA_ET_RESERVE | AREA_SP_SELF_PASS | AREA_SP_ENEMY_PASS,  //0x3F,       /* 可通过 */

    // 阵营占3位(9-11位)
    AREA_CAMP_BIT_1         = 0x0100,       /* 阵营 */
    AREA_CAMP_BIT_2         = 0x0200,       /* 阵营 */
    AREA_CAMP_BIT_3         = 0x0400,       /* 阵营 */
};

static const int MAX_TIMER = 128;

enum ObstacleTimer
{
    ETIMER_UPDATE_PATH = 0,
    ETIMER_UPDATE_PATH_MANUAL = 1,
    ETIMER_OBSTACLE_REMOVE = 2,
};

enum TileAction 
{
    ADD = 0,
    REMOVE = 0,
};

enum SC_PATH_NET_MESSAGE
{
    SC_PATH_NET_INVALID = 0,     // 无效消息
    SC_PATH_NET_ADD_OBSTACLE,    // 通知客户端添加动态障碍物
    SC_PATH_NET_REMOVE_OBSTACLE, // 通知客户端删除指定障碍物
	SC_PATH_NET_GET_NAVMESH_HEAD,
	SC_PATH_NET_GET_NAVMESH,
    SC_PATH_NET_MAX,             // 最大消息
};

// 记录保存在PathFind服务中的障碍物所关联的怪物实体信息
struct EntityObsInfo
{
    UID                 entityUid;
    int                 nFlag;          // 怪物实体的障碍物标志位
    unsigned int        polygonRef;     // 多边形引用
};

struct AgentNeighbour
{
	int		m_nIndex;		
	float	m_fDist;

	AgentNeighbour()
	{
		memset(this, 0, sizeof(*this));
	}
};
//-------------------------动态障碍相关--2016-08-26-嘉文----------------//

struct dtObstacleCylinder
{
	float pos[3];
	float radius;
	float height;
	float temp;		//补位用
};

struct dtObstacleBox
{
	float bmin[3];
	float bmax[3];
};

enum ObstacleType
{
	DT_OBSTACLE_CYLINDER,
	DT_OBSTACLE_BOX,
};

struct OnlyPhysicsObstacle
{
	union {
		dtObstacleCylinder cylinder;
		dtObstacleBox box;
	};
	ObstacleType type;
	int nFlag;
	int salt;
	UID uid;
	float angle;			//逆时针旋转
	unsigned char nCamp;
};

struct dtTileObstacle
{
	union {
		dtObstacleCylinder cylinder;
		dtObstacleBox box;
	};
	int* keyTouched;
	ObstacleType type;
	int nTouched;
	int nFlag;
	int salt;
	UID uid;
	float angle;			//逆时针旋转
	dtTileObstacle* next;	//用于组建障碍物的链表
	bool isPreload;
	unsigned char nCamp;

	bool ContainKey(int key)
	{
		for (int i = 0; i < nTouched; i++)
		{
			if (keyTouched[i] == key)
				return true;
		}
		return false;
	}
	dtTileObstacle() { };
	dtTileObstacle(const dtTileObstacle* ob,Vector3 curPos,float angleY)
	{
		switch (ob->type)
		{
		case DT_OBSTACLE_BOX:
			box.bmin[0] = ob->box.bmin[0] + curPos[0];
			box.bmin[1] = ob->box.bmin[1] + curPos[1];					//高度先不考虑
			box.bmin[2] = ob->box.bmin[2] + curPos[2];
							
			box.bmax[0] = ob->box.bmax[0] + curPos[0];
			box.bmax[1] = ob->box.bmax[1] + curPos[1];						//高度先不考虑
			box.bmax[2] = ob->box.bmax[2] + curPos[2];
			uid = ob->uid;
			type = DT_OBSTACLE_BOX;
			nFlag = ob->nFlag;
			nCamp = ob->nCamp;
			isPreload = false;
			angle = angleY;
			break;
		case DT_OBSTACLE_CYLINDER:
			cylinder.pos[0] = ob->cylinder.pos[0] + curPos[0];
			cylinder.pos[1] = ob->cylinder.pos[1] + curPos[1];
			cylinder.pos[2] = ob->cylinder.pos[2] + curPos[2];
			cylinder.radius = ob->cylinder.radius;
			cylinder.height = ob->cylinder.height;
			uid = ob->uid;
			type = DT_OBSTACLE_CYLINDER;
			nFlag = ob->nFlag;
			nCamp = ob->nCamp;
			isPreload = false;
			break;
		}
	}
};


#pragma pack(1)
struct SObstacleNode
{
	UID				EntityUid;		// 障碍物实体UID
	int				nFlag;			// 障碍物有效性标志flag
	float			data[6];		// 障碍物数据
	BYTE			type;			// 障碍物形状类型 ObstacleType
	BYTE			nCamp;			// 障碍物阵营
	bool			isPreload;		// 是否需要需处理
	float			angle;		    // 障碍物旋转角度

	SObstacleNode()
	{
		memset(this, 0, sizeof(*this));
	}
	SObstacleNode(dtTileObstacle* info)
	{
		memset(this, 0, sizeof(*this));
		EntityUid = info->uid;
		memcpy(data, &info->box, sizeof(float) * 6);
		nCamp = info->nCamp;
		nFlag = info->nFlag;
		isPreload = info->isPreload;
		type = info->type;
		angle = info->angle;
	}
};
typedef SObstacleNode MsgPathObstacleInfo;
#pragma pack()

//-------------------------动态障碍相关--2016-08-26-嘉文----------------//
//----------------------------------END---------------------------------//