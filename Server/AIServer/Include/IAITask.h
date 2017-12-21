/*******************************************************************
** �ļ���:	IAITask.h
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾ 2008 - All Rights Reserved
** ������:	ljw
** ��  ��:	
** ��  ��:	1.0
** ��  ��:  AI����ӿ�
** Ӧ  ��:  	
********************************************************************/

#pragma once
#include "Vector3.h"

// AI����״̬
enum AITaskState
{
	ATS_NONE,         // ��
	ATS_RUNNING,	  // ����ִ��
	ATS_SUSPEND,      // ��ͣ
	ATS_FINISH,		  // ���
};

// AI��������
enum AITaskType
{
	ATT_SIM,					// ģ��������� ����
	ATT_FORWARD_LINE,			// ����
	ATT_HELP_OTHER_LINE,		// ֧Ԯ����������
	ATT_FOLLOW,					// ����
	ATT_BACK,					// �س�
	ATT_HUNTING,				// ��Ұ
	ATT_MAX,
};

#define TASK_PRIORITY_HIGHEST 100

// AI����ӿ�
struct IAITask
{
	// ִ��
	virtual bool run(AITaskState& oriState) = 0;

	// ��ͣ
	virtual bool suspend(AITaskState& oriState) = 0;

	// ���
	virtual void finish() = 0;

	// ��ǰ״̬
	virtual AITaskState state() = 0;

	// ��ȡ����ص�
	virtual Vector3 getLoc() = 0;

	// ��ȡ��������
	virtual AITaskType getType() = 0;

	// ��ȡ���ȼ�
	virtual int getPriority() = 0;

	// ���¹�������
	virtual void updateAttackMask(int nAttackMask) = 0;

	// �������ȼ�
	virtual void updatePriority(int nPriority) = 0;

	// ���³�ʱʱ��
	virtual void updateTimeOut(int nTimeOut) = 0;

    // ����context
    virtual void updateContext(void* context) = 0;

    virtual std::string& log(const char* szLog) = 0;

	// �ͷ�
	virtual void release() = 0;
};