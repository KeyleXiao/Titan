/*******************************************************************
** 文件名:	Gates.h
** 版  权:	(C) 深圳冰川网络股份有限公司
** 创建人:	baoliang.shen
** 日  期:	2017/12/05
** 版  本:	1.0
** 描  述:	管理所有通往Gate的连接
** 应  用:  	
**************************** 修改记录 ******************************
** 修改人: 
** 日  期: 
** 描  述: 
********************************************************************/
#pragma once
#include "ViewMsgDef_Server.h"
#include "ByteRecord.h"
#include "Processer.h"

class GateConnector;

class Gates : public Singleton<Gates>
{
public:
	Gates();
	~Gates();

	// 对比本地的所有Gate信息，没连的全都连上
	void Update(VecGateInfo& vecGates);
	// 查找指定ID的Gate连接
	GateConnector*	Get(WORD wID);

private:
	// 建立通往Gate的连接
	void NewConnect(SGateInfo*& pInfo);

	// 查找指定IP和端口的Gate连接
	GateConnector*	Get(const std::string& ip, const WORD& port);

private:
	//<ID, Gate连接>
	typedef	std::map<WORD, GateConnector*>	GateMap;

	GateMap						m_map;
	ByteRecord					m_br;
	Processer<GateConnector>	m_processer;
};
extern Gates& gGates;
