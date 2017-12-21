/*******************************************************************
** �ļ���:	ReplayMng.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/12/18
** ��  ��:	1.0
** ��  ��:	��ս������
** Ӧ  ��:  �����¼��ս�߼��������߼���Ϣ
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "ReplayNode.h"


enum EnumResultRequestReplay;

class ReplayMng : public Singleton<ReplayMng>
{
public:
	~ReplayMng();

	EnumResultRequestReplay	Find(ReplayID rID, ReplayNode*& pNode);

private:
	//************************************
	// Returns:   ViewID
	// Qualifier: ���Ҹ��������ViewServer��������ID������Ҳ������򷵻�0
	// Parameter: ViewID exceptID	Ҫ�ų���ViewServerID
	//************************************
	ViewID	GetIdleView(ViewID exceptID = 0);

private:
	typedef std::map<ViewID, ReplayNode*>		OneReplayMap;	// <ViewServerID, �ڸ�ViewServer�ۿ���ս���Ľڵ���Ϣ>
	typedef	std::map<ReplayID, OneReplayMap*>	ReplayMap;		// <ս��ID���ۿ��ⳡս�������нڵ�Map>
	typedef	std::map<ViewID, DWORD>				ViewMap;		// <ViewServerID, ��ViewServer������>

	ReplayMap	m_map;
	ViewMap		m_ViewMap;

	//MyRand		m_Rand;
};
extern ReplayMng& gReplayMng;