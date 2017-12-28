/*******************************************************************
** �ļ���:	Gates.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/12/05
** ��  ��:	1.0
** ��  ��:	��������ͨ��Gate������
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
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

	// �Աȱ��ص�����Gate��Ϣ��û����ȫ������
	void Update(VecGateInfo& vecGates);
	// ����ָ��ID��Gate����
	GateConnector*	Get(WORD wID);

private:
	// ����ͨ��Gate������
	void NewConnect(SGateInfo*& pInfo);

	// ����ָ��IP�Ͷ˿ڵ�Gate����
	GateConnector*	Get(const std::string& ip, const WORD& port);

private:
	//<ID, Gate����>
	typedef	std::map<WORD, GateConnector*>	GateMap;

	GateMap						m_map;
	ByteRecord					m_br;
	Processer<GateConnector>	m_processer;
};
extern Gates& gGates;
