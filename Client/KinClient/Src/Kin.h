/*******************************************************************
** 文件名:	e:\Rocket\Client\Kin\Src\Kin.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	xiaobao
** 日  期:	2009/3/9
** 版  本:	1.0
** 描  述:	战队对象类
** 应  用:  	
********************************************************************/
#ifndef __Kin_h__
#define __Kin_h__

#include "IKinClient.h"
#include "IEntityClient.h"
#include "Kin_ManagedDef.h"

using namespace Kin_ManagerDef;

// 战队对象类
class CKin : public IKin, public IGameViewEventHandler
{
public:
    

	/// purpose: 构造函数
	CKin();

	/// purpose: 虚构函数
	virtual ~CKin();
	
    void release();

	/// purpose: 创建
	bool create(void);

	/// purpose: 释放
	virtual void close(void);

	/// purpose: 清空所有数据
	void reset(void);


    ////////////////////////////IGameViewEventHandler////////////////////////////////////////////////////
    virtual bool onViewEvent( int32 eventID,int nParam,const char * strParam,void * ptrParam );

	/// purpose: 是否有效
	virtual bool IsVaild(void);

	/// purpose: 取得战队的整型数据
	virtual int getNumProp(DWORD dwPropID);

	/// purpose: 设置战队的整型数据
	virtual bool setNumProp(DWORD dwPropID, int nValue);

	/// purpose: 取得战队的字符型数据
	virtual string getStringProp(DWORD dwPropID);

	/// purpose: 设置战队的字符型数据
	virtual bool setStringProp(DWORD dwPropID, string strValue);

	/// purpose: 取得成员的个数
	virtual int getMemberCount();

	/// purpose: 取得成员的数据
	//@ param  : 下标，第几个成员
	virtual SClientKinMember* getMemberByIndex(int nIndex);

	/// purpose: 取得成员的数据
	//@ param  : 玩家ID
	virtual SClientKinMember* getMemberByID(DWORD dwPDBID);

	/// purpose: 删除成员
	virtual void removeMember(DWORD dwPDBID);

	/// purpose: 取得申请成员的个数
	virtual int getApplyMemberCount();

	/// purpose: 取得成员的数据
	//@ param  : 下标，第几个成员
	virtual SKinClientApplyMember* getApplyMemberByIndex(int nIndex);

	/// purpose: 删除删除成员
	virtual void removeApplyMember(DWORD dwPDBID);

	/// purpose: 排序
	//@ param  : nSortType 参照TKinSortType
	virtual void sortMember(int nSortType);

	/// purpose: 取得排名
	virtual int getPlace();

    // purpose: 处理客户端事件
    // note   : 在事件发送出去前，本地先对事件做处理
    bool dealClientEvent(LPCSTR pszMsg, int nLen);

	/// purpose: 取得自动接收信息
	virtual SAutoAccept* getAutoAccept();
    
    /// purpose: 响应消息
    virtual void onMessage( SNetMsgHead* head, void* data, size_t len);
private:
	// purpose: 更新战队属性
	void onUpdateKinProp(LPCSTR pszMsg, int nLen);

	// purpose: 更新战队基本数据
	void onUpdateKinBaseData(LPCSTR pszMsg, int nLen);

    // purpose: 更新战队基本数据
    void onRecvKinBaseData(LPCSTR pszMsg, int nLen);

	// purpose: 更新战队成员数据
	void onUpdateKinMemData(LPCSTR pszMsg, int nLen);

    // purpose: 更新战队事件列表数据
    void onUpdateKinEventList(LPCSTR pszMsg, int nLen);

	// purpose: 更新战队荣誉数据
	void onUpdateKinGloryData(LPCSTR pszMsg, int nLen);

	// purpose: 更新战队总历史战绩
	void onUpdateKinWarRecordTotal(LPCSTR pszMsg, int nLen);

	// purpose: 发起战队战
	void onAddKinWar(LPCSTR pszMsg, int nLen);

	// purpose: 战队战结束
	void onKinWarEnd(LPCSTR pszMsg, int nLen);

	// purpose: 清除所有的战队战
	void onKinWarClean(LPCSTR pszMsg, int nLen);

    // purpose: 更新客户端登陆之后的战队主要信息
    void onKinMemberLogin(LPCSTR pszMsg, int nLen);

	// purpose: 更新客户端资源信息
	void onKinUpdateClient(LPCSTR pszMsg, int nLen);

	// purpose: 更新客户端Boss
	void onKinUpdateBoss(LPCSTR pszMsg, int nLen);

	// purpose: 更新留言数据
	void onKinUpdateWord(LPCSTR pszMsg, int nLen);
    
    // purpose: 解散战队
    void onKinDisMiss(LPCSTR pszMsg, int nLen);

    /// purpose: 请求申请人员列表
    void onViewApplyListRequest();
    
    void onViewAutoAcceptRequest();

private:
	// 战队数据
	char m_szManifesto[KIN_MANIFESTO_LEN];			// 战队的宣言
	char m_szNotice[KIN_NOTICE_LEN];				// 战队的每日公告
	char m_szName[KIN_NAME_SIZE];					// 战队名
	char m_szShaikhName[ACTORNAME_MAXSIZE];			// 族长名字
	char m_szLabel[KIN_LABEL_SIZE];					// 战队标签

	bool m_KinVaild;								// 战队对象是否有效	
	DWORD m_dwID;									// 战队ID
	DWORD m_dwCountryID;							// 国家ID
	int m_nState;									// 战队的状态：临时，正式，解散中
	int	m_nEstablishTime;							// 战队创立时间
	
	int m_nShaikhID;								// 战队族长PDBID
	int m_nLevel;									// 战队等级
	DWORD m_nServiceOpenFlag;						// 各种功能是否开放，比如是否给成员学某种技能，是否给成员领工资，是否给成员领取某种材料，是否开放聊天频道
	int m_nActivity;								// 活跃度
	int m_nWeekActivity;                            // 周活跃度
	int m_nMoney;							        // 资金
	
	int m_nSysTime;									// 系统时间
	int m_nTickCount;								// 更新系统时的TickCount
	
	char m_szSkillData[KINSKILL_RSDATA_LEN];		// 技能数据

	int	m_nPlace;									// 战队排行

	bool m_bFirstOpen;								// 第一次登陆时 打开对话框

	char  m_szSlogan[KIN_SLOGAN_LEN];				// 宣传口号
	SAutoAccept m_AutoSetting;						// 自动接收数据

	int	nDismissApplyTime;							// 解散解散时间


	// 成员数据
	vector<SClientKinMember>	m_MemberList;
	
	// 申请中的成员列表
	vector<SKinClientApplyMember>	m_ApplyMemberList;

	// 整型数据的地址数组
	int* m_propAddress[emKinProp_NumPropCount];
};

extern CKin* g_pKin;

#endif // __Kin_h__