/*******************************************************************
** 文件名:	SchemeMatchRoom.h 
** 版  权:	(C) 深圳冰川网络技术有限公司 2008 - All Rights Reserved
** 创建人:	卜涛
** 日  期:	3/30/2015	15:46
** 版  本:	1.0
** 描  述:	比赛大厅房间配置
            
********************************************************************/

#pragma once

#include <map>
#include <vector>
#include "ISchemeEngine.h"
#include "ISchemeCenter.h"
#include "SchemeCache.h"
#include "MatchSchemeDef.h"

class CSchemeMatchRoom : public ISchemeMatchRoom, public ISchemeUpdateSink, public CSchemeCache<CSchemeMatchRoom>
{
public:
	typedef map<int,SSchemeSelectControl >		MapSelectControlSchemes;
	typedef vector<SSchemeMatchRoom* >			MatchRoomSchemes;
	typedef map<BYTE, MatchRoomSchemes>			GameMode2Schemes;
	typedef map<DWORD, MatchRoomSchemes>		Map2Schemes;
	typedef	vector<uchar>						GameModes;
    typedef map<int,SSchemeComputerSumSkill >	MapComputerSumSkillSchemes;
	static MatchRoomSchemes						ms_NullSchemeInfo;

public:
	//////////////////////ISchemeMatchRoom////////////////////////////	
	virtual vector<SSchemeMatchRoom* >&		getByMode(uchar ModeID);
	virtual vector<SSchemeMatchRoom* >&		getByMap(DWORD MapID);
	virtual vector<uchar>&					getAllMode(){return m_ModeIDs;};
	virtual SSchemeSelectControl*			getSelectControlScheme(int nID);
    virtual SSchemeMatchRoom*			    getSchemeMatchRoomByMatchTypeID(DWORD dwMatchTypeID);
    virtual SSchemeComputerSumSkill*		getSchemeComputerSumSkill(int nHeroID);
	//////////ISchemeUpdateSink////////////////////////////////////////	
	/** CSV配置载入时通知
	@param   type   : 读取器类型
		            : 如果是READER_CSV : 就可以把pReader转换成ICSVReader*
					: READER_XML : 就可以把pReader转换成TiXmlDocument*
	@param   pReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/ 
	virtual bool			OnSchemeLoad(SCRIPT_READER reader,const char* szFileName);

	
	/** 配置动态更新时通知
	@param   pCSVReader：读取CSV的返回接口
	@param   szFileName：配置文件名
	@param   
	@return  
	@note     
	@warning 不要在此方法释放pCSVReader或者pTiXmlDocument,因为一个文件对应多个sink
	@retval buffer 
	*/
	virtual bool			OnSchemeUpdate(SCRIPT_READER reader, const char* szFileName);

	bool LoadScheme(void);

	void Close(void);

private:
	bool	getModes(ICSVReader* pCSVReader, int nRow, int nIndex, bool bStates[MatchRoomStates]);
	bool	LoadMatchRoomScheme(ICSVReader* pCSVReader);
	bool	LoadSelectControlScheme(ICSVReader* pCSVReader);
    bool	LoadComputerSumSkillScheme(ICSVReader* pCSVReader);
    bool	LoadPunishScheme(ICSVReader* pCSVReader);
private:
	MapSelectControlSchemes	m_SchemeSelectControl;
	MatchRoomSchemes		m_SchemeInfo;
	GameMode2Schemes		m_Mode2Datas;
	Map2Schemes				m_Map2Datas;
	GameModes				m_ModeIDs;
    MapComputerSumSkillSchemes	m_SchemeComputerSumSkill;

};