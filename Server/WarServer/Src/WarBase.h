/*******************************************************************
** 文件名: CWarBase.h
** 版  权: (C) 深圳冰川网络技术有限公司(www.weimingtech.com)
** 创建人: 李有红
** 日  期: 2017/11/1
** 版  本: 1.0
** 描  述: 战场基类-实现最基本的战场逻辑
** 应  用: 
**************************** 修改记录 ******************************
** 修改人:  李有红
** 日  期:  2017/11/5
** 描  述:  该类只实现基本的战场逻辑，方便以后改新的玩法
********************************************************************/
#pragma once

#include "EntityDef.h"
#include "IWarService.h"
#include "SchemeWarDef.h"
#include "WarMessageDef.h"
#include "IMonsterExec.h"
#include "ICamp.h"
#include "ITimeSyncService.h"
#include "IServerGlobal.h"
#include "WarDef.h"
#include "IWar.h"
#include "WarDef_Internal.h"
#include "IWarMgr.h"

class CWarDrop;
class CWarAddBuff;
class CWarLogRecord;
struct CureContext;

class CWarBase : public IWar
{
public:
	CWarBase();
	virtual ~CWarBase();

public:
	/////////////////////////////////IWarBase Start/////////////////////////////////////////
	// 初始化
	virtual bool Init() override;

	// 反初始化
	virtual void UnInit() override;

	// 开启战场
	virtual void Start() override;

	// 结束战场
	virtual void End() override;

	// 释放战场
	virtual void Release() override{};

	// 销毁战场
	virtual void destroy() override {};

	// 战场正式开始
	virtual void onWarRealStart() override {};

	// 初始化子功能
	virtual bool InitSubFunc() override { return true; };

	// 所有的玩家都已进入战场
	virtual void onAllActorEnter() override {};

	// 是否结束
	virtual bool isEnd() override;

	// 初始化
	virtual bool create(IWarService* pWarService, SWarCreateContext& context) override  { return true; };

	// 获取战场所有人物UID
	virtual int getAllPerson(UID *PersonArray, int nArraySize) override;

	// 获取场景ID
	virtual int getWarSceneID() override;

	// 获得战场类型ID
	virtual int getWarTypeId() override;

	virtual int getStaticID() override;

	// 获得战场的创建序列号 创建怪物使用
	virtual int getMapID() override;

	virtual int getWarID() override;

	virtual DWORD  getMatchTypeID() override;

	// 获得战场持续的时间
	virtual DWORD getWarContinueTime() override;

	// 把客户端添加进战场
	virtual bool ClientPrepareEnterWar(const SActorPreaperEnterWar& sActorPreaperEnterWar) override { return true; };

	// 玩家进入战场事件
	virtual void onActorEnterWar(const SActorEnterWarBaseInfo& sActorEnterWarBaseInfo) override {};

	// 设置杯赛信息
	virtual void setLegendCupInfo(const SMsgLegendCupRoomInfo& sLegendCupInfo) override {};

	// 广播玩家信息
	virtual bool broadCastDataToAllPlayer(BYTE byKeyAction, LPCSTR pszData, int nDataLen, int nCamp = BCASTALLCAMP, bool ToView = false) override;

	// 广播给其他阵营玩家 除了 nCamp 阵营 其他全部广播
	virtual bool broadCastDataToAllOtherCamp(BYTE byKeyAction, LPCSTR pszData, int nDataLen, int nCamp = BCASTALLCAMP, bool ToView = false) override;

	// 广播给其他同阵营玩家
	virtual bool broadCastDataToSameCamp(BYTE byKeyAction, LPCSTR pszData, int nDataLen, int nCamp, bool ToView = false) override;

	// 广播给有视野的玩家信息
	virtual bool broadCastDataToHaveSightPlayer(Vector3 vec, BYTE byKeyAction, LPCSTR pszData, int nDataLen, bool ToView = false) override;

	// 玩家信息更新到客户端
	virtual void bcastPersonInfo(UID uid, EWarPersonInfoUpdateType nUpdateType) override;

	// 发送数据到玩家客户端
	virtual void sendDataToClient(UID uid, BYTE byKeyAction, void* data, size_t len, bool bToView=false) override;

	virtual void setWinCamp(int nCamp) override;

	virtual int getWinCamp() override { return m_WarInfo.nWinCamp; };

	// 战场游戏时间（正式开始后计时,毫秒）
	virtual DWORD getWarPlayTime() override;

	/** 序列化战场内玩家的信息
	@param uid		: 玩家UID
	@param out		: 序列化的数据
	@param flag		: 序列化标识
	*/
	virtual void deSerialzed(UID uid, rkt::obuf & out, int flag) override {};

	// 获得战场DBKey
	virtual LONGLONG getWarDBKey() override;

	// 强制结束战场
	virtual void forceEndWar(int nCamp, bool bNormalEnd) override {};

	// 获取上下文
	virtual SWarCreateContext* getWarCreateContext() override { return &m_context; };

	// 获取战场信息
	virtual SWarInfo* getWarInfo() override { return &m_WarInfo; };

