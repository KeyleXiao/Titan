/*******************************************************************
** �ļ���:	IDistrictListClient.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	������ (sww8@163.com)
** ��  ��:	2012/3/19
** ��  ��:	1.0
** ��  ��:	��Ϸѡ���ӿ�
** Ӧ  ��:  	
	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once

#ifndef __I_DistrictListClient_H__
#define __I_DistrictListClient_H__

struct SEventSystemDefaultServerChanged
{
	DWORD nID;
	DWORD nGroup;
	DWORD nDistrict;
	DWORD nServer;

	DWORD nID0;		// ����������ID
	DWORD nGroup0;		// �������
	DWORD nDistrict0;	// �汾�������
	DWORD nServer0;		// ��Ϸ���������

	SEventSystemDefaultServerChanged(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// ��Ϸ����������Ϣ
struct SClientGameGroupInfo
{
	DWORD nGroup;					// �������
	DWORD nServerCounts;			// ��Ϸ�������
	DWORD nRecommends;				// �Ƽ�ֵ��
	char szName[32];				// ��������

	SClientGameGroupInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// ������������Ϣ
struct ServerIndexInfo
{
	DWORD		nID;			// ����ID
	DWORD		nGroup;			// �������
	DWORD		nDistrict;		// �汾�������
	DWORD		nServer;		// ��Ϸ���������
	int			nNumber;		// ��������

	ServerIndexInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};

// һ����Ϸ����������Ϣ
struct SClientGameServerInfo
{
	DWORD nID;				// ����ID
	DWORD nGroup;			// �������
	DWORD nDistrict;		// �汾�������
	DWORD nServer;			// ��Ϸ���������

	DWORD nWorldID;			// ��Ϸ����ID
	DWORD nLatencty;		// �����ӳ�
	DWORD nState;			// ״̬ 0 ���� 1 ��æ 2 ���� 3 ά��
	DWORD nRecommend;		// �Ƽ�ֵ 1:�Ƽ� 0:��
	DWORD dwHid;

	char szName[32];			// ����������
	char szGroupName[32];		// ��������
	char szVerName[32];			// �汾��������
	char szVerShowName[32];		// �汾������ʾ����
	char szDesc[256];			// ����������
	char szAnnounce[256];		// ����������
	char szSameWorld[256];		// �ϳ�һ�������б�
	char szMainten[64];			// ά����Ϣ

	SClientGameServerInfo(void)
	{
		memset(this, 0, sizeof(*this));
	}
};


// ��Ϸ�����б�
struct IDistrictListClient
{
	virtual bool Initialize(void) = NULL;

	virtual void Close(void) = NULL;

	//  ������IDȡ����Ϸ��������Ϣ
	virtual const SClientGameServerInfo * GetGameServerInfoByID(DWORD nIndex) = NULL;

	//  ������IDȡ����Ϸ��������Ϣ
	virtual const SClientGameServerInfo *GetGameServerInfoByWorldID(DWORD dwWorldID) = NULL;

	//  ����+��+��������ȡ����Ϸ����������Ϣ
	virtual const SClientGameServerInfo * GetGameServerInfo(DWORD nGroup,DWORD nDistrict,DWORD nServer) = NULL;

	//  ����������Ϸ��ȡ����Ϸ����������Ϣ szAreaName:������,��:����һ��,szWorldName:��Ϸ����,bAutoMatch:�羫ȷ�Ҳ�����ֻ����Ϸ������,�����û������ҵ�ƥ����Ϸ��
	virtual const SClientGameServerInfo * GetGameServerInfoByName(const char * szAreaName,const char * szWorldName,bool bAutoMatch=true) = NULL;

	//  ȡ���Ƽ�����Ϸ����������Ϣ
	virtual const SClientGameServerInfo * GetRecommendGameServerInfo() = NULL;

	//  ȡ�õ�ǰ����Ϸ����������Ϣ
	virtual const SClientGameServerInfo * GetSelectGameServerInfo() = NULL;

	// ȡ����Ϸ��������
	virtual const char* GetWorldNameByID(DWORD dwWorldID) = NULL;

	//  �趨��ǰ��Ϸ����������Ϣ bSendEvent:�Ƿ��¼�
	virtual bool SetSelectGameServerInfo(DWORD nGroup,DWORD nDistrict,DWORD nServer,bool bSendEvent=true) = NULL;

	// ������Ϸ���¼�����
	virtual void OnEnterServer(DWORD nGroup,DWORD nDistrict,DWORD nServer) = NULL;

	// ��Ϸ�����Ƿ������Ĺؼ���ƥ��,������������ĸƥ��
	virtual bool IsKeyWordMatchServer(DWORD nGroup,DWORD nDistrict,DWORD nServer,const char * szKeyWord) = NULL;

	// ��������ͨ���ǵ��� 1.����,2.��ͨ,3.�ƶ�
	virtual DWORD GetMyISP() = NULL;

	// �趨��������ͨ���ǵ��� 1.����,2.��ͨ,3.�ƶ�
	virtual void SetMyISP(DWORD dwISP) = NULL;
};
#endif