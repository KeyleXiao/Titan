/*******************************************************************
** �ļ���:	ViewRedisData.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/12/26
** ��  ��:	1.0
** ��  ��:	��սRedis�е����ݽṹ����
** Ӧ  ��:  	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "ISerializableData.h"


#pragma pack(1)
namespace ViewRedisData
{
	/*******************************************************************/
	// Ԥ����Ϣ���� <[ս��ID]:Prep, List>
	// List�еľ���Ԫ������:
	static const long indexBattlePrepInfo = 0;
	static const long indexBeginTime = 1;
	static const long indexEndTime = 2;
	/*******************************************************************/
	// 0.��ս��֮ǰ��Ԥ����Ϣ���ͻ�����Ҫ���������Ϣ�����س�����Ӣ�۵ȣ�
	struct BattlePrepInfo
	{

	};
	// 1.STime ��ʼʱ�� ��ʶʱ��Ľṹ�壨��ȷ�����룩
	// 2.STime ����ʱ�� ��ʶʱ��Ľṹ�壨��ȷ�����룩
	/*******************************************************************/



	/*******************************************************************/
	// ����ս����Ϣ����
	/*******************************************************************/
	// ս��
	struct BattleInfo : ISerializableData
	{
		BYTE	byType;		// Battle��Ϣ��(ViewServer����������ֶΣ��������Ϳͻ��˹���)
		DWORD	tTime;		// ʱ���(�ӱ�ս����һ�������Ϣ��ʼ�������ĺ�����)
		DWORD	dwLength;	// ��Ϣ�峤��
		char*	psz;		// ��Ϣ��

		BattleInfo()
		{
			memset(this, 0, sizeof(BattleInfo));
		}

		// ��������
		~BattleInfo()
		{
			safeDelete(psz);
			dwLength = 0;
		}

		// ��������
		BattleInfo(const BattleInfo& rhs)
			: byType(rhs.byType)
			, tTime(rhs.tTime)
			, dwLength(rhs.dwLength)
		{
			psz = new char[rhs.dwLength];
			memcpy(psz, rhs.psz, dwLength);
		}

		//�ƶ����캯��
		BattleInfo(BattleInfo&& rhs) : psz(rhs.psz)
			, byType(rhs.byType)
			, tTime(rhs.tTime)
			, dwLength(rhs.dwLength)
		{
			rhs.psz = nullptr;//����ʱֵ��ָ���Ա�ÿ�
			rhs.dwLength = 0;
		}

		virtual void fromBytes(const char* p, size_t len) override
		{
			ibuffer ibufData(p, len);
			ibufData >> byType >> tTime >> dwLength;
			if (dwLength > 0)
				ibufData.pop(psz, dwLength);
		}


		virtual void toBytes(obuf& obufData) const override
		{
			obufData << byType << tTime << dwLength;
			if (dwLength > 0)
				obufData.push_back(psz, dwLength);
		}
	};
	/*******************************************************************/
};
#pragma pack()