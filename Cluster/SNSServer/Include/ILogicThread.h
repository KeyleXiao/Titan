/*******************************************************************
** �ļ���:	ILogicThread.h 
** ��  Ȩ:	(C) ���ڱ������缼�����޹�˾
** ������:	��绪
** ��  ��:	11/28/2016
** ��  ��:	1.0
** ��  ��:	�߼��߳̽ӿ�
** Ӧ  ��:  	
	
**************************** �޸ļ�¼ ******************************
** �޸���: 
** ��  ��: 
** ��  ��: 
********************************************************************/

#ifndef __I_LOGICTHREAD_H__
#define __I_LOGICTHREAD_H__



/// �߼��߳�����ӿ�
struct ILogicThreadTask
{
	/** �߼��߳��źŵ�ʱ������˽ӿ�
	@return  ����true:��ʾ�����Ѵ����꣬����false:��ʾ����δ������
	*/
	virtual bool						OnDo(void) = NULL;
};

/// �߼��߳����������
struct ILogicThread
{
	/// ���߼��߳��Ϲ�һ������
	virtual bool						AttachTask(ILogicThreadTask * pTask, LPCSTR pszDebug) = NULL;

	/// ���߼��߳���ȡ��һ������
	virtual void						DetachTask(ILogicThreadTask * pTask) = NULL;
};



#endif //__I_LOGICTHREAD_H__
