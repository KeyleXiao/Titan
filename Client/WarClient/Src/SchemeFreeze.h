/*******************************************************************
** 文件名:	e:\Rocket\Server\WarClient\Src\SchemeWar.h
** 版  权:	(C) 深圳冰川网络技术有限公司
** 创建人:	隆寒辉(Forlion Lon)（Forlion@126.com）
** 日  期:	2008/2/27  16:57
** 版  本:	1.0
** 描  述:	冷却配置脚本
** 应  用:  	
	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once

#include "ISchemeEngine.h"
#include <list>
#include <map>
#include <set>
#include <vector>
using namespace std;
using namespace stdext;

// 冷却配置脚本名字
#define FREEZE_SCHEME_FILENAME				"Scp\\War.xml"

// 冷却配置信息
struct SWarScheme
{	
	struct SWarGroup
	{
		int				nGroupID;
		int				nTime;
	};
	int					nWarID;		// 冷却ID
	int					nTime;			// 冷却时长	
	int					nSaveDB;		// 是否存数据库（0：不存，1：存）
	int					nShowTime;		// 冷却时长	客户端显示用

	list<SWarGroup>	listGroup;		// 子节点
};

// 冷却配置
class CSchemeWar : public ISchemeUpdateSink
{	
	// freezeid + SWarScheme
	typedef map<long, SWarScheme>		TMAP_FREEZESCHEME;

	// groupid + list<freezeid>
	typedef map<long, list<int> >			TMAP_FREEZEGROUP;

	struct SWarClass
	{
		int					nClassID;		

		TMAP_FREEZESCHEME	mapWarScheme;

		TMAP_FREEZEGROUP	mapWarGroup;
	};
	typedef vector<SWarClass>			TVECTOR_FREEZECLASS;

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
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	/** XML配置载入时通知
	@param   pTiXmlDocument：读取XML的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader,const char* szFileName);

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

	/** 
	@param   
	@param   
	@return  
	*/
	SWarScheme *			GetWarScheme(DWORD dwClassID, DWORD dwWarID);

	/** 
	@param   
	@param   
	@return  
	*/
	list<int> *				GetWarGroup(DWORD dwClassID, DWORD dwGroupID);

	/** 
	@param   
	@param   
	@return  
	*/
	CSchemeWar(void);

	/** 
	@param   
	@param   
	@return  
	*/
	virtual ~CSchemeWar(void);

private:
	// 所有冷却配置信息
	TVECTOR_FREEZECLASS		m_vectorWarClass;
};
