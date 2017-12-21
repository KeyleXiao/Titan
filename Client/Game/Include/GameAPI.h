/*******************************************************************
** 文件名:	GameAPI.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	李界华
** 日  期:	12/11/2014
** 版  本:	1.0
** 描  述:	C++导出API接口
			
********************************************************************/


#ifndef __GAME_GAMEAPI_H__
#define __GAME_GAMEAPI_H__

#ifdef GAME_EXPORTS
#define GAME_EXPORT		__declspec(dllexport)
#define GAME_API		extern "C" __declspec(dllexport)
#else
#define GAME_IMPORT		__declspec(dllimport)
#define GAME_API		extern "C" __declspec(dllimport)
#pragma comment(lib, "game.lib")
#endif

GAME_API void GetMainMonitorSize(int &width,int &height);

// 获取主窗口句柄
GAME_API HWND getMainWindHwnd();

GAME_API void OpenLog();

GAME_API void Start(void* pRenderEntry);

GAME_API void Stop();

GAME_API void onFrameUpdate();

GAME_API int GetCurNetClass();

/** 接受实体显示层发出的事件
@param entityID   : 实体ID
@param eventID    : 事件ID,具体定义见(EntityViewDef.h)
@param arg1       : 时间参数1
@param arg2       : 时间参数2
@return           : 是否接收成功
*/
GAME_API void onEntityEvent( ENTITY_ID entityID,int eventID,int nParam,const char * strParam,void * ptrParam, int len);

/** 接受显示层发出的事件
@param entityID   : 实体ID
@param eventID : 事件ID,具体定义见(GameViewDef.h)
@param arg1    : 时间参数1
@param arg2    : 时间参数2
@return        : 是否接收成功
*/
GAME_API void onGameViewEvent( int eventID,int nParam,const char * strParam,void * ptrParam, int len);


/** 输出调试信息
*/
GAME_API void traceLog(const char* msg,int level);

/** 获得当前Tick数
*/
GAME_API int getSyncTickCount();

/** 获得当前Tick数
*/
GAME_API int GetTickCountEx();

// 获取C风格字符串长度
GAME_API int getStringLen(const char* str);

// 获取小地图信息
GAME_API void* getMinimapInfo(int nMapID);

// 获取小地图图标信息
GAME_API void* getMinimapIconInfo(int nIconID);

// 根据地图ID 静态打点序号获得静态打点信息
GAME_API void* getMinimapStaticObjInfo(int nMapID, int nIndex); 

// 根据序号，获取下一个静态打点信息
GAME_API void* getNextMinimapStaticIcon(int nMapID, int nFromIndex);

// 获取角色后退速度因子
GAME_API float getHeroBackwardSpeedFactor(void);

// 获取角色左右移动速度因子
GAME_API float getHeroSidewaySpeedFactor(void);
// 获取角色天赋最大可使用数
GAME_API int getHeroTalentMaxUseCount(void);

// 获取网络延迟
GAME_API int getNetLatency(void);

// 获取网络包
GAME_API int getPendingCount(void);

// 查询网络服务供应商
GAME_API bool queryNetworkSerivceProvider(void);

// 获取路径
GAME_API bool findPath(void* pContext);

// 获取地面高度 最底下的一层可走层
GAME_API float getGroundLayer(float fPosX, float fPosZ, float fAgentHeight);

// 获取视野范围
GAME_API int getSight(int nHeroId);

// 检测FPS输出阀值，0将不输出
GAME_API int getAlertFPS(void);

 // 检测FPS统计均值间隔ms，默认1000，0.将不输出
GAME_API int getFPSInterVal(void);

// 获取语言目录名 
GAME_API const char* getLanguageDirName(void);

// 获得可被秒杀时的血条颜色
GAME_API int getCanseckillHpColor( int nMonsterId );

// 获取技能信息
GAME_API void* getSpellData(int nSpellID);

// 获取技能整形信息
GAME_API int getSpellIntData(int nSpellID, int nSpellDataIndex);

// 获取技能字符串信息
GAME_API const char* getSpellStrData(int nSpellID, int nSpellDataIndex);

// 获取技能对应的槽位
GAME_API int GetSlotIndexOfSpell(int nSpellID);

//获取快捷键描述(这个时候显示层无法获取配置结构，所以先获取一个属性用着)
GAME_API const char* getShortcutDes(int nIndex, int nType);

// 获取冷却时间
GAME_API int GetFreezeTime(DWORD dwClassID, DWORD dwFreezeID, int nIsShowFlag);

// 获取小怪监视视野
GAME_API int getMonsterDistance();

// 获取塔怪的监视视野
GAME_API int getTowerDistance();

// 获得效果死亡光效
GAME_API int getMonsterEffect();

// 获得英雄的最高星级
GAME_API int getHeroStarLvLimit(int nHeroID);

// 获得英雄升星需要金钱
GAME_API int getHeroUpgradeNeedGold(int nHeroID, int nStarLv);

// 获得英雄等级
GAME_API int getPlayerLv();

// 获取英雄对应的配置信息
GAME_API void* getPersonModel(WORD wHeroID); 

// 获得某个阵营的人数限制信息
GAME_API int getCampPersonNumRestriction(int nMatchTypeID, int nCamp);

// 获得总人数限制
GAME_API int getMatchAllPersonRestriction(int nMatchTypeID);

// 获得机器人战绩炫耀标志位
GAME_API int getAIMilitaryExploitsFlag();

// 当前地图是否禁止PK, 0.可PK,1.不能PK
GAME_API int isForbitPK();

// 当前地图是否启用实体右键菜单
GAME_API bool isEnableEntityRightMenu();

