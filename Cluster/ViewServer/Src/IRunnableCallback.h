/*******************************************************************
** �ļ���:	IRunnableCallback.h
** ��  Ȩ:	(C) ���ڱ�������ɷ����޹�˾
** ������:	baoliang.shen
** ��  ��:	2017/12/23
** ��  ��:	1.0
** ��  ��:	�̳д���Ķ���Ĵ��������٣���Ӧ�����߼��߳���
** Ӧ  ��:  ����Redis�̵߳Ļ���
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/
#pragma once
#include "ICallback.h"


struct IRunnableCallback : public ICallback, public IRunnable
{
	IRunnableCallback() : isDone(false)
	{}

	bool	IsDone() { return isDone; }
	void	SetDone() { isDone = true; }

	virtual	bool NeedRecovery() = 0;
	// �Ƿ���Ҫ�ٴ�Ͷ�ݵ�Redis�߳�
	virtual bool NeedSpawnAgain() { return false; }

	void	OnSpawn()
	{
		UnDone();

		_OnSpawn();
	}

protected:
	virtual	void	_OnSpawn() = 0;

private:
	void	UnDone() { isDone = false; }

protected:
	bool isDone;
};