	// 根据uPBBID获得玩家当前的信息
	virtual SWPerson* getPlayerInfoByPDBIU(PDBID uPBBID) override;

	virtual int getPlayTimeMin() override;

	// 获取阵营列表
	virtual vector<int> getCampList() override;

	// 根据阵营ID获得阵营信息
	virtual SWarCamp* getWarCamp(int nCamp) override;

	// 根据uid获得玩家当前的信息
	virtual SWPerson* getPlayerInfo(UID uid) override;

	// client 的网络消息
	virtual bool onWarClientMsg(UID uidActor, BYTE byKeyAction, const char* pData, size_t nLen) override  { return true; }

	// 获得胜利条件
	virtual EWarEndReason getWarEndReason() override;

	// 设置结束原因
	virtual void setWarEndReason(EWarEndReason eReason) override;

	// 战场正常结束标示
	virtual void setNormalEnd(bool bNormalEnd) override { m_WarInfo.bNormalEnd = bNormalEnd; };

	// 救治队友
	virtual void cureMember(UID uid) override {};

	// 玩家复活
	virtual void onEntityRelive(UID uid)override;

	// 获取管理器
	virtual __IWarMgr* getWarMgr(int nWarMgrID)override;

	/** 获取战场指定类型UID
	@param byGetCampType		: 阵营类型 EWarCampType
	@param bySerchType		    : 查找类型 EWarMonsterExec
	*/
	virtual DWORD warSerchTypeUIDList(BYTE bySelfCamp, BYTE byGetCampType, BYTE bySerchType, PDBID* pReturnArray, DWORD dwArrayMaxSize) override { return 0; }

	/** 获取战场指定类型UID
	@param byGetCampType		: 阵营类型 EWarCampType
	@param bySerchType		    : 查找类型 EWarMonsterExec
	*/
	virtual void warAddSerchTypeBuff(SWarEffectAddBuff sData) override { return; }
	/////////////////////////////////IWarBase End/////////////////////////////////////////

protected:
	// 获取显示添加经验金币配置
	bool getWarShowAddEXPAndMoney() { return m_WarInfo.bIsWarShowAddEXPAndMoney; };

	// 获取战场当前流程
	EWarPhase getWarPhase() { return m_WarInfo.WarPhase; };
	void setWarPhase(EWarPhase WarPhase) { m_WarInfo.WarPhase = WarPhase; };

	// 战场开始时间，用于记录战场进行了多久
	DWORD getWarStarTime() { return m_WarInfo.nStartTimeTick; };

	// 获取阵营所有玩家的pdbid字符串（提供给任务系统-lua层）
	const void getCampPdbidString(int nCamp, char* pdbidStr, size_t nLen);

	// 停止电脑AI
	void stopComputerAI();


private:
	// 生成战场唯一id
	LONGLONG createDBWarId(int nSceneId);

protected:
	////////////////////////////////客户端消息//////////////////////////////////////////
	// 客户端请求战场信息
	void onClientReqWarBaseInfo(UID uid, void* pData, size_t stLen);

protected:

	// 发送数据到客户端
	void sendAllPersonInfoToAllClient();

    // 广播数据到客户端
    void sendToOpenTabInfoClient(PDBID uNeedUpdateActor, SWPerson& sWPerson);


	// 发送所有已经创建好的角色的uid到客户端
	void sendMsgAllUidToClient();

	// 发送战场信息
	void sendWarBaseInfoToClient(UID uid);

	// 发送玩家基本信息
	void sendPlayerBaseInfoToClient(UID uId);

	// 发送战绩数据
	void sendWarRecordInfo(UID uid);

protected:
	// 战场基本信息
	SWarInfo m_WarInfo;

	volatile LONG m_lMutex;
	IWarService* m_pWarService;
	SWarCreateContext m_context;  // 战场创建上下文

	WarCampMap m_mapWarCamp;  // 战场阵营信息

	map<PDBID, SPendingCreateActor> m_mapPendingCreateActor; // 准备进入战场的角色

	std::map<PDBID, WarPersonVector::iterator> m_mapPDBID2IterList; // 角色PDBID对应人物列表迭代器列表，用于提高查找人性能
	std::map<UID, WarPersonVector::iterator> m_mapUID2IterList;    // 角色UID对应人物列表迭代器列表，用于提高查找人性能

	map<int, SCampTeamRecordInfo> m_mapCampTeamRecord;	// 战后不同阵营的团队信息

	map<PDBID, sTabRecordInfo> m_mapTableRequest;    // table键请求记录

    // 记录最后小地图战斗信息时间
    map<UID,DWORD> m_mapUidMiniMapInfoRecord;

	map<PDBID, UID>  m_mapRealPlayerUid;  // 所有已经创建好的真实玩家uid

	map<PDBID, SActorEnterWarBaseInfo> m_OBPlayerBaseInfo;  // OB玩家

	// 怪物相关
protected:
	// 各阵营的所有玩家pdbid字符串
	string					m_CampPdbidStr[CAMP_MAX];

	__IWarMgr*				m_WarMgrList[Mgr_Max];
};