// 指定地图是否启用实体右键菜单
GAME_API bool isMapEnableEntityRightMenu(int nMapID);

// 当前地图启用快捷键方案配置序号
GAME_API int getHotkeyIndex();
// 当前地图启用快捷键方案配置序号
GAME_API int getMapHotkeyIndex(int nMapID);


// 主角是否在战场
GAME_API int isInWarScene();

// 主角是否成年人
GAME_API bool isAdult();

// 是否可用七龙珠功能
GAME_API int isOpenSupperStone();

// 根据EntityID取等级
GAME_API int getPlayerLevel(ENTITY_ID viewID);

// 获取英雄对应的攻击范围属性
GAME_API int getPersonPropAttackRange(int nHeroID); 

// 取英雄的星级ID
GAME_API int getHeroStarIconID(int nHeroID, int nStarLv);

// 取皮肤配置
GAME_API void* getHeroSkinConfig(int nSkinID);

// 取星级配置
GAME_API void* getHeroStarConfig(int nHeroID, int nStarLv);

// 是否拥有某个BUFF
GAME_API bool IsExistBuff(DWORD dwBuffID);

// 取得当前的比赛类型
GAME_API DWORD getCurRoomMatchType();

// 取得当前房间类型
GAME_API int getCurRoomType();

GAME_API int getCurrentRoomID();

// 加载SCP配置
GAME_API int LoadScp(const char* pszConfigName, void * &pBuff);
// 释放SCP配置
GAME_API void FreeScpBuff(void);

// 获取组队TeamID
GAME_API int getTeamID(void);

// 取得英雄解锁等级
GAME_API int getUnlockLevel(WORD wVocation);

// 获取实体的位置
GAME_API void* getEntityLocation(ENTITY_ID viewID);

// 获取玩家的userID
GAME_API DWORD getPlayerUserID(ENTITY_ID viewID);

// 名称是否存在屏蔽字
GAME_API bool ExistKeyWordOfName(const char* szInStr);

// 名称过滤屏蔽字
GAME_API int filterKeyWordOfName(const char* szInStr, const char * &szOutStr);

// 过滤屏蔽字
GAME_API int filterKeyWord(const char* szInStr, const char * &szOutStr);

// 获取玩家的PDBID
GAME_API DWORD getPlayerPDBID(ENTITY_ID viewID);

// 获取选人光效ID
GAME_API int getSelectEffectID(int nSkinID);

// 取贡献金币光效
GAME_API const char* toCoinLighting(int vocation, int type);

// 是否fps英雄
GAME_API bool checkIsFpsHero(WORD wVocation);

// 是否已实名认证
GAME_API bool isRealNameAuthed();

// 取英雄的星级框图标ID
GAME_API int getHeroStarFrameID(int nHeroID, int nStarLv);

// 取英雄的星级光效ID
GAME_API int getHeroStarEffectID(int nHeroID, int nStarLv);

//获取野怪头像ID
GAME_API int getMonsterHeadIconID(int nMonsterID);

// 打印Task相关信息
GAME_API void printTask(void);

// 获取连杀间隔时间
GAME_API int getTimeCountKillInterval(int nWarID);

// 获取战场ID
GAME_API int getConfigWarID();

// 战场结束后结束界面延迟显示时间（秒）
GAME_API int getWarEndWndDelayTime();

//是否是电脑PDBID
GAME_API bool bIsComputerPDBID(int nPDBID);

// 是否主城地图
GAME_API bool isMainMap(int nMapID);

// 取英雄的星级所需卡牌
GAME_API int getHeroStarNeedCard(int nHeroID, int nStarLv, bool bIsLifeHero);

// 获取主角来自世界信息
GAME_API DWORD getMasterFromGameWorldID();



// 获取当前登录的账号名
GAME_API const char* getActorClientAccount();

// 获取段位图片ID
GAME_API int getActorRankIconID(int nMatchType, int nRankScore);

GAME_API int getRankIconIDByGrade(int nMatchType, int nGrade);

GAME_API int getActorCurRankMaxStar(int nMatchType, int nRankScore);

GAME_API int getActorRankStar(int nMatchType, int nRankScore);

GAME_API const char* getActorRankName(int nMatchType, int nRankScore);

GAME_API const char* getActorRankNameByGrade(int nMatchType, int nRankGrade);

// 获取比赛类型的王者段位基础分
GAME_API int getMatchtTypeBaseKingRankScore(int nMatchType);

// 获取添加分数之后的分数 这里有牵扯到保底段位分 
GAME_API int getAddMatchScoreAfterScore(int nMatchType, int nRankScore, int nAddScore);

// 获取玩家拥有的该英雄的卡牌数量
GAME_API int getCardCount(int heroID);

// 获取主角战场使用的召唤师技能
GAME_API int getWarSummerID();

// 判断英雄是否可以升星
GAME_API bool isHeroCanUpStar(int heroID, int starLv, bool isLifeHero);

// 获取当前游戏模式
GAME_API int getCurGameMode();

// 获取当前游戏模式根据比赛类型
GAME_API int getGameModeByMatchType(int nMatchType);

// 获取比赛类型名称
GAME_API const char* getMatchNameByMatchType(int nMatchType);

//是否是GM
GAME_API bool bIsGM(int nPDBID);

// 获取客户端是否为续玩
GAME_API bool IsContinuePlay();

// 根据英雄ID获取技能ID
GAME_API void* GetSlotSpellID(int heroID);

// 根据等级获得战场内升级需要的经验
GAME_API int GetWarHeroNextLevelExp(int nEntityID);

// 联盟能否发布杯赛
GAME_API bool clanCanPublicLegendCup();

#endif	// __GAME_GAMEAPI_H__