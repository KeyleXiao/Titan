/*******************************************************************
** 文件名:	E:\speed\Social\KinServer\Src\KinService_Proxy.h
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	秦其勇
** 日  期:	10/16/2015	10:50
** 版  本:	1.0
** 描  述:	战队代理
********************************************************************/

#pragma once
#include "IServiceContainer.h"
#include "IKinService.h"


using namespace rkt;
using namespace std;

class KinService_Proxy:public IKinService
{
public:
	///////////////////////////////IKinService///////////////////////////////////////////

    // 处理其它服务器发送过来的消息
    virtual void handleServerMsg(DWORD serverID, SNetMsgHead head, void * data, size_t len);

    // 处理客户端发送过来的消息
    virtual void handleClientMsg(DWORD client, SNetMsgHead head, void * data, size_t len);


     /** 根据战队ID获取Kin接口
	@param dwKinID		:战队ID
	*/
    virtual IKin*   getKin(DWORD dwKinID);

        
    /** 根据战队ID获取Kin数据
	@param dwKinID		:战队ID
	*/
	virtual SKinInfo getKinInfo(DWORD dwKinID);

    /** 获取战队成员数据接口
	@param dwKinID		:战队ID
	*/ 
    virtual SKinMember getKinMemberInfo(DWORD dwKinID);

     /** 根据战队ID获取成员列表接口
	@param          dwKinID		:战队ID
    @param return 战队成员数	:成员数量
	*/
	virtual DWORD   getKinMemberList(DWORD dwKinID, PDBID* pReturnArray, DWORD dwArrayMaxSize);

    /** 外部接口 删除战队成员
	@param          dwKinID		:战队ID
    @param return 战队成员数	:成员数量
	*/
    virtual bool deleteKinMember(DWORD dwKinID, DWORD ActorID);

     /** 获取战队成员数量
	@param dwKinID		:战队ID 
	*/
    virtual DWORD getKinMemberCount(DWORD dwKinID);

    /** 获取战队成员数量
	@param dwKinID		:战队ID 
	*/
    virtual DWORD getTotalFightScore(DWORD dwKinID);

    /// purpose: 设置战队贡献度
    virtual bool  addClanCtrb(DWORD dwPDBID, int nClanCtrb);

	/// purpose: 新增战队杯赛奖励
	virtual void addKinLegendAward(SKinCupAwardNode sAwardNode);

	/// purpose: 新增战队杯赛战绩
	virtual void addKinLegendWarResult(SKinWarRecordNode sRecordNode);

	// purpose:  战队杯赛荣誉增加
	virtual void addKinLegendGlory(int nKinID, int nNum, int nGloryType);

	// purpose: 取战队杯赛荣誉
	virtual int getKinLegendGlory(int nKinID, int nGloryType);

	/// purpose: 获取周活跃度
	virtual int getWeekActivity(DWORD dwKinID);

public:
	SERVICE_PTR m_pContainer;

private:
	void PostMsg(obuf256& oMsg);
};