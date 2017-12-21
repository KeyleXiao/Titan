/*******************************************************************
** 文件名:	e:\Rocket\Server\EffectServer\Src\SchemeEffect.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/19  15:08
** 版  本:	1.0
** 描  述:	效棵配置
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include <list>
#include <map>
using namespace std;
using namespace stdext;

// Effect配置脚本名字
#define EFFECT_SCHEME_FILENAME			"Scp\\Effect.xml"


// Effect的标志集
enum
{
	// 自动将此类型的BUFF加到玩家所乘的双人载具上
	EEffect_Flag_AutoChangeToTank = 0x0001,
};

// Effect配置信息
struct SEffectSchemeInfo
{	
	// EFFECT ID
	int					nEffectID;

	// GROUP ID
	int					nGroupID;

	// 效果描述
	char				szDesc[128];

	// 效果参数
	list<int>			listParam;

	// VALUE ID
	int					nValueID;

	// 标志
	int					nFlag;


	SEffectSchemeInfo(void)
	{

	};
};

// Effect配置
class CSchemeEffect : public ISchemeUpdateSink
{
	typedef map<DWORD, SEffectSchemeInfo>	TMAP_EFFECTSCHEMEINFO;
	typedef	stdext::hash_map<int, std::string> GroupNameMap;

public:
	///////////////////////////////ISchemeUpdateSink///////////////////	
	/** CSV配置载入时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/ 
	virtual bool			OnSchemeLoad(ICSVReader * pCSVReader,const char* szFileName);

	/** XML配置载入时通知
	@param   pTiXmlDocument：读取XML的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			OnSchemeLoad(TiXmlDocument * pTiXmlDocument,const char* szFileName);

	/** 配置动态更新时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			OnSchemeUpdate(ICSVReader * pCSVReader, const char* szFileName);

	/** 配置动态更新时通知
	@param   pTiXmlDocument：读取XML的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			OnSchemeUpdate(TiXmlDocument * pTiXmlDocument, const char* szFileName);

	///////////////////////////////CSchemeBuff//////////////////////////
	/** 载入脚本
	@param   
	@param   
	@return  
	*/
	bool					LoadScheme(void);

	/** 关闭
	@param   
	@param   
	@return  
	*/
	void					Close(void);

	/** 取得效果配置信息
	@param   
	@param   
	@return  
	*/
	SEffectSchemeInfo *		GetEffectSchemeInfo(DWORD dwEffectID);

	/** 取得效果组ID对应的效果组名
	@param nGroupID	：效果组ID
	@return  成功：效果组名，失败：""
	*/
	const char *			GetEffectGroupName(int nGroupID);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeEffect(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeEffect(void);

private:
	// 所有效果配置属性	
	TMAP_EFFECTSCHEMEINFO	m_mapEffectSchemeInfo;

	// 检测效果组映射
	GroupNameMap	m_mapCheckGroupMap;
};
