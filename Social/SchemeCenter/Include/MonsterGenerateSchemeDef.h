#pragma once
#include "Vector3.h"

#pragma pack(push, 1)

struct GenMonsterStrategy
{
	enum
	{
		REBORN_MODE_FIX_INTERVAL,  // �̶������ˢ
		REBORN_MODE_AFTER_DIE_INTERVAL, // ������nRebornInterval��ˢ
	};

	struct MonsterGenInfo
	{
		int nMonsterID;  // ����ID
		int nGroup;		 // ��Ӫ
		int nAIMode;	 // AIģʽ
		int nRadii;		 // ��Ұ
		int nBornTime;	 // ˢ����������nBornTime��ˢ����һ��
		Vector3 ptBorn;	 // ������
		float fAngle;    // ����
		int nNumPerBatch; // һ����������
		int nRebornMode;  // 0--�̶����nRebornInterval��ˢһ�� 1--������nRebornInterval��ˢһ��
		int nRebornInterval; // ��ˢ���
		int nNumUperLimit;	// ��������
		std::vector<Vector3> vecWayPoint;	// Ѳ��·��

		MonsterGenInfo() : nMonsterID(0), nGroup(0), nAIMode(0), nRadii(0), nBornTime(0), 
			nNumPerBatch(0), nRebornMode(0), nRebornInterval(0), nNumUperLimit(0)
		{
			vecWayPoint.clear();
		}
	};

	std::vector<MonsterGenInfo> vecInfo;

	GenMonsterStrategy()
	{
		vecInfo.clear();
	}
};

#pragma pack(pop)