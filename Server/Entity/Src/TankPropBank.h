/*******************************************************************
** 文件名:	e:\Rocket\Server\EntityServer\Src\TankPropBank.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Long)（Forlion@126.com）
** 日  期:	2009/4/21 14:53
** 版  本:	1.0
** 描  述:	载具属性银行
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "DEntityProp.h"
#include "IZoneManager.h"
#include "ISchemeCenter.h"
#include "BoxPopedom.h"
#include "DEntityBuild.h"

class CTank;
class CTankPropBank
{
public:
	/** 设置UID
	@param   
	@param   
	@return  
	*/
	void					SetUID(LONGLONG uid);

	/** 取得UID
	@param   
	@param   
	@return  
	*/
	LONGLONG				GetUID(void);

	/** 取得场景ID
	@param   
	@param   
	@return  
	*/
	bool					SetZoneID(DWORD dwZoneID);

	/** 取得场景ID
	@param   
	@param   
	@return  
	*/
	DWORD					GetZoneID(void);

	/** 取得地图ID
	@param   
	@param   
	@return  
	*/
	DWORD					GetMapID(void);

	/** 设置地图位置
	@param   
	@param   
	@return  
	*/
	void					SetMapLoc(POINT ptMapTile);

	/** 取得所在的地图位置
	@param   
	@param   
	@return  
	*/
	POINT					GetMapLoc(void);

	/** 取得数值属性
	@param   
	@param   
	@return  
	*/
	int						GetNumProp(DWORD dwPropID);

	/** 设置数值属性
	@param   nValue    ：每个属性有不同的更新方式（覆盖，禁止修改，增量）
	@param   bSyncFlag ：是否要同步
	@return  
	*/
	bool					SetNumProp(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pInfo);

	/** 导入默认创建现场
	@param   
	@param   
	@return  
	*/
	bool					ImportDefaultCreateContext(LPCSTR pszContext, int nLen);

	/** 导入地图创建现场
	@param   
	@param   
	@return  
	*/
	bool					ImportMapCreateContext(LPCSTR pszContext, int nLen);

	/** 导出创建别人的客户端现场
	@param   
	@param   
	@return  
	*/
	bool					ExportShadowContext(LPSTR pszContext, int &nLen);

	/** 导出创建自已的客户端现场
	@param   
	@param   
	@return  
	*/
	bool					ExportHeroContext(LPSTR pszContext, int &nLen);

	/** 导出数据库属性现场
	@param   
	@param   
	@return  
	*/
	bool					ExportMapContext(LPSTR pszContext, int &nLen);

	/** 取得怪物名字
	@param   
	@param   
	@return  
	*/
	LPCSTR					GetTitle(void);

	/** 设置怪物名字
	@param   
	@param   
	@return  
	*/
	void					SetTitle(LPCSTR pszTitle);

	/** 固化移动速度
	@param   nMoveSpeed ：0（表示取消固化移动速度），其他表示将当前移动速度固化在第几档
	@param   
	@return  
	*/
	void					SolidifyMoveSpeed(int nMoveSpeed);

	/** 还原,不释放内存，只将状态还原到创建时状态
	@param   未来继续使用
	@param   
	@return  
	*/
	void					Restore(void);

	/** 创建
	@param   
	@param   
	@return  
	*/
	bool					Create(CTank * pTank);

	/** 取载具等级
	@param   
	@param   
	@return  
	*/
	long					GetTankLevel(void);

	/** 获得所在场景对像
	@param   
	@param   
	@return  
	*/
	IGameZone *				GetGameZone(void);

	/** 取得扩展数据
	@param   
	@param   
	@return  
	*/
	LPCSTR					GetExtendData(void);

	/** 设置扩展数据
	@param szExtendData 扩展数据
	@param nLen			扩展数据长度  
	@return  
	*/
	bool					SetExtendData(LPCSTR szExtendData, int nLen);

	/** 
	@param   
	@param   
	@return  
	*/
	CTankPropBank(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CTankPropBank(void);

private:
	/** 禁止修改 非负数
	@param   
	@param   
	@return  
	*/
	void					Forbid_NoNegative(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 禁止修改 非负数 大小比
	@param   
	@param   
	@return  
	*/
	void					Forbid_NoNegative_Compare(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 增量修改 非负数
	@param   
	@param   
	@return  
	*/
	void					Increment_NoNegative(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 增量修改 非负数 大小比
	@param   
	@param   
	@return  
	*/
	void					Increment_NoNegative_Compare(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 覆盖修改 非负数
	@param   
	@param   
	@return  
	*/
	void					Cover_NoNegative(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 当前血量
	@param   
	@param   
	@return  
	*/
	void					SetProp_CurHP(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_Move_Speed(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);	

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_Max_HP(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_Max_HP_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_Max_MP(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_Max_MP_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);	

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_P_A(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_P_A_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_M_A(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_M_A_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_P_D(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_P_D_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_F_D(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_I_D(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_T_D(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_D_D(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_M_D(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_M_D_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_PM_Fatal_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_Persent_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_Hedge_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_Resist_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 
	@param   
	@param   
	@return  
	*/
	void					SetPlus_Penetrate_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	///** 
	//@param   
	//@param   
	//@return  
	//*/
	//void					SetPlus_P_Absorb_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	///** 
	//@param   
	//@param   
	//@return  
	//*/
	//void					SetPlus_M_Absorb_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	///** 
	//@param   
	//@param   
	//@return  
	//*/
	//void					SetPlus_PM_Absorb(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	///** 
	//@param   
	//@param   
	//@return  
	//*/
	//void					SetPlus_PM_Absorb_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	///** 
	//@param   
	//@param   
	//@return  
	//*/
	//void					SetPlus_Rebound_Damage(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	///** 
	//@param   
	//@param   
	//@return  
	//*/
	//void					SetPlus_Kick_Fatal_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 物理、法术伤害吸收
	@param   
	@param   
	@return  
	*/
	void					SetPlus_Absorb_Counter(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 附加反致命概率
	@param   
	@param   
	@return  
	*/
	void					SetPlus_Kick_Fatal_Per(DWORD dwPropID, int nValue, bool bSyncFlag, STankSchemeInfo * pSchemeInfo);

	/** 同步客户端属性
	@param   
	@param   
	@return  
	*/
	void					UpdateClientProp(DWORD dwPropID, bool bSyncFlag);

	/** 同步客户端名字
	@param   
	@param   
	@return  
	*/
	void					UpdateClientTitle(void);

	/** 检查是否需延时刷新血量
	@param   
	@param   
	@return  
	*/
	void					CheckWaitSyncPropBroadcast(void);

private:
	// UID
	LONGLONG				m_uid;

	// 场景
	IGameZone *				m_pGameZone;

	// 位置
	POINT					m_ptMapTile;	

	// 数值属性
	int						m_nNumProp[TANK_PROP_MAXID];

	// 载具称号
	char					m_szTitle[32];

	// 载具实体
	CTank *					m_pMaster;

	// 是否固化移动速度
	bool					m_bSolidifyMoveSpeed;

	// 是否启动了延时刷新血量
	DWORD					m_dwWaitSyncHPStartTick;

	// 载具等级
	long					m_lLevel;

	// 扩展数据
	char					m_szExtendData[TRANSMAP_MAX_EXTEND_LEN];
};