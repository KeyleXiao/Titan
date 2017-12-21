/*******************************************************************
** 文件名:	e:\Rocket\Server\KinServer\Src\Kin.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	秦其勇
** 日  期:	2016/3/9
** 版  本:	1.0
** 描  述:	战队对象类
** 应  用:	
**************************************1******************************/

#ifndef __Kin_h__
#define __Kin_h__

#include "IKinScene.h"
#include "Kin_ManagedDef.h"
using namespace Kin_ManagerDef;

class KinScenService;

// 战队对象实现类
class CKin : public IKin
{
public:
	/// purpose: 构造函数
	CKin(void);

	/// purpose: 析构函数
	virtual ~CKin(void);

public:
	/// purpose: 创建
	bool create(KinScenService *pKinScenService, LPCSTR pszData, size_t nLen);

	/// purpose: 关闭
	virtual void release(void);

	/// purpose: 取得战队的整型数据
	virtual int getNumProp(DWORD dwPropID);

	/// purpose: 设置战队的整型数据（覆盖型）
	virtual bool coverNumProp(DWORD dwPropID, int nValue);

	/// purpose: 取得战队名字
	virtual LPCSTR getName();

	/// purpose: 更新数据
	bool updateData(LPCSTR pszData, size_t nLen);

	/// purpose: 添加一个成员
	void addMember(DWORD dwMemberID);

	/// purpose: 删除一个成员
	void removeMember(DWORD dwMemberID);

	/// purpose: 取得成员个数
	int getMemberCount();

    DWORD getMemberList(PDBID* pReturnArray, DWORD dwArrayMaxSize);

private:
    KinScenService *m_pKinScenService;

	DWORD m_dwReadFlag;								// 已经从数据库读出数据的标志
	DWORD m_dwID;									// 战队ID
	char m_szName[KIN_NAME_SIZE];					// 战队名
	int m_nState;									// 战队的状态：临时，正式，解散中
	int m_nLevel;									// 战队等级
	
	int m_nFightCount;								// 正在对战的战队个数
	int m_nActivity;								// 活跃度
	int m_nWeekActivity;							// 周活跃度
	int m_nPlace;									// 排名
	int m_nShaikhID;								// 战队族长PDBID
	int m_nMoney;							        // 资金
	int m_nChangeLand;								// 战队移民状态

	int m_nWorldID;									// 游戏世界ID
	char m_szShaikhzName[KIN_NAME_SIZE];			// 战队族长名

	char m_szLabel[KIN_LABEL_SIZE];					// 战队标签

	list<DWORD> m_MemberList;						// 在线成员列表
	list<DWORD>::iterator m_MembertIt;				// 战队成员迭代器



	// 整型数据的地址数组
	int* m_propAddress[emKinProp_NumPropCount];
};

#endif // __Kin_h